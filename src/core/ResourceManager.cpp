#include "ResourceManager.h"
#include "stb_image.h"
#include <iostream>
#include <algorithm>

// Initialize static member
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

std::shared_ptr<Texture> ResourceManager::loadTexture(const std::string& filePath) {
    // Check if already loaded
    auto it = textures.find(filePath);
    if (it != textures.end()) {
        std::cout << "Texture already loaded (sharing): " << filePath << std::endl;
        return it->second;
    }
    
    // Check OpenGL context
    if (!glGetString(GL_VERSION)) {
        std::cerr << "Error: No OpenGL context active when loading texture: " << filePath << std::endl;
        return nullptr;
    }
    
    // Check if file exists
    FILE* file = fopen(filePath.c_str(), "r");
    if (!file) {
        std::cerr << "Error: Texture file not found: " << filePath << std::endl;
        return nullptr;
    }
    fclose(file);
    
    int width, height, channels;
    
    // Flip vertically since OpenGL expects 0,0 at bottom-left
    stbi_set_flip_vertically_on_load(true);
    
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        std::cerr << "Error: Failed to load texture: " << filePath << std::endl;
        std::cerr << "STB Image error: " << stbi_failure_reason() << std::endl;
        return nullptr;
    }
    
    // Determine OpenGL format
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
    
    // Generate OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Free stb image data
    stbi_image_free(data);
    
    // Create texture object
    auto texture = std::make_shared<Texture>(textureID, width, height, channels, filePath);
    
    // Store in cache
    textures[filePath] = texture;
    
    std::cout << "Texture loaded successfully: " << filePath 
              << " (" << width << "x" << height << ", " << channels << " channels)" << std::endl;
    
    return texture;
}

std::shared_ptr<Texture> ResourceManager::getTexture(const std::string& filePath) {
    auto it = textures.find(filePath);
    if (it != textures.end()) {
        return it->second;
    }
    return nullptr;
}

void ResourceManager::unloadTexture(const std::string& filePath) {
    auto it = textures.find(filePath);
    if (it != textures.end()) {
        // Check if texture is still referenced elsewhere
        if (it->second.use_count() == 1) {
            glDeleteTextures(1, &it->second->id);
            std::cout << "Texture unloaded: " << filePath << std::endl;
        } else {
            std::cout << "Texture still in use, not deleting: " << filePath 
                      << " (references: " << it->second.use_count() << ")" << std::endl;
        }
        textures.erase(it);
    }
}

void ResourceManager::unloadAllTextures() {
    for (auto& pair : textures) {
        if (pair.second && pair.second->id != 0) {
            glDeleteTextures(1, &pair.second->id);
            std::cout << "Texture unloaded: " << pair.first << std::endl;
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
        if (it->second.use_count() == 1) {  // Only this map holds reference
            glDeleteTextures(1, &it->second->id);
            std::cout << "Cleaned up unused texture: " << it->first << std::endl;
            it = textures.erase(it);
        } else {
            ++it;
        }
    }
}