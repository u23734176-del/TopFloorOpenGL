#include "Course.h"

// ---------------------------------------------------------------------------
// Course Layout — Crescent Head Country Club
//
// Coordinate mapping from the 2D plan (Image 1):
//   image left -> right  == world -X -> +X
//   image bottom -> top  == world -Z -> +Z
//   gazebo at origin (0,0,0); lake to the NE (+X, +Z)
//
// Holes form an outer loop (~35u radius) around the central lake + gazebo,
// hand-placed to match the map's clustering. Holes that are visibly curved on
// the plan use a `centerline` (faceted ribbon sweep, tee -> cup); near-straight
// holes stay simple rectangles.
//
// Centerline points are LOCAL XZ (before setPosition/setRotation), ordered
// tee -> cup. +Z is "forward". The flag is auto-placed at the last point.
// ---------------------------------------------------------------------------

Course::Course()
    : numRocks(0), rockIndex(0), gazebo(nullptr)
{
}

Course::~Course()
{
    if (gazebo)
        delete gazebo;
    for (auto tree : trees)
        delete tree;
    for (auto lamp : lamps)
        delete lamp;
}

Rock &Course::nextRock()
{
    if (rockIndex < MAX_ROCKS)
    {
        return rocks[rockIndex++];
    }
    return rocks[MAX_ROCKS - 1];
}

void Course::spawnTree(const glm::vec3 &position, float scale)
{
    AssimpModel *tree = new AssimpModel("external/Tree_One.obj");
    tree->setPosition(position);
    tree->setScale(glm::vec3(scale));
    trees.push_back(tree);
}

void Course::spawnLamp(const glm::vec3 &position)
{
    AssimpModel *lamp = new AssimpModel("external/Light_Post.obj");
    lamp->setPosition(position);
    lamp->setScale(glm::vec3(1.0f));
    lamps.push_back(lamp);
}

// ---------------------------------------------------------------------------
// HOLE SETUP — front 9 (south edge sweeping up the right/east side)
// ---------------------------------------------------------------------------

void Course::setupHole1()
{
    // Bottom-centre opening hole. Plan shows a wide rounded blob with a gentle
    // banana bend — a shallow S running left then back to centre.
    GolfHole &h = holes[0];
    h.holeNumber = 1;
    h.centerlineWidth = 3.5f;
    h.centerline = {
        {0.0f, -6.0f},
        {-1.5f, -2.0f},
        {-1.0f, 2.0f},
        {1.0f, 5.0f},
    };
    h.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    h.setPosition(glm::vec3(0.0f, 0.0f, -34.0f));
}

void Course::setupHole2()
{
    // South, just right of centre. L-shape as a SINGLE continuous centerline
    // (no extraSegments): run down the main arm, then dogleg east.
    GolfHole &h = holes[1];
    h.holeNumber = 2;
    h.centerlineWidth = 3.0f;
    h.centerline = {
        {0.0f, 4.0f},
        {0.0f, 0.0f},
        {0.0f, -3.5f},
        {3.0f, -4.0f},
        {6.5f, -4.0f},
    };
    h.setRotation(glm::vec3(0.0f, -20.0f, 0.0f));
    h.setPosition(glm::vec3(12.0f, 0.0f, -30.0f));
}

void Course::setupHole3()
{
    // Right edge, upper. Aimed NE following the east perimeter — slight bend.
    GolfHole &h = holes[2];
    h.holeNumber = 3;
    h.centerlineWidth = 2.5f;
    h.centerline = {
        {0.0f, -7.5f},
        {0.8f, -2.0f},
        {0.4f, 3.0f},
        {1.2f, 7.5f},
    };
    h.setRotation(glm::vec3(0.0f, 65.0f, 0.0f));
    h.setPosition(glm::vec3(33.0f, 0.0f, 14.0f));
}

void Course::setupHole4()
{
    // Right edge, mid (due east). Sand bunker shown on plan. Mostly straight.
    GolfHole &h = holes[3];
    h.holeNumber = 4;
    h.holeWidth = 4.0f;
    h.holeLength = 10.0f;
    h.hasSand = true;
    h.sandOffset = glm::vec3(0.0f, 0.0f, 2.0f);
    h.sandSize = glm::vec2(2.0f, 2.0f);
    h.setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    h.setPosition(glm::vec3(37.0f, 0.0f, -3.0f));
}

