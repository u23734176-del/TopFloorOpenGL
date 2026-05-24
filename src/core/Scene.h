#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include "SceneObject.h"


class Scene {
    private:
        std::vector<SceneObject*> objects;
        
    public: 
        void addObject(SceneObject* obj);
        void build();

        void drawOpaqueObjects(const glm::mat4 &view,
                               const glm::mat4 &projection,
                               const LightSet &lights);

        void drawTransparentObjects(const glm::mat4 &view,
                                    const glm::mat4 &projection,
                                    const LightSet &lights);

        void drawDepthAllObjects(GLuint depthShaderProgram);
        
};

#endif
