/*************************************************************
Copyright (C) 2017 University of Southern California

This file is part of Smartbody.

Smartbody is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Smartbody is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Smartbody.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************/

#include <sb/SBTypes.h>

#if!defined(EMSCRIPTEN)
#include "GL/glew.h"
#endif
#include "SbmDeformableMeshGPU.h"
#include "SbmTexture.h"
#include <string>
#include <set>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#if !defined(EMSCRIPTEN)
typedef std::pair<int,float> IntFloatPair;

static bool intFloatComp(const IntFloatPair& p1, const IntFloatPair& p2)
{
	return (p1.second > p2.second);
}


const char* ShaderDir = "../../smartbody-lib/src/sbm/GPU/shaderFiles/";
const char* VSName = "vs_skin_pos.vert";
const char* FSName = "fs_skin_render.frag";
const std::string shaderName = "Skin_Basic";
const std::string shaderFaceName = "Skin_Face";
const std::string shadowShaderName = "Skin_Shadow";
bool SbmDeformableMeshGPU::initShader = false;
bool SbmDeformableMeshGPU::useGPUDeformableMesh = false;
bool SbmDeformableMeshGPU::disableRendering = false;
bool SbmDeformableMeshGPU::useShadowPass = false;
GLuint SbmDeformableMeshGPU::shadowMapID = -1;

std::string shaderVS = 
"#version 120 \n\
#extension GL_EXT_gpu_shader4 : require \n\
uniform samplerBuffer Transform; \n\
attribute vec4 BoneID1,BoneID2;   \n\
attribute vec4 BoneWeight1,BoneWeight2;\n \
attribute vec3 tangent, binormal;\n\
varying vec4 vPos;\n\
varying vec3 normal,lightDir[2],halfVector[2];\n\
varying vec3 tv,bv;\n\
varying float dist[2];\n\
mat3 GetTransformation(float id)\n \
{ \n\
	int idx = int(id);\n \
	mat3 rot;\n  \
	for (int i=0;i<3;i++)\n \
	{ \n  \
		for (int j=0;j<3;j++)\n \
			rot[j][i] = texelFetchBuffer(Transform,(idx*16+i*4+j)).x;\n		\
	}\n	\
	return rot;\n \
}\n \
vec3 GetTranslation(float id)\n \
{\n  \
	int idx = int(id);\n \
	vec3 tran;\n \
	tran[0] = texelFetchBuffer(Transform,(idx*16+12)).x;\n \
	tran[1] = texelFetchBuffer(Transform,(idx*16+13)).x;\n \
	tran[2] = texelFetchBuffer(Transform,(idx*16+14)).x;\n \
	return tran;\n	\
}\n  \
mat4 TransformPos(vec3 position, vec3 normal, vec3 tang, vec3 binorm, vec4 boneid, vec4 boneweight)\n\
{\n\
	vec3 pos = vec3(0,0,0);\n\
	vec3 n = vec3(0,0,0);\n\
	vec3 t = vec3(0,0,0);\n\
	vec3 b = vec3(0,0,0);\n\
	mat3 tempT;\n\
	vec3 tempt;\n\
	for (int i=0;i<4;i++)\n\
	{\n\
		tempT = GetTransformation(boneid[i]);\n\
		tempt = GetTranslation(boneid[i]);\n\
		pos += (position*tempT+tempt)*boneweight[i];\n\
		n   += (normal*tempT)*boneweight[i];\n\
		t   += (tang*tempT)*boneweight[i];\n\
		b   += (binorm*tempT)*boneweight[i];\n\
	}	\n\
	mat4 result;\n\
	result[0] = vec4(pos,1.0);\n\
	result[1] = vec4(n,1.0);\n\
	result[2] = vec4(t,1.0);\n\
	result[3] = vec4(b,1.0);\n\
	return result;\n\
}\n\
void main()\n \
{\n	\
	// the following three lines provide the same result\n \
	vec3 pos = vec3(gl_Vertex.xyz);\n \
	mat4 skin = TransformPos(pos,gl_Normal,tangent,binormal,BoneID1,BoneWeight1) + TransformPos(pos,gl_Normal,tangent,binormal,BoneID2,BoneWeight2);\n\
	vPos = gl_TextureMatrix[7]* gl_ModelViewMatrix*vec4(skin[0].xyz,1.0);\n\
	//vPos = gl_TextureMatrix[7]* gl_ModelViewMatrix*vec4(gl_Vertex.xyz,1.0);\n\
	gl_Position = gl_ModelViewProjectionMatrix*vec4(skin[0].xyz,1.0);\n\
	lightDir[0] = normalize((vec4(gl_LightSource[0].position.xyz,0.0)).xyz);\n\
	halfVector[0] = normalize((vec4(gl_LightSource[0].halfVector.xyz,0.0)).xyz);\n\
	dist[0] = 0.0;\n\
	vec3 posDir = vec3(gl_LightSource[1].position);// - gl_ModelViewMatrix * vec4(skin[0].xyz,1.0));\n\
	dist[1] = 0.0;//length(posDir);\n\
	lightDir[1] = normalize(( vec4(posDir,0.0)).xyz);\n\
	halfVector[1] = normalize((vec4(gl_LightSource[1].halfVector.xyz,0.0)).xyz);\n\
	int colorIdx = int(gl_Vertex.w); \n\
	gl_TexCoord[0] = gl_MultiTexCoord0;\n\
	normal = normalize(gl_NormalMatrix * skin[1].xyz);\n\
	tv     = normalize(gl_NormalMatrix * tangent.xyz);\n\
	bv     = normalize(gl_NormalMatrix * binormal.xyz);\n\
}\n";


std::string shaderVS_2 = 
"#version 120 \n\
const int NumLight =2;\n\
uniform mat4 Transform[120]; \n\
uniform int  updateNormal;\n\
uniform float meshScale;\n\
attribute vec4 BoneID1,BoneID2;   \n\
attribute vec4 BoneWeight1,BoneWeight2;\n \
attribute vec3 tangent, binormal;\n\
varying vec4 vPos;\n\
varying vec3 normal,lightDir[NumLight],halfVector[NumLight];\n\
varying vec3 tv,bv;\n\
mat4 GetTransformation(float id)\n \
{ \n\
int idx = int(id);\n \
return Transform[idx];\n \
}\n \
mat4 TransformPos(vec3 position, vec3 normal, vec3 tang, vec3 binorm, vec4 boneid, vec4 boneweight)\n\
{\n\
vec3 pos = vec3(0,0,0);\n\
vec3 n = vec3(0,0,0);\n\
vec3 t = vec3(0,0,0);\n\
vec3 b = vec3(0,0,0);\n\
mat4 tempT;\n\
vec3 tempt;\n\
for (int i=0;i<4;i++)\n\
{\n\
tempT = GetTransformation(boneid[i]);\n\
pos += (vec4(position,1.0)*tempT).xyz*boneweight[i];\n\
n   += (vec4(normal,0.0)*tempT).xyz*boneweight[i];\n\
t   += (vec4(tang,0.0)*tempT).xyz*boneweight[i];\n\
b   += (vec4(binorm,0.0)*tempT).xyz*boneweight[i];\n\
}	\n\
mat4 result;\n\
result[0] = vec4(pos,1.0);\n\
result[1] = vec4(n,1.0);\n\
result[2] = vec4(t,1.0);\n\
result[3] = vec4(b,1.0);\n\
return result;\n\
}\n\
void main()\n \
{\n	\
// the following three lines provide the same result\n \
vec3 pos = vec3(gl_Vertex.xyz)*meshScale;\n \
mat4 skin = TransformPos(pos,gl_Normal,tangent,binormal,BoneID1,BoneWeight1) + TransformPos(pos,gl_Normal,tangent,binormal,BoneID2,BoneWeight2);\n\
vPos = gl_TextureMatrix[7]* gl_ModelViewMatrix * vec4(skin[0].xyz,1.0);\n\
gl_Position = gl_ModelViewProjectionMatrix*vec4(skin[0].xyz,1.0);\n\
//vPos = gl_TextureMatrix[7]* gl_ModelViewMatrix * vec4(pos.xyz,1.0);\n\
//gl_Position = gl_ModelViewProjectionMatrix*vec4(pos.xyz,1.0);\n\
//dist[0] = 0.0;\n\
//vec3 posDir = vec3(gl_LightSource[1].position);// - gl_ModelViewMatrix * vec4(skin[0].xyz,1.0));\n\
//dist[1] = 0.0;//length(posDir);\n\
//lightDir[0] = normalize((vec4(gl_LightSource[0].position.xyz,0.0)).xyz);\n\
//halfVector[0] = normalize((vec4(gl_LightSource[0].halfVector.xyz,0.0)).xyz);\n\
for (int i=0;i<NumLight;i++)\n\
{\n\
vec3 posDir = vec3(gl_LightSource[i].position);\n\
vec4 hv = vec4(gl_LightSource[i].halfVector);\n\
lightDir[i] = normalize(posDir);\n\
halfVector[i] = normalize(hv.xyz);\n\
}\n\
int colorIdx = int(gl_Vertex.w); \n\
gl_TexCoord[0] = gl_MultiTexCoord0;\n\
//if (updateNormal == 1 )\n\
//{\n\
//normal = normalize(gl_NormalMatrix * skin[1].xyz);\n\
//tv     = normalize(gl_NormalMatrix * skin[2].xyz);\n\
//bv     = normalize(gl_NormalMatrix * skin[3].xyz);\n\
//}\n\
//else {\n\
normal = normalize(gl_NormalMatrix * gl_Normal.xyz);\n\
tv     = normalize(gl_NormalMatrix * tangent.xyz);\n\
bv     = normalize(gl_NormalMatrix * binormal.xyz);\n\
//}\n\
}\n";

