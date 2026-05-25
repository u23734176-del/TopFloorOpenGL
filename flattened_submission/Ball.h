#ifndef BALL_H
#define BALL_H

#include "Cube.h"

class Ball : public Cube
{
public:
    Ball()
    {
        setScale(glm::vec3(0.5f, 0.5f, 0.5f));
    }
};

#endif