#include "BallPhysics.h"

// ---------------------------------------------------------------------------
// BallPhysics.cpp  (Slice E - Karabo)
// ---------------------------------------------------------------------------

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
    // Horizontal speed only -- a ball settling vertically onto the green is not
    // "at rest" in the gameplay sense until it has stopped rolling.
    float horizSpeedSq = velocity.x * velocity.x + velocity.z * velocity.z;
    return horizSpeedSq < (restingEps * restingEps);
}

void BallPhysics::integrate(float dt)
{
    // --- gravity (vertical) -------------------------------------------------
    // Only accumulate downward velocity when not resting on a surface; when
    // grounded, PhysicsWorld has already cancelled downward motion.
    if (!grounded)
    {
        velocity.y += gravity * dt;
    }

    // --- rolling friction (horizontal) --------------------------------------
    // Exponential damping toward zero. Using expf keeps it stable for any dt
    // (a linear "subtract friction*dt" can overshoot past zero on big frames).
    if (grounded)
    {
        float damp = expf(-friction * dt);
        velocity.x *= damp;
        velocity.z *= damp;

        // Clamp tiny residual horizontal motion to a clean stop.
        if (isAtRest())
        {
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }

    // --- integrate position -------------------------------------------------
    position += velocity * dt;
}

void BallPhysics::reflect(const glm::vec3 &normal, float restitution)
{
    // v' = v - (1 + e)(v . n) n
    float vDotN = velocity.x * normal.x + velocity.y * normal.y + velocity.z * normal.z;

    // Only reflect if moving INTO the surface (vDotN < 0); avoids sticking when
    // the ball is already separating.
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