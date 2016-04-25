#version 430

struct LightInfo
{
	vec4 position;
	vec4 ambient, diffuse, specular, atten;
	float coang, exponent;
	int type;
	bool isOpen;
};

layout(location = 0) uniform mat4 projMat;
layout(location = 1) uniform mat4 viewMat;
layout(location = 2) uniform mat4 modelMat;
layout(location = 3) uniform mat4 normMat;

layout(std140) uniform lightBlock
{
	LightInfo lights[8];
};
layout(std140) uniform materialBlock
{
	vec4 ambient, diffuse, specular, emission;
	float shiness, reflect, refract, rfr;
} material;

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNorm;
layout(location = 2) in vec3 vertColor;
layout(location = 3) in vec2 vertTexc;

out perVert
{
	vec3 pos;
	vec3 norm;
	vec2 texc;
};
out lightDat
{
	vec3 cAmbient,cDiffuse,cSpecular;
} lgtDat[8];

void main() 
{
	pos = (modelMat * vec4(vertPos, 1.0f)).rgb;
	gl_Position = projMat * viewMat * vec4(pos, 1.0f);
	norm = (normMat * vec4(vertNorm, 1.0f)).rgb;

	for(int id = 0; id < 2; id++)
	{
		lgtDat[id].cAmbient = (material.ambient * lights[id].ambient + material.emission).rgb;
		lgtDat[id].cDiffuse = (material.diffuse * lights[id].diffuse).rgb;
		lgtDat[id].cSpecular = (material.specular * lights[id].specular).rgb;
	}
}