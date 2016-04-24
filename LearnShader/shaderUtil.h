#pragma once

#include "rely.h"
#include "3DElement.h"

class oglShader
{
public:
	enum class Type
	{
		Vertex, Geometry, Fragment, TessCtrl, TessEval, Compute
	};
private:
	Type shaderType;
public:
	GLuint shaderID = 0;
	char * dat = nullptr;

	oglShader(Type, const char *);
	oglShader(const oglShader &) = delete;
	oglShader & operator = (const oglShader &) = delete;
	oglShader(oglShader &&);
	oglShader & operator = (oglShader &&);
	~oglShader();

	bool compile(string & msg);
};

class oglProgram
{
private:
	GLuint programID = 0;
	vector<oglShader> shaders;
	mat4 matrix_Proj, matrix_View, matrix_Model, matrix_Norm;
	GLuint ID_lgtVBO, ID_mtVBO;
public:
	const static GLuint
		IDX_projMat = 0,
		IDX_viewMat = 1,
		IDX_modelMat = 2,
		IDX_normMat = 3,
		IDX_camPos = 4;
	const static GLuint
		IDX_Vert_Pos = 0,
		IDX_Vert_Norm = 1,
		IDX_Vert_Color = 2,
		IDX_Vert_Texc = 3;
	const static GLuint
		IDX_Uni_Light = 0,
		IDX_Uni_Material = 1;

	~oglProgram();
	
	GLuint getPID() { return programID; };

	void init();
	void addShader(oglShader && shader);
	bool link(string & msg);
	void use();

	void setProject(const Camera &, const int wdWidth, const int wdHeight);
	void setCamera(const Camera &);
	void setLight(const Light &);
	void setMaterial(const Material &);
	void drawObject(const function<void(void)> &, const Vertex & vTrans, const Vertex & vRotate, const float fScale);
};