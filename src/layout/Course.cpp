#include "Course.h"

// ---------------------------------------------------------------------------
// World positions derived from the annotated course map.
// The map shows holes arranged around a central lake.
// We use a scale of roughly 1 OpenGL unit = 1 metre.
//
// Coordinate origin (0,0,0) is approximately at the gazebo/centre.
// X: positive = right (east on map)
// Z: positive = forward/south; negative = north
// ---------------------------------------------------------------------------

Course::Course()
    : numRocks(0), rockIndex(0)
{}

Course::~Course() {}

Rock& Course::nextRock()
{
    if (rockIndex < MAX_ROCKS) {
        return rocks[rockIndex++];
    }
    // Fallback: return last slot (shouldn't happen if MAX_ROCKS is large enough)
    return rocks[MAX_ROCKS - 1];
}

// ---------------------------------------------------------------------------
// HOLE SETUP — dimensions from drawings, positions from annotated map
// ---------------------------------------------------------------------------

void Course::setupHole1()
{
    GolfHole& h = holes[0];
    h.holeNumber = 1;
    h.holeWidth  = 10.0f;
    h.holeLength = 5.0f;
    // Kidney shape approximated as single rect + slight extra segment
    h.hasSand    = true;
    h.sandOffset = glm::vec3(-2.5f, 0.0f, 0.5f);
    h.sandSize   = glm::vec2(2.5f, 1.5f);
    h.setPosition(glm::vec3(8.0f, 0.0f, 32.0f));  // bottom-right on map
    h.setRotation(glm::vec3(0.0f, 10.0f, 0.0f));
}

void Course::setupHole2()
{
    GolfHole& h = holes[1];
    h.holeNumber = 2;
    h.holeWidth  = 10.0f;
    h.holeLength = 5.0f;
    h.hasSand    = false;
    h.setPosition(glm::vec3(20.0f, 0.0f, 25.0f));
    h.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
}

void Course::setupHole3()
{
    GolfHole& h = holes[2];
    h.holeNumber = 3;
    // L-shaped: main segment + one perpendicular segment
    h.holeWidth  = 3.0f;
    h.holeLength = 8.0f;
    h.hasSand    = true;
    h.sandOffset = glm::vec3(0.0f, 0.0f, -3.0f);
    h.sandSize   = glm::vec2(1.5f, 1.2f);
    // Extra arm going left
    HoleSegment arm;
    arm.offsetX  = -3.0f;
    arm.offsetZ  = 4.0f;
    arm.width    = 4.0f;
    arm.length   = 3.0f;
    arm.rotY     = 0.0f;
    h.extraSegments.push_back(arm);
    h.setPosition(glm::vec3(28.0f, 0.0f, 16.0f));
    h.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
}

void Course::setupHole4()
{
    GolfHole& h = holes[3];
    h.holeNumber = 4;
    h.holeWidth  = 8.0f;
    h.holeLength = 13.0f;
    h.hasSand    = true;
    h.sandOffset = glm::vec3(0.0f, 0.0f, 2.0f);
    h.sandSize   = glm::vec2(5.0f, 1.2f);
    h.setPosition(glm::vec3(30.0f, 0.0f, 3.0f));
    h.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
}

void Course::setupHole5()
{
    GolfHole& h = holes[4];
    h.holeNumber = 5;
    h.holeWidth  = 8.0f;
    h.holeLength = 21.0f;
    h.hasSand    = false;
    h.setPosition(glm::vec3(28.0f, 0.0f, -18.0f));
    h.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));

    // Rocks (grey circles on drawing)
    Rock& r1 = nextRock();
    r1.setPosition(glm::vec3(28.0f, 0.3f, -12.0f));
    r1.setScale(glm::vec3(0.5f, 0.35f, 0.5f));
    r1.setColor(glm::vec3(0.55f, 0.55f, 0.55f));

    Rock& r2 = nextRock();
    r2.setPosition(glm::vec3(29.5f, 0.3f, -14.0f));
    r2.setScale(glm::vec3(0.4f, 0.3f, 0.4f));
    r2.setColor(glm::vec3(0.5f, 0.5f, 0.55f));
}

