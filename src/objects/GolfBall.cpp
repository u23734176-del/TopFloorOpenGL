#include "objects/GolfBall.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GolfBall::GolfBall() : VAO(0), VBO(0), EBO(0), indexCount(0) {}

GolfBall::~GolfBall(){
    if (VAO != 0){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void GolfBall::build(){
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 32;
    const unsigned int Y_SEGMENTS = 32;
    const float RADIUS = 0.021f; // Standard golf ball radius in meters

    for (unsigned int x = 0; x <= X_SEGMENTS; ++x){
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y){
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = RADIUS * std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
            float yPos = RADIUS * std::cos(ySegment * M_PI);
            float zPos = RADIUS * std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);

            // Positions
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);

            // Normals (normalized positions for a sphere at origin)
            vertices.push_back(xPos / RADIUS);
            vertices.push_back(yPos / RADIUS);
            vertices.push_back(zPos / RADIUS);

            // UVs
            vertices.push_back(xSegment);
            vertices.push_back(ySegment);
        }
    }

    for (unsigned int y = 0; y < Y_SEGMENTS; ++y){
        for (unsigned int x = 0; x < X_SEGMENTS; ++x){
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x + 1);

            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x + 1);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x + 1);
        }
    }

    indexCount = indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void GolfBall::draw(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights){
    (void)view;
    (void)proj;
    (void)lights;

    // Shader binding, uniform setting (view, proj, model matrix mapping position) will occur here

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}