#include "Windmill.h"
#include "../core/ShaderManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>

// ---------------------------------------------------------------------------
// Geometry dimensions  (local units — scale with setScale in world space)
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Geometry dimensions  (local units — scale with setScale in world space)
// ---------------------------------------------------------------------------
static const float PI  = 3.14159265358979f;
static const float PI2 = 2.0f * PI;

// NEW: Bottom Base (Cream Octagon)
static const float BOT_BASE_R   = 2.3f;  // Slightly wider than the frustum bottom
static const float BOT_BASE_H   = 0.8f;

// Frustum (barber-pole body)
static const int   STRIPE_COUNT = 4;       // number of stripe pairs (must be even)
static const float FR_BOT       = 2.0f;    // bottom radius
static const float FR_TOP       = 1.0f;    // top radius
static const float FR_H         = 3.5f;    // height

// Skirt brim
static const float SKIRT_INNER  = FR_TOP;
static const float SKIRT_OUTER  = FR_TOP;
static const float SKIRT_Y      = BOT_BASE_H + FR_H; // Shifted up to sit on base!

// NEW: Red Cube underneath the house
static const float RED_CUBE_W   = 1.6f;
static const float RED_CUBE_H   = 0.6f;
static const float RED_CUBE_D   = 1.6f;
static const float RED_CUBE_Y   = SKIRT_Y + 0.01f;

// House collar (dark band)
static const float HBASE_W      = 1.8f;
static const float HBASE_H      = 0.2f; // Made thinner to match reference
static const float HBASE_D      = 1.8f;
static const float HBASE_Y      = RED_CUBE_Y + RED_CUBE_H;

// House walls (white box)
static const float HOUSE_W      = 1.4f;
static const float HOUSE_H      = 1.1f;
static const float HOUSE_D      = 1.4f;
static const float HOUSE_Y      = HBASE_Y + HBASE_H;

// Roof pyramid
static const float ROOF_BASE_W  = 1.55f;
static const float ROOF_BASE_D  = 1.55f;
static const float ROOF_H_V     = 1.0f;
static const float ROOF_Y       = HOUSE_Y + HOUSE_H;

// Axle & rotor
static const float AXLE_R       = 0.06f;
static const float AXLE_LEN     = HOUSE_D + 1.0f;
static const float AXLE_Y_CENT  = HOUSE_Y + HOUSE_H * 0.5f;
static const float AXLE_Z_FRONT = HOUSE_D * 0.5f + 0.6f;

// Blades
static const float BLADE_HALF_L = 1.7f;
static const float BLADE_HALF_W = 0.22f;
static const float BLADE_HALF_T = 0.06f;
static const float BLADE_OFFSET = 0.35f;

// Colours
static const glm::vec3 COL_CREAM (0.93f, 0.89f, 0.75f);
static const glm::vec3 COL_RED   (0.85f, 0.08f, 0.08f);
static const glm::vec3 COL_DARK  (0.15f, 0.15f, 0.18f);
static const glm::vec3 COL_WHITE (0.95f, 0.95f, 0.96f);
static const glm::vec3 COL_BLUE  (0.25f, 0.45f, 0.75f);
static const glm::vec3 COL_BLADE (0.72f, 0.65f, 0.30f);
static const glm::vec3 COL_SKIRT (0.20f, 0.20f, 0.22f);

// ---------------------------------------------------------------------------
// Tiny helpers
// ---------------------------------------------------------------------------
static void pv(std::vector<float>& b, float x, float y, float z,
                                      float nx,float ny,float nz)
{
    b.push_back(x);  b.push_back(y);  b.push_back(z);
    b.push_back(nx); b.push_back(ny); b.push_back(nz);
}

static void tri(std::vector<float>& b,
                float x0,float y0,float z0,
                float x1,float y1,float z1,
                float x2,float y2,float z2,
                float nx,float ny,float nz)
{
    pv(b,x0,y0,z0,nx,ny,nz);
    pv(b,x1,y1,z1,nx,ny,nz);
    pv(b,x2,y2,z2,nx,ny,nz);
}

static void quad(std::vector<float>& b,
                 float x0,float y0,float z0,
                 float x1,float y1,float z1,
                 float x2,float y2,float z2,
                 float x3,float y3,float z3,
                 float nx,float ny,float nz)
{
    tri(b, x0,y0,z0, x1,y1,z1, x2,y2,z2, nx,ny,nz);
    tri(b, x0,y0,z0, x2,y2,z2, x3,y3,z3, nx,ny,nz);
}

