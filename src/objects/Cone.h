#ifndef CONE_H
#define CONE_H

#include "../core/SceneObject.h"
#include "../core/shader.hpp"



class Cone : public SceneObject
{
private:
    GLuint vao;
    GLuint vbo;
    int    vertexCount;
    int    segments;

public:
    explicit Cone(int segments = 24);

    void build()     override;
    void draw(const glm::mat4& view,
              const glm::mat4& proj,
              const LightSet&  lights) override;
    void drawDepth(GLuint depthShaderProgram) override;
};

#endif