std::string shaderVS_Weight4 =
"#version 120 \n\
	const int MaxLights =8;\n\
	uniform int numLights=2;\n\
	uniform mat4 Transform[120]; \n\
	uniform int  updateNormal;\n\
	uniform float meshScale;\n\
	attribute vec4 BoneID1;   \n\
	attribute vec4 BoneWeight1;\n \
	attribute vec3 tangent, binormal;\n\
	varying vec4 vPos;\n\
	varying vec3 normal,lightDir[MaxLights],halfVector[MaxLights];\n\
	varying vec3 tv,bv;\n\
	mat4 GetTransformation(float id)\n \
	{ \n\
	int idx = int(id);\n \
	return Transform[idx];\n \
	}\n \
	mat4 TransformPos(vec3 position, vec3 normal, vec3 tang, vec3 binorm, vec4 boneid, vec4 boneweight)\n\
	{\n\
	vec3 pos = vec3(0,0,0);\n\
	vec3 n = vec3(0,0,0);\n\
	vec3 t = vec3(0,0,0);\n\
	vec3 b = vec3(0,0,0);\n\
	mat4 tempT;\n\
	vec3 tempt;\n\
	for (int i=0;i<4;i++)\n\
	{\n\
	tempT = GetTransformation(boneid[i]);\n\
	pos += (vec4(position,1.0)*tempT).xyz*boneweight[i];\n\
	n   += (vec4(normal,0.0)*tempT).xyz*boneweight[i];\n\
	t   += (vec4(tang,0.0)*tempT).xyz*boneweight[i];\n\
	b   += (vec4(binorm,0.0)*tempT).xyz*boneweight[i];\n\
	}	\n\
	mat4 result;\n\
	result[0] = vec4(pos,1.0);\n\
	result[1] = vec4(n,1.0);\n\
	result[2] = vec4(t,1.0);\n\
	result[3] = vec4(b,1.0);\n\
	return result;\n\
	}\n\
	void main()\n \
	{\n	\
	// the following three lines provide the same result\n \
	vec3 pos = vec3(gl_Vertex.xyz)*meshScale;\n \
	mat4 skin = TransformPos(pos,gl_Normal,tangent,binormal,BoneID1,BoneWeight1);\n\
	vPos = gl_TextureMatrix[7]* gl_ModelViewMatrix * vec4(skin[0].xyz,1.0);\n\
	gl_Position = gl_ModelViewProjectionMatrix*vec4(skin[0].xyz,1.0);\n\
	for (int i=0;i<numLights;i++)\n\
	{\n\
	vec3 posDir = vec3(gl_LightSource[i].position);\n\
	vec4 hv = vec4(gl_LightSource[i].halfVector);\n\
	lightDir[i] = normalize(posDir);\n\
	halfVector[i] = normalize(hv.xyz);\n\
	}\n\
	int colorIdx = int(gl_Vertex.w); \n\
	gl_TexCoord[0] = gl_MultiTexCoord0;\n\
	normal = normalize(gl_NormalMatrix * gl_Normal.xyz);\n\
	tv     = normalize(gl_NormalMatrix * tangent.xyz);\n\
	bv     = normalize(gl_NormalMatrix * binormal.xyz);\n\
	}\n";

std::string shaderBasicFS =
"void main (void)\n\
{  \n\
gl_FragColor = vec4(1,1,1,1);\n\
}";

std::string shaderFS =
"#version 120\n\
const int MaxLights = 8;\n\
const vec3 ambient = vec3(0.0,0.0,0.0);//(vec3(255 + 127, 241, 0 + 2)/255.0)*(vec3(0.2,0.2,0.2));\n\
uniform int numLights = 2;\n\
uniform sampler2D diffuseTexture;\n\
uniform sampler2D normalTexture;\n\
uniform sampler2D specularTexture;\n\
uniform sampler2D glossyTexture;\n\
uniform sampler2D tex;\n\
uniform int  useTexture;\n\
uniform int  useNormalMap;\n\
uniform int  useSpecularMap;\n\
uniform int  useShadowMap;\n\
varying vec3 normal,lightDir[MaxLights],halfVector[MaxLights];\n\
varying vec3 tv,bv;\n\
varying vec4 vPos;\n\
uniform vec4 diffuseMaterial;\n\
uniform vec4 specularMaterial;\n\
uniform float  shineness;\n\
//uniform vec3 specularColors;\n\
float shadowCoef()\n\
{\n\
int index = 0;\n\
vec4 shadow_coord = vPos/vPos.w;\n\
shadow_coord.z += 0.000005;\n\
float shadow_d = texture2D(tex, shadow_coord.st).x;\n\
float diff = 1.0;\n\
diff = (shadow_d - shadow_coord.z);\n\
return clamp( diff*850.0 + 1.0, 0.0, 1.0);//(shadow_d-0.9)*10;//clamp( diff*250.0 + 1.0, 0.0, 1.0);\n\
}\n\
void main (void)\n\
{  \n\
	vec3 n,halfV;\n\
	float NdotL,NdotHV;\n\
	float att;\n\
	vec4 color = vec4(ambient,1.0);	\n\
	vec3 newtv = tv;//normalize(tv - bv*dot(tv,bv));\n\
	vec3 newbv = bv;//normalize(bv);\n\
	vec3 newn  = normal;//normalize(normal);//normalize(cross(tv,bv));\n\
	vec4 texColor = texture2D(diffuseTexture,gl_TexCoord[0].st);\n\
	vec3 normalColor = normalize(texture2D(normalTexture,gl_TexCoord[0].st).xyz* 2.0 - 1.0);\n\
	vec3 normalMapN = normalize(-newtv*normalColor.x-newbv*normalColor.y+newn*normalColor.z); \n\
	vec4 specularColor = texture2D(specularTexture, gl_TexCoord[0].st);\n\
	vec3 specMat = specularMaterial.rgb;\n\
	float glossy = 1.0;\n\
	if (useTexture == 0) \n\
		texColor = diffuseMaterial;//vec4(matColor,1.0);\n\
	color.a = texColor.a*diffuseMaterial.a;\n\
	//color.rgb = texColor.rgb;\n\
	n = normalize(normal);\n\
	if (useNormalMap == 1 && dot(normalMapN,n) > 	0.0)\n\
	{\n\
		n = normalMapN;\n\
	}\n\
	if (useSpecularMap == 1)\n\
	{\n\
		specMat = specularColor.rgb;\n\
		glossy = max(0.001,specularColor.a);\n\
	}\n\
	float shadowWeight = 1.0;\n\
	if (useShadowMap == 1)\n\
	{\n\
		shadowWeight = shadowCoef();\n\
	}\n\
	for (int i=0;i<numLights;i++)\n\
	{\n\
		//att = 1.0;//1.0/(gl_LightSource[i].constantAttenuation + gl_LightSource[i].linearAttenuation * dist[i] + gl_LightSource[i].quadraticAttenuation * dist[i] * dist[i]);	\n\
		vec3 posDir = lightDir[i];//normalize(vec3(gl_LightSource[i].position));\n\
		NdotL = max(dot(n,posDir),0.0);\n\
		if (NdotL > 0.0) {\n\
			//color += vec4(texColor.xyz*gl_LightSource[i].diffuse.xyz*NdotL,0)*att;\n\
			color += vec4(texColor.xyz*gl_LightSource[i].diffuse.xyz*NdotL,0);\n\
			//color += vec4(texColor.xyz*NdotL,0);\n\
			//color += NdotL;\n\
			halfV = normalize(halfVector[i]);\n\
			NdotHV = max(dot(n,halfV),0.0);\n\
			//color += vec4(specMat.rgb*pow(NdotHV, 10),0);\n\
			//color += vec4(specMat.rgb*pow(NdotHV, (shineness+1.0)*glossy),0);\n\
			color += vec4(specMat.rgb*pow(NdotHV, (shineness+1.0)),0);\n\
			//color += vec4(specMat.rgb*pow(NdotHV, shineness+1.0),0)*att;\n\
		}   \n\
	}\n\
	const float shadow_ambient = 1.0;\n\
	gl_FragColor = vec4(color.rgb,color.a);//*shadowWeight*shadow_ambient,color.a);//color*shadow_ambient*shadowWeight;//vec4(color.rgb*shadowWeight,color.a);//color*shadowWeight;\n\
	//gl_FragColor = vec4(n.xyz, 1.0);\n\
	//gl_FragColor = vec4(color.rgb, color.a);\n\
	//gl_FragColor = vec4(1.0,0.0,0.0,1.0);\n\
}";