// ---------------------------------------------------------------------------
// Windmill
// ---------------------------------------------------------------------------
Windmill::Windmill()
    : skirtVao(0),skirtVbo(0),skirtCount(0)
    , hbaseVao(0),hbaseVbo(0),hbaseCount(0)
    , houseVao(0),houseVbo(0),houseCount(0)
    , winVao(0),winVbo(0),winCount(0)
    , roofVao(0),roofVbo(0),roofCount(0)
    , axleVao(0),axleVbo(0),axleCount(0)
    , bladeVao(0),bladeVbo(0),bladeCount(0)
    , botBaseVao(0),botBaseVbo(0),botBaseCount(0)
    , redCubeVao(0),redCubeVbo(0),redCubeCount(0)
    , frustumTopY(FR_H), houseBaseY(HBASE_Y), houseTopY(HOUSE_Y+HOUSE_H)
    , roofY(ROOF_Y), axleY(AXLE_Y_CENT), axleZ(AXLE_Z_FRONT)
    , bladeOffset(BLADE_OFFSET)
{}

// ---------------------------------------------------------------------------
void Windmill::uploadBuf(const std::vector<float>& buf,
                          GLuint& vao, GLuint& vbo, int& count)
{
    count = (int)(buf.size() / 6);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,(GLsizeiptr)(buf.size()*sizeof(float)),
                 buf.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

// ---------------------------------------------------------------------------
// One trapezoidal frustum panel between angles a0..a1
std::vector<float> Windmill::makeFrustumPanel(float rBot, float rTop, float h,
                                               float a0, float a1)
{
    std::vector<float> b;
    float am = (a0+a1)*0.5f;
    // outward normal: points radially outward and slightly up due to taper
    float slope = (rBot - rTop) / h;
    float nl = sqrtf(1.0f + slope*slope);
    float nx = cosf(am)/nl, nz = sinf(am)/nl, ny = slope/nl;

    float bx0=rBot*cosf(a0), bz0=rBot*sinf(a0);
    float bx1=rBot*cosf(a1), bz1=rBot*sinf(a1);
    float tx0=rTop*cosf(a0), tz0=rTop*sinf(a0);
    float tx1=rTop*cosf(a1), tz1=rTop*sinf(a1);

    // side quad
    quad(b, bx0,0,bz0, bx1,0,bz1, tx1,h,tz1, tx0,h,tz0, nx,ny,nz);

    // bottom cap tri
    tri(b, 0,0,0, bx1,0,bz1, bx0,0,bz0, 0,-1,0);

    // top cap tri
    tri(b, 0,h,0, tx0,h,tz0, tx1,h,tz1, 0,1,0);

    return b;
}

// ---------------------------------------------------------------------------
std::vector<float> Windmill::makeSkirtBuf(float rIn, float rOut, float y)
{
    std::vector<float> b;
    int segs = STRIPE_COUNT * 2;
    for(int i=0;i<segs;++i){
        float a0=(float)i/segs*PI2, a1=(float)(i+1)/segs*PI2;
        float ix0=rIn*cosf(a0), iz0=rIn*sinf(a0);
        float ix1=rIn*cosf(a1), iz1=rIn*sinf(a1);
        float ox0=rOut*cosf(a0), oz0=rOut*sinf(a0);
        float ox1=rOut*cosf(a1), oz1=rOut*sinf(a1);
        // top face (upward normal)
        quad(b, ix0,y,iz0, ox0,y,oz0, ox1,y,oz1, ix1,y,iz1, 0,1,0);
    }
    return b;
}

// ---------------------------------------------------------------------------
std::vector<float> Windmill::makeCuboidBuf(float w, float h, float d)
{
    std::vector<float> b;
    float hw=w*0.5f, hh=h*0.5f, hd=d*0.5f;
    // Front +Z
    quad(b,-hw,-hh, hd, hw,-hh, hd, hw, hh, hd,-hw, hh, hd, 0,0,1);
    // Back -Z
    quad(b, hw,-hh,-hd,-hw,-hh,-hd,-hw, hh,-hd, hw, hh,-hd, 0,0,-1);
    // Left -X
    quad(b,-hw,-hh,-hd,-hw,-hh, hd,-hw, hh, hd,-hw, hh,-hd,-1,0,0);
    // Right +X
    quad(b, hw,-hh, hd, hw,-hh,-hd, hw, hh,-hd, hw, hh, hd, 1,0,0);
    // Top +Y
    quad(b,-hw, hh, hd, hw, hh, hd, hw, hh,-hd,-hw, hh,-hd, 0,1,0);
    // Bottom -Y
    quad(b,-hw,-hh,-hd, hw,-hh,-hd, hw,-hh, hd,-hw,-hh, hd, 0,-1,0);
    return b;
}

// ---------------------------------------------------------------------------
// 4-sided pyramid roof, base centred at y=0
std::vector<float> Windmill::makeRoofBuf(float bw, float bd, float rh)
{
    std::vector<float> b;
    float hw=bw*0.5f, hd=bd*0.5f;
    // apex
    float ax=0, ay=rh, az=0;

    // Front face (+Z)
    {
        glm::vec3 n = glm::normalize(glm::cross(
            glm::vec3(hw,0,hd)-glm::vec3(-hw,0,hd),
            glm::vec3(ax,ay,az)-glm::vec3(-hw,0,hd)));
        tri(b,-hw,0, hd, hw,0, hd, ax,ay,az, n.x,n.y,n.z);
    }
    // Back face (-Z)
    {
        glm::vec3 n = glm::normalize(glm::cross(
            glm::vec3(-hw,0,-hd)-glm::vec3(hw,0,-hd),
            glm::vec3(ax,ay,az)-glm::vec3(hw,0,-hd)));
        tri(b, hw,0,-hd,-hw,0,-hd, ax,ay,az, n.x,n.y,n.z);
    }
    // Left face (-X)
    {
        glm::vec3 n = glm::normalize(glm::cross(
            glm::vec3(-hw,0,-hd)-glm::vec3(-hw,0,hd),
            glm::vec3(ax,ay,az)-glm::vec3(-hw,0,hd)));
        tri(b,-hw,0, hd,-hw,0,-hd, ax,ay,az, n.x,n.y,n.z);
    }
    // Right face (+X)
    {
        glm::vec3 n = glm::normalize(glm::cross(
            glm::vec3(hw,0,hd)-glm::vec3(hw,0,-hd),
            glm::vec3(ax,ay,az)-glm::vec3(hw,0,-hd)));
        tri(b, hw,0,-hd, hw,0, hd, ax,ay,az, n.x,n.y,n.z);
    }
    // Base cap
    quad(b,-hw,0,-hd, hw,0,-hd, hw,0, hd,-hw,0, hd, 0,-1,0);
    return b;
}

// ---------------------------------------------------------------------------
// Cylinder along Z (for axle): radius r, length len, centred at origin
std::vector<float> Windmill::makeCylinderBuf(float r, float len, int segs)
{
    std::vector<float> b;
    float hl = len*0.5f;
    for(int i=0;i<segs;++i){
        float a0=(float)i/segs*PI2, a1=(float)(i+1)/segs*PI2;
        float am=(a0+a1)*0.5f;
        float nx=cosf(am), ny=sinf(am);
        float x0=r*cosf(a0),y0=r*sinf(a0);
        float x1=r*cosf(a1),y1=r*sinf(a1);
        // side quad (z = -hl to +hl)
        quad(b, x0,y0,-hl, x1,y1,-hl, x1,y1, hl, x0,y0, hl, nx,ny,0);
    }
    // end caps
    for(int pass=0;pass<2;++pass){
        float z=(pass==0)?-hl:hl;
        float nz=(pass==0)?-1.0f:1.0f;
        for(int i=0;i<segs;++i){
            float a0=(float)i/segs*PI2, a1=(float)(i+1)/segs*PI2;
            float x0=r*cosf(a0),y0=r*sinf(a0);
            float x1=r*cosf(a1),y1=r*sinf(a1);
            if(pass==0) tri(b, 0,0,z, x1,y1,z, x0,y0,z, 0,0,nz);
            else        tri(b, 0,0,z, x0,y0,z, x1,y1,z, 0,0,nz);
        }
    }
    return b;
}

// ---------------------------------------------------------------------------
// One blade plank: spans y = -halfLen..+halfLen, thin on Z
std::vector<float> Windmill::makeBladeBuf(float hl, float hw, float ht)
{
    std::vector<float> b;
    // Front +Z
    quad(b,-hw,-hl, ht, hw,-hl, ht, hw, hl, ht,-hw, hl, ht, 0,0,1);
    // Back -Z
    quad(b, hw,-hl,-ht,-hw,-hl,-ht,-hw, hl,-ht, hw, hl,-ht, 0,0,-1);
    // Top +Y
    quad(b,-hw, hl, ht, hw, hl, ht, hw, hl,-ht,-hw, hl,-ht, 0,1,0);
    // Bottom -Y
    quad(b,-hw,-hl,-ht, hw,-hl,-ht, hw,-hl, ht,-hw,-hl, ht, 0,-1,0);
    // Left -X
    quad(b,-hw,-hl,-ht,-hw,-hl, ht,-hw, hl, ht,-hw, hl,-ht,-1,0,0);
    // Right +X
    quad(b, hw,-hl, ht, hw,-hl,-ht, hw, hl,-ht, hw, hl, ht, 1,0,0);
    return b;
}

// ---------------------------------------------------------------------------
// Flat window quad (in XY plane, normal +Z)
std::vector<float> Windmill::makeWindowBuf(float w, float h)
{
    std::vector<float> b;
    float hw=w*0.5f, hh=h*0.5f;
    quad(b,-hw,-hh,0, hw,-hh,0, hw, hh,0,-hw, hh,0, 0,0,1);
    return b;
}

// ---------------------------------------------------------------------------
void Windmill::build()
{
    // ---- FRUSTUM STRIPES ----
    // Each stripe is 1 segment wide. We alternate cream/red.
    // Total segments = STRIPE_COUNT * 2 (cream + red alternating)
    int totalSegs = STRIPE_COUNT * 2;
    panels.resize(totalSegs);

    for(int i = 0; i < totalSegs; ++i)
    {
        float a0 = (float)i       / totalSegs * PI2;
        float a1 = (float)(i + 1) / totalSegs * PI2;

        auto buf = makeFrustumPanel(FR_BOT, FR_TOP, FR_H, a0, a1);

        StripePanel& p = panels[i];
        p.isCream = (i % 2 == 0);
        uploadBuf(buf, p.vao, p.vbo, p.count);
    }

    uploadBuf(makeCylinderBuf(BOT_BASE_R, BOT_BASE_H, 8),
              botBaseVao, botBaseVbo, botBaseCount);

    // ... (Keep your existing skirt generation) ...

    // ---- NEW: RED CUBE ----
    uploadBuf(makeCuboidBuf(RED_CUBE_W, RED_CUBE_H, RED_CUBE_D),
              redCubeVao, redCubeVbo, redCubeCount);

    // ---- SKIRT BRIM ----
    uploadBuf(makeSkirtBuf(SKIRT_INNER, SKIRT_OUTER, SKIRT_Y),
              skirtVao, skirtVbo, skirtCount);

    // ---- HOUSE BASE COLLAR ----
    // Cuboid centred at its own midpoint; we'll translate in draw()
    uploadBuf(makeCuboidBuf(HBASE_W, HBASE_H, HBASE_D),
              hbaseVao, hbaseVbo, hbaseCount);

    // ---- HOUSE WALLS ----
    uploadBuf(makeCuboidBuf(HOUSE_W, HOUSE_H, HOUSE_D),
              houseVao, houseVbo, houseCount);

    // ---- ROOF ----
    uploadBuf(makeRoofBuf(ROOF_BASE_W, ROOF_BASE_D, ROOF_H_V),
              roofVao, roofVbo, roofCount);

    // ---- AXLE ----
    uploadBuf(makeCylinderBuf(AXLE_R, AXLE_LEN, 12),
              axleVao, axleVbo, axleCount);

    // ---- BLADE ----
    uploadBuf(makeBladeBuf(BLADE_HALF_L, BLADE_HALF_W, BLADE_HALF_T),
              bladeVao, bladeVbo, bladeCount);

    // ---- WINDOW ----
    uploadBuf(makeWindowBuf(0.30f, 0.28f),
              winVao, winVbo, winCount);

    // AABB
    aabb.min = glm::vec3(-FR_BOT, 0.0f, -FR_BOT);
    aabb.max = glm::vec3( FR_BOT, ROOF_Y + ROOF_H_V + BLADE_HALF_L, FR_BOT);
}

// ---------------------------------------------------------------------------
// draw helpers
// ---------------------------------------------------------------------------
void Windmill::drawOne(GLuint vao, int count,
                        const glm::mat4& mvp,
                        const glm::vec3& col,
                        GLuint shader) const
{
    glUniformMatrix4fv(glGetUniformLocation(shader,"model"),
                       1,GL_FALSE,&mvp[0][0]);
    glUniform3f(glGetUniformLocation(shader,"objectColor"),col.x,col.y,col.z);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES,0,count);
    glBindVertexArray(0);
}

