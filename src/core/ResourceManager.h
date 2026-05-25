#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <unordered_map>
#include <string>
#include <GL/glew.h>

class AssimpModel;

struct Texture {
    GLuint id;
    int width;
    int height;
    int channels;
    std::string path;
    int refCount;  
    
    Texture() : id(0), width(0), height(0), channels(0), refCount(0) {}
    Texture(GLuint _id, int _w, int _h, int _c, const std::string& _path) 
        : id(_id), width(_w), height(_h), channels(_c), path(_path), refCount(1) {}
};

class ResourceManager {
private:
    
    static ResourceManager* instance;
    
    
    std::unordered_map<std::string, Texture*> textures;
    std::unordered_map<std::string, AssimpModel*> models;

    
    ResourceManager();
    
public:
    
    AssimpModel *getOrLoadModel(const std::string &path);
    void unloadAllModels();
    
    
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    
    
    ~ResourceManager();
    
    
    static ResourceManager* getInstance();
    
    
    Texture* loadTexture(const std::string& filePath);
    
    
    void releaseTexture(const std::string& filePath);
    void releaseTexture(Texture* texture);
    
    
    Texture* getTexture(const std::string& filePath);
    
    
    void unloadAllTextures();
    
    
    size_t getTextureCount() const { return textures.size(); }
    
    
    bool isTextureLoaded(const std::string& filePath) const;
    
    
    void cleanupUnusedTextures();
};

#endif