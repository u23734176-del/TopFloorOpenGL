#include "Cone.h"
#include "../core/ShaderManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>

Cone::Cone(int segs)
    : vao(0), vbo(0), vertexCount(0), segments(segs)
{}

void Cone::build()
{
    std::vector<float> verts;
    verts.reserve(segments * 2 * 3 * 6);

    const float PI     = 3.14159265358979f;
    const float TWO_PI = 2.0f * PI;

    glm::vec3 apex(0.0f, 1.0f, 0.0f);
    
    const float slopeY = 0.70710678f;  
    const float slopeR = 0.70710678f;  

    for (int i = 0; i < segments; ++i)
    {
        float a0 = (float)i       / segments * TWO_PI;
        float a1 = (float)(i + 1) / segments * TWO_PI;
        float aMid = (a0 + a1) * 0.5f;

        glm::vec3 base0(cosf(a0), -1.0f, sinf(a0));
        glm::vec3 base1(cosf(a1), -1.0f, sinf(a1));

        
        glm::vec3 n0(slopeR * cosf(a0),   slopeY, slopeR * sinf(a0));
        glm::vec3 n1(slopeR * cosf(a1),   slopeY, slopeR * sinf(a1));
        glm::vec3 nApex(slopeR * cosf(aMid), slopeY, slopeR * sinf(aMid));

        auto push = [&](const glm::vec3& p, const glm::vec3& n) {
            verts.push_back(p.x); verts.push_back(p.y); verts.push_back(p.z);
            verts.push_back(n.x); verts.push_back(n.y); verts.push_back(n.z);
        };

        push(apex,  nApex);
        push(base0, n0);
        push(base1, n1);
    }

    
    {
        glm::vec3 centre(0.0f, -1.0f, 0.0f);
        glm::vec3 norm(0.0f, -1.0f, 0.0f);
        for (int i = 0; i < segments; ++i)
        {
            float a0 = (float)i       / segments * TWO_PI;
            float a1 = (float)(i + 1) / segments * TWO_PI;
            glm::vec3 p0(cosf(a0), -1.0f, sinf(a0));
            glm::vec3 p1(cosf(a1), -1.0f, sinf(a1));

            auto push3 = [&](const glm::vec3& p) {
                verts.push_back(p.x); verts.push_back(p.y); verts.push_back(p.z);
                verts.push_back(norm.x); verts.push_back(norm.y); verts.push_back(norm.z);
            };
            push3(centre); push3(p1); push3(p0);
        }
    }

    vertexCount = (int)(verts.size() / 6);

    initAABBFromPrimitive(glm::vec3(1.0f, 1.0f, 1.0f));

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(verts.size() * sizeof(float)),
                 verts.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Cone::draw(const glm::mat4& view,
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

void Cone::drawDepth(GLuint depthShader)
{
    glUseProgram(depthShader);
    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, &model[0][0]);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}