void Course::setupHole6()
{
    GolfHole& h = holes[5];
    h.holeNumber = 6;
    // Has a water hazard at top — Tadi's water plane will sit here.
    // The green itself: ~8x7m (below the water section)
    h.holeWidth  = 8.0f;
    h.holeLength = 12.0f;
    h.hasSand    = false;
    // Sloped: mark for Tadi; we just set it flat here
    h.setPosition(glm::vec3(22.0f, 0.0f, -32.0f));
    h.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
}

void Course::setupHole7()
{
    GolfHole& h = holes[6];
    h.holeNumber = 7;
    // S-curve: main + two extra segments offset
    h.holeWidth  = 8.0f;
    h.holeLength = 12.0f;

    HoleSegment s1;
    s1.offsetX = -5.0f; s1.offsetZ = 9.0f;
    s1.width   = 7.0f;  s1.length  = 10.0f;
    s1.rotY    = 0.0f;
    h.extraSegments.push_back(s1);

    HoleSegment s2;
    s2.offsetX =  4.0f; s2.offsetZ = -10.0f;
    s2.width   =  6.0f; s2.length  =  8.0f;
    s2.rotY    = 0.0f;
    h.extraSegments.push_back(s2);

    h.setPosition(glm::vec3(8.0f, 0.0f, -32.0f));

    // Tunnel across the S-curve path
    tunnel.tunnelRadius  = 1.2f;
    tunnel.tunnelLength  = 3.5f;
    tunnel.wallThickness = 0.2f;
    tunnel.setPosition(glm::vec3(4.0f, 1.2f, -26.0f));
    tunnel.setColor(glm::vec3(0.45f, 0.32f, 0.18f)); // brown wood
}

void Course::setupHole8()
{
    GolfHole& h = holes[7];
    h.holeNumber = 8;
    h.holeWidth  = 20.0f;  // 50m wide in drawing — scaled down
    h.holeLength = 10.0f;
    h.hasSand    = true;
    h.sandOffset = glm::vec3(5.0f, 0.0f, 0.0f);
    h.sandSize   = glm::vec2(4.0f, 3.0f);
    h.setPosition(glm::vec3(-5.0f, 0.0f, -28.0f));
    h.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));

    // Windmill lives near this hole
    windmill.towerRadius  = 0.5f;
    windmill.towerHeight  = 4.0f;
    windmill.bladeLength  = 2.0f;
    windmill.bladeWidth   = 0.35f;
    windmill.setPosition(glm::vec3(0.0f, 0.0f, -24.0f));
    windmill.setRotation(glm::vec3(0.0f, 30.0f, 0.0f));
}

void Course::setupHole9()
{
    GolfHole& h = holes[8];
    h.holeNumber = 9;
    h.holeWidth  = 30.0f;
    h.holeLength = 5.0f;  // elongated horizontal (30x15 → scaled)
    h.hasSand    = false;
    h.setPosition(glm::vec3(-10.0f, 0.0f, -12.0f));
    h.setRotation(glm::vec3(0.0f, 90.0f, 0.0f)); // rotated horizontal

    // Two rocks from drawing (red + grey)
    Rock& r1 = nextRock();
    r1.setPosition(glm::vec3(2.0f, 0.4f, -12.0f));
    r1.setScale(glm::vec3(0.7f, 0.45f, 0.6f));
    r1.setColor(glm::vec3(0.65f, 0.25f, 0.2f)); // reddish

    Rock& r2 = nextRock();
    r2.setPosition(glm::vec3(-4.0f, 0.35f, -12.0f));
    r2.setScale(glm::vec3(0.6f, 0.4f, 0.55f));
    r2.setColor(glm::vec3(0.5f, 0.5f, 0.5f));
}

