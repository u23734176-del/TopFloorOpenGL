#ifndef DECORATION_H
#define DECORATION_H

#include "../core/SceneObject.h"
#include "../core/shader.hpp"
#include <vector>
#include <string>
#include <map>


enum class DecorationKind
{
    GAZEBO,
    BRIDGE,
    TREE,
    BENCH,
    BILLBOARD,
    PATHWAY_LIGHT
};


class DecorationType
{
public:
    DecorationType();
    ~DecorationType();

    
    void build(DecorationKind kind);

    
    void draw(const glm::mat4 &model,
              const glm::vec3 &color,
              const glm::mat4 &view,
              const glm::mat4 &proj) const;

    
    void drawDepth(const glm::mat4 &model, GLuint depthShaderProgram) const;

    bool isBuilt() const { return built; }

private:
    GLuint vao;
    GLuint vbo;
    GLuint shader;
    int vertexCount;
    bool built;

    
    static void genGazebo(std::vector<float> &data);
    static void genBridge(std::vector<float> &data);
    static void genTree(std::vector<float> &data);
    static void genBench(std::vector<float> &data);
    static void genBillboard(std::vector<float> &data);
    static void genPathwayLight(std::vector<float> &data);

    
    static void appendBox(std::vector<float> &data,
                          const glm::vec3 &centre, const glm::vec3 &halfExtents);
    static void appendCylinder(std::vector<float> &data,
                               const glm::vec3 &base, float radius,
                               float height, int segments);

    void upload(const std::vector<float> &data);
};


class Decoration : public SceneObject
{
public:
    explicit Decoration(DecorationType *shared);

    void build() override; 
    void draw(const glm::mat4 &view,
              const glm::mat4 &proj,
              const LightSet &lights) override;
    void drawDepth(GLuint depthShaderProgram) override;

private:
    DecorationType *type; 
};


class DecorationFactory
{
public:
    static DecorationFactory *getInstance();
    ~DecorationFactory();

    
    DecorationType *getType(DecorationKind kind);

    
    
    Decoration *create(DecorationKind kind, const glm::vec3 &position);

private:
    DecorationFactory();
    static DecorationFactory *instance;
    std::map<DecorationKind, DecorationType *> types;
};

#endif