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

#include <vector>

class Course
{
public:
    Course();
    ~Course();

    void build();
    void addToScene(void (*addFn)(SceneObject *));
    void update(float dt);

private:
    Floor baseFloor;
    Water lake; // Moving lake inside course manager!

    GolfHole holes[18];

    static const int MAX_ROCKS = 32;
    Rock rocks[MAX_ROCKS];
    int numRocks;
    int rockIndex;

    Tunnel tunnel;
    Windmill windmill;

    // Decoration arrays
    AssimpModel *gazebo;
    std::vector<Floor *> pathways;
    std::vector<AssimpModel *> trees;
    std::vector<AssimpModel *> lamps;
    std::vector<AssimpModel *> bushes;
    std::vector<AssimpModel *> benches;

    // Generators
    Rock &nextRock();
    void createPathSegment(const glm::vec3 &pos, const glm::vec3 &scale, float rotY);
    void spawnTree(const glm::vec3 &pos, float scale);
    void spawnBush(const glm::vec3 &pos, float scale);
    void spawnLamp(const glm::vec3 &pos);
    void spawnBench(const glm::vec3 &pos, float rotY);

    void generatePathways();
    void scatterDecorations();

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