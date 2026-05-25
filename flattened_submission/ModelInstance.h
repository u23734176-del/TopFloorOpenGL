#ifndef MODELINSTANCE_H
#define MODELINSTANCE_H

#include "SceneObject.h"
#include "AssimpModel.h"

class ModelInstance : public SceneObject
{
public:
    explicit ModelInstance(AssimpModel *sharedMesh);
    ~ModelInstance() override = default;

    
    void build() override; 
    void draw(const glm::mat4 &view,
              const glm::mat4 &proj,
              const LightSet &lights) override;
    void drawDepth(GLuint depthShader) override;

    
    AssimpModel *getMesh() const { return mesh; }

private:
    AssimpModel *mesh; 
};

#endif