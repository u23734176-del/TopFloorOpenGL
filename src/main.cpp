#include <iostream>
#include <vector>

// opengl imports
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Core
#include "core/Scene.h"
#include "core/Camera.h"
#include "core/ShaderManager.h"
#include "lighting/LightSet.h"
#include "lighting/shadowMap.h"

// Course (Slice D)
#include "layout/Course.h"
#include "objects/SphereBall.h"
#include "physics/BallPhysics.h"
#include "physics/PhysicsWorld.h"

// External Objects (Slice E - StaticMesh wrappers)
#include "objects/PalmTree.h"
#include "objects/Bush.h"
#include "objects/BenchChair.h"
#include "objects/BenchTable.h"
#include "objects/LightPost.h"
#include "objects/BillBoard.h"
#include "objects/AssimpModel.h"

// Post Processor
#include "ui/PostProcessor.h"

// HUD
#include "ui/HUD.h"

//SkyBox
#include "../skybox/skybox.h"

// Slices A, B, C
#include "objects/Drone.h"
#include "objects/Water.h"

const int WIDTH = 1000;
const int HEIGHT = 800;
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

// ---------------------------------------------------------------------------
// Course addToScene callback — must be at file scope (not inside main)
// ---------------------------------------------------------------------------
static Scene* g_scene = nullptr;
static PhysicsWorld *g_physics = nullptr;
static void addObjectToScene(SceneObject *obj)
{
    g_scene->addObject(obj);
    if (dynamic_cast<GolfHole *>(obj))
    {
        g_physics->addCollider(obj, Surface::TURF);
    }
    else
    {
        g_physics->addCollider(obj, Surface::SOLID);
    }
}

// Write skybox faces in the easy order:
// right, left, front, back, top, bottom.
// OpenGL cubemaps must be uploaded in this order:
// +X right, -X left, +Y top, -Y bottom, +Z front, -Z back.
static std::vector<std::string> skyboxFaces(const std::string& right,
                                            const std::string& left,
                                            const std::string& front,
                                            const std::string& back,
                                            const std::string& top,
                                            const std::string& bottom)
{
    return {
        right,
        left,
        top,
        bottom,
        front,
        back
    };
}

