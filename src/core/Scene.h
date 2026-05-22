#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <GL/glew.h>
#include "core/SceneObject.h"
#include "lighting/LightSet.h"

class Scene{
public:
    Scene();
    ~Scene();

    void addOpaqueObject(SceneObject *obj);
    void addTransparentObject(SceneObject *obj);
    void setSkybox(SceneObject *skyboxObj);

    void render(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights);

private:
    std::vector<SceneObject *> opaqueObjects;
    std::vector<SceneObject *> transparentObjects;
    SceneObject *skybox;

    void renderShadowPass();
    void renderOpaquePass(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights);
    void renderSkyboxPass(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights);
    void renderTransparentPass(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights);
};

#endif