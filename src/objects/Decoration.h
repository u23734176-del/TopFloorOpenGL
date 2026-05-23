#ifndef DECORATION_H
#define DECORATION_H

// ---------------------------------------------------------------------------
// Decoration.h  (Slice E - Karabo)
//
// FLYWEIGHT pattern for repeated scenery. The expensive, shared (intrinsic)
// state -- the geometry (VAO/VBO/vertex count) and the compiled shader -- lives
// once in a DecorationType. Each Decoration instance is lightweight: it holds
// only extrinsic state (position/rotation/scale/colour, inherited from
// SceneObject) and a pointer to its shared DecorationType.
//
// So 30 pathway lights = 1 DecorationType (one VBO) + 30 Decoration instances.
//
// DecorationFactory owns the DecorationType flyweights and hands out shared
// pointers, building each geometry only the first time it is requested.
// ---------------------------------------------------------------------------

#include "../core/SceneObject.h"
#include "../core/shader.hpp"
#include <vector>
#include <string>
#include <map>

// The kinds of scenery we provide (>=5 distinct decorations as the spec asks).
enum class DecorationKind
{
    GAZEBO,
    BRIDGE,
    TREE,
    BENCH,
    BILLBOARD,
    PATHWAY_LIGHT
};

// ---- the shared flyweight: geometry built once, reused by every instance ----
class DecorationType
{
public:
    DecorationType();
    ~DecorationType();

    // Build the geometry for a given kind (called once by the factory).
    void build(DecorationKind kind);

    // Bind + draw this shared geometry with a caller-supplied transform/colour.
    void draw(const glm::mat4 &model,
              const glm::vec3 &color,
              const glm::mat4 &view,
              const glm::mat4 &proj) const;

    // Depth-only draw for the shadow map pass (model uniform + geometry only).
    void drawDepth(const glm::mat4 &model, GLuint depthShaderProgram) const;

    bool isBuilt() const { return built; }

private:
    GLuint vao;
    GLuint vbo;
    GLuint shader;
    int vertexCount;
    bool built;

    // geometry generators -- each appends interleaved pos+normal to `data`
    static void genGazebo(std::vector<float> &data);
    static void genBridge(std::vector<float> &data);
    static void genTree(std::vector<float> &data);
    static void genBench(std::vector<float> &data);
    static void genBillboard(std::vector<float> &data);
    static void genPathwayLight(std::vector<float> &data);

    // shared primitive helpers (append a box / cylinder to data)
    static void appendBox(std::vector<float> &data,
                          const glm::vec3 &centre, const glm::vec3 &halfExtents);
    static void appendCylinder(std::vector<float> &data,
                               const glm::vec3 &base, float radius,
                               float height, int segments);

    void upload(const std::vector<float> &data);
};

// ---- the lightweight instance: references a shared DecorationType -----------
class Decoration : public SceneObject
{
public:
    explicit Decoration(DecorationType *shared);

    void build() override; // no-op for geometry: the flyweight already built it
    void draw(const glm::mat4 &view,
              const glm::mat4 &proj,
              const LightSet &lights) override;
    void drawDepth(GLuint depthShaderProgram) override;

private:
    DecorationType *type; // shared, NOT owned
};

// ---- factory: owns the flyweights, builds each kind at most once ------------
class DecorationFactory
{
public:
    static DecorationFactory *getInstance();
    ~DecorationFactory();

    // Get (build on first request) the shared geometry for a kind.
    DecorationType *getType(DecorationKind kind);

    // Convenience: create an instance at a position. Caller owns the pointer
    // and adds it to the Scene. Geometry is shared via the flyweight.
    Decoration *create(DecorationKind kind, const glm::vec3 &position);

private:
    DecorationFactory();
    static DecorationFactory *instance;
    std::map<DecorationKind, DecorationType *> types;
};

#endif