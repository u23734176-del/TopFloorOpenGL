#ifndef FLOOR_H
#define FLOOR_H

#include "Cube.h"

class Floor : public Cube
{
public:

    Floor()
    {
        setScale(glm::vec3(20.0f, 0.2f, 20.0f));
    }
};

#endif