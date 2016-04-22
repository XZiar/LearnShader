#version 430

in perVert
{
	vec3 color;
	vec3 norm;
	vec2 texc;
};

out vec4 FragColor;

void main() 
{
	FragColor = vec4(color, 1.0f);
}