void Course::setupHole5()
{
    // Right edge, low (ESE). Plan shows a curved lobe wrapping a rock.
    GolfHole &h = holes[4];
    h.holeNumber = 5;
    h.centerlineWidth = 3.0f;
    h.centerline = {
        {0.0f, -5.0f},
        {1.5f, -1.0f},
        {1.0f, 3.0f},
        {-0.5f, 5.0f},
    };
    h.setRotation(glm::vec3(0.0f, 115.0f, 0.0f));
    h.setPosition(glm::vec3(33.0f, 0.0f, -16.0f));

    Rock &r = nextRock();
    r.setPosition(glm::vec3(30.0f, 0.0f, -13.0f));
    r.setScale(glm::vec3(1.2f));
}

void Course::setupHole6()
{
    // Top-right (NE corner). Plan shows a small, sharp banana bend.
    GolfHole &h = holes[5];
    h.holeNumber = 6;
    h.centerlineWidth = 3.0f;
    h.centerline = {
        {0.0f, -6.0f},
        {2.0f, -2.5f},
        {2.5f, 1.0f},
        {0.5f, 5.0f},
    };
    h.setRotation(glm::vec3(0.0f, 45.0f, 0.0f));
    h.setPosition(glm::vec3(24.0f, 0.0f, 27.0f));
}

void Course::setupHole7()
{
    // Top, right of centre. Tunnel hole, aimed north. Mostly straight.
    GolfHole &h = holes[6];
    h.holeNumber = 7;
    h.holeWidth = 3.0f;
    h.holeLength = 16.0f;
    h.setRotation(glm::vec3(0.0f, 10.0f, 0.0f));
    h.setPosition(glm::vec3(12.0f, 0.0f, 33.0f));

    tunnel.setPosition(glm::vec3(12.0f, 0.0f, 33.0f));
    tunnel.setColor(glm::vec3(0.45f, 0.28f, 0.15f)); // Wood
}

void Course::setupHole8()
{
    // Inside the ring, just south of the lake near the gazebo. Windmill hole.
    GolfHole &h = holes[7];
    h.holeNumber = 8;
    h.holeWidth = 4.0f;
    h.holeLength = 15.0f;
    h.setRotation(glm::vec3(0.0f, -25.0f, 0.0f));
    h.setPosition(glm::vec3(6.0f, 0.0f, 8.0f));

    windmill.setPosition(glm::vec3(6.0f, 0.0f, 8.0f));
}

void Course::setupHole9()
{
    // South, just left of centre. Gentle curve closing the front 9.
    GolfHole &h = holes[8];
    h.holeNumber = 9;
    h.centerlineWidth = 3.0f;
    h.centerline = {
        {0.0f, -6.0f},
        {-1.0f, -1.5f},
        {-0.5f, 3.0f},
        {1.0f, 6.0f},
    };
    h.setRotation(glm::vec3(0.0f, 20.0f, 0.0f));
    h.setPosition(glm::vec3(-9.0f, 0.0f, -30.0f));
}

// ---------------------------------------------------------------------------
// HOLE SETUP — back 9 (top sweeping down the left/west side, then inward)
// ---------------------------------------------------------------------------

void Course::setupHole10()
{
    // Bottom-left (SW corner). Plan shows a large kidney shape.
    GolfHole &h = holes[9];
    h.holeNumber = 10;
    h.centerlineWidth = 4.0f;
    h.centerline = {
        {-2.0f, -5.0f},
        {0.5f, -2.0f},
        {2.0f, 1.5f},
        {0.0f, 4.5f},
        {-2.5f, 6.0f},
    };
    h.setRotation(glm::vec3(0.0f, -45.0f, 0.0f));
    h.setPosition(glm::vec3(-24.0f, 0.0f, -26.0f));
}

void Course::setupHole11()
{
    // Left edge, lower (WSW). Long curved sliver. Sand bunker on the plan.
    GolfHole &h = holes[10];
    h.holeNumber = 11;
    h.centerlineWidth = 3.0f;
    h.centerline = {
        {0.0f, -7.0f},
        {-1.5f, -2.5f},
        {-1.0f, 2.5f},
        {1.0f, 7.0f},
    };
    h.hasSand = true;
    h.sandOffset = glm::vec3(-1.0f, 0.0f, 0.0f);
    h.sandSize = glm::vec2(1.5f, 3.0f);
    h.setRotation(glm::vec3(0.0f, -110.0f, 0.0f));
    h.setPosition(glm::vec3(-33.0f, 0.0f, -16.0f));
}

