#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include "shader.hpp"


class ShaderManager{
    private:
        GLuint id;
        ShaderManager* instance;
        ShaderManager();
    
    public:
        static GLuint get(const std::string& name);
};

#endif