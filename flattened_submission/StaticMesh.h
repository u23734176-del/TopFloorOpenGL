#ifndef STATICMESH_H
#define STATICMESH_H

#include "SceneObject.h"
#include "shader.hpp"
#include <vector>



class StaticMesh : public SceneObject
{
private:
    GLuint vao;
    GLuint vbo;
    int vertexCount;
    std::vector<float> vertexData;

public:
    
    explicit StaticMesh(const std::vector<float> &geometry);

    void build() override;

    void draw(const glm::mat4 &view,
              const glm::mat4 &proj,
              const LightSet &lights) override;

    void drawDepth(GLuint depthShaderProgram) override;
};

#endif