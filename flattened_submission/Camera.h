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
        float roll;

        bool firstMouse;
        float lastX, lastY;
        bool mouseInWindow;
        float sensitivity;
        
        void updateFront();
    public:
        Camera();

        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix(float width, float height);

        void setFirstPerson(const glm::vec3& pos, const glm::vec3& droneF);
        void processInput(GLFWwindow* window, float deltaTime);
        void processMouseMovement(float xpos, float ypos);
        glm::vec3 getPosition();
        void resetMouse();
        float getRoll();

        float getYaw();
        float getPitch();
        void followTarget(const glm::vec3& targetPos, const glm::vec3& targetFront, float rollDeg);

};

#endif