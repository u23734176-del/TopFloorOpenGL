#include "Cube.h"

#include <glm/gtc/matrix_transform.hpp>

float vertices[] = {
    -1,-1,-1,
     1,-1,-1,
     1, 1,-1,

     1, 1,-1,
    -1, 1,-1,
    -1,-1,-1
};

void Cube::build()
{
    shader = LoadShaders(
        "shaders/basic.vert",
        "shaders/basic.frag"
    );

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
        3 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);
}

void Cube::draw(
    const glm::mat4& view,
    const glm::mat4& proj,
    const LightSet& lights
)
{
    glUseProgram(shader);

    glm::mat4 model = glm::mat4(1.0f);

    GLuint modelLoc = glGetUniformLocation(shader, "model");
    GLuint viewLoc = glGetUniformLocation(shader, "view");
    GLuint projLoc = glGetUniformLocation(shader, "projection");

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

    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}