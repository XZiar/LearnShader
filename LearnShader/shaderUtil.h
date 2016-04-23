#pragma once

#include "rely.h"

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
	vector<oglShader> shaders;
public:
	const static GLuint
		IDX_projMat = 0,
		IDX_viewMat = 1,
		IDX_modelMat = 2,
		IDX_normMat = 3;
	const static GLuint
		IDX_Vert_Pos = 0,
		IDX_Vert_Norm = 1,
		IDX_Vert_Color = 2,
		IDX_Vert_Texc = 3;

	GLuint programID = 0;

	~oglProgram();
	
	GLuint getPID() { return programID; };

	void init();
	void addShader(oglShader && shader);
	bool link(string & msg);
	void use();

};