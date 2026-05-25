#ifndef ASSIMPMODEL_H
#define ASSIMPMODEL_H

#include "../core/SceneObject.h"
#include <string>
#include <vector>

struct SubMesh
{
    GLuint vao = 0;
    GLuint vbo = 0;
    int vertexCount = 0;
    glm::vec3 diffuseColor{0.5f, 0.5f, 0.5f};
    std::string name;
};

// ---------------------------------------------------------------------------
// AssimpModel
//
// Changes from original:
//   - build() is now IDEMPOTENT: a second call is a no-op if geometry is
//     already uploaded. ResourceManager calls build() once; Scene::build()
//     iterating over ModelInstances won't re-upload anything.
//   - draw() uses ShaderManager::setMat4 etc. instead of raw
//     glGetUniformLocation — view/projection are set ONCE per draw call,
//     not once per submesh.
//   - getSubMeshes() is exposed so ModelInstance can iterate submeshes
//     while supplying its own model matrix.
//   - AssimpModel can still draw itself standalone (backward-compatible),
//     but Course now creates ModelInstance wrappers instead of drawing
//     the shared pointer directly.
// ---------------------------------------------------------------------------
class AssimpModel : public SceneObject
{
public:
    explicit AssimpModel(const std::string &path);
    ~AssimpModel() override;

    // Uploads geometry to GPU. Safe to call multiple times — only runs once.
    void build() override;

    // Standalone draw using this object's own transform.
    // For shared meshes used via ModelInstance, prefer ModelInstance::draw().
    void draw(const glm::mat4 &view,
              const glm::mat4 &proj,
              const LightSet &lights) override;

    void drawDepth(GLuint depthShaderProgram) override;

    // Read-only access for ModelInstance
    const std::vector<SubMesh> &getSubMeshes() const { return subMeshes; }

    // Color overrides
    void setSubMeshColor(const std::string &keyword, glm::vec3 color);
    void printSubMeshes() const;

private:
    std::vector<SubMesh> subMeshes;
    std::string filePath;
    bool built = false; // Guard against double-build
};

#endif