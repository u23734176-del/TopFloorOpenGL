#include "PhysicsWorld.h"
#include <cmath>

// ---------------------------------------------------------------------------
// PhysicsWorld.cpp  (Slice E - Karabo)
// ---------------------------------------------------------------------------

PhysicsWorld::PhysicsWorld()
    : ballObject(nullptr), ballPhysics(nullptr), groundLevel(0.0f), hasGround(false), restitution(0.6f), ballInCup(false), cupCaptureSpeed(2.5f) // ball must be slower than this to drop, else lips out
{
}

void PhysicsWorld::setBall(SceneObject *obj, BallPhysics *physics)
{
    ballObject = obj;
    ballPhysics = physics;
    if (ballObject && ballPhysics)
    {
        // Seed physics position from wherever the object was placed.
        ballPhysics->setPosition(ballObject->getModelMatrix()[3]);
    }
}

void PhysicsWorld::addCollider(SceneObject *obj, Surface surface)
{
    if (obj)
        colliders.push_back({obj, surface});
}

void PhysicsWorld::setGroundLevel(float y)
{
    groundLevel = y;
    hasGround = true;
}

void PhysicsWorld::setRestitution(float e) { restitution = e; }

BallPhysics *PhysicsWorld::getBallPhysics() const { return ballPhysics; }

bool PhysicsWorld::isBallInCup() const { return ballInCup; }
void PhysicsWorld::clearCup() { ballInCup = false; }

BoundingSphere PhysicsWorld::ballSphere() const
{
    BoundingSphere s;
    s.centre = ballPhysics ? ballPhysics->getPosition() : glm::vec3(0.0f);
    s.radius = ballPhysics ? ballPhysics->getRadius() : 0.25f;
    return s;
}

// Pick the box face whose outward normal best matches the direction from the
// box centre to the contact point. Good enough for axis-aligned mini-golf
// walls and far cheaper than full manifold generation.
glm::vec3 PhysicsWorld::faceNormalFor(const AABB &box, const glm::vec3 &point) const
{
    glm::vec3 centre = (box.min + box.max) * 0.5f;
    glm::vec3 d = point - centre;
    glm::vec3 half = (box.max - box.min) * 0.5f;

    // Normalise each axis distance by the box half-extent, largest wins.
    float nx = (half.x > 0.0f) ? fabsf(d.x) / half.x : 0.0f;
    float ny = (half.y > 0.0f) ? fabsf(d.y) / half.y : 0.0f;
    float nz = (half.z > 0.0f) ? fabsf(d.z) / half.z : 0.0f;

    if (nx >= ny && nx >= nz)
        return glm::vec3((d.x < 0.0f) ? -1.0f : 1.0f, 0.0f, 0.0f);
    if (ny >= nx && ny >= nz)
        return glm::vec3(0.0f, (d.y < 0.0f) ? -1.0f : 1.0f, 0.0f);
    return glm::vec3(0.0f, 0.0f, (d.z < 0.0f) ? -1.0f : 1.0f);
}

// Which surface is the ball currently rolling on? If it sits on top of a
// tagged collider, use that collider's friction; otherwise default to TURF.
Surface PhysicsWorld::surfaceUnderBall() const
{
    if (!ballPhysics)
        return Surface::TURF;

    BoundingSphere s = ballSphere();
    // Probe a thin sphere just below the ball.
    BoundingSphere probe = s;
    probe.centre.y -= s.radius * 0.5f;

    for (const Collider &c : colliders)
    {
        if (c.surface == Surface::SOLID)
            continue; // walls aren't "floor"
        AABB box = c.obj->getWorldAABB();
        if (intersects(probe, box))
        {
            return c.surface;
        }
    }
    return Surface::TURF;
}

static float frictionFor(Surface s)
{
    switch (s)
    {
    case Surface::CONCRETE:
        return SurfaceFriction::CONCRETE;
    case Surface::SAND:
        return SurfaceFriction::SAND;
    case Surface::DIRT:
        return SurfaceFriction::DIRT;
    case Surface::TURF:
    default:
        return SurfaceFriction::TURF;
    }
}

