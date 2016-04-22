#include "rely.h"
#include "shaderUtil.h"

oglShader::oglShader(ShaderType type, const char * fpath) : shaderType(type)
{
	FILE * fp;
	if (fopen_s(&fp, fpath, "rb") != 0)
		return;

	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	dat = new char[fsize + 1];
	fread(dat, fsize, 1, fp);
	dat[fsize] = '\0';

	fclose(fp);

	GLenum stype;
	switch (type)
	{
	case ShaderType::Vertex:
		stype = GL_VERTEX_SHADER; break;
	case ShaderType::Fragment:
		stype = GL_FRAGMENT_SHADER; break;
	case ShaderType::Geometry:
		stype = GL_GEOMETRY_SHADER; break;
	case ShaderType::TessCtrl:
		stype = 0; break;
	case ShaderType::TessEval:
		stype = 0; break;
	}
	shaderID = glCreateShader(stype);
	glShaderSource(shaderID, 1, &dat, NULL);
}

oglShader::oglShader(oglShader && other)
{
	*this = move(other);
}

oglShader & oglShader::operator = (oglShader && other)
{
	shaderID = other.shaderID;
	shaderType = other.shaderType;
	if (dat != nullptr)
		delete[] dat;
	dat = other.dat;
	other.dat = nullptr;
	other.shaderID = 0;

	return *this;
}

oglShader::~oglShader()
{
	if (shaderID)
		glDeleteShader(shaderID);
	if(dat != nullptr)
		delete[] dat;
}

bool oglShader::compile(string & msg)
{
	glCompileShader(shaderID);

	GLint result;
	char logstr[4096] = { 0 };

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(shaderID, sizeof(logstr), NULL, logstr);
		msg.assign(logstr);
		return false;
	}

	return true;
}

oglProgram::~oglProgram()
{
	if (programID)
		glDeleteProgram(programID);
}

void oglProgram::init()
{
	programID = glCreateProgram();
}

void oglProgram::addShader(oglShader && shader)
{
	glAttachShader(programID, shader.shaderID);
	shaders.push_back(forward<oglShader>(shader));
}

bool oglProgram::link(string & msg)
{
	glLinkProgram(programID);

	int result;
	char logstr[4096] = { 0 };

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(programID, sizeof(logstr), NULL, logstr);
		glDeleteProgram(programID);
		msg.assign(logstr);
		return false;
	}
	return true;
}

void oglProgram::use()
{
	glUseProgram(programID);
}
