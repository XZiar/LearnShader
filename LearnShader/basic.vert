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
	bool isOpen;
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
	vec3 color;
	vec3 norm;
	vec2 texc;
};

void main() 
{
	gl_Position = projMat * viewMat * modelMat * vec4(vertPos, 1.0f);
	vec4 tmp = normMat * vec4(vertNorm, 1.0f);
	norm = tmp.rgb;

	vec3 baseColor = vec3(0.644f, 0.644f, 0.644f);
	baseColor = norm;

	color = baseColor * lights.ambient;

	vec3 lightRay = normalize(lights.position - vec4(vertPos, 1.0f)).rgb;
	vec3 p2l = normalize(lights.position).rgb;
	color += baseColor * lights.diffuse * max(dot(lightRay, p2l), 0.0f);

	/* blinn-phong model */
	vec3 eyeRay = normalize(gl_Position.rgb - camPos);
	vec3 h = normalize(p2l - eyeRay);
	float nn = max(dot(lightRay, h), 0.0f);
	color += baseColor * lights.specular * nn * pow(nn, 10);

}