void Course::setupHole12()
{
    // Top-left (NW). U-turn as a SINGLE continuous centerline (no extraSegments)
    // so it builds as one seamless ribbon: up the right arm, around the top,
    // down the left arm. Tee at bottom-right, cup at bottom-left.
    GolfHole &h = holes[11];
    h.holeNumber = 12;
    h.centerlineWidth = 3.0f;
    h.centerline = {
        {3.5f, -5.0f},
        {3.5f, -0.5f},
        {3.0f, 3.0f},
        {0.5f, 5.5f},
        {-3.0f, 3.0f},
        {-3.5f, -0.5f},
        {-3.5f, -5.0f},
    };
    h.setRotation(glm::vec3(0.0f, -130.0f, 0.0f));
    h.setPosition(glm::vec3(-24.0f, 0.0f, 25.0f));
}

void Course::setupHole13()
{
    // Upper-left edge (WNW). Long banana hugging the lake's west shore.
    GolfHole &h = holes[12];
    h.holeNumber = 13;
    h.centerlineWidth = 2.5f;
    h.centerline = {
        {0.0f, -7.0f},
        {1.5f, -3.0f},
        {2.0f, 1.0f},
        {1.0f, 4.5f},
        {-0.5f, 7.0f},
    };
    h.setRotation(glm::vec3(0.0f, -115.0f, 0.0f));
    h.setPosition(glm::vec3(-33.0f, 0.0f, 16.0f));
}

void Course::setupHole14()
{
    // Top centre (due north). Teardrop — evenly spaced so the cup cap lands at
    // the true end (no facet shorter than the fairway width).
    GolfHole &h = holes[13];
    h.holeNumber = 14;
    h.centerlineWidth = 4.0f;
    h.centerline = {
        {0.0f, -5.0f},
        {0.2f, -1.0f},
        {0.4f, 2.5f},
        {0.0f, 6.0f},
    };
    h.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    h.setPosition(glm::vec3(0.0f, 0.0f, 34.0f));
}

void Course::setupHole15()
{
    // Top, left of centre (N/NW). Short, slight bend.
    GolfHole &h = holes[14];
    h.holeNumber = 15;
    h.centerlineWidth = 3.0f;
    h.centerline = {
        {0.0f, -4.5f},
        {-0.8f, -1.0f},
        {-0.4f, 2.0f},
        {0.6f, 4.5f},
    };
    h.setRotation(glm::vec3(0.0f, -20.0f, 0.0f));
    h.setPosition(glm::vec3(-10.0f, 0.0f, 31.0f));
}

void Course::setupHole16()
{
    // Left edge, mid-upper (due west). Curved kidney; rock feature on the plan.
    GolfHole &h = holes[15];
    h.holeNumber = 16;
    h.centerlineWidth = 3.5f;
    h.centerline = {
        {-1.5f, -6.0f},
        {0.5f, -2.5f},
        {1.5f, 1.0f},
        {0.0f, 4.0f},
        {-1.5f, 6.0f},
    };
    h.setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
    h.setPosition(glm::vec3(-37.0f, 0.0f, 4.0f));

    Rock &r = nextRock();
    r.setPosition(glm::vec3(-34.0f, 0.0f, 6.0f));
}

void Course::setupHole17()
{
    // Left edge, mid (west). Sand bunker on the plan. Mostly straight.
    GolfHole &h = holes[16];
    h.holeNumber = 17;
    h.holeWidth = 3.0f;
    h.holeLength = 10.0f;
    h.hasSand = true;
    h.sandOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    h.sandSize = glm::vec2(2.5f, 2.5f);
    h.setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
    h.setPosition(glm::vec3(-35.0f, 0.0f, -6.0f));
}

void Course::setupHole18()
{
    // Inside the ring, lower-left near the gazebo — closing hole, long & gentle.
    GolfHole &h = holes[17];
    h.holeNumber = 18;
    h.centerlineWidth = 4.0f;
    h.centerline = {
        {0.0f, -7.5f},
        {1.0f, -3.0f},
        {0.5f, 2.0f},
        {-1.0f, 7.5f},
    };
    h.setRotation(glm::vec3(0.0f, 35.0f, 0.0f));
    h.setPosition(glm::vec3(-14.0f, 0.0f, -9.0f));
}

// ---------------------------------------------------------------------------
// build() — build all geometry
// ---------------------------------------------------------------------------

