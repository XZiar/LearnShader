#include "rely.h"
#include "Model.h"

#define pS(a,b,c) (a)*65536+(b)*256+(c)

static inline int32_t parseStr(string str)
{
	if (str.length() < 3)
		str += "****";
	return str[2] + 256 * str[1] + 65536 * str[0];
}

int32_t Model::loadobj(const wstring &objname)
{
	Loader ldr(objname);
	string ele[5];
	int8_t num;
	int ti[16];
	bool bFirstT = true,
		bFirstO = true;

	//clean
	vers.reserve(8000);
	nors.reserve(8000);
	txcs.reserve(8000);

	//load
	vector<Point> part;
	part.reserve(6000);
	Vertex v;
	Normal n;
	Coord2D tc;
	Point p;

	vers.push_back(v);
	nors.push_back(n);
	txcs.push_back(tc);

	while (true)
	{
		num = ldr.read(ele);
		if (num == INT8_MIN)
			break;
		if (num == -1)
			continue;
		switch (parseStr(ele[0]))
		{
		case pS('v', '*', '*')://vertex
			v = Vertex(atof(ele[1].c_str()), atof(ele[2].c_str()), atof(ele[3].c_str()));
			vers.push_back(v);
			break;
		case pS('v', 'n', '*')://normal
			n = Normal(atof(ele[1].c_str()), atof(ele[2].c_str()), atof(ele[3].c_str()));
			nors.push_back(n);
			break;
		case pS('v', 't', '*')://texture coord
			tc = Coord2D(atof(ele[1].c_str()), atof(ele[2].c_str()));
			txcs.push_back(tc);
			break;
		case pS('f', '*', '*')://triangle
			ldr.parseInt(ele[1], &ti[0]);
			ldr.parseInt(ele[2], &ti[3]);
			ldr.parseInt(ele[3], &ti[6]);

			part.push_back(move(
				Point(vers[ti[0]], nors[ti[2]], txcs[ti[1]])
			));
			part.push_back(move(
				Point(vers[ti[3]], nors[ti[5]], txcs[ti[4]])
			));
			part.push_back(move(
				Point(vers[ti[6]], nors[ti[8]], txcs[ti[7]])
			));
			if (num > 4)//quad
			{
				ldr.parseInt(ele[4], &ti[9]);

				part.push_back(move(
					Point(vers[ti[0]], nors[ti[2]], txcs[ti[1]])
				));
				part.push_back(move(
					Point(vers[ti[6]], nors[ti[8]], txcs[ti[7]])
				));
				part.push_back(move(
					Point(vers[ti[9]], nors[ti[11]], txcs[ti[10]])
				));
			}
			break;
		case pS('u', 's', 'e')://object part
			if (!bFirstO)
			{
				part.shrink_to_fit();
				parts.push_back(move(part));
				part.reserve(2000);
			}
			bFirstO = false;
			uint8_t a = mtls.size();
			while (--a > 0)
				if (mtls[a].name == ele[1])
					break;
			part_mtl.push_back(a);
			break;
		}
	}
	/*if (bFirstO)
	{
		part_mtl.push_back(0);
	}*/
	part.shrink_to_fit();
	parts.push_back(move(part));
	
	return parts.size();
}

void Model::loadmtl(const wstring &mtlname)
{
	Loader ldr(mtlname);
	string ele[5];
	int8_t num;
	int8_t cur_tex_id = -1;

	bool bFirstT = true,
		bFirstM = true;

	//clean

	Material mtl;
	mtls.push_back(mtl);
	//mtl_tex.push_back(cur_tex_id);
	
	//load
	while (true)
	{
		num = ldr.read(ele);
		if (num == INT8_MIN)
			break;
		if (num == -1)
			continue;
		switch (parseStr(ele[0]))
		{
		case pS('n', 'e', 'w')://mtl part
			if (!bFirstM)
			{
				mtls.push_back(mtl);
				//mtl_tex.push_back(cur_tex_id);
			}
			mtl.name = ele[1];
			cur_tex_id = -1;
			bFirstM = false;
			break;
		case pS('K', 'a', '*')://ambient
			mtl.SetMtl(Material::Property::Ambient, atof(ele[1].c_str()), atof(ele[2].c_str()), atof(ele[3].c_str()));
			break;
		case pS('K', 'd', '*')://diffuse
			mtl.SetMtl(Material::Property::Diffuse, atof(ele[1].c_str()), atof(ele[2].c_str()), atof(ele[3].c_str()));
			break;
		case pS('K', 's', '*')://specular
			mtl.SetMtl(Material::Property::Specular, atof(ele[1].c_str()), atof(ele[2].c_str()), atof(ele[3].c_str()));
			break;
		case pS('K', 'e', '*')://emission
			mtl.SetMtl(Material::Property::Emission, atof(ele[1].c_str()), atof(ele[2].c_str()), atof(ele[3].c_str()));
			break;
		case pS('N','s','*')://shiness
			mtl.SetMtl(Material::Property::Shiness, atof(ele[1].c_str()));
			break;
		case pS('m', 'a', 'p')://Texture
			/*if (ele[0] == "map_Kd****")
			{
				auto loff = ele[1].find_last_of('\\'),
					roff = ele[1].find_last_of('.');
				ele[1] = ele[1].substr(loff + 1, roff - loff) + "bmp";

				int8_t a = texs.size();
				while (--a >= 0)
					if (texs[a].name == ele[1])
					{
						cur_tex_id = a;
						break;
					}
				if (a < 0)
				{
					loadtex(ele[1]);
					cur_tex_id = texs.size() - 1;
				}
			}*/
			break;
		}
	}
	mtls.push_back(mtl);
	//mtl_tex.push_back(cur_tex_id);
	return;
}

