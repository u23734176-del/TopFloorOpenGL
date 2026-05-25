#ifndef TUNNEL_H
#define TUNNEL_H

#include "../core/SceneObject.h"
#include "../core/shader.hpp"





class Tunnel : public SceneObject
{
public:
    float tunnelRadius;   
    float tunnelLength;   
    float wallThickness;  

    Tunnel();

    void build()     override;
    void draw(const glm::mat4& view,
              const glm::mat4& proj,
              const LightSet&  lights) override;
    void drawDepth(GLuint depthShaderProgram) override;

private:
    GLuint vao;
    GLuint vbo;
    int    vertexCount;
    int    segments;
};

#endif
