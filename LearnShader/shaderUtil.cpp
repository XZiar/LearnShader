#include "rely.h"
#include "shaderUtil.h"

oglShader::oglShader(Type type, const char * fpath) : shaderType(type)
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
	case Type::Vertex:
		stype = GL_VERTEX_SHADER; break;
	case Type::Fragment:
		stype = GL_FRAGMENT_SHADER; break;
	case Type::Geometry:
		stype = GL_GEOMETRY_SHADER; break;
	case Type::TessCtrl:
		stype = 0; break;
	case Type::TessEval:
		stype = 0; break;
	default:
		stype = 0;
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
	glGenBuffers(1, &ID_lgtVBO);
	glGenBuffers(1, &ID_mtVBO);
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
	GLuint idx;

	//set light
	idx = glGetUniformBlockIndex(programID, "lightBlock");
	glBindBuffer(GL_UNIFORM_BUFFER, ID_lgtVBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, IDX_Uni_Light, ID_lgtVBO);
	glUniformBlockBinding(programID, idx, IDX_Uni_Light);
	glBufferData(GL_UNIFORM_BUFFER, 96, NULL, GL_DYNAMIC_DRAW);

	//set material
	idx = glGetUniformBlockIndex(programID, "materialBlock");
	glBindBuffer(GL_UNIFORM_BUFFER, ID_mtVBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, IDX_Uni_Material, ID_mtVBO);
	glUniformBlockBinding(programID, idx, IDX_Uni_Material);
	glBufferData(GL_UNIFORM_BUFFER, 80, NULL, GL_DYNAMIC_DRAW);
}

void oglProgram::setProject(const Camera & cam, const int wdWidth, const int wdHeight)
{
	glViewport((wdWidth & 0x3f) / 2, (wdHeight & 0x3f) / 2, cam.width, cam.height);

	//mat4 projMat = glm::frustum(-RProjZ, +RProjZ, -Aspect*RProjZ, +Aspect*RProjZ, 1.0, 32768.0);
	matrix_Proj = glm::perspective(cam.fovy, cam.aspect, cam.zNear, cam.zFar);
	glUniformMatrix4fv(IDX_projMat, 1, GL_FALSE, glm::value_ptr(matrix_Proj));
}

void oglProgram::setCamera(const Camera & cam)
{
	matrix_View = glm::lookAt(vec3(cam.position), vec3(cam.position + cam.n), vec3(cam.v));
	glUniformMatrix4fv(IDX_viewMat, 1, GL_FALSE, glm::value_ptr(matrix_View));
	glUniform3fv(IDX_camPos, 1, cam.position);
}

void oglProgram::setLight(const Light & light)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ID_lgtVBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 96, &light);
}

void oglProgram::drawObject(const function<void(void)>& draw, const Vertex & vTrans, const Vertex & vRotate, const float fScale)
{
	mat4 rMat = glm::rotate(mat4(), vRotate.x * float(M_PI), vec3(1.0f, 0.0f, 0.0f));
	rMat = glm::rotate(rMat, vRotate.y * float(M_PI), vec3(0.0f, 1.0f, 0.0f));
	rMat = glm::rotate(rMat, vRotate.z * float(M_PI), vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(IDX_normMat, 1, GL_FALSE, glm::value_ptr(matrix_Norm * rMat));

	mat4 mMat = glm::translate(matrix_Model, vec3(vTrans));
	mMat = glm::scale(mMat, vec3(fScale));
	glUniformMatrix4fv(IDX_modelMat, 1, GL_FALSE, glm::value_ptr(mMat * rMat));
	
	draw();

	glUniformMatrix4fv(IDX_modelMat, 1, GL_FALSE, glm::value_ptr(matrix_Model));
	glUniformMatrix4fv(IDX_normMat, 1, GL_FALSE, glm::value_ptr(matrix_Norm));
}
