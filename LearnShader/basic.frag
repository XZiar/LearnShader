#version 430

layout(location = 4) uniform vec3 camPos;

layout(std140) uniform lightBlock
{
	vec4 position;
	vec4 ambient, diffuse, specular, atten;
	float coang, exponent;
	int type;
	int isOpen;
} lights;

layout(std140) uniform materialBlock
{
	vec4 ambient, diffuse, specular, emission;
	float shiness, reflect, refract, rfr;
} material;

in perVert
{
	vec3 cAmbient,cDiffuse,cSpecular;
	vec3 pos;
	vec3 norm;
	vec2 texc;
};

out vec4 FragColor;

void main() 
{
	vec3 tmpColor = cAmbient;

	vec3 pNorm = normalize(norm);
	vec3 p2l = normalize(lights.position.rgb);
	tmpColor += cDiffuse * max(dot(pNorm, p2l), 0.0f);

	/* blinn-phong model */
	vec3 eyeRay = normalize(pos - camPos);
	vec3 h = normalize(p2l - eyeRay);
	float nn = max(dot(pNorm, h), 0.0f);
	tmpColor += cSpecular * nn * pow(nn, material.shiness);

	FragColor = vec4(tmpColor, 1.0f);
}