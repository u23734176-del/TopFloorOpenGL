#include "Camera.h"

Camera::Camera() {
    position = glm::vec3(0.0f, 3.0f, 10.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);

    yaw = -90.0f;
    pitch = 0.0f;
    speed = 5.0f;
    roll = 0.0f;

    sensitivity = 0.05f;
    firstMouse  = true;
    lastX       = 0.0f;
    lastY       = 0.0f;
    mouseInWindow = true;
}
void Camera::setFirstPerson(const glm::vec3& pos, const glm::vec3& droneF)
{
    
    position = pos + droneF * 0.4f + glm::vec3(0.0f, 0.15f, 0.0f);

    yaw   = glm::degrees(atan2(droneF.z, droneF.x));
    pitch = glm::degrees(asin(glm::clamp(droneF.y, -1.0f, 1.0f)));

    updateFront();
}
void Camera::followTarget(const glm::vec3& targetPos, const glm::vec3& targetFront, float rollDeg)
{
    float distance = 3.0f;
    float height   = 1.5f;

    position = targetPos - targetFront * distance + glm::vec3(0.0f, height, 0.0f);

    glm::vec3 dir = glm::normalize(targetPos - position);
    yaw   = glm::degrees(atan2(dir.z, dir.x));
    pitch = glm::degrees(asin(glm::clamp(dir.y, -1.0f, 1.0f)));

    updateFront();
}

void Camera::updateFront()
{
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
}

glm::mat4 Camera::getViewMatrix()
{
    glm::mat4 rollMat  = glm::rotate(glm::mat4(1.0f), glm::radians(roll), front);
    glm::vec3 rolledUp = glm::normalize(glm::vec3(rollMat * glm::vec4(up, 0.0f)));
    return  glm::lookAt(position, position + front, rolledUp);
}

glm::mat4 Camera::getProjectionMatrix(float width, float height)
{
    return glm::perspective(
        glm::radians(45.0f),
        width / height,
        0.1f,
        200.0f
    );
}

void Camera::processInput(GLFWwindow* window, float deltaTime)
{
    float velocity = speed * deltaTime;
    float rotationSpeed = 60.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) velocity *= 2.0f;

    
    
    
    

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += front * velocity;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= front * velocity;

    glm::vec3 right = glm::normalize(glm::cross(front, up));

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * velocity;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * velocity;

    
    
    

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        yaw -= rotationSpeed;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        yaw += rotationSpeed;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        pitch += rotationSpeed;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pitch -= rotationSpeed;

    
    if (pitch > 89.0f)
        pitch = 89.0f;

    if (pitch < -89.0f)
        pitch = -89.0f;

    
    
    

    glm::vec3 direction;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(direction);

     
    float rollSpeed = 60.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        roll -= rollSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        roll += rollSpeed;
}

glm::vec3 Camera::getPosition()
{
    return position;
}

void Camera::processMouseMovement(float xpos, float ypos)
{
    if (!mouseInWindow) return;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    yaw   += (xpos - lastX) * sensitivity;
    pitch += (lastY - ypos) * sensitivity;

    lastX = xpos;
    lastY = ypos;

    if (pitch >  89.0f) pitch =  89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateFront();
}

void Camera::resetMouse()
{
    firstMouse = true;
}

float Camera::getRoll()
{
    return this->roll;
}

float Camera::getYaw()
{
    return this->yaw;
}

float Camera::getPitch()
{
    return this->pitch;
}