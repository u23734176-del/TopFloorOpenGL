#include "Water.h"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "../core/ShaderManager.h"



Water::Water()
    : vao(0), vbo(0), shader(0), vertexCount(0), sizeX(6.0f), sizeZ(6.0f), alpha(0.55f), shininess(128.0f)
{
    setColor(glm::vec3(0.1f, 0.3f, 0.5f)); 
    
    initAABBFromPrimitive(glm::vec3(sizeX, 0.05f, sizeZ));
}

void Water::setSize(float halfX, float halfZ)
{
    sizeX = halfX;
    sizeZ = halfZ;
    initAABBFromPrimitive(glm::vec3(sizeX, 0.05f, sizeZ));
}

void Water::setAlpha(float a) { alpha = a; }
void Water::setShininess(float s) { shininess = s; }

void Water::build()
{
    
    
    shader = ShaderManager::get("water");

    
    
    float v[] = {
        -sizeX, 0.0f, -sizeZ, 0.0f, 1.0f, 0.0f,
        sizeX, 0.0f, -sizeZ, 0.0f, 1.0f, 0.0f,
        sizeX, 0.0f, sizeZ, 0.0f, 1.0f, 0.0f,

        sizeX, 0.0f, sizeZ, 0.0f, 1.0f, 0.0f,
        -sizeX, 0.0f, sizeZ, 0.0f, 1.0f, 0.0f,
        -sizeX, 0.0f, -sizeZ, 0.0f, 1.0f, 0.0f};
    vertexCount = 6;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Water::draw(const glm::mat4 &view,
                 const glm::mat4 &proj,
                 const LightSet &lights)
{
    glUseProgram(shader);

    
    
    
    
    

    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &proj[0][0]);
    glUniform3fv(glGetUniformLocation(shader, "objectColor"), 1, &color[0]);
    glUniform1f(glGetUniformLocation(shader, "alpha"), alpha);
    glUniform1f(glGetUniformLocation(shader, "shininess"), shininess);

    
    
    
    
    GLboolean depthMaskWas;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMaskWas);
    glDepthMask(GL_FALSE);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    glDepthMask(depthMaskWas); 
}

void Water::drawDepth(GLuint depthShaderProgram)
{
    
    
    
    (void)depthShaderProgram;
}