void Windmill::drawOneDepth(GLuint vao, int count,
                             const glm::mat4& model,
                             GLuint shader) const
{
    glUniformMatrix4fv(glGetUniformLocation(shader,"model"),
                       1,GL_FALSE,&model[0][0]);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES,0,count);
    glBindVertexArray(0);
}

// ---------------------------------------------------------------------------
void Windmill::draw(const glm::mat4& view,
                     const glm::mat4& proj,
                     const LightSet& /*lights*/)
{
    GLuint sh = ShaderManager::get("basic");
    glUseProgram(sh);

    // Upload view + projection once — draw() sets model per object
    glUniformMatrix4fv(glGetUniformLocation(sh,"view"),       1,GL_FALSE,&view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sh,"projection"), 1,GL_FALSE,&proj[0][0]);

    glm::mat4 base = getModelMatrix();

    // ---- NEW: Bottom Base ----
    {
        // Translate to half-height to sit flush on y=0
        // Rotate 90 deg on X to stand the cylinder up vertically
        // Rotate 22.5 deg on Z so the flat faces align nicely with the world axes
        glm::mat4 m = base *
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, BOT_BASE_H * 0.5f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(22.5f), glm::vec3(0.0f, 0.0f, 1.0f));
        drawOne(botBaseVao, botBaseCount, m, COL_CREAM, sh);
    }

    // ---- Frustum stripes ----
    // Shift the frustum up by the height of the bottom base
    glm::mat4 frustumBase = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, BOT_BASE_H, 0.0f));
    for(const auto& p : panels)
        drawOne(p.vao, p.count, frustumBase, p.isCream ? COL_CREAM : COL_RED, sh);

    // ---- Skirt ----
    drawOne(skirtVao, skirtCount, base, COL_SKIRT, sh);

    // ---- NEW: Red Cube underneath the house ----
    {
        glm::mat4 m = base *
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, RED_CUBE_Y + RED_CUBE_H * 0.5f, 0.0f));
        drawOne(redCubeVao, redCubeCount, m, COL_RED, sh);
    }

    // ---- House base collar ----
    {
        glm::mat4 m = base *
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, HBASE_Y + HBASE_H*0.5f, 0.0f));
        drawOne(hbaseVao, hbaseCount, m, COL_DARK, sh);
    }

    // ---- House walls ----
    {
        glm::mat4 m = base *
            glm::translate(glm::mat4(1.0f),
                           glm::vec3(0.0f, HOUSE_Y + HOUSE_H*0.5f, 0.0f));
        drawOne(houseVao, houseCount, m, COL_WHITE, sh);
    }

    // ---- Roof — base sits at ROOF_Y ----
    {
        glm::mat4 m = base *
            glm::translate(glm::mat4(1.0f),
                           glm::vec3(0.0f, ROOF_Y, 0.0f));
        drawOne(roofVao, roofCount, m, COL_RED, sh);
    }

    // ---- Windows on front, left, right faces of house ----
    {
        float wy = HOUSE_Y + HOUSE_H*0.5f;
        float eps = 0.005f;

        // Front (+Z)
        {
            glm::mat4 m = base *
                glm::translate(glm::mat4(1.0f),
                               glm::vec3(0.0f, wy, HOUSE_D*0.5f + eps));
            drawOne(winVao, winCount, m, COL_BLUE, sh);
        }
        // Left (-X face) — rotate 90° Y
        {
            glm::mat4 m = base *
                glm::translate(glm::mat4(1.0f),
                               glm::vec3(-HOUSE_W*0.5f - eps, wy, 0.0f)) *
                glm::rotate(glm::mat4(1.0f),
                            glm::radians(-90.0f), glm::vec3(0,1,0));
            drawOne(winVao, winCount, m, COL_BLUE, sh);
        }
        // Right (+X face)
        {
            glm::mat4 m = base *
                glm::translate(glm::mat4(1.0f),
                               glm::vec3( HOUSE_W*0.5f + eps, wy, 0.0f)) *
                glm::rotate(glm::mat4(1.0f),
                            glm::radians(90.0f), glm::vec3(0,1,0));
            drawOne(winVao, winCount, m, COL_BLUE, sh);
        }
    }

    // ---- Axle (cylinder along Z through house, at blade hub height) ----
    {
        // Cylinder is built along Z already; just translate to hub position
        glm::mat4 m = base *
            glm::translate(glm::mat4(1.0f),
                           glm::vec3(0.0f, AXLE_Y_CENT, 0.0f));
        drawOne(axleVao, axleCount, m, COL_DARK, sh);
    }

    // ---- Blades — 4 planks rotating around Z, hub at front of house ----
    {
        float spin = (float)glfwGetTime() * 60.0f; // degrees/sec

        // Hub world position: front face of house, at axle height
        glm::vec3 hubLocal(0.0f, AXLE_Y_CENT, AXLE_Z_FRONT);

        for(int i = 0; i < 4; ++i)
        {
            float angleRad = glm::radians(spin + i * 90.0f);

            // Rotate around local Z (which is the axle axis)
            // Translate blade centre by bladeOffset along its local Y
            // so the hub end is near the axle rather than the centre
            glm::mat4 m = base
                * glm::translate(glm::mat4(1.0f), hubLocal)
                * glm::rotate(glm::mat4(1.0f), angleRad,
                              glm::vec3(0.0f, 0.0f, 1.0f))
                * glm::translate(glm::mat4(1.0f),
                                 glm::vec3(0.0f, BLADE_OFFSET, 0.0f));

            drawOne(bladeVao, bladeCount, m, COL_BLADE, sh);
        }
    }
}

