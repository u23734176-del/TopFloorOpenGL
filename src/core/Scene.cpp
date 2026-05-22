#include "Scene.h"

void Scene::addObject(SceneObject* object) {
    objects.push_back(object);
}

void Scene::build()
{
    for (SceneObject* object : objects)
    {
        object->build();
    }
}

void Scene::drawAllObjects(const glm::mat4& view,
                           const glm::mat4& projection,
                           const LightSet& lights) {
    for (SceneObject* obj : objects) {
        obj->draw(view, projection, lights);
    }
}