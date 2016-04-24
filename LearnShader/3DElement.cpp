#include "3DElement.h"


inline float mod(const float &l, const float &r)
{
	float t, e;
	modf(l / r, &t);
	e = t*r;
	return l - e;
}

inline void Coord_sph2car(float &angy, float &angz, const float dis, Vertex &v)
{
	v.z = dis * sin(angy*PI / 180) * cos(angz*PI / 180.0);
	v.x = dis * sin(angy*PI / 180) * sin(angz*PI / 180);
	v.y = dis * cos(angy*PI / 180);
}

inline void Coord_sph2car2(float &angy, float &angz, const float dis, Vertex &v)
{
	bool fix = false;
	if (angz >= 180)
		angz = mod(angz, 180), angy = mod(360 - angy, 360), fix = true;
	if (angy < 1e-6)
		angy = 360;
	v.z = dis * sin(angy*PI / 180) * cos(angz*PI / 180.0);
	v.x = dis * sin(angy*PI / 180) * sin(angz*PI / 180);
	if (fix && mod(angy, 180) < 1e-6)
		v.z *= -1, v.x *= -1;
	v.y = dis * cos(angy*PI / 180);
}



Coord2D Coord2D::operator+(const Coord2D &c) const
{
	return Coord2D(u + c.u, v + c.v);
}

Coord2D Coord2D::operator*(const float &n) const
{
	return Coord2D(u*n, v*n);
}



Vertex::Vertex()
{
#ifdef SSE2
	dat = _mm_setzero_ps();
#else
	x = y = z = 0;
#endif
}
Vertex::Vertex(const __m128 &idat)
{
	dat = idat;
}
float Vertex::length() const
{
#ifdef SSE4
	__m128 ans = _mm_dp_ps(dat, dat, 0b01110001);
	return _mm_cvtss_f32(_mm_sqrt_ss(ans));
#else
	return sqrt(x*x + y*y + z*z);
#endif
}
float Vertex::length_sqr() const
{
#ifdef SSE4
	__m128 ans = _mm_dp_ps(dat, dat, 0b01110001);
	return _mm_cvtss_f32(ans);
#else
	return x*x + y*y + z*z;
#endif
}
Vertex Vertex::muladd(const float & n, const Vertex & v) const
{
#ifdef FMA
	__m128 tmp = _mm_set1_ps(n);
	return _mm_fmadd_ps(dat, tmp, v);
#else
	return Vertex();
#endif
}
Vertex Vertex::mixmul(const Vertex & v) const
{
#ifdef SSE2
	return _mm_mul_ps(dat, v);
#else
	return Vertex(x * v.x, y * v.y, z * v.z);
#endif
}
Vertex Vertex::operator+(const Vertex &v) const
{
#ifdef SSE2
	return _mm_add_ps(dat, v);
#else
	return Vertex(x + v.x, y + v.y, z + v.z);
#endif
}
Vertex &Vertex::operator+=(const Vertex & right)
{
#ifdef SSE2
	return *this = _mm_add_ps(dat, right);
#else
	x += right.x, y += right.y, z += right.z;
	return *this;
#endif
}
Vertex Vertex::operator-(const Vertex &v) const
{
#ifdef SSE2
	return _mm_sub_ps(dat, v);
#else
	return Vertex(x - v.x, y - v.y, z - v.z);
#endif
}
Vertex &Vertex::operator-=(const Vertex & right)
{
#ifdef SSE2
	return *this = _mm_sub_ps(dat, right);
#else
	x += right.x, y += right.y, z += right.z;
	return *this;
#endif
}
Vertex Vertex::operator/(const float &n) const
{
	float rec = 1 / n;
#ifdef SSE2
	__m128 tmp = _mm_set1_ps(rec);
	return _mm_mul_ps(dat, tmp);
#else
	return Vertex(x * rec, y * rec, z * rec);
#endif
}
Vertex &Vertex::operator/=(const float & right)
{
	float rec = 1 / right;
#ifdef SSE2
	__m128 tmp = _mm_set1_ps(rec);
	return *this = _mm_mul_ps(dat, tmp);
#else
	x *= rec, y *= rec, z *= rec;
	return *this;
#endif
}
Vertex Vertex::operator*(const float &n) const
{
#ifdef SSE2
	__m128 tmp = _mm_set1_ps(n);
	return _mm_mul_ps(dat, tmp);
#else
	return Vertex(x * n, y * n, z * n);
#endif
}
Vertex &Vertex::operator*=(const float & right)
{
#ifdef SSE2
	__m128 tmp = _mm_set1_ps(right);
	return *this = _mm_mul_ps(dat, tmp);
#else
	x *= right, y *= right, z *= right;
	return *this;
#endif
}
Vertex Vertex::operator*(const Vertex &v) const
{
#ifdef AVX
	__m128 t1 = _mm_permute_ps(dat, _MM_SHUFFLE(3, 0, 2, 1)),
		t2 = _mm_permute_ps(v.dat, _MM_SHUFFLE(3, 1, 0, 2)),
		t3 = _mm_permute_ps(dat, _MM_SHUFFLE(3, 1, 0, 2)),
		t4 = _mm_permute_ps(v.dat, _MM_SHUFFLE(3, 0, 2, 1));
	__m128 l = _mm_mul_ps(t1, t2),
		r = _mm_mul_ps(t3, t4);
	return _mm_sub_ps(l, r);
#else
	float a, b, c;
	a = y*v.z - z*v.y;
	b = z*v.x - x*v.z;
	c = x*v.y - y*v.x;
	return Vertex(a, b, c);
#endif
}
float Vertex::operator&(const Vertex &v) const
{
#ifdef SSE4
	__m128 ans = _mm_dp_ps(dat, v.dat, 0b01110001);
	return _mm_cvtss_f32(ans);
#else
	return x*v.x + y*v.y + z*v.z;
#endif
}



