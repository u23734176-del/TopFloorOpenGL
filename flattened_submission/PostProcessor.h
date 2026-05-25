#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

#include <GL/glew.h>

class PostProcessor
{
private:
    GLuint fbo;
    GLuint textureColorBuffer;
    GLuint rbo;

    GLuint quadVAO;
    GLuint quadVBO;

    GLuint shader;

    int mode;
    

public:
    PostProcessor();

    void build(int width, int height);

    void beginRender();

    void endRender();

    void draw();

    void setMode(int newMode);
    int getMode();
};

#endif