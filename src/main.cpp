#include <iostream>
#include <vector>
#include <cstdio> // for snprintf

// OpenGL imports
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Core Systems
#include "core/Scene.h"
#include "core/Camera.h"
#include "core/ShaderManager.h"
#include "lighting/LightSet.h"
#include "lighting/shadowMap.h"

// Physics & Gameplay
#include "physics/BallPhysics.h"
#include "physics/PhysicsWorld.h"
#include "objects/SphereBall.h"

// Level & Environment
#include "layout/Course.h"
#include "../skybox/skybox.h"
#include "objects/Drone.h"
#include "objects/Water.h"

// UI & Post Processing
#include "ui/PostProcessor.h"
#include "ui/HUD.h"

const int WIDTH = 1000;
const int HEIGHT = 800;
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

// ---------------------------------------------------------------------------
// Global Callbacks for Scene & Physics Routing
// ---------------------------------------------------------------------------
static Scene *g_scene = nullptr;
static PhysicsWorld *g_physics = nullptr;

static void addObjectToScene(SceneObject *obj)
{
    g_scene->addObject(obj);

    // Route collisions: Turf for playable surfaces, Solid for walls/obstacles
    if (dynamic_cast<GolfHole *>(obj))
    {
        g_physics->addCollider(obj, Surface::TURF);
    }
    else
    {
        g_physics->addCollider(obj, Surface::SOLID);
    }
}

// Helper to easily format the 6 skybox texture paths
static std::vector<std::string> skyboxFaces(const std::string &right, const std::string &left,
                                            const std::string &front, const std::string &back,
                                            const std::string &top, const std::string &bottom)
{
    return {right, left, top, bottom, front, back};
}

// ---------------------------------------------------------------------------
// GLFW / GLEW setup
// ---------------------------------------------------------------------------
const char *getError()
{
    const char *errorDescription;
    glfwGetError(&errorDescription);
    return errorDescription;
}

inline void startUpGLFW()
{
    glewExperimental = true;
    if (!glfwInit())
        throw getError();
}

inline void startUpGLEW()
{
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        throw getError();
    }
}

