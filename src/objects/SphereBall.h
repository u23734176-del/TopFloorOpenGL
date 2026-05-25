#ifndef SPHEREBALL_H
#define SPHEREBALL_H
#include "../core/SceneObject.h"
#include "../core/shader.hpp"
#include <vector>

class SphereBall : public SceneObject
{
private:
    GLuint vao;
    GLuint vbo;
    GLuint shader;
    int vertexCount; 

    int stacks;   
    int slices;   
    float radius; 

public:
    SphereBall();

    
    void setResolution(int stacks_, int slices_);
    void setRadius(float r);
    float getRadius() const;

    void build() override;

    void draw(
        const glm::mat4 &view,
        const glm::mat4 &proj,
        const LightSet &lights) override;

    void drawDepth(GLuint depthShaderProgram) override;
};

#endif