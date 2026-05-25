#include "Scene.h"
#include "ShaderManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void Scene::addObject(SceneObject *obj)
{
    if (!obj)
        return;

    if (obj->isTransparent())
        transparentObjects.push_back(obj);
    else
        opaqueObjects.push_back(obj);
}




void Scene::build()
{
    
    
    for (SceneObject *obj : opaqueObjects)
        obj->build();
    for (SceneObject *obj : transparentObjects)
        obj->build();

    std::cout << "Scene built: "
              << opaqueObjects.size() << " opaque, "
              << transparentObjects.size() << " transparent objects.\n";
}


void Scene::drawOpaqueObjects(const glm::mat4 &view,
                              const glm::mat4 &projection,
                              const LightSet &lights)
{
    if (opaqueObjects.empty())
        return;

    GLuint shader = ShaderManager::get("basic");
    glUseProgram(shader);

    
    ShaderManager::setMat4(shader, "view", glm::value_ptr(view));
    ShaderManager::setMat4(shader, "projection", glm::value_ptr(projection));

    for (SceneObject *obj : opaqueObjects)
        obj->draw(view, projection, lights);
}




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
void Scene::drawDepthAllObjects(GLuint depthShader)
{
    glUseProgram(depthShader);

    for (SceneObject *obj : opaqueObjects)
        obj->drawDepth(depthShader);

    
    
    for (SceneObject *obj : transparentObjects)
        obj->drawDepth(depthShader);
}