std::string shaderFSFace =
	"const int MaxLights = 8;\n\
	const vec3 ambient = vec3(0.0,0.0,0.0);//(vec3(255 + 127, 241, 0 + 2)/255.0)*(vec3(0.2,0.2,0.2));\n\
	const vec3 sssColor = vec3(0.6,0.6,0.6);\n\
	const float aspExp = 5.0;\n\
	const float aspIntensity = 1.0;\n\
	uniform int numLights = 2;\n\
	uniform sampler2D diffuseTexture;\n\
	uniform sampler2D normalTexture;\n\
	uniform sampler2D specularTexture;\n\
	uniform sampler2D tex;\n\
	uniform int  useTexture;\n\
	uniform int  useNormalMap;\n\
	uniform int  useSpecularMap;\n\
	uniform int  useShadowMap;\n\
	varying vec3 normal,lightDir[MaxLights],halfVector[MaxLights];\n\
	varying vec3 tv,bv;\n\
	varying vec4 vPos;\n\
	uniform vec4 diffuseMaterial;\n\
	uniform vec4 specularMaterial;\n\
	uniform float  shineness;\n\
	//uniform vec3 specularColors;\n\
	float shadowCoef()\n\
	{\n\
	int index = 0;\n\
	vec4 shadow_coord = vPos/vPos.w;\n\
	shadow_coord.z += 0.000005;\n\
	float shadow_d = texture2D(tex, shadow_coord.st).x;\n\
	float diff = 1.0;\n\
	diff = (shadow_d - shadow_coord.z);\n\
	return clamp( diff*850.0 + 1.0, 0.0, 1.0);//(shadow_d-0.9)*10;//clamp( diff*250.0 + 1.0, 0.0, 1.0);\n\
	}\n\
	vec3 diffuseReflection(vec3 normalVec, vec3 lightVec, vec3 viewVec, vec3 diffuseColor)\n\
	{\n\
		float dotValue = dot(normalVec, lightVec);\n\
		vec3 dotVec = vec3(dotValue,dotValue,dotValue);\n\
		return pow(clamp((1.0-dotVec)*sssColor + dotVec,0.0, 1.0), sssColor*4.0 + 1.0) * diffuseColor;\n\
	}\n\
	vec3 specularReflection(vec3 normalVec, vec3 lightVec, vec3 viewVec, vec3 halfVec, vec3 specularColor)\n\
	{\n\
	float asperity = 1.125 * max(0.0,dot(normalVec,halfVec));\n\
	float asperityStrength = pow(min(1.0,1.0 - dot(normalVec,viewVec)),aspExp);\n\
	float np = pow(2.0,shineness);\n\
	float npNorm = np*0.125 + 1.0;\n\
	float fresnel = pow(min(1.0, 1.0- dot(normalVec,viewVec)), 5.0)*0.92 + 0.08;\n\
	float blinnPhong = npNorm*pow(max(0.0,dot(normalVec,halfVec)), np);\n\
	float blinnPhongStrength = min(fresnel, 1.0-asperityStrength);\n\
	return specularColor*(blinnPhong * blinnPhongStrength + asperity*asperityStrength)*max(0.0,dot(normalVec,lightVec));\n\
	}\n\
	void main (void)\n\
	{  \n\
	vec3 n,halfV;\n\
	float NdotL,NdotHV;\n\
	float att;\n\
	vec4 color = vec4(ambient,1.0);	\n\
	vec3 newtv = normalize(tv - bv*dot(tv,bv));\n\
	vec3 newbv = normalize(bv);\n\
	vec3 newn  = normalize(normal);//normalize(cross(tv,bv));\n\
	vec4 texColor = texture2D(diffuseTexture,gl_TexCoord[0].st);\n\
	vec3 normalColor = normalize(texture2D(normalTexture,gl_TexCoord[0].st).xyz* 2.0 - 1.0);\n\
	vec3 normalMapN = normalize(-newtv*normalColor.x-newbv*normalColor.y+newn*normalColor.z); \n\
	vec3 specularColor = texture2D(specularTexture, gl_TexCoord[0].st).xyz;\n\
	vec3 specMat = specularMaterial.rgb;\n\
	if (useTexture == 0) \n\
	texColor = diffuseMaterial;//vec4(matColor,1.0);\n\
	color.a = texColor.a*diffuseMaterial.a;\n\
	n = normalize(normal);\n\
	if (useNormalMap == 1 && dot(normalMapN,n) > 0.0)\n\
	{\n\
	n = normalMapN;\n\
	}\n\
	if (useSpecularMap == 1)\n\
	specMat = specularColor;\n\
	float shadowWeight = 1.0;\n\
	if (useShadowMap == 1)\n\
	{\n\
	shadowWeight = shadowCoef();\n\
	}\n\
	for (int i=0;i<numLights;i++)\n\
	{\n\
	att = 1.0;//1.0/(gl_LightSource[i].constantAttenuation + gl_LightSource[i].linearAttenuation * dist[i] + gl_LightSource[i].quadraticAttenuation * dist[i] * dist[i]);	\n\
	halfV = normalize(halfVector[i]);\n\
	vec3 viewDir = halfV*dot(halfV,lightDir[i])*2.0 - lightDir[i];\n\
	color += vec4(diffuseReflection(n,lightDir[i],viewDir,texColor.rgb), 0.0);\n\
	color += vec4(specularReflection(n,lightDir[i],viewDir, halfV,specMat.rgb), 0.0);\n\
	}\n\
	const float shadow_ambient = 1.0;\n\
	gl_FragColor = vec4(color.rgb*shadowWeight*shadow_ambient,color.a);//color*shadow_ambient*shadowWeight;//vec4(color.rgb*shadowWeight,color.a);//color*shadowWeight;\n\
	//gl_FragColor = vec4(1.0,0.0,0.0,1.0);\n\
	}";

	
	std::string shaderFSMac =
	"#version 120\n\
	const vec3 ambient = vec3(0.0,0.0,0.0);//(vec3(255 + 127, 241, 0 + 2)/255.0)*(vec3(0.2,0.2,0.2));\n\
	uniform sampler2D diffuseTexture;\n\
	uniform sampler2D normalTexture;\n\
	uniform sampler2D specularTexture;\n\
	uniform sampler2D tex;\n\
	uniform int  useTexture;\n\
	uniform int  useNormalMap;\n\
	uniform int  useSpecularMap;\n\
	uniform int  useShadowMap;\n\
	varying vec3 normal,lightDir[2],halfVector[2];\n\
	varying vec3 tv,bv;\n\
	varying vec4 vPos;\n\
	uniform vec4 diffuseMaterial;\n\
	uniform vec4 specularMaterial;\n\
	uniform float  shineness;\n\
	float shadowCoef()\n\
	{\n\
	int index = 0;\n\
	vec4 shadow_coord = vPos/vPos.w;\n\
	shadow_coord.z += 0.000005;\n\
	float shadow_d = texture2D(tex, shadow_coord.st).x;\n\
	float diff = 1.0;\n\
	diff = (shadow_d - shadow_coord.z);\n\
	return clamp( diff*850.0 + 1.0, 0.0, 1.0);//(shadow_d-0.9)*10;//clamp( diff*250.0 + 1.0, 0.0, 1.0);\n\
	}\n\
	void main (void)\n\
	{  \n\
	vec3 n,halfV;\n\
	float NdotL,NdotHV;\n\
	float att;\n\
	vec4 color = vec4(ambient,1.0);	\n\
	vec3 newtv = normalize(tv - bv*dot(tv,bv));\n\
	vec3 newbv = normalize(bv);\n\
	vec3 newn  = normalize(normal);//normalize(cross(tv,bv));\n\
	vec4 texColor = texture2D(diffuseTexture,gl_TexCoord[0].st);\n\
	vec3 specMat = specularMaterial.rgb;\n\
	if (useTexture == 0) \n\
	texColor = diffuseMaterial;//vec4(matColor,1.0);\n\
	color.a = texColor.a*diffuseMaterial.a;\n\
	//color.rgb = texColor.rgb;\n\
	float shadowWeight = 1.0;\n\
	n = newn; \n\
	if (useShadowMap == 1)\n\
	{\n\
	shadowWeight = shadowCoef();\n\
	}\n\
	for (int i=0;i<2;i++)\n\
	{\n\
	vec3 posDir = lightDir[i];//normalize(vec3(gl_LightSource[i].position));\n\
	NdotL = max(dot(n,posDir),0.0);\n\
	if (NdotL > 0.0) {\n\
	color += vec4(texColor.xyz*gl_LightSource[i].diffuse.xyz*NdotL,0);\n\
	halfV = normalize(halfVector[i]);\n\
	NdotHV = max(dot(n,halfV),0.0);\n\
	color += vec4(specMat.rgb*pow(NdotHV, shineness+1.0),0);\n\
	}   \n\
	}\n\
	const float shadow_ambient = 1.0;\n\
	gl_FragColor = vec4(color.rgb*shadowWeight*shadow_ambient,color.a);//color*shadow_ambient*shadowWeight;//vec4(color.rgb*shadowWeight,color.a);//color*shadowWeight;\n\
	//gl_FragColor = vec4(color.rgb, color.a);\n\
	//gl_FragColor = vec4(1.0,0.0,0.0,1.0);\n\
	}";

/*
std::string shaderFSSimple =
	"const vec3 ambient = vec3(0.0,0.0,0.0);//(vec3(255 + 127, 241, 0 + 2)/255.f)*(vec3(0.2,0.2,0.2));\n\
	uniform sampler2D diffuseTexture;\n\
	uniform sampler2D tex;\n\
	uniform int  useTexture;\n\
	uniform int  useShadowMap;\n\
	varying vec3 normal,lightDir[4],halfVector[4];\n\
	varying vec3 tv,bv;\n\
	varying vec4 vPos;\n\
	uniform vec4 diffuseMaterial;\n\
	uniform vec4 specularMaterial;\n\
	uniform float  shineness;\n\
	//uniform vec3 specularColors;\n\
	varying float dist[2];\n\
	float shadowCoef()\n\
	{\n\
	int index = 0;\n\
	vec4 shadow_coord = vPos/vPos.w;\n\
	shadow_coord.z += 0.000005;\n\
	float shadow_d = texture2D(tex, shadow_coord.st).x;\n\
	float diff = 1.0;\n\
	diff = (shadow_d - shadow_coord.z);\n\
	return clamp( diff*850.0 + 1.0, 0.0, 1.0);//(shadow_d-0.9)*10;//clamp( diff*250.0 + 1.0, 0.0, 1.0);\n\
	}\n\
	void main (void)\n\
	{  \n\
	vec3 n,halfV;\n\
	float NdotL,NdotHV;\n\
	float att;\n\
	vec4 color = vec4(ambient,1.0);	\n\
	vec3 newtv = normalize(tv - bv*dot(tv,bv));\n\
	vec3 newbv = normalize(bv);\n\
	vec3 newn  = normalize(normal);//normalize(cross(tv,bv));\n\
	vec4 texColor = texture2D(diffuseTexture,gl_TexCoord[0].st);\n\
	vec3 normalColor = normalize(texture2D(normalTexture,gl_TexCoord[0].st).xyz* 2.0 - 1.0);\n\
	vec3 normalMapN = normalize(-newtv*normalColor.x-newbv*normalColor.y+newn*normalColor.z); \n\
	vec3 specularColor = texture2D(specularTexture, gl_TexCoord[0].st).xyz;\n\
	vec3 specMat = specularMaterial.rgb;\n\
	if (useTexture == 0) \n\
	texColor = diffuseMaterial;//vec4(matColor,1.0);\n\
	color.a = texColor.a*diffuseMaterial.a;\n\
	n = normalize(normal);\n\
	if (useNormalMap == 1 && dot(normalMapN,n) > 0.0)\n\
	{\n\
	n = normalMapN;\n\
	}\n\
	if (useSpecularMap == 1)\n\
	specMat = specularColor;\n\
	float shadowWeight = 1.0;\n\
	if (useShadowMap == 1)\n\
	{\n\
	shadowWeight = shadowCoef();\n\
	}\n\
	for (int i=0;i<4;i++)\n\
	{\n\
	att = 1.0/(gl_LightSource[i].constantAttenuation + gl_LightSource[i].linearAttenuation * dist[i] + gl_LightSource[i].quadraticAttenuation * dist[i] * dist[i]);	\n\
	NdotL = max(dot(n,lightDir[i]),0.0);\n\
	if (NdotL > 0.0) {\n\
	//color += vec4(texColor.xyz*gl_LightSource[i].diffuse.xyz*NdotL,0)*att;\n\
	color += vec4(texColor.xyz*gl_LightSource[i].diffuse.xyz*NdotL,0);\n\
	halfV = normalize(halfVector[i]);\n\
	NdotHV = max(dot(n,halfV),0.0);\n\
	color += vec4(specMat.rgb*pow(NdotHV, shineness+1.0),0);\n\
	//color += vec4(specMat.rgb*pow(NdotHV, shineness+1.0),0)*att;\n\
	}   \n\
	}\n\
	const float shadow_ambient = 1.0;\n\
	gl_FragColor = vec4(color.rgb*shadowWeight*shadow_ambient,color.a);//color*shadow_ambient*shadowWeight;//vec4(color.rgb*shadowWeight,color.a);//color*shadowWeight;\n\
	//gl_FragColor = vec4(1.0,0.0,0.0,1.0);\n\
	}";
*/

typedef std::pair<int,int> IntPair;

SbmDeformableMeshGPU::SbmDeformableMeshGPU(void) : DeformableMesh()
{
	useGPU = false;	
	VBOPos = nullptr;
	VBONormal = nullptr;
	VBOTangent = nullptr;
	VBOBiNormal = nullptr;
	VBOTexCoord = nullptr;
	VBOTri = nullptr;
	VBOBoneID1 = nullptr;
	VBOBoneID2 = nullptr;
	VBOWeight1 = nullptr;
	VBOWeight2 = nullptr;
	TBOTran = nullptr;
	initGPUVertexBuffer = false;
}

SbmDeformableMeshGPU::~SbmDeformableMeshGPU(void)
{
  SmartBody::util::log("Delete Deformable Mesh GPU '%s'", this->getName().c_str());
	initShader = false;
	if (VBOPos) delete VBOPos;
	if (VBOTangent) delete VBOTangent;
	if (VBOBiNormal) delete VBOBiNormal;
    if (VBONormal) delete VBONormal;
	if (VBOTexCoord) delete VBOTexCoord;
	if (VBOTri) delete VBOTri;
	if (VBOBoneID1) delete VBOBoneID1;	
	if (VBOWeight1) delete VBOWeight1;

#if USE_SKIN_WEIGHT_SIZE_8
	if (VBOWeight2) delete VBOWeight2;
	if (VBOBoneID2) delete VBOBoneID2;
#endif
	if (TBOTran) delete TBOTran;
	for (unsigned int i=0;i<subMeshTris.size();i++)
	{
		delete subMeshTris[i];
	}
	subMeshTris.clear();

	for (unsigned int i=0;i<meshSubset.size();i++)
		delete meshSubset[i];
	meshSubset.clear();
}

