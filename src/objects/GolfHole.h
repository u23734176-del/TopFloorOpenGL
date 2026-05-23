#ifndef GOLFHOLE_H
#define GOLFHOLE_H

#include "../core/SceneObject.h"
#include "../core/shader.hpp"

#include <vector>

// -----------------------------------------------------------------------
// GolfHole
//
// A self-contained hole built from flat quads (floor + border walls) and
// a flag (thin pole + coloured triangle).
//
// Coordinate convention (local before setPosition):
//   X = width direction
//   Y = up
//   Z = length direction (tee at +Z end, cup at -Z end)
//
// The hole is centred on the world position you give it.
//
// For non-rectangular holes (L-shapes, curves) create two GolfHoles and
// position them to form the shape — or use the segment list constructor.
// -----------------------------------------------------------------------

// One rectangular segment of a hole (used for multi-segment layouts)
struct HoleSegment {
    float offsetX;   // centre offset from hole origin in X
    float offsetZ;   // centre offset from hole origin in Z
    float width;
    float length;
    float rotY;      // rotation around Y (degrees) to allow L/curve shapes
};

class GolfHole : public SceneObject
{
public:
    // ---- build parameters ----
    float               holeWidth;    // fairway width  (X)
    float               holeLength;   // fairway length (Z)
    float               wallHeight;   // height of border walls
    float               wallThick;    // thickness of border walls
    glm::vec3           turfColor;    // green
    glm::vec3           wallColor;    // border colour
    glm::vec3           flagColor;    // flag triangle colour
    bool                hasSand;      // draw a sand patch?
    glm::vec3           sandOffset;   // where in the hole (local XZ)
    glm::vec2           sandSize;     // width, length of sand patch
    int                 holeNumber;   // for identification only

    // Extra rectangular segments (for L-bends etc.)
    std::vector<HoleSegment> extraSegments;

    GolfHole();

    void build()     override;
    void draw(const glm::mat4& view,
              const glm::mat4& proj,
              const LightSet&  lights) override;
    void drawDepth(GLuint depthShaderProgram) override;

private:
    // ---- GPU data ----
    // We pack everything into one VAO/VBO for simplicity
    GLuint vao;
    GLuint vbo;
    int    vertexCount;

    // sand patch
    GLuint sandVao;
    GLuint sandVbo;
    int    sandVertexCount;

    // flag pole + triangle
    GLuint flagVao;
    GLuint flagVbo;
    int    flagVertexCount;

    // ---- helpers ----
    void pushQuad(std::vector<float>& buf,
                  glm::vec3 tl, glm::vec3 tr,
                  glm::vec3 bl, glm::vec3 br,
                  glm::vec3 normal);

    void buildSegment(std::vector<float>& turfBuf,
                      float cx, float cz,
                      float w,  float l, float rotY);
};

#endif
