#pragma once

#include <GL/glew.h>

class ShadowMap
{
public:
    ShadowMap();
    ~ShadowMap();

    bool init(unsigned int width = 2048, unsigned int height = 2048);

    void beginDepthPass() const;
    void endDepthPass(int screenWidth, int screenHeight) const;

    GLuint getDepthTexture() const;

    unsigned int getWidth() const;
    unsigned int getHeight() const;

private:
    GLuint fbo;
    GLuint depthTexture;

    unsigned int width;
    unsigned int height;

};