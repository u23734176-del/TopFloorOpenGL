#ifndef RAMP_H
#define RAMP_H

#include "Cube.h"

class Ramp : public Cube
{
public:
    Ramp()
    {
        setScale(glm::vec3(4.0f, 0.5f, 8.0f));
        setRotation(glm::vec3(-20.0f, 0.0f, 0.0f));
    }
};

#endif