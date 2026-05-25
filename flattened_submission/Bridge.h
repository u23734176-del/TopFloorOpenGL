#ifndef BRIDGE_H
#define BRIDGE_H

#include "Cube.h"

class Bridge : public Cube
{
public:
    Bridge()
    {
        setScale(glm::vec3(6.0f, 1.0f, 3.0f));
    }
};

#endif