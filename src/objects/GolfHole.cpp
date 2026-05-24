#include "GolfHole.h"
#include "../core/ShaderManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void pushVert(std::vector<float>& buf,
                     const glm::vec3& p, const glm::vec3& n)
{
    buf.push_back(p.x); buf.push_back(p.y); buf.push_back(p.z);
    buf.push_back(n.x); buf.push_back(n.y); buf.push_back(n.z);
}

// Rotate a point around the Y axis (degrees)
static glm::vec3 rotY(const glm::vec3& p, float deg)
{
    float r = glm::radians(deg);
    return glm::vec3(
        cosf(r) * p.x - sinf(r) * p.z,
        p.y,
        sinf(r) * p.x + cosf(r) * p.z
    );
}

// ---------------------------------------------------------------------------
// GolfHole
// ---------------------------------------------------------------------------

GolfHole::GolfHole()
    : holeWidth(4.0f), holeLength(10.0f)
    , wallHeight(0.3f), wallThick(0.15f)
    , turfColor(0.15f, 0.55f, 0.15f)
    , wallColor(0.25f, 0.18f, 0.08f)
    , flagColor(1.0f, 0.0f, 0.0f)
    , hasSand(false)
    , sandOffset(0.0f), sandSize(1.5f, 1.5f)
    , holeNumber(0)
    , vao(0), vbo(0), vertexCount(0)
    , sandVao(0), sandVbo(0), sandVertexCount(0)
    , flagVao(0), flagVbo(0), flagVertexCount(0)
{}

// Push one axis-aligned quad (two triangles) with given normal
void GolfHole::pushQuad(std::vector<float>& buf,
                         glm::vec3 tl, glm::vec3 tr,
                         glm::vec3 bl, glm::vec3 br,
                         glm::vec3 normal)
{
    // tri 1: bl, br, tl
    pushVert(buf, bl, normal);
    pushVert(buf, br, normal);
    pushVert(buf, tl, normal);
    // tri 2: br, tr, tl
    pushVert(buf, br, normal);
    pushVert(buf, tr, normal);
    pushVert(buf, tl, normal);
}

