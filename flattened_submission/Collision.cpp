#include "Collision.h"

bool intersects(const AABB &a, const AABB &b)
{
    if (a.max.x < b.min.x || a.min.x > b.max.x)
        return false;
    if (a.max.y < b.min.y || a.min.y > b.max.y)
        return false;
    if (a.max.z < b.min.z || a.min.z > b.max.z)
        return false;
    return true;
}




bool intersects(const BoundingSphere &s, const AABB &box)
{
    
    
    
    glm::vec3 closest;
    closest.x = fmaxf(box.min.x, fminf(s.centre.x, box.max.x));
    closest.y = fmaxf(box.min.y, fminf(s.centre.y, box.max.y));
    closest.z = fmaxf(box.min.z, fminf(s.centre.z, box.max.z));

    glm::vec3 d = s.centre - closest;
    float distSq = d.x * d.x + d.y * d.y + d.z * d.z;

    return distSq <= (s.radius * s.radius);
}