// ---------------------------------------------------------------------------
// GLFW / GLEW setup
// ---------------------------------------------------------------------------
const char* getError()
{
    const char* errorDescription;
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

// ---------------------------------------------------------------------------
// Lighting
// ---------------------------------------------------------------------------
void updateLightingForDayNight(LightSet& lights, bool isNight)
{
    lights.isNight = isNight;

    if (isNight)
    {
        lights.directional.direction = glm::normalize(glm::vec3(0.5f, -0.6f, 0.3f));
        lights.directional.color     = glm::vec3(0.6f, 0.6f, 0.8f);
        lights.ambient               = glm::vec3(0.05f, 0.05f, 0.15f);
        lights.numPointLights        = 3;

        lights.pointLights[0].position  = glm::vec3(5.0f, 1.5f, -10.0f);
        lights.pointLights[0].color     = glm::vec3(1.0f, 0.8f, 0.4f);
        lights.pointLights[0].intensity = 0.8f;
        lights.pointLights[0].radius    = 15.0f;
        lights.pointLights[0].linear    = 0.09f;
        lights.pointLights[0].quadratic = 0.032f;

        lights.pointLights[1].position  = glm::vec3(-5.0f, 1.5f, -5.0f);
        lights.pointLights[1].color     = glm::vec3(1.0f, 0.8f, 0.5f);
        lights.pointLights[1].intensity = 0.7f;
        lights.pointLights[1].radius    = 15.0f;
        lights.pointLights[1].linear    = 0.09f;
        lights.pointLights[1].quadratic = 0.032f;

        lights.pointLights[2].position  = glm::vec3(0.0f, 2.0f, -15.0f);
        lights.pointLights[2].color     = glm::vec3(0.4f, 0.6f, 1.0f);
        lights.pointLights[2].intensity = 0.5f;
        lights.pointLights[2].radius    = 20.0f;
        lights.pointLights[2].linear    = 0.07f;
        lights.pointLights[2].quadratic = 0.017f;
    }
    else
    {
        lights.directional.direction = glm::normalize(glm::vec3(-0.3f, -0.9f, -0.2f));
        lights.directional.color     = glm::vec3(1.0f, 0.95f, 0.8f);
        lights.ambient               = glm::vec3(0.4f, 0.42f, 0.45f);
        lights.numPointLights        = 0;
    }
}

void passLightingUniforms(GLuint shaderProgram, const LightSet& lights, const glm::vec3& cameraPos)
{
    glUniform3f(glGetUniformLocation(shaderProgram, "directional.direction"),
                lights.directional.direction.x, lights.directional.direction.y, lights.directional.direction.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "directional.color"),
                lights.directional.color.x, lights.directional.color.y, lights.directional.color.z);

    glUniform3f(glGetUniformLocation(shaderProgram, "ambient"),
                lights.ambient.x, lights.ambient.y, lights.ambient.z);

    glUniform1i(glGetUniformLocation(shaderProgram, "numPointLights"), lights.numPointLights);
    for (int i = 0; i < lights.numPointLights; ++i)
    {
        std::string base = "pointLights[" + std::to_string(i) + "]";
        glUniform3f(glGetUniformLocation(shaderProgram, (base + ".position").c_str()),
                    lights.pointLights[i].position.x, lights.pointLights[i].position.y, lights.pointLights[i].position.z);
        glUniform3f(glGetUniformLocation(shaderProgram, (base + ".color").c_str()),
                    lights.pointLights[i].color.x, lights.pointLights[i].color.y, lights.pointLights[i].color.z);
        glUniform1f(glGetUniformLocation(shaderProgram, (base + ".intensity").c_str()),  lights.pointLights[i].intensity);
        glUniform1f(glGetUniformLocation(shaderProgram, (base + ".radius").c_str()),     lights.pointLights[i].radius);
        glUniform1f(glGetUniformLocation(shaderProgram, (base + ".linear").c_str()),     lights.pointLights[i].linear);
        glUniform1f(glGetUniformLocation(shaderProgram, (base + ".quadratic").c_str()),  lights.pointLights[i].quadratic);
    }

    glUniform3f(glGetUniformLocation(shaderProgram, "spotlight.position"),
                lights.spotlight.position.x, lights.spotlight.position.y, lights.spotlight.position.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "spotlight.direction"),
                lights.spotlight.direction.x, lights.spotlight.direction.y, lights.spotlight.direction.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "spotlight.color"),
                lights.spotlight.color.x, lights.spotlight.color.y, lights.spotlight.color.z);
    glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.intensity"),    lights.spotlight.intensity);
    glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.innerCutoff"),  lights.spotlight.innerCutoff);
    glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.outerCutoff"),  lights.spotlight.outerCutoff);
    glUniform1i(glGetUniformLocation(shaderProgram, "spotlightEnabled"),       lights.spotlightEnabled ? 1 : 0);

    glUniform1i(glGetUniformLocation(shaderProgram, "isNight"),  lights.isNight ? 1 : 0);
    glUniform3f(glGetUniformLocation(shaderProgram, "cameraPos"),
                cameraPos.x, cameraPos.y, cameraPos.z);
}

