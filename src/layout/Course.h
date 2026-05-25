#ifndef COURSE_H
#define COURSE_H

#include "../core/SceneObject.h"
#include "../objects/GolfHole.h"
#include "../objects/Rock.h"
#include "../objects/Tunnel.h"
#include "../objects/Windmill.h"
#include "../objects/Floor.h"
#include "../objects/Water.h"
#include "../objects/AssimpModel.h"
#include "../objects/GrassBase.h"

#include <vector>

// Course: builds and owns all 18 holes, obstacles, water, and environmental
// decorations. Add to scene via addToScene().
//
// Layout reconstructed from the Crescent Head Country Club plan:
//   - Image left->right  maps to world -X -> +X
//   - Image bottom->top  maps to world -Z -> +Z
//   - Gazebo at origin (0,0,0); lake sits to the NE (+X,+Z)
//   - Holes form an outer ring (~35u) looping the central lake/gazebo,
//     hand-placed to match the map's clustering rather than a clean circle.
class Course
{
public:
    Course();
    ~Course();

    // Build all geometry (call before addToScene)
    void build();

    // Register all objects with the scene's addObject function.
    // Transparent objects (Water) are routed by SceneObject::isTransparent(),
    // so a single addFn is sufficient.
    void addToScene(void (*addFn)(SceneObject *));

    void update(float dt);

private:
    // ---- Level Base ----
    Floor baseFloor;
    Cube pathFloor;
    Cube boundWalls[4];
    // ---- Holes ----
    GolfHole holes[18];
    GrassBase grassBases[18];

    // ---- Rocks ----
    static const int MAX_ROCKS = 32;
    Rock rocks[MAX_ROCKS];
    int numRocks;
    int rockIndex;

    // ---- Obstacles ----
    Tunnel tunnel;     // hole 7
    Windmill windmill; // hole 8

    // ---- Water ----
    // Central lake, NE of origin. Two overlapping quads approximate the
    // irregular crescent shown on the plan.
    static const int NUM_WATER = 2;
    Water water[NUM_WATER];

    // ---- Assimp Decorations ----
    AssimpModel *gazebo;
    std::vector<AssimpModel *> trees;
    std::vector<AssimpModel *> lamps;

    // ---- setup helpers ----
    Rock &nextRock();
    void spawnTree(const glm::vec3 &position, float scale);
    void spawnLamp(const glm::vec3 &position);

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
};

#endif