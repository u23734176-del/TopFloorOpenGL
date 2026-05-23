#ifndef DRONE_H
#define DRONE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "../core/shader.hpp"
#include "../core/SceneObject.h"
#include "../lighting/LightSet.h"

class Drone : public SceneObject {
public:
    Drone();
    ~Drone();

    void build()  override;
    void draw(const glm::mat4& view, const glm::mat4& proj, const LightSet& lights) override;

    void processInput(GLFWwindow* window, float deltaTime);

    glm::vec3 getPosition() const;
    glm::vec3 getFront()    const;
    float     getRoll()     const;

private:
    // one VAO per part
    GLuint bodyVAO,  bodyCount;
    GLuint armVAO,   armCount;
    GLuint rotorVAO, rotorCount;
    GLuint guardVAO, guardCount;

    GLuint shaderProgram;

    // drone state
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 worldUp;
    float     yaw, pitch, roll;
    float     speed;
    float     rotorAngle;

    void updateFront();

    // same style as your prac — returns vector<float> pos+colour
    std::vector<float> makeSphere  (float r, int stacks, int slices, float cr, float cg, float cb);
    std::vector<float> makeCylinder(float r, float h, int slices,    float cr, float cg, float cb);
    std::vector<float> makeDisc    (float r, int slices,             float cr, float cg, float cb);
    std::vector<float> makeTorus   (float mainR, float tubeR, int mainSegs, int tubeSegs,
                                    float cr, float cg, float cb);

    GLuint uploadMesh(const std::vector<float>& verts);

    void drawPart(GLuint vao, int count,
                  const glm::mat4& model,
                  const glm::mat4& view,
                  const glm::mat4& proj);

    GLuint loadDroneShaders();
    void drawDepth(GLuint depthShaderProgram) override;
};

#endif