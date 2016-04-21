#include "rely.h"
#include "shaderUtil.h"
#include "3DElement.h"

static GLuint ID_vs, ID_fs, ID_sp;
static GLuint ID_modelMat, ID_viewMat, ID_projMat;
static GLuint ID_VAO[4], ID_vertVBO[4], ID_normVBO[4], ID_colorVBO[4], ID_idxVBO[4];

static bool bMovPOI = false;
static int sx, sy, mx, my;
static Camera cam;


void CreateSphere(const float radius, const unsigned int rings, const unsigned int sectors, float *vertices, float *normals, float *texcoords, GLushort *indices)
{
	float const R = 1.0 / (float)(rings - 1);
	float const S = 1.0 / (float)(sectors - 1);

	float* v = vertices;
	float* n = normals;
	float* t = texcoords;
	GLushort *i = indices;

	for (int r = 0; r < rings; r++)
		for (int s = 0; s < sectors; s++)
		{
			float const y = sin(-M_PI_2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);
			*t++ = s*S;
			*t++ = r*R;
			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;

			*n++ = x;
			*n++ = y;
			*n++ = z;
		}

	for (int r = 0; r < rings - 1; r++)
		for (int s = 0; s < sectors - 1; s++)
		{
			*i++ = r * sectors + s;
			*i++ = r * sectors + (s + 1);
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = (r + 1) * sectors + s;
		}
}

void setSphere()
{
	float vertices[80 * 80 * 3];
	float normals[80 * 80 * 3];
	float texcoords[80 * 80 * 2];
	GLushort indices[80 * 80 * 4];
	CreateSphere(0.5f, 80, 80, vertices, normals, texcoords, indices);

	glBindVertexArray(ID_VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, ID_vertVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);//vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, ID_normVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);//normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID_idxVBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void init(void)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	const GLubyte *version = glGetString(GL_VERSION);
	printf("GL Version:%s\n", version);

	loadShaders("basic.vert", "basic.frag", ID_vs, ID_fs);
	setShader(ID_sp, ID_vs, ID_fs);

	ID_modelMat = glGetUniformLocation(ID_sp, "modelMat");
	ID_viewMat = glGetUniformLocation(ID_sp, "viewMat");
	ID_projMat = glGetUniformLocation(ID_sp, "projMat");

	glUseProgram(ID_sp);

	glGenVertexArrays(4, ID_VAO);
	glGenBuffers(4, ID_vertVBO);
	glGenBuffers(4, ID_normVBO);
	glGenBuffers(4, ID_idxVBO);

	glBindVertexArray(ID_VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, ID_vertVBO[0]);
	GLfloat DatVert[] =
	{
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,

		2.0f, 2.0f, -2.0f,
		1.0f, 0.0f, -2.0f,
		3.0f, 0.0f, -2.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(DatVert), DatVert, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);//vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, ID_normVBO[0]);
	GLfloat DatNorm[] =
	{
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(DatNorm), DatNorm, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);//normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	setSphere();

}

void display(void)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 viewMat = glm::lookAt(glm::vec3(cam.position), glm::vec3(cam.position + cam.n), glm::vec3(cam.v));
	glUniformMatrix4fv(ID_viewMat, 1, GL_FALSE, glm::value_ptr(viewMat));

	mat4 modelMat;
	glUniformMatrix4fv(ID_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));

	glBindVertexArray(ID_VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(ID_VAO[1]);
	glDrawElements(GL_QUADS, 79 * 79 * 4, GL_UNSIGNED_SHORT, NULL);

	glBindVertexArray(0);
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	cam.resize(w & 0x8fc0, h & 0x8fc0);
	glViewport((w & 0x3f) / 2, (h & 0x3f) / 2, cam.width, cam.height);

	mat4 projMat = glm::perspective(cam.fovy, cam.aspect, cam.zNear, cam.zFar);
	glUniformMatrix4fv(ID_projMat, 1, GL_FALSE, glm::value_ptr(projMat));
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	default:
		break;
	}
	glutPostRedisplay();
}

void onMouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			bMovPOI = true;
			sx = x, sy = y;
		}
		else
			bMovPOI = false;
		return;
	}

}

void onWheel(int button, int dir, int x, int y)
{
	if (dir == 1)//forward
		cam.move(0, 0, -1);
	else if (dir == -1)//backward
		cam.move(0, 0, 1);
	glutPostRedisplay();

}

void onMouse(int x, int y)
{
	if (bMovPOI)
	{
		int dx = x - sx, dy = y - sy;
		sx = x, sy = y;
		float pdx = 10.0*dx / cam.width, pdy = 10.0*dy / cam.height;
		cam.move(-pdx, pdy, 0);
		glutPostRedisplay();
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(cam.width, cam.height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	glewInit();
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutKeyboardFunc(keyboard);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMouse);
	glutMouseWheelFunc(onWheel);

	glutMainLoop();
	return 0;
}
