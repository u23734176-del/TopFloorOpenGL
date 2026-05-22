#ifndef LIGHTSET_H
#define LIGHTSET_H

#include <glm/glm.hpp>

struct DirectionalLight{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight{
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

struct LightSet{
    DirectionalLight sunMoon;
    PointLight pathLights[10];
    SpotLight droneSpot;
    glm::vec3 globalAmbient;
    float timeOfDay;
};

#endif