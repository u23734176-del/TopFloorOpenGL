#ifndef WINDMILL_H
#define WINDMILL_H

#include "Cube.h"

class Windmill : public Cube
{
public:
    Windmill()
    {
        setScale(glm::vec3(1.0f, 6.0f, 1.0f));
    }
};

#endif