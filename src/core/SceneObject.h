#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "../lighting/LightSet.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class SceneObject
{
protected:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 color;

public:
    SceneObject()
    {
        position = glm::vec3(0.0f);
        rotation = glm::vec3(0.0f);
        scale = glm::vec3(1.0f);
        color = glm::vec3(1.0f);
    }

    virtual ~SceneObject() {}

    virtual void build() = 0;

    virtual void draw(
        const glm::mat4& view,
        const glm::mat4& projection,
        const LightSet& lights
    ) = 0;

    virtual void drawDepth(GLuint depthShaderProgram) = 0;

    void setPosition(glm::vec3 p)
    {
        position = p;
    }

    void setRotation(glm::vec3 r)
    {
        rotation = r;
    }

    void setScale(glm::vec3 s)
    {
        scale = s;
    }

    void setColor(glm::vec3 c)
    {
        color = c;
    }

    glm::vec3 getColor()
    {
        return color;
    }

    glm::mat4 getModelMatrix()
    {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, position);

        model = glm::rotate(
            model,
            glm::radians(rotation.x),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        model = glm::rotate(
            model,
            glm::radians(rotation.y),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        model = glm::rotate(
            model,
            glm::radians(rotation.z),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        model = glm::scale(model, scale);

        return model;
    }
};

#endif