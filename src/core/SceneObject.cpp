#include "SceneObject.h"
#include <iostream>
#include <algorithm>
#include <cfloat>

SceneObject::SceneObject() 
    : position(0.0f)
    , rotation(0.0f)
    , scale(1.0f)
    , color(1.0f)
    , hasTexture(false)
    , texture(nullptr)
{
    // Default to unit cube AABB
    aabb.min = glm::vec3(-0.5f);
    aabb.max = glm::vec3(0.5f);
}

SceneObject::~SceneObject() {
    // Release texture if we have one
    if (texture) {
        releaseTexture();
    }
}

void SceneObject::setPosition(glm::vec3 p) {
    position = p;
}

void SceneObject::setRotation(glm::vec3 r) {
    rotation = r;
}

void SceneObject::setScale(glm::vec3 s) {
    scale = s;
}

void SceneObject::setColor(glm::vec3 c) {
    color = c;
}

glm::vec3 SceneObject::getColor() const {
    return color;
}

glm::mat4 SceneObject::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}

bool SceneObject::loadTexture(const std::string& filePath) {
    // Get ResourceManager instance
    ResourceManager* rm = ResourceManager::getInstance();
    if (!rm) {
        std::cerr << "Error: ResourceManager not available" << std::endl;
        return false;
    }
    
    // Release any existing texture first
    if (texture) {
        releaseTexture();
    }
    
    // Load or retrieve texture from cache
    texture = rm->loadTexture(filePath);
    
    if (texture) {
        hasTexture = true;
        texturePath = filePath;
        std::cout << "Texture assigned to object: " << filePath << std::endl;
        return true;
    } else {
        hasTexture = false;
        texture = nullptr;
        texturePath.clear();
        return false;
    }
}

void SceneObject::releaseTexture() {
    if (texture) {
        ResourceManager* rm = ResourceManager::getInstance();
        if (rm) {
            rm->releaseTexture(texture);
        }
        texture = nullptr;
        hasTexture = false;
        texturePath.clear();
    }
}

AABB SceneObject::getWorldAABB() const {
    AABB worldAABB;
    glm::vec3 corners[8] = {
        glm::vec3(aabb.min.x, aabb.min.y, aabb.min.z),
        glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z),
        glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z),
        glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z),
        glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z),
        glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z),
        glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z),
        glm::vec3(aabb.max.x, aabb.max.y, aabb.max.z)
    };
    
    glm::mat4 model = getModelMatrix();
    worldAABB.min = glm::vec3(FLT_MAX);
    worldAABB.max = glm::vec3(-FLT_MAX);
    
    for (int i = 0; i < 8; i++) {
        glm::vec3 transformed = glm::vec3(model * glm::vec4(corners[i], 1.0f));
        worldAABB.min = glm::min(worldAABB.min, transformed);
        worldAABB.max = glm::max(worldAABB.max, transformed);
    }
    
    return worldAABB;
}

void SceneObject::initAABBFromVertices(const std::vector<glm::vec3>& vertices) {
    if (vertices.empty()) {
        aabb.min = glm::vec3(-0.5f);
        aabb.max = glm::vec3(0.5f);
        return;
    }
    
    aabb.min = glm::vec3(FLT_MAX);
    aabb.max = glm::vec3(-FLT_MAX);
    
    for (const auto& v : vertices) {
        aabb.min.x = std::min(aabb.min.x, v.x);
        aabb.min.y = std::min(aabb.min.y, v.y);
        aabb.min.z = std::min(aabb.min.z, v.z);
        
        aabb.max.x = std::max(aabb.max.x, v.x);
        aabb.max.y = std::max(aabb.max.y, v.y);
        aabb.max.z = std::max(aabb.max.z, v.z);
    }
}

void SceneObject::initAABBFromPrimitive(const glm::vec3& halfExtents) {
    aabb.min = -halfExtents;
    aabb.max = halfExtents;
}

AABB SceneObject::getLocalAABB() const { return aabb; }
Texture* SceneObject::getTexture() const { return texture; }
bool SceneObject::hasTextureEnabled() const { return hasTexture; }

std::vector<AABB> SceneObject::getCollisionAABBs() const
{
    return {getWorldAABB()};
}