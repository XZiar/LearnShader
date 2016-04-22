#version 430

uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 modelMat;

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNorm;
layout(location = 2) in vec3 vertColor;
layout(location = 3) in vec2 vertTex;

out perVert
{
	vec3 color;
	vec3 norm;
};

void main() 
{
	gl_Position = projMat * viewMat * modelMat * vec4(vertPos, 1.0f);
	color = vertNorm;
	norm = vertNorm;
}