void SbmDeformableMeshGPU::skinTransformGPU(DeformableMeshInstance* meshInstance)
{
#if !defined(__ANDROID__) && !defined(SB_IPHONE)
	SbmDeformableMeshGPUInstance* gpuMeshInstance = dynamic_cast<SbmDeformableMeshGPUInstance*>(meshInstance);
	
	TBOData* tranTBO = gpuMeshInstance->getTBOTransforBuffer();
	std::vector<SrMat>& tranBuffer = gpuMeshInstance->getTransformBuffer();

	std::string activeShader = shaderName;//SbmDeformableMeshGPU::useShadowPass ? shadowShaderName : shaderName;
	if (SmartBody::SBScene::getScene()->getBoolAttribute("enableFaceShader"))
		activeShader = shaderFaceName;

	int numLightInScene = SmartBody::SBScene::getScene()->getIntAttribute("numLightsInScene");
	bool useDefaultLights = SmartBody::SBScene::getScene()->getBoolAttribute("useDefaultLights");
	//std::string activeShader = shaderFaceName;

	GLuint program = SbmShaderManager::singleton().getShader(activeShader)->getShaderProgram();		
	glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );	
	glDisable(GL_POLYGON_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable ( GL_ALPHA_TEST );
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto skel = meshInstance->getSkeleton();
	SmartBody::SBPawn* pawn = skel->getPawn();
	double alphaThreshold = pawn->getDoubleAttribute("alphaThreshold");

	glAlphaFunc ( GL_GREATER,(float)alphaThreshold) ;
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glUseProgram(program);		
	int iActiveTex = 0;	

	GLuint diffuse_sampler_location = glGetUniformLocation(program,"diffuseTexture");	
	GLuint normal_sampler_location = glGetUniformLocation(program,"normalTexture");	
	GLuint specular_sampler_location = glGetUniformLocation(program,"specularTexture");	
	GLuint glossy_sampler_location = glGetUniformLocation(program, "glossyTexture");
	GLuint shadow_sampler_location = glGetUniformLocation(program,"tex");	
	GLuint meshScale_location = glGetUniformLocation(program,"meshScale");	
	GLuint bone_loc1 = glGetAttribLocation(program,"BoneID1");
	GLuint weight_loc1 = glGetAttribLocation(program,"BoneWeight1");	
#if USE_SKIN_WEIGHT_SIZE_8
	GLuint bone_loc2 = glGetAttribLocation(program,"BoneID2");
	GLuint weight_loc2 = glGetAttribLocation(program,"BoneWeight2");
#endif
	GLuint tangent_loc = glGetAttribLocation(program,"tangent");
	GLuint binormal_loc = glGetAttribLocation(program,"binormal");
	GLuint diffuseLoc = glGetUniformLocation(program,"diffuseMaterial");	
	GLuint specularLoc = glGetUniformLocation(program,"specularMaterial");	
	GLuint shinenessLoc = glGetUniformLocation(program,"shineness");
	GLuint useTextureLoc = glGetUniformLocation(program,"useTexture");
	GLuint updateNormalLoc = glGetUniformLocation(program,"updateNormal");
	GLuint useNormalMapLoc = glGetUniformLocation(program,"useNormalMap");
	GLuint useSpecularMapLoc = glGetUniformLocation(program,"useSpecularMap");
	GLuint useShadowMapLoc = glGetUniformLocation(program,"useShadowMap");
	GLuint numLightsLoc = glGetUniformLocation(program, "numLights");

	// update normal vectors for the deformable mesh. it is significantly slower to do this. So turn off by default. 
	glUniform1i(updateNormalLoc,1);
	SrVec scale = meshInstance->getMeshScale();
	glUniform1f(meshScale_location, (float) scale[0]);

	GLuint idQuery;
	GLuint count = 0;
	glGenQueries(1, &idQuery);

#if 0
	if (SbmShaderManager::getShaderSupport() == SbmShaderManager::SUPPORT_OPENGL_3_0)
	{
		GLuint my_sampler_uniform_location = glGetUniformLocation(program,"Transform");	
		tranTBO->UpdateTBOData((float*)getPtr(tranBuffer));
		glActiveTexture(GL_TEXTURE0_ARB);		
		tranTBO->BindBufferToTexture();
		glUniform1i(my_sampler_uniform_location, iActiveTex);
	}
	else
#endif
	{
		GLuint transformLocation = glGetUniformLocation(program,"Transform");
		glUniformMatrix4fv(transformLocation,tranBuffer.size(),true,(GLfloat*)getPtr(tranBuffer));
	}


	if (!useDefaultLights && numLightInScene == 0)
	{
		glUniform1i(numLightsLoc, 0);
	}
	else if (useDefaultLights && numLightInScene == 0)
	{
		glUniform1i(numLightsLoc, 3);
	}
	else
	{ 
		glUniform1i(numLightsLoc, numLightInScene);
	}
	
	
	glEnableClientState(GL_VERTEX_ARRAY);	
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	VBOPos->VBO()->BindBuffer();
	//SbmShaderProgram::printOglError("after bind buffer\n");
	glVertexPointer( 3, GL_FLOAT, 0, 0);
	//SbmShaderProgram::printOglError("after vertex pointer\n");
	
	glEnableVertexAttribArray(weight_loc1);
	VBOWeight1->VBO()->BindBuffer();
	glVertexAttribPointer(weight_loc1,4,GL_FLOAT,0,0,0);	
	//glBindAttribLocation(program,VBOWeight1->VBO()->m_ArrayType,"BoneWeight1");
	//glVertexAttribPointer(VBOWeight1->VBO()->m_ArrayType,4,GL_FLOAT,0,0,0);	
	
	glEnableVertexAttribArray(bone_loc1);
	VBOBoneID1->VBO()->BindBuffer();
	//glVertexAttribIPointer(bone_loc1,4,GL_INT,0,0);	
	glVertexAttribPointer(bone_loc1,4,GL_FLOAT,0,0,0);	
	//glBindAttribLocation(program,VBOBoneID1->VBO()->m_ArrayType,"BoneID1");
	//glVertexAttribPointer(VBOBoneID1->VBO()->m_ArrayType,4,GL_FLOAT,0,0,0);	
#if USE_SKIN_WEIGHT_SIZE_8
	glEnableVertexAttribArray(weight_loc2);
	VBOWeight2->VBO()->BindBuffer();
	glVertexAttribPointer(weight_loc2,4,GL_FLOAT,0,0,0);	
	//glBindAttribLocation(program,VBOWeight2->VBO()->m_ArrayType,"BoneWeight2");
	//glVertexAttribPointer(VBOWeight2->VBO()->m_ArrayType,4,GL_FLOAT,0,0,0);
	// 
	
	glEnableVertexAttribArray(bone_loc2);
	VBOBoneID2->VBO()->BindBuffer();
	//glVertexAttribIPointer(bone_loc2,4,GL_INT,0,0);	
	glVertexAttribPointer(bone_loc2,4,GL_FLOAT,0,0,0);	
#endif	
	
	glEnableVertexAttribArray(tangent_loc);
	VBOTangent->VBO()->BindBuffer();
	glVertexAttribPointer(tangent_loc,3,GL_FLOAT,0,0,0);	

	glEnableVertexAttribArray(binormal_loc);
	VBOBiNormal->VBO()->BindBuffer();
	glVertexAttribPointer(binormal_loc,3,GL_FLOAT,0,0,0);	

	
	SbmTextureManager& texManager = SbmTextureManager::singleton();
	VBONormal->VBO()->BindBuffer();
	glNormalPointer(GL_FLOAT,0,0);
	VBOTexCoord->VBO()->BindBuffer();
	glTexCoordPointer(2,GL_FLOAT,0,0);
	
	
	for (unsigned int i=0;i<subMeshList.size();i++)
	{	
		SbmSubMesh* mesh = subMeshList[i];
		float color[4];
		mesh->material.diffuse.get(color);		
		glUniform4f(diffuseLoc,color[0],color[1],color[2],color[3]);
		mesh->material.specular.get(color);		
		glUniform4f(specularLoc,color[0],color[1],color[2],color[3]);	
		//printf("shineness = %d\n",mesh->material.shininess);
		glUniform1f(shinenessLoc,mesh->material.shininess);
		//SmartBody::util::log("mat color = %f %f %f\n",color[0],color[1],color[2]);
		auto tex = texManager.findTexture(mesh->texName.c_str());

		if (mesh->material.useAlphaBlend)
		{
			myGLDisable(GL_CULL_FACE);
			myGLEnable(GL_ALPHA_TEST);
			myGLEnable(GL_BLEND);

		}
		else
		{
			myGLEnable(GL_CULL_FACE);
			myGLDisable(GL_ALPHA_TEST);
			myGLDisable(GL_BLEND);
		}


		if (tex)
		{
			glActiveTexture(GL_TEXTURE1_ARB);
			glBindTexture(GL_TEXTURE_2D,tex->getID());
			glUniform1i(diffuse_sampler_location, 1);
			glUniform1i(useTextureLoc,1);
		}		
		else
		{
			glUniform1i(useTextureLoc,0);
		}

		auto texNormal = texManager.findTexture(mesh->normalMapName.c_str());
		if (texNormal)
		{
			//SmartBody::util::log("use texture normal, id = %d", texNormal->getID());
			glActiveTexture(GL_TEXTURE2_ARB);
			glBindTexture(GL_TEXTURE_2D,texNormal->getID());
			glUniform1i(normal_sampler_location, 2);
			glUniform1i(useNormalMapLoc,1);
		}		
		else
		{
			glUniform1i(useNormalMapLoc,0);
		}

		auto texSpecular = texManager.findTexture(mesh->specularMapName.c_str());
		if (texSpecular)
		{
			//SmartBody::util::log("use texture specualr, id = %d",texSpecular->getID());
			glActiveTexture(GL_TEXTURE3_ARB);
			glBindTexture(GL_TEXTURE_2D,texSpecular->getID());
			glUniform1i(specular_sampler_location, 3);
			glUniform1i(useSpecularMapLoc,1);
		}		
		else
		{
			glUniform1i(useSpecularMapLoc,0);
		}
		
		if (texSpecular)
		{
			//SmartBody::util::log("use texture specualr, id = %d",texSpecular->getID());
			glActiveTexture(GL_TEXTURE3_ARB);
			glBindTexture(GL_TEXTURE_2D, texSpecular->getID());
			glUniform1i(specular_sampler_location, 3);
			glUniform1i(useSpecularMapLoc, 1);
		}
		else
		{
			glUniform1i(useSpecularMapLoc, 0);
		}

		if (SbmDeformableMeshGPU::shadowMapID != -1)
		{
			glActiveTexture(GL_TEXTURE7_ARB);
			glBindTexture(GL_TEXTURE_2D,shadowMapID);
#if !defined(ANDROID_BUILD)
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
#endif
			//glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);  
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
			glUniform1i(shadow_sampler_location, 7);
			glUniform1i(useShadowMapLoc,1);			
			glCullFace(GL_BACK);
		}
		else
		{
			glUniform1i(useShadowMapLoc,0);
		}

		subMeshTris[i]->VBO()->BindBuffer();
		//if (mesh->isHair)
		if (mesh->material.useAlphaBlend)
		{
			glEnable(GL_BLEND);
			glEnable ( GL_ALPHA_TEST );
// 			glAlphaFunc ( GL_GEQUAL, 1.f ) ; // discard all fragments with alpha value smaller than 1
// 			glDisable(GL_CULL_FACE);
			glDrawElements(GL_TRIANGLES,3*mesh->numTri,GL_UNSIGNED_INT,0);
// 
// 			glAlphaFunc ( GL_LESS, 1.f ) ; // discard all fragments with alpha value smaller than 1
// 			glEnable(GL_CULL_FACE);
// 			glCullFace(GL_FRONT);
// 			glDepthMask(GL_FALSE);
// 			glDrawElements(GL_TRIANGLES,3*mesh->numTri,GL_UNSIGNED_INT,0);
// 
// 			glCullFace(GL_BACK);
// 			//glDepthMask(GL_TRUE);
// 			glDrawElements(GL_TRIANGLES,3*mesh->numTri,GL_UNSIGNED_INT,0);
// 
// 			glDepthMask(GL_TRUE);
// 			glAlphaFunc( GL_GREATER, 0.05f );			
// 			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_BLEND);
			glDisable ( GL_ALPHA_TEST );
			glDrawElements(GL_TRIANGLES,3*mesh->numTri,GL_UNSIGNED_INT,0);
		}		
		subMeshTris[i]->VBO()->UnbindBuffer();

		glActiveTexture(GL_TEXTURE1_ARB);
		glBindTexture(GL_TEXTURE_2D,0);
		glActiveTexture(GL_TEXTURE2_ARB);
		glBindTexture(GL_TEXTURE_2D,0);
		glActiveTexture(GL_TEXTURE3_ARB);
		glBindTexture(GL_TEXTURE_2D,0);
	}		

	// 	VBOTri->VBO()->BindBuffer();
	// 	glDrawElements(GL_TRIANGLES,3*numTotalTris,GL_UNSIGNED_INT,0);
	// 	VBOTri->VBO()->UnbindBuffer();


	VBOTexCoord->VBO()->UnbindBuffer();
	VBONormal->VBO()->UnbindBuffer();
	
	VBOBiNormal->VBO()->UnbindBuffer();
	VBOTangent->VBO()->UnbindBuffer();
	
#if USE_SKIN_WEIGHT_SIZE_8
	VBOBoneID2->VBO()->UnbindBuffer();	
	VBOWeight2->VBO()->UnbindBuffer();	
#endif

	VBOBoneID1->VBO()->UnbindBuffer();	
	VBOWeight1->VBO()->UnbindBuffer();	
	VBOPos->VBO()->UnbindBuffer();

#if USE_SKIN_WEIGHT_SIZE_8
	glDisableVertexAttribArray(bone_loc2);
	glDisableVertexAttribArray(weight_loc2);
#endif

	glDisableVertexAttribArray(bone_loc1);
	glDisableVertexAttribArray(weight_loc1);
	glDisableVertexAttribArray(binormal_loc);
	glDisableVertexAttribArray(tangent_loc);
	glDisableClientState(GL_VERTEX_ARRAY);	
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glUseProgram(0);		
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST) ;
	glDisable(GL_BLEND);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glActiveTexture(GL_TEXTURE0);
