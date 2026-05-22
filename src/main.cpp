#include <iostream>
#include <vector>
#include <random>

// opengl imports
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>

include <cmath>

// Our stuff
#include "core/Scene.h"
#include "core/Camera.h"

#include "objects/Cube.h"

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
    window = glfwCreateWindow(width, height, "u24676412", NULL, NULL);
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

inline void handleInput(GLFWwindow*& window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, 1);
    }
}

int main()
{
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

    // Scene
    Scene scene;

    // Objects
    Cube cube;

    scene.addObject(&cube);

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
        handleInput(window);
        camera.processInput(window, deltaTime);

        // Clear screen
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(WIDTH, HEIGHT);

        // Draw scene
        scene.drawAllObjects(view, projection, lights);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}