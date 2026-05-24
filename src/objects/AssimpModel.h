#ifndef ASSIMPMODEL_H
#define ASSIMPMODEL_H

#include "../core/SceneObject.h"
#include <string>
#include <vector>

struct SubMesh
{
    GLuint vao;
    GLuint vbo;
    int vertexCount;
    glm::vec3 diffuseColor;
    std::string name; // <--- NEW: To store the mesh name
};

class AssimpModel : public SceneObject
{
private:
    std::vector<SubMesh> subMeshes;
    std::string filePath;

public:
    explicit AssimpModel(const std::string &path);
    ~AssimpModel();

    void build() override;

    void draw(const glm::mat4 &view,
              const glm::mat4 &proj,
              const LightSet &lights) override;

    void drawDepth(GLuint depthShaderProgram) override;

    // <--- NEW: Methods to manually color parts of the model
    void setSubMeshColor(const std::string &keyword, glm::vec3 color);
    void printSubMeshes() const;
};

#endif