#endif
}


void SbmDeformableMeshGPU::initShaderProgram()
{
	std::string vsPathName = ShaderDir;
	vsPathName += VSName;

	std::string fsPathName = ShaderDir;
	fsPathName += FSName;

	initShader = true;
	//shaderProgram.initShaderProgram(vsPathName.c_str(),nullptr);
	//SbmShaderManager::singleton().addShader(shaderName.c_str(),vsPathName.c_str(),fsPathName.c_str());
	if (SbmShaderManager::getShaderSupport() == SbmShaderManager::SUPPORT_OPENGL_3_0)
	{
		SbmShaderManager::singleton().addShader(shadowShaderName.c_str(),shaderVS.c_str(),shaderBasicFS.c_str(),false);
		SbmShaderManager::singleton().addShader(shaderName.c_str(),shaderVS.c_str(),shaderFS.c_str(),false);
		SbmShaderManager::singleton().addShader(shaderFaceName.c_str(),shaderVS.c_str(),shaderFSFace.c_str(),false);
	}
	else if (SbmShaderManager::getShaderSupport() == SbmShaderManager::SUPPORT_OPENGL_2_0)
	{
#if USE_SKIN_WEIGHT_SIZE_8
		std::string VSShaderName = shaderVS_2;
#else
		std::string VSShaderName = shaderVS_Weight4;
#endif
		SbmShaderManager::singleton().addShader(shadowShaderName.c_str(),VSShaderName.c_str(),shaderBasicFS.c_str(),false);
#ifdef __APPLE__
		SbmShaderManager::singleton().addShader(shaderName.c_str(),VSShaderName.c_str(),shaderFSMac.c_str(),false);
		SbmShaderManager::singleton().addShader(shaderFaceName.c_str(),VSShaderName.c_str(),shaderFSMac.c_str(),false);
#else
		SbmShaderManager::singleton().addShader(shaderName.c_str(),VSShaderName.c_str(),shaderFS.c_str(),false);
		SbmShaderManager::singleton().addShader(shaderFaceName.c_str(),VSShaderName.c_str(),shaderFSFace.c_str(),false);
#endif
		
	}
	else
	{
		initShader = false;	
	}
}



