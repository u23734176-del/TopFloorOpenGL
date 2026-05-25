#include <iostream>
#include <vector>
#include <cstdio>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scene.h"
#include "Camera.h"
#include "ShaderManager.h"
#include "LightSet.h"
#include "shadowMap.h"
#include "BallPhysics.h"
#include "PhysicsWorld.h"
#include "SphereBall.h"
#include "Course.h"
#include "skybox.h"
#include "Drone.h"
#include "Water.h"
#include "PostProcessor.h"
#include "HUD.h"

const int WIDTH  = 1000;
const int HEIGHT = 800;
const unsigned int SHADOW_WIDTH  = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

static Scene*        g_scene   = nullptr;
static PhysicsWorld* g_physics = nullptr;

static void addObjectToScene(SceneObject* obj)
{
    g_scene->addObject(obj);
    if (dynamic_cast<GolfHole*>(obj))
        g_physics->addCollider(obj, Surface::TURF);
    else
        g_physics->addCollider(obj, Surface::SOLID);
}

static std::vector<std::string> skyboxFaces(const std::string& right, const std::string& left,
                                            const std::string& front, const std::string& back,
                                            const std::string& top,   const std::string& bottom)
{
    return {right, left, top, bottom, front, back};
}

const char* getError()
{
    const char* errorDescription;
    glfwGetError(&errorDescription);
    return errorDescription;
}

inline void startUpGLFW()
{
    glewExperimental = true;
    if (!glfwInit()) throw getError();
}

inline void startUpGLEW()
{
    glewExperimental = true;
    if (glewInit() != GLEW_OK) { glfwTerminate(); throw getError(); }
}

inline GLFWwindow* setUp(int width, int height)
{
    startUpGLFW();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "TopFloor - OpenGL Golf", NULL, NULL);
    if (window == NULL)
    {
        std::cout << getError() << std::endl;
        glfwTerminate();
        throw "Failed to open GLFW window.\n";
    }
    glfwMakeContextCurrent(window);
    startUpGLEW();
    glViewport(0, 0, width, height);
    return window;
}

void updateLightingForDayNight(LightSet &lights, bool isNight)
{
    lights.isNight = isNight;
    if (isNight)
    {
        lights.directional.direction = glm::normalize(glm::vec3(0.5f, -0.6f, 0.3f));
        lights.directional.color = glm::vec3(0.6f, 0.6f, 0.8f);
        lights.ambient = glm::vec3(0.05f, 0.05f, 0.15f);
        lights.numPointLights = 3;

        lights.pointLights[0].position = glm::vec3(25.0f, 2.0f, 0.0f);
        lights.pointLights[0].color = glm::vec3(1.0f, 0.9f, 0.7f);
        lights.pointLights[0].intensity = 1.0f;
        lights.pointLights[0].radius = 12.0f;

        lights.pointLights[1].position = glm::vec3(-25.0f, 2.0f, 0.0f);
        lights.pointLights[1].color = glm::vec3(1.0f, 0.9f, 0.7f);
        lights.pointLights[1].intensity = 1.0f;
        lights.pointLights[1].radius = 12.0f;

        lights.pointLights[2].position = glm::vec3(0.0f, 2.0f, 25.0f);
        lights.pointLights[2].color = glm::vec3(1.0f, 0.9f, 0.7f);
        lights.pointLights[2].intensity = 1.0f;
        lights.pointLights[2].radius = 12.0f;
    }
    else
    {
        lights.directional.direction = glm::normalize(glm::vec3(-0.3f, -0.9f, -0.2f));
        lights.directional.color = glm::vec3(1.0f, 0.95f, 0.8f);
        lights.ambient = glm::vec3(0.4f, 0.42f, 0.45f);
        lights.numPointLights = 0;
    }
}

