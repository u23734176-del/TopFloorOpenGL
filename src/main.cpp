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

    // Placeholder matrices (will be updated by the Drone Camera later)
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 200.0f);

    while (!glfwWindowShouldClose(window)){
        handleInput(window);
        
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
    delete water;
    delete ball;
    glfwTerminate();
    return 0;
}