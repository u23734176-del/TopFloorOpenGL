#include "GolfHole.h"
#include "../core/ShaderManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>





static void pushVert(std::vector<float> &buf,
                     const glm::vec3 &p, const glm::vec3 &n)
{
    buf.push_back(p.x);
    buf.push_back(p.y);
    buf.push_back(p.z);
    buf.push_back(n.x);
    buf.push_back(n.y);
    buf.push_back(n.z);
}


static glm::vec3 rotY(const glm::vec3 &p, float deg)
{
    float r = glm::radians(deg);
    return glm::vec3(
        cosf(r) * p.x - sinf(r) * p.z,
        p.y,
        sinf(r) * p.x + cosf(r) * p.z);
}

static glm::vec2 norm2(glm::vec2 v)
{
    float L = sqrtf(v.x * v.x + v.y * v.y);
    return (L > 1e-9f) ? v / L : glm::vec2(0.0f);
}





GolfHole::GolfHole()
    : holeWidth(4.0f), holeLength(10.0f), wallHeight(0.3f), wallThick(0.15f), turfColor(0.15f, 0.55f, 0.15f), wallColor(0.25f, 0.18f, 0.08f), flagColor(1.0f, 0.0f, 0.0f), hasSand(false), sandOffset(0.0f), sandSize(1.5f, 1.5f), holeNumber(0), centerlineWidth(0.0f), vao(0), vbo(0), vertexCount(0), sandVao(0), sandVbo(0), sandVertexCount(0), flagVao(0), flagVbo(0), flagVertexCount(0)
{
}



void GolfHole::pushQuad(std::vector<float> &buf,
                        glm::vec3 tl, glm::vec3 tr,
                        glm::vec3 bl, glm::vec3 br,
                        glm::vec3 normal)
{
    pushVert(buf, bl, normal);
    pushVert(buf, br, normal);
    pushVert(buf, tl, normal);
    pushVert(buf, br, normal);
    pushVert(buf, tr, normal);
    pushVert(buf, tl, normal);
}


















