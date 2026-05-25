#ifndef LIGHTSET_UPLOADER_H
#define LIGHTSET_UPLOADER_H

#include "LightSet.h"
#include <GL/glew.h>



void applyLightSet(const LightSet &lights, GLuint shaderProgram);

#endif