void Course::setupHole10()
{
    GolfHole& h = holes[9];
    h.holeNumber = 10;
    h.holeWidth  = 7.0f;
    h.holeLength = 8.0f;
    h.hasSand    = false;
    // Sloped green (Tadi handles vertex displacement; we provide the flat mesh)
    h.setPosition(glm::vec3(-12.0f, 0.0f, -22.0f));

    // Rocks
    Rock& r1 = nextRock();
    r1.setPosition(glm::vec3(-13.0f, 0.35f, -24.0f));
    r1.setScale(glm::vec3(0.5f, 0.35f, 0.5f));
    r1.setColor(glm::vec3(0.52f, 0.52f, 0.52f));

    Rock& r2 = nextRock();
    r2.setPosition(glm::vec3(-11.0f, 0.3f, -25.5f));
    r2.setScale(glm::vec3(0.45f, 0.3f, 0.45f));
    r2.setColor(glm::vec3(0.48f, 0.48f, 0.5f));
}

void Course::setupHole11()
{
    GolfHole& h = holes[10];
    h.holeNumber = 11;
    h.holeWidth  = 20.0f;
    h.holeLength = 11.0f; // 20x22 → split evenly
    h.hasSand    = true;
    h.sandOffset = glm::vec3(3.0f, 0.0f, 1.0f);
    h.sandSize   = glm::vec2(4.0f, 3.5f);
    h.setPosition(glm::vec3(-18.0f, 0.0f, -30.0f));

    // Two rocks from drawing
    Rock& r1 = nextRock();
    r1.setPosition(glm::vec3(-20.0f, 0.4f, -28.0f));
    r1.setScale(glm::vec3(0.6f, 0.4f, 0.6f));
    r1.setColor(glm::vec3(0.5f, 0.5f, 0.5f));

    Rock& r2 = nextRock();
    r2.setPosition(glm::vec3(-16.0f, 0.35f, -31.0f));
    r2.setScale(glm::vec3(0.55f, 0.38f, 0.5f));
    r2.setColor(glm::vec3(0.48f, 0.48f, 0.52f));
}

void Course::setupHole12()
{
    GolfHole& h = holes[11];
    h.holeNumber = 12;
    h.holeWidth  = 10.0f;
    h.holeLength = 11.0f; // 10x22 → scaled
    // Peanut/figure-8: two segments
    HoleSegment s2;
    s2.offsetX = 1.0f; s2.offsetZ = -8.0f;
    s2.width   = 8.0f; s2.length  =  9.0f;
    s2.rotY    = 0.0f;
    h.extraSegments.push_back(s2);
    h.hasSand    = false;
    h.setPosition(glm::vec3(-22.0f, 0.0f, -40.0f));
    // Sloped — flag goes in the upper lobe

    // Grey rock centre
    Rock& r1 = nextRock();
    r1.setPosition(glm::vec3(-21.5f, 0.4f, -42.0f));
    r1.setScale(glm::vec3(0.8f, 0.5f, 1.2f));
    r1.setColor(glm::vec3(0.58f, 0.58f, 0.6f));
}

void Course::setupHole13()
{
    GolfHole& h = holes[12];
    h.holeNumber = 13;
    h.holeWidth  = 5.0f;
    h.holeLength = 17.0f;
    // L-hook: main vertical + arm going right at bottom
    HoleSegment arm;
    arm.offsetX = 4.0f;  arm.offsetZ = -7.0f;
    arm.width   = 6.0f;  arm.length  =  3.0f;
    arm.rotY    = 0.0f;
    h.extraSegments.push_back(arm);
    h.hasSand    = false;
    h.setPosition(glm::vec3(-5.0f, 0.0f, -38.0f));
}

void Course::setupHole14()
{
    GolfHole& h = holes[13];
    h.holeNumber = 14;
    h.holeWidth  = 7.0f;
    h.holeLength = 14.0f;
    h.hasSand    = true;
    h.sandOffset = glm::vec3(-1.0f, 0.0f, 1.0f);
    h.sandSize   = glm::vec2(2.5f, 2.0f);
    h.setPosition(glm::vec3(4.0f, 0.0f, -44.0f));
    h.setRotation(glm::vec3(0.0f, 90.0f, 0.0f)); // banana runs horizontally

    // Row of rocks (dots along inner edge in drawing)
    float rockZ = -44.0f;
    for (int i = 0; i < 5; ++i) {
        Rock& r = nextRock();
        r.setPosition(glm::vec3(-3.0f + i * 2.5f, 0.3f, rockZ));
        r.setScale(glm::vec3(0.3f, 0.22f, 0.3f));
        r.setColor(glm::vec3(0.55f, 0.55f, 0.55f));
        numRocks = rockIndex;
    }
}

