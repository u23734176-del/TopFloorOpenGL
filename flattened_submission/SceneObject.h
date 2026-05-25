#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "LightSet.h"
#include "Collision.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

class Texture;

class SceneObject
{
protected:
    glm::vec3 position;
    
    glm::vec3 scale;
    glm::vec3 color;
    AABB aabb;
    
    
    Texture* texture;
    bool hasTexture;
    std::string texturePath;

public:
    glm::vec3 rotation;
    SceneObject();
    virtual ~SceneObject();
    
    virtual void build() = 0;

    virtual void draw(
        const glm::mat4& view,
        const glm::mat4& projection,
        const LightSet& lights
    ) = 0;

    virtual void drawDepth(GLuint depthShaderProgram) = 0;

    void setPosition(glm::vec3 p);
    void setRotation(glm::vec3 r);
    void setScale(glm::vec3 s);
    void setColor(glm::vec3 c);
    glm::vec3 getColor() const;
    glm::mat4 getModelMatrix() const;
    
    
    bool loadTexture(const std::string& filePath);
    
    
    void releaseTexture();
    
    
    Texture* getTexture() const;
    bool hasTextureEnabled() const;
    
    
    AABB getLocalAABB() const;
    AABB getWorldAABB() const;

    
    virtual std::vector<AABB> getCollisionAABBs() const;

    virtual bool isTransparent() const { return false; }

protected:
    
    void initAABBFromVertices(const std::vector<glm::vec3>& vertices);
    void initAABBFromPrimitive(const glm::vec3& halfExtents);
};

#endif