#include "Course.h"

// ---------------------------------------------------------------------------
// Ground layering (y values):
//   -0.06 : dark earth base (Cube)
//   -0.01 : grey concrete path — top face at exactly y=0 (Cube, h=0.02)
//    0.01 : GrassBase ellipses — grass rough surround, clearly above path
//    0.04 : GolfHole fairway turf — clearly above grass base
//    0.06 : water surfaces
//
// Key fix: grass bases and holes use EXPLICIT y constants, not derived from
// getModelMatrix() before build() is called.
//
// Hole spacing: minimum gap between any two hole centrelines = 14 units,
// which at max centerlineWidth=5 leaves a 4.5u path gap on each side.
// ---------------------------------------------------------------------------

static const float BOUND    = 38.0f;  // half-extent of path floor
static const float WH       = 0.45f;
static const float WT       = 0.35f;

static const float Y_PATH   = -0.01f; // pathFloor centre y  (top face = 0.0)
static const float Y_GRASS  =  0.02f; // GrassBase y — above path, below turf
static const float Y_TURF   =  0.04f; // GolfHole y  — above grass base

static const glm::vec3 ROUGH_COL(0.13f, 0.48f, 0.10f);
static const glm::vec3 WALL_COL (0.12f, 0.10f, 0.07f);

Course::Course()
    : numRocks(0), rockIndex(0), gazebo(nullptr)
{}

Course::~Course()
{
    if (gazebo) delete gazebo;
    for (auto t : trees) delete t;
    for (auto l : lamps) delete l;
}

Rock& Course::nextRock()
{
    if (rockIndex < MAX_ROCKS) return rocks[rockIndex++];
    return rocks[MAX_ROCKS - 1];
}

void Course::spawnTree(const glm::vec3& pos, float s)
{
    AssimpModel* t = new AssimpModel("Tree_One.obj");
    t->setPosition(pos); t->setScale(glm::vec3(s));
    trees.push_back(t);
}

void Course::spawnLamp(const glm::vec3& pos)
{
    AssimpModel* l = new AssimpModel("Light_Post.obj");
    l->setPosition(pos); l->setScale(glm::vec3(1.0f));
    lamps.push_back(l);
}

// ---------------------------------------------------------------------------
// Hole + GrassBase setup.
// GrassBase position is set EXPLICITLY here using the same world XZ as the
// hole — never derived from getModelMatrix() which isn't valid until build().
// All holes use Y_TURF, all grass bases use Y_GRASS.
// ---------------------------------------------------------------------------

void Course::setupHole1()
{
    glm::vec3 pos(0, Y_TURF, -28); glm::vec3 rot(0,0,0);
    GolfHole& h = holes[0];
    h.holeNumber = 1; h.centerlineWidth = 4.5f;
    h.centerline = {{0,-5},{-1.5f,-1.5f},{-1,2},{1,5}};
    h.setPosition(pos); h.setRotation(rot);

    grassBases[0].radiusX = 6.5f; grassBases[0].radiusZ = 8.0f;
    grassBases[0].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[0].setRotation(rot); grassBases[0].setColor(ROUGH_COL);
}

void Course::setupHole2()
{
    glm::vec3 pos(10, Y_TURF, -24); glm::vec3 rot(0,-20,0);
    GolfHole& h = holes[1];
    h.holeNumber = 2; h.centerlineWidth = 4.0f;
    h.centerline = {{0,4},{0,0},{0,-3},{3,-3.5f},{6,-3.5f}};
    h.setPosition(pos); h.setRotation(rot);

    grassBases[1].radiusX = 8.0f; grassBases[1].radiusZ = 7.0f;
    grassBases[1].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[1].setRotation(rot); grassBases[1].setColor(ROUGH_COL);
}

void Course::setupHole3()
{
    glm::vec3 pos(27, Y_TURF, 12); glm::vec3 rot(0,65,0);
    GolfHole& h = holes[2];
    h.holeNumber = 3; h.centerlineWidth = 3.5f;
    h.centerline = {{0,-6},{0.8f,-2},{0.4f,3},{1.2f,6}};
    h.setPosition(pos); h.setRotation(rot);

    grassBases[2].radiusX = 5.0f; grassBases[2].radiusZ = 9.0f;
    grassBases[2].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[2].setRotation(rot); grassBases[2].setColor(ROUGH_COL);
}

void Course::setupHole4()
{
    glm::vec3 pos(30, Y_TURF, -2); glm::vec3 rot(0,90,0);
    GolfHole& h = holes[3];
    h.holeNumber = 4; h.holeWidth = 4.5f; h.holeLength = 12.0f;
    h.hasSand = true;
    h.sandOffset = glm::vec3(0,0,2); h.sandSize = glm::vec2(2.5f,2.5f);
    h.setPosition(pos); h.setRotation(rot);

    grassBases[3].radiusX = 7.5f; grassBases[3].radiusZ = 5.0f;
    grassBases[3].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[3].setRotation(rot); grassBases[3].setColor(ROUGH_COL);
}