// Build one rectangular segment (floor + 4 walls) rotated by rotY degrees
void GolfHole::buildSegment(std::vector<float>& buf,
                              float cx, float cz,
                              float w,  float l, float rotDeg)
{
    float hw = w * 0.5f;
    float hl = l * 0.5f;
    float wt = wallThick;
    float wh = wallHeight;

    glm::vec3 upN(0.0f, 1.0f, 0.0f);

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

    // Calculate hitboxes for the 4 walls
    addWallBox(glm::vec3(-hw - wt, 0.0f, -hl), glm::vec3(-hw, wh, hl));           // Left
    addWallBox(glm::vec3(hw, 0.0f, -hl), glm::vec3(hw + wt, wh, hl));             // Right
    addWallBox(glm::vec3(-hw - wt, 0.0f, -hl - wt), glm::vec3(hw + wt, wh, -hl)); // Back
    addWallBox(glm::vec3(-hw - wt, 0.0f, hl), glm::vec3(hw + wt, wh, hl + wt));   // Front

    // ---- FLOOR ----
    // corners in local segment space, y = 0
    glm::vec3 fbl(-hw,  0.0f, -hl);
    glm::vec3 fbr( hw,  0.0f, -hl);
    glm::vec3 ftl(-hw,  0.0f,  hl);
    glm::vec3 ftr( hw,  0.0f,  hl);

    auto applyOffset = [&](glm::vec3 p) {
        p = rotY(p, rotDeg);
        p.x += cx; p.z += cz;
        return p;
    };

    pushQuad(buf,
             applyOffset(ftl), applyOffset(ftr),
             applyOffset(fbl), applyOffset(fbr),
             upN);

    // ---- WALLS ----
    // Left wall  (-X side)
    {
        glm::vec3 wbl(-hw - wt, 0.0f,  -hl);
        glm::vec3 wbr(-hw,      0.0f,  -hl);
        glm::vec3 wtl(-hw - wt,   wh,  -hl);
        glm::vec3 wtr(-hw,         wh,  -hl);
        // We need two quads: the front face (facing -Z) is enough —
        // actually we want the 4-sided box strip along Z.
        // Simplification: build top face of wall + one inner face.

        // Wall top
        glm::vec3 wbl2(-hw - wt, wh,  hl);
        glm::vec3 wbr2(-hw,       wh,  hl);
        // wall top quad (facing up)
        pushQuad(buf,
                 applyOffset(glm::vec3(-hw-wt, wh,  hl)),
                 applyOffset(glm::vec3(-hw,    wh,  hl)),
                 applyOffset(glm::vec3(-hw-wt, wh, -hl)),
                 applyOffset(glm::vec3(-hw,    wh, -hl)),
                 upN);
        // wall outer face (facing -X)
        glm::vec3 outerN = rotY(glm::vec3(-1.0f, 0.0f, 0.0f), rotDeg);
        pushQuad(buf,
                 applyOffset(glm::vec3(-hw-wt, wh,  hl)),
                 applyOffset(glm::vec3(-hw-wt, wh, -hl)),
                 applyOffset(glm::vec3(-hw-wt, 0.0f,  hl)),
                 applyOffset(glm::vec3(-hw-wt, 0.0f, -hl)),
                 outerN);
        // wall inner face (facing +X)
        glm::vec3 innerN = rotY(glm::vec3(1.0f, 0.0f, 0.0f), rotDeg);
        pushQuad(buf,
                 applyOffset(glm::vec3(-hw, wh, -hl)),
                 applyOffset(glm::vec3(-hw, wh,  hl)),
                 applyOffset(glm::vec3(-hw, 0.0f, -hl)),
                 applyOffset(glm::vec3(-hw, 0.0f,  hl)),
                 innerN);
    }

    // Right wall (+X side)
    {
        glm::vec3 outerN = rotY(glm::vec3(1.0f, 0.0f, 0.0f), rotDeg);
        glm::vec3 innerN = rotY(glm::vec3(-1.0f, 0.0f, 0.0f), rotDeg);
        // wall top quad
        pushQuad(buf,
                 applyOffset(glm::vec3(hw,    wh,  hl)),
                 applyOffset(glm::vec3(hw+wt, wh,  hl)),
                 applyOffset(glm::vec3(hw,    wh, -hl)),
                 applyOffset(glm::vec3(hw+wt, wh, -hl)),
                 upN);
        // outer face
        pushQuad(buf,
                 applyOffset(glm::vec3(hw+wt, wh, -hl)),
                 applyOffset(glm::vec3(hw+wt, wh,  hl)),
                 applyOffset(glm::vec3(hw+wt, 0.0f, -hl)),
                 applyOffset(glm::vec3(hw+wt, 0.0f,  hl)),
                 outerN);
        // inner face
        pushQuad(buf,
                 applyOffset(glm::vec3(hw, wh,  hl)),
                 applyOffset(glm::vec3(hw, wh, -hl)),
                 applyOffset(glm::vec3(hw, 0.0f,  hl)),
                 applyOffset(glm::vec3(hw, 0.0f, -hl)),
                 innerN);
    }

    // Back wall (-Z)
    {
        glm::vec3 outerN = rotY(glm::vec3(0.0f, 0.0f, -1.0f), rotDeg);
        glm::vec3 innerN = rotY(glm::vec3(0.0f, 0.0f,  1.0f), rotDeg);
        pushQuad(buf,
                 applyOffset(glm::vec3(-hw-wt, wh, -hl)),
                 applyOffset(glm::vec3( hw+wt, wh, -hl)),
                 applyOffset(glm::vec3(-hw-wt, 0.0f, -hl)),
                 applyOffset(glm::vec3( hw+wt, 0.0f, -hl)),
                 outerN);
        pushQuad(buf,
                 applyOffset(glm::vec3( hw, wh, -hl+wt)),
                 applyOffset(glm::vec3(-hw, wh, -hl+wt)),
                 applyOffset(glm::vec3( hw, 0.0f, -hl+wt)),
                 applyOffset(glm::vec3(-hw, 0.0f, -hl+wt)),
                 innerN);
    }

    // Front wall (+Z)
    {
        glm::vec3 outerN = rotY(glm::vec3(0.0f, 0.0f, 1.0f), rotDeg);
        glm::vec3 innerN = rotY(glm::vec3(0.0f, 0.0f, -1.0f), rotDeg);
        pushQuad(buf,
                 applyOffset(glm::vec3( hw+wt, wh, hl)),
                 applyOffset(glm::vec3(-hw-wt, wh, hl)),
                 applyOffset(glm::vec3( hw+wt, 0.0f, hl)),
                 applyOffset(glm::vec3(-hw-wt, 0.0f, hl)),
                 outerN);
        pushQuad(buf,
                 applyOffset(glm::vec3(-hw, wh, hl-wt)),
                 applyOffset(glm::vec3( hw, wh, hl-wt)),
                 applyOffset(glm::vec3(-hw, 0.0f, hl-wt)),
                 applyOffset(glm::vec3( hw, 0.0f, hl-wt)),
                 innerN);
    }
}