void GolfHole::buildRibbon(std::vector<float> &buf,
                           const std::vector<glm::vec2> &pts,
                           float width,
                           bool capStart, bool capEnd)
{
    const int n = (int)pts.size();
    if (n < 2)
        return;

    const float h = width * 0.5f;
    const float wt = wallThick;
    const float wh = wallHeight;
    const glm::vec3 upN(0.0f, 1.0f, 0.0f);

    
    std::vector<glm::vec2> Lc(n), Rc(n); 
    std::vector<glm::vec2> mdir(n);      
    std::vector<float> mfac(n);          

    for (int i = 0; i < n; ++i)
    {
        glm::vec2 ml;
        float factor = 1.0f;
        if (i == 0)
        {
            glm::vec2 d = norm2(pts[1] - pts[0]);
            ml = glm::vec2(-d.y, d.x);
        }
        else if (i == n - 1)
        {
            glm::vec2 d = norm2(pts[i] - pts[i - 1]);
            ml = glm::vec2(-d.y, d.x);
        }
        else
        {
            glm::vec2 d0 = norm2(pts[i] - pts[i - 1]);
            glm::vec2 d1 = norm2(pts[i + 1] - pts[i]);
            glm::vec2 n0(-d0.y, d0.x);
            glm::vec2 n1(-d1.y, d1.x);
            ml = norm2(n0 + n1);
            float dot = ml.x * n0.x + ml.y * n0.y;              
            factor = (fabsf(dot) > 0.2f) ? (1.0f / dot) : 5.0f; 
        }
        mdir[i] = ml;
        mfac[i] = factor;
        Lc[i] = pts[i] + ml * (h * factor);
        Rc[i] = pts[i] - ml * (h * factor);
    }

    auto addBox = [&](glm::vec3 a, glm::vec3 b)
    {
        AABB box;
        box.min = glm::min(a, b);
        box.max = glm::max(a, b);
        
        box.min.y = 0.0f;
        box.max.y = wh;
        wallBoxes.push_back(box);
    };

    
    for (int i = 0; i + 1 < n; ++i)
    {
        glm::vec2 l0 = Lc[i], l1 = Lc[i + 1];
        glm::vec2 r0 = Rc[i], r1 = Rc[i + 1];

        
        
        pushQuad(buf,
                 glm::vec3(l1.x, 0.0f, l1.y),
                 glm::vec3(r1.x, 0.0f, r1.y),
                 glm::vec3(l0.x, 0.0f, l0.y),
                 glm::vec3(r0.x, 0.0f, r0.y),
                 upN);

        
        glm::vec2 lo0 = l0 + mdir[i] * wt;
        glm::vec2 lo1 = l1 + mdir[i + 1] * wt;
        glm::vec2 ro0 = r0 - mdir[i] * wt;
        glm::vec2 ro1 = r1 - mdir[i + 1] * wt;

        
        glm::vec2 fdir = norm2(pts[i + 1] - pts[i]);
        glm::vec3 leftN(-fdir.y, 0.0f, fdir.x);  
        glm::vec3 rightN(fdir.y, 0.0f, -fdir.x); 

        
        
        pushQuad(buf,
                 glm::vec3(lo1.x, wh, lo1.y),
                 glm::vec3(l1.x, wh, l1.y),
                 glm::vec3(lo0.x, wh, lo0.y),
                 glm::vec3(l0.x, wh, l0.y),
                 upN);
        
        pushQuad(buf,
                 glm::vec3(l1.x, wh, l1.y),
                 glm::vec3(l0.x, wh, l0.y),
                 glm::vec3(l1.x, 0.0f, l1.y),
                 glm::vec3(l0.x, 0.0f, l0.y),
                 -leftN);
        
        pushQuad(buf,
                 glm::vec3(lo0.x, wh, lo0.y),
                 glm::vec3(lo1.x, wh, lo1.y),
                 glm::vec3(lo0.x, 0.0f, lo0.y),
                 glm::vec3(lo1.x, 0.0f, lo1.y),
                 leftN);
        addBox(glm::vec3(lo0.x, 0.0f, lo0.y), glm::vec3(l1.x, wh, l1.y));

        
        pushQuad(buf,
                 glm::vec3(r1.x, wh, r1.y),
                 glm::vec3(ro1.x, wh, ro1.y),
                 glm::vec3(r0.x, wh, r0.y),
                 glm::vec3(ro0.x, wh, ro0.y),
                 upN);
        
        pushQuad(buf,
                 glm::vec3(r0.x, wh, r0.y),
                 glm::vec3(r1.x, wh, r1.y),
                 glm::vec3(r0.x, 0.0f, r0.y),
                 glm::vec3(r1.x, 0.0f, r1.y),
                 -rightN);
        
        pushQuad(buf,
                 glm::vec3(ro1.x, wh, ro1.y),
                 glm::vec3(ro0.x, wh, ro0.y),
                 glm::vec3(ro1.x, 0.0f, ro1.y),
                 glm::vec3(ro0.x, 0.0f, ro0.y),
                 rightN);
        addBox(glm::vec3(ro0.x, 0.0f, ro0.y), glm::vec3(r1.x, wh, r1.y));
    }

    
    auto buildCap = [&](glm::vec2 lc, glm::vec2 rc, glm::vec2 outDir)
    {
        glm::vec2 lo = lc + outDir * wt;
        glm::vec2 ro = rc + outDir * wt;
        glm::vec3 capN(outDir.x, 0.0f, outDir.y);
        
        pushQuad(buf,
                 glm::vec3(lo.x, wh, lo.y),
                 glm::vec3(ro.x, wh, ro.y),
                 glm::vec3(lc.x, wh, lc.y),
                 glm::vec3(rc.x, wh, rc.y),
                 upN);
        
        pushQuad(buf,
                 glm::vec3(lo.x, wh, lo.y),
                 glm::vec3(ro.x, wh, ro.y),
                 glm::vec3(lo.x, 0.0f, lo.y),
                 glm::vec3(ro.x, 0.0f, ro.y),
                 capN);
        addBox(glm::vec3(lo.x, 0.0f, lo.y), glm::vec3(rc.x, wh, rc.y));
    };

    if (capStart)
    {
        glm::vec2 outDir = -norm2(pts[1] - pts[0]); 
        buildCap(Lc[0], Rc[0], outDir);
    }
    if (capEnd)
    {
        glm::vec2 outDir = norm2(pts[n - 1] - pts[n - 2]); 
        buildCap(Lc[n - 1], Rc[n - 1], outDir);
    }
}




void GolfHole::buildSegment(std::vector<float> &buf,
                            float cx, float cz,
                            float w, float l, float rotDeg)
{
    glm::vec3 tee = rotY(glm::vec3(0.0f, 0.0f, l * 0.5f), rotDeg);
    glm::vec3 cup = rotY(glm::vec3(0.0f, 0.0f, -l * 0.5f), rotDeg);
    std::vector<glm::vec2> pts = {
        glm::vec2(tee.x + cx, tee.z + cz),
        glm::vec2(cup.x + cx, cup.z + cz),
    };
    buildRibbon(buf, pts, w, true, true);
}

