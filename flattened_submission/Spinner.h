#ifndef SPINNER_H
#define SPINNER_H

#include "Cube.h"

class Spinner : public Cube
{
public:
    Spinner()
    {
        setScale(glm::vec3(5.0f, 0.2f, 0.5f));
    }
};

#endif