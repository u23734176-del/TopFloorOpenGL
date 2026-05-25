#ifndef GRASSBASE_H
#define GRASSBASE_H

#include "../core/SceneObject.h"
#include "../core/shader.hpp"

// GrassBase: a flat elliptical green "rough" pad placed under each golf hole.
// It sits between the grey path (y=0) and the hole turf (y=0.02),
// bleeding out beyond the fairway walls to mimic the organic green surround
// visible in the top-down course map.
//
// Built as a faceted disc (ellipse) using a triangle fan.
// Set radiusX and radiusZ before calling build().
class GrassBase : public SceneObject
{
public:
    float radiusX;   // half-extent in X
    float radiusZ;   // half-extent in Z
    int   segments;  // radial slices (default 24)

    GrassBase();

    void build()     override;
    void draw(const glm::mat4& view,
              const glm::mat4& proj,
              const LightSet&  lights) override;
    void drawDepth(GLuint depthShaderProgram) override;

private:
    GLuint vao, vbo;
    int    vertexCount;
};

#endif
