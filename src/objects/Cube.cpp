#include "Cube.h"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

float vertices[] = {

    // FRONT FACE
    -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
     1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,

     1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 1.0f,

    // BACK FACE
    -1.0f, -1.0f, -1.0f,   0.0f, 0.0f,-1.0f,
    -1.0f,  1.0f, -1.0f,   0.0f, 0.0f,-1.0f,
     1.0f,  1.0f, -1.0f,   0.0f, 0.0f,-1.0f,

     1.0f,  1.0f, -1.0f,   0.0f, 0.0f,-1.0f,
     1.0f, -1.0f, -1.0f,   0.0f, 0.0f,-1.0f,
    -1.0f, -1.0f, -1.0f,   0.0f, 0.0f,-1.0f,

    // LEFT FACE
    -1.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,  -1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,  -1.0f, 0.0f, 0.0f,

    -1.0f, -1.0f, -1.0f,  -1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,  -1.0f, 0.0f, 0.0f,

    // RIGHT FACE
     1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,
     1.0f,  1.0f, -1.0f,   1.0f, 0.0f, 0.0f,

     1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 0.0f, 0.0f,
     1.0f, -1.0f,  1.0f,   1.0f, 0.0f, 0.0f,

    // TOP FACE
    -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,

     1.0f,  1.0f,  1.0f,   0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,   0.0f, 1.0f, 0.0f,

    // BOTTOM FACE
    -1.0f, -1.0f, -1.0f,   0.0f,-1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   0.0f,-1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,   0.0f,-1.0f, 0.0f,

     1.0f, -1.0f,  1.0f,   0.0f,-1.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,   0.0f,-1.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,   0.0f,-1.0f, 0.0f
};

void Cube::build()
{
    shader = LoadShaders(
        "shaders/basic.vert",
        "shaders/basic.frag"
    );

    // Generate buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // Bind VAO
    glBindVertexArray(vao);

    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    // =========================================
    // Position Attribute
    // =========================================

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // =========================================
    // Normal Attribute
    // =========================================

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Cube::draw(
    const glm::mat4& view,
    const glm::mat4& proj,
    const LightSet& lights
)
{
    glUseProgram(shader);

    // =========================================
    // Model Matrix
    // =========================================

    glm::mat4 model = getModelMatrix();

    // =========================================
    // Uniforms
    // =========================================

    GLuint modelLoc = glGetUniformLocation(shader, "model");
    GLuint viewLoc  = glGetUniformLocation(shader, "view");
    GLuint projLoc  = glGetUniformLocation(shader, "projection");

    glUniformMatrix4fv(
        modelLoc,
        1,
        GL_FALSE,
        &model[0][0]
    );

    glUniformMatrix4fv(
        viewLoc,
        1,
        GL_FALSE,
        &view[0][0]
    );

    glUniformMatrix4fv(
        projLoc,
        1,
        GL_FALSE,
        &proj[0][0]
    );

    // =========================================
    // Draw Cube
    // =========================================

    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
}