#include "StaticMesh.h"
#include "ShaderManager.h"
#include "ResourceManager.h"




#include <glm/gtc/type_ptr.hpp>

StaticMesh::StaticMesh(const std::vector<float> &geometry)
    : vao(0), vbo(0), vertexCount(0), vertexData(geometry)
{
}

void StaticMesh::build()
{
    if (vertexData.empty())
        return;

    
    vertexCount = (int)(vertexData.size() / 8);

    
    std::vector<glm::vec3> positions;
    positions.reserve(vertexCount);
    for (size_t i = 0; i < vertexData.size(); i += 8)
    {
        positions.push_back(glm::vec3(vertexData[i], vertexData[i + 1], vertexData[i + 2]));
    }
    initAABBFromVertices(positions);
    

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
    vertexData.clear();
}

void StaticMesh::draw(const glm::mat4 &view, const glm::mat4 &proj, const LightSet & )
{
    GLuint shader = ShaderManager::get("basic");
    glUseProgram(shader);

    glm::mat4 model = getModelMatrix();
    ShaderManager::setMat4(shader, "model", glm::value_ptr(model));
    ShaderManager::setMat4(shader, "view", glm::value_ptr(view));
    ShaderManager::setMat4(shader, "projection", glm::value_ptr(proj));
    ShaderManager::setVec3(shader, "objectColor", color.x, color.y, color.z);

    if (hasTextureEnabled() && getTexture() != nullptr)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getTexture()->id);
        ShaderManager::setInt(shader, "useTexture", 1);
    }
    else
    {
        ShaderManager::setInt(shader, "useTexture", 0);
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

void StaticMesh::drawDepth(GLuint depthShader)
{
    glUseProgram(depthShader);
    glm::mat4 model = getModelMatrix();
    ShaderManager::setMat4(depthShader, "model", glm::value_ptr(model));
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}