void Course::setupHole5()
{
    glm::vec3 pos(27, Y_TURF, -14); glm::vec3 rot(0,115,0);
    GolfHole& h = holes[4];
    h.holeNumber = 5; h.centerlineWidth = 4.0f;
    h.centerline = {{0,-5},{1.5f,-1},{1,3},{-0.5f,5}};
    h.setPosition(pos); h.setRotation(rot);

    grassBases[4].radiusX = 6.0f; grassBases[4].radiusZ = 8.0f;
    grassBases[4].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[4].setRotation(rot); grassBases[4].setColor(ROUGH_COL);

    Rock& r = nextRock();
    r.setPosition({24.5f, Y_TURF, -11.5f}); r.setScale(glm::vec3(1.0f));
}

void Course::setupHole6()
{
    glm::vec3 pos(20, Y_TURF, 23); glm::vec3 rot(0,45,0);
    GolfHole& h = holes[5];
    h.holeNumber = 6; h.centerlineWidth = 4.0f;
    h.centerline = {{0,-5},{2,-2},{2.5f,1},{0.5f,4.5f}};
    h.setPosition(pos); h.setRotation(rot);

    grassBases[5].radiusX = 6.0f; grassBases[5].radiusZ = 8.0f;
    grassBases[5].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[5].setRotation(rot); grassBases[5].setColor(ROUGH_COL);
}

void Course::setupHole7()
{
    glm::vec3 pos(10, Y_TURF, 27); glm::vec3 rot(0,10,0);
    GolfHole& h = holes[6];
    h.holeNumber = 7; h.holeWidth = 4.0f; h.holeLength = 14.0f;
    h.setPosition(pos); h.setRotation(rot);

    grassBases[6].radiusX = 5.5f; grassBases[6].radiusZ = 9.5f;
    grassBases[6].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[6].setRotation(rot); grassBases[6].setColor(ROUGH_COL);

    tunnel.setPosition({pos.x, Y_TURF, pos.z});
    tunnel.setColor(glm::vec3(0.45f, 0.28f, 0.15f));
}

void Course::setupHole8()
{
    glm::vec3 pos(5, Y_TURF, 7); glm::vec3 rot(0,-25,0);
    GolfHole& h = holes[7];
    h.holeNumber = 8; h.holeWidth = 5.0f; h.holeLength = 14.0f;
    h.setPosition(pos); h.setRotation(rot);

    grassBases[7].radiusX = 7.5f; grassBases[7].radiusZ = 10.0f;
    grassBases[7].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[7].setRotation(rot); grassBases[7].setColor(ROUGH_COL);

    windmill.setPosition({pos.x, 0.0f, pos.z}); // base always at y=0
}

void Course::setupHole9()
{
    glm::vec3 pos(-8, Y_TURF, -25); glm::vec3 rot(0,20,0);
    GolfHole& h = holes[8];
    h.holeNumber = 9; h.centerlineWidth = 4.0f;
    h.centerline = {{0,-5},{-1,-1.5f},{-0.5f,3},{1,5.5f}};
    h.setPosition(pos); h.setRotation(rot);

    grassBases[8].radiusX = 6.0f; grassBases[8].radiusZ = 8.0f;
    grassBases[8].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[8].setRotation(rot); grassBases[8].setColor(ROUGH_COL);
}

void Course::setupHole10()
{
    glm::vec3 pos(-20, Y_TURF, -22); glm::vec3 rot(0,-45,0);
    GolfHole& h = holes[9];
    h.holeNumber = 10; h.centerlineWidth = 4.5f;
    h.centerline = {{-2,-5},{0.5f,-2},{2,1.5f},{0,4.5f},{-2.5f,6}};
    h.setPosition(pos); h.setRotation(rot);

    grassBases[9].radiusX = 7.5f; grassBases[9].radiusZ = 10.0f;
    grassBases[9].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[9].setRotation(rot); grassBases[9].setColor(ROUGH_COL);
}

void Course::setupHole11()
{
    glm::vec3 pos(-28, Y_TURF, -14); glm::vec3 rot(0,-110,0);
    GolfHole& h = holes[10];
    h.holeNumber = 11; h.centerlineWidth = 4.0f;
    h.centerline = {{0,-6},{-1.5f,-2.5f},{-1,2.5f},{1,6}};
    h.hasSand = true;
    h.sandOffset = glm::vec3(-1,0,0); h.sandSize = glm::vec2(2.0f,3.5f);
    h.setPosition(pos); h.setRotation(rot);

    grassBases[10].radiusX = 6.0f; grassBases[10].radiusZ = 9.0f;
    grassBases[10].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[10].setRotation(rot); grassBases[10].setColor(ROUGH_COL);
}

