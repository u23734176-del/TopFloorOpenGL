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
    
    GLFWwindow *window = glfwCreateWindow(width, height, "TopFloor — OpenGL Golf", NULL, NULL);
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

// =====================================
// Update Day/Night Lighting
// =====================================
void updateLightingForDayNight(LightSet& lights, bool isNight)
{
    lights.isNight = isNight;
    
    if (isNight) {
        // ===== NIGHT MODE =====
        // Moon instead of sun
        lights.directional.direction = glm::normalize(glm::vec3(0.5f, -0.6f, 0.3f));
        lights.directional.color = glm::vec3(0.6f, 0.6f, 0.8f);  // cool blue moonlight
        
        // Ambient: dark blue night sky
        lights.ambient = glm::vec3(0.05f, 0.05f, 0.15f);
        
        // Enable point lights (path lights, building lights, etc.)
        lights.numPointLights = 3;
        
        // Warm path light 1
        lights.pointLights[0].position = glm::vec3(5.0f, 1.5f, -10.0f);
        lights.pointLights[0].color = glm::vec3(1.0f, 0.8f, 0.4f);
        lights.pointLights[0].intensity = 0.8f;
        lights.pointLights[0].radius = 15.0f;
        lights.pointLights[0].linear = 0.09f;
        lights.pointLights[0].quadratic = 0.032f;
        
        // Warm path light 2
        lights.pointLights[1].position = glm::vec3(-5.0f, 1.5f, -5.0f);
        lights.pointLights[1].color = glm::vec3(1.0f, 0.8f, 0.5f);
        lights.pointLights[1].intensity = 0.7f;
        lights.pointLights[1].radius = 15.0f;
        lights.pointLights[1].linear = 0.09f;
        lights.pointLights[1].quadratic = 0.032f;
        
        // Cool accent light
        lights.pointLights[2].position = glm::vec3(0.0f, 2.0f, -15.0f);
        lights.pointLights[2].color = glm::vec3(0.4f, 0.6f, 1.0f);
        lights.pointLights[2].intensity = 0.5f;
        lights.pointLights[2].radius = 20.0f;
        lights.pointLights[2].linear = 0.07f;
        lights.pointLights[2].quadratic = 0.017f;
        
    } else {
        // ===== DAY MODE =====
        // Sun high in sky
        lights.directional.direction = glm::normalize(glm::vec3(-0.3f, -0.9f, -0.2f));
        lights.directional.color = glm::vec3(1.0f, 0.95f, 0.8f);  // warm daylight
        
        // Bright white ambient (sky)
        lights.ambient = glm::vec3(0.4f, 0.42f, 0.45f);
        
        // Disable point lights during day
        lights.numPointLights = 0;
    }
}

inline void handleInput(GLFWwindow*& window, float& rotationX, float& rotationY, float deltaTime, bool& isNight, LightSet& lights)
{
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
    
    // Day/Night toggle (N key)
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

// =====================================
// Pass Lighting Uniforms to Shader
// =====================================
void passLightingUniforms(GLuint shaderProgram, const LightSet& lights, const glm::vec3& cameraPos)
{
    // === Directional Light ===
    glUniform3f(glGetUniformLocation(shaderProgram, "directional.direction"),
                lights.directional.direction.x, lights.directional.direction.y, lights.directional.direction.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "directional.color"),
                lights.directional.color.x, lights.directional.color.y, lights.directional.color.z);
    
    // === Ambient ===
    glUniform3f(glGetUniformLocation(shaderProgram, "ambient"),
                lights.ambient.x, lights.ambient.y, lights.ambient.z);
    
    // === Point Lights ===
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
    
    // === Spotlight ===
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
    
    // === Day/Night & Camera ===
    glUniform1i(glGetUniformLocation(shaderProgram, "isNight"), lights.isNight ? 1 : 0);
    glUniform3f(glGetUniformLocation(shaderProgram, "cameraPos"),
                cameraPos.x, cameraPos.y, cameraPos.z);
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

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ===== LOAD SHADERS =====
    ShaderManager::load("basic", "shaders/basic.vert", "shaders/basic.frag");

    // Camera
    Camera camera;

    // Scene
    Scene scene;

    // Objects
    Cube cube1;
    cube1.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    Cube cube2;
    cube2.setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
    cube2.setScale(glm::vec3(2.0f));

    Cube cube3;
    cube3.setPosition(glm::vec3(-5.0f, 0.0f, 0.0f));
    cube3.setRotation(glm::vec3(45.0f, 45.0f, 0.0f));

    scene.addObject(&cube1);
    scene.addObject(&cube2);
    scene.addObject(&cube3);

    // Floor
    Floor ground;
    ground.setPosition(glm::vec3(0.0f, -2.0f, 0.0f));
    scene.addObject(&ground);

    // Obstacles
    Cube obstacle;
    obstacle.setPosition(glm::vec3(3.0f, 0.0f, -5.0f));
    obstacle.setScale(glm::vec3(1.0f, 3.0f, 1.0f));
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

    // ===== LIGHTING SETUP =====
    LightSet lights;
    updateLightingForDayNight(lights, isNight);  // Start in day mode
    
    // Print initial mode
    std::cout << "=== TopFloor Golf - Lighting System ===" << std::endl;
    std::cout << "Press 'N' to toggle Day/Night" << std::endl;
    std::cout << "Current mode: Day" << std::endl;

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
        handleInput(window, rotationX, rotationY, deltaTime, isNight, lights);
        camera.processInput(window, deltaTime);

        // Clear screen
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(WIDTH, HEIGHT);

        // Update cube rotation
        cube1.setRotation(glm::vec3(
            glm::degrees(rotationX),
            glm::degrees(rotationY),
            0.0f
        ));
        
        // Get the shader program
        GLuint shaderProgram = ShaderManager::get("basic");
        glUseProgram(shaderProgram);
        
        // Pass lighting uniforms
        passLightingUniforms(shaderProgram, lights, camera.getPosition());
        
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