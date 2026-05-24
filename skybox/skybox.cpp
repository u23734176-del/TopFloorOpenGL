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
 
// static std::vector<unsigned char> resizeToSquareRgba(const unsigned char* source,
//                                                      int width,
//                                                      int height,
//                                                      int size)
// {
//     std::vector<unsigned char> result(size * size * 4);

//     for (int y = 0; y < size; ++y)
//     {
//         // Nearest neighbor Y coordinate
//         int srcY = (y * height) / size;
//         srcY = std::max(0, std::min(srcY, height - 1));

//         for (int x = 0; x < size; ++x)
//         {
//             // Nearest neighbor X coordinate
//             int srcX = (x * width) / size;
//             srcX = std::max(0, std::min(srcX, width - 1));

//             int destIndex = (y * size + x) * 4;
//             int srcIndex = (srcY * width + srcX) * 4;

//             // Copy RGBA channels directly—no fractional blending!
//             result[destIndex + 0] = source[srcIndex + 0];
//             result[destIndex + 1] = source[srcIndex + 1];
//             result[destIndex + 2] = source[srcIndex + 2];
//             result[destIndex + 3] = source[srcIndex + 3];
//         }
//     }

//     return result;
// }

static std::vector<unsigned char> stretchToSquareRgba(const unsigned char* source,
                                                      int width, int height,
                                                      int targetSize)
{
    std::vector<unsigned char> result(targetSize * targetSize * 4);

    for (int y = 0; y < targetSize; ++y)
    {
        float srcYf = (float)y * (height - 1) / (targetSize - 1);
        int y0 = (int)srcYf;
        int y1 = std::min(y0 + 1, height - 1);
        float fy = srcYf - y0;

        for (int x = 0; x < targetSize; ++x)
        {
            float srcXf = (float)x * (width - 1) / (targetSize - 1);
            int x0 = (int)srcXf;
            int x1 = std::min(x0 + 1, width - 1);
            float fx = srcXf - x0;

            int destIndex = (y * targetSize + x) * 4;

            for (int c = 0; c < 4; ++c)
            {
                float tl = source[(y0 * width + x0) * 4 + c];
                float tr = source[(y0 * width + x1) * 4 + c];
                float bl = source[(y1 * width + x0) * 4 + c];
                float br = source[(y1 * width + x1) * 4 + c];
                float val = tl*(1-fx)*(1-fy) + tr*fx*(1-fy)
                          + bl*(1-fx)*fy     + br*fx*fy;
                result[destIndex + c] = (unsigned char)(val + 0.5f);
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
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // Clamp to edge to prevent border sampling
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); ++i)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 4);
        if (data)
        {
            stbi_set_flip_vertically_on_load(false);
            // Crop and scale to exactly 512x512
            std::vector<unsigned char> squareFace = 
                stretchToSquareRgba(data, width, height, cubeFaceSize);

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         GL_RGBA,
                         cubeFaceSize, // GUARANTEED 512
                         cubeFaceSize, // GUARANTEED 512
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
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    // REMOVED: glGenerateMipmap
    // REMOVED: GL_TEXTURE_BASE_LEVEL / MAX_LEVEL
    // REMOVED: Anisotropic filtering block

    return textureID;
}