void Course::setupHole12()
{
    glm::vec3 pos(-20, Y_TURF, 21); glm::vec3 rot(0,-130,0);
    GolfHole& h = holes[11];
    h.holeNumber = 12; h.centerlineWidth = 3.5f;
    h.centerline = {
        {3.5f,-5},{3.5f,-0.5f},{3,3},{0.5f,5.5f},
        {-3,3},{-3.5f,-0.5f},{-3.5f,-5}
    };
    h.setPosition(pos); h.setRotation(rot);

    grassBases[11].radiusX = 8.5f; grassBases[11].radiusZ = 9.5f;
    grassBases[11].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[11].setRotation(rot); grassBases[11].setColor(ROUGH_COL);
}

void Course::setupHole13()
{
    glm::vec3 pos(-28, Y_TURF, 13); glm::vec3 rot(0,-115,0);
    GolfHole& h = holes[12];
    h.holeNumber = 13; h.centerlineWidth = 3.5f;
    h.centerline = {{0,-6},{1.5f,-2.5f},{2,1},{1,4.5f},{-0.5f,6}};
    h.setPosition(pos); h.setRotation(rot);

    grassBases[12].radiusX = 5.5f; grassBases[12].radiusZ = 9.0f;
    grassBases[12].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[12].setRotation(rot); grassBases[12].setColor(ROUGH_COL);
}

void Course::setupHole14()
{
    glm::vec3 pos(0, Y_TURF, 28); glm::vec3 rot(0,0,0);
    GolfHole& h = holes[13];
    h.holeNumber = 14; h.centerlineWidth = 4.5f;
    h.centerline = {{0,-5},{0.2f,-1},{0.4f,2.5f},{0,6}};
    h.setPosition(pos); h.setRotation(rot);

    grassBases[13].radiusX = 6.5f; grassBases[13].radiusZ = 9.0f;
    grassBases[13].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[13].setRotation(rot); grassBases[13].setColor(ROUGH_COL);
}

void Course::setupHole15()
{
    glm::vec3 pos(-9, Y_TURF, 26); glm::vec3 rot(0,-20,0);
    GolfHole& h = holes[14];
    h.holeNumber = 15; h.centerlineWidth = 4.0f;
    h.centerline = {{0,-4.5f},{-0.8f,-1},{-0.4f,2},{0.6f,4.5f}};
    h.setPosition(pos); h.setRotation(rot);

    grassBases[14].radiusX = 6.0f; grassBases[14].radiusZ = 7.5f;
    grassBases[14].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[14].setRotation(rot); grassBases[14].setColor(ROUGH_COL);
}

void Course::setupHole16()
{
    glm::vec3 pos(-31, Y_TURF, 3); glm::vec3 rot(0,-90,0);
    GolfHole& h = holes[15];
    h.holeNumber = 16; h.centerlineWidth = 4.0f;
    h.centerline = {{-1.5f,-6},{0.5f,-2.5f},{1.5f,1},{0,4},{-1.5f,6}};
    h.setPosition(pos); h.setRotation(rot);

    grassBases[15].radiusX = 7.0f; grassBases[15].radiusZ = 9.0f;
    grassBases[15].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[15].setRotation(rot); grassBases[15].setColor(ROUGH_COL);

    Rock& r = nextRock();
    r.setPosition({-28.5f, Y_TURF, 5.0f});
}

void Course::setupHole17()
{
    glm::vec3 pos(-29, Y_TURF, -5); glm::vec3 rot(0,-90,0);
    GolfHole& h = holes[16];
    h.holeNumber = 17; h.holeWidth = 4.0f; h.holeLength = 11.0f;
    h.hasSand = true;
    h.sandOffset = glm::vec3(0,0,0); h.sandSize = glm::vec2(3.0f,3.0f);
    h.setPosition(pos); h.setRotation(rot);

    grassBases[16].radiusX = 7.0f; grassBases[16].radiusZ = 5.0f;
    grassBases[16].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[16].setRotation(rot); grassBases[16].setColor(ROUGH_COL);
}

void Course::setupHole18()
{
    glm::vec3 pos(-12, Y_TURF, -8); glm::vec3 rot(0,35,0);
    GolfHole& h = holes[17];
    h.holeNumber = 18; h.centerlineWidth = 4.5f;
    h.centerline = {{0,-7},{1,-3},{0.5f,2},{-1,7}};
    h.setPosition(pos); h.setRotation(rot);

    grassBases[17].radiusX = 7.0f; grassBases[17].radiusZ = 11.0f;
    grassBases[17].setPosition({pos.x, Y_GRASS, pos.z});
    grassBases[17].setRotation(rot); grassBases[17].setColor(ROUGH_COL);
}