void PhysicsWorld::resolveCollisions(float dt)
{
    if (!ballPhysics)
        return;

    BoundingSphere s = ballSphere();
    bool groundedThisStep = false;

    // --- ground plane -------------------------------------------------------
    if (hasGround)
    {
        float floorY = groundLevel + s.radius;
        if (ballPhysics->getPosition().y <= floorY)
        {
            glm::vec3 p = ballPhysics->getPosition();
            p.y = floorY;
            ballPhysics->setPosition(p);

            glm::vec3 v = ballPhysics->getVelocity();
            if (v.y < 0.0f)
                v.y = 0.0f; // cancel downward motion
            ballPhysics->setVelocity(v);
            groundedThisStep = true;
        }
    }

    // --- static colliders ---------------------------------------------------
    for (const Collider &c : colliders)
    {
        AABB box = c.obj->getWorldAABB();
        s = ballSphere();
        if (!intersects(s, box))
            continue;

        // ---- CUP CAPTURE ---------------------------------------------------
        // A cup-tagged collider sits at the hole. If the ball reaches it slowly
        // enough, it drops and stops; if it is travelling too fast it lips out
        // and is treated as a normal (bouncy) obstacle, just like real golf.
        if (c.surface == Surface::CUP)
        {
            glm::vec3 v = ballPhysics->getVelocity();
            float horizSpeed = sqrtf(v.x * v.x + v.z * v.z);
            if (horizSpeed <= cupCaptureSpeed)
            {
                glm::vec3 cupCentre = (box.min + box.max) * 0.5f;
                cupCentre.y = box.min.y + s.radius; // rest at the bottom of the cup
                ballPhysics->setPosition(cupCentre);
                ballPhysics->setVelocity(glm::vec3(0.0f));
                ballPhysics->setGrounded(true);
                ballInCup = true;
                return; // hole sunk -- nothing else to resolve this step
            }
            // too fast: fall through and let it bounce off the rim like a solid
        }

        glm::vec3 normal = faceNormalFor(box, s.centre);

        // Top-face contact = standing on it (acts like ground). Side faces =
        // wall, bounce the ball off.
        if (normal.y > 0.5f)
        {
            glm::vec3 p = ballPhysics->getPosition();
            p.y = box.max.y + s.radius;
            ballPhysics->setPosition(p);
            glm::vec3 v = ballPhysics->getVelocity();
            if (v.y < 0.0f)
                v.y = 0.0f;
            ballPhysics->setVelocity(v);
            groundedThisStep = true;
        }
        else
        {
            // Push the ball out along the face normal so it no longer overlaps,
            // then reflect its velocity (the bounce).
            glm::vec3 closest;
            closest.x = fmaxf(box.min.x, fminf(s.centre.x, box.max.x));
            closest.y = fmaxf(box.min.y, fminf(s.centre.y, box.max.y));
            closest.z = fmaxf(box.min.z, fminf(s.centre.z, box.max.z));
            glm::vec3 d = s.centre - closest;
            float dist = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
            float pen = s.radius - dist;
            if (pen > 0.0f)
            {
                glm::vec3 p = ballPhysics->getPosition();
                p += normal * pen; // depenetrate
                ballPhysics->setPosition(p);
            }
            ballPhysics->reflect(normal, restitution);
        }
    }

    ballPhysics->setGrounded(groundedThisStep);

    // Apply the surface the ball is resting on (friction for next integrate).
    if (groundedThisStep)
    {
        ballPhysics->setFriction(frictionFor(surfaceUnderBall()));
    }
}

void PhysicsWorld::update(float dt)
{
    if (!ballObject || !ballPhysics)
        return;

    // Guard against huge frames (e.g. after a stall) blowing up the sim.
    if (dt > 0.05f)
        dt = 0.05f;

    ballPhysics->integrate(dt);
    resolveCollisions(dt);

    // Write simulated position back to the drawable object.
    ballObject->setPosition(ballPhysics->getPosition());
}