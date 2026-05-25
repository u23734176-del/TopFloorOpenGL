#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <GL/glew.h>
#include <string>
#include <unordered_map>

// ---------------------------------------------------------------------------
// ShaderManager
//
// Changes from original:
//   - Added uniform location cache (getUniformLocation / invalidateCache)
//   - glGetUniformLocation is now called ONCE per uniform per program,
//     then stored. All draw code should call ShaderManager::uniform()
//     instead of glGetUniformLocation directly.
// ---------------------------------------------------------------------------
class ShaderManager
{
public:
    // Load and compile a shader program, store under 'name'
    static void load(const std::string &name,
                     const std::string &vertPath,
                     const std::string &fragPath);

    // Retrieve a compiled program by name (returns 0 if not found)
    static GLuint get(const std::string &name);

    // Cached glGetUniformLocation. Returns -1 if not found.
    // Key is (programID, uniformName) so different programs don't collide.
    static GLint getUniformLocation(GLuint program, const std::string &uniformName);

    // Call after recompiling a shader to flush its cached locations
    static void invalidateCache(GLuint program);

    // Convenience wrappers — these replace every raw glUniform* call
    // in draw() functions. They look up the location from cache automatically.
    static void setMat4(GLuint prog, const std::string &name, const float *value);
    static void setVec3(GLuint prog, const std::string &name, float x, float y, float z);
    static void setVec3v(GLuint prog, const std::string &name, const float *value);
    static void setFloat(GLuint prog, const std::string &name, float value);
    static void setInt(GLuint prog, const std::string &name, int value);

private:
    // name -> GL program ID
    static std::unordered_map<std::string, GLuint> programs;

    // (programID << 32 | hash(uniformName)) -> GLint location
    // Using a string key "programID:uniformName" for simplicity
    static std::unordered_map<std::string, GLint> locationCache;

    static std::string cacheKey(GLuint program, const std::string &uniformName);
};

#endif