// ---------------------------------------------------------------------------
// build()
// ---------------------------------------------------------------------------
void Course::build()
{
    // ---- GROUND LAYERS ----
    // Dark earth base
    baseFloor.setScale(glm::vec3(BOUND + 2.0f, 0.04f, BOUND + 2.0f));
    baseFloor.setPosition(glm::vec3(0, -0.07f, 0));
    baseFloor.setColor(glm::vec3(0.18f, 0.13f, 0.08f));
    baseFloor.build();

    // Grey path — height=0.02, centred at Y_PATH=-0.01, so top face = exactly 0.0
    pathFloor.setScale(glm::vec3(BOUND, 0.02f, BOUND));
    pathFloor.setPosition(glm::vec3(0, Y_PATH, 0));
    pathFloor.setColor(glm::vec3(0.52f, 0.52f, 0.54f));
    pathFloor.build();

    // ---- BOUNDARY WALLS ----
    float H = BOUND * 0.5f;

    // ---- GAZEBO ----
    gazebo = new AssimpModel("Gazboo.obj");
    gazebo->setPosition({0,0,0}); gazebo->setScale(glm::vec3(0.04f));
    gazebo->build();
    gazebo->setSubMeshColor("", glm::vec3(0.8f,0.8f,0.8f));

    // ---- TREES ----
    spawnTree({ 16, 0,  18}, 0.5f);
    spawnTree({-18, 0,  16}, 0.6f);
    spawnTree({ 24, 0, -18}, 0.4f);
    spawnTree({-24, 0, -18}, 0.5f);
    spawnTree({-26, 0,  19}, 0.45f);
    spawnTree({  7, 0,  16}, 0.4f);

    // ---- LAMPS: 5 inside + 2 outside boundary ----
    spawnLamp({  5, 0,   5});
    spawnLamp({-10, 0, -10});
    spawnLamp({ 24, 0,   3});
    spawnLamp({-24, 0,   2});
    spawnLamp({  0, 0,  24});
    spawnLamp({ H + 2.5f, 0, 0}); // east outside boundary
    spawnLamp({-H - 2.5f, 0, 0}); // west outside boundary

    for (auto t : trees) { t->build(); t->setSubMeshColor("", {0.15f,0.4f,0.15f}); }
    for (auto l : lamps) { l->build(); l->setSubMeshColor("", {0.2f,0.2f,0.2f}); }

    // ---- WATER ----
    water[0].setSize(10.0f, 7.0f);
    water[0].setPosition({ 9.0f, 0.03f, 10.0f}); water[0].setColor({0.10f,0.35f,0.55f});
    water[0].setAlpha(0.7f); water[0].setShininess(128.0f); water[0].build();

    water[1].setSize(5.5f, 4.5f);
    water[1].setPosition({17.0f, 0.03f, 3.0f}); water[1].setColor({0.10f,0.35f,0.55f});
    water[1].setAlpha(0.7f); water[1].setShininess(128.0f); water[1].build();

    // ---- HOLES + GRASS BASES ----
    setupHole1();  setupHole2();  setupHole3();
    setupHole4();  setupHole5();  setupHole6();
    setupHole7();  setupHole8();  setupHole9();
    setupHole10(); setupHole11(); setupHole12();
    setupHole13(); setupHole14(); setupHole15();
    setupHole16(); setupHole17(); setupHole18();

    for (int i = 0; i < 18; ++i) {
        grassBases[i].build();  // build grass base first
        holes[i].build();       // then hole turf on top
    }

    // ---- ROCKS ----
    for (int i = 0; i < rockIndex; ++i) {
        rocks[i].build();
        rocks[i].setColor({0.52f, 0.52f, 0.52f});
    }
    numRocks = rockIndex;

    tunnel.build();
    windmill.build();
}

void Course::update(float dt) { windmill.update(dt); }

void Course::addToScene(void (*addFn)(SceneObject*))
{
    addFn(&baseFloor);
    addFn(&pathFloor);                          // y=0 grey path

    for (int i = 0; i < 18; ++i)
        addFn(&grassBases[i]);                  // y=0.01 rough surround

    if (gazebo) addFn(gazebo);
    for (auto t : trees) addFn(t);
    for (auto l : lamps) addFn(l);

    for (int i = 0; i < 18; ++i)
        addFn(&holes[i]);                       // y=0.04 fairway turf

    for (int i = 0; i < numRocks; ++i)
        addFn(&rocks[i]);

    addFn(&tunnel);
    addFn(&windmill);

    for (int i = 0; i < NUM_WATER; ++i)
        addFn(&water[i]);
}