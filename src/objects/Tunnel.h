#ifndef TUNNEL_H
#define TUNNEL_H

#include "../core/SceneObject.h"
#include "../core/shader.hpp"

// Tunnel: a hollow cylinder lying on its side (axis along Z).
// Inner ring visible from inside; outer ring visible from outside.
// Length along Z, radius = tunnelRadius.
// Place at the centre of the tunnel passage.
class Tunnel : public SceneObject
{
public:
    float tunnelRadius;   // default 1.0
    float tunnelLength;   // default 2.0
    float wallThickness;  // default 0.15

    Tunnel();

    void build()     override;
    void draw(const glm::mat4& view,
              const glm::mat4& proj,
              const LightSet&  lights) override;
    void drawDepth(GLuint depthShaderProgram) override;

private:
    GLuint vao;
    GLuint vbo;
    int    vertexCount;
    int    segments;
};

#endif