void passLightingUniforms(GLuint shaderProgram, const LightSet& lights, const glm::vec3& cameraPos)
{
    ShaderManager::setVec3(shaderProgram, "directional.direction",
                           lights.directional.direction.x, lights.directional.direction.y, lights.directional.direction.z);
    ShaderManager::setVec3(shaderProgram, "directional.color",
                           lights.directional.color.x, lights.directional.color.y, lights.directional.color.z);
    ShaderManager::setVec3(shaderProgram, "ambient",
                           lights.ambient.x, lights.ambient.y, lights.ambient.z);
    ShaderManager::setInt(shaderProgram, "numPointLights", lights.numPointLights);

    char buf[64];
    for (int i = 0; i < lights.numPointLights; ++i) {
        snprintf(buf, sizeof(buf), "pointLights[%d].position",  i);
        ShaderManager::setVec3(shaderProgram, buf, lights.pointLights[i].position.x, lights.pointLights[i].position.y, lights.pointLights[i].position.z);
        snprintf(buf, sizeof(buf), "pointLights[%d].color",     i);
        ShaderManager::setVec3(shaderProgram, buf, lights.pointLights[i].color.x, lights.pointLights[i].color.y, lights.pointLights[i].color.z);
        snprintf(buf, sizeof(buf), "pointLights[%d].intensity", i);
        ShaderManager::setFloat(shaderProgram, buf, lights.pointLights[i].intensity);
        snprintf(buf, sizeof(buf), "pointLights[%d].radius",    i);
        ShaderManager::setFloat(shaderProgram, buf, lights.pointLights[i].radius);
        snprintf(buf, sizeof(buf), "pointLights[%d].linear",    i);
        ShaderManager::setFloat(shaderProgram, buf, lights.pointLights[i].linear);
        snprintf(buf, sizeof(buf), "pointLights[%d].quadratic", i);
        ShaderManager::setFloat(shaderProgram, buf, lights.pointLights[i].quadratic);
    }

    ShaderManager::setVec3 (shaderProgram, "spotlight.position",    lights.spotlight.position.x,    lights.spotlight.position.y,    lights.spotlight.position.z);
    ShaderManager::setVec3 (shaderProgram, "spotlight.direction",   lights.spotlight.direction.x,   lights.spotlight.direction.y,   lights.spotlight.direction.z);
    ShaderManager::setVec3 (shaderProgram, "spotlight.color",       lights.spotlight.color.x,       lights.spotlight.color.y,       lights.spotlight.color.z);
    ShaderManager::setFloat(shaderProgram, "spotlight.intensity",   lights.spotlight.intensity);
    ShaderManager::setFloat(shaderProgram, "spotlight.innerCutoff", lights.spotlight.innerCutoff);
    ShaderManager::setFloat(shaderProgram, "spotlight.outerCutoff", lights.spotlight.outerCutoff);
    ShaderManager::setInt  (shaderProgram, "spotlightEnabled",      lights.spotlightEnabled ? 1 : 0);
    ShaderManager::setInt  (shaderProgram, "isNight",               lights.isNight ? 1 : 0);
    ShaderManager::setVec3 (shaderProgram, "cameraPos",             cameraPos.x, cameraPos.y, cameraPos.z);
}

