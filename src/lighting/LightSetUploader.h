#ifndef LIGHTSET_UPLOADER_H
#define LIGHTSET_UPLOADER_H

#include "LightSet.h"
#include <GL/glew.h>

/// Uploads all light uniforms from a LightSet to the currently active shader.
/// This replaces the missing LightSet::apply() method.
void applyLightSet(const LightSet &lights, GLuint shaderProgram);

#endif