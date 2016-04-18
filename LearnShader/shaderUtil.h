#pragma once

#include "rely.h"

bool loadShaders(const char * FName_vs, const char * FName_fs, GLuint & ID_vs, GLuint & ID_fs);
bool setShader(GLuint & ID_program, GLuint ID_vs, GLuint ID_fs);