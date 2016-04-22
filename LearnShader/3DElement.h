#pragma once

#include "rely.h"

class Coord2D
{
public:
	float u, v;
	Coord2D() { u = v = 0.0; };
	Coord2D(const float &iu, const float &iv) :u(iu), v(iv) { };

	Coord2D operator+(const Coord2D &c) const;
	Coord2D operator*(const float &n) const;
	operator float*() { return &u; };
};

_MM_ALIGN16 class Vertex
{
public:
	union
	{
		__m128 dat;
		struct
		{
			float x, y, z, w;
		};
		struct
		{
			float r, g, b, alpha;
		};
	};
	Vertex();
	Vertex(const __m128 &idat);
	Vertex(const float ix, const float iy, const float iz, const float ia = 0) :x(ix), y(iy), z(iz), w(ia) { };
	operator float*() { return &x; };
	operator __m128() const { return dat; };
	operator glm::tvec3<float, glm::highp>() const { return vec3(x, y, z); };

	float length() const;
	float length_sqr() const;
	Vertex muladd(const float &n, const Vertex &v) const;
	Vertex mixmul(const Vertex &v) const;

	Vertex operator+(const Vertex &v) const;
	Vertex &operator+=(const Vertex &right);
	Vertex operator-(const Vertex &v) const;
	Vertex &operator-=(const Vertex &right);
	Vertex operator/(const float &n) const;
	Vertex &operator/=(const float &right);
	Vertex operator*(const float &n) const;
	Vertex &operator*=(const float &right);
	Vertex operator*(const Vertex &v) const;
	float operator&(const Vertex &v) const;//点积
};

class Normal : public Vertex
{
public:
	Normal() : Vertex() { };
	Normal(const float &ix, const float &iy, const float &iz, const float &iw = 0.0f) :Vertex(ix, iy, iz, iw) { };
	Normal(const Vertex &v);//归一化
};

class Point
{
public:
	Vertex pos;
	Normal norm;
	Coord2D tcoord;

	Point() { };
	Point(const Vertex &v, const Normal &n, const Coord2D &t) : pos(v), norm(n), tcoord(t) { };
};

class Triangle
{
public:
	Vertex points[3];
	Normal norms[3];
	Coord2D tcoords[3];

	Triangle();
	Triangle(const Vertex &va, const Vertex &vb, const Vertex &vc);
	Triangle(const Vertex &va, const Normal &na, const Vertex &vb, const Normal &nb, const Vertex &vc, const Normal &nc);
	Triangle(const Vertex &va, const Normal &na, const Coord2D &ta, const Vertex &vb, const Normal &nb, const Coord2D &tb, const Vertex &vc, const Normal &nc, const Coord2D &tc);
};

class Camera
{
public:
	Normal u, v, n;//to right,up,toward
	Vertex position;
	GLint width, height;
	float fovy, aspect, zNear, zFar;
	Camera(GLint w = 1120, GLint h = 630);
	void move(const float &x, const float &y, const float &z);
	void yaw(const float angz);
	void pitch(float angy);
	void resize(GLint w, GLint h);
};