bool SbmDeformableMeshGPU::initBuffer1()
{
#if 0
	// feng : the CPU version of deformable mesh consists of some mesh segments, with their corresponding bone weights loosely stored.
	// this is very bad for GPU processing. Thus I reorganize the data into a single array, to avoid redundancy in memory storage.
	if (skinWeights.size() == 0 )
		return false;

	int nTotalVtxs=0, nTotalTris = 0, nTotalBones = 0;
	std::map<std::string,int> boneIdxMap;
	std::vector<std::set<IntPair> > vtxNormalIdxMap;
	std::vector<std::set<int> > vtxMaterialIdxMap;
	std::map<IntPair,int> ntNewVtxIdxMap;	
	std::map<int,std::vector<int> > vtxNewVtxIdxMap;
	std::map<int,std::vector<int> > meshSubsetMap;

	GLuint program = SbmShaderManager::singleton().getShader(shaderName)->getShaderProgram();	
	
	SrColor colorArray[6] = { SrColor::blue, SrColor::red, SrColor::green, SrColor::magenta, SrColor::gray, SrColor::yellow};
	// setup deformable mesh color	
	int nMaterial = 1;	
	std::vector<SrMaterial> allMatList;
	//std::vector<SrColor> allMatList;
	std::vector<std::string> allTexNameList;
	std::vector<std::string> allNormalTexNameList;
	SrMaterial defaultMaterial;
	defaultMaterial.diffuse = SrColor(0.6f,0.6f,0.6f);//SrColor::gray;	
	defaultMaterial.specular = SrColor(101,101,101);//SrColor::gray;
	defaultMaterial.shininess = 29;

	allMatList.emplace_back(defaultMaterial);
	allTexNameList.emplace_back("");
	allNormalTexNameList.emplace_back("");
	meshSubsetMap[0] = std::vector<int>(); // default material group : gray color
	for (unsigned int skinCounter = 0; skinCounter < skinWeights.size(); skinCounter++)
	{
		SkinWeight* skinWeight = skinWeights[skinCounter];		
		int pos;
		int globalCounter = 0;
		pos = this->getMesh(skinWeight->sourceMesh);
		if (pos != -1)
		{
			SrSnModel* dMeshDynamic = dMeshDynamic_p[pos];
			SrSnModel* dMeshStatic = dMeshStatic_p[pos];
			dMeshStatic->shape().computeTangentBiNormal();
			SrArray<SrMaterial>& matList = dMeshDynamic->shape().M; 	
			std::map<std::string,std::string> mtlTexMap = dMeshDynamic->shape().mtlTextureNameMap;
			std::map<std::string,std::string> mtlNormalTexMap = dMeshDynamic->shape().mtlNormalTexNameMap;
			printf("meshName = %s, material list = %d\n", &dMeshDynamic->shape().name.get(0), matList.size());
			for (int j=0;j<matList.size();j++)
			{			
				SrMaterial& mat = matList[j];	
				std::string mtlName = dMeshDynamic->shape().mtlnames[j];
				if (mtlTexMap.find(mtlName) != mtlTexMap.end())
				{
					allTexNameList.emplace_back(mtlTexMap[mtlName]);
				}
				else
				{
					allTexNameList.emplace_back("");
				}	

				if (mtlNormalTexMap.find(mtlName) != mtlNormalTexMap.end())
				{
					allNormalTexNameList.emplace_back(mtlNormalTexMap[mtlName]);
				}
				else
				{
					allNormalTexNameList.emplace_back("");
				}
				allMatList.emplace_back(mat);
				//colorArray[j%6].get(fcolor);				
				meshSubsetMap[nMaterial] = std::vector<int>(); 
				nMaterial++;
			}
		}				
	}	

	//printf("num of mesh subset =  %d\n",meshSubsetMap.size());

	int iMaterialOffset = 1;
	boneJointList.clear();
	int iFaceIdxOffset = 0, iNormalIdxOffset = 0, iTextureIdxOffset = 0;
	int iFace = 0;
	SrModel::Face defaultIdx;
	defaultIdx.a = defaultIdx.b = defaultIdx.c = -1;
	for (unsigned int skinCounter = 0; skinCounter < skinWeights.size(); skinCounter++)
	{
		SkinWeight* skinWeight = skinWeights[skinCounter];		
		int pos;
		int globalCounter = 0;
		pos = this->getMesh(skinWeight->sourceMesh);
		if (pos != -1)
		{
			SrSnModel* dMeshStatic = dMeshStatic_p[pos];
			SrSnModel* dMeshDynamic = dMeshDynamic_p[pos];			
			int nVtx = dMeshStatic->shape().V.size();	
			int nFace = dMeshStatic->shape().F.size();
			int nNormal = dMeshStatic->shape().N.size();	
			int nTexture = dMeshStatic->shape().T.size();
			for (int i=0;i<nVtx;i++)
			{
				vtxNormalIdxMap.emplace_back(std::set<IntPair>());
			}

			nTotalVtxs += nVtx;				
			nTotalTris += nFace;
			for (unsigned int k=0;k<skinWeight->infJointName.size();k++)
			{
				std::string& jointName = skinWeight->infJointName[k];
				SkJoint* curJoint = skinWeight->infJoint[k];
				if (boneIdxMap.find(jointName) == boneIdxMap.end()) // new joint
				{
					boneIdxMap[jointName] = nTotalBones++;
					boneJointList.emplace_back(curJoint);
					bindPoseMatList.emplace_back(skinWeight->bindShapeMat*skinWeight->bindPoseMat[k]);
				}
			}
			int numTris = dMeshStatic->shape().F.size();
			for (int i=0; i < numTris ; i++)
			{
				SrModel& model = dMeshStatic->shape();
				if (dMeshStatic->shape().F.size() == 0)
					continue;
				SrModel::Face& faceIdx = dMeshStatic->shape().F[i];			
				if (dMeshStatic->shape().Fn.size() == 0)
					continue;
				SrModel::Face& nIdx = dMeshStatic->shape().Fn[i];
				SrModel::Face& tIdx = defaultIdx;
				if (model.Ft.size() > i)
					tIdx = model.Ft[i];
								
				vtxNormalIdxMap[faceIdx.a + iFaceIdxOffset].insert(IntPair(nIdx.a+iNormalIdxOffset,tIdx.a+iTextureIdxOffset));
				vtxNormalIdxMap[faceIdx.b + iFaceIdxOffset].insert(IntPair(nIdx.b+iNormalIdxOffset,tIdx.b+iTextureIdxOffset));
				vtxNormalIdxMap[faceIdx.c + iFaceIdxOffset].insert(IntPair(nIdx.c+iNormalIdxOffset,tIdx.c+iTextureIdxOffset));
				
				int nMatIdx = 0; // if no corresponding materials, push into the default gray material group
				if (i < dMeshStatic->shape().Fm.size())
					nMatIdx = dMeshStatic->shape().Fm[i] + iMaterialOffset;		
				meshSubsetMap[nMatIdx].emplace_back(iFace);
				iFace++;
			}
			iFaceIdxOffset += nVtx;
			iNormalIdxOffset += nNormal;
			iMaterialOffset += dMeshDynamic->shape().M.size();
			iTextureIdxOffset += nTexture;
			//printf("iMaterial Offset = %d\n",iMaterialOffset);
		}			
	}

	if (nTotalVtxs == 0 || nTotalTris ==0)
		return false;

	//printf("orig vtxs = %d\n",nTotalVtxs);

	for (unsigned int i=0;i<vtxNormalIdxMap.size();i++)
	{
		std::set<IntPair>& idxMap = vtxNormalIdxMap[i];
		if (idxMap.size() > 1)
		{
			vtxNewVtxIdxMap[i] = std::vector<int>();
			std::set<IntPair>::iterator si = idxMap.begin();
			si++;
			for ( ;
				  si != idxMap.end();
				  si++)
			{
				vtxNewVtxIdxMap[i].emplace_back(nTotalVtxs);
				ntNewVtxIdxMap[*si] = nTotalVtxs;
				nTotalVtxs++;
			}
		}
	}

	//printf("new vtxs = %d\n",nTotalVtxs);

	// temporary storage 
	ublas::vector<Vec4f> posBuffer(nTotalVtxs);
	ublas::vector<Vec3f> normalBuffer(nTotalVtxs),texCoordBuffer(nTotalVtxs), tangentBuffer(nTotalVtxs), binormalBuffer(nTotalVtxs);
	ublas::vector<Vec3i> triBuffer(nTotalTris);
	ublas::vector<Vec4f> boneID1(nTotalVtxs),boneID2(nTotalVtxs),weight1(nTotalVtxs),weight2(nTotalVtxs);
	transformBuffer.resize(nTotalBones);	
	
	int iVtx = 0;
	iFace = 0;
	iFaceIdxOffset = 0;
	iNormalIdxOffset = 0;
	iTextureIdxOffset = 0;
	for (unsigned int skinCounter = 0; skinCounter < skinWeights.size(); skinCounter++)
	{
		SkinWeight* skinWeight = skinWeights[skinCounter];		
		int pos;
		int globalCounter = 0;
		pos = this->getMesh(skinWeight->sourceMesh);
		if (pos != -1)
		{
			SrSnModel* dMeshStatic = dMeshStatic_p[pos];
			SrSnModel* dMeshDynamic = dMeshDynamic_p[pos];
			dMeshDynamic->visible(false);
			int numVertices = dMeshStatic->shape().V.size();
			int numNormals = dMeshStatic->shape().N.size();
			int numTexCoords = dMeshStatic->shape().T.size();
			for (int i = 0; i < numVertices; i++)
			{
				if (i >= (int) skinWeight->numInfJoints.size())
					continue;
				int numOfInfJoints = skinWeight->numInfJoints[i];				
				SrVec& lv = dMeshStatic->shape().V[i];					
				posBuffer(iVtx) = Vec4f(lv[0],lv[1],lv[2],1.f);

				SrVec& lt =	dMeshStatic->shape().Tangent[i];		
				SrVec  lb = dMeshStatic->shape().BiNormal[i];
				tangentBuffer(iVtx) = Vec3f(lt[0],lt[1],lt[2]);
				binormalBuffer(iVtx) = Vec3f(lb[0],lb[1],lb[2]);
				//normalBuffer(iVtx) = Vec3f(ln[0],ln[1],ln[2]);
				boneID1(iVtx) = Vec4f(0,0,0,0);
				boneID2(iVtx) = Vec4f(0,0,0,0);
				weight1(iVtx) = Vec4f(0,0,0,0);
				weight2(iVtx) = Vec4f(0,0,0,0);
				
				std::vector<IntFloatPair> weightList;
// 				if (numOfInfJoints > 8)
// 					printf("vtx %d : \n",iVtx);
				for (int j = 0; j < numOfInfJoints; j++)
				{
					const std::string& curJointName = skinWeight->infJointName[skinWeight->jointNameIndex[globalCounter]];					
					float jointWeight = skinWeight->bindWeight[skinWeight->weightIndex[globalCounter]];
					int    jointIndex  = boneIdxMap[curJointName];
// 					if (numOfInfJoints > 8)
// 						printf("w = %d : %f\n",jointIndex,jointWeight);
					weightList.emplace_back(IntFloatPair(jointIndex,jointWeight));
					globalCounter ++;									
				}
				std::sort(weightList.begin(),weightList.end(),intFloatComp);				
				int numWeight = numOfInfJoints > 8 ? 8 : numOfInfJoints;
				float weightSum = 0.f;
				for (int j=0;j<numWeight;j++)
				{
					IntFloatPair& w = weightList[j];
					weightSum += w.second;
					if ( j < 4)
					{
						boneID1(iVtx)[j] = (float)w.first;
						weight1(iVtx)[j] = w.second;
					}
					else if (j < 8)
					{
						boneID2(iVtx)[j-4] = (float)w.first;
						weight2(iVtx)[j-4] = w.second;
					}	
				}
				for (int j=0;j<4;j++)
				{
					weight1(iVtx)[j] /= weightSum;
					weight2(iVtx)[j] /= weightSum;
				}	
				
				if (vtxNewVtxIdxMap.find(iVtx) != vtxNewVtxIdxMap.end())
				{
					std::vector<int>& idxMap = vtxNewVtxIdxMap[iVtx];
					// copy related vtx components 
					for (unsigned int k=0;k<idxMap.size();k++)
					{
						posBuffer(idxMap[k]) = posBuffer(iVtx);
						tangentBuffer(idxMap[k]) = tangentBuffer(iVtx);
						binormalBuffer(idxMap[k]) = binormalBuffer(iVtx);
						boneID1(idxMap[k]) = boneID1(iVtx);
						boneID2(idxMap[k]) = boneID2(iVtx);
						weight1(idxMap[k]) = weight1(iVtx);
						weight2(idxMap[k]) = weight2(iVtx);
					}
				}
				iVtx++;
			}

			int numTris = dMeshStatic->shape().F.size();
			for (int i=0; i < numTris ; i++)
			{
				if (dMeshStatic->shape().F.size() <= i)
					continue;
				if (dMeshStatic->shape().Fn.size() <= i)
					continue;
				SrModel::Face& faceIdx = dMeshStatic->shape().F[i];
				SrModel::Face& normalIdx = dMeshStatic->shape().Fn[i];
				SrModel::Face& texCoordIdx = defaultIdx;
				if (dMeshStatic->shape().Ft.size() > i)
					texCoordIdx = dMeshStatic->shape().Ft[i];
				int fIdx[3] = { faceIdx.a, faceIdx.b, faceIdx.c};
				int nIdx[3] = { normalIdx.a, normalIdx.b, normalIdx.c};
				int tIdx[3] = { texCoordIdx.a, texCoordIdx.b, texCoordIdx.c};

				for (int k=0;k<3;k++)
				{
					SrVec nvec;
					SrPnt2 tvec = SrPnt2(0,0);
					nvec = dMeshStatic->shape().N[nIdx[k]];
					if (dMeshStatic->shape().T.size() > tIdx[k] && dMeshStatic->shape().T.size() > 0 && dMeshStatic->shape().Ft.size() > 0)
						tvec = dMeshStatic->shape().T[tIdx[k]];
					int newNIdx = nIdx[k] + iNormalIdxOffset;
					int newTIdx = tIdx[k] + iTextureIdxOffset;
					int vIdx = fIdx[k] + iFaceIdxOffset;
					if (ntNewVtxIdxMap.find(IntPair(newNIdx,newTIdx)) != ntNewVtxIdxMap.end())
						vIdx = ntNewVtxIdxMap[IntPair(newNIdx,newTIdx)];
					normalBuffer(vIdx) = Vec3f(nvec.x,nvec.y,nvec.z);
					texCoordBuffer(vIdx) = Vec3f(tvec.x,1.f-tvec.y,0.f);
					triBuffer(iFace)[k] = vIdx;
				}			
				iFace++;
			}
			iFaceIdxOffset += numVertices;
			iNormalIdxOffset += numNormals;
			iTextureIdxOffset += numTexCoords;
		}			
	}		

	numTotalVtxs = iVtx;
	numTotalTris = iFace;

	std::map<int,std::vector<int> >::iterator vi;
	for (vi  = meshSubsetMap.begin();
		 vi != meshSubsetMap.end();
		 vi++)
	{
		int iMaterial = vi->first;

		std::vector<int>& faceIdxList = vi->second;	
		if (faceIdxList.size() == 0)
			continue;
		ublas::vector<Vec3i> subsetBuffer(faceIdxList.size());
		for (unsigned int k=0;k<faceIdxList.size();k++)
		{
			subsetBuffer(k) = triBuffer(faceIdxList[k]);
		}
		MeshSubset* mesh = new MeshSubset();
		mesh->material = allMatList[iMaterial];
		mesh->texName  = allTexNameList[iMaterial];
		mesh->normalMapName = allNormalTexNameList[iMaterial];
		mesh->VBOTri = new VBOVec3i((char*)"TriIdx",GL_ELEMENT_ARRAY_BUFFER,subsetBuffer);
		mesh->numTri = faceIdxList.size();
		meshSubset.emplace_back(mesh);
	}
	printf("meshSubset size = %d\n",meshSubset.size());
	// initial GPU buffer memory
	// Vertex Buffer Object	
	VBOPos = new VBOVec4f((char*)"RestPos",VERTEX_POSITION,posBuffer);		
	VBOTangent = new VBOVec3f((char*)"Tangent",VERTEX_TANGENT, tangentBuffer);
	VBOBiNormal = new VBOVec3f((char*)"BiNormal",VERTEX_BINORMAL, binormalBuffer);
	VBONormal  =  new VBOVec3f((char*)"Normal",VERTEX_VBONORMAL, normalBuffer);
	VBOTexCoord = new VBOVec3f((char*)"TexCoord",VERTEX_TEXCOORD, texCoordBuffer);
	VBOWeight1 = new VBOVec4f((char*)"Weight1",VERTEX_BONE_WEIGHT_1,weight1);
	VBOWeight2 = new VBOVec4f((char*)"Weight2",VERTEX_BONE_WEIGHT_2,weight2);
	//VBOOutPos  = new VBOVec4f((char*)"OutPos",VERTEX_POSITION,posBuffer);
	VBOTri     = new VBOVec3i((char*)"TriIdx",GL_ELEMENT_ARRAY_BUFFER,triBuffer);
	VBOBoneID1 = new VBOVec4f((char*)"BoneID1",VERTEX_BONE_ID_1,boneID1);
	VBOBoneID2 = new VBOVec4f((char*)"BoneID2",VERTEX_BONE_ID_2,boneID2);

	// Texture Buffer Object
	if (SbmShaderManager::getShaderSupport() == SbmShaderManager::SUPPORT_OPENGL_3_0)
	{
		int tranSize = boneJointList.size()*16;
		int colorSize = DeformableMesh::dMeshDynamic_p.size()*3;
		//TBOTran    = new TBOData((char*)"BoneTran",tranSize); 
	}
#endif
	return true;
}

