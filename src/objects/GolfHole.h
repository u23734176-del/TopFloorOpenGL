#ifndef GOLFHOLE_H
#define GOLFHOLE_H

#include "../core/SceneObject.h"
#include "../core/shader.hpp"
#include <vector>
#include <glm/glm.hpp>

// -----------------------------------------------------------------------
// GolfHole: Reworked for Crescent Head aesthetics
// -----------------------------------------------------------------------

struct HoleSegment
{
    float offsetX;
    float offsetZ;
    float width;
    float length;
    float rotY;
};

class GolfHole : public SceneObject
{
public:
    // ---- Aesthetic & Proportional Tunables ----
    float holeWidth;
    float holeLength;
    float wallHeight; // Exposed for visual tuning
    float wallThick;  // Exposed for visual tuning

    glm::vec3 turfColor; // Bright synthetic green
    glm::vec3 wallColor; // Wood/Brick border tone
    glm::vec3 flagColor;

    bool hasSand;
    glm::vec3 sandOffset;
    glm::vec2 sandSize;

    int holeNumber;

    // Centerline points for kidney/curved fairways (local XZ, tee -> cup)
    std::vector<glm::vec2> centerline;
    std::vector<HoleSegment> extraSegments;

    GolfHole();
    ~GolfHole();

    void build() override;
    void draw(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights) override;
    void drawDepth(GLuint depthShaderProgram) override;
    std::vector<AABB> getCollisionAABBs() const override;

private:
    // ---- GPU data ----
    GLuint vao, vbo;
    int vertexCount;

    GLuint sandVao, sandVbo;
    int sandVertexCount;

    GLuint flagVao, flagVbo;
    int flagVertexCount;

    // Physics hitboxes
    std::vector<AABB> wallBoxes;

    // ---- helpers ----
    void pushQuad(std::vector<float> &buf, glm::vec3 tl, glm::vec3 tr, glm::vec3 bl, glm::vec3 br, glm::vec3 normal);
    void buildSegment(std::vector<float> &turfBuf, float cx, float cz, float w, float l, float rotY);
    void buildRibbon(std::vector<float> &turfBuf, const std::vector<glm::vec2> &pts, float width);
};

#endif