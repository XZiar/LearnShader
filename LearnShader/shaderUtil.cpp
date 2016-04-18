#include "rely.h"

bool loadShaders(const char * FName_vs, const char * FName_fs, GLuint & ID_vs, GLuint & ID_fs)
{
	ID_vs = glCreateShader(GL_VERTEX_SHADER);
	ID_fs = glCreateShader(GL_FRAGMENT_SHADER);

	FILE * FP_vs, * FP_fs;
	fopen_s(&FP_vs, FName_vs, "rb");
	fopen_s(&FP_fs, FName_fs, "rb");

	fseek(FP_vs, 0, SEEK_END);
	fseek(FP_fs, 0, SEEK_END);

	long FileSize_vs = ftell(FP_vs),
		FileSize_fs = ftell(FP_fs);

	fseek(FP_vs, 0, SEEK_SET);
	fseek(FP_fs, 0, SEEK_SET);

	char * Dat_vs = new char[FileSize_vs + 1];
	char * Dat_fs = new char[FileSize_fs + 1];

	fread(Dat_vs, FileSize_vs, 1, FP_vs);
	fread(Dat_fs, FileSize_fs, 1, FP_fs);
	Dat_fs[FileSize_fs] = Dat_vs[FileSize_vs] = '\0';

	fclose(FP_vs);
	fclose(FP_fs);

	glShaderSource(ID_vs, 1, &Dat_vs, NULL);
	glShaderSource(ID_fs, 1, &Dat_fs, NULL);

	delete[] Dat_vs;
	delete[] Dat_fs;

	glCompileShader(ID_vs);
	glCompileShader(ID_fs);

	bool res = true;
	GLint result;
	char logstr[4096] = { 0 };

	glGetShaderiv(ID_vs, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		//void glGetProgramInfoLog(GLuint object, int maxLen, int *len, char *log);  
		glGetShaderInfoLog(ID_vs, sizeof(logstr), NULL, logstr);
		glDeleteShader(ID_vs);
		printf("ERROR on Vertex Shader Compiler:\n%s\n", logstr);
		res = false;
	}
	glGetShaderiv(ID_fs, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(ID_fs, sizeof(logstr), NULL, logstr);
		glDeleteShader(ID_fs);
		printf("ERROR on Fragment Shader Compiler:\n%s\n", logstr);
		res = false;
	}

	return res;
}


bool setShader(GLuint & ID_program, GLuint ID_vs, GLuint ID_fs)
{
	ID_program = glCreateProgram();

	glAttachShader(ID_program, ID_vs);
	glAttachShader(ID_program, ID_fs);

	glLinkProgram(ID_program);
	//glUseProgram(ID_program);

	bool res = true;
	int result;
	char logstr[4096] = { 0 };

	glGetProgramiv(ID_program, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(ID_program, sizeof(logstr), NULL, logstr);
		glDeleteProgram(ID_program);
		printf("ERROR on Shader Program Linker:\n%s\n", logstr);
		res = false;
	}

	return res;
}