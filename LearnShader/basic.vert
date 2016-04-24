#version 430

layout(location = 0) uniform mat4 projMat;
layout(location = 1) uniform mat4 viewMat;
layout(location = 2) uniform mat4 modelMat;
layout(location = 3) uniform mat4 normMat;
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

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNorm;
layout(location = 2) in vec3 vertColor;
layout(location = 3) in vec2 vertTexc;

out perVert
{
	vec3 cAmbient,cDiffuse,cSpecular;
	vec3 pos;
	vec3 norm;
	vec2 texc;
};

void main() 
{
	pos = (modelMat * vec4(vertPos, 1.0f)).rgb;
	gl_Position = projMat * viewMat * vec4(pos, 1.0f);
	norm = (normMat * vec4(vertNorm, 1.0f)).rgb;

	cAmbient = (material.ambient * lights.ambient + material.emission).rgb;
	cDiffuse = (material.diffuse * lights.diffuse).rgb;
	cSpecular = (material.specular * lights.specular).rgb;
}