Normal::Normal(const Vertex &v)//¹éÒ»»¯
{
#ifdef AVX2
	__m128 ans = _mm_dp_ps(v.dat, v.dat, 0b01110001);
	__m128 tmp = _mm_broadcastss_ps(_mm_sqrt_ss(ans));
	dat = _mm_div_ps(v.dat, tmp);
#else
#    ifdef SSE4
	__m128 ans = _mm_dp_ps(v.dat, v.dat, 0b01110001);
	ans = _mm_sqrt_ss(ans);
	__m128 tmp = _mm_set1_ps(_mm_cvtss_f32(ans));
	dat = _mm_div_ps(v.dat, tmp);
#    else
	float s = v.x*v.x + v.y*v.y + v.z*v.z;
	s = 1 / sqrt(s);
	x = v.x * s;
	y = v.y * s;
	z = v.z * s;
#    endif
#endif
}



Triangle::Triangle()
{
}

Triangle::Triangle(const Vertex &va, const Vertex &vb, const Vertex &vc)
{
	points[0] = va, points[1] = vb, points[2] = vc;
	norms[0] = Normal(), norms[1] = Normal(), norms[2] = Normal();
	tcoords[0] = Coord2D(), tcoords[1] = Coord2D(), tcoords[2] = Coord2D();
}

Triangle::Triangle(const Vertex &va, const Normal &na, const Vertex &vb, const Normal &nb, const Vertex &vc, const Normal &nc)
{
	points[0] = va, points[1] = vb, points[2] = vc;
	norms[0] = na, norms[1] = nb, norms[2] = nc;
	tcoords[0] = Coord2D(), tcoords[1] = Coord2D(), tcoords[2] = Coord2D();
}

Triangle::Triangle(const Vertex &va, const Normal &na, const Coord2D &ta, const Vertex &vb, const Normal &nb, const Coord2D &tb, const Vertex &vc, const Normal &nc, const Coord2D &tc)
{
	points[0] = va, points[1] = vb, points[2] = vc;
	norms[0] = na, norms[1] = nb, norms[2] = nc;
	tcoords[0] = ta, tcoords[1] = tb, tcoords[2] = tc;
}



Light::Light(const Type type)
{
	this->type = (int)type;
	bLight = true;
	rangy = 90, rangz = 0, rdis = 16;
	move(0, 0, 0);
	SetProperty((int)Property::Ambient, 0.05f, 0.05f, 0.05f);
	SetProperty((int)Property::Diffuse | (int)Property::Specular, 1.0f, 1.0f, 1.0f);
	SetProperty((int)Property::Atten, 1.0f, 0.0f, 0.0f);
	switch (type)
	{
	case Type::Parallel:
		position.alpha = 0.0f;
		break;
	case Type::Point:
		position.alpha = 1.0f;
		break;
	case Type::Spot:
		position.alpha = 1.0f;
		break;
	}
}

