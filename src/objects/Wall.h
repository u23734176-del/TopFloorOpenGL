#ifndef WALL_H
#define WALL_H

#include "Cube.h"

class Wall : public Cube
{
public:

    Wall()
    {
        setScale(glm::vec3(10.0f, 3.0f, 0.5f));
    }
};

#endif