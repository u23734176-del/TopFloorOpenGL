#ifndef TREEDECORATION_H
#define TREEDECORATION_H

#include "core/SceneObject.h"
#include <vector>

class TreeDecoration : public SceneObject{
public:
    TreeDecoration();
    ~TreeDecoration() override;

    void build() override;
    void draw(const glm::mat4 &view, const glm::mat4 &proj, const LightSet &lights) override;

    // Flyweight method: add a new instance without duplicating geometry
    void addInstance(const glm::mat4 &transform);

private:
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    unsigned int indexCount;

    std::vector<glm::mat4> instanceTransforms;
};

#endif