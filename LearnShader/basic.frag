#version 430

in perVert
{
	vec3 cAmbient,cDiffuse,cSpecular;
	vec3 norm;
	vec2 texc;
};

out vec4 FragColor;

void main() 
{
	FragColor = vec4(cAmbient + cDiffuse + cSpecular, 1.0f);
}