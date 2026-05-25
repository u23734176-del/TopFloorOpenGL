#include "Scene.h"
#include "ShaderManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// ---------------------------------------------------------------------------
// addObject
// ---------------------------------------------------------------------------
void Scene::addObject(SceneObject *obj)
{
    if (!obj)
        return;

    if (obj->isTransparent())
        transparentObjects.push_back(obj);
    else
        opaqueObjects.push_back(obj);
}

// ---------------------------------------------------------------------------
// build
// ---------------------------------------------------------------------------
void Scene::build()
{
    // Iterate both lists. AssimpModel::build() is idempotent so shared meshes
    // referenced by multiple ModelInstances are only uploaded once.
    for (SceneObject *obj : opaqueObjects)
        obj->build();
    for (SceneObject *obj : transparentObjects)
        obj->build();

    std::cout << "Scene built: "
              << opaqueObjects.size() << " opaque, "
              << transparentObjects.size() << " transparent objects.\n";
}

// ---------------------------------------------------------------------------
// drawOpaqueObjects
//
// Key optimisation: bind the "basic" shader and upload view + projection ONCE,
// then let each object upload only its model matrix and per-object uniforms.
// This cuts (N_objects * 2) mat4 uploads down to 2 per frame for the common
// case where every object uses the same shader.
// ---------------------------------------------------------------------------
void Scene::drawOpaqueObjects(const glm::mat4 &view,
                              const glm::mat4 &projection,
                              const LightSet &lights)
{
    if (opaqueObjects.empty())
        return;

    GLuint shader = ShaderManager::get("basic");
    glUseProgram(shader);

    // Upload shared matrices once
    ShaderManager::setMat4(shader, "view", glm::value_ptr(view));
    ShaderManager::setMat4(shader, "projection", glm::value_ptr(projection));

    for (SceneObject *obj : opaqueObjects)
        obj->draw(view, projection, lights);
}

// ---------------------------------------------------------------------------
// drawTransparentObjects
// ---------------------------------------------------------------------------
void Scene::drawTransparentObjects(const glm::mat4 &view,
                                   const glm::mat4 &projection,
                                   const LightSet &lights)
{
    if (transparentObjects.empty())
        return;

    GLuint shader = ShaderManager::get("basic");
    glUseProgram(shader);
    ShaderManager::setMat4(shader, "view", glm::value_ptr(view));
    ShaderManager::setMat4(shader, "projection", glm::value_ptr(projection));

    for (SceneObject *obj : transparentObjects)
        obj->draw(view, projection, lights);
}

// ---------------------------------------------------------------------------
// drawDepthAllObjects
//
// Shadow pass: bind depth shader once, upload lightSpaceMatrix once (done by
// main.cpp before this call), then each object only uploads its model matrix.
// ---------------------------------------------------------------------------
void Scene::drawDepthAllObjects(GLuint depthShader)
{
    glUseProgram(depthShader);

    for (SceneObject *obj : opaqueObjects)
        obj->drawDepth(depthShader);

    // Transparent objects cast shadows too — include them if desired.
    // Comment this out if you want water/glass to not occlude light.
    for (SceneObject *obj : transparentObjects)
        obj->drawDepth(depthShader);
}