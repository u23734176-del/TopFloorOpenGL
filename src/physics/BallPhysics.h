#ifndef BALLPHYSICS_H
#define BALLPHYSICS_H

// ---------------------------------------------------------------------------
// BallPhysics.h  (Slice E - Karabo)
//
// Holds the dynamic state of the playable golf ball (position, velocity) and
// integrates it forward in time with gravity + rolling friction. Collision
// RESPONSE lives in PhysicsWorld, which has access to the registered colliders;
// this class is just the ball's own motion model.
//
// Includes are limited to the spec's allowed list: <cmath> and glm only.
// ---------------------------------------------------------------------------

#include <glm/glm.hpp>
#include <cmath>

// Per-surface friction. The ball picks up whichever surface it is currently
// rolling on; PhysicsWorld sets this when it detects the ball over a sand zone,
// turf, etc. Values are "fraction of horizontal speed retained per second"
// expressed as a damping coefficient (higher = more drag).
struct SurfaceFriction
{
    static constexpr float TURF = 1.2f;     // smooth green, rolls far
    static constexpr float CONCRETE = 0.8f; // path, rolls furthest
    static constexpr float SAND = 6.0f;     // bunker, kills speed fast
    static constexpr float DIRT = 3.0f;     // rough
};

class BallPhysics
{
public:
    BallPhysics();

    // --- state access -------------------------------------------------------
    void setPosition(const glm::vec3 &p);
    glm::vec3 getPosition() const;

    void setVelocity(const glm::vec3 &v);
    glm::vec3 getVelocity() const;

    void setRadius(float r);
    float getRadius() const;

    // Apply an instantaneous "putt" impulse (adds to current velocity).
    void putt(const glm::vec3 &impulse);

    // Currently-active ground friction coefficient (set by PhysicsWorld).
    void setFriction(float coeff);
    float getFriction() const;

    // True once the ball has effectively stopped (speed below a small epsilon).
    bool isAtRest() const;

    // --- integration --------------------------------------------------------
    // Advance position by velocity, apply gravity (vertical) and rolling
    // friction (horizontal). Does NOT do collision -- PhysicsWorld calls this
    // and then resolves contacts.
    void integrate(float dt);

    // Reflect velocity about a surface normal with restitution (a bounce).
    void reflect(const glm::vec3 &normal, float restitution);

private:
    glm::vec3 position;
    glm::vec3 velocity;
    float radius;
    float friction;   // active ground friction coefficient
    float restingEps; // speed below which we clamp to rest
    bool grounded;    // resting on a surface this step
    float gravity;    // m/s^2, negative = downward

public:
    // PhysicsWorld toggles this when the ball is supported from below.
    void setGrounded(bool g);
    bool isGrounded() const;
};

#endif