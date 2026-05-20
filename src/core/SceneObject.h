#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include "../lighting/LightSet.h"

class SceneObject{
    public:
        virtual void build() = 0;
        virtual void draw(
            const glm::mat4& view,
            const glm::mat4& proj,
            const LightSet& lights
        ) = 0;

        virtual ~SceneObject() = default;
};


#endif