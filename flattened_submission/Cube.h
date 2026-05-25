#ifndef CUBE_H
#define CUBE_H

#include "SceneObject.h"
#include "shader.hpp"

class Cube : public SceneObject
{
private:
    GLuint vao;
    GLuint vbo;

public:
    void build() override;

    void draw(
        const glm::mat4& view,
        const glm::mat4& proj,
        const LightSet& lights
    ) override;

    void drawDepth(GLuint depthShaderProgram) override;
};

#endif