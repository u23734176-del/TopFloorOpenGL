#ifndef BALLPHYSICS_H
#define BALLPHYSICS_H

#include <glm/glm.hpp>
#include <cmath>
struct SurfaceFriction
{
    static constexpr float TURF = 1.2f;     
    static constexpr float CONCRETE = 0.8f; 
    static constexpr float SAND = 6.0f;     
    static constexpr float DIRT = 3.0f;     
};

class BallPhysics
{
public:
    BallPhysics();

    
    void setPosition(const glm::vec3 &p);
    glm::vec3 getPosition() const;

    void setVelocity(const glm::vec3 &v);
    glm::vec3 getVelocity() const;

    void setRadius(float r);
    float getRadius() const;

    
    void putt(const glm::vec3 &impulse);

    
    void setFriction(float coeff);
    float getFriction() const;

    
    bool isAtRest() const;

    
    void integrate(float dt);

    
    void reflect(const glm::vec3 &normal, float restitution);

private:
    glm::vec3 position;
    glm::vec3 velocity;
    float radius;
    float friction;   
    float restingEps; 
    bool grounded;    
    float gravity;    

public:
    
    void setGrounded(bool g);
    bool isGrounded() const;
};

#endif