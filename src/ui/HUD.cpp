#include "HUD.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../core/shader.hpp"

HUD::HUD()
{
    VAO = 0;
    VBO = 0;
    shaderProgram = 0;
}

void HUD::build()
{
    float crosshairVertices[] =
    {
        // Horizontal line
        -0.02f,  0.0f,
        0.02f,  0.0f,

        // Vertical line
        0.0f, -0.02f,
        0.0f,  0.02f
    };

    // Create VAO + VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(crosshairVertices),
        crosshairVertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Load shaders
    shaderProgram = LoadShaders(
        "shaders/hud.vert",
        "shaders/hud.frag"
    );
}

void HUD::draw(Camera& camera, int width, int height, int mode)
{
    // HUD should render on top
    glDisable(GL_DEPTH_TEST);

    // Use HUD shader
    glUseProgram(shaderProgram);

    // Draw crosshair
    glBindVertexArray(VAO);

    glDrawArrays(GL_LINES, 0, 4);

    glBindVertexArray(0);

    glUseProgram(0);

    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);
}