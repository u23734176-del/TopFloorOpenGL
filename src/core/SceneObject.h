#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../lighting/LightSet.h"

class SceneObject
{
protected:

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

public:

    SceneObject()
    {
        position = glm::vec3(0.0f);
        rotation = glm::vec3(0.0f);
        scale = glm::vec3(1.0f);
    }

    virtual void build() = 0;

    virtual void draw(
        const glm::mat4& view,
        const glm::mat4& projection,
        const LightSet& lights
    ) = 0;

    // =========================
    // Transform Functions
    // =========================

    void setPosition(glm::vec3 pos)
    {
        position = pos;
    }

    void setRotation(glm::vec3 rot)
    {
        rotation = rot;
    }

    void setScale(glm::vec3 s)
    {
        scale = s;
    }

    glm::vec3 getPosition()
    {
        return position;
    }

    glm::vec3 getRotation()
    {
        return rotation;
    }

    glm::vec3 getScale()
    {
        return scale;
    }

    glm::mat4 getModelMatrix()
    {
        glm::mat4 model = glm::mat4(1.0f);

        // Translation
        model = glm::translate(
            model,
            position
        );

        // Rotation X
        model = glm::rotate(
            model,
            glm::radians(rotation.x),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        // Rotation Y
        model = glm::rotate(
            model,
            glm::radians(rotation.y),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // Rotation Z
        model = glm::rotate(
            model,
            glm::radians(rotation.z),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        // Scale
        model = glm::scale(
            model,
            scale
        );

        return model;
    }
};

#endif