#include "StaticMesh.h"
#include "../core/ShaderManager.h"

StaticMesh::StaticMesh(const std::vector<float> &geometry)
    : vao(0), vbo(0), vertexCount(0), vertexData(geometry)
{
}

void StaticMesh::build()
{
    if (vertexData.empty())
        return;

    // 8 floats per vertex: [x, y, z, nx, ny, nz, u, v]
    vertexCount = (int)(vertexData.size() / 8);

    // --- NEW: Extract the glm::vec3 positions to satisfy the AABB function ---
    std::vector<glm::vec3> positions;
    positions.reserve(vertexCount);
    for (size_t i = 0; i < vertexData.size(); i += 8)
    {
        positions.push_back(glm::vec3(vertexData[i], vertexData[i + 1], vertexData[i + 2]));
    }
    initAABBFromVertices(positions);
    // -------------------------------------------------------------------------

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // Attribute 0: Position (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Attribute 1: Normal (nx, ny, nz)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Attribute 2: TexCoords (u, v)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Clear the vector from RAM since it's now living on the GPU VRAM
    vertexData.clear();
}

void StaticMesh::draw(const glm::mat4 &view, const glm::mat4 &proj, const LightSet & /*lights*/)
{
    GLuint shader = ShaderManager::get("basic");
    glUseProgram(shader);

    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &proj[0][0]);
    glUniform3f(glGetUniformLocation(shader, "objectColor"), color.x, color.y, color.z);

    // Texture binding hook for when Slice C is integrated
    if (hasTextureEnabled() && getTexture() != nullptr)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getTexture()->id);
        // Note: Make sure "useTexture" is added to basic.frag later to toggle sampling
        glUniform1i(glGetUniformLocation(shader, "useTexture"), 1);
    }
    else
    {
        glUniform1i(glGetUniformLocation(shader, "useTexture"), 0);
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

void StaticMesh::drawDepth(GLuint depthShader)
{
    glUseProgram(depthShader);
    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}