inline GLFWwindow *setUp(int width, int height)
{
    startUpGLFW();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(width, height, "TopFloor - OpenGL Golf", NULL, NULL);
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

// ---------------------------------------------------------------------------
// Lighting
// ---------------------------------------------------------------------------
void updateLightingForDayNight(LightSet &lights, bool isNight)
{
    lights.isNight = isNight;

    if (isNight)
    {
        lights.directional.direction = glm::normalize(glm::vec3(0.5f, -0.6f, 0.3f));
        lights.directional.color = glm::vec3(0.6f, 0.6f, 0.8f);
        lights.ambient = glm::vec3(0.05f, 0.05f, 0.15f);
        lights.numPointLights = 3;

        // Populate your 3 point lights here (Gazebo, Lamp posts, etc)
        lights.pointLights[0].position = glm::vec3(5.0f, 1.5f, -10.0f);
        lights.pointLights[0].color = glm::vec3(1.0f, 0.8f, 0.4f);
        lights.pointLights[0].intensity = 0.8f;
        lights.pointLights[0].radius = 15.0f;
        lights.pointLights[0].linear = 0.09f;
        lights.pointLights[0].quadratic = 0.032f;

        lights.pointLights[1].position = glm::vec3(-5.0f, 1.5f, -5.0f);
        lights.pointLights[1].color = glm::vec3(1.0f, 0.8f, 0.5f);
        lights.pointLights[1].intensity = 0.7f;
        lights.pointLights[1].radius = 15.0f;
        lights.pointLights[1].linear = 0.09f;
        lights.pointLights[1].quadratic = 0.032f;

        lights.pointLights[2].position = glm::vec3(0.0f, 2.0f, -15.0f);
        lights.pointLights[2].color = glm::vec3(0.4f, 0.6f, 1.0f);
        lights.pointLights[2].intensity = 0.5f;
        lights.pointLights[2].radius = 20.0f;
        lights.pointLights[2].linear = 0.07f;
        lights.pointLights[2].quadratic = 0.017f;
    }
    else
    {
        lights.directional.direction = glm::normalize(glm::vec3(-0.3f, -0.9f, -0.2f));
        lights.directional.color = glm::vec3(1.0f, 0.95f, 0.8f);
        lights.ambient = glm::vec3(0.4f, 0.42f, 0.45f);
        lights.numPointLights = 0;
    }
}

// ---------------------------------------------------------------------------
// Uniform passing
// ---------------------------------------------------------------------------
void passLightingUniforms(GLuint shaderProgram, const LightSet &lights, const glm::vec3 &cameraPos)
{
    ShaderManager::setVec3(shaderProgram, "directional.direction",
                           lights.directional.direction.x,
                           lights.directional.direction.y,
                           lights.directional.direction.z);
    ShaderManager::setVec3(shaderProgram, "directional.color",
                           lights.directional.color.x,
                           lights.directional.color.y,
                           lights.directional.color.z);
    ShaderManager::setVec3(shaderProgram, "ambient",
                           lights.ambient.x, lights.ambient.y, lights.ambient.z);

    ShaderManager::setInt(shaderProgram, "numPointLights", lights.numPointLights);

    char buf[64];
    for (int i = 0; i < lights.numPointLights; ++i)
    {
        snprintf(buf, sizeof(buf), "pointLights[%d].position", i);
        ShaderManager::setVec3(shaderProgram, buf,
                               lights.pointLights[i].position.x,
                               lights.pointLights[i].position.y,
                               lights.pointLights[i].position.z);

        snprintf(buf, sizeof(buf), "pointLights[%d].color", i);
        ShaderManager::setVec3(shaderProgram, buf,
                               lights.pointLights[i].color.x,
                               lights.pointLights[i].color.y,
                               lights.pointLights[i].color.z);

        snprintf(buf, sizeof(buf), "pointLights[%d].intensity", i);
        ShaderManager::setFloat(shaderProgram, buf, lights.pointLights[i].intensity);

        snprintf(buf, sizeof(buf), "pointLights[%d].radius", i);
        ShaderManager::setFloat(shaderProgram, buf, lights.pointLights[i].radius);

        snprintf(buf, sizeof(buf), "pointLights[%d].linear", i);
        ShaderManager::setFloat(shaderProgram, buf, lights.pointLights[i].linear);

        snprintf(buf, sizeof(buf), "pointLights[%d].quadratic", i);
        ShaderManager::setFloat(shaderProgram, buf, lights.pointLights[i].quadratic);
    }

    ShaderManager::setVec3(shaderProgram, "spotlight.position",
                           lights.spotlight.position.x,
                           lights.spotlight.position.y,
                           lights.spotlight.position.z);
    ShaderManager::setVec3(shaderProgram, "spotlight.direction",
                           lights.spotlight.direction.x,
                           lights.spotlight.direction.y,
                           lights.spotlight.direction.z);
    ShaderManager::setVec3(shaderProgram, "spotlight.color",
                           lights.spotlight.color.x,
                           lights.spotlight.color.y,
                           lights.spotlight.color.z);
    ShaderManager::setFloat(shaderProgram, "spotlight.intensity", lights.spotlight.intensity);
    ShaderManager::setFloat(shaderProgram, "spotlight.innerCutoff", lights.spotlight.innerCutoff);
    ShaderManager::setFloat(shaderProgram, "spotlight.outerCutoff", lights.spotlight.outerCutoff);
    ShaderManager::setInt(shaderProgram, "spotlightEnabled", lights.spotlightEnabled ? 1 : 0);
    ShaderManager::setInt(shaderProgram, "isNight", lights.isNight ? 1 : 0);
    ShaderManager::setVec3(shaderProgram, "cameraPos",
                           cameraPos.x, cameraPos.y, cameraPos.z);
}

glm::mat4 calculateLightSpaceMatrix(const LightSet &lights)
{
    glm::vec3 sceneCenter(0.0f, 0.0f, 0.0f);
    glm::vec3 lightPosition = sceneCenter - lights.directional.direction * 80.0f;
    glm::mat4 lightView = glm::lookAt(lightPosition, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = glm::ortho(-80.0f, 80.0f, -80.0f, 80.0f, 1.0f, 200.0f);
    return lightProjection * lightView;
}

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------
inline void handleInput(GLFWwindow *&window, float deltaTime, PostProcessor &postProcessor,
                        bool &isNight, LightSet &lights, Camera &camera, BallPhysics &ballState)
{
    static float lastPutt = 0.0f;
    float currentFrame = glfwGetTime();

    // Press Spacebar to putt the ball
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (currentFrame - lastPutt > 0.5f)
        {
            float yaw = glm::radians(camera.getYaw());
            float pitch = glm::radians(camera.getPitch());
            glm::vec3 front(cos(yaw) * cos(pitch), sin(pitch), sin(yaw) * cos(pitch));

            glm::vec3 impulse = front;
            impulse.y = 0.0f; // horizontal putt only
            if (glm::length(impulse) > 0.001f)
            {
                impulse = glm::normalize(impulse) * 15.0f; // Adjust putt speed here
                ballState.putt(impulse);
                lastPutt = currentFrame;
            }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
        postProcessor.setMode(0);
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
        postProcessor.setMode(1);
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
        postProcessor.setMode(2);

    static bool nKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !nKeyPressed)
    {
        nKeyPressed = true;
        isNight = !isNight;
        updateLightingForDayNight(lights, isNight);
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE)
        nKeyPressed = false;
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main()
{
    bool isNight = false;
    GLFWwindow *window;
    try
    {
        window = setUp(WIDTH, HEIGHT);
    }
    catch (const char *e)
    {
        std::cout << e << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // ---- Load Shaders ----
    ShaderManager::load("basic", "shaders/basic.vert", "shaders/basic.frag");
    ShaderManager::load("shadow_depth", "shaders/shadow_depth.vert", "shaders/shadow_depth.frag");
    ShaderManager::load("water", "shaders/water.vert", "shaders/water.frag"); // Slice C

    // ---- Initialize Subsystems ----
    Skybox skybox;
    std::vector<std::string> dayFaces = skyboxFaces(
        "skybox/day/right.png", "skybox/day/left.png", "skybox/day/front.png",
        "skybox/day/back.png", "skybox/day/top.png", "skybox/day/bottom.png");
    std::vector<std::string> nightFaces = skyboxFaces(
        "skybox/night/right.png", "skybox/night/left.png", "skybox/night/front.png",
        "skybox/night/back.png", "skybox/night/top.png", "skybox/night/bottom.png");
    if (!skybox.build(dayFaces, nightFaces))
    {
        std::cerr << "Failed to build skybox. Check shader and texture paths.\n";
    }

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

    // ---- Setup Scene & Physics ----
    Scene scene;
    g_scene = &scene;

    PhysicsWorld physicsWorld;
    g_physics = &physicsWorld;
    physicsWorld.setGroundLevel(-0.1f); // Set to base floor height

    // Playable Ball (Slice E)
    SphereBall playableBall;
    playableBall.setPosition(glm::vec3(30.0f, 0.5f, 0.0f));
    playableBall.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
    scene.addObject(&playableBall);

    BallPhysics ballState;
    physicsWorld.setBall(&playableBall, &ballState);

    // Level Building (Slice D)
    Course course;
    course.build();
    course.addToScene(addObjectToScene);

    // Fly-Cam Drone (Slice A)
    Drone drone;
    drone.setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    scene.addObject(&drone);

    // Upload to VRAM
    scene.build();

    // ---- Final Preparations ----
    LightSet lights;
    updateLightingForDayNight(lights, isNight);

    std::cout << "=== Crescent Head Country Club ===" << std::endl;
    std::cout << "WASD = fly  |  Mouse = look  |  SPACE = putt  |  N = day/night  |  8/9/0 = colour filter" << std::endl;

    float lastFrame = 0.0f;
    static float titleTimer = 0.0f;

    // ---------------------------------------------------------------------------
    // THE GAME LOOP
    // ---------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // ---------------------------------------------------------------------------
        // 1. Logic & Input Routing
        // ---------------------------------------------------------------------------
        handleInput(window, deltaTime, postProcessor, isNight, lights, camera, ballState);

        // DRONE VIEW: Fly the drone, camera follows drone
        drone.processInput(window, deltaTime);
        camera.followTarget(drone.getPosition(), drone.getFront(), drone.getRoll());

        // Spotlight stays attached to the drone
        lights.spotlight.position = drone.getPosition();
        lights.spotlight.direction = drone.getFront();
        lights.spotlightEnabled = isNight;

        // 2. Physics & Animation Updates
        physicsWorld.update(deltaTime);
        course.update(deltaTime); // Syncs the windmill shadow!

        // 3. Matrix Setup
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix((float)WIDTH, (float)HEIGHT);
        glm::mat4 lightSpaceMatrix = calculateLightSpaceMatrix(lights);

        // ==========================================
        // RENDER PIPELINE
        // ==========================================

        // PASS 1: Depth/Shadow Map
        GLuint depthShader = ShaderManager::get("shadow_depth");
        glUseProgram(depthShader);
        glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
        shadowMap.beginDepthPass();
        scene.drawDepthAllObjects(depthShader);
        shadowMap.endDepthPass(WIDTH, HEIGHT);

        // PASS 2: Main Geometry
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        postProcessor.beginRender();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLuint shaderProgram = ShaderManager::get("basic");
        glUseProgram(shaderProgram);
        passLightingUniforms(shaderProgram, lights, camera.getPosition());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMap.getDepthTexture());
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 1);

        // 2A. Opaque Geometry
        scene.drawOpaqueObjects(view, projection, lights);

        // 2B. Skybox Background
        glDepthFunc(GL_LEQUAL);
        skybox.draw(view, projection, isNight);
        glDepthFunc(GL_LESS);

        // 2C. Transparent Geometry (Water)
        glEnable(GL_BLEND);

        // Pass lighting to water shader so it isn't black
        GLuint waterShader = ShaderManager::get("water");
        glUseProgram(waterShader);
        passLightingUniforms(waterShader, lights, camera.getPosition());
        glUniformMatrix4fv(glGetUniformLocation(waterShader, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
        glUniform1i(glGetUniformLocation(waterShader, "shadowMap"), 1);

        scene.drawTransparentObjects(view, projection, lights);
        glDisable(GL_BLEND);

        // PASS 3: Post-Processing & HUD
        postProcessor.endRender();
        postProcessor.draw();
        hud.draw(camera, WIDTH, HEIGHT, postProcessor.getMode());

        // Update Window Title (Throttled)
        titleTimer += deltaTime;
        if (titleTimer >= 1.0f)
        {
            titleTimer = 0.0f;
            glm::vec3 camPos = camera.getPosition();
            const char *modeText = (postProcessor.getMode() == 1)   ? "Grayscale"
                                   : (postProcessor.getMode() == 2) ? "Inverted"
                                                                    : "Normal";
            char title[128];
            snprintf(title, sizeof(title), "Mini Golf | X:%d Y:%d Z:%d | %s",
                     (int)camPos.x, (int)camPos.y, (int)camPos.z, modeText);
            glfwSetWindowTitle(window, title);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}