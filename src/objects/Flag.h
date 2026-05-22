#ifndef FLAG_H
#define FLAG_H

#include "Cube.h"

class Flag : public Cube
{
public:
    Flag()
    {
        setScale(glm::vec3(0.2f, 3.0f, 0.2f));
    }
};

#endif