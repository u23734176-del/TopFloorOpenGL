#ifndef MODELINSTANCE_H
#define MODELINSTANCE_H

#include "../core/SceneObject.h"
#include "../objects/AssimpModel.h"

// ---------------------------------------------------------------------------
// ModelInstance
//
// Problem it solves:
//   ResourceManager::getOrLoadModel() returns the SAME AssimpModel* for every
//   tree/bush/etc. that shares a path. Calling setPosition() on that pointer
//   moves ALL of them simultaneously — they all end up stacked at the last
//   position that was set.
//
// Fix:
//   AssimpModel owns only GPU data (VAOs/VBOs) and the submesh list.
//   ModelInstance is a thin SceneObject wrapper that holds its OWN transform
//   (position/rotation/scale) and delegates all drawing to the shared model.
//
// Usage (replaces the old pattern in Course.cpp):
//   ModelInstance* tree = new ModelInstance(
//       ResourceManager::getInstance()->getOrLoadModel("external/Tree_One.obj")
//   );
//   tree->setPosition(pos);
//   tree->setScale(glm::vec3(scale));
//   scene.addObject(tree);   // Scene owns the instance; ResourceManager owns the mesh
// ---------------------------------------------------------------------------
class ModelInstance : public SceneObject
{
public:
    explicit ModelInstance(AssimpModel *sharedMesh);
    ~ModelInstance() override = default;

    // SceneObject interface
    void build() override; // No-op: geometry lives in the shared AssimpModel
    void draw(const glm::mat4 &view,
              const glm::mat4 &proj,
              const LightSet &lights) override;
    void drawDepth(GLuint depthShader) override;

    // Access the underlying shared mesh (e.g. to call setSubMeshColor)
    AssimpModel *getMesh() const { return mesh; }

private:
    AssimpModel *mesh; // Non-owning — ResourceManager owns this
};

#endif