void GolfHole::build()
{
    wallBoxes.clear();

    bool curved = (centerline.size() >= 2);
    float ribW = (centerlineWidth > 0.0f) ? centerlineWidth : holeWidth;

    
    glm::vec2 cup = curved ? centerline.back()
                           : glm::vec2(0.0f, -(holeLength * 0.5f - 0.5f));

    std::vector<float> turfBuf;

    
    if (curved)
    {
        buildRibbon(turfBuf, centerline, ribW, true, true);
    }
    else
    {
        buildSegment(turfBuf, 0.0f, 0.0f, holeWidth, holeLength, 0.0f);
    }

    
    for (const auto &seg : extraSegments)
    {
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    
    if (hasSand)
    {
        std::vector<float> sandBuf;
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        float sx = sandOffset.x;
        float sz = sandOffset.z;
        float sw = sandSize.x * 0.5f;
        float sl = sandSize.y * 0.5f;
        float sy = 0.01f;
        pushQuad(sandBuf,
                 glm::vec3(sx - sw, sy, sz + sl),
                 glm::vec3(sx + sw, sy, sz + sl),
                 glm::vec3(sx - sw, sy, sz - sl),
                 glm::vec3(sx + sw, sy, sz - sl),
                 up);
        sandVertexCount = (int)(sandBuf.size() / 6);

        glGenVertexArrays(1, &sandVao);
        glGenBuffers(1, &sandVbo);
        glBindVertexArray(sandVao);
        glBindBuffer(GL_ARRAY_BUFFER, sandVbo);
        glBufferData(GL_ARRAY_BUFFER,
                     (GLsizeiptr)(sandBuf.size() * sizeof(float)),
                     sandBuf.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    
    {
        float cupX = cup.x;
        float cupZ = cup.y;
        float poleH = 1.2f;
        float py = 0.0f;

        std::vector<float> flagBuf;

        float pt = 0.04f;
        pushQuad(flagBuf,
                 glm::vec3(cupX - pt, py + poleH, cupZ),
                 glm::vec3(cupX + pt, py + poleH, cupZ),
                 glm::vec3(cupX - pt, py, cupZ),
                 glm::vec3(cupX + pt, py, cupZ),
                 glm::vec3(0.0f, 0.0f, 1.0f));

        float fw = 0.6f;
        float fh = 0.4f;
        glm::vec3 flagN(0.0f, 0.0f, 1.0f);
        glm::vec3 ftop(cupX, py + poleH, cupZ + 0.01f);
        glm::vec3 fbot(cupX, py + poleH - fh, cupZ + 0.01f);
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    
    if (curved)
    {
        glm::vec2 lo(1e30f), hi(-1e30f);
        for (const auto &p : centerline)
        {
            lo = glm::min(lo, p);
            hi = glm::max(hi, p);
        }
        float pad = ribW * 0.5f + wallThick;
        aabb.min = glm::vec3(lo.x - pad, 0.0f, lo.y - pad);
        aabb.max = glm::vec3(hi.x + pad, wallHeight + 1.5f, hi.y + pad);
    }
    else
    {
        float hw = (holeWidth * 0.5f) + wallThick;
        float hl = (holeLength * 0.5f) + wallThick;
        aabb.min = glm::vec3(-hw, 0.0f, -hl);
        aabb.max = glm::vec3(hw, wallHeight + 1.5f, hl);
    }
}




static void drawBuffer(GLuint vao, int count,
                       GLuint shader,
                       const glm::mat4 &model,
                       const glm::mat4 &view,
                       const glm::mat4 &proj,
                       const glm::vec3 &col)
{
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &proj[0][0]);
    glUniform3f(glGetUniformLocation(shader, "objectColor"),
                col.x, col.y, col.z);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, count);
    glBindVertexArray(0);
}

void GolfHole::draw(const glm::mat4 &view,
                    const glm::mat4 &proj,
                    const LightSet & )
{
    GLuint shader = ShaderManager::get("basic");
    glUseProgram(shader);

    glm::mat4 model = getModelMatrix();

    drawBuffer(vao, vertexCount, shader, model, view, proj, turfColor);

    if (hasSand && sandVertexCount > 0)
    {
        glm::vec3 sandColor(0.85f, 0.78f, 0.45f);
        drawBuffer(sandVao, sandVertexCount, shader, model, view, proj, sandColor);
    }

    if (flagVertexCount > 0)
    {
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &proj[0][0]);
        glUniform3f(glGetUniformLocation(shader, "objectColor"), 0.8f, 0.8f, 0.8f);
        glBindVertexArray(flagVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
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