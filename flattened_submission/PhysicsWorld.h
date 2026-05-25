#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "SceneObject.h"
#include "Collision.h"
#include "BallPhysics.h"
#include <vector>
#include <glm/glm.hpp>



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

    
    
    
    void setBall(SceneObject *ballObject, BallPhysics *physics);

    
    
    
    void addCollider(SceneObject *obj, Surface surface = Surface::SOLID);

    
    
    void setGroundLevel(float y);

    
    void setRestitution(float e); 

    
    
    void update(float dt);

    
    
    bool isBallInCup() const;

    
    void clearCup();

    
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
    bool ballInCup;        
    float cupCaptureSpeed; 

    
    BoundingSphere ballSphere() const;
    glm::vec3 faceNormalFor(const AABB &box, const glm::vec3 &point) const;
    Surface surfaceUnderBall() const;
    void resolveCollisions(float dt);
};

#endif