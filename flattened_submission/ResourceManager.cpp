#include "ResourceManager.h"
#include "stb_image.h"
#include <iostream>
#include <cstdio>
#include "AssimpModel.h"


ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager() {
    std::cout << "ResourceManager initialized" << std::endl;
}

ResourceManager::~ResourceManager() {
    unloadAllTextures();
    std::cout << "ResourceManager destroyed" << std::endl;
}

ResourceManager* ResourceManager::getInstance() {
    if (!instance) {
        instance = new ResourceManager();
    }
    return instance;
}

Texture* ResourceManager::loadTexture(const std::string& filePath) {
    
    auto it = textures.find(filePath);
    if (it != textures.end()) {
        it->second->refCount++;
        std::cout << "Texture already loaded (sharing): " << filePath 
                  << " (refCount: " << it->second->refCount << ")" << std::endl;
        return it->second;
    }
    
    
    if (!glGetString(GL_VERSION)) {
        std::cerr << "Error: No OpenGL context active when loading texture: " << filePath << std::endl;
        return nullptr;
    }
    
    
    FILE* file = fopen(filePath.c_str(), "r");
    if (!file) {
        std::cerr << "Error: Texture file not found: " << filePath << std::endl;
        return nullptr;
    }
    fclose(file);
    
    int width, height, channels;
    
    
    stbi_set_flip_vertically_on_load(true);
    
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        std::cerr << "Error: Failed to load texture: " << filePath << std::endl;
        std::cerr << "STB Image error: " << stbi_failure_reason() << std::endl;
        return nullptr;
    }
    
    
    GLenum format;
    switch (channels) {
        case 1: format = GL_RED; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default:
            std::cerr << "Error: Unsupported texture channel count: " << channels << std::endl;
            stbi_image_free(data);
            return nullptr;
    }
    
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
    stbi_image_free(data);
    
    
    Texture* texture = new Texture(textureID, width, height, channels, filePath);
    
    
    textures[filePath] = texture;
    
    std::cout << "Texture loaded successfully: " << filePath 
              << " (" << width << "x" << height << ", " << channels << " channels)" 
              << " (refCount: 1)" << std::endl;
    
    return texture;
}

void ResourceManager::releaseTexture(const std::string& filePath) {
    auto it = textures.find(filePath);
    if (it != textures.end()) {
        it->second->refCount--;
        std::cout << "Texture released: " << filePath 
                  << " (refCount: " << it->second->refCount << ")" << std::endl;
        
        if (it->second->refCount <= 0) {
            glDeleteTextures(1, &it->second->id);
            delete it->second;
            textures.erase(it);
            std::cout << "Texture deleted: " << filePath << std::endl;
        }
    }
}

void ResourceManager::releaseTexture(Texture* texture) {
    if (!texture) return;
    releaseTexture(texture->path);
}

Texture* ResourceManager::getTexture(const std::string& filePath) {
    auto it = textures.find(filePath);
    if (it != textures.end()) {
        return it->second;
    }
    return nullptr;
}

void ResourceManager::unloadAllTextures() {
    for (auto& pair : textures) {
        if (pair.second) {
            glDeleteTextures(1, &pair.second->id);
            std::cout << "Texture unloaded: " << pair.first << std::endl;
            delete pair.second;
        }
    }
    textures.clear();
}

bool ResourceManager::isTextureLoaded(const std::string& filePath) const {
    return textures.find(filePath) != textures.end();
}

void ResourceManager::cleanupUnusedTextures() {
    auto it = textures.begin();
    while (it != textures.end()) {
        if (it->second->refCount <= 0) {
            glDeleteTextures(1, &it->second->id);
            std::cout << "Cleaned up unused texture: " << it->first << std::endl;
            delete it->second;
            it = textures.erase(it);
        } else {
            ++it;
        }
    }
}

AssimpModel *ResourceManager::getOrLoadModel(const std::string &path)
{
    if (models.find(path) == models.end())
    {
        AssimpModel *m = new AssimpModel(path);
        m->build();
        models[path] = m;
    }
    return models[path];
}

void ResourceManager::unloadAllModels()
{
    for (auto &pair : models)
        delete pair.second;
    models.clear();
}



void cleanupResourceManager() {
    delete ResourceManager::getInstance();
}