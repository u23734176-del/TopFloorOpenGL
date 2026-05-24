#include "AssimpModel.h"
#include "../core/ShaderManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

AssimpModel::AssimpModel(const std::string &path)
    : filePath(path) {}

AssimpModel::~AssimpModel()
{
    // Clean up all the VRAM we allocated
    for (const auto &mesh : subMeshes)
    {
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteVertexArrays(1, &mesh.vao);
    }
}

void AssimpModel::build()
{
    Assimp::Importer importer;
    // aiProcess_Triangulate ensures all faces are triangles
    const aiScene *scene = importer.ReadFile(filePath,
                                             aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    std::vector<glm::vec3> allPositions; // For the master AABB

    // Loop through EVERY sub-mesh in the .obj file
    for (unsigned int m = 0; m < scene->mNumMeshes; m++)
    {
        aiMesh *mesh = scene->mMeshes[m];
        std::vector<float> vertexData;

        // 1. Extract Diffuse Color from the .mtl file
        glm::vec3 meshColor(0.5f, 0.5f, 0.5f); // Default grey
        if (scene->mMaterials)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            aiColor3D color(0.5f, 0.5f, 0.5f);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            meshColor = glm::vec3(color.r, color.g, color.b);
        }

        // 2. Unroll the faces for this specific sub-mesh
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                unsigned int idx = face.mIndices[j];

                // Position
                glm::vec3 pos(mesh->mVertices[idx].x, mesh->mVertices[idx].y, mesh->mVertices[idx].z);
                vertexData.push_back(pos.x);
                vertexData.push_back(pos.y);
                vertexData.push_back(pos.z);
                allPositions.push_back(pos);

                // Normal
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

                // UVs (kept for stride consistency)
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

        // 3. Generate VAO/VBO for this sub-mesh
        SubMesh sub;
        sub.vertexCount = (int)(vertexData.size() / 8);
        sub.diffuseColor = meshColor;

        glGenVertexArrays(1, &sub.vao);
        glGenBuffers(1, &sub.vbo);

        glBindVertexArray(sub.vao);
        glBindBuffer(GL_ARRAY_BUFFER, sub.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // ... inside the for (m = 0; m < scene->mNumMeshes) loop ...

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Store the name Assimp read from the .obj file
        sub.name = mesh->mName.C_Str();

        // Store it
        subMeshes.push_back(sub);
    }

    // Calculate the bounding box for the entire combined object
    initAABBFromVertices(allPositions);
}

void AssimpModel::draw(const glm::mat4 &view, const glm::mat4 &proj, const LightSet & /*lights*/)
{
    GLuint shader = ShaderManager::get("basic");
    glUseProgram(shader);

    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &proj[0][0]);

    // Loop through and draw each sub-mesh with its own color
    for (const auto &sub : subMeshes)
    {
        // We override the base SceneObject color with the material color from the .mtl
        glUniform3f(glGetUniformLocation(shader, "objectColor"), sub.diffuseColor.x, sub.diffuseColor.y, sub.diffuseColor.z);

        glBindVertexArray(sub.vao);
        glDrawArrays(GL_TRIANGLES, 0, sub.vertexCount);
    }
    glBindVertexArray(0);
}

void AssimpModel::drawDepth(GLuint depthShader)
{
    glUseProgram(depthShader);
    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, &model[0][0]);

    for (const auto &sub : subMeshes)
    {
        glBindVertexArray(sub.vao);
        glDrawArrays(GL_TRIANGLES, 0, sub.vertexCount);
    }
    glBindVertexArray(0);
}

// Searches for a keyword in the mesh name and colours it
void AssimpModel::setSubMeshColor(const std::string &keyword, glm::vec3 color)
{
    for (auto &sub : subMeshes)
    {
        // If the submesh name contains the keyword
        if (sub.name.find(keyword) != std::string::npos)
        {
            sub.diffuseColor = color;
        }
    }
}

// Helpful utility so you know what names to target for your other objects
void AssimpModel::printSubMeshes() const
{
    std::cout << "Submeshes in " << filePath << ":\n";
    for (const auto &sub : subMeshes)
    {
        std::cout << " - " << sub.name << "\n";
    }
}