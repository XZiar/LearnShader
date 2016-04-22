#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#pragma comment(lib,"glew32s.lib")

#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include <GL/glm/gtc/type_ptr.hpp>
using glm::mat4;

#define FREEGLUT_STATIC
#include <GL/freeglut.h>//Free GLUT Header


#define PI 3.1415926535897932384
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923

#include <cstdio>
#include <intrin.h>
#include <cstdlib>
#include <cstdint>
#include <locale>
#include <cmath>
#include <vector>
#include <Windows.h>
using namespace std;