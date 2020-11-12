#version 430
#define MAX_LIGHTS 20
layout(binding=0) uniform sampler2D uNormalTex;
layout(binding=1) uniform sampler2D uSSAOTex;
layout(binding=2) uniform sampler2D uDiffuseTex;

uniform int numLights = 2;
uniform vec3 lightDir[MAX_LIGHTS];
uniform vec3 lightColor[MAX_LIGHTS];

varying vec2 texCoord;

layout(location=0) out vec4 fResult;

vec3 decodeNormal(in vec3 normal) {
	vec3 n = normal;
	n = n * 2.0 - 1.0;
	vec3 result = n;
	return result;
}


/*----------------------------------------------------------------------------*/
void main() {	
	vec3 lcolor = vec3(0,0,0);	
	//vec3 normal = decodeNormal(texture(uNormalTex, texCoord).rgb);
	vec3 normal = texture(uNormalTex, texCoord).rgb;
	vec4 diffuseColor = texture(uDiffuseTex, texCoord);
	float ssaoVal = texture(uSSAOTex, texCoord).r;

	for (int i=0;i<numLights;i++)
	{
		float ndotl = max(dot(lightDir[i], normal), 0.f);
		lcolor += lightColor[i]*ndotl;
	}
	fResult = vec4(lcolor*diffuseColor.xyz*ssaoVal, 1.f);
	//fResult = vec4(lcolor, 1.0);
}