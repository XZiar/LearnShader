#version 430

layout(location = 0) uniform mat4 projMat;
layout(location = 1) uniform mat4 viewMat;
layout(location = 2) uniform mat4 modelMat;
layout(location = 3) uniform mat4 normMat;

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNorm;
layout(location = 2) in vec3 vertColor;
layout(location = 3) in vec2 vertTexc;

out perVert
{
	vec3 color;
	vec3 norm;
	vec2 texc;
};

void main() 
{
	gl_Position = projMat * viewMat * modelMat * vec4(vertPos, 1.0f);
	vec4 tmp = normalize(normMat * vec4(vertNorm, 1.0f));
	norm = tmp.rgb;
	color = norm;
}