// ---------------------------------------------------------------------------
void Windmill::drawDepth(GLuint depthShader)
{
    glUseProgram(depthShader);
    glm::mat4 base = getModelMatrix();

    // ---- NEW: Bottom Base ----
    {
        glm::mat4 m = base *
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, BOT_BASE_H * 0.5f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(22.5f), glm::vec3(0.0f, 0.0f, 1.0f));
        drawOneDepth(botBaseVao, botBaseCount, m, depthShader);
    }

    // ---- Frustum stripes ----
    // Shifted up by the bottom base height
    glm::mat4 frustumBase = base * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, BOT_BASE_H, 0.0f));
    for(const auto& p : panels)
        drawOneDepth(p.vao, p.count, frustumBase, depthShader);

    // ---- Skirt ----
    drawOneDepth(skirtVao, skirtCount, base, depthShader);

    // ---- NEW: Red Cube underneath the house ----
    {
        glm::mat4 m = base *
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, RED_CUBE_Y + RED_CUBE_H * 0.5f, 0.0f));
        drawOneDepth(redCubeVao, redCubeCount, m, depthShader);
    }

    // ---- House base collar ----
    {
        glm::mat4 m = base * glm::translate(glm::mat4(1.0f),
                                             glm::vec3(0, HBASE_Y + HBASE_H * 0.5f, 0));
        drawOneDepth(hbaseVao, hbaseCount, m, depthShader);
    }
    
    // ---- House walls ----
    {
        glm::mat4 m = base * glm::translate(glm::mat4(1.0f),
                                             glm::vec3(0, HOUSE_Y + HOUSE_H * 0.5f, 0));
        drawOneDepth(houseVao, houseCount, m, depthShader);
    }
    
    // ---- Roof ----
    {
        glm::mat4 m = base * glm::translate(glm::mat4(1.0f),
                                             glm::vec3(0, ROOF_Y, 0));
        drawOneDepth(roofVao, roofCount, m, depthShader);
    }

    // ---- Blades ----
    float spin = (float)glfwGetTime() * 60.0f;
    glm::vec3 hubLocal(0.0f, AXLE_Y_CENT, AXLE_Z_FRONT);
    for(int i = 0; i < 4; ++i)
    {
        float angleRad = glm::radians(spin + i * 90.0f);
        glm::mat4 m = base
            * glm::translate(glm::mat4(1.0f), hubLocal)
            * glm::rotate(glm::mat4(1.0f), angleRad, glm::vec3(0,0,1))
            * glm::translate(glm::mat4(1.0f), glm::vec3(0,BLADE_OFFSET,0));
        drawOneDepth(bladeVao, bladeCount, m, depthShader);
    }
}