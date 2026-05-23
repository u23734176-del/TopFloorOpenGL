#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <unordered_map>
#include <string>
#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Texture {
    GLuint id;
    int width;
    int height;
    int channels;
    std::string path;
    
    Texture() : id(0), width(0), height(0), channels(0) {}
    Texture(GLuint _id, int _w, int _h, int _c, const std::string& _path) 
        : id(_id), width(_w), height(_h), channels(_c), path(_path) {}
};

class ResourceManager {
private:
    // Singleton instance
    static ResourceManager* instance;
    
    // Texture cache
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
    
    // Optional: Model cache for future expansion
    // std::unordered_map<std::string, std::shared_ptr<Model>> models;
    
    // Optional: Shader cache for future expansion
    // std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
    
    // Private constructor for singleton
    ResourceManager();
    
public:
    // Delete copy constructor and assignment
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    // Destructor
    ~ResourceManager();
    
    // Get singleton instance
    static ResourceManager* getInstance();
    
    // Load texture (returns shared_ptr to allow sharing)
    std::shared_ptr<Texture> loadTexture(const std::string& filePath);
    
    // Get texture if already loaded (returns nullptr if not found)
    std::shared_ptr<Texture> getTexture(const std::string& filePath);
    
    // Unload specific texture
    void unloadTexture(const std::string& filePath);
    
    // Unload all textures
    void unloadAllTextures();
    
    // Get texture count for debugging
    size_t getTextureCount() const { return textures.size(); }
    
    // Check if texture is loaded
    bool isTextureLoaded(const std::string& filePath) const;
    
    // Clean up orphaned textures (optional)
    void cleanupUnusedTextures();
};

#endif