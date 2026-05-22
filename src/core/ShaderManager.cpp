#include "ShaderManager.h"
#inlcude "shader.hpp"

std::map<std::string, GLuint> ShaderManager::shaders;

void ShaderManager::load(const std::string& name, const char* vertexPath, const char* fragmentPath) {
    GLuint shaderID = LoadShaders(vertexPath, fragmentPath);
    shaders[name] = shaderID;
}

GLuint ShaderManager::get(const std::string& name) {
    if (shaders.find(name) != shaders.end()) {
        return shaders[name];
    } else {
        std::cerr << "Shader not found: " << name << std::endl;
        return 0; // Return 0 for invalid shader
    }
}