void SbmDeformableMeshGPU::update()
{	
//#define USE_GPU_TRANSFORM 1
//#if USE_GPU_TRANSFORM
#if 0 // obsolete, use MeshInstance->update instead
	if (disableRendering) return; // do nothing

	if (!useGPUDeformableMesh)
	{
		DeformableMesh::update();
		return;
	}

	if (!binding)	return; 

	if (!initShader)
		initShaderProgram();

	SbmShaderProgram* program = SbmShaderManager::singleton().getShader(shaderName);	
	bool hasGLContext = SbmShaderManager::singleton().initOpenGL() && SbmShaderManager::singleton().initGLExtension();
	if (!useGPU && hasGLContext && program && program->finishBuild())
	{
		// initialize 
		useGPU = buildVertexBuffer();
	}

	if (!useGPU || !hasGLContext)
	{	
		for (unsigned int i=0;i<dMeshDynamic_p.size();i++)
		{
			dMeshDynamic_p[i]->visible(true);
		}
		DeformableMesh::update();		
	}	
	else
	{
		// GPU update and rendering
		//printf("GPU Deformable Model Update\n");
		for (unsigned int i=0;i<dMeshDynamic_p.size();i++)
		{
			dMeshDynamic_p[i]->visible(false);
		}
		skeleton->update_global_matrices();
		updateTransformBuffer();
		skinTransformGPU(transformBuffer,TBOTran);
	}
#endif
//#else
	
//#endif
}

void SbmDeformableMeshGPU::updateTransformBuffer()
{	
	if (transformBuffer.size() != boneJointList.size())
		transformBuffer.resize(boneJointList.size());
	for (unsigned int i=0;i<boneJointList.size();i++)
	{
		SkJoint* joint = boneJointList[i];		
		if (!joint)
			continue;
		transformBuffer[i] = bindPoseMatList[i]*joint->gmat();	
		SrQuat q = SrQuat(transformBuffer[i]);		
	}
}

void SbmDeformableMeshGPU::cleanBuffer()
{
	SmartBody::util::log(" DeformableMeshGPU::cleanBuffer 1");
	if (VBOPos) delete VBOPos;
	SmartBody::util::log(" DeformableMeshGPU::cleanBuffer 2");
	if (VBOTangent) delete VBOTangent;
	SmartBody::util::log(" DeformableMeshGPU::cleanBuffer 3");
	if (VBOBiNormal) delete VBOBiNormal;
	SmartBody::util::log(" DeformableMeshGPU::cleanBuffer 4");
	if (VBONormal) delete VBONormal;
	SmartBody::util::log(" DeformableMeshGPU::cleanBuffer 5");
	if (VBOTexCoord) delete VBOTexCoord;
	SmartBody::util::log(" DeformableMeshGPU::cleanBuffer 6");
	if (VBOBoneID1) delete VBOBoneID1;
	SmartBody::util::log(" DeformableMeshGPU::cleanBuffer 7");
	if (VBOWeight1) delete VBOWeight1;
	SmartBody::util::log(" DeformableMeshGPU::cleanBuffer 8");

	for (unsigned int i = 0; i < subMeshTris.size(); i++)
	{
		delete subMeshTris[i];
	}
	subMeshTris.clear();
	SmartBody::util::log(" DeformableMeshGPU::cleanBuffer 9");
}


void SbmDeformableMeshGPU::rebuildVertexBuffer(bool rebuild)
{
	DeformableMesh::rebuildVertexBuffer(rebuild);
#if 0
	for (size_t i = 0; i < this->getNumMeshes(); i++)
	{
		// update skin weights with new vertices
		SkinWeight* subSkin = this->skinWeights[i];
		subSkin->bindWeight.clear();
		subSkin->jointNameIndex.clear();
		subSkin->numInfJoints.clear();
		subSkin->weightIndex.clear();
		subSkin->bindWeight.emplace_back(1.0f);
		int num = subSkin->getNumVertices();
		for (unsigned int i = 0; i < num; i++)
		{
			subSkin->jointNameIndex.emplace_back(0);
			subSkin->numInfJoints.emplace_back(1);
			subSkin->weightIndex.emplace_back(0);
		}
	}

	DeformableMesh::rebuildVertexBuffer(rebuild);
	bool ok = rebuildVertexBufferGPU(rebuild);
	if (!ok)
		SmartBody::util::log("Problem rebuilding vertex buffer for %s", this->getName().c_str());
	
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::vector < std::string > pawnNames = scene->getPawnNames();
	for (unsigned int i = 0; i < pawnNames.size(); i++)
	{
		SmartBody::SBPawn* pawn = scene->getPawn(pawnNames[i]);
		DeformableMeshInstance* pawnMeshInst = pawn->getActiveMesh();
		if (pawnMeshInst && pawnMeshInst->getDeformableMesh() == this) // need to update mesh instance for this pawn
		{
			SmartBody::util::log("Updating deformable model in pawn '%s'", pawnNames[i].c_str());
			SbmDeformableMeshGPUInstance* gpuMeshInst = (SbmDeformableMeshGPUInstance*)pawnMeshInst;
			gpuMeshInst->setDeformableMesh(this);
			gpuMeshInst->initBuffer();
		}
	}
#endif
}


bool SbmDeformableMeshGPU::rebuildVertexBufferGPU(bool rebuild)
{
	if (rebuild)
	{
		initGPUVertexBuffer = false;
		cleanBuffer();
		return buildVertexBufferGPU();
	}
  return false;
}


bool SbmDeformableMeshGPU::buildVertexBufferGPU()
{
	//SmartBody::util::log(" DeformableMesh::buildSkinnedVertexBufferGPU()");
	bool hasGLContext = SbmShaderManager::singleton().initOpenGL() && SbmShaderManager::singleton().initGLExtension();
	//SmartBody::util::log(" DeformableMesh::hasGLContext = %d", hasGLContext);
	if (!hasGLContext) return false;
	//SmartBody::util::log(" Skin Weight Size = %d", skinWeights.size());
	//if (skinWeights.size() == 0 )
	//	return false;
	if (initGPUVertexBuffer) return true;
	bool hasSkinBuffer = DeformableMesh::buildSkinnedVertexBuffer();
	//GLuint program = SbmShaderManager::singleton().getShader(shaderName)->getShaderProgram();	
	SbmShaderProgram::printOglError("SbmDeformableMeshGPU::buildVertexBufferGPU #0");
	//SmartBody::util::log("posBuf size = %d", posBuf.size());
	VBOPos		= new VBOVec3f((char*)"RestPos",VERTEX_POSITION,posBuf);	

	SbmShaderProgram::printOglError("SbmDeformableMeshGPU::buildVertexBufferGPU #0.5 VBOPos");
	//SmartBody::util::log("tangentBuf size = %d", tangentBuf.size());
	VBOTangent	= new VBOVec3f((char*)"Tangent",VERTEX_TANGENT, tangentBuf);

	SbmShaderProgram::printOglError("SbmDeformableMeshGPU::buildVertexBufferGPU #1 VBOTangent");
	//SmartBody::util::log("binormalBuf size = %d", binormalBuf.size());
	VBOBiNormal = new VBOVec3f((char*)"BiNormal",VERTEX_BINORMAL, binormalBuf);
	VBONormal	= new VBOVec3f((char*)"Normal",VERTEX_VBONORMAL, normalBuf);
	//SbmShaderProgram::printOglError("SbmDeformableMeshGPU::buildVertexBufferGPU #2");
	VBOTexCoord = new VBOVec2f((char*)"TexCoord",VERTEX_TEXCOORD, texCoordBuf);

	if (skinWeights.size() != 0)
	{
		VBOBoneID1 = new VBOVec4f((char*)"BoneID1", VERTEX_BONE_ID_1, boneIDBuf_f[0]);
		VBOWeight1 = new VBOVec4f((char*)"Weight1", VERTEX_BONE_WEIGHT_1, boneWeightBuf[0]);
	}	
	//SbmShaderProgram::printOglError("SbmDeformableMeshGPU::buildVertexBufferGPU #3");

#if USE_SKIN_WEIGHT_SIZE_8
	VBOBoneID2 = new VBOVec4f((char*)"BoneID2",VERTEX_BONE_ID_2,boneIDBuf_f[1]);
	VBOWeight2 = new VBOVec4f((char*)"Weight2",VERTEX_BONE_WEIGHT_2,boneWeightBuf[1]);
#endif
	//VBOOutPos  = new VBOVec4f((char*)"OutPos",VERTEX_POSITION,posBuffer);
	//SmartBody::util::log("triBuf size = %d", triBuf.size());
	VBOTri     = new VBOVec3i((char*)"TriIdx",GL_ELEMENT_ARRAY_BUFFER,triBuf);	
	SbmShaderProgram::printOglError("SbmDeformableMeshGPU::buildVertexBufferGPU #2 triBuf");
	
	for (unsigned int i=0;i<subMeshList.size();i++)
	{
		SbmSubMesh* subMesh = subMeshList[i];
		
		//SmartBody::util::log("subMeshTriBuf size = %d", subMesh->triBuf.size());
		VBOVec3i* subMeshTriBuf = new VBOVec3i((char*)"TriIdx",GL_ELEMENT_ARRAY_BUFFER,subMesh->triBuf);
		SbmShaderProgram::printOglError("SbmDeformableMeshGPU::buildVertexBufferGPU #3 subMesh");
		subMeshTris.emplace_back(subMeshTriBuf);
	}
	// Texture Buffer Object
	if (SbmShaderManager::getShaderSupport() == SbmShaderManager::SUPPORT_OPENGL_3_0)
	{
		int tranSize = boneJointList.size()*16;
		int colorSize = DeformableMesh::dMeshDynamic_p.size()*3;
		//TBOTran    = new TBOData((char*)"BoneTran",tranSize); 
	}
	//SbmShaderProgram::printOglError("SbmDeformableMeshGPU::buildVertexBufferGPU #4");
	initGPUVertexBuffer = true;
	return true;
}

