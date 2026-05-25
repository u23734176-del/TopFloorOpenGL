#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "SceneObject.h"
#include "../lighting/LightSet.h"

// ---------------------------------------------------------------------------
// Scene
//
// Changes from original:
//   - Objects are split into opaque/transparent lists at addObject() time
//     instead of iterating everything and checking isTransparent() each frame.
//   - build() skips objects that are already built (AssimpModel guard) so
//     shared meshes used by multiple ModelInstances aren't re-uploaded.
//   - view/projection are set ONCE per draw pass via a scene-level bind,
//     then each object only uploads its own model matrix.
//     (Requires draw() implementations to not redundantly re-bind view/proj
//     when they detect the scene has already done so — see note below.)
//
// Note on view/projection binding:
//   The scene binds view + projection once before iterating objects. Each
//   object's draw() should only upload "model" and per-object uniforms.
//   AssimpModel, GolfHole, StaticMesh, and ModelInstance have been updated
//   to accept a pre-bound flag via a thread-local or just rely on the fact
//   that setting the same uniform value twice is cheap (the driver ignores
//   no-change uploads on most implementations). For now the scene sets them
//   once and objects set them again — this is still far cheaper than the
//   original glGetUniformLocation-every-submesh pattern.
// ---------------------------------------------------------------------------
class Scene
{
public:
    // Registers an object. Transparent objects (isTransparent()==true) go
    // into a separate list so the main pass never has to test the flag.
    void addObject(SceneObject *obj);

    // Calls build() on every object. AssimpModel's idempotent guard means
    // shared meshes (trees, bushes) are only uploaded to VRAM once.
    void build();

    // Draw all opaque objects. Sets view/projection once, then iterates.
    void drawOpaqueObjects(const glm::mat4 &view,
                           const glm::mat4 &projection,
                           const LightSet  &lights);

    // Draw all transparent objects (sorted back-to-front is caller's job).
    void drawTransparentObjects(const glm::mat4 &view,
                                const glm::mat4 &projection,
                                const LightSet  &lights);

    // Shadow depth pass — draws everything with the depth shader.
    void drawDepthAllObjects(GLuint depthShaderProgram);

    int opaqueCount()      const { return (int)opaqueObjects.size(); }
    int transparentCount() const { return (int)transparentObjects.size(); }

private:
    std::vector<SceneObject *> opaqueObjects;
    std::vector<SceneObject *> transparentObjects;
};

#endif