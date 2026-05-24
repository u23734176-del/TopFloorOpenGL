#ifndef SPHEREBALL_H
#define SPHEREBALL_H

// ---------------------------------------------------------------------------
// SphereBall.h  (Slice E - Karabo)
//
// The playable golf ball with real UV-sphere geometry (not a cube). Generated
// procedurally in build() -- no pre-built model, satisfying the spec rule that
// every model is made by us.
//
// Vertex layout matches Cube: interleaved [position.xyz, normal.xyz], attribute
// 0 = position, 1 = normal, so it works with the existing basic.vert/frag and
// the LightSet draw signature. The shared objects/Ball.h is intentionally NOT
// modified -- this is a separate drawable.
// ---------------------------------------------------------------------------

#include "../core/SceneObject.h"
#include "../core/shader.hpp"
#include <vector>

class SphereBall : public SceneObject
{
private:
    GLuint vao;
    GLuint vbo;
    GLuint shader;
    int vertexCount; // number of vertices to draw (triangles)

    int stacks;   // latitude divisions
    int slices;   // longitude divisions
    float radius; // geometric radius (matches physics BoundingSphere)

public:
    SphereBall();

    // Optionally tune tessellation / size before build().
    void setResolution(int stacks_, int slices_);
    void setRadius(float r);
    float getRadius() const;

    void build() override;

    void draw(
        const glm::mat4 &view,
        const glm::mat4 &proj,
        const LightSet &lights) override;

    void drawDepth(GLuint depthShaderProgram) override;
};

#endif