#ifndef WINDMILL_H
#define WINDMILL_H

#include "SceneObject.h"
#include "shader.hpp"

#include <vector>



class Windmill : public SceneObject
{
public:
    Windmill();

    void update(float dt); 

    void build()     override;
    void draw(const glm::mat4& view,
              const glm::mat4& proj,
              const LightSet&  lights) override;
    void drawDepth(GLuint depthShaderProgram) override;

private:
    
    struct StripePanel {
        GLuint vao, vbo;
        int    count;
        bool   isCream;   
    };
    std::vector<StripePanel> panels;

    GLuint botBaseVao, botBaseVbo;
    int botBaseCount;

    GLuint redCubeVao, redCubeVbo;
    int redCubeCount;

    
    GLuint skirtVao, skirtVbo;
    int    skirtCount;

    
    GLuint hbaseVao, hbaseVbo;
    int    hbaseCount;

    
    GLuint houseVao, houseVbo;
    int    houseCount;

    
    GLuint winVao, winVbo;
    int    winCount;

    
    GLuint roofVao, roofVbo;
    int    roofCount;

    
    GLuint axleVao, axleVbo;
    int    axleCount;

    
    GLuint bladeVao, bladeVbo;
    int    bladeCount;

    
    float frustumTopY;    
    float houseBaseY;     
    float houseTopY;      
    float roofY;          
    float axleY;          
    float axleZ;          
    float bladeOffset;    

    
    void uploadBuf(const std::vector<float>& buf, GLuint& vao, GLuint& vbo, int& count);

    void drawOne(GLuint vao, int count,
                 const glm::mat4& mvp,
                 const glm::vec3& col,
                 GLuint shader) const;

    void drawOneDepth(GLuint vao, int count,
                      const glm::mat4& model,
                      GLuint shader) const;

    
    
    std::vector<float> makeCuboidBuf(float w, float h, float d);
    std::vector<float> makeRoofBuf(float baseW, float baseD, float roofH);
    std::vector<float> makeSkirtBuf(float rInner, float rOuter, float y);
    std::vector<float> makeCylinderBuf(float r, float len, int segs);
    std::vector<float> makeBladeBuf(float halfLen, float halfW, float halfThick);
    std::vector<float> makeFrustumPanel(float rBot, float rTop, float h,
                                        float a0, float a1);
    std::vector<float> makeWindowBuf(float w, float h);

    float rotorAngle; 
};

#endif
