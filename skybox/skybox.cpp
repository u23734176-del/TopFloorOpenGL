#include "skybox.h"
 
#include "../src/core/ShaderManager.h"
 
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
 
// stb_image symbols are compiled in stb_image.cpp.
extern "C" {
    extern unsigned char* stbi_load(const char* filename, int* x, int* y,
                                    int* comp, int req_comp);
    extern void           stbi_image_free(void* retval_from_stbi_load);
    extern void           stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);
}
 
static std::vector<unsigned char> resizeToSquareRgba(const unsigned char* source,
                                                     int width,
                                                     int height,
                                                     int size)
{
    std::vector<unsigned char> result(size * size * 4);
 
    for (int y = 0; y < size; ++y)
    {
        float sourceY = (y + 0.5f) * height / size - 0.5f;
        sourceY = std::max(0.0f, std::min(sourceY, (float)height - 1.0f));
        
        int y0 = (int)sourceY;
        int y1 = std::min(y0 + 1, height - 1);
        float fy = sourceY - y0;
 
        for (int x = 0; x < size; ++x)
        {
            float sourceX = (x + 0.5f) * width / size - 0.5f;
            sourceX = std::max(0.0f, std::min(sourceX, (float)width - 1.0f));
            
            int x0 = (int)sourceX;
            int x1 = std::min(x0 + 1, width - 1);
            float fx = sourceX - x0;
 
            unsigned char* dst = &result[(y * size + x) * 4];
 
            // Bilinear interpolation for smoother edge transitions
            for (int c = 0; c < 4; ++c)
            {
                float p00 = source[(y0 * width + x0) * 4 + c];
                float p10 = source[(y0 * width + x1) * 4 + c];
                float p01 = source[(y1 * width + x0) * 4 + c];
                float p11 = source[(y1 * width + x1) * 4 + c];
 
                float p0 = p00 * (1.0f - fx) + p10 * fx;
                float p1 = p01 * (1.0f - fx) + p11 * fx;
                float p = p0 * (1.0f - fy) + p1 * fy;
 
                dst[c] = (unsigned char)(p + 0.5f);
            }
        }
    }
 
    return result;
}
 
// Inward-facing cube, 36 vertices, position only (location 0).
static const float skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
 
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
 
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
 
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
 
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
 
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};
 
Skybox::Skybox()
    : vao(0), vbo(0), dayCubemap(0), nightCubemap(0), shaderProgram(0)
{
}
 
Skybox::~Skybox()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    if (dayCubemap)   glDeleteTextures(1, &dayCubemap);
    if (nightCubemap) glDeleteTextures(1, &nightCubemap);
}
 
bool Skybox::build(const std::vector<std::string>& dayFaces,
                   const std::vector<std::string>& nightFaces)
{
    if (dayFaces.size() != 6 || nightFaces.size() != 6)
    {
        std::cerr << "Skybox::build needs exactly 6 day faces and 6 night faces.\n";
        return false;
    }
 
    ShaderManager::load("skybox", "shaders/skybox.vert", "shaders/skybox.frag");
    shaderProgram = ShaderManager::get("skybox");
    if (shaderProgram == 0)
    {
        std::cerr << "Skybox::build failed to load skybox shader program.\n";
        return false;
    }
 
#ifdef GL_TEXTURE_CUBE_MAP_SEAMLESS
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#endif
 
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
 
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
 
    glBindVertexArray(0);
 
    dayCubemap = loadCubemap(dayFaces);
    nightCubemap = loadCubemap(nightFaces);
 
    if (!dayCubemap || !nightCubemap)
    {
        std::cerr << "Skybox::build: one or more cubemaps failed to load.\n";
        return false;
    }
 
    return true;
}
 
void Skybox::draw(const glm::mat4& view, const glm::mat4& projection, bool isNight)
{
    if (shaderProgram == 0 || vao == 0 || dayCubemap == 0 || nightCubemap == 0)
        return;
 
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
 
    glDisable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
 
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),
                       1, GL_FALSE, glm::value_ptr(viewNoTranslation));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
                       1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 0);
 
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
 
    glBindTexture(GL_TEXTURE_CUBE_MAP, isNight ? nightCubemap : dayCubemap);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
 
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
}
 
GLuint Skybox::loadCubemap(const std::vector<std::string>& faces)
{
    if (faces.size() != 6)
        return 0;
 
    const int cubeFaceSize = 512;
 
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    stbi_set_flip_vertically_on_load(false);
 
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); ++i)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 4);
        if (data)
        {
            std::vector<unsigned char> squareFace =
                resizeToSquareRgba(data, width, height, cubeFaceSize);
 
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         GL_RGBA,
                         cubeFaceSize,
                         cubeFaceSize,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         &squareFace[0]);
 
            stbi_image_free(data);
        }
        else
        {
            std::cerr << "Skybox: failed to load face [" << i << "]: " << faces[i] << "\n";
            glDeleteTextures(1, &textureID);
            return 0;
        }
    }
 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
 
    // Generate mipmaps for better filtering
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
 
    // Apply anisotropic filtering if available (helps with edge seams)
    GLfloat maxAniso = 1.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);
 
    return textureID;
}