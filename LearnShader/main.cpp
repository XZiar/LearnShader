#include "rely.h"
#include "shaderUtil.h"
#include "3DElement.h"
#include "Model.h"

static oglProgram glProg;

static GLuint ID_VAO[4], ID_vertVBO[4], ID_normVBO[4], ID_colorVBO[4], ID_texcVBO[4], ID_idxVBO[4];
//static GLuint ID_lgtVBO;

static bool bMovPOI = false;
static int sx, sy, mx, my;
static Camera cam;
static Light light(Light::Type::Parallel);
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

void setTriangles()
{
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
		oglShader vert(oglShader::Type::Vertex, "basic.vert");
		if (vert.compile(msg))
			glProg.addShader(std::move(vert));
		else
			printf("ERROR on Vertex Shader Compiler:\n%s\n", msg.c_str());
	}
	{
		oglShader frag(oglShader::Type::Fragment, "basic.frag");
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
	

	setTriangles();
	setSphere();

	model.loadOBJ(L"F:\\Project\\RayTrace\\objs\\0.obj", L"F:\\Project\\RayTrace\\objs\\0.mtl");

	glBindVertexArray(0);
}

void onExit()
{
	glDeleteVertexArrays(4, ID_VAO);
	glDeleteBuffers(4, ID_vertVBO);
	glDeleteBuffers(4, ID_normVBO);
	glDeleteBuffers(4, ID_texcVBO);
	glDeleteBuffers(4, ID_idxVBO);
	//glDeleteBuffers(1, &ID_lgtVBO);
}

void display(void)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glProg.setLight(light);

	//set camera
	glProg.setCamera(cam);

	glProg.drawObject([&]()
	{
		glBindVertexArray(ID_VAO[1]);
		glDrawElements(GL_QUADS, 79 * 79 * 4, GL_UNSIGNED_SHORT, NULL);
	}, Vertex(0.0f, -1.0f, 0.0f), Vertex(), 1.0f);

	glProg.drawObject([&]()
	{
		model.draw();
	}, Vertex(), Vertex(-0.5f, 0.0f, 0.0f), 0.04f);

	glBindVertexArray(0);
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	cam.resize(w & 0x8fc0, h & 0x8fc0);
	
	glProg.setProject(cam, w, h);
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

void onKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	//move light
	case '2':
	case '4':
	case '6':
	case '8':
	case 43://+
	case 45://-
		switch (key)
		{
		case '2':
			light.move(3, 0, 0); break;
		case '4':
			light.move(0, -3, 0); break;
		case '6':
			light.move(0, 3, 0); break;
		case '8':
			light.move(-3, 0, 0); break;
		case 43://+
			light.move(0, 0, 1); break;
		case 45://-
			light.move(0, 0, -1); break;
		}
		break;
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
	glutKeyboardFunc(onKeyboard);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMouse);
	glutMouseWheelFunc(onWheel);

	glutMainLoop();

	onExit();
	return 0;
}
