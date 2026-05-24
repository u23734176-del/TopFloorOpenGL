#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "../lighting/LightSet.h"
#include "../physics/Collision.h"
#include "ResourceManager.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class SceneObject
{
protected:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 color;
    AABB aabb;
    
    // Texture handling (raw pointer with manual ref counting)
    Texture* texture;
    bool hasTexture;
    std::string texturePath;

public:
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
    
    // Texture loading (uses ResourceManager)
    bool loadTexture(const std::string& filePath);
    
    // Release texture
    void releaseTexture();
    
    // Get texture for binding
    Texture* getTexture() const;
    bool hasTextureEnabled() const;
    
    // For AABB
    AABB getLocalAABB() const;
    AABB getWorldAABB() const;

    // For complex objects to return granular hitboxes (like GolfHole walls)
    virtual std::vector<AABB> getCollisionAABBs() const;

protected:
    // Helper for initializing AABB
    void initAABBFromVertices(const std::vector<glm::vec3>& vertices);
    void initAABBFromPrimitive(const glm::vec3& halfExtents);
};

#endif