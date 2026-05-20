#ifndef COLLISION_H
#define COLLISION_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>

struct AABB { glm::vec3 min, max; };

struct BoundingSphere { 
    glm::vec3 centre;
    float radius;
};

bool intersects(const AABB&, const AABB&);
bool intersects(const BoundingSphere&, const AABB&);

#endif