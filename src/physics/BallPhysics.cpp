#include "BallPhysics.h"





BallPhysics::BallPhysics()
    : position(0.0f), velocity(0.0f), radius(0.25f), friction(SurfaceFriction::TURF), restingEps(0.05f), grounded(false), gravity(-9.81f)
{
}

void BallPhysics::setPosition(const glm::vec3 &p) { position = p; }
glm::vec3 BallPhysics::getPosition() const { return position; }

void BallPhysics::setVelocity(const glm::vec3 &v) { velocity = v; }
glm::vec3 BallPhysics::getVelocity() const { return velocity; }

void BallPhysics::setRadius(float r) { radius = r; }
float BallPhysics::getRadius() const { return radius; }

void BallPhysics::putt(const glm::vec3 &impulse)
{
    velocity += impulse;
}

void BallPhysics::setFriction(float coeff) { friction = coeff; }
float BallPhysics::getFriction() const { return friction; }

void BallPhysics::setGrounded(bool g) { grounded = g; }
bool BallPhysics::isGrounded() const { return grounded; }

bool BallPhysics::isAtRest() const
{
    
    
    float horizSpeedSq = velocity.x * velocity.x + velocity.z * velocity.z;
    return horizSpeedSq < (restingEps * restingEps);
}

void BallPhysics::integrate(float dt)
{
    
    
    
    if (!grounded)
    {
        velocity.y += gravity * dt;
    }

    
    
    
    if (grounded)
    {
        float damp = expf(-friction * dt);
        velocity.x *= damp;
        velocity.z *= damp;

        
        if (isAtRest())
        {
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }

    
    position += velocity * dt;
}

void BallPhysics::reflect(const glm::vec3 &normal, float restitution)
{
    
    float vDotN = velocity.x * normal.x + velocity.y * normal.y + velocity.z * normal.z;

    
    
    if (vDotN < 0.0f)
    {
        glm::vec3 reflected;
        float factor = (1.0f + restitution) * vDotN;
        reflected.x = velocity.x - factor * normal.x;
        reflected.y = velocity.y - factor * normal.y;
        reflected.z = velocity.z - factor * normal.z;
        velocity = reflected;
    }
}