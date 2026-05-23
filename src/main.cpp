#include <iostream>
#include <vector>
#include <random>

// opengl imports
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Our objects
#include "core/Scene.h"
#include "core/Camera.h"
#include "core/ShaderManager.h"
#include "lighting/LightSet.h"
#include "lighting/shadowMap.h"
#include "objects/Cube.h"
#include "objects/Floor.h"
#include "objects/Wall.h"
#include "objects/Ramp.h"
#include "objects/Bridge.h"
#include "objects/Water.h"
#include "objects/Spinner.h"
#include "objects/Tunnel.h"
#include "objects/Ball.h"
#include "objects/Flag.h"
#include "objects/Windmill.h"
#include "objects/SphereBall.h"
#include "physics/BallPhysics.h"
#include "physics/PhysicsWorld.h"

// Post Processor 
#include "ui/PostProcessor.h"

// HUD 
#include "ui/HUD.h"

const int WIDTH = 1000;
const int HEIGHT = 800;
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

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
    {
        throw getError();
    }
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
    
    GLFWwindow *window = glfwCreateWindow(width, height, "TopFloor â€” OpenGL Golf", NULL, NULL);
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

void updateLightingForDayNight(LightSet& lights, bool isNight)
{
    lights.isNight = isNight;
    
    if (isNight) {
        lights.directional.direction = glm::normalize(glm::vec3(0.5f, -0.6f, 0.3f));
        lights.directional.color = glm::vec3(0.6f, 0.6f, 0.8f);
        lights.ambient = glm::vec3(0.05f, 0.05f, 0.15f);
        lights.numPointLights = 3;
        
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
        
    } else {
        lights.directional.direction = glm::normalize(glm::vec3(-0.3f, -0.9f, -0.2f));
        lights.directional.color = glm::vec3(1.0f, 0.95f, 0.8f);
        lights.ambient = glm::vec3(0.4f, 0.42f, 0.45f);
        lights.numPointLights = 0;
    }
}

inline void handleInput(GLFWwindow*& window, float& rotationX, float& rotationY, float deltaTime, PostProcessor& postProcessor, bool& isNight, LightSet& lights)
{
    // Close window
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, 1);
    }

    float rotationSpeed = 2.0f * deltaTime;

    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        rotationY -= rotationSpeed;
    }

    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        rotationY += rotationSpeed;
    }

    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        rotationX -= rotationSpeed;
    }

    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        rotationX += rotationSpeed;
    }

    if(glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        postProcessor.setMode(0);
    }

    if(glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    {
        postProcessor.setMode(1);
    }

    if(glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    {
        postProcessor.setMode(2);
    }
    
    static bool nKeyPressed = false;
    if(glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !nKeyPressed) {
        nKeyPressed = true;
        isNight = !isNight;
        updateLightingForDayNight(lights, isNight);
        std::cout << (isNight ? "Night Mode ON" : "Day Mode ON") << std::endl;
    }
    if(glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE) {
        nKeyPressed = false;
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
    for (int i = 0; i < lights.numPointLights; ++i) {
        std::string base = "pointLights[" + std::to_string(i) + "]";
        
        glUniform3f(glGetUniformLocation(shaderProgram, (base + ".position").c_str()),
                    lights.pointLights[i].position.x, lights.pointLights[i].position.y, lights.pointLights[i].position.z);
        glUniform3f(glGetUniformLocation(shaderProgram, (base + ".color").c_str()),
                    lights.pointLights[i].color.x, lights.pointLights[i].color.y, lights.pointLights[i].color.z);
        glUniform1f(glGetUniformLocation(shaderProgram, (base + ".intensity").c_str()),
                    lights.pointLights[i].intensity);
        glUniform1f(glGetUniformLocation(shaderProgram, (base + ".radius").c_str()),
                    lights.pointLights[i].radius);
        glUniform1f(glGetUniformLocation(shaderProgram, (base + ".linear").c_str()),
                    lights.pointLights[i].linear);
        glUniform1f(glGetUniformLocation(shaderProgram, (base + ".quadratic").c_str()),
                    lights.pointLights[i].quadratic);
    }
    
    glUniform3f(glGetUniformLocation(shaderProgram, "spotlight.position"),
                lights.spotlight.position.x, lights.spotlight.position.y, lights.spotlight.position.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "spotlight.direction"),
                lights.spotlight.direction.x, lights.spotlight.direction.y, lights.spotlight.direction.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "spotlight.color"),
                lights.spotlight.color.x, lights.spotlight.color.y, lights.spotlight.color.z);
    glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.intensity"),
                lights.spotlight.intensity);
    glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.innerCutoff"),
                lights.spotlight.innerCutoff);
    glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.outerCutoff"),
                lights.spotlight.outerCutoff);
    glUniform1i(glGetUniformLocation(shaderProgram, "spotlightEnabled"),
                lights.spotlightEnabled ? 1 : 0);
    
    glUniform1i(glGetUniformLocation(shaderProgram, "isNight"), lights.isNight ? 1 : 0);
    glUniform3f(glGetUniformLocation(shaderProgram, "cameraPos"),
                cameraPos.x, cameraPos.y, cameraPos.z);
}

