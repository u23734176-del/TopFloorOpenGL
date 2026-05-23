#ifndef COURSE_H
#define COURSE_H

#include "../core/SceneObject.h"
#include "../objects/GolfHole.h"
#include "../objects/Rock.h"
#include "../objects/Tunnel.h"
#include "../objects/Windmill.h"

#include <vector>

// Course: builds and owns all 18 holes + Slice D obstacles.
// Add to scene via addToScene().
//
// World layout (top-down, matches the annotated map):
//   Holes 1-9  roughly form the outer clockwise ring.
//   Holes 10-18 fill the upper-left side.
//   The central lake area is around (0, 0, 0) in world space.
//
// All Y positions start at 0 (ground plane).
// Positions are approximate layout coordinates derived from the course map.

class Course
{
public:
    Course();
    ~Course();

    // Build all geometry (call before addToScene)
    void build();

    // Register all objects with the scene's addObject function
    // Uses a callback so we don't need to include Scene.h here.
    void addToScene(void (*addFn)(SceneObject*));

private:
    // ---- Holes ----
    GolfHole holes[18];

    // ---- Rocks ----
    // Holes with rocks: 5, 9, 10, 11, 12, 14, 15, 16
    // We give a fixed-size pool; unused ones are not added to scene.
    static const int MAX_ROCKS = 32;
    Rock   rocks[MAX_ROCKS];
    int    numRocks;

    // ---- Obstacles ----
    Tunnel   tunnel;     // hole 7
    Windmill windmill;   // hole 8

    // ---- setup helpers ----
    void setupHole1();
    void setupHole2();
    void setupHole3();
    void setupHole4();
    void setupHole5();
    void setupHole6();
    void setupHole7();
    void setupHole8();
    void setupHole9();
    void setupHole10();
    void setupHole11();
    void setupHole12();
    void setupHole13();
    void setupHole14();
    void setupHole15();
    void setupHole16();
    void setupHole17();
    void setupHole18();

    Rock& nextRock();
    int   rockIndex;
};

#endif
