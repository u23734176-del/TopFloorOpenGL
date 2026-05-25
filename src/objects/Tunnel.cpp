#include "Tunnel.h"
#include "../core/ShaderManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>

Tunnel::Tunnel()
    : tunnelRadius(1.0f), tunnelLength(2.0f), wallThickness(0.15f)
    , vao(0), vbo(0), vertexCount(0), segments(24)
{}

void Tunnel::build()
{
    
    
    
    

    const float PI     = 3.14159265358979f;
    const float r      = tunnelRadius;
    const float ro     = r + wallThickness;
    const float hl     = tunnelLength * 0.5f;
    int         segs   = segments; 
    int         halfS  = segs;     

    std::vector<float> verts;

    auto pushV = [&](const glm::vec3& p, const glm::vec3& n) {
        verts.push_back(p.x); verts.push_back(p.y); verts.push_back(p.z);
        verts.push_back(n.x); verts.push_back(n.y); verts.push_back(n.z);
    };

    
    for (int i = 0; i < segs; ++i)
    {
        float a0 = (float)i       / segs * 2.0f * PI;
        float a1 = (float)(i + 1) / segs * 2.0f * PI;

        glm::vec3 o00(ro * cosf(a0), ro * sinf(a0), -hl);
        glm::vec3 o10(ro * cosf(a1), ro * sinf(a1), -hl);
        glm::vec3 o01(ro * cosf(a0), ro * sinf(a0),  hl);
        glm::vec3 o11(ro * cosf(a1), ro * sinf(a1),  hl);

        glm::vec3 n0(cosf(a0), sinf(a0), 0.0f);
        glm::vec3 n1(cosf(a1), sinf(a1), 0.0f);

        pushV(o00, n0); pushV(o10, n1); pushV(o01, n0);
        pushV(o10, n1); pushV(o11, n1); pushV(o01, n0);
    }

    
    for (int i = 0; i < segs; ++i)
    {
        float a0 = (float)i       / segs * 2.0f * PI;
        float a1 = (float)(i + 1) / segs * 2.0f * PI;

        glm::vec3 i00(r * cosf(a0), r * sinf(a0), -hl);
        glm::vec3 i10(r * cosf(a1), r * sinf(a1), -hl);
        glm::vec3 i01(r * cosf(a0), r * sinf(a0),  hl);
        glm::vec3 i11(r * cosf(a1), r * sinf(a1),  hl);

        glm::vec3 n0(-cosf(a0), -sinf(a0), 0.0f); 
        glm::vec3 n1(-cosf(a1), -sinf(a1), 0.0f);

        
        pushV(i01, n0); pushV(i10, n1); pushV(i00, n0);
        pushV(i01, n0); pushV(i11, n1); pushV(i10, n1);
    }

    
    glm::vec3 frontN(0.0f, 0.0f,  1.0f);
    glm::vec3 backN (0.0f, 0.0f, -1.0f);

    for (int i = 0; i < segs; ++i)
    {
        float a0 = (float)i       / segs * 2.0f * PI;
        float a1 = (float)(i + 1) / segs * 2.0f * PI;

        
        glm::vec3 fi0(r  * cosf(a0), r  * sinf(a0),  hl);
        glm::vec3 fi1(r  * cosf(a1), r  * sinf(a1),  hl);
        glm::vec3 fo0(ro * cosf(a0), ro * sinf(a0),  hl);
        glm::vec3 fo1(ro * cosf(a1), ro * sinf(a1),  hl);

        pushV(fi0, frontN); pushV(fo1, frontN); pushV(fi1, frontN);
        pushV(fi0, frontN); pushV(fo0, frontN); pushV(fo1, frontN);

        
        glm::vec3 bi0(r  * cosf(a0), r  * sinf(a0), -hl);
        glm::vec3 bi1(r  * cosf(a1), r  * sinf(a1), -hl);
        glm::vec3 bo0(ro * cosf(a0), ro * sinf(a0), -hl);
        glm::vec3 bo1(ro * cosf(a1), ro * sinf(a1), -hl);

        pushV(bi1, backN); pushV(bo1, backN); pushV(bi0, backN);
        pushV(bo1, backN); pushV(bo0, backN); pushV(bi0, backN);
    }

    vertexCount = (int)(verts.size() / 6);

    initAABBFromPrimitive(glm::vec3(ro, ro, hl));

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(verts.size() * sizeof(float)),
                 verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Tunnel::draw(const glm::mat4& view,
                   const glm::mat4& proj,
                   const LightSet&  )
{
    GLuint shader = ShaderManager::get("basic");
    glUseProgram(shader);

    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"),      1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"),       1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &proj[0][0]);
    glUniform3f(glGetUniformLocation(shader, "objectColor"),
                color.x, color.y, color.z);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

void Tunnel::drawDepth(GLuint depthShader)
{
    glUseProgram(depthShader);
    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, &model[0][0]);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}
