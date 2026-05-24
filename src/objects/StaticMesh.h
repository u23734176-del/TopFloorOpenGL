#ifndef STATICMESH_H
#define STATICMESH_H

#include "../core/SceneObject.h"
#include "../core/shader.hpp"
#include <vector>

// A reusable wrapper for any baked OBJ data.
// Handles the VAO/VBO generation with the new 8-float stride (Pos, Norm, UV).
class StaticMesh : public SceneObject
{
private:
    GLuint vao;
    GLuint vbo;
    int vertexCount;
    std::vector<float> vertexData;

public:
    // Takes the generated vector from the python script
    explicit StaticMesh(const std::vector<float> &geometry);

    void build() override;

    void draw(const glm::mat4 &view,
              const glm::mat4 &proj,
              const LightSet &lights) override;

    void drawDepth(GLuint depthShaderProgram) override;
};

#endif