void GolfHole::build()
{
    wallBoxes.clear();
    
    // ==== MAIN TURF + WALLS ====
    std::vector<float> turfBuf;
    buildSegment(turfBuf, 0.0f, 0.0f, holeWidth, holeLength, 0.0f);
    for (const auto& seg : extraSegments) {
        buildSegment(turfBuf, seg.offsetX, seg.offsetZ,
                     seg.width, seg.length, seg.rotY);
    }

    vertexCount = (int)(turfBuf.size() / 6);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(turfBuf.size() * sizeof(float)),
                 turfBuf.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // ==== SAND PATCH ====
    if (hasSand) {
        std::vector<float> sandBuf;
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        float sx = sandOffset.x;
        float sz = sandOffset.z;
        float sw = sandSize.x * 0.5f;
        float sl = sandSize.y * 0.5f;
        float sy = 0.01f; // tiny offset above turf to prevent z-fight
        pushQuad(sandBuf,
                 glm::vec3(sx-sw, sy,  sz+sl),
                 glm::vec3(sx+sw, sy,  sz+sl),
                 glm::vec3(sx-sw, sy,  sz-sl),
                 glm::vec3(sx+sw, sy,  sz-sl),
                 up);
        sandVertexCount = (int)(sandBuf.size() / 6);

        glGenVertexArrays(1, &sandVao);
        glGenBuffers(1, &sandVbo);
        glBindVertexArray(sandVao);
        glBindBuffer(GL_ARRAY_BUFFER, sandVbo);
        glBufferData(GL_ARRAY_BUFFER,
                     (GLsizeiptr)(sandBuf.size() * sizeof(float)),
                     sandBuf.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    // ==== FLAG (pole + triangle) ====
    // Cup at back end of main segment (-Z), flag at centre of cup area.
    // Pole goes from y=0 to y=1.2 (scaled by scene).
    {
        float cupX = 0.0f;
        float cupZ = -(holeLength * 0.5f - 0.5f); // near the back end
        float poleH = 1.2f;
        float py    = 0.0f; // base of pole at floor level

        std::vector<float> flagBuf;
        glm::vec3 poleN(1.0f, 0.0f, 0.0f); // arbitrary — it's a thin line

        // Pole: a thin quad (2 triangles) — very thin on X, full height on Y
        float pt = 0.04f;
        pushQuad(flagBuf,
                 glm::vec3(cupX-pt, py + poleH, cupZ),
                 glm::vec3(cupX+pt, py + poleH, cupZ),
                 glm::vec3(cupX-pt, py,          cupZ),
                 glm::vec3(cupX+pt, py,          cupZ),
                 glm::vec3(0.0f, 0.0f, 1.0f));

        // Flag triangle (two tris forming a right triangle hanging from top)
        float fw = 0.6f;  // flag width
        float fh = 0.4f;  // flag height
        glm::vec3 flagN(0.0f, 0.0f, 1.0f);
        glm::vec3 ftop(cupX, py + poleH,        cupZ + 0.01f);
        glm::vec3 fbot(cupX, py + poleH - fh,   cupZ + 0.01f);
        glm::vec3 ftip(cupX + fw, py + poleH - fh * 0.5f, cupZ + 0.01f);

        pushVert(flagBuf, ftop, flagN);
        pushVert(flagBuf, fbot, flagN);
        pushVert(flagBuf, ftip, flagN);

        flagVertexCount = (int)(flagBuf.size() / 6);

        glGenVertexArrays(1, &flagVao);
        glGenBuffers(1, &flagVbo);
        glBindVertexArray(flagVao);
        glBindBuffer(GL_ARRAY_BUFFER, flagVbo);
        glBufferData(GL_ARRAY_BUFFER,
                     (GLsizeiptr)(flagBuf.size() * sizeof(float)),
                     flagBuf.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    // AABB covers the whole hole (approximate)
    float hw = (holeWidth  * 0.5f) + wallThick;
    float hl = (holeLength * 0.5f) + wallThick;
    aabb.min = glm::vec3(-hw, 0.0f, -hl);
    aabb.max = glm::vec3( hw, wallHeight + 1.5f, hl);
}

// ---------------------------------------------------------------------------
// draw helpers
// ---------------------------------------------------------------------------
static void drawBuffer(GLuint vao, int count,
                       GLuint shader,
                       const glm::mat4& model,
                       const glm::mat4& view,
                       const glm::mat4& proj,
                       const glm::vec3& col)
{
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"),      1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"),       1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &proj[0][0]);
    glUniform3f(glGetUniformLocation(shader, "objectColor"),
                col.x, col.y, col.z);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, count);
    glBindVertexArray(0);
}

void GolfHole::draw(const glm::mat4& view,
                     const glm::mat4& proj,
                     const LightSet&  /*lights*/)
{
    GLuint shader = ShaderManager::get("basic");
    glUseProgram(shader);

    glm::mat4 model = getModelMatrix();

    // turf + walls (turf colour for floor, wall colour for walls)
    // We drew floor first, then walls — but they're in one buffer with one
    // color. Split into two buffers in a future pass; for now use turfColor
    // which makes walls slightly different via the normal-based shading.
    drawBuffer(vao, vertexCount, shader, model, view, proj, turfColor);

    if (hasSand && sandVertexCount > 0) {
        glm::vec3 sandColor(0.85f, 0.78f, 0.45f);
        drawBuffer(sandVao, sandVertexCount, shader, model, view, proj, sandColor);
    }

    if (flagVertexCount > 0) {
        // pole: grey/white
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"),      1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"),       1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &proj[0][0]);
        // pole is first 6 verts (one quad = 6 verts)
        glUniform3f(glGetUniformLocation(shader, "objectColor"), 0.8f, 0.8f, 0.8f);
        glBindVertexArray(flagVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // flag triangle
        glUniform3f(glGetUniformLocation(shader, "objectColor"),
                    flagColor.x, flagColor.y, flagColor.z);
        glDrawArrays(GL_TRIANGLES, 6, flagVertexCount - 6);
        glBindVertexArray(0);
    }
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

        // Transform local wall box to world space
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
