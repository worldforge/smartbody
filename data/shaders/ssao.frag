#version 430
//#include "normal.glsl"

layout(binding=0) uniform sampler2D uGBufferGeometricTex;
layout(binding=1) uniform sampler2D uGBufferDepthTex;
layout(binding=2) uniform sampler2D uNoiseTex;
layout(binding=3) uniform sampler2D uDiffuseTex;

uniform mat4 uProjectionMatrix; // current projection matrix, for linearized depth

//	ssao uniforms:
const int MAX_KERNEL_SIZE = 128;
uniform int uKernelSize;
uniform vec3 uKernelOffsets[MAX_KERNEL_SIZE];
uniform float uRadius = 15.0;
uniform float uPower = 1.0;
uniform float farPlane;
uniform float nearPlane;

varying vec2 texCoord; 
varying vec3 viewRay;

layout(location=0) out vec4 fResult;

float linearizeDepth(in float depth, in mat4 projMatrix) {
	//return -projMatrix[3][2] / (-depth - projMatrix[2][2]);	
	float depthSample = depth;//2.0 * depth - 1.0;
	return 2.0*nearPlane*farPlane/(farPlane + nearPlane - depthSample*(farPlane-nearPlane));
}

vec3 decodeNormal(in vec3 normal) {
	vec3 n = normal;
	n = n * 2.0 - 1.0;
	vec3 result = n;
	return result;
}


/*----------------------------------------------------------------------------*/
float ssao(in mat3 kernelBasis, in vec3 originPos, in float radius) {
	float occlusion = 0.0;
	for (int i = 0; i < uKernelSize; ++i) {
	//	get sample position:
		vec3 samplePos = kernelBasis * uKernelOffsets[i];
		samplePos = samplePos * radius + originPos;
		
	//	project sample position:
		vec4 offset = uProjectionMatrix * vec4(samplePos, 1.0);
		offset.xy /= offset.w; // only need xy
		offset.xy = offset.xy * 0.5 + 0.5; // scale/bias to texcoords
		
	//	get sample depth:
		float sampleDepth = texture(uGBufferDepthTex, offset.xy).r;
		sampleDepth = -linearizeDepth(sampleDepth, uProjectionMatrix);
		
		float edgeError = step(0.0, offset.x) * step(0.0, 1.0 - offset.x) *
                      step(0.0, offset.y) * step(0.0, 1.0 - offset.y);

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(originPos.z - sampleDepth));
		occlusion += rangeCheck*step(samplePos.z, sampleDepth)*edgeError;
		//occlusion += rangeCheck*step(sampleDepth, samplePos.z);
		//occlusion += rangeCheck;
	}
	
	occlusion = 1.0 - (occlusion / float(uKernelSize));
	//occlusion = (occlusion / float(uKernelSize));	
	return (pow(occlusion, uPower));
}

/*----------------------------------------------------------------------------*/
void main() {
//	get noise texture coords:
	vec2 noiseTexCoords = vec2(textureSize(uGBufferDepthTex, 0)) / vec2(textureSize(uNoiseTex, 0));
	noiseTexCoords *= texCoord;
	
//	get view space origin:
	float originDepth = texture(uGBufferDepthTex, texCoord).r;
	if (originDepth == 1.0) discard;
	float linearizeDepth = linearizeDepth(originDepth, uProjectionMatrix);
	vec3 originPos = vec3(viewRay.xy * linearizeDepth, -linearizeDepth);
	//vec3 originPos = texture(uGBufferDepthTex, texCoord).xyz;

//	get view space normal:
	//vec3 normal = decodeNormal(texture(uGBufferGeometricTex, texCoord).rgb);
	vec3 normal = (texture(uGBufferGeometricTex, texCoord).rgb);
	
//	construct kernel basis matrix:
	vec3 rvec = texture(uNoiseTex, noiseTexCoords).rgb * 2.0 - 1.0;
	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	vec3 bitangent = cross(tangent, normal);
	mat3 kernelBasis = mat3(tangent, bitangent, normal);
	
	float ssaoVal = ssao(kernelBasis, originPos, uRadius);
	//fResult = vec4(ssao(kernelBasis, originPos, uRadius));
	vec4 diffuseColor = texture(uDiffuseTex, texCoord);
	//fResult = diffuseColor*vec4(ssaoVal, ssaoVal, ssaoVal, 1.0);	
	fResult = vec4(ssaoVal, ssaoVal, ssaoVal, 1.0);	
	//fResult = vec4(normal, 1.0);
	//fResult = vec4(originPos*0.02, 1.0);
	//float testDepth = abs(linearizeDepth)/farPlane;//exp(originPos.y);//abs(originPos.z * 100.0);
	//fResult = vec4(testDepth, testDepth, testDepth, 1.0);
}