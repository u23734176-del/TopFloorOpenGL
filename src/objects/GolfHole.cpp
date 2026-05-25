#include "GolfHole.h"
#include "../core/ShaderManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

GolfHole::GolfHole()
    : holeNumber(1), holeWidth(3.0f), holeLength(10.0f),
      hasSand(false), sandOffset(0.0f), sandSize(2.0f),
      vao(0), vbo(0), vertexCount(0)
{
    // Brighter synthetic putting green
    turfColor = glm::vec3(0.25f, 0.55f, 0.2f);
    // Sleek Wood/Brick border walls
    wallColor = glm::vec3(0.55f, 0.35f, 0.25f);
    // Standard red flag
    flagColor = glm::vec3(0.8f, 0.1f, 0.1f);
}

GolfHole::~GolfHole()
{
    if (vao)
        glDeleteVertexArrays(1, &vao);
    if (vbo)
        glDeleteBuffers(1, &vbo);
}

// Transform a point using a Y-axis rotation
static glm::vec3 rotY(const glm::vec3 &p, float deg)
{
    float rad = glm::radians(deg);
    float c = cos(rad), s = sin(rad);
    return glm::vec3(p.x * c + p.z * s, p.y, -p.x * s + p.z * c);
}

void GolfHole::buildSegment(std::vector<float> &data, float cx, float cz, float hw, float hl, float rotDeg)
{
    // Sleeker, lower walls typical of modern mini-golf
    float wt = 0.3f;  // wall thickness
    float wh = 0.25f; // wall height

    glm::vec3 upN(0.0f, 1.0f, 0.0f);

    // Dynamic AABB generation for Physics
    auto addWallBox = [&](glm::vec3 minP, glm::vec3 maxP)
    {
        AABB b;
        b.min = glm::vec3(1e30f);
        b.max = glm::vec3(-1e30f);
        glm::vec3 corners[8] = {
            {minP.x, minP.y, minP.z}, {maxP.x, minP.y, minP.z}, {minP.x, maxP.y, minP.z}, {maxP.x, maxP.y, minP.z}, {minP.x, minP.y, maxP.z}, {maxP.x, minP.y, maxP.z}, {minP.x, maxP.y, maxP.z}, {maxP.x, maxP.y, maxP.z}};
        for (int i = 0; i < 8; ++i)
        {
            glm::vec3 p = corners[i];
            p = rotY(p, rotDeg);
            p.x += cx;
            p.z += cz;
            b.min = glm::min(b.min, p);
            b.max = glm::max(b.max, p);
        }
        wallBoxes.push_back(b);
    };

    addWallBox(glm::vec3(-hw - wt, 0.0f, -hl), glm::vec3(-hw, wh, hl));           // Left
    addWallBox(glm::vec3(hw, 0.0f, -hl), glm::vec3(hw + wt, wh, hl));             // Right
    addWallBox(glm::vec3(-hw - wt, 0.0f, -hl - wt), glm::vec3(hw + wt, wh, -hl)); // Back
    addWallBox(glm::vec3(-hw - wt, 0.0f, hl), glm::vec3(hw + wt, wh, hl + wt));   // Front

    auto addQuad = [&](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 n, float type)
    {
        p0 = rotY(p0, rotDeg);
        p0.x += cx;
        p0.z += cz;
        p1 = rotY(p1, rotDeg);
        p1.x += cx;
        p1.z += cz;
        p2 = rotY(p2, rotDeg);
        p2.x += cx;
        p2.z += cz;
        p3 = rotY(p3, rotDeg);
        p3.x += cx;
        p3.z += cz;
        n = rotY(n, rotDeg);

        glm::vec3 pts[6] = {p0, p1, p2, p0, p2, p3};
        for (int i = 0; i < 6; ++i)
        {
            data.push_back(pts[i].x);
            data.push_back(pts[i].y);
            data.push_back(pts[i].z);
            data.push_back(n.x);
            data.push_back(n.y);
            data.push_back(n.z);
            data.push_back(type); // Pack color type into U coordinate
            data.push_back(0.0f);
        }
    };

    // Floor (Type 0.0 = Turf)
    addQuad(glm::vec3(-hw, 0.0f, -hl), glm::vec3(-hw, 0.0f, hl),
            glm::vec3(hw, 0.0f, hl), glm::vec3(hw, 0.0f, -hl), upN, 0.0f);

    // Walls (Type 1.0 = Wall)
    // Left Wall
    addQuad(glm::vec3(-hw - wt, wh, -hl), glm::vec3(-hw - wt, wh, hl), glm::vec3(-hw, wh, hl), glm::vec3(-hw, wh, -hl), upN, 1.0f);
    addQuad(glm::vec3(-hw, 0.0f, -hl), glm::vec3(-hw, 0.0f, hl), glm::vec3(-hw, wh, hl), glm::vec3(-hw, wh, -hl), glm::vec3(1, 0, 0), 1.0f);
    // Right Wall
    addQuad(glm::vec3(hw, wh, -hl), glm::vec3(hw, wh, hl), glm::vec3(hw + wt, wh, hl), glm::vec3(hw + wt, wh, -hl), upN, 1.0f);
    addQuad(glm::vec3(hw, 0.0f, hl), glm::vec3(hw, 0.0f, -hl), glm::vec3(hw, wh, -hl), glm::vec3(hw, wh, hl), glm::vec3(-1, 0, 0), 1.0f);
    // Back Wall
    addQuad(glm::vec3(-hw - wt, wh, -hl - wt), glm::vec3(-hw - wt, wh, -hl), glm::vec3(hw + wt, wh, -hl), glm::vec3(hw + wt, wh, -hl - wt), upN, 1.0f);
    addQuad(glm::vec3(hw, 0.0f, -hl), glm::vec3(-hw, 0.0f, -hl), glm::vec3(-hw, wh, -hl), glm::vec3(hw, wh, -hl), glm::vec3(0, 0, 1), 1.0f);
    // Front Wall
    addQuad(glm::vec3(-hw - wt, wh, hl), glm::vec3(-hw - wt, wh, hl + wt), glm::vec3(hw + wt, wh, hl + wt), glm::vec3(hw + wt, wh, hl), upN, 1.0f);
    addQuad(glm::vec3(-hw, 0.0f, hl), glm::vec3(hw, 0.0f, hl), glm::vec3(hw, wh, hl), glm::vec3(-hw, wh, hl), glm::vec3(0, 0, -1), 1.0f);
}

