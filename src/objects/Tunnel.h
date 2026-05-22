#ifndef TUNNEL_H
#define TUNNEL_H

#include "Cube.h"

class Tunnel : public Cube
{
public:
    Tunnel()
    {
        setScale(glm::vec3(4.0f, 2.0f, 6.0f));
    }
};

#endif