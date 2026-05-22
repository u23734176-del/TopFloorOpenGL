#ifndef GOLFBALL_H
#define GOLFBALL_H

#include "core/SceneObject.h"
#include "physics/Collision.h"
#include <vector>

class GolfBall : public SceneObject{
public:
    GolfBall();
    ~GolfBall() override;

    void build() override;
    void draw(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights) override;

    // Physics Engine methods
    void update(float dt);
    void applyForce(const glm::vec3 &force);
    void resolveCollision(const AABB &obstacle);
    void resolveGroundCollision(float groundHeight, float surfaceFriction);

    glm::vec3 getPosition() const;
    BoundingSphere getBoundingSphere() const;

private:
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    unsigned int indexCount;

    // Physics State
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float radius;
    float mass;
    float restitution; // Bounciness
};

#endif