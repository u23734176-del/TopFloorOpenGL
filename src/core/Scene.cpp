#include "core/Scene.h"
#include <iostream>

Scene::Scene() : skybox(nullptr) {}

Scene::~Scene(){
    opaqueObjects.clear();
    transparentObjects.clear();
    skybox = nullptr;
}

void Scene::addOpaqueObject(SceneObject *obj){
    opaqueObjects.push_back(obj);
}

void Scene::addTransparentObject(SceneObject *obj){
    transparentObjects.push_back(obj);
}

void Scene::setSkybox(SceneObject *skyboxObj){
    skybox = skyboxObj;
}

void Scene::render(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights){
    renderShadowPass();
    renderOpaquePass(view, proj, lights);
    renderSkyboxPass(view, proj, lights);
    renderTransparentPass(view, proj, lights);
}

void Scene::renderShadowPass(){
    // Pass 1: Depth FBO generation from directional light perspective
    // Note: FBO binding and depth shader execution will go here once lighting is integrated
}

void Scene::renderOpaquePass(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights){
    // Pass 2: Opaque geometry
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    for (SceneObject *obj : opaqueObjects)
    {
        obj->draw(view, proj, lights);
    }
}

void Scene::renderSkyboxPass(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights){
    // Pass 3: Skybox rendering
    if (skybox != nullptr)
    {
        // Change depth function so depth test passes when values are equal to depth buffer's content
        glDepthFunc(GL_LEQUAL);
        skybox->draw(view, proj, lights);
        glDepthFunc(GL_LESS); // Restore default
    }
}

void Scene::renderTransparentPass(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights){
    // Pass 4: Transparent geometry (Water, Glass Sphere)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth writing so transparent objects don't occlude geometry behind them
    glDepthMask(GL_FALSE);

    for (SceneObject *obj : transparentObjects)
    {
        obj->draw(view, proj, lights);
    }

    // Restore state
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}