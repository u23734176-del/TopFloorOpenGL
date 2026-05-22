#include <iostream>
#include <vector>
#include <random>
#include <cmath>

// opengl imports
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Our stuff
#include "core/Scene.h"
#include "layout/Water.h"
#include "objects/GolfBall.h"
#include "objects/TreeDecoration.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

const char *getError(){
    const char *errorDescription;
    glfwGetError(&errorDescription);
    return errorDescription;
}

inline void startUpGLFW(){
    glewExperimental = true; // Needed for core profile
    if (!glfwInit())
    {
        throw getError();
    }
}

inline void startUpGLEW(){
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        throw getError();
    }
}

inline GLFWwindow *setUp(int width, int height){
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

int main(){

    GLFWwindow *window;
    try{
        window = setUp(WIDTH, HEIGHT);
    }
    catch (const char *e){
        std::cout << e << std::endl;
        throw;
    }

    // Clear colour
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    Scene scene;
    LightSet lights; // Initialize with default values later

    Water *water = new Water();
    water->build();

    GolfBall *ball = new GolfBall();
    ball->build();

    scene.addTransparentObject(water);
    scene.addTransparentObject(ball);

    TreeDecoration *trees = new TreeDecoration();
    trees->build();

    // Add multiple instances of the tree around the course using Flyweight pattern
    trees->addInstance(glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 5.0f)));
    trees->addInstance(glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 8.0f)));
    trees->addInstance(glm::translate(glm::mat4(1.0f), glm::vec3(12.0f, 0.0f, -3.0f)));
    trees->addInstance(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 0.0f, -6.0f)), glm::vec3(1.5f)));

    scene.addOpaqueObject(trees);

    // Add these variables before the main loop
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // Create dummy obstacle for testing
    AABB testObstacle = {glm::vec3(2.0f, 0.0f, 2.0f), glm::vec3(3.0f, 1.0f, 3.0f)};

    // Placeholder matrices (will be updated by the Drone Camera later)
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 200.0f);

    while (!glfwWindowShouldClose(window)){
        // Calculate deltaTime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleInput(window);

        // Give ball initial push for testing if spacebar is pressed
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
            ball->applyForce(glm::vec3(0.5f, 0.0f, 0.5f));
        }

        // Fixed step physics update (simulating turf friction of 0.98)
        ball->update(deltaTime);
        ball->resolveGroundCollision(0.0f, 0.98f);

        if (intersects(ball->getBoundingSphere(), testObstacle)){
            ball->resolveCollision(testObstacle);
        }

        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Execute the ordered rendering pipeline
        scene.render(view, proj, lights);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    


    glfwDestroyWindow(window);
    glDeleteProgram(0);
    // 1. Delete all dynamically allocated SceneObjects
    // This triggers their destructors, which call glDeleteVertexArrays and glDeleteBuffers
    delete water;
    delete ball;
    delete trees;
    // 2. Clear Scene vectors (optional but good practice)
    // Note: If Scene owned the pointers, it would delete them in its destructor.
    // Since we created them in main, we delete them in main.

    
    // 3. Terminate GLFW context
    glfwTerminate();
    return 0;
}