glm::mat4 calculateLightSpaceMatrix(const LightSet& lights)
{
    glm::vec3 sceneCenter(0.0f, 0.0f, 0.0f);
    glm::vec3 lightPosition = sceneCenter - lights.directional.direction * 80.0f;
    glm::mat4 lightView = glm::lookAt(lightPosition, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProj = glm::ortho(-80.0f, 80.0f, -80.0f, 80.0f, 1.0f, 200.0f);
    return lightProj * lightView;
}

inline void handleInput(GLFWwindow*& window, float deltaTime,
                        PostProcessor& postProcessor,
                        bool& isNight, LightSet& lights,
                        Camera& camera, BallPhysics& ballState,
                        bool& firstPerson)                        
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    // colour filters
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) postProcessor.setMode(0);
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) postProcessor.setMode(1);
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) postProcessor.setMode(2);

    // day/night
    static bool nWasDown = false;
    bool nDown = glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS;
    if (nDown && !nWasDown) {
        isNight = !isNight;
        updateLightingForDayNight(lights, isNight);
        std::cout << (isNight ? "Night" : "Day") << std::endl;
    }
    nWasDown = nDown;

    // first / third person toggle
    static bool vWasDown = false;
    bool vDown = glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS;
    if (vDown && !vWasDown) {
        firstPerson = !firstPerson;
        std::cout << (firstPerson ? "First Person (FPV)" : "Third Person") << std::endl;
    }
    vWasDown = vDown;

    // ball putt
    static float lastPutt = 0.0f;
    float currentFrame = (float)glfwGetTime();
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && currentFrame - lastPutt > 0.5f)
    {
        float yaw   = glm::radians(camera.getYaw());
        float pitch = glm::radians(camera.getPitch());
        glm::vec3 front(cos(yaw) * cos(pitch), sin(pitch), sin(yaw) * cos(pitch));
        glm::vec3 impulse = glm::normalize(glm::vec3(front.x, 0.0f, front.z)) * 15.0f;
        ballState.putt(impulse);
        lastPutt = currentFrame;
    }
}

