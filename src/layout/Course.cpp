#include "Course.h"
#include "../core/ResourceManager.h"
#include <glm/gtc/constants.hpp>
#include <cstdlib>

Course::Course() : numRocks(0), rockIndex(0), gazebo(nullptr) {}

Course::~Course()
{
    if (gazebo)
        delete gazebo; // Note: if managed by ResourceManager, handle carefully
    for (auto p : pathways)
        delete p;
    for (auto t : trees)
        delete t;
    for (auto l : lamps)
        delete l;
    for (auto b : bushes)
        delete b;
    for (auto b : benches)
        delete b;
}

Rock &Course::nextRock()
{
    if (rockIndex < MAX_ROCKS)
        return rocks[rockIndex++];
    return rocks[MAX_ROCKS - 1];
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
void Course::createPathSegment(const glm::vec3 &pos, const glm::vec3 &scale, float rotY)
{
    Floor *p = new Floor();
    p->setPosition(pos);
    p->setScale(scale);
    p->setRotation(glm::vec3(0.0f, rotY, 0.0f));
    p->setColor(glm::vec3(0.65f, 0.65f, 0.65f)); // Concrete grey
    pathways.push_back(p);
}

void Course::spawnTree(const glm::vec3 &pos, float scale)
{
    trees.push_back(ResourceManager::getInstance()->getOrLoadModel("external/Tree_One.obj"));
    trees.back()->setPosition(pos);
    trees.back()->setScale(glm::vec3(scale));
}

void Course::spawnBush(const glm::vec3 &pos, float scale)
{
    bushes.push_back(ResourceManager::getInstance()->getOrLoadModel("external/Bush.obj"));
    bushes.back()->setPosition(pos);
    bushes.back()->setScale(glm::vec3(scale));
}

void Course::spawnLamp(const glm::vec3 &pos)
{
    lamps.push_back(ResourceManager::getInstance()->getOrLoadModel("external/Light_Post.obj"));
    lamps.back()->setPosition(pos);
}

// ---------------------------------------------------------------------------
// Layout Logic
// ---------------------------------------------------------------------------

void Course::generatePathways()
{
    int numSegments = 40;
    float radius = 18.0f;
    for (int i = 0; i < numSegments; ++i)
    {
        float theta = (glm::two_pi<float>() / numSegments) * i;
        float nextTheta = (glm::two_pi<float>() / numSegments) * (i + 1);
        glm::vec3 p1(radius * cos(theta), -0.05f, radius * sin(theta));
        glm::vec3 p2(radius * cos(nextTheta), -0.05f, radius * sin(nextTheta));
        glm::vec3 mid = (p1 + p2) * 0.5f;
        float dist = glm::length(p2 - p1);
        float angle = glm::degrees(atan2(p1.x - p2.x, p1.z - p2.z));
        createPathSegment(mid, glm::vec3(1.8f, 0.1f, dist * 0.6f), angle);
    }
}

void Course::scatterDecorations()
{
    for (int i = 0; i < 360; i += 20)
    {
        float rad = glm::radians((float)i);
        float r = 40.0f + (rand() % 10);
        spawnTree(glm::vec3(r * cos(rad), 0.0f, r * sin(rad)), 0.7f + (rand() % 3) * 0.1f);
        if (i % 40 == 0)
            spawnBush(glm::vec3((r - 5.0f) * cos(rad), 0.0f, (r - 5.0f) * sin(rad)), 0.6f);
    }
}

// ---------------------------------------------------------------------------
// Holes (Setup)
// ---------------------------------------------------------------------------
void Course::setupHole1() { holes[0].setPosition(glm::vec3(30.0f, 0.0f, 0.0f)); }
void Course::setupHole2() { holes[1].setPosition(glm::vec3(28.0f, 0.0f, 12.0f)); }
void Course::setupHole3() { holes[2].setPosition(glm::vec3(22.0f, 0.0f, 24.0f)); }
void Course::setupHole4() { holes[3].setPosition(glm::vec3(12.0f, 0.0f, 30.0f)); }
void Course::setupHole5() { holes[4].setPosition(glm::vec3(0.0f, 0.0f, 32.0f)); }
void Course::setupHole6() { holes[5].setPosition(glm::vec3(-12.0f, 0.0f, 30.0f)); }
void Course::setupHole7() { holes[6].setPosition(glm::vec3(-24.0f, 0.0f, 22.0f)); }
void Course::setupHole8() { holes[7].setPosition(glm::vec3(-30.0f, 0.0f, 10.0f)); }
void Course::setupHole9() { holes[8].setPosition(glm::vec3(-32.0f, 0.0f, -4.0f)); }
void Course::setupHole10() { holes[9].setPosition(glm::vec3(-28.0f, 0.0f, -16.0f)); }
void Course::setupHole11() { holes[10].setPosition(glm::vec3(-20.0f, 0.0f, -26.0f)); }
void Course::setupHole12() { holes[11].setPosition(glm::vec3(-8.0f, 0.0f, -30.0f)); }
void Course::setupHole13() { holes[12].setPosition(glm::vec3(4.0f, 0.0f, -32.0f)); }
void Course::setupHole14() { holes[13].setPosition(glm::vec3(16.0f, 0.0f, -28.0f)); }
void Course::setupHole15() { holes[14].setPosition(glm::vec3(26.0f, 0.0f, -20.0f)); }
void Course::setupHole16() { holes[15].setPosition(glm::vec3(32.0f, 0.0f, -10.0f)); }
void Course::setupHole17() { holes[16].setPosition(glm::vec3(34.0f, 0.0f, 5.0f)); }
void Course::setupHole18() { holes[17].setPosition(glm::vec3(22.0f, 0.0f, -4.0f)); }

// ---------------------------------------------------------------------------
// Build & Integrate
// ---------------------------------------------------------------------------
void Course::build()
{
    baseFloor.setScale(glm::vec3(60.0f, 0.1f, 60.0f));
    baseFloor.setPosition(glm::vec3(0.0f, -0.15f, 0.0f));
    baseFloor.setColor(glm::vec3(0.18f, 0.40f, 0.15f));
    baseFloor.build();

    lake.setPosition(glm::vec3(-10.0f, -0.05f, 0.0f));
    lake.build();

    gazebo = ResourceManager::getInstance()->getOrLoadModel("external/Gazboo.obj");
    gazebo->setPosition(glm::vec3(0.0f, -0.1f, 0.0f));
    gazebo->setScale(glm::vec3(0.03f));

    generatePathways();
    scatterDecorations();

    for (auto p : pathways)
        p->build();
    for (auto t : trees)
        t->build();
    for (auto b : bushes)
        b->build();
    for (auto l : lamps)
        l->build();

    // Set hole properties (standardize here)
    for (int i = 0; i < 18; ++i)
    {
        holes[i].holeNumber = i + 1;
        holes[i].build();
    }
}

void Course::update(float dt) { windmill.update(dt); }

void Course::addToScene(void (*addFn)(SceneObject *))
{
    addFn(&baseFloor);
    addFn(&lake);
    if (gazebo)
        addFn(gazebo);
    for (auto p : pathways)
        addFn(p);
    for (auto t : trees)
        addFn(t);
    for (auto b : bushes)
        addFn(b);
    for (auto l : lamps)
        addFn(l);
    for (int i = 0; i < 18; ++i)
        addFn(&holes[i]);
    addFn(&tunnel);
    addFn(&windmill);
}