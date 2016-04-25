#version 430

struct LightInfo
{
	vec4 position;
	vec4 ambient, diffuse, specular, atten;
	float coang, exponent;
	int type;
	bool isOpen;
};

layout(location = 4) uniform vec3 camPos;

layout(std140) uniform lightBlock
{
	LightInfo lights[8];
};
layout(std140) uniform materialBlock
{
	vec4 ambient, diffuse, specular, emission;
	float shiness, reflect, refract, rfr;
} material;

in perVert
{
	vec3 pos;
	vec3 norm;
	vec2 texc;
};
in lightDat
{
	vec3 cAmbient,cDiffuse,cSpecular;
} lgtDat[8];

out vec4 FragColor;

vec3 Light_Parallel(int id)
{
	vec3 tmpColor = lgtDat[id].cAmbient;

	vec3 pNorm = normalize(norm);
	vec3 p2l = normalize(lights[id].position.rgb);
	tmpColor += lgtDat[id].cDiffuse * max(dot(pNorm, p2l), 0.0f);

	/* blinn-phong model */
	vec3 eyeRay = normalize(pos - camPos);
	vec3 h = normalize(p2l - eyeRay);
	float nn = max(dot(pNorm, h), 0.0f);
	tmpColor += lgtDat[id].cSpecular * nn * pow(nn, material.shiness);

	return tmpColor;
}

vec3 Light_Point(int id)
{
	vec3 tmpColor = lgtDat[id].cAmbient;

	vec3 pNorm = normalize(norm);
	vec3 p2l = normalize(lights[id].position.rgb);
	tmpColor += lgtDat[id].cDiffuse * max(dot(pNorm, p2l), 0.0f);

	/* blinn-phong model */
	vec3 eyeRay = normalize(pos - camPos);
	vec3 h = normalize(p2l - eyeRay);
	float nn = max(dot(pNorm, h), 0.0f);
	tmpColor += lgtDat[id].cSpecular * nn * pow(nn, material.shiness);

	tmpColor /= 3;
	tmpColor.g = 0.0f;
	return tmpColor;
}

void main() 
{
	vec3 tmpColor = vec3(0.0f);
	for(int id = 0; id < 8; id++)
	{
		if(lights[id].isOpen)
		{
			switch(lights[id].type)
			{
			case 1:
				tmpColor += Light_Parallel(id); break;
			case 2:
				tmpColor += Light_Point(id); break;
			}
		}
	}
	FragColor = vec4(tmpColor, 1.0f);
}