bool SbmDeformableMeshGPU::initBuffer()
{
	//SmartBody::util::log("SbmDeformableMeshGPU::initBuffer");

	return this->buildVertexBufferGPU();
#if 0
	GLuint program = SbmShaderManager::singleton().getShader(shaderName)->getShaderProgram();	

	VBOPos = new VBOVec3f((char*)"RestPos",VERTEX_POSITION,posBuf);		
	VBOTangent = new VBOVec3f((char*)"Tangent",VERTEX_TANGENT, tangentBuf);
	VBOBiNormal = new VBOVec3f((char*)"BiNormal",VERTEX_BINORMAL, binormalBuf);
	VBONormal  =  new VBOVec3f((char*)"Normal",VERTEX_VBONORMAL, normalBuf);
	VBOTexCoord = new VBOVec2f((char*)"TexCoord",VERTEX_TEXCOORD, texCoordBuf);
	VBOWeight1 = new VBOVec4f((char*)"Weight1",VERTEX_BONE_WEIGHT_1,boneWeightBuf[0]);
	VBOWeight2 = new VBOVec4f((char*)"Weight2",VERTEX_BONE_WEIGHT_2,boneWeightBuf[1]);
	//VBOOutPos  = new VBOVec4f((char*)"OutPos",VERTEX_POSITION,posBuffer);
	VBOTri     = new VBOVec3i((char*)"TriIdx",GL_ELEMENT_ARRAY_BUFFER,triBuf);
	VBOBoneID1 = new VBOVec4f((char*)"BoneID1",VERTEX_BONE_ID_1,boneIDBuf_f[0]);
	VBOBoneID2 = new VBOVec4f((char*)"BoneID2",VERTEX_BONE_ID_2,boneIDBuf_f[1]);

	for (unsigned int i=0;i<subMeshList.size();i++)
	{
		SbmSubMesh* subMesh = subMeshList[i];
		VBOVec3i* subMeshTriBuf = new VBOVec3i((char*)"TriIdx",GL_ELEMENT_ARRAY_BUFFER,subMesh->triBuf);
		subMeshTris.emplace_back(subMeshTriBuf);
	}

	// Texture Buffer Object
	if (SbmShaderManager::getShaderSupport() == SbmShaderManager::SUPPORT_OPENGL_3_0)
	{
		int tranSize = boneJointList.size()*16;
		int colorSize = DeformableMesh::dMeshDynamic_p.size()*3;
		//TBOTran    = new TBOData((char*)"BoneTran",tranSize); 
	}
	return true;
#endif	
}

SbmDeformableMeshGPUInstance::SbmDeformableMeshGPUInstance()
{
	_mesh = nullptr;
	TBOTran = nullptr;
	VBODeformPos = nullptr;
	VBODeformNormal = nullptr;
	VBODeformTangent = nullptr;
	bufferReady = false;
}

SbmDeformableMeshGPUInstance::~SbmDeformableMeshGPUInstance()
{
  cleanBuffer();
	if (TBOTran)
		delete TBOTran;
}



void SbmDeformableMeshGPUInstance::update()
{		
	if (!_skeleton || !_mesh) return; // do nothing if there is no mesh or skeleton in the instance
	if (SbmDeformableMeshGPU::disableRendering) return; // do nothing
	if (!_updateMesh) return;
	if (isStaticMesh()) return;

// #if defined(__ANDROID__)
// 	return;
// #endif
	if (!SbmDeformableMeshGPU::useGPUDeformableMesh)
	{
		DeformableMeshInstance::update();		
		return;
	}	

	
	SbmShaderProgram* program = SbmShaderManager::singleton().getShader(shaderName);	
	bool hasGLContext = SbmShaderManager::singleton().initOpenGL() && SbmShaderManager::singleton().initGLExtension();	

	if (!SbmDeformableMeshGPU::initShader && hasGLContext)
	{
		SbmDeformableMeshGPU::initShaderProgram();
	}
	
	if (!bufferReady && hasGLContext && program && program->finishBuild())
	{
		// initialize 
		bufferReady = initBuffer();
	}
	
	if (!bufferReady || !hasGLContext)
	{	
		DeformableMeshInstance::setVisibility(true);
		DeformableMeshInstance::update();		
	}	
	else if (dynamic_cast<SbmDeformableMeshGPU*>(_mesh))
	{
		SbmDeformableMeshGPU* gpuMesh = dynamic_cast<SbmDeformableMeshGPU*>(_mesh);
		// GPU update and rendering
		//printf("GPU Deformable Model Update\n");
		DeformableMeshInstance::setVisibility(false);
		_updateMesh = true;		
		_skeleton->update_global_matrices();
		gpuBlendShape();
		updateTransformBuffer();		
		gpuMesh->skinTransformGPU(this);
	}
}

void SbmDeformableMeshGPUInstance::gpuBlendShape()
{
#if 1 //!defined(__ANDROID__)
	//SmartBody::util::log("gpuBlendShape::start gpuBlendShape");
	SbmDeformableMeshGPU* gpuMesh = dynamic_cast<SbmDeformableMeshGPU*>(_mesh);
	if (!gpuMesh) return;
	if (gpuMesh->blendShapeMap.size() == 0) return;

	for (std::map<std::string, std::vector<SrSnModel*> >::iterator mIter = gpuMesh->blendShapeMap.begin();
		mIter != gpuMesh->blendShapeMap.end();
		mIter++)
	{
		SrSnModel* writeToBaseModel = nullptr;
		SkinWeight* skinWeight = nullptr;
		int vtxBaseIdx = 0;
		std::vector<SrSnModel*>& targets = (*mIter).second;
		//SmartBody::util::log("gpuBlendShape::gpuMesh->dMeshStatic_p.size() = %d", gpuMesh->dMeshStatic_p.size());
		for (size_t i = 0; i < gpuMesh->dMeshStatic_p.size(); ++i)
		{
			if (strcmp(gpuMesh->dMeshStatic_p[i]->shape().name, mIter->first.c_str()) == 0)
			{
				writeToBaseModel = gpuMesh->dMeshStatic_p[i];
				if (gpuMesh->skinWeights.size() > i)
					skinWeight = gpuMesh->skinWeights[i];
				else
					skinWeight = nullptr;
				break;
			}
			else
			{
				// skip vertices for this sub mesh
				vtxBaseIdx += gpuMesh->dMeshStatic_p[i]->shape().V.size();
			}
		}

		if (!writeToBaseModel)
		{
			if (gpuMesh->dMeshStatic_p.size() == 0 || targets.size() == 0)
				return;
			if (gpuMesh->dMeshStatic_p[0]->shape().V.size() == targets[0]->shape().V.size())
			{
				// can't find the base model, assuming it's the first mesh
				//SmartBody::util::log("Can't find BlendShape BaseModel, will assume it's the first mesh.");
				writeToBaseModel = gpuMesh->dMeshStatic_p[0];
				skinWeight = gpuMesh->skinWeights[0];
				vtxBaseIdx = 0;
			}
			//continue;
		}
// 		if (!writeToBaseModel)
// 			return;

		//if (!skinWeight)
		//	return;
		//SmartBody::util::log("gpuBlendShape::before CPU BlendShape");
		//DeformableMeshInstance::blendShapes();
		//SmartBody::util::log("gpuBlendShape::after CPU Blendshape");
		SrMat bindShapeMat;
		

		VBOVec3f* posVBO = this->getVBODeformPos();
		//gpuMesh->getPosVBO();		
		if (skinWeight)
		{
			bindShapeMat = skinWeight->bindShapeMat;
			posVBO = gpuMesh->getPosVBO();
		}
		glBindBuffer(GL_ARRAY_BUFFER, posVBO->VBO()->m_iVBO_ID);
		//SmartBody::util::log("gpuBlendShape::after glBindBuffer");
#if defined(__ANDROID__) || defined(SB_IPHONE) 
		float* pData = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, gpuMesh->posBuf.size()*sizeof(SrVec), GL_MAP_WRITE_BIT);
		//SmartBody::util::log("gpuBlendShape::after glMapBufferRange");
#else
		float* pData = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
#endif
		std::map<int, std::vector<int> >& vtxNewVtxIdxMap = gpuMesh->vtxNewVtxIdxMap;
		SrModel& baseModel = writeToBaseModel->shape();

		for (int i = 0; i < baseModel.V.size(); i++)
		{
			int iVtx = vtxBaseIdx + i;
			SrVec basePos = baseModel.V[i] * bindShapeMat;
			pData[iVtx * 3] = basePos[0];
			pData[iVtx * 3 + 1] = basePos[1];
			pData[iVtx * 3 + 2] = basePos[2];
			if (vtxNewVtxIdxMap.find(iVtx) != vtxNewVtxIdxMap.end())
			{
				std::vector<int>& idxMap = vtxNewVtxIdxMap[iVtx];
				// copy related vtx components 
				for (unsigned int k = 0; k < idxMap.size(); k++)
				{
					int idx = idxMap[k];
					pData[idx * 3] = basePos[0];
					pData[idx * 3 + 1] = basePos[1];
					pData[idx * 3 + 2] = basePos[2];
				}
			}
		}
		//SmartBody::util::log("gpuBlendShape::before glUnmapBuffer");
		glUnmapBuffer(GL_ARRAY_BUFFER);
		//SmartBody::util::log("gpuBlendShape::after glUnmapBuffer");
	}
#endif
}


bool SbmDeformableMeshGPUInstance::initBuffer()
{	
	SbmDeformableMeshGPU* gpuMesh = dynamic_cast<SbmDeformableMeshGPU*>(_mesh);
	//SmartBody::util::log("SbmDeformableMeshGPUInstance::initBuffer, mesh = %d, gpuMesh = %d", _mesh, gpuMesh);
	if (!gpuMesh)
		return false;
	bool hasVertexBuffer = gpuMesh->buildVertexBufferGPU();	
	if (!hasVertexBuffer) return false;
	if (SbmShaderManager::getShaderSupport() == SbmShaderManager::SUPPORT_OPENGL_2_0)
	{
		cleanBuffer();
		int tranSize = _mesh->boneJointIdxMap.size()*16;
		//int colorSize = dynamicMesh.size()*3;
		//TBOTran    = new TBOData((char*)"BoneTran",tranSize); 
		
		//SmartBody::util::log("SbmDeformableMeshGPUInstance::initBuffer, _deformPosBuf size = %d, posBuf size = %d", _deformPosBuf.size(), gpuMesh->posBuf.size());
		//SbmShaderProgram::printOglError("SbmDeformableMeshGPUInstance::initBuffer #0");
		VBODeformPos = new VBOVec3f((char*)"DeformPos", VERTEX_POSITION, _deformPosBuf);
		//SbmShaderProgram::printOglError("SbmDeformableMeshGPUInstance::initBuffer #1");
		VBODeformNormal = new VBOVec3f((char*)"DeformNormal", VERTEX_POSITION, _deformNormalBuf);
		//SbmShaderProgram::printOglError("SbmDeformableMeshGPUInstance::initBuffer #2");
		VBODeformTangent = new VBOVec3f((char*)"DeformTangent", VERTEX_POSITION, _deformTangentBuf);
		//SbmShaderProgram::printOglError("SbmDeformableMeshGPUInstance::initBuffer #3");
		bufferReady = true;
	}
	return true;
}

void SbmDeformableMeshGPUInstance::cleanBuffer()
{
	if (TBOTran) delete TBOTran;	
	if (VBODeformPos) delete VBODeformPos;
	if (VBODeformNormal) delete VBODeformNormal;
	if (VBODeformTangent) delete VBODeformTangent;
}

void SbmDeformableMeshGPUInstance::setDeformableMesh( DeformableMesh* mesh )
{
	DeformableMeshInstance::setDeformableMesh(mesh);
	bufferReady = false;
}

MeshSubset::~MeshSubset()
{
	if (VBOTri)
		delete VBOTri;

}
#endif