bool Light::turn()
{
	return bLight = !bLight;
}

void Light::move(const float dangy, const float dangz, const float ddis)
{
	rdis += ddis;
	if (rdis < 2)
		rdis = 2;
	else if (rdis > 64)
		rdis = 64;
	angy = rangy = mod(360 + rangy + dangy, 360);
	angz = rangz = mod(360 + rangz + dangz, 360);
	dis = rdis;

	Coord_sph2car2(angy, angz, dis, position);
}

void Light::SetProperty(const uint8_t prop, float r, float g, float b, float a)
{
	Vertex set(r, g, b, a);
	if (prop & uint8_t(Property::Ambient))
		ambient = set;
	if (prop & uint8_t(Property::Diffuse))
		diffuse = set;
	if (prop & uint8_t(Property::Specular))
		specular = set;
	if (prop & uint8_t(Property::Atten))
		attenuation = set;
	if (prop & uint8_t(Property::Position))
		position = set;
}

void Light::SetLumi(const float lum)
{
	float ext = lum / attenuation.alpha;
	attenuation.alpha = lum;
	ambient *= ext;
	diffuse *= ext;
	specular *= ext;
}



Material::Material()
{
	name = "simple";
	SetMtl((uint8_t)Property::Ambient | (uint8_t)Property::Diffuse, Vertex(0.588f, 0.588f, 0.588f));
	SetMtl((uint8_t)Property::Emission | (uint8_t)Property::Specular, Vertex());
	SetMtl((uint8_t)Property::Shiness, 10.0f);
	SetMtl((uint8_t)Property::Reflect | (uint8_t)Property::Refract, 0.0f);
	SetMtl((uint8_t)Property::RefractRate, 1.0f);
}

Material::~Material()
{
}

void Material::SetMtl(const uint8_t prop, const Vertex & v)
{
	if (prop & (uint8_t)Property::Ambient)
		ambient = v;
	if (prop & (uint8_t)Property::Diffuse)
		diffuse = v;
	if (prop & (uint8_t)Property::Emission)
		emission = v;
	if (prop & (uint8_t)Property::Specular)
		specular = v;
}

void Material::SetMtl(const uint8_t prop, const float val)
{
	if (prop & (uint8_t)Property::Shiness)
		shiness = val;
	if (prop & (uint8_t)Property::Reflect)
		shiness = val;
	if (prop & (uint8_t)Property::Refract)
		shiness = val;
	if (prop & (uint8_t)Property::RefractRate)
		shiness = val;
}



Camera::Camera(GLint w, GLint h)
{
	width = w, height = h;
	aspect = (float)w / h;
	fovy = 45.0, zNear = 1.0, zFar = 100.0;

	position = Vertex(0, 4, 15);
	u = Vertex(1, 0, 0);
	v = Vertex(0, 1, 0);
	n = Vertex(0, 0, -1);
}

void Camera::move(const float & x, const float & y, const float & z)
{
	position += u*x;
	position += v*y;
	position += n*z;
}

void Camera::yaw(const float angz)
{
	//rotate n(toward)
	float oangy = acos(n.y / 1) * 180 / PI,
		oangz = atan2(n.x, n.z) * 180 / PI;
	oangz -= angz;
	Coord_sph2car(oangy, oangz, 1, n);

	//rotate u(right)
	u = (v * n) * -1;
}

void Camera::pitch(float angy)
{
	//rotate n(toward)
	float oangy = acos(n.y / 1) * 180 / PI,
		oangz = atan2(n.x, n.z) * 180 / PI;
	if (oangy - angy < 1.0)
		angy = oangy - 1.0;
	if (oangy - angy > 179.0)
		angy = oangy - 179.0;
	oangy -= angy;
	Coord_sph2car(oangy, oangz, 1, n);

	//rotate v(up)
	v = u * n;
}

void Camera::resize(GLint w, GLint h)
{
	width = w, height = h;
	aspect = (float)w / h;
}
