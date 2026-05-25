#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "SceneObject.h"
#include "../lighting/LightSet.h"
class Scene
{
public:
    
    
    void addObject(SceneObject *obj);

    
    
    void build();

    
    void drawOpaqueObjects(const glm::mat4 &view,
                           const glm::mat4 &projection,
                           const LightSet  &lights);

    
    void drawTransparentObjects(const glm::mat4 &view,
                                const glm::mat4 &projection,
                                const LightSet  &lights);

    
    void drawDepthAllObjects(GLuint depthShaderProgram);

    int opaqueCount()      const { return (int)opaqueObjects.size(); }
    int transparentCount() const { return (int)transparentObjects.size(); }

private:
    std::vector<SceneObject *> opaqueObjects;
    std::vector<SceneObject *> transparentObjects;
};

#endif