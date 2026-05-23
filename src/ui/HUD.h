#ifndef HUD_H
#define HUD_H

#include <GL/glew.h>
#include "../core/Camera.h"

class HUD
{
private:
    GLuint VAO;
    GLuint VBO;

    GLuint shaderProgram;

public:
    HUD();

    void build();

    void draw(Camera& camera,
              int width,
              int height,
              int mode);
};

#endif