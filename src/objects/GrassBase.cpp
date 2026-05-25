#include "GrassBase.h"
#include "../core/ShaderManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>

GrassBase::GrassBase()
    : radiusX(6.0f), radiusZ(8.0f), segments(24)
    , vao(0), vbo(0), vertexCount(0)
{}

void GrassBase::build()
{
    const float PI2 = 2.0f * 3.14159265f;
    const glm::vec3 up(0.0f, 1.0f, 0.0f);

    std::vector<float> buf;
    buf.reserve(segments * 3 * 6);

    // Triangle fan from centre
    for (int i = 0; i < segments; ++i)
    {
        float a0 = (float)i       / segments * PI2;
        float a1 = (float)(i + 1) / segments * PI2;

        glm::vec3 centre(0.0f, 0.0f, 0.0f);
        glm::vec3 p0(radiusX * cosf(a0), 0.0f, radiusZ * sinf(a0));
        glm::vec3 p1(radiusX * cosf(a1), 0.0f, radiusZ * sinf(a1));

        // push centre, p0, p1
        auto pv = [&](const glm::vec3& p) {
            buf.push_back(p.x); buf.push_back(p.y); buf.push_back(p.z);
            buf.push_back(up.x); buf.push_back(up.y); buf.push_back(up.z);
        };
        pv(centre); pv(p0); pv(p1);
    }

    vertexCount = (int)(buf.size() / 6);
    initAABBFromPrimitive(glm::vec3(radiusX, 0.05f, radiusZ));

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(buf.size() * sizeof(float)),
                 buf.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void GrassBase::draw(const glm::mat4& view,
                      const glm::mat4& proj,
                      const LightSet& /*lights*/)
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

void GrassBase::drawDepth(GLuint depthShader)
{
    glUseProgram(depthShader);
    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, &model[0][0]);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}
