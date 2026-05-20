#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <GL/glut.h>
#include <cmath>
#include "SceneObject.h"


class Scene{
    private:
        std::vector<SceneObject*> objects;
        
};

#endif
