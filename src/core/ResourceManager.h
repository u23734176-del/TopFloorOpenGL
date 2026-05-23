#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <unordered_map>
#include <string>
#include <GL/glew.h>

struct Texture {
    GLuint id;
    int width;
    int height;
    int channels;
    std::string path;
    int refCount;  // Manual reference counting
    
    Texture() : id(0), width(0), height(0), channels(0), refCount(0) {}
    Texture(GLuint _id, int _w, int _h, int _c, const std::string& _path) 
        : id(_id), width(_w), height(_h), channels(_c), path(_path), refCount(1) {}
};

class ResourceManager {
private:
    // Singleton instance
    static ResourceManager* instance;
    
    // Texture cache
    std::unordered_map<std::string, Texture*> textures;
    
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
    
    // Load texture (returns pointer, increments ref count if already loaded)
    Texture* loadTexture(const std::string& filePath);
    
    // Release texture (decrements ref count, deletes if zero)
    void releaseTexture(const std::string& filePath);
    void releaseTexture(Texture* texture);
    
    // Get texture if already loaded (returns nullptr if not found)
    Texture* getTexture(const std::string& filePath);
    
    // Unload all textures (force delete all)
    void unloadAllTextures();
    
    // Get texture count for debugging
    size_t getTextureCount() const { return textures.size(); }
    
    // Check if texture is loaded
    bool isTextureLoaded(const std::string& filePath) const;
    
    // Clean up unused textures (refCount == 0)
    void cleanupUnusedTextures();
};

#endif