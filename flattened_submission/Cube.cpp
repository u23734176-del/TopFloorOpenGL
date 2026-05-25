#include "Cube.h"
#include "ShaderManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

float vertices[] = {

    
    -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
     1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,

     1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,

    
    -1.0f, -1.0f, -1.0f,   0.0f, 0.0f,-1.0f,
    -1.0f,  1.0f, -1.0f,   0.0f, 0.0f,-1.0f,
     1.0f,  1.0f, -1.0f,   0.0f, 0.0f,-1.0f,

     1.0f,  1.0f, -1.0f,   0.0f, 0.0f,-1.0f,
     1.0f, -1.0f, -1.0f,   0.0f, 0.0f,-1.0f,
    -1.0f, -1.0f, -1.0f,   0.0f, 0.0f,-1.0f,

    
    -1.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,  -1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,  -1.0f, 0.0f, 0.0f,

    -1.0f, -1.0f, -1.0f,  -1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,

    
     1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,
     1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,

     1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f,
     1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 0.0f,

    
    -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,

     1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,

    
    -1.0f, -1.0f, -1.0f,   0.0f,-1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   0.0f,-1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,   0.0f,-1.0f, 0.0f,

     1.0f, -1.0f,  1.0f,   0.0f,-1.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,   0.0f,-1.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,   0.0f,-1.0f, 0.0f
};

void Cube::build()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    initAABBFromPrimitive(glm::vec3(1.0f)); 
}

void Cube::draw(
    const glm::mat4& view,
    const glm::mat4& proj,
    const LightSet& lights
)
{
    GLuint shaderProgram = ShaderManager::get("basic");
    glUseProgram(shaderProgram);

    glm::mat4 model = getModelMatrix();

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc  = glGetUniformLocation(shaderProgram, "view");
    GLuint projLoc  = glGetUniformLocation(shaderProgram, "projection");
    GLuint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj[0][0]);

    glUniform3f(colorLoc, color.x, color.y, color.z);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Cube::drawDepth(GLuint depthShaderProgram)
{
    glUseProgram(depthShaderProgram);

    glm::mat4 model = getModelMatrix();
    GLuint modelLoc = glGetUniformLocation(depthShaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}