int32_t Model::loadtex(const string &texname)
{
	BITMAPFILEHEADER FileHeader;    //接受位图文件头
	BITMAPINFOHEADER InfoHeader;    //接受位图信息头
	FILE *fp;
	if (fopen_s(&fp, texname.c_str(), "rb") != 0)
		return -1;
	fread(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	if (FileHeader.bfType != 0x4d42)  //确保文件是一个位图文件，效验文件类型
	{
		fclose(fp);
		return -1;
	}
	fread(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	int32_t width = InfoHeader.biWidth,
		height = InfoHeader.biHeight,
		size = InfoHeader.biSizeImage;
	if (InfoHeader.biSizeImage == 0)          //确保图像数据的大小
		size = width*height * 3;
	fseek(fp, FileHeader.bfOffBits, SEEK_SET);  //将文件指针移动到实际图像数据处
	uint8_t *image = new uint8_t[size];
	fread(image, 1, size, fp);

	//texs.push_back(move(Texture(texname, width, height, image)));
	delete[] image;
	fclose(fp);
	return int32_t();
}

void Model::reset()
{
	auto partCnt = ID_VAOs.size();
	if (partCnt > 0)
	{
		glDeleteVertexArrays(partCnt, &ID_VAOs[0]);
		ID_VAOs.clear();
		glDeleteBuffers(partCnt, &ID_VBOs[0]);
		ID_VBOs.clear();
	}
	//glDeleteTextures(texs.size(), texList);
	memset(texList, 0x0, sizeof(texList));

	//mtl_tex.swap(vector<int8_t>());
	part_mtl.swap(vector<uint8_t>());
	//texs.swap(vector<Texture>());
	mtls.swap(vector<Material>());
	parts.swap(vector<vector<Point>>());

	vers.swap(vector<Vertex>());
	nors.swap(vector<Normal>());
	txcs.swap(vector<Coord2D>());
}

void Model::prepare()
{
	GLuint IDvaos[64], IDvbos[64];
	int partCnt = parts.size();

	glGenVertexArrays(partCnt, IDvaos);
	ID_VAOs.assign(IDvaos, IDvaos + partCnt);
	glGenBuffers(partCnt, IDvbos);
	ID_VBOs.assign(IDvbos, IDvbos + partCnt);
	
	for (int a = 0; a < partCnt; a++)
	{
		const vector<Point> & part = parts[a];

		glBindVertexArray(ID_VAOs[a]);
		glBindBuffer(GL_ARRAY_BUFFER, ID_VBOs[a]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * part.size(), &part[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);//vertex
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);

		glEnableVertexAttribArray(1);//normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(4 * sizeof(GLfloat)));
	}
}


Model::~Model()
{
	reset();
}

int32_t Model::loadOBJ(const wstring & objn, const wstring & mtln)
{
	objname = objn, mtlname = mtln;
	reset();
	loadmtl(mtlname);
	loadobj(objname);
	prepare();
	return 1;
}

void Model::draw()
{
	int partCnt = parts.size();
	for (int a = 0; a < partCnt; a++)
	{
		funSetMt(mtls[part_mtl[a]]);
		glBindVertexArray(ID_VAOs[a]);
		glDrawArrays(GL_TRIANGLES, 0, parts[a].size());
	}
}

/*
void Model::SetMtl(const Material & mtl)
{
	for (auto &old : mtls)
		old = mtl;
	//mtls[0] = mtl;
}
*/







Model::Loader::Loader(const wstring &fname)
{
	filename = fname;
	_wfopen_s(&fp, filename.c_str(), L"r");
}

Model::Loader::~Loader()
{
	if (fp != NULL)
		fclose(fp);
}

int8_t Model::Loader::read(string data[])
{
	if (fp == NULL)
		return INT8_MIN;
	if (fgets(tmpdat[0], 256, fp) != NULL)
	{
		auto a = sscanf_s(tmpdat[0], "%s%s%s%s%s", tmpdat[1], 256, tmpdat[2], 256, tmpdat[3], 256, tmpdat[4], 256, tmpdat[5], 256);
		strcat_s(tmpdat[1], "****");
		for (auto b = 0; b < a; ++b)
			data[b] = string(tmpdat[b + 1]);
		return a;
	}
	else
		return INT8_MIN;
}

int8_t Model::Loader::parseFloat(const string &in, float out[])
{
	auto a = sscanf_s(in.c_str(), "%f/%f/%f/%f", &out[0], &out[1], &out[2], &out[3]);
	return a;
}

int8_t Model::Loader::parseInt(const string &in, int out[])
{
	auto a = sscanf_s(in.c_str(), "%d/%d/%d/%d", &out[0], &out[1], &out[2], &out[3]);
	if (a < 2)
	{
		a = sscanf_s(in.c_str(), "%d//%d/%d", &out[0], &out[2], &out[3]);
		out[1] = 0;
	}
	return a;
}
