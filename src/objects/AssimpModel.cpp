#include "AssimpModel.h"
#include "../core/ShaderManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

AssimpModel::AssimpModel(const std::string &path)
    : filePath(path), built(false)
{
}

AssimpModel::~AssimpModel()
{
    for (const auto &mesh : subMeshes)
    {
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteVertexArrays(1, &mesh.vao);
    }
}

void AssimpModel::build()
{

    if (built)
        return;
    built = true;

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filePath,
                                             aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "AssimpModel: failed to load '" << filePath << "': "
                  << importer.GetErrorString() << "\n";
        return;
    }

    std::vector<glm::vec3> allPositions;

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
    {
        aiMesh *mesh = scene->mMeshes[m];
        std::vector<float> vertexData;
        vertexData.reserve(mesh->mNumFaces * 3 * 8);

        
        glm::vec3 meshColor(0.5f, 0.5f, 0.5f);
        if (scene->mMaterials && mesh->mMaterialIndex < scene->mNumMaterials)
        {
            aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
            aiColor3D c(0.5f, 0.5f, 0.5f);
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, c);
            meshColor = glm::vec3(c.r, c.g, c.b);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace &face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                unsigned int idx = face.mIndices[j];

                glm::vec3 pos(mesh->mVertices[idx].x,
                              mesh->mVertices[idx].y,
                              mesh->mVertices[idx].z);
                vertexData.push_back(pos.x);
                vertexData.push_back(pos.y);
                vertexData.push_back(pos.z);
                allPositions.push_back(pos);

                if (mesh->HasNormals())
                {
                    vertexData.push_back(mesh->mNormals[idx].x);
                    vertexData.push_back(mesh->mNormals[idx].y);
                    vertexData.push_back(mesh->mNormals[idx].z);
                }
                else
                {
                    vertexData.push_back(0.0f);
                    vertexData.push_back(1.0f);
                    vertexData.push_back(0.0f);
                }

                if (mesh->mTextureCoords[0])
                {
                    vertexData.push_back(mesh->mTextureCoords[0][idx].x);
                    vertexData.push_back(mesh->mTextureCoords[0][idx].y);
                }
                else
                {
                    vertexData.push_back(0.0f);
                    vertexData.push_back(0.0f);
                }
            }
        }

        SubMesh sub;
        sub.vertexCount = (int)(vertexData.size() / 8);
        sub.diffuseColor = meshColor;
        sub.name = mesh->mName.C_Str();

        glGenVertexArrays(1, &sub.vao);
        glGenBuffers(1, &sub.vbo);

        glBindVertexArray(sub.vao);
        glBindBuffer(GL_ARRAY_BUFFER, sub.vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     vertexData.size() * sizeof(float),
                     vertexData.data(),
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        subMeshes.push_back(sub);
    }

    initAABBFromVertices(allPositions);
}void AssimpModel::draw(const glm::mat4 &view, const glm::mat4 &proj, const LightSet & )
{
    GLuint shader = ShaderManager::get("basic");
    glUseProgram(shader);

    glm::mat4 model = getModelMatrix();

    
    ShaderManager::setMat4(shader, "model", glm::value_ptr(model));
    ShaderManager::setMat4(shader, "view", glm::value_ptr(view));
    ShaderManager::setMat4(shader, "projection", glm::value_ptr(proj));
    ShaderManager::setInt(shader, "useTexture", 0);

    for (const auto &sub : subMeshes)
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

void AssimpModel::drawDepth(GLuint depthShader)
{
    glUseProgram(depthShader);
    glm::mat4 model = getModelMatrix();
    ShaderManager::setMat4(depthShader, "model", glm::value_ptr(model));

    for (const auto &sub : subMeshes)
    {
        glBindVertexArray(sub.vao);
        glDrawArrays(GL_TRIANGLES, 0, sub.vertexCount);
    }
    glBindVertexArray(0);
}

void AssimpModel::setSubMeshColor(const std::string &keyword, glm::vec3 color)
{
    for (auto &sub : subMeshes)
    {
        if (sub.name.find(keyword) != std::string::npos)
            sub.diffuseColor = color;
    }
}

void AssimpModel::printSubMeshes() const
{
    std::cout << "Submeshes in " << filePath << ":\n";
    for (const auto &sub : subMeshes)
        std::cout << "  - " << sub.name << "\n";
}