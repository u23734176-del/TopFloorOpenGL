#ifndef LIGHTSET_H
#define LIGHTSET_H

#include <iostream>
#include "ShaderManager.h"

#include <glm/glm.hpp>

struct LightSet
{
    glm::vec3 direction;
    
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};


#endif