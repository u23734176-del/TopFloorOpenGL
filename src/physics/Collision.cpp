#include "Collision.h"

// ---------------------------------------------------------------------------
// Collision.cpp  (Slice E - Karabo)
//
// Bodies for the two intersection tests declared in Collision.h. Signatures are
// frozen by the Day-0 contract, so only the implementations live here. No other
// shared file is touched.
// ---------------------------------------------------------------------------

// Standard AABB vs AABB overlap test (separating-axis on each world axis).
// Two boxes overlap only if they overlap on all three axes simultaneously.
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

// Sphere vs AABB. Find the closest point on the box to the sphere centre by
// clamping the centre into the box, then test the squared distance against the
// squared radius (squared avoids a sqrt).
bool intersects(const BoundingSphere &s, const AABB &box)
{
    // Clamp the sphere centre into the box on each axis. fminf/fmaxf come from
    // <cmath> (already pulled in by Collision.h) so we stay inside the spec's
    // allowed-include list -- <algorithm> is NOT on that list.
    glm::vec3 closest;
    closest.x = fmaxf(box.min.x, fminf(s.centre.x, box.max.x));
    closest.y = fmaxf(box.min.y, fminf(s.centre.y, box.max.y));
    closest.z = fmaxf(box.min.z, fminf(s.centre.z, box.max.z));

    glm::vec3 d = s.centre - closest;
    float distSq = d.x * d.x + d.y * d.y + d.z * d.z;

    return distSq <= (s.radius * s.radius);
}