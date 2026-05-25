#ifndef GOLFHOLE_H
#define GOLFHOLE_H

#include "SceneObject.h"
#include "shader.hpp"

#include <vector>

struct HoleSegment
{
    float offsetX; 
    float offsetZ; 
    float width;
    float length;
    float rotY; 
};

class GolfHole : public SceneObject
{
public:
    
    float holeWidth;      
    float holeLength;     
    float wallHeight;     
    float wallThick;      
    glm::vec3 turfColor;  
    glm::vec3 wallColor;  
    glm::vec3 flagColor;  
    bool hasSand;         
    glm::vec3 sandOffset; 
    glm::vec2 sandSize;   
    int holeNumber;       

    
    std::vector<HoleSegment> extraSegments;

    
    
    
    
    std::vector<glm::vec2> centerline;
    float centerlineWidth; 

    GolfHole();

    void build() override;
    void draw(const glm::mat4 &view,
              const glm::mat4 &proj,
              const LightSet &lights) override;
    void drawDepth(GLuint depthShaderProgram) override;

    std::vector<AABB> getCollisionAABBs() const override;

private:
    
    
    GLuint vao;
    GLuint vbo;
    int vertexCount;

    
    GLuint sandVao;
    GLuint sandVbo;
    int sandVertexCount;

    
    GLuint flagVao;
    GLuint flagVbo;
    int flagVertexCount;

    
    void pushQuad(std::vector<float> &buf,
                  glm::vec3 tl, glm::vec3 tr,
                  glm::vec3 bl, glm::vec3 br,
                  glm::vec3 normal);

    
    void buildSegment(std::vector<float> &turfBuf,
                      float cx, float cz,
                      float w, float l, float rotY);

    
    
    
    void buildRibbon(std::vector<float> &turfBuf,
                     const std::vector<glm::vec2> &pts,
                     float width,
                     bool capStart, bool capEnd);

    std::vector<AABB> wallBoxes;
};

#endif