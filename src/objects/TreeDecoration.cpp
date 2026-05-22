#include "TreeDecoration.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

TreeDecoration::TreeDecoration() : VAO(0), VBO(0), EBO(0), indexCount(0) {}

TreeDecoration::~TreeDecoration(){
    if (VAO != 0){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void TreeDecoration::addInstance(const glm::mat4 &transform){
    instanceTransforms.push_back(transform);
}

void TreeDecoration::build(){
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const int segments = 12;
    const float trunkRadius = 0.2f;
    const float trunkHeight = 1.0f;
    const float leafRadius = 0.8f;
    const float leafHeight = 2.0f;

    // 1. Generate Trunk (Cylinder)
    unsigned int trunkStartIndex = 0;
    for (int i = 0; i <= segments; ++i){
        float theta = (float)i / segments * 2.0f * M_PI;
        float x = std::cos(theta);
        float z = std::sin(theta);

        // Bottom vertex
        vertices.push_back(x * trunkRadius);
        vertices.push_back(0.0f);
        vertices.push_back(z * trunkRadius);
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z); // Normal
        vertices.push_back((float)i / segments);
        vertices.push_back(0.0f); // UV

        // Top vertex
        vertices.push_back(x * trunkRadius);
        vertices.push_back(trunkHeight);
        vertices.push_back(z * trunkRadius);
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z); // Normal
        vertices.push_back((float)i / segments);
        vertices.push_back(1.0f); // UV
    }

    for (int i = 0; i < segments; ++i){
        unsigned int bl = trunkStartIndex + i * 2;
        unsigned int tl = trunkStartIndex + i * 2 + 1;
        unsigned int br = trunkStartIndex + (i + 1) * 2;
        unsigned int tr = trunkStartIndex + (i + 1) * 2 + 1;

        indices.push_back(bl);
        indices.push_back(br);
        indices.push_back(tl);
        indices.push_back(tl);
        indices.push_back(br);
        indices.push_back(tr);
    }

    // 2. Generate Leaves (Cone)
    unsigned int leafStartIndex = vertices.size() / 8;

    // Cone tip
    vertices.push_back(0.0f);
    vertices.push_back(trunkHeight + leafHeight);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);
    vertices.push_back(0.0f); // Normal up
    vertices.push_back(0.5f);
    vertices.push_back(1.0f); // UV

    for (int i = 0; i <= segments; ++i){
        float theta = (float)i / segments * 2.0f * M_PI;
        float x = std::cos(theta);
        float z = std::sin(theta);

        // Calculate cone normal
        glm::vec3 normal = glm::normalize(glm::vec3(x * leafHeight, leafRadius, z * leafHeight));

        // Cone base vertex
        vertices.push_back(x * leafRadius);
        vertices.push_back(trunkHeight);
        vertices.push_back(z * leafRadius);
        vertices.push_back(normal.x);
        vertices.push_back(normal.y);
        vertices.push_back(normal.z);
        vertices.push_back((float)i / segments);
        vertices.push_back(0.0f); // UV
    }

    for (int i = 0; i < segments; ++i){
        indices.push_back(leafStartIndex); // Tip
        indices.push_back(leafStartIndex + 1 + i);
        indices.push_back(leafStartIndex + 1 + i + 1);
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

void TreeDecoration::draw(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights){
    (void)view;
    (void)proj;
    (void)lights;

    if (instanceTransforms.empty())
        return;

    glBindVertexArray(VAO);

    for (const glm::mat4 &modelTransform : instanceTransforms){
        // Shader binding: glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelTransform));
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}