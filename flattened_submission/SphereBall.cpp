#include "SphereBall.h"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

SphereBall::SphereBall()
    : vao(0), vbo(0), shader(0), vertexCount(0), stacks(16), slices(24), radius(0.25f)
{
    setScale(glm::vec3(1.0f));
    initAABBFromPrimitive(glm::vec3(radius));
    setColor(glm::vec3(1.0f, 1.0f, 1.0f));
}

void SphereBall::setResolution(int stacks_, int slices_)
{
    if (stacks_ >= 2)
        stacks = stacks_;
    if (slices_ >= 3)
        slices = slices_;
}

void SphereBall::setRadius(float r)
{
    radius = r;
    initAABBFromPrimitive(glm::vec3(radius));
}

float SphereBall::getRadius() const { return radius; }

void SphereBall::build()
{
    shader = LoadShaders("ball.vert", "ball.frag");

    std::vector<float> data;
    data.reserve(stacks * slices * 6 * 6);

    const float PI = 3.14159265358979323846f;

    for (int i = 0; i < stacks; ++i)
    {
        float phi0 = PI * (float)i / (float)stacks;
        float phi1 = PI * (float)(i + 1) / (float)stacks;

        for (int j = 0; j < slices; ++j)
        {
            float theta0 = 2.0f * PI * (float)j / (float)slices;
            float theta1 = 2.0f * PI * (float)(j + 1) / (float)slices;

            glm::vec3 p00(sinf(phi0) * cosf(theta0), cosf(phi0), sinf(phi0) * sinf(theta0));
            glm::vec3 p01(sinf(phi0) * cosf(theta1), cosf(phi0), sinf(phi0) * sinf(theta1));
            glm::vec3 p10(sinf(phi1) * cosf(theta0), cosf(phi1), sinf(phi1) * sinf(theta0));
            glm::vec3 p11(sinf(phi1) * cosf(theta1), cosf(phi1), sinf(phi1) * sinf(theta1));

            auto push = [&](const glm::vec3 &u)
            {
                data.push_back(u.x * radius);
                data.push_back(u.y * radius);
                data.push_back(u.z * radius);
                data.push_back(u.x);
                data.push_back(u.y);
                data.push_back(u.z);
            };

            push(p00);
            push(p10);
            push(p11);

            push(p00);
            push(p11);
            push(p01);
        }
    }

    vertexCount = (int)(data.size() / 6);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void SphereBall::draw(
    const glm::mat4 &view,
    const glm::mat4 &proj,
    const LightSet &lights)
{
    glUseProgram(shader);

    glm::mat4 model = getModelMatrix();

    GLuint modelLoc = glGetUniformLocation(shader, "model");
    GLuint viewLoc = glGetUniformLocation(shader, "view");
    GLuint projLoc = glGetUniformLocation(shader, "projection");
    GLuint colorLoc = glGetUniformLocation(shader, "objectColor");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj[0][0]);
    glUniform3fv(colorLoc, 1, &color[0]);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

void SphereBall::drawDepth(GLuint depthShaderProgram)
{

    glm::mat4 model = getModelMatrix();
    GLuint modelLoc = glGetUniformLocation(depthShaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}