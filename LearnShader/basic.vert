#version 430

uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 modelMat;

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec3 tex;
out vec3 color;
void main() 
{
	gl_Position = projMat * viewMat * modelMat * vec4(vert, 1.0f);
	color = (vert - vec3(-1, -1, -1)) / 2;
	color = norm;
}