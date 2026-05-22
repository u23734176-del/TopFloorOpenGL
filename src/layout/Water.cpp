#include "layout/Water.h"

Water::Water() : VAO(0), VBO(0) {}

Water::~Water(){
    if (VAO != 0){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void Water::build(){
    // Basic quad covering a wide area at y = 0.0f
    float vertices[] = {
        // Positions          // Normals           // UVs
        -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 50.0f,
        50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 50.0f, 50.0f,
        50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 50.0f, 0.0f,

        50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 50.0f, 0.0f,
        -50.0f, 0.0f, 50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 50.0f};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // UV attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Water::draw(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights){
    (void)view;
    (void)proj;
    (void)lights; // Prevent unused parameter warnings pending shader integration

    // Shader binding, uniform setting (view, proj, model, light data) will occur here

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}