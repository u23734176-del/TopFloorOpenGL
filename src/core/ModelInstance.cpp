#include "ModelInstance.h"
#include "../core/ShaderManager.h"
#include <glm/gtc/type_ptr.hpp>

ModelInstance::ModelInstance(AssimpModel *sharedMesh)
    : mesh(sharedMesh)
{
}

void ModelInstance::build()
{
    // Geometry already uploaded by AssimpModel::build() via ResourceManager.
    // Copy the shared mesh's local AABB so collision/culling works correctly.
    if (mesh)
    {
        AABB shared = mesh->getLocalAABB();
        initAABBFromPrimitive((shared.max - shared.min) * 0.5f);
    }
}

void ModelInstance::draw(const glm::mat4 &view,
                         const glm::mat4 &proj,
                         const LightSet & /*lights*/)
{
    if (!mesh)
        return;

    GLuint shader = ShaderManager::get("basic");
    glUseProgram(shader);

    // Upload THIS instance's model matrix — not the shared mesh's transform
    glm::mat4 model = getModelMatrix();
    ShaderManager::setMat4(shader, "model", glm::value_ptr(model));
    ShaderManager::setMat4(shader, "view", glm::value_ptr(view));
    ShaderManager::setMat4(shader, "projection", glm::value_ptr(proj));
    ShaderManager::setInt(shader, "useTexture", 0);

    // Draw each submesh with its material colour
    for (const SubMesh &sub : mesh->getSubMeshes())
    {
        ShaderManager::setVec3(shader, "objectColor",
                               sub.diffuseColor.x,
                               sub.diffuseColor.y,
                               sub.diffuseColor.z);
        glBindVertexArray(sub.vao);
        glDrawArrays(GL_TRIANGLES, 0, sub.vertexCount);
    }
    glBindVertexArray(0);
}

void ModelInstance::drawDepth(GLuint depthShader)
{
    if (!mesh)
        return;

    glUseProgram(depthShader);
    glm::mat4 model = getModelMatrix();
    ShaderManager::setMat4(depthShader, "model", glm::value_ptr(model));

    for (const SubMesh &sub : mesh->getSubMeshes())
    {
        glBindVertexArray(sub.vao);
        glDrawArrays(GL_TRIANGLES, 0, sub.vertexCount);
    }
    glBindVertexArray(0);
}