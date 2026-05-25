#include "Rock.h"
#include "ShaderManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>

Rock::Rock() : vao(0), vbo(0), vertexCount(0) {}


static float rockNoise(float x, float y, float z)
{
    
    float v = sinf(x * 127.1f + y * 311.7f + z * 74.7f) * 43758.5453f;
    return v - floorf(v);  
}

void Rock::build()
{
    const int   stacks   = 10;
    const int   slices   = 12;
    const float PI       = 3.14159265358979f;
    const float TWO_PI   = 2.0f * PI;
    const float displAmt = 0.18f;   

    std::vector<float> verts;
    verts.reserve(stacks * slices * 6 * 6);

    auto spherePt = [&](float phi, float theta) -> glm::vec3 {
        glm::vec3 p(sinf(phi) * cosf(theta),
                    cosf(phi),
                    sinf(phi) * sinf(theta));
        
        float d = 1.0f + displAmt * (rockNoise(p.x, p.y, p.z) - 0.5f);
        return p * d;
    };

    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            float phi0   = (float)i       / stacks * PI;
            float phi1   = (float)(i + 1) / stacks * PI;
            float theta0 = (float)j       / slices * TWO_PI;
            float theta1 = (float)(j + 1) / slices * TWO_PI;

            glm::vec3 p00 = spherePt(phi0, theta0);
            glm::vec3 p10 = spherePt(phi0, theta1);
            glm::vec3 p01 = spherePt(phi1, theta0);
            glm::vec3 p11 = spherePt(phi1, theta1);

            
            glm::vec3 n0 = glm::normalize(glm::cross(p10 - p00, p01 - p00));
            glm::vec3 n1 = glm::normalize(glm::cross(p01 - p10, p11 - p10));

            auto push = [&](const glm::vec3& p, const glm::vec3& n) {
                verts.push_back(p.x); verts.push_back(p.y); verts.push_back(p.z);
                verts.push_back(n.x); verts.push_back(n.y); verts.push_back(n.z);
            };

            
            push(p00, n0); push(p10, n0); push(p01, n0);
            
            push(p10, n1); push(p11, n1); push(p01, n1);
        }
    }

    vertexCount = (int)(verts.size() / 6);
    initAABBFromPrimitive(glm::vec3(1.2f, 1.2f, 1.2f)); 

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

void Rock::draw(const glm::mat4& view,
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

void Rock::drawDepth(GLuint depthShader)
{
    glUseProgram(depthShader);
    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, &model[0][0]);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}
