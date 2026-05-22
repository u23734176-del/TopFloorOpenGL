#ifndef GOLFBALL_H
#define GOLFBALL_H

#include "core/SceneObject.h"
#include <vector>

class GolfBall : public SceneObject{
public:
    GolfBall();
    ~GolfBall() override;

    void build() override;
    void draw(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights) override;

private:
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    unsigned int indexCount;
};

#endif