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


class AssimpModel : public SceneObject
{
public:
    explicit AssimpModel(const std::string &path);
    ~AssimpModel() override;

    
    void build() override;

    void draw(const glm::mat4 &view,
              const glm::mat4 &proj,
              const LightSet &lights) override;

    void drawDepth(GLuint depthShaderProgram) override;

    
    const std::vector<SubMesh> &getSubMeshes() const { return subMeshes; }

    
    void setSubMeshColor(const std::string &keyword, glm::vec3 color);
    void printSubMeshes() const;

private:
    std::vector<SubMesh> subMeshes;
    std::string filePath;
    bool built = false; 
};

#endif