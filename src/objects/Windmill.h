#ifndef WINDMILL_H
#define WINDMILL_H

#include "../core/SceneObject.h"
#include "../core/shader.hpp"

#include <vector>

// Windmill matching the reference render:
//
//  Structure (bottom to top):
//    1. Frustum body  — wide truncated cone, alternating red/cream panels
//                       (many thin segments like a barber pole)
//    2. Skirt brim    — flat wide disc/ring around the top of the frustum
//    3. House base    — short dark cuboid collar
//    4. House walls   — white cuboid
//    5. House windows — small blue quads on each face
//    6. Roof          — 4-sided pyramid (red)
//    7. Axle          — thin horizontal cylinder through house
//    8. Blades        — 4 flat rectangular planks in a cross, rotating around Z
//
// All geometry is in local space with base at y=0.
// setScale / setPosition / setRotation from SceneObject work normally.

class Windmill : public SceneObject
{
public:
    Windmill();

    void update(float dt); // <-- NEW

    void build()     override;
    void draw(const glm::mat4& view,
              const glm::mat4& proj,
              const LightSet&  lights) override;
    void drawDepth(GLuint depthShaderProgram) override;

private:
    // ---- Frustum body (alternating stripe panels, each a separate VAO) ----
    struct StripePanel {
        GLuint vao, vbo;
        int    count;
        bool   isCream;   // true = cream, false = red
    };
    std::vector<StripePanel> panels;

    GLuint botBaseVao, botBaseVbo;
    int botBaseCount;

    GLuint redCubeVao, redCubeVbo;
    int redCubeCount;

    // ---- Skirt brim (flat ring at top of frustum) ----
    GLuint skirtVao, skirtVbo;
    int    skirtCount;

    // ---- House base collar (dark cuboid) ----
    GLuint hbaseVao, hbaseVbo;
    int    hbaseCount;

    // ---- House walls (white cuboid) ----
    GLuint houseVao, houseVbo;
    int    houseCount;

    // ---- Windows (flat quads, one VAO drawn on 3 sides) ----
    GLuint winVao, winVbo;
    int    winCount;

    // ---- Roof pyramid (4-sided, red) ----
    GLuint roofVao, roofVbo;
    int    roofCount;

    // ---- Axle cylinder (horizontal, through house) ----
    GLuint axleVao, axleVbo;
    int    axleCount;

    // ---- Blade (one rectangular plank, drawn 4 times rotated) ----
    GLuint bladeVao, bladeVbo;
    int    bladeCount;

    // Geometry constants stored after build() for draw-time positioning
    float frustumTopY;    // y at top of frustum = base of skirt
    float houseBaseY;     // y at base of house collar
    float houseTopY;      // y at top of house walls
    float roofY;          // y at base of roof pyramid
    float axleY;          // y-centre of axle / blade hub
    float axleZ;          // z position of axle (front face of house)
    float bladeOffset;    // distance from axle centre to blade centre (half-span)

    // ---- GPU helpers ----
    void uploadBuf(const std::vector<float>& buf, GLuint& vao, GLuint& vbo, int& count);

    void drawOne(GLuint vao, int count,
                 const glm::mat4& mvp,
                 const glm::vec3& col,
                 GLuint shader) const;

    void drawOneDepth(GLuint vao, int count,
                      const glm::mat4& model,
                      GLuint shader) const;

    // ---- Geometry builders ----
    // Returns a flat buffer: position(3) + normal(3) per vertex
    std::vector<float> makeCuboidBuf(float w, float h, float d);
    std::vector<float> makeRoofBuf(float baseW, float baseD, float roofH);
    std::vector<float> makeSkirtBuf(float rInner, float rOuter, float y);
    std::vector<float> makeCylinderBuf(float r, float len, int segs);
    std::vector<float> makeBladeBuf(float halfLen, float halfW, float halfThick);
    std::vector<float> makeFrustumPanel(float rBot, float rTop, float h,
                                        float a0, float a1);
    std::vector<float> makeWindowBuf(float w, float h);

    float rotorAngle; // <-- NEW
};

#endif
