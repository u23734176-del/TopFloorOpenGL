#include "ShaderManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Static member definitions
std::unordered_map<std::string, GLuint> ShaderManager::programs;
std::unordered_map<std::string, GLint> ShaderManager::locationCache;

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------
static GLuint compileShader(GLenum type, const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "ShaderManager: cannot open " << path << "\n";
        return 0;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    std::string src = ss.str();
    const char *cstr = src.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &cstr, nullptr);
    glCompileShader(shader);

    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "ShaderManager: compile error in " << path << "\n"
                  << log << "\n";
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------
void ShaderManager::load(const std::string &name,
                         const std::string &vertPath,
                         const std::string &fragPath)
{
    GLuint vert = compileShader(GL_VERTEX_SHADER, vertPath);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragPath);
    if (!vert || !frag)
        return;

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetProgramInfoLog(prog, 512, nullptr, log);
        std::cerr << "ShaderManager: link error for '" << name << "'\n"
                  << log << "\n";
        glDeleteProgram(prog);
    }
    else
    {
        // If we're replacing an existing program, flush its cache entries
        auto it = programs.find(name);
        if (it != programs.end())
            invalidateCache(it->second);

        programs[name] = prog;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

GLuint ShaderManager::get(const std::string &name)
{
    auto it = programs.find(name);
    return (it != programs.end()) ? it->second : 0;
}

std::string ShaderManager::cacheKey(GLuint program, const std::string &uniformName)
{
    // Simple string key — built once per unique (program, name) pair.
    // The cost of this lookup is far lower than a driver round-trip.
    return std::to_string(program) + ":" + uniformName;
}

GLint ShaderManager::getUniformLocation(GLuint program, const std::string &uniformName)
{
    const std::string key = cacheKey(program, uniformName);
    auto it = locationCache.find(key);
    if (it != locationCache.end())
        return it->second;

    // Cache miss — ask the driver once, then store
    GLint loc = glGetUniformLocation(program, uniformName.c_str());
    locationCache[key] = loc;
    return loc;
}

void ShaderManager::invalidateCache(GLuint program)
{
    // Erase all entries whose key starts with this program's ID prefix
    const std::string prefix = std::to_string(program) + ":";
    for (auto it = locationCache.begin(); it != locationCache.end();)
    {
        if (it->first.rfind(prefix, 0) == 0)
            it = locationCache.erase(it);
        else
            ++it;
    }
}

// ---------------------------------------------------------------------------
// Convenience setters — these are what draw() functions should call
// ---------------------------------------------------------------------------
void ShaderManager::setMat4(GLuint prog, const std::string &name, const float *value)
{
    GLint loc = getUniformLocation(prog, name);
    if (loc != -1)
        glUniformMatrix4fv(loc, 1, GL_FALSE, value);
}

void ShaderManager::setVec3(GLuint prog, const std::string &name, float x, float y, float z)
{
    GLint loc = getUniformLocation(prog, name);
    if (loc != -1)
        glUniform3f(loc, x, y, z);
}

void ShaderManager::setVec3v(GLuint prog, const std::string &name, const float *value)
{
    GLint loc = getUniformLocation(prog, name);
    if (loc != -1)
        glUniform3fv(loc, 1, value);
}

void ShaderManager::setFloat(GLuint prog, const std::string &name, float value)
{
    GLint loc = getUniformLocation(prog, name);
    if (loc != -1)
        glUniform1f(loc, value);
}

void ShaderManager::setInt(GLuint prog, const std::string &name, int value)
{
    GLint loc = getUniformLocation(prog, name);
    if (loc != -1)
        glUniform1i(loc, value);
}