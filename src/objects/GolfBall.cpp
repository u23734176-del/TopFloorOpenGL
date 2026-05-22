#include "objects/GolfBall.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GolfBall::GolfBall() : VAO(0), VBO(0), EBO(0), indexCount(0),
                       position(0.0f, 1.0f, 0.0f), velocity(0.0f), acceleration(0.0f),
                       radius(0.021f), mass(0.045f), restitution(0.6f) {}

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

    // Shader binding will occur here.
    // Construct model matrix from position state:
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void GolfBall::update(float dt){
    // Apply gravity
    applyForce(glm::vec3(0.0f, -9.81f * mass, 0.0f));

    // Euler Integration
    velocity += acceleration * dt;
    position += velocity * dt;

    // Reset acceleration for next frame
    acceleration = glm::vec3(0.0f);
}

void GolfBall::applyForce(const glm::vec3 &force){
    acceleration += force / mass;
}

void GolfBall::resolveCollision(const AABB &obstacle){
    // Find closest point on AABB to sphere center
    float closestX = std::max(obstacle.min.x, std::min(position.x, obstacle.max.x));
    float closestY = std::max(obstacle.min.y, std::min(position.y, obstacle.max.y));
    float closestZ = std::max(obstacle.min.z, std::min(position.z, obstacle.max.z));

    glm::vec3 closestPoint(closestX, closestY, closestZ);
    glm::vec3 difference = position - closestPoint;
    float distance = glm::length(difference);

    if (distance < radius){
        // Collision occurred. Calculate reflection normal
        glm::vec3 normal(0.0f, 1.0f, 0.0f); // Default up
        if (distance > 0.0001f){
            normal = glm::normalize(difference);
        }

        // Penetration resolution: move ball out of obstacle
        position = closestPoint + normal * radius;

        // Velocity reflection with restitution (bounce)
        velocity = glm::reflect(velocity, normal) * restitution;
    }
}

void GolfBall::resolveGroundCollision(float groundHeight, float surfaceFriction){
    if (position.y - radius < groundHeight){
        position.y = groundHeight + radius;

        // Reflect Y velocity
        if (velocity.y < 0){
            velocity.y = -velocity.y * restitution;
        }

        // Apply surface friction to X and Z components
        velocity.x *= surfaceFriction;
        velocity.z *= surfaceFriction;

        // Stop completely if moving very slowly
        if (glm::length(glm::vec2(velocity.x, velocity.z)) < 0.05f){
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }
}

glm::vec3 GolfBall::getPosition() const{
    return position;
}

BoundingSphere GolfBall::getBoundingSphere() const{
    return {position, radius};
}