void Course::build()
{
    // 1. Build Base Floor
    baseFloor.setScale(glm::vec3(50.0f, 0.1f, 50.0f));    // 100x100m play area
    baseFloor.setPosition(glm::vec3(0.0f, -0.11f, 0.0f)); // Just beneath the turf
    baseFloor.setColor(glm::vec3(0.2f, 0.15f, 0.1f));     // Dark dirt color
    baseFloor.build();

    // 2. Build Assimp Models
    gazebo = new AssimpModel("external/Gazboo.obj");
    gazebo->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    gazebo->setScale(glm::vec3(0.04f)); // 3x larger
    gazebo->build();
    gazebo->setSubMeshColor("", glm::vec3(0.8f, 0.8f, 0.8f));

    // Decorate map — spread around the ring and along the lakeside.
    spawnTree(glm::vec3(18.0f, 0.0f, 20.0f), 0.5f);   // NE, lake edge
    spawnTree(glm::vec3(-20.0f, 0.0f, 18.0f), 0.6f);  // NW
    spawnTree(glm::vec3(28.0f, 0.0f, -22.0f), 0.4f);  // SE corner
    spawnTree(glm::vec3(-28.0f, 0.0f, -20.0f), 0.5f); // SW corner
    spawnTree(glm::vec3(-30.0f, 0.0f, 22.0f), 0.45f); // W/NW
    spawnTree(glm::vec3(8.0f, 0.0f, 18.0f), 0.4f);    // lakeside, near gazebo

    spawnLamp(glm::vec3(6.0f, 0.0f, 6.0f));     // by gazebo / hole 8
    spawnLamp(glm::vec3(-12.0f, 0.0f, -12.0f)); // inner SW, near hole 18
    spawnLamp(glm::vec3(28.0f, 0.0f, 4.0f));    // east, near holes 3/4
    spawnLamp(glm::vec3(-28.0f, 0.0f, 2.0f));   // west, near holes 16/17
    spawnLamp(glm::vec3(0.0f, 0.0f, 28.0f));    // north, near holes 7/14

    for (auto tree : trees)
    {
        tree->build();
        tree->setSubMeshColor("", glm::vec3(0.15f, 0.4f, 0.15f));
    }
    for (auto lamp : lamps)
    {
        lamp->build();
        lamp->setSubMeshColor("", glm::vec3(0.2f, 0.2f, 0.2f));
    }

    // 3. Build Water — central lake, NE of origin. Two overlapping quads
    //    approximate the irregular crescent on the plan.
    water[0].setSize(11.0f, 8.0f); // main body
    water[0].setPosition(glm::vec3(10.0f, 0.05f, 12.0f));
    water[0].setColor(glm::vec3(0.10f, 0.35f, 0.55f));
    water[0].setAlpha(0.7f);
    water[0].setShininess(128.0f);
    water[0].build();

    water[1].setSize(6.0f, 5.0f); // SE tail of the crescent
    water[1].setPosition(glm::vec3(20.0f, 0.05f, 4.0f));
    water[1].setColor(glm::vec3(0.10f, 0.35f, 0.55f));
    water[1].setAlpha(0.7f);
    water[1].setShininess(128.0f);
    water[1].build();

    // 4. Build Holes
    setupHole1();
    setupHole2();
    setupHole3();
    setupHole4();
    setupHole5();
    setupHole6();
    setupHole7();
    setupHole8();
    setupHole9();
    setupHole10();
    setupHole11();
    setupHole12();
    setupHole13();
    setupHole14();
    setupHole15();
    setupHole16();
    setupHole17();
    setupHole18();

    for (int i = 0; i < 18; ++i)
    {
        holes[i].build();
    }

    // 5. Build Obstacles
    for (int i = 0; i < rockIndex; ++i)
    {
        rocks[i].build();
        rocks[i].setColor(glm::vec3(0.5f, 0.5f, 0.5f)); // Grey rock
    }
    numRocks = rockIndex;

    tunnel.build();
    windmill.build();
}

void Course::update(float dt)
{
    windmill.update(dt);
}

void Course::addToScene(void (*addFn)(SceneObject *))
{
    addFn(&baseFloor);

    if (gazebo)
        addFn(gazebo);
    for (auto tree : trees)
        addFn(tree);
    for (auto lamp : lamps)
        addFn(lamp);

    for (int i = 0; i < 18; ++i)
    {
        addFn(&holes[i]);
    }
    for (int i = 0; i < numRocks; ++i)
    {
        addFn(&rocks[i]);
    }

    addFn(&tunnel);
    addFn(&windmill);

    // Water is transparent (isTransparent()==true); the scene routes it into
    // the transparent pass automatically.
    for (int i = 0; i < NUM_WATER; ++i)
    {
        addFn(&water[i]);
    }
}