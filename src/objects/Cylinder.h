#ifndef CYLINDER_H
#define CYLINDER_H

#include "../core/SceneObject.h"
#include "../core/shader.hpp"

// Upright cylinder centred at origin, radius 1, half-height 1 in local space.
// Use setScale(glm::vec3(radius, halfHeight, radius)) to resize.
class Cylinder : public SceneObject
{
private:
    GLuint vao;
    GLuint vbo;
    int    vertexCount;
    int    segments;   // number of radial slices

public:
    explicit Cylinder(int segments = 24);

    void build()     override;
    void draw(const glm::mat4& view,
              const glm::mat4& proj,
              const LightSet&  lights) override;
    void drawDepth(GLuint depthShaderProgram) override;
};

#endif
