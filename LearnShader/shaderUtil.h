#pragma once

#include "rely.h"

class oglShader
{
public:
	enum class ShaderType
	{
		Vertex, Geometry, Fragment, TessCtrl, TessEval
	};
private:
	ShaderType shaderType;
	
public:
	GLuint shaderID = 0;
	char * dat = nullptr;
	oglShader()
	{
		printf("normal make\n");
	};
	oglShader(ShaderType, const char *);
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
	GLuint programID = 0;
	~oglProgram();
	
	GLuint getPID() { return programID; };

	void init();
	void addShader(oglShader && shader);
	bool link(string & msg);
	void use();

};