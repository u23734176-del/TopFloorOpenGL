#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    private: 
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;

        float yaw;
        float pitch;
        float speed;
    
    public:
        Camera();

        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix(float width, float height);

        void processInput(GLFWwindow* window, float deltaTime);

        glm::vec3 getPosition();
};

#endif