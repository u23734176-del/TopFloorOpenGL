#include "Camera.h"

glm::mat4 Camera::getView() const{
    glm::vec3 front;

    front.x = cos(glm::radians(yaw))* cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    glm::vec3 worldUp = glm::vec3(-sin(glm::radians(roll)), cos(glm::radians(roll)),0.0f);

    glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
    glm::vec3 up =  glm::normalize(glm::cross(right, front));

    return glm::lookAt(position, position + front, up);
}

void Camera::processMouse(float dx, float dy) {
    yaw   += dx * sensitivity;
    pitch += dy * sensitivity;
    pitch  = glm::clamp(pitch, -89.0f, 89.0f);
}

glm::mat4 Camera::getProjection(float aspect) const {
    return glm::perspective(glm::radians(45.0f), aspect, 0.1f, 200.0f);
}

void Camera::processMouse(float dx, float dy) {
    yaw   += dx * sensitivity;
    pitch += dy * sensitivity;                         
    pitch  = glm::clamp(pitch, -89.0f, 89.0f);
}

void Camera::registerCallbacks(GLFWwindow* w) {
    glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(w, this);           // store camera pointer on the window
    glfwSetCursorPosCallback(w, mouseCallback);
}

void Camera::mouseCallback(GLFWwindow* w, double xpos, double ypos) {
    Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(w));

    if (cam->firstMouse) {
        cam->lastX = (float)xpos;
        cam->lastY = (float)ypos;
        cam->firstMouse = false;
        return;
    }

    float dx = (float)xpos - cam->lastX;
    float dy = cam->lastY - (float)ypos;

    cam->lastX = (float)xpos;
    cam->lastY = (float)ypos;

    cam->processMouse(dx, dy);
}