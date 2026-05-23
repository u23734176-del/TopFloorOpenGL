#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

// ---------------------------------------------------------------------------
// PhysicsWorld.h  (Slice E - Karabo)
//
// Self-contained physics driver. The render loop calls update(dt) once a frame
// after building the scene. It owns NO rendering and edits NO shared class:
// colliders are registered as plain SceneObject* and queried through the
// getWorldAABB() that already exists on the frozen SceneObject interface.
//
// Integration shape (main.cpp), three added lines, no Scene change:
//     PhysicsWorld world;
//     world.setBall(&sphereBall, &ballPhysics);
//     world.addCollider(&leftWall); world.addCollider(&backWall); ...
//     // in the loop:
//     world.update(deltaTime);
// ---------------------------------------------------------------------------

#include "../core/SceneObject.h"
#include "Collision.h"
#include "BallPhysics.h"
#include <vector>
#include <glm/glm.hpp>

// A surface tag lets the world apply per-surface friction when the ball is
// rolling over a collider. Defaults to TURF if a collider is added untagged.
enum class Surface
{
    TURF,
    CONCRETE,
    SAND,
    DIRT,
    SOLID,
    CUP
};

class PhysicsWorld
{
public:
    PhysicsWorld();

    // Register the playable ball. The SceneObject is what gets drawn; the
    // BallPhysics is its motion state. The world keeps both in sync each step
    // (writes the simulated position back onto the SceneObject).
    void setBall(SceneObject *ballObject, BallPhysics *physics);

    // Register a static collider (wall, rock, raised border, ...). Surface tag
    // controls friction when the ball rolls on top of it / SOLID means a wall
    // the ball bounces off.
    void addCollider(SceneObject *obj, Surface surface = Surface::SOLID);

    // Register the ground plane the ball rolls on (its top Y supports the ball).
    // Optional convenience so the ball doesn't fall through the world.
    void setGroundLevel(float y);

    // Tunables.
    void setRestitution(float e); // wall bounciness 0..1

    // Step the simulation: integrate the ball, resolve collisions, write the
    // resulting position back to the ball SceneObject.
    void update(float dt);

    // True once the ball has dropped into a cup-tagged collider. The game/HUD
    // layer reads this to register the hole as sunk.
    bool isBallInCup() const;

    // Reset cup state (e.g. when starting the next hole).
    void clearCup();

    // Expose the ball physics for the HUD / input layer to read or putt.
    BallPhysics *getBallPhysics() const;

private:
    struct Collider
    {
        SceneObject *obj;
        Surface surface;
    };

    SceneObject *ballObject;
    BallPhysics *ballPhysics;
    std::vector<Collider> colliders;
    float groundLevel;
    bool hasGround;
    float restitution;
    bool ballInCup;        // latched once the ball drops
    float cupCaptureSpeed; // max horiz speed that still drops

    // Helpers.
    BoundingSphere ballSphere() const;
    glm::vec3 faceNormalFor(const AABB &box, const glm::vec3 &point) const;
    Surface surfaceUnderBall() const;
    void resolveCollisions(float dt);
};

#endif