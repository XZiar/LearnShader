#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#pragma comment(lib,"glew32s.lib")

#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include <GL/glm/gtc/type_ptr.hpp>
using glm::mat4;

#define USING_FREEGLUT true
#if USING_FREEGLUT
#    define FREEGLUT_STATIC
#    include <GL/freeglut.h>//Free GLUT Header
#else
#    include <GL/glut.h>   // The GL Utility Toolkit (Glut) Header
#endif

#define PI 3.1415926535897932384
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923

#include <cstdio>
#include <conio.h>
#include <cstdlib>
#include <cstdint>
#include <locale>
#include <cmath>
#include <Windows.h>
using namespace std;