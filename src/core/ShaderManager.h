#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <iostream>
#include <string>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include "shader.hpp"


class ShaderManager{
    private:
        static std::map<std::string, GLuint> shaders;
        GLuint id;
        ShaderManager* instance;
        ShaderManager();
    
    public:
        static GLuint get(const std::string& name);
        static void load(const std::string& name, const char* vertexPath, const char* fragmentPath);
};

#endif