glm::mat4 calculateLightSpaceMatrix(const LightSet& lights)
{
    glm::vec3 sceneCenter(0.0f, 0.0f, -20.0f);
    glm::vec3 lightPosition = sceneCenter - lights.directional.direction * 80.0f;
    glm::mat4 lightView = glm::lookAt(lightPosition, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = glm::ortho(-80.0f, 80.0f, -80.0f, 80.0f, 1.0f, 200.0f);
    return lightProjection * lightView;
}

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------
inline void handleInput(GLFWwindow *&window, float deltaTime,
                        PostProcessor &postProcessor,
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
                impulse = glm::normalize(impulse) * 15.0f; // Adjust speed here
                ballState.putt(impulse);
                lastPutt = currentFrame;
            }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) postProcessor.setMode(0);
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) postProcessor.setMode(1);
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) postProcessor.setMode(2);

    static bool nKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !nKeyPressed)
    {
        nKeyPressed = true;
        isNight = !isNight;
        updateLightingForDayNight(lights, isNight);
        std::cout << (isNight ? "Night Mode ON" : "Day Mode ON") << std::endl;
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

    GLFWwindow* window;
    try
    {
        window = setUp(WIDTH, HEIGHT);
    }
    catch (const char* e)
    {
        std::cout << e << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ShaderManager::load("basic", "shaders/basic.vert", "shaders/basic.frag");
    ShaderManager::load("shadow_depth", "shaders/shadow_depth.vert", "shaders/shadow_depth.frag");

    // ADD THIS LINE:
    ShaderManager::load("water", "shaders/water.vert", "shaders/water.frag");

    // ---- Skybox ----
    Skybox skybox;
    std::vector<std::string> dayFaces = skyboxFaces(
        "skybox/day/right.png",
        "skybox/day/left.png",
        "skybox/day/front.png",
        "skybox/day/back.png",
        "skybox/day/top.png",
        "skybox/day/bottom.png"
    );

    std::vector<std::string> nightFaces = skyboxFaces(
        "skybox/night/right.png",
        "skybox/night/left.png",
        "skybox/night/front.png",
        "skybox/night/back.png",
        "skybox/night/top.png",
        "skybox/night/bottom.png"
    );

    if (!skybox.build(dayFaces, nightFaces))
    {
        std::cerr << "Failed to build skybox. Check shader and texture paths.\n";
        return -1;
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

    // ---- Scene + Course ----
    Scene scene;
    g_scene = &scene;

    PhysicsWorld physicsWorld;
    g_physics = &physicsWorld;
    physicsWorld.setGroundLevel(0.0f); // Prevents ball falling through the world

    SphereBall playableBall;
    playableBall.setPosition(glm::vec3(8.0f, 0.5f, 32.0f)); // Start at Hole 1
    scene.addObject(&playableBall);

    BallPhysics ballState;
    physicsWorld.setBall(&playableBall, &ballState);

    Course course;
    course.build();
    course.addToScene(addObjectToScene);

    // ==========================================================
    // DECORATIONS & EXTERNAL OBJECTS (ASSIMP)
    // ==========================================================

    // 1. Palm Trees (Multi-colored)
    AssimpModel *palm1 = new AssimpModel("external/Palm_Tree.obj");
    palm1->setPosition(glm::vec3(5.0f, 0.0f, 28.0f));
    palm1->setScale(glm::vec3(1.5f));
    palm1->build();                                                     // Build VRAM before coloring
    palm1->setSubMeshColor("Cylinder", glm::vec3(0.45f, 0.28f, 0.15f)); // Trunk
    palm1->setSubMeshColor("Plane", glm::vec3(0.15f, 0.5f, 0.15f));     // Leaves
    scene.addObject(palm1);

    AssimpModel *palm2 = new AssimpModel("external/Palm_Tree.obj");
    palm2->setPosition(glm::vec3(-10.0f, 0.0f, -15.0f));
    palm2->setScale(glm::vec3(1.2f));
    palm2->setRotation(glm::vec3(0.0f, 45.0f, 0.0f));
    palm2->build();
    palm2->setSubMeshColor("Cylinder", glm::vec3(0.45f, 0.28f, 0.15f));
    palm2->setSubMeshColor("Plane", glm::vec3(0.15f, 0.5f, 0.15f));
    scene.addObject(palm2);

    // 2. Bushes (Solid Green)
    AssimpModel *bush1 = new AssimpModel("external/Bush.obj");
    bush1->setPosition(glm::vec3(2.0f, 0.0f, 25.0f));
    bush1->setScale(glm::vec3(0.8f));
    bush1->build();
    bush1->setSubMeshColor("", glm::vec3(0.1f, 0.4f, 0.1f)); // "" colors the whole object
    scene.addObject(bush1);

    AssimpModel *bush2 = new AssimpModel("external/Bush.obj");
    bush2->setPosition(glm::vec3(22.0f, 0.0f, -28.0f));
    bush2->build();
    bush2->setSubMeshColor("", glm::vec3(0.15f, 0.45f, 0.15f)); // Slightly lighter green
    scene.addObject(bush2);

    // 3. Bench Table Area (Solid Wood)
    AssimpModel *table = new AssimpModel("external/Bench_Table.obj");
    table->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    table->build();
    table->setSubMeshColor("", glm::vec3(0.45f, 0.28f, 0.15f));
    scene.addObject(table);

    AssimpModel *chair1 = new AssimpModel("external/bench_Chair.obj");
    chair1->setPosition(glm::vec3(0.0f, 0.0f, 3.5f));
    chair1->build();
    chair1->setSubMeshColor("", glm::vec3(0.45f, 0.28f, 0.15f));
    scene.addObject(chair1);

    AssimpModel *chair2 = new AssimpModel("external/bench_Chair.obj");
    chair2->setPosition(glm::vec3(0.0f, 0.0f, 6.5f));
    chair2->setRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    chair2->build();
    chair2->setSubMeshColor("", glm::vec3(0.45f, 0.28f, 0.15f));
    scene.addObject(chair2);

    // 4. Light Post (Solid Grey/Metal)
    AssimpModel *lamp = new AssimpModel("external/Light_post_2.obj");
    lamp->setPosition(glm::vec3(3.0f, 0.0f, 5.0f));
    lamp->build();
    lamp->setSubMeshColor("", glm::vec3(0.3f, 0.3f, 0.35f));
    lamp->setSubMeshColor("Cube.004", glm::vec3(1.0f, 1.0f, 0.0f));
        scene.addObject(lamp);
    lamp->printSubMeshes();

    // 5. Billboard (Solid White/Grey canvas)
    AssimpModel *board = new AssimpModel("external/BillBoard.obj");
    board->setPosition(glm::vec3(-5.0f, 0.0f, 35.0f));
    board->setRotation(glm::vec3(0.0f, -20.0f, 0.0f));
    board->setScale(glm::vec3(1.5f));
    board->build();
    board->setSubMeshColor("", glm::vec3(0.8f, 0.8f, 0.8f));
    scene.addObject(board);
    
    // ==========================================================
    // 1. DRONE (Slice A)
    // ==========================================================
    Drone drone;
    drone.setPosition(glm::vec3(0.0f, 5.0f, 0.0f)); // Start hovering above the gazebo
    scene.addObject(&drone);                        // Adds to opaque render bucket automatically
    
    // ==========================================================
    // 2. WATER (Slice C)
    // ==========================================================
    Water lake;
    lake.setPosition(glm::vec3(-15.0f, 0.0f, -15.0f)); // Adjust to match course map
    lake.setSize(12.0f, 12.0f);
    scene.addObject(&lake); // Scene will route this to drawTransparentObjects
    
    scene.build(); // This triggers Assimp to load the files into VRAM
    // ==========================================================

    LightSet lights;
    updateLightingForDayNight(lights, isNight);

    std::cout << "=== TopFloor Golf ===" << std::endl;
    std::cout << "WASD = fly  |  Mouse = look  |  N = day/night  |  8/9/0 = colour filter" << std::endl;

    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 1. Process Drone Flight
        drone.processInput(window, deltaTime);

        // 2. Process Game/UI Input (Putting, Day/Night toggle, Esc to quit)
        handleInput(window, deltaTime, postProcessor, isNight, lights, camera, ballState);

        // 3. Slave Camera to Drone (3rd Person Follow)
        camera.followTarget(drone.getPosition(), drone.getFront(), drone.getRoll());

        // 4. Attach Drone Spotlight
        lights.spotlight.position = drone.getPosition();
        lights.spotlight.direction = drone.getFront();
        lights.spotlightEnabled = isNight;

        // 5. Physics Step
        physicsWorld.update(deltaTime);
        
        course.update(deltaTime);

        // Calculate Matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix((float)WIDTH, (float)HEIGHT);
        glm::mat4 lightSpaceMatrix = calculateLightSpaceMatrix(lights);

        // ---- Shadow pass ----
        GLuint depthShader = ShaderManager::get("shadow_depth");
        glUseProgram(depthShader);
        glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightSpaceMatrix"),
                           1, GL_FALSE, &lightSpaceMatrix[0][0]);
        shadowMap.beginDepthPass();
        scene.drawDepthAllObjects(depthShader);
        shadowMap.endDepthPass(WIDTH, HEIGHT);

        // ---- Main pass ----
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black; skybox covers it
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

        // --- 1. OPAQUE PASS ---
        scene.drawOpaqueObjects(view, projection, lights);

        // --- 2. SKYBOX PASS ---
        glDepthFunc(GL_LEQUAL);
        skybox.draw(view, projection, isNight);
        glDepthFunc(GL_LESS);

        // --- 3. TRANSPARENT PASS ---
        glEnable(GL_BLEND);
        // Note: Your Water.cpp already manages glDepthMask(GL_FALSE) natively!
        scene.drawTransparentObjects(view, projection, lights);
        glDisable(GL_BLEND);

        // ---- Post Processing & UI ----
        postProcessor.endRender();
        postProcessor.draw();

        hud.draw(camera, WIDTH, HEIGHT, postProcessor.getMode());

        // ---- Window title ----
        glm::vec3 camPos = camera.getPosition();
        std::string modeText = (postProcessor.getMode() == 1)   ? "Grayscale"
                               : (postProcessor.getMode() == 2) ? "Inverted"
                                                                : "Normal";
        std::string title =
            "Mini Golf | X:" + std::to_string((int)camPos.x) +
            " Y:" + std::to_string((int)camPos.y) +
            " Z:" + std::to_string((int)camPos.z) +
            " | " + modeText;
        glfwSetWindowTitle(window, title.c_str());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
