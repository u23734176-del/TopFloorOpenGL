#ifndef WATER_H
#define WATER_H

#include "SceneObject.h"
#include "shader.hpp"

class Water : public SceneObject
{
private:
    GLuint vao;
    GLuint vbo;
    GLuint shader;
    int vertexCount;

    float sizeX;     
    float sizeZ;     
    float alpha;     
    float shininess; 

public:
    Water();

    void setSize(float halfX, float halfZ);
    void setAlpha(float a);
    void setShininess(float s);

    void build() override;
    void draw(const glm::mat4 &view,
              const glm::mat4 &proj,
              const LightSet &lights) override;
    void drawDepth(GLuint depthShaderProgram) override;
    bool isTransparent() const override { return true; }
};

#endif