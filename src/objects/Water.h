#ifndef WATER_H
#define WATER_H

// ---------------------------------------------------------------------------
// Water.h  (Slice C - Karabo's water task)
//
// A flat, semi-transparent water surface. Unlike the old Water (a scaled cube),
// this is a single horizontal quad with an upward normal, drawn with its own
// water shader (alpha-blended, high specular). It is a SceneObject so it slots
// into the scene, but it should be drawn AFTER opaque geometry (see draw()).
//
// Transparency requires basic.frag's hardcoded alpha=1.0 to be bypassed, which
// is why water uses its own water.vert/water.frag pair rather than "basic".
// ---------------------------------------------------------------------------

#include "../core/SceneObject.h"
#include "../core/shader.hpp"

class Water : public SceneObject
{
private:
    GLuint vao;
    GLuint vbo;
    GLuint shader;
    int vertexCount;

    float sizeX;     // half-extent in x
    float sizeZ;     // half-extent in z
    float alpha;     // transparency (0 = invisible, 1 = opaque)
    float shininess; // specular exponent (high for water sheen)

public:
    Water();

    void setSize(float halfX, float halfZ);
    void setAlpha(float a);
    void setShininess(float s);

    void build() override;
    void draw(const glm::mat4 &view,
              const glm::mat4 &proj,
              const LightSet &lights) override;
    void drawDepth(GLuint depthShaderProgram) override;
};

#endif