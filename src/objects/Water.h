#ifndef WATER_H
#define WATER_H

#include "Cube.h"

class Water : public Cube
{
public:
    Water()
    {
        setScale(glm::vec3(6.0f, 0.1f, 6.0f));
    }
};

#endif