void GolfHole::build()
{
    wallBoxes.clear();
    std::vector<float> data;

    // Main fairway
    buildSegment(data, 0.0f, 0.0f, holeWidth, holeLength, 0.0f);

    // Extra segments (L-bends, U-turns)
    for (const auto &seg : extraSegments)
    {
        buildSegment(data, seg.offsetX, seg.offsetZ, seg.width, seg.length, seg.rotY);
    }

    // Flag pole (Type 2.0 = Flag)
    float px = 0.0f, pz = holeLength - 2.0f;
    glm::vec3 flagN(-1.0f, 0.0f, 0.0f);
    glm::vec3 p0(px, 0.0f, pz);
    glm::vec3 p1(px, 4.0f, pz);
    glm::vec3 p2(px + 1.5f, 3.5f, pz);
    glm::vec3 p3(px, 3.0f, pz);

    glm::vec3 pts[6] = {p0, p1, p2, p0, p2, p3}; // Simplified flag shape
    for (int i = 0; i < 6; ++i)
    {
        data.push_back(pts[i].x);
        data.push_back(pts[i].y);
        data.push_back(pts[i].z);
        data.push_back(flagN.x);
        data.push_back(flagN.y);
        data.push_back(flagN.z);
        data.push_back(2.0f);
        data.push_back(0.0f);
    }

    vertexCount = (int)(data.size() / 8);

    std::vector<glm::vec3> positions;
    for (size_t i = 0; i < data.size(); i += 8)
    {
        positions.push_back(glm::vec3(data[i], data[i + 1], data[i + 2]));
    }
    initAABBFromVertices(positions);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GolfHole::draw(const glm::mat4 &view, const glm::mat4 &proj, const LightSet & /*lights*/)
{
    GLuint shader = ShaderManager::get("basic");
    glUseProgram(shader);
    glUniform3fv(glGetUniformLocation(shader, "objectColor"), 1, &turfColor[0]);

    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &proj[0][0]);

    // Pass colors. Basic shader will use the packed U coord to lerp between these.
    glUniform3fv(glGetUniformLocation(shader, "turfColor"), 1, &turfColor[0]);
    glUniform3fv(glGetUniformLocation(shader, "wallColor"), 1, &wallColor[0]);
    glUniform3fv(glGetUniformLocation(shader, "flagColor"), 1, &flagColor[0]);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

void GolfHole::drawDepth(GLuint depthShader)
{
    glUseProgram(depthShader);
    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, &model[0][0]);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

std::vector<AABB> GolfHole::getCollisionAABBs() const
{
    std::vector<AABB> worldBoxes;
    glm::mat4 model = getModelMatrix();

    for (const AABB &localBox : wallBoxes)
    {
        AABB w;
        w.min = glm::vec3(1e30f);
        w.max = glm::vec3(-1e30f);
        glm::vec3 corners[8] = {
            {localBox.min.x, localBox.min.y, localBox.min.z}, {localBox.max.x, localBox.min.y, localBox.min.z}, {localBox.min.x, localBox.max.y, localBox.min.z}, {localBox.max.x, localBox.max.y, localBox.min.z}, {localBox.min.x, localBox.min.y, localBox.max.z}, {localBox.max.x, localBox.min.y, localBox.max.z}, {localBox.min.x, localBox.max.y, localBox.max.z}, {localBox.max.x, localBox.max.y, localBox.max.z}};
        for (int i = 0; i < 8; i++)
        {
            glm::vec3 transformed = glm::vec3(model * glm::vec4(corners[i], 1.0f));
            w.min = glm::min(w.min, transformed);
            w.max = glm::max(w.max, transformed);
        }
        worldBoxes.push_back(w);
    }
    return worldBoxes;
}