int main()
{
    bool isNight     = false;
    bool firstPerson = false;       

    GLFWwindow* window;
    try { window = setUp(WIDTH, HEIGHT); }
    catch (const char* e) { std::cout << e << std::endl; return -1; }

    glEnable(GL_DEPTH_TEST);

    ShaderManager::load("basic",        "basic.vert",        "basic.frag");
    ShaderManager::load("shadow_depth", "shadow_depth.vert", "shadow_depth.frag");
    ShaderManager::load("water",        "water.vert",        "water.frag");

    Skybox skybox;
    std::vector<std::string> dayFaces = skyboxFaces(
        "skybox/day/right.png",  "skybox/day/left.png",  "skybox/day/front.png",
        "skybox/day/back.png",   "skybox/day/top.png",   "skybox/day/bottom.png");
    std::vector<std::string> nightFaces = skyboxFaces(
        "skybox/night/right.png", "skybox/night/left.png", "skybox/night/front.png",
        "skybox/night/back.png",  "skybox/night/top.png",  "skybox/night/bottom.png");
    if (!skybox.build(dayFaces, nightFaces))
        std::cerr << "Failed to build skybox.\n";

    ShadowMap shadowMap;
    if (!shadowMap.init(SHADOW_WIDTH, SHADOW_HEIGHT))
    {
        std::cerr << "Failed to initialize shadow map." << std::endl;
        return -1;
    }

    Camera camera;
    PostProcessor postProcessor;
    postProcessor.build(WIDTH, HEIGHT);
    HUD hud;
    hud.build();

    Scene scene;
    g_scene = &scene;

    PhysicsWorld physicsWorld;
    g_physics = &physicsWorld;
    physicsWorld.setGroundLevel(-0.1f);

    SphereBall playableBall;
    playableBall.setPosition(glm::vec3(30.0f, 0.5f, 0.0f));
    playableBall.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
    scene.addObject(&playableBall);

    BallPhysics ballState;
    physicsWorld.setBall(&playableBall, &ballState);

    Course course;
    course.build();
    course.addToScene(addObjectToScene);

    Drone drone;
    drone.setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    scene.addObject(&drone);

    scene.build();

    LightSet lights;
    updateLightingForDayNight(lights, isNight);

    std::cout << "=== Crescent Head Country Club ===" << std::endl;
    std::cout << "WASD          : fly drone"           << std::endl;
    std::cout << "Arrow keys    : rotate drone"        << std::endl;
    std::cout << "Q / E         : roll"                << std::endl;
    std::cout << "Space         : putt ball"           << std::endl;
    std::cout << "V             : first/third person"  << std::endl;
    std::cout << "N             : day / night"         << std::endl;
    std::cout << "8 / 9 / 0     : colour filters"      << std::endl;
    std::cout << "Escape        : quit"                << std::endl;

    float lastFrame  = 0.0f;
    float titleTimer = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        float deltaTime    = currentFrame - lastFrame;
        lastFrame          = currentFrame;

        // input
        handleInput(window, deltaTime, postProcessor, isNight, lights,
                    camera, ballState, firstPerson);

        drone.processInput(window, deltaTime);

        // camera mode
        if (firstPerson)
            camera.setFirstPerson(drone.getPosition(), drone.getFront());
        else
            camera.followTarget(drone.getPosition(), drone.getFront(), drone.getRoll());

        // spotlight follows drone
        lights.spotlight.position    = drone.getPosition();
        lights.spotlight.direction   = drone.getFront();
        lights.spotlight.color       = glm::vec3(1.0f, 1.0f, 0.9f);
        lights.spotlight.intensity   = isNight ? 1.0f : 0.0f;
        lights.spotlight.innerCutoff = glm::cos(glm::radians(12.0f));
        lights.spotlight.outerCutoff = glm::cos(glm::radians(17.0f));
        lights.spotlightEnabled      = isNight;

        // physics & animation
        physicsWorld.update(deltaTime);
        course.update(deltaTime);

        // matrices
        glm::mat4 view             = camera.getViewMatrix();
        glm::mat4 projection       = camera.getProjectionMatrix((float)WIDTH, (float)HEIGHT);
        glm::mat4 lightSpaceMatrix = calculateLightSpaceMatrix(lights);

        // pass 1: shadow map
        GLuint depthShader = ShaderManager::get("shadow_depth");
        glUseProgram(depthShader);
        glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightSpaceMatrix"),
                           1, GL_FALSE, &lightSpaceMatrix[0][0]);
        shadowMap.beginDepthPass();
        scene.drawDepthAllObjects(depthShader);
        shadowMap.endDepthPass(WIDTH, HEIGHT);

        // pass 2: main geometry
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        postProcessor.beginRender();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLuint shaderProgram = ShaderManager::get("basic");
        glUseProgram(shaderProgram);
        passLightingUniforms(shaderProgram, lights, camera.getPosition());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"),
                           1, GL_FALSE, &lightSpaceMatrix[0][0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMap.getDepthTexture());
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 1);

        scene.drawOpaqueObjects(view, projection, lights);

        // skybox
        glDepthFunc(GL_LEQUAL);
        skybox.draw(view, projection, isNight);
        glDepthFunc(GL_LESS);

        // transparent (water)
        glEnable(GL_BLEND);
        GLuint waterShader = ShaderManager::get("water");
        glUseProgram(waterShader);
        passLightingUniforms(waterShader, lights, camera.getPosition());
        glUniformMatrix4fv(glGetUniformLocation(waterShader, "lightSpaceMatrix"),
                           1, GL_FALSE, &lightSpaceMatrix[0][0]);
        glUniform1i(glGetUniformLocation(waterShader, "shadowMap"), 1);
        scene.drawTransparentObjects(view, projection, lights);
        glDisable(GL_BLEND);

        // pass 3: post process + HUD
        postProcessor.endRender();
        postProcessor.draw();
        hud.draw(camera, WIDTH, HEIGHT, postProcessor.getMode());

        // window title (throttled)
        titleTimer += deltaTime;
        if (titleTimer >= 1.0f)
        {
            titleTimer = 0.0f;
            glm::vec3 pos = drone.getPosition();
            const char* viewMode = firstPerson ? "FPV" : "3rd";
            const char* modeText = postProcessor.getMode() == 1 ? "Grayscale"
                                 : postProcessor.getMode() == 2 ? "Inverted"
                                 :                                "Normal";
            char title[128];
            snprintf(title, sizeof(title), "Mini Golf | %s | X:%d Y:%d Z:%d | %s | %s",
                     viewMode, (int)pos.x, (int)pos.y, (int)pos.z,
                     modeText, isNight ? "Night" : "Day");
            glfwSetWindowTitle(window, title);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}