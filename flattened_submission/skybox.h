#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Skybox {
public:
    Skybox();
    ~Skybox();
    
    
    bool build(const std::vector<std::string>& dayFaces,  const std::vector<std::string>& nightFaces);
    void draw(const glm::mat4& view, const glm::mat4& projection, bool isNight);

private:
    GLuint vao, vbo;
    GLuint dayCubemap, nightCubemap;
    GLuint shaderProgram;

    GLuint loadCubemap(const std::vector<std::string>& faces);
};

#endif