void Course::setupHole15()
{
    GolfHole& h = holes[14];
    h.holeNumber = 15;
    h.holeWidth  = 10.0f;
    h.holeLength = 8.0f; // 10x16 scaled
    HoleSegment s2;
    s2.offsetX = -3.0f; s2.offsetZ = 6.0f;
    s2.width   =  8.0f; s2.length  = 7.0f;
    s2.rotY    = 0.0f;
    h.extraSegments.push_back(s2);
    h.hasSand    = false;
    h.setPosition(glm::vec3(-14.0f, 0.0f, -50.0f));

    // Rocks
    Rock& r1 = nextRock();
    r1.setPosition(glm::vec3(-17.0f, 0.35f, -48.0f));
    r1.setScale(glm::vec3(0.45f, 0.3f, 0.45f));
    r1.setColor(glm::vec3(0.52f, 0.52f, 0.52f));
}

void Course::setupHole16()
{
    GolfHole& h = holes[15];
    h.holeNumber = 16;
    h.holeWidth  = 7.0f;
    h.holeLength = 15.0f;
    h.hasSand    = false;
    h.setPosition(glm::vec3(-26.0f, 0.0f, -40.0f));

    // Rocks + orange dot path (path dots = very small rocks or decorations)
    Rock& r1 = nextRock();
    r1.setPosition(glm::vec3(-25.0f, 0.4f, -36.0f));
    r1.setScale(glm::vec3(0.55f, 0.38f, 0.55f));
    r1.setColor(glm::vec3(0.55f, 0.55f, 0.58f));
}

void Course::setupHole17()
{
    GolfHole& h = holes[16];
    h.holeNumber = 17;
    h.holeWidth  = 10.0f; // 20x25 scaled
    h.holeLength = 12.0f;
    h.hasSand    = true;
    h.sandOffset = glm::vec3(1.5f, 0.0f, 0.0f);
    h.sandSize   = glm::vec2(4.0f, 8.0f);
    h.setPosition(glm::vec3(-28.0f, 0.0f, -22.0f));

    // Rocks
    Rock& r1 = nextRock();
    r1.setPosition(glm::vec3(-27.0f, 0.4f, -20.0f));
    r1.setScale(glm::vec3(0.5f, 0.35f, 0.5f));
    r1.setColor(glm::vec3(0.5f, 0.5f, 0.5f));
}

void Course::setupHole18()
{
    GolfHole& h = holes[17];
    h.holeNumber = 18;
    h.holeWidth  = 9.0f;
    h.holeLength = 13.0f; // 9x26 scaled
    h.hasSand    = true;
    h.sandOffset = glm::vec3(0.5f, 0.0f, 0.0f);
    h.sandSize   = glm::vec2(2.0f, 6.0f);
    h.setPosition(glm::vec3(-26.0f, 0.0f, -8.0f));
    // Brown centre strip (bridge) — Karabo's Bridge object will go here
}

// ---------------------------------------------------------------------------
// build() — build all geometry
// ---------------------------------------------------------------------------

void Course::build()
{
    setupHole1();  setupHole2();  setupHole3();
    setupHole4();  setupHole5();  setupHole6();
    setupHole7();  setupHole8();  setupHole9();
    setupHole10(); setupHole11(); setupHole12();
    setupHole13(); setupHole14(); setupHole15();
    setupHole16(); setupHole17(); setupHole18();

    for (int i = 0; i < 18; ++i) {
        holes[i].build();
    }

    for (int i = 0; i < rockIndex; ++i) {
        rocks[i].build();
    }
    numRocks = rockIndex;

    tunnel.build();
    windmill.build();
}

// ---------------------------------------------------------------------------
// addToScene()
// ---------------------------------------------------------------------------

void Course::addToScene(void (*addFn)(SceneObject*))
{
    for (int i = 0; i < 18; ++i) {
        addFn(&holes[i]);
    }
    for (int i = 0; i < numRocks; ++i) {
        addFn(&rocks[i]);
    }
    addFn(&tunnel);
    addFn(&windmill);
}
