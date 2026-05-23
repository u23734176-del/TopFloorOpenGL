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
#include <cmath>

// Our objects
#include "core/Scene.h"
#include "core/Camera.h"
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

// Post Processor 
#include "ui/PostProcessor.h"

// HUD 
#include "ui/HUD.h"

const int WIDTH = 1000;
const int HEIGHT = 800;

const char *getError()
{
    const char *errorDescription;
    glfwGetError(&errorDescription);
    return errorDescription;
}

inline void startUpGLFW()
{
    glewExperimental = true; // Needed for core profile
    if (!glfwInit())
    {
        throw getError();
    }
}

inline void startUpGLEW()
{
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        throw getError();
    }
}

inline GLFWwindow *setUp(int width, int height)
{
    startUpGLFW();
    glfwWindowHint(GLFW_SAMPLES, 4);               // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
    GLFWwindow *window;                                            // (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow(width, height, "Homework Assignment", NULL, NULL);
    if (window == NULL)
    {
        std::cout << getError() << std::endl;
        glfwTerminate();
        throw "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n";
    }
    glfwMakeContextCurrent(window); // Initialize GLEW
    startUpGLEW();
    glViewport(0, 0, width, height);
    return window;
}

inline void handleInput(GLFWwindow*& window, float& rotationX, float& rotationY, float deltaTime, PostProcessor& postProcessor) {
    // Close window
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, 1);
    }

    float rotationSpeed = 2.0f * deltaTime;

    // Rotate left/right
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        rotationY -= rotationSpeed;
    }

    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        rotationY += rotationSpeed;
    }

    // Rotate up/down
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
}

int main()
{
    float rotationX = 0.0f;
    float rotationY = 0.0f;

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

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Camera
    Camera camera;

    // PostProcessor
    PostProcessor postProcessor;
    postProcessor.build(WIDTH, HEIGHT);

    // HUD
    HUD hud;
    hud.build();

    // Scene
    Scene scene;

    // Objects
    // Cube
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

    // Floor
    Floor ground;
    ground.setPosition(glm::vec3(0.0f, -2.0f, 0.0f));
    ground.setColor(glm::vec3(0.1f, 0.6f, 0.1f));
    scene.addObject(&ground);

    // Obstacles
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

    // Build all scene objects
    scene.build();

    // Lighting
    LightSet lights;
    lights.direction = glm::vec3(-1.0f, -1.0f, -1.0f);
    lights.ambient  = glm::vec3(0.2f);
    lights.diffuse  = glm::vec3(1.0f);
    lights.specular = glm::vec3(1.0f);

    // Timing
    float lastFrame = 0.0f;

    // =========================
    // Render Loop
    // =========================
    while (!glfwWindowShouldClose(window))
    {
        // Time calculations
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        handleInput(window, rotationX, rotationY, deltaTime, postProcessor);
        camera.processInput(window, deltaTime);

        // Clear screen
        postProcessor.beginRender();

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(WIDTH, HEIGHT);

        // Rotation
        cube1.setRotation(glm::vec3(
            glm::degrees(rotationX),
            glm::degrees(rotationY),
            0.0f
        )); 
        // Draw scene
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

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}