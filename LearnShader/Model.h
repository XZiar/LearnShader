#pragma once

#include "rely.h"
#include "3DElement.h"

class Model
{
private:
	class Loader
	{
		wstring filename;
		FILE *fp;
		char tmpdat[8][256];
	public:
		Loader(const wstring &fname);
		~Loader();
		int8_t read(string data[]);
		int8_t parseFloat(const string &in, float out[]);
		int8_t parseInt(const string &in, int out[]);
	};
	vector<GLuint> ID_VAOs;
	vector<GLuint> ID_VBOs;
	GLuint texList[64];

	vector<Vertex> vers;
	vector<Normal> nors;
	vector<Coord2D> txcs;
	vector<vector<Point>> parts;

	wstring objname, mtlname;
private:
	void prepare();
	int32_t loadobj(const wstring &objname);
	int32_t loadmtl(const wstring &mtlname);
	int32_t loadtex(const string &texname);
	void reset();
public:
	Model() { };
	~Model();
	int32_t loadOBJ(const wstring &objname, const wstring &mtlname);
	void draw();
};


