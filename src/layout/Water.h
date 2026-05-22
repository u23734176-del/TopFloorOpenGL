#ifndef WATER_H
#define WATER_H

#include "core/SceneObject.h"

class Water : public SceneObject
{
public:
    Water();
    ~Water() override;

    void build() override;
    void draw(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights) override;

private:
    GLuint VAO;
    GLuint VBO;
};

#endif