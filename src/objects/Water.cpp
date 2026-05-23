#include "Water.h"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "../core/ShaderManager.h"

// ---------------------------------------------------------------------------
// Water.cpp  (Slice C - Karabo)
//
// Flat horizontal quad (two triangles) with an upward normal, drawn with the
// "water" shader so it can be alpha-blended. The transparency-correct draw
// disables depth WRITES (but keeps depth TEST) so opaque geometry behind the
// water shows through, then restores the depth mask.
//
// IMPORTANT (render order): water must be drawn AFTER all opaque objects. The
// cleanest way with the current single-pass Scene is to keep this Water out of
// scene.addObject() and call water.draw(view, proj, lights) explicitly right
// after scene.drawAllObjects(...). See SLICE_E_INTEGRATION.md.
// ---------------------------------------------------------------------------

Water::Water()
    : vao(0), vbo(0), shader(0), vertexCount(0), sizeX(6.0f), sizeZ(6.0f), alpha(0.55f), shininess(128.0f)
{
    setColor(glm::vec3(0.1f, 0.3f, 0.5f)); // watery blue
    // Flat plane AABB (thin in y) for any collision queries.
    initAABBFromPrimitive(glm::vec3(sizeX, 0.05f, sizeZ));
}

void Water::setSize(float halfX, float halfZ)
{
    sizeX = halfX;
    sizeZ = halfZ;
    initAABBFromPrimitive(glm::vec3(sizeX, 0.05f, sizeZ));
}

void Water::setAlpha(float a) { alpha = a; }
void Water::setShininess(float s) { shininess = s; }

void Water::build()
{
    // Dedicated water program (alpha-capable). main.cpp must register it:
    //   ShaderManager::load("water", "shaders/water.vert", "shaders/water.frag");
    shader = ShaderManager::get("water");

    // Two triangles forming a horizontal quad at local y = 0, normal +Y.
    // Interleaved [pos.xyz, normal.xyz] to match the vertex layout (loc 0/1).
    float v[] = {
        -sizeX, 0.0f, -sizeZ, 0.0f, 1.0f, 0.0f,
        sizeX, 0.0f, -sizeZ, 0.0f, 1.0f, 0.0f,
        sizeX, 0.0f, sizeZ, 0.0f, 1.0f, 0.0f,

        sizeX, 0.0f, sizeZ, 0.0f, 1.0f, 0.0f,
        -sizeX, 0.0f, sizeZ, 0.0f, 1.0f, 0.0f,
        -sizeX, 0.0f, -sizeZ, 0.0f, 1.0f, 0.0f};
    vertexCount = 6;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Water::draw(const glm::mat4 &view,
                 const glm::mat4 &proj,
                 const LightSet &lights)
{
    glUseProgram(shader);

    // NOTE: main.cpp sets the lighting/shadow uniforms on "basic" each frame,
    // NOT on "water". So water needs those uniforms set on ITS program too.
    // The simplest integration is to call main's passLightingUniforms(shader,
    // lights, cameraPos) for the water program as well (see integration note).
    // Here we set only what this class owns: matrices, colour, alpha, shininess.

    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &proj[0][0]);
    glUniform3fv(glGetUniformLocation(shader, "objectColor"), 1, &color[0]);
    glUniform1f(glGetUniformLocation(shader, "alpha"), alpha);
    glUniform1f(glGetUniformLocation(shader, "shininess"), shininess);

    // Transparent draw: keep depth TEST (so geometry in front still occludes
    // water) but disable depth WRITES (so opaque geometry behind shows through
    // and multiple transparent frags blend). Blending is already enabled in
    // main.cpp with (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA).
    GLboolean depthMaskWas;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMaskWas);
    glDepthMask(GL_FALSE);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    glDepthMask(depthMaskWas); // restore whatever it was
}

void Water::drawDepth(GLuint depthShaderProgram)
{
    // Water is transparent; it generally should NOT cast hard shadows. Skipping
    // the depth pass keeps it from blacking out the green beneath it. If you do
    // want it in the shadow map, set model and draw like other objects.
    (void)depthShaderProgram;
}