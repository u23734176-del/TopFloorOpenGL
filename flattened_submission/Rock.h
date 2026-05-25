#ifndef ROCK_H
#define ROCK_H

#include "SceneObject.h"
#include "shader.hpp"



class Rock : public SceneObject
{
private:
    GLuint vao;
    GLuint vbo;
    int    vertexCount;

public:
    Rock();

    void build()     override;
    void draw(const glm::mat4& view,
              const glm::mat4& proj,
              const LightSet&  lights) override;
    void drawDepth(GLuint depthShaderProgram) override;
};

#endif
