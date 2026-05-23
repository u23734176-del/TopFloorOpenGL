#ifndef LIGHTSET_H
#define LIGHTSET_H

#include <glm/glm.hpp>

// =====================================
// Directional Light (Sun/Moon)
// =====================================
struct DirectionalLight {
    glm::vec3 direction;
    glm::vec3 color;
};

// =====================================
// Point Light (with attenuation)
// =====================================
struct PointLight {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    float radius;
    float linear;
    float quadratic;
};

// =====================================
// Spotlight (inner/outer cones)
// =====================================
struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float innerCutoff;
    float outerCutoff;
};

// =====================================
// Global Lighting Set
// =====================================
struct LightSet {
    DirectionalLight directional;
    glm::vec3 ambient;
    
    static const int MAX_POINT_LIGHTS = 4;
    PointLight pointLights[MAX_POINT_LIGHTS];
    int numPointLights;
    
    SpotLight spotlight;
    bool spotlightEnabled;
    bool isNight;
    
    LightSet() 
        : numPointLights(0), spotlightEnabled(false), isNight(false)
    {
        directional.direction = glm::normalize(glm::vec3(-0.5f, -0.8f, -0.3f));
        directional.color = glm::vec3(1.0f);
        ambient = glm::vec3(0.3f);
        
        spotlight.position = glm::vec3(0.0f);
        spotlight.direction = glm::vec3(0.0f, -1.0f, 0.0f);
        spotlight.color = glm::vec3(1.0f, 0.95f, 0.8f);
        spotlight.intensity = 1.0f;
        spotlight.innerCutoff = glm::cos(glm::radians(12.5f));
        spotlight.outerCutoff = glm::cos(glm::radians(17.5f));
    }
};

#endif