#include "LightSetUploader.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>

void applyLightSet(const LightSet &lights, GLuint shaderProgram)
{
    glUseProgram(shaderProgram);

    
    glUniform3fv(glGetUniformLocation(shaderProgram, "ambientLight"), 1,
                 glm::value_ptr(lights.ambient));

    
    glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.direction"), 1,
                 glm::value_ptr(lights.directional.direction));
    glUniform3fv(glGetUniformLocation(shaderProgram, "dirLight.color"), 1,
                 glm::value_ptr(lights.directional.color));

    
    glUniform1i(glGetUniformLocation(shaderProgram, "numPointLights"),
                lights.numPointLights);
    for (int i = 0; i < lights.numPointLights && i < LightSet::MAX_POINT_LIGHTS; ++i)
    {
        std::string base = "pointLights[" + std::to_string(i) + "]";
        const PointLight &p = lights.pointLights[i];
        glUniform3fv(glGetUniformLocation(shaderProgram, (base + ".position").c_str()), 1,
                     glm::value_ptr(p.position));
        glUniform3fv(glGetUniformLocation(shaderProgram, (base + ".color").c_str()), 1,
                     glm::value_ptr(p.color));
        glUniform1f(glGetUniformLocation(shaderProgram, (base + ".intensity").c_str()),
                    p.intensity);
        glUniform1f(glGetUniformLocation(shaderProgram, (base + ".radius").c_str()),
                    p.radius);
        glUniform1f(glGetUniformLocation(shaderProgram, (base + ".linear").c_str()),
                    p.linear);
        glUniform1f(glGetUniformLocation(shaderProgram, (base + ".quadratic").c_str()),
                    p.quadratic);
    }

    
    glUniform1i(glGetUniformLocation(shaderProgram, "spotlight.enabled"),
                lights.spotlightEnabled ? 1 : 0);
    if (lights.spotlightEnabled)
    {
        glUniform3fv(glGetUniformLocation(shaderProgram, "spotlight.position"), 1,
                     glm::value_ptr(lights.spotlight.position));
        glUniform3fv(glGetUniformLocation(shaderProgram, "spotlight.direction"), 1,
                     glm::value_ptr(lights.spotlight.direction));
        glUniform3fv(glGetUniformLocation(shaderProgram, "spotlight.color"), 1,
                     glm::value_ptr(lights.spotlight.color));
        glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.intensity"),
                    lights.spotlight.intensity);
        glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.innerCutoff"),
                    lights.spotlight.innerCutoff);
        glUniform1f(glGetUniformLocation(shaderProgram, "spotlight.outerCutoff"),
                    lights.spotlight.outerCutoff);
    }
}