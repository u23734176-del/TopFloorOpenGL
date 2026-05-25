#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <GL/glew.h>
#include <string>
#include <unordered_map>

class ShaderManager
{
public:
    
    static void load(const std::string &name,
                     const std::string &vertPath,
                     const std::string &fragPath);

    
    static GLuint get(const std::string &name);

    
    
    static GLint getUniformLocation(GLuint program, const std::string &uniformName);

    
    static void invalidateCache(GLuint program);

    
    
    static void setMat4(GLuint prog, const std::string &name, const float *value);
    static void setVec3(GLuint prog, const std::string &name, float x, float y, float z);
    static void setVec3v(GLuint prog, const std::string &name, const float *value);
    static void setFloat(GLuint prog, const std::string &name, float value);
    static void setInt(GLuint prog, const std::string &name, int value);

private:
    
    static std::unordered_map<std::string, GLuint> programs;

    
    
    static std::unordered_map<std::string, GLint> locationCache;

    static std::string cacheKey(GLuint program, const std::string &uniformName);
};

#endif