// Camera.h
#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    private: 
        glm::vec3 position = glm::vec3(0.0f, 2.0f, 5.0f);
        float yaw = -90.0f;
        float pitch = 0.0f;
        float roll = 0.0f;

        float speed = 5.0f;
        float rollSpeed = 45.0f;   
        float sensitivity =  0.1f;  
        static void mouseCallback(GLFWwindow* w, double xpos, double ypos);
    public:
        glm::mat4 getView() const;
        glm::mat4 getProjection(float aspect) const;

        void processKeyboard(GLFWwindow* w, float dt);
        void processMouse(float dx, float dy);
        void registerCallbacks(GLFWwindow* w);
    

};