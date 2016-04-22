#include "rely.h"
#include "shaderUtil.h"
#include "3DElement.h"
#include "Model.h"

static oglProgram glProg;

static GLuint ID_VAO[4], ID_vertVBO[4], ID_normVBO[4], ID_colorVBO[4], ID_texcVBO[4], ID_idxVBO[4];

static bool bMovPOI = false;
static int sx, sy, mx, my;
static Camera cam;
static Model model;


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
	CreateSphere(0.8f, 80, 80, vertices, normals, texcoords, indices);

	glBindVertexArray(ID_VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, ID_vertVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(glProg.IDX_Vert_Pos);//vertex
	glVertexAttribPointer(glProg.IDX_Vert_Pos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, ID_normVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(glProg.IDX_Vert_Norm);//normal
	glVertexAttribPointer(glProg.IDX_Vert_Norm, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, ID_texcVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(glProg.IDX_Vert_Texc);//texc
	glVertexAttribPointer(glProg.IDX_Vert_Texc, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID_idxVBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

}

void init(void)
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	const GLubyte *version = glGetString(GL_VERSION);
	printf("GL Version:%s\n", version);
	glProg.init();
	
	string msg;

	{
		oglShader vert(oglShader::ShaderType::Vertex, "basic.vert");
		if (vert.compile(msg))
			glProg.addShader(std::move(vert));
		else
			printf("ERROR on Vertex Shader Compiler:\n%s\n", msg.c_str());
	}
	{
		oglShader frag(oglShader::ShaderType::Fragment, "basic.frag");
		if (frag.compile(msg))
			glProg.addShader(std::move(frag));
		else
			printf("ERROR on Fragment Shader Compiler:\n%s\n", msg.c_str());
	}
	{
		if (!glProg.link(msg))
			printf("ERROR on Program Linker:\n%s\n", msg.c_str());
	}
	glProg.use();

	glGenVertexArrays(4, ID_VAO);
	glGenBuffers(4, ID_vertVBO);
	glGenBuffers(4, ID_normVBO);
	glGenBuffers(4, ID_texcVBO);
	glGenBuffers(4, ID_idxVBO);

	glBindVertexArray(ID_VAO[0]);

	GLfloat DatVert[] =
	{
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		-1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		
		1.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		2.0f, 2.0f, -2.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, -2.0f,
		0.0f, 1.0f, 0.0f,

		3.0f, 0.0f, -2.0f,
		1.0f, 0.0f, 0.0f,
	};
	glBindBuffer(GL_ARRAY_BUFFER, ID_vertVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DatVert), DatVert, GL_STATIC_DRAW);

	glEnableVertexAttribArray(glProg.IDX_Vert_Pos);//vertex
	glVertexAttribPointer(glProg.IDX_Vert_Pos, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);

	glEnableVertexAttribArray(glProg.IDX_Vert_Norm);//normal
	glVertexAttribPointer(glProg.IDX_Vert_Norm, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	setSphere();

	model.loadOBJ(L"F:\\Project\\RayTrace\\objs\\0.obj", L"F:\\Project\\RayTrace\\objs\\0.mtl");

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void onExit()
{
	glDeleteVertexArrays(4, ID_VAO);
	glDeleteBuffers(4, ID_vertVBO);
	glDeleteBuffers(4, ID_normVBO);
	glDeleteBuffers(4, ID_texcVBO);
	glDeleteBuffers(4, ID_idxVBO);
}

void display(void)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 viewMat = glm::lookAt(vec3(cam.position), vec3(cam.position + cam.n), vec3(cam.v));
	glUniformMatrix4fv(glProg.IDX_viewMat, 1, GL_FALSE, glm::value_ptr(viewMat));
	mat4 modelMat;
	glUniformMatrix4fv(glProg.IDX_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));
	mat4 normMat;
	glUniformMatrix4fv(glProg.IDX_normMat, 1, GL_FALSE, glm::value_ptr(normMat));
	
	{
		mat4 tMat = glm::translate(modelMat, vec3(0.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(glProg.IDX_modelMat, 1, GL_FALSE, glm::value_ptr(tMat));

		glBindVertexArray(ID_VAO[1]);
		glDrawElements(GL_QUADS, 79 * 79 * 4, GL_UNSIGNED_SHORT, NULL);
	}
	{
		mat4 rMat = glm::rotate(modelMat, -float(M_PI_2), vec3(1.0f, 0.0f, 0.0f));
		mat4 sMat = glm::scale(rMat, vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(glProg.IDX_modelMat, 1, GL_FALSE, glm::value_ptr(sMat));
		glUniformMatrix4fv(glProg.IDX_normMat, 1, GL_FALSE, glm::value_ptr(rMat));

		model.draw();
	}
	glBindVertexArray(0);
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	cam.resize(w & 0x8fc0, h & 0x8fc0);
	glViewport((w & 0x3f) / 2, (h & 0x3f) / 2, cam.width, cam.height);

	//mat4 projMat = glm::frustum(-RProjZ, +RProjZ, -Aspect*RProjZ, +Aspect*RProjZ, 1.0, 32768.0);
	
	mat4 projMat = glm::perspective(cam.fovy, cam.aspect, cam.zNear, cam.zFar);
	glUniformMatrix4fv(glProg.IDX_projMat, 1, GL_FALSE, glm::value_ptr(projMat));
}

void onKeyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		cam.yaw(-3); break;
	case GLUT_KEY_RIGHT:
		cam.yaw(3); break;
	case GLUT_KEY_UP:
		cam.pitch(3); break;
	case GLUT_KEY_DOWN:
		cam.pitch(-3); break;
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
	
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutSpecialFunc(onKeyboard);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMouse);
	glutMouseWheelFunc(onWheel);

	glutMainLoop();

	onExit();
	return 0;
}