glm::mat4 calculateLightSpaceMatrix(const LightSet& lights)
{
    glm::vec3 sceneCenter(0.0f, 0.0f, -8.0f);
    glm::vec3 lightPosition = sceneCenter - lights.directional.direction * 35.0f;
    glm::mat4 lightView = glm::lookAt(
        lightPosition,
        sceneCenter,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 lightProjection = glm::ortho(
        -35.0f, 35.0f,
        -35.0f, 35.0f,
        1.0f, 90.0f
    );

    return lightProjection * lightView;
}

int main()
{
    float rotationX = 0.0f;
    float rotationY = 0.0f;
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

    ShadowMap shadowMap;
    if (!shadowMap.init(SHADOW_WIDTH, SHADOW_HEIGHT))
    {
        std::cerr << "Failed to initialize shadow map." << std::endl;
        return -1;
    }

    Camera camera;

    // PostProcessor
    PostProcessor postProcessor;
    postProcessor.build(WIDTH, HEIGHT);

    // HUD
    HUD hud;
    hud.build();

    // Scene
    Scene scene;

    Cube cube1;
    cube1.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    cube1.setColor(glm::vec3(1.0f, 0.0f, 0.0f));

    Cube cube2;
    cube2.setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
    cube2.setScale(glm::vec3(2.0f));
    cube2.setColor(glm::vec3(0.0f, 0.0f, 1.0f));

    Cube cube3;
    cube3.setPosition(glm::vec3(-5.0f, 0.0f, 0.0f));
    cube3.setRotation(glm::vec3(45.0f, 45.0f, 0.0f));
    cube3.setColor(glm::vec3(1.0f, 1.0f, 0.0f));

    scene.addObject(&cube1);
    scene.addObject(&cube2);
    scene.addObject(&cube3);

    Floor ground;
    ground.setPosition(glm::vec3(0.0f, -2.0f, 0.0f));
    ground.setColor(glm::vec3(0.1f, 0.6f, 0.1f));
    scene.addObject(&ground);

    Cube obstacle;
    obstacle.setPosition(glm::vec3(3.0f, 0.0f, -5.0f));
    obstacle.setScale(glm::vec3(1.0f, 3.0f, 1.0f));
    obstacle.setColor(glm::vec3(1.0f, 0.5f, 0.0f));
    scene.addObject(&obstacle);

    Wall leftWall;
    leftWall.setPosition(glm::vec3(-10.0f, 0.0f, 0.0f));
    leftWall.setScale(glm::vec3(1.0f, 2.0f, 20.0f));
    scene.addObject(&leftWall);

    Wall rightWall;
    rightWall.setPosition(glm::vec3(10.0f, 0.0f, 0.0f));
    rightWall.setScale(glm::vec3(1.0f, 2.0f, 20.0f));
    scene.addObject(&rightWall);

    Wall backWall;
    backWall.setPosition(glm::vec3(0.0f, 0.0f, -20.0f));
    backWall.setScale(glm::vec3(20.0f, 2.0f, 1.0f));
    scene.addObject(&backWall);

    Ramp ramp;
    ramp.setPosition(glm::vec3(0.0f, 0.0f, -10.0f));
    scene.addObject(&ramp);

    Bridge bridge;
    bridge.setPosition(glm::vec3(10.0f, 0.0f, -15.0f));
    scene.addObject(&bridge);

    Water water;
    water.setPosition(glm::vec3(-10.0f, -1.5f, -10.0f));
    scene.addObject(&water);

    Ball ball;
    ball.setPosition(glm::vec3(0.0f, -1.0f, 5.0f));
    scene.addObject(&ball);

    SphereBall golfBall;
    golfBall.setRadius(0.5f);
    golfBall.setColor(glm::vec3(1.0f, 1.0f, 1.0f));     // white
    golfBall.setPosition(glm::vec3(2.0f, -1.0f, 5.0f)); // beside the cube ball
    scene.addObject(&golfBall);

    scene.build();

    BallPhysics ballPhysics;
    ballPhysics.setRadius(0.5f); // MUST match golfBall radius

    PhysicsWorld world;
    world.setBall(&golfBall, &ballPhysics);
    world.setGroundLevel(-2.0f); // top of Floor (ground at y=-2)
    world.addCollider(&leftWall, Surface::SOLID);
    world.addCollider(&rightWall, Surface::SOLID);
    world.addCollider(&backWall, Surface::SOLID);
    world.addCollider(&obstacle, Surface::SOLID);

    LightSet lights;
    updateLightingForDayNight(lights, isNight);
    
    std::cout << "=== TopFloor Golf - Lighting System ===" << std::endl;
    std::cout << "Press 'N' to toggle Day/Night" << std::endl;
    std::cout << "Current mode: Day" << std::endl;

    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleInput(window, rotationX, rotationY, deltaTime, postProcessor, isNight, lights);
        camera.processInput(window, deltaTime);

        static bool spaceWasDown = false;
        bool spaceDown = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (spaceDown && !spaceWasDown){
            ballPhysics.putt(glm::vec3(0.0f, 0.0f, -6.0f)); // fixed −Z putt for now
        }
        spaceWasDown = spaceDown;

        world.update(deltaTime);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(WIDTH, HEIGHT);

        cube1.setRotation(glm::vec3(
            glm::degrees(rotationX),
            glm::degrees(rotationY),
            0.0f
        ));

        glm::mat4 lightSpaceMatrix = calculateLightSpaceMatrix(lights);

        GLuint depthShader = ShaderManager::get("shadow_depth");
        glUseProgram(depthShader);
        glUniformMatrix4fv(
            glGetUniformLocation(depthShader, "lightSpaceMatrix"),
            1,
            GL_FALSE,
            &lightSpaceMatrix[0][0]
        );

        shadowMap.beginDepthPass();
        scene.drawDepthAllObjects(depthShader);
        shadowMap.endDepthPass(WIDTH, HEIGHT);

        // Clear screen
        postProcessor.beginRender();
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        GLuint shaderProgram = ShaderManager::get("basic");
        glUseProgram(shaderProgram);
        
        passLightingUniforms(shaderProgram, lights, camera.getPosition());
        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "lightSpaceMatrix"),
            1,
            GL_FALSE,
            &lightSpaceMatrix[0][0]
        );

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMap.getDepthTexture());
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 1);
        
        scene.drawAllObjects(view, projection, lights);

        // PostProcessor 
        postProcessor.endRender();

        postProcessor.draw();

        /// HUD
        hud.draw(camera, WIDTH, HEIGHT, postProcessor.getMode());

        // =========================
        // Window HUD Text
        // =========================

        glm::vec3 camPos = camera.getPosition();

        std::string modeText = "Normal";

        if(postProcessor.getMode() == 1)
        {
            modeText = "Grayscale";
        }
        else if(postProcessor.getMode() == 2)
        {
            modeText = "Inverted";
        }

        std::string title =
            "Mini Golf | "
            "X: " + std::to_string(camPos.x) +
            " Y: " + std::to_string(camPos.y) +
            " Z: " + std::to_string(camPos.z) +
            " | Filter: " + modeText;

        glfwSetWindowTitle(window, title.c_str());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}