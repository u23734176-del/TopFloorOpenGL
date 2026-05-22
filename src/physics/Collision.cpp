#include "physics/Collision.h"
#include <algorithm>
#include <cmath>

bool intersects(const AABB &a, const AABB &b){
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool intersects(const BoundingSphere &s, const AABB &a){
    // Find the closest point on the AABB to the sphere's center
    float x = std::max(a.min.x, std::min(s.centre.x, a.max.x));
    float y = std::max(a.min.y, std::min(s.centre.y, a.max.y));
    float z = std::max(a.min.z, std::min(s.centre.z, a.max.z));

    // Calculate distance from closest point to sphere center
    float distance = std::sqrt((x - s.centre.x) * (x - s.centre.x) +
                               (y - s.centre.y) * (y - s.centre.y) +
                               (z - s.centre.z) * (z - s.centre.z));

    return distance < s.radius;
}