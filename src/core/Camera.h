#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    private: 
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
    
    public:
        Camera();

        glm::mat4 getViewMatrix() const;
}

#endif