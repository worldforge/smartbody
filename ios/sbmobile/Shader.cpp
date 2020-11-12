/*
	Shader code for Smartbody JS
	Note: due to the hardware implementation, the maximum number of vertex uniform vectors vary from vendors to vendors
	Some hardware supports 1024 at maximum, but some old one's only support 254 at maximum.
	Also for Windows platform, WebGL uses ANGEL - thin layer between DirectX and OpenGL which interprets OGL commands to DirectX.
	But it is said, DirectX 9.0 version of ANGEL only supports 254 uniform vectors, while DirectX 11 supports more than that.
	For Firefox, you could disable ANGEL and use native OPENGL, but it is not the case for Chrome.
	To accommodate very limited number of vertex uniform vectors, let's say 254 at most, we did the following changes:
	1. Use Quaternion + Translation representation of Xforms to reduce half of the uniform needed. 
	2. Hard coded Material in Shader
	3. Supports only two light source each only with position + diffuse
	Thus, total uniform vector needed is:
	QT(vec4 * 120 * 2) + light(vec4 * 3) + MVP(vec4 * 4) + MV(vec4 * 4) + meshScale(vec4 * 1) = 252 vec4
*/
#include <string>
#include <vector>
#include <stdlib.h>
#include "Shader.h"
#include "sb/SBScene.h"
#include "sb/SBAttribute.h"
#include <sb/SBUtilities.h>
#include "sbm/GPU/SbmTexture.h"
#include "sr/sr_camera.h"
#include "sr/sr_light.h"
#include "sr/sr_euler.h"
#include "sr/sr_quat.h"
#include "boost/lexical_cast.hpp"
#include "esUtil.h"

#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <sbm/GPU/VBOData.h>

#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/gtc/type_ptr.hpp"
#include "external/glm/glm/gtc/matrix_transform.hpp"

#if defined(__ANDROID__)
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#elif defined(IPHONE_BUILD)
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#elif defined(EMSCRIPTEN)
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#endif

#define MAX_VERTEX_UNIFORM_1024 1024
#if !defined(__ANDROID__) && !defined(SB_IPHONE)
#include<emscripten.h>
#endif

using namespace std;
using namespace SmartBody;

#if __cplusplus
extern "C"
{
#endif

	char vShaderStr[] =  
		"struct lightSource {							\n"
		"	vec4	position;							\n"
		"	vec4	ambient;							\n"
		"	vec4	diffuse;							\n"
		"};												\n"
		"struct material {								\n"
		"	vec4	ambient;							\n"
		"	vec4    diffuse;							\n"
		"};												\n"
		"const int numOfLights = 4;						\n"
		"attribute vec4 aPosition;						\n"
		"attribute vec3 aNormal;						\n"
		"uniform   mat4 uMVPMatrix;						\n"
		"uniform   mat4 uMVMatrix;						\n"
		"attribute vec2 aTexCoord;						\n"
		"uniform   material	uMtrl;						\n"
		"uniform   lightSource uLights[numOfLights];    \n"
		"varying   vec4 vComputedLightColor;			\n"
		"varying   vec2 vTexCoord;						\n"
		"uniform   vec4 uQuaternion[120];				\n"
		"uniform   vec4 uTranslation[120];				\n"
		"uniform   float uMeshScale;					\n"
		"attribute vec4 BoneID1;						\n"
		"attribute vec4 BoneWeight1;					\n"
		"const float c_zero = 0.0;											\n"
		"const float c_one  = 1.0;											\n"
		"vec4 calculateDirectionalShading(int i){							\n"
		"	vec4 computedColor = vec4(c_zero, c_zero, c_zero, c_zero);		\n"
		//	ambient
		"   computedColor += (uLights[i].ambient * uMtrl.ambient);			\n"
		//	diffuse light direction is normalized and in eye space
		"   vec3 normal = normalize((uMVMatrix * vec4(aNormal, c_zero)).xyz);  \n"
		"   vec3 nlightDir = normalize((uMVMatrix * uLights[i].position).xyz);\n" 
		"   float ndotl = max(dot(normal, nlightDir), c_zero);					\n"
		"   computedColor += (ndotl * uLights[i].diffuse * uMtrl.diffuse);	\n"
		"   return computedColor;											\n"
		"}																    \n"
		"vec4 GetRotationQuaternion(float id){								\n"
		"	int idx = int(id);												\n"
		"	return uQuaternion[idx];										\n"
		"}																	\n"
		"vec3 GetTranslation(float id){										\n"
		"	int idx = int(id);												\n"
		"	return uTranslation[idx].xyz;									\n"
		"}																	\n"
		"vec4 quaternionMul(vec4 q1, vec4 q2){								\n"
		"	vec4 q;															\n"
		"	q.w = (q1.w*q2.w) - (q1.x*q2.x + q1.y*q2.y + q1.z*q2.z);		\n"
		"	q.x = q1.y*q2.z - q1.z*q2.y;									\n"
		"	q.y = q1.z*q2.x - q1.x*q2.z;									\n"
		"	q.z = q1.x*q2.y - q1.y*q2.x;									\n"
		"	q.x += (q1.x*q2.w) + (q2.x*q1.w);								\n"
		"	q.y += (q1.y*q2.w) + (q2.y*q1.w);								\n"
		"	q.z += (q1.z*q2.w) + (q2.z*q1.w);								\n"
		"	return q;"
		"}																	\n"
		"vec4 TransformPos(vec3 position, vec4 boneid, vec4 boneweight){	\n"
		"	vec3 pos = vec3(c_zero,c_zero,c_zero);							\n"
		"	vec4 tempQ, tempQConj;											\n"
		"   vec3 tempT;														\n"
		"	for (int i = int(c_zero); i < 4; i++)							\n"
		"	{																\n"
		"		tempQ = GetRotationQuaternion(boneid[i]);					\n"
		"		tempQConj = vec4(-tempQ.xyz, tempQ.w);						\n"
		"		tempT = GetTranslation(boneid[i]);							\n"
		//transform a point by QT v' = Q * v * conj(Q) + T
		"		pos += (quaternionMul(quaternionMul(tempQ, vec4(position, c_zero)),tempQConj).xyz + tempT)*boneweight[i];\n"		
		"	}															    \n"
		"	return vec4(pos,c_one);											\n"
		"}										 							\n"
		"void main()						\n"
		"{									\n"
		"	gl_PointSize = 2.0;				\n"
		"   vTexCoord = aTexCoord;			\n"
		"   vec3 pos = vec3(aPosition.xyz)*uMeshScale;						\n"
		"	vec4 skinPos = TransformPos(pos.xyz,BoneID1,BoneWeight1);		\n"
		"	gl_Position = uMVPMatrix * skinPos;								\n"
		"   vec4 shadeColor = vec4(c_zero, c_zero, c_zero, c_zero);			\n"
		"	for(int i = 0; i < numOfLights; i++){						    \n"
		"		shadeColor += calculateDirectionalShading(i);				\n"
		"	}																\n"
		"	vComputedLightColor = shadeColor;								\n"
		"}																    \n"

		;
	char vShaderLimitedStr[] =
		"struct lightSource {												\n"
				"	vec4	position;												\n"
				"	vec4	diffuse;												\n"
				"};																	\n"
				"const int numOfLights = 4;											\n"
				"const vec4 mtrlAmbient = vec4(0.2, 0.2, 0.2, 1.0);					\n"
				"const vec4 mtrlDiffuse = vec4(0.8, 0.8, 0.8, 1.0);					\n"
				"const vec4 lightAmbient = vec4(0.1, 0.1, 0.1, 1.0);				\n"
				"attribute vec4 aPosition;											\n"
				"attribute vec3 aNormal;											\n"
				"uniform   mat4 uMVPMatrix;											\n"
				"uniform   mat4 uMVMatrix;											\n"
				"attribute vec2 aTexCoord;											\n"
				"uniform   lightSource uLights[numOfLights];						\n"
				"varying   vec4 vComputedLightColor;								\n"
				"varying   vec2 vTexCoord;											\n"
				"uniform   vec4 uQuaternion[120];									\n"
				"uniform   vec4 uTranslation[120];									\n"
				"uniform   float uMeshScale;										\n"
				"attribute vec4 BoneID1;											\n"
				"attribute vec4 BoneWeight1;										\n"
				"const float c_zero = 0.0;											\n"
				"const float c_one  = 1.0;											\n"
				"vec4 calculateDirectionalShading(int i){							\n"
				"	vec4 computedColor = vec4(c_zero, c_zero, c_zero, c_zero);		\n"
				"   computedColor += (lightAmbient * mtrlAmbient);					\n"
				"   vec3 normal = normalize((uMVMatrix * vec4(aNormal, c_zero)).xyz);  \n"
				"   vec3 nlightDir = normalize((uMVMatrix * uLights[i].position).xyz);\n"
				"   float ndotl = max(dot(normal, nlightDir), c_zero);				\n"
				"   computedColor += (ndotl * uLights[i].diffuse * mtrlDiffuse);	\n"
				"   return computedColor;											\n"
				"}																    \n"
				"vec4 GetRotationQuaternion(float id){								\n"
				"	int idx = int(id);												\n"
				"	return uQuaternion[idx];										\n"
				"}																	\n"
				"vec3 GetTranslation(float id){										\n"
				"	int idx = int(id);												\n"
				"	return uTranslation[idx].xyz;									\n"
				"}																	\n"
				"vec4 quaternionMul(vec4 q1, vec4 q2){								\n"
				"	vec4 q;															\n"
				"	q.w = (q1.w*q2.w) - (q1.x*q2.x + q1.y*q2.y + q1.z*q2.z);		\n"
				"	q.x = q1.y*q2.z - q1.z*q2.y;									\n"
				"	q.y = q1.z*q2.x - q1.x*q2.z;									\n"
				"	q.z = q1.x*q2.y - q1.y*q2.x;									\n"
				"	q.x += (q1.x*q2.w) + (q2.x*q1.w);								\n"
				"	q.y += (q1.y*q2.w) + (q2.y*q1.w);								\n"
				"	q.z += (q1.z*q2.w) + (q2.z*q1.w);								\n"
				"	return q;"
				"}																	\n"
				"vec4 TransformPos(vec3 position, vec4 boneid, vec4 boneweight){	\n"
				"	vec3 pos = vec3(c_zero,c_zero,c_zero);							\n"
				"	vec4 tempQ, tempQConj;											\n"
				"   vec3 tempT;														\n"
				"	for (int i = int(c_zero); i < 4; i++)							\n"
				"	{																\n"
				"		tempQ = GetRotationQuaternion(boneid[i]);					\n"
				"		tempQConj = vec4(-tempQ.xyz, tempQ.w);						\n"
				"		tempT = GetTranslation(boneid[i]);							\n"
				//transform a point by QT v' = Q * v * conj(Q) + T
				"		pos += (quaternionMul(quaternionMul(tempQ, vec4(position, c_zero)),tempQConj).xyz + tempT)*boneweight[i];\n"
				"	}															    \n"
				"	return vec4(pos,c_one);											\n"
				"}										 							\n"
				"void main()						\n"
				"{									\n"
				"	gl_PointSize = 2.0;				\n"
				"   vTexCoord = aTexCoord;			\n"
				"   vec3 pos = vec3(aPosition.xyz) * uMeshScale;					\n"
				//"	gl_Position = uMVPMatrix * vec4(aPosition.xyz, 1.0);			\n"
				"	vec4 skinPos = TransformPos(pos.xyz,BoneID1,BoneWeight1);		\n"
				"	gl_Position = uMVPMatrix * skinPos;								\n"
				"   vec4 shadeColor = vec4(c_zero, c_zero, c_zero, c_zero);			\n"
				"	for(int i = 0; i < numOfLights; i++){						    \n"
				"		shadeColor += calculateDirectionalShading(i);				\n"
				"	}																\n"
				"	vComputedLightColor = shadeColor;								\n"
				"}																    \n"

;

	char vShaderStaticMeshStr[] =
		"struct lightSource {							\n"
				"	vec4	position;							\n"
				"	vec4	ambient;							\n"
				"	vec4	diffuse;							\n"
				"};												\n"
				"struct material {								\n"
				"	vec4	ambient;							\n"
				"	vec4    diffuse;							\n"
				"};												\n"
				"const int numOfLights = 4;						\n"
				"attribute vec4 aPosition;						\n"
				"attribute vec3 aNormal;						\n"
				"uniform   mat4 uMVPMatrix;						\n"
				"uniform   mat4 uMVMatrix;						\n"
				"attribute vec2 aTexCoord;						\n"
				"uniform   material	uMtrl;						\n"
				"uniform   lightSource uLights[numOfLights];    \n"
				"varying   vec4 vComputedLightColor;			\n"
				"varying   vec2 vTexCoord;						\n"
				"uniform   float uMeshScale;					\n"
				"const float c_zero = 0.0;											\n"
				"const float c_one  = 1.0;											\n"
				"vec4 calculateDirectionalShading(int i){							\n"
				"	vec4 computedColor = vec4(c_zero, c_zero, c_zero, c_zero);		\n"
				//	ambient
				"   computedColor += (uLights[i].ambient * uMtrl.ambient);			\n"
				//	diffuse light direction is normalized and in eye space
				"   vec3 normal = normalize((uMVMatrix * vec4(aNormal, c_zero)).xyz);  \n"
				"   vec3 nlightDir = normalize((uMVMatrix * uLights[i].position).xyz);\n"
				"   float ndotl = max(dot(normal, nlightDir), c_zero);					\n"
				"   //computedColor += (ndotl * uLights[i].diffuse * uMtrl.diffuse);	\n"
				"   computedColor += ndotl;	\n"
				"   return computedColor;											\n"
				"}																    \n"
				"void main()						\n"
				"{									\n"
				"	gl_PointSize = 2.0;				\n"
				"   vTexCoord = aTexCoord;			\n"
				"   vec3 pos = vec3(aPosition.xyz); //*uMeshScale;						\n"
				"	vec4 skinPos = vec4(pos.xyz,1.0);		\n"
				"	gl_Position = uMVPMatrix * skinPos;								\n"
				"   vec4 shadeColor = vec4(c_zero, c_zero, c_zero, c_zero);			\n"
				"	for(int i = 0; i < numOfLights; i++){						    \n"
				"		shadeColor += calculateDirectionalShading(i);				\n"
				"	}																\n"
				"	vComputedLightColor = vec4(shadeColor.rgb, 1.0);						\n"
				"}																    \n";


	 char vShaderNormalMapStr[] =
                    //"precision mediump float;                       \n"
                    "struct lightSource {							\n"
                    "	mediump vec4	position;							\n"
                    "	mediump vec4	ambient;							\n"
                    "	mediump vec4	diffuse;							\n"
                    "};												\n"
                    "const int numOfLights = 4;						\n"
                    "attribute vec4 aPosition;						\n"
                    "attribute vec3 aNormal, aTangent;				\n"
                    "uniform   mat4 uMVPMatrix;						\n"
                    "uniform   mat4 uMVMatrix;						\n"
                    "attribute vec2 aTexCoord;						\n"
                    "uniform   lightSource uLights[numOfLights];    \n"
                    "varying   vec2 vTexCoord;						\n"
                    "varying   vec3 lightDir[numOfLights], halfVec[numOfLights];  \n"
                    "uniform   float uMeshScale;					\n"
                    "const float c_zero = 0.0;											\n"
                    "const float c_one  = 1.0;											\n"
                    "void main()						\n"
                    "{									\n"
                    "	gl_PointSize = 2.0;				\n"
                    "   vTexCoord = aTexCoord;			\n"
                    "   vec3 pos = vec3(aPosition.xyz)* 1.0;						\n"
                    "	vec4 skinPos = vec4(pos.xyz,1.0);		                        \n"
                    "	gl_Position = uMVPMatrix * skinPos;								\n"
                    
                    "   vec3 vertexPos = -normalize(vec3(uMVMatrix * vec4(skinPos.xyz,1.0)));          \n"
                    "   mat3 tangentMat;                                                \n"
					"   tangentMat[0] = normalize((uMVMatrix * vec4(aTangent,0.0)).xyz);            \n"
					"   tangentMat[2] = normalize((uMVMatrix * vec4(aNormal,0.0)).xyz);           \n"
                    "   tangentMat[1] = cross(tangentMat[0], tangentMat[2]);            \n"
                    "   vec3 eyeVec = normalize(vertexPos*tangentMat);                  \n"
                    "   for (int i=0;i<numOfLights;i++)                                \n"
                    "   {                                                              \n"
                    "       vec3 v = normalize((uMVMatrix * vec4(uLights[i].position.xyz,0)).xyz); \n"
                    "       lightDir[i] = normalize(v*tangentMat);                     \n"
                    "       halfVec[i] = normalize(lightDir[i]+eyeVec);                \n"
                    "   }                                                              \n"                    
                    "}																    \n"

    ;

	 char vShaderSkinStr[] =
		 //"#version 430          \n"
		 "#version 300 es         \n"
		 "layout(location = 0) in vec3 position; \n"
		 "layout(location = 1) in vec3 normal; \n"
		 "layout(location = 2) in vec3 tangent; \n"
		 "layout(location = 3) in vec4 BoneID1; \n"
		 "layout(location = 4) in vec4 BoneWeight1; \n"
		 "uniform mat4 Transform[63]; \n"
		 "uniform float meshScale; \n"
		 "out vec3 deformPos; \n"
		 "out vec3 deformNormal; \n"
		 "out vec3 deformTangent; \n"
		 "mat4 GetTransformation(float id) \n"
		 "{ \n"
		 "	int idx = int(id); \n"
		 "	return Transform[idx]; \n"
		 "} \n"
		 "mat3 TransformPos(vec3 position, vec3 normal, vec3 tang, vec4 boneid, vec4 boneweight) \n"
		 "{ \n"
		 "	vec3 pos = vec3(0, 0, 0); \n"
		 "	vec3 n = vec3(0, 0, 0); \n"
		 "	vec3 t = vec3(0, 0, 0); \n"
		 "	vec3 b = vec3(0, 0, 0); \n"
		 "	mat4 tempT; \n"
		 "	vec3 tempt; \n"
		 "	for (int i = 0; i<4; i++) \n"
		 "	{ \n"
		 "		tempT = GetTransformation(boneid[i]); \n"
		 "		pos += (vec4(position, 1.0)*tempT).xyz*boneweight[i]; \n"
		 "		n += (vec4(normal, 0.0)*tempT).xyz*boneweight[i]; \n"
		 "		t += (vec4(tang, 0.0)*tempT).xyz*boneweight[i]; \n"
		 "	} \n"
		 "	mat3 result; \n"
		 "	result[0] = vec3(pos); \n"
		 "	result[1] = vec3(n); \n"
		 "	result[2] = vec3(t); \n"
		 "	return result; \n"
		 "} \n"
		 "void main() \n"
		 "{ \n"
		 "	// the following three lines provide the same result \n"
		 "	vec3 pos = vec3(position.xyz)*meshScale; \n"
		 "	mat3 skin = TransformPos(pos, normal, tangent, BoneID1, BoneWeight1); \n"
		 "	deformPos = skin[0];	 \n"
		 "	deformNormal = skin[1]; \n"
		 "	deformTangent = skin[2]; \n"
		 "}\n";

	// dummy fragment shader to avoid linking error ?
	char fShaderSkinStr[] =
		"#version 300 es \n"
		"precision mediump float;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"  fragColor = vec4(1.0,1.0,1.0,1.0);\n"
		"}";


	char vShaderBackgroundStr[] =
				"uniform   mat4 uMVPMatrix;						\n"
				"attribute vec4 aPosition;						\n"
				"attribute vec2 aTexCoord;						\n"
				"varying   vec2 vTexCoord;						\n"
				"const float c_zero = 0.0;											\n"
				"const float c_one  = 1.0;											\n"
				"void main()						\n"
				"{									\n"
				"   vTexCoord = aTexCoord;			\n"
				"	gl_Position = uMVPMatrix * aPosition;							\n"
				"}																    \n"

;
	char fShaderBackgroundStr[] = 
		"precision mediump float;											\n"
		"varying   vec2 vTexCoord;											\n"
		"uniform   sampler2D sTexture;										\n"
		"void main()														\n"
		"{																	\n"
		"  vec4 texColor = texture2D( sTexture, vTexCoord );				\n"
		"  gl_FragColor  =  texColor;                   			\n"
		"  //gl_FragColor  =  vec4(1.0,0,0,1.0);                   			\n"
		"}																	\n"

		;




	char fShaderStr[] = 
		"precision mediump float;											\n"
		"varying   vec2 vTexCoord;											\n"
		"uniform   sampler2D sTexture;										\n"
		"uniform   int blendTextureWithLight;								\n"
		"varying   vec4 vComputedLightColor;								\n"
		"void main()														\n"
		"{																	\n"
		"  vec4 texColor = texture2D( sTexture, vTexCoord );				\n"
		"  vec4 lightColor = vec4(1.0,1.0,1.0,1.0);							\n"
		"  //if (blendTextureWithLight==1)									\n"
		"  lightColor = vComputedLightColor;										\n"
		"  if (texColor.a < 0.2) discard;                                   \n"
		"  gl_FragColor  = texColor * lightColor;							\n"
		"  //gl_FragColor  =  vec4(1.0,0,0,1.0);                   			\n"
		"  //gl_FragColor  =  texColor;                   			\n"
		"}																	\n"

		;


   


    char fShaderNormalMapStr[] =
                    //"precision mediump float;                       \n"
    				"precision mediump float;                       \n"
                    "struct material {								\n"
                    "	vec4	ambient;							\n"
                    "	vec4    diffuse;							\n"
                    "	float   shininess;							\n"
                    "};												\n"
                    "struct lightSource {							\n"
                    "	mediump vec4	position;							\n"
                    "	mediump vec4	ambient;							\n"
                    "	mediump vec4	diffuse;							\n"
                    "};												\n"
                    "const int numOfLights = 4;						\n"
                    "varying   vec3 lightDir[numOfLights], halfVec[numOfLights];\n"
                    "varying   vec2 vTexCoord;											\n"
                    "uniform   sampler2D sTexture, normalTexture, specularTexture;			            \n"
                    "uniform   lightSource uLights[numOfLights];    \n"
                    "uniform   material	uMtrl;						\n"
                    ""
                    "vec4 calculateLighting(vec4 lightDiffuse, vec4 specularColor, vec3 normal, vec3 vlightDir, vec3 vHalfVec, vec3 tcolor) {							\n"
                    "	vec4 computedColor = vec4(0.0, 0.0, 0.0, 0.0);		\n"
                    "   float ndotl = max(dot(normal, vlightDir), 0.0);					\n"
                    //"   computedColor += (lightDiffuse.xyz * uMtrl.diffuse.xyz * tcolor * ndotl);	\n"
                    "   computedColor.xyz += (lightDiffuse.xyz * tcolor * ndotl);	\n"
                    //"   computedColor.xyz += (ndotl);	\n"
                    "   float specularIntensity = pow (max (dot (normalize(vHalfVec), normal), 0.0), uMtrl.shininess + 0.01);\n"
                    //"   float specularIntensity = dot (normalize(vHalfVec), normal);\n"
                    //"   computedColor.xyz += (specularIntensity);        \n"
                    "   computedColor.xyz += (specularIntensity * specularColor.xyz);        \n"
                    //"   computedColor += (specularIntensity * vec4(1.0,1.0,1.0,1.0));        \n"
                    //"   computedColor += ndotl;        \n"
                    "   return computedColor;											\n"
                    "}																    \n"
                    "void main()														\n"
                    "{																	\n"
                    "  vec4 texColor = texture2D( sTexture, vTexCoord );				\n"
					"  //if (texColor.a < 0.8) discard;                                   \n"
					"  if (texColor.a < 0.2) discard;                                   \n"
                    "  vec3 normal = 2.0 * texture2D(normalTexture, vTexCoord.st).rgb - 1.0;\n"
                    "  vec4 specularColor = texture2D( specularTexture, vTexCoord );				\n"
                    "  normal = normalize (normal);                                  \n"
                    "  vec4 vComputedLightColor = vec4(0.0,0.0,0.0,0.0);             \n"
                    "  for (int i=0;i<3; i++)                              \n"
                    "  {                                                             \n"
                    "       vComputedLightColor += calculateLighting(uLights[i].diffuse, specularColor, normal, lightDir[i], halfVec[i], texColor.xyz); \n"
                    "  }                                                             \n"
                    "  vComputedLightColor.a = 1.0;                                  \n"
                    //"  gl_FragColor  = texColor * vComputedLightColor;				 \n"
                    "  gl_FragColor  = vComputedLightColor;				 \n"
                    "  //gl_FragColor  =  vec4(specularColor.r , specularColor.g , specularColor.b ,1.0);                   			     \n"
                    "}																	\n"

    ;
	char vShaderStrSphere[] = 
		"struct lightSource {				\n"
		"	vec4	position;				\n"
		"	vec4	ambient;				\n"
		"	vec4	diffuse;				\n"
		"};									\n"
		"attribute vec4 aPosition;											\n"
		"attribute vec3 aNormal;											\n"
		"uniform   vec4 uColor;												\n"
		"uniform   mat4 uMVPMatrix;											\n"
		"uniform   mat4 uMVMatrix;											\n"
		"uniform   int uLightEnabled;										\n"
		"const int numOfLights = 4;											\n"
		"const float c_zero = 0.0;											\n"
		"const float c_one  = 1.0;											\n"
		"uniform   lightSource uLights[numOfLights];						\n"
		"varying   vec4 vComputedColor;										\n"
		"varying   vec4 vColor;												\n"
		"vec4 calculateDirectionalShading(int i){							\n"
		"	vec4 computedColor = vec4(c_zero, c_zero, c_zero, c_zero);		\n"
		//	ambient
		"   computedColor += (uLights[i].ambient * vec4(0.2, 0.2, 0.2, 1.0));\n"
		//	diffuse light direction is normalized and in eye space
		"   vec3 normal = normalize((uMVMatrix * vec4(aNormal, c_zero)).xyz);\n"
		"   vec3 nlightDir = normalize((uMVMatrix * uLights[i].position).xyz);\n" 
		"   float ndotl = max(dot(normal, nlightDir), c_zero);				\n"
		"   computedColor += (ndotl * uLights[i].diffuse * vec4(0.8, 0.8, 0.8, 1.0));\n"
		"   return computedColor;											\n"
		"}																    \n"
		"void main()														\n"
		"{																	\n"
		"	gl_PointSize = 5.0;												\n"
		"   vec4 shadeColor;												\n"
		"	gl_Position = uMVPMatrix * aPosition;							\n"
		"	if( uLightEnabled == 1 ){										\n"
		"			shadeColor = vec4(c_zero, c_zero, c_zero, c_zero);		\n"
		"		for(int i = 0; i < numOfLights; i++){						\n"
		"			shadeColor += calculateDirectionalShading(i);			\n"
		"		}															\n"
		"	}																\n"
		"	else															\n"
		"		shadeColor = vec4(c_one, c_one, c_one, c_one);				\n"														
		"	vComputedColor = shadeColor;									\n"
		"   vColor		   = uColor;										\n"
		"}																	\n"

		;
	char fShaderStrSphere[] =  
		"precision mediump float;											\n"
		"varying   vec4 vComputedColor;										\n"
		"varying   vec4 vColor;												\n"
		"void main()														\n"
		"{																	\n"
		"  gl_FragColor  = vComputedColor * vColor;							\n"
		//"  gl_FragColor  = vColor;											\n"
		"}																	\n"

		;
 
	std::vector<SrLight> _lights;
	GLint maxVUniforms = 0;
	SbmShaderProgram* skinningShader;

	GLuint SHADER_API esLoadShader(GLenum type, const char *shaderSrc)
	{

		GLuint shader;
		GLint compiled;

		//step 1: create the Shader object
		shader = glCreateShader(type);

		if(shader == 0)
			return 0;
		//step 2: load the Shader source
		glShaderSource(shader, 1, &shaderSrc, NULL);
		//SbmShaderProgram::printOglError("esLoadShader::glShaderSource");

		//step 3: compile the Shader
		glCompileShader(shader);
		//SbmShaderProgram::printOglError("esLoadShader::glCompileShader");

		//step 4: check the compilation
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

		if(!compiled)
		{
			//SmartBody::util::log("shader error : %s", shaderSrc);
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

			if(infoLen > 0){ 
				char* infoLog = (char*)malloc(sizeof(char) * infoLen);
				glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
                SmartBody::util::log("Shader Build Log = %s", infoLog);
				free(infoLog);
			}

			glDeleteShader(shader);
			return 0;
		}
		else
		{
			//SmartBody::util::log("shader is complied correctly");
		}

		return shader;
	}

	GLuint SHADER_API esLoadProgram(const char *vertShaderSrc, const char *fragShaderSrc)
	{
		GLuint vertexShader;
		GLuint fragmentShader;
		GLuint programObject;
		GLint  linked;
		//step 1: load vertex shader
		vertexShader		= esLoadShader(GL_VERTEX_SHADER, vertShaderSrc);
		if(vertexShader == 0)
		{
			glDeleteShader(vertexShader);
			return 0;
		}
		//step 2: load fragment shader
		fragmentShader		= esLoadShader(GL_FRAGMENT_SHADER, fragShaderSrc);
		if(fragmentShader == 0)
		{
			glDeleteShader(fragmentShader);
			return 0;
		}
		//setp 3: create program object
		programObject = glCreateProgram();
		if(programObject == 0)
			return 0;

		//step 4: attach vertexShader and fragmentShader to the program
		glAttachShader(programObject, vertexShader);
		//SbmShaderProgram::printOglError("esLoadShader::glAttachShader, vertexShader");
		glAttachShader(programObject, fragmentShader);
		//SbmShaderProgram::printOglError("esLoadShader::glAttachShader, fragmentShader");
		//step 5: link the program
		glLinkProgram(programObject);
		//SbmShaderProgram::printOglError("esLoadShader::glLinkProgram");

		//step 6: check linking result
		glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
		if(!linked)
		{
			GLint infoLen = 0;
			//SmartBody::util::log("program %d error", programObject);
			glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

			if(infoLen > 0)
			{
				char *infoLog = (char*)malloc(sizeof(char) * infoLen);

				glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
				//SmartBody::util::log("Error linking program:\n%s\n", infoLog);
				free(infoLog);

			}

			glDeleteProgram(programObject);
			return 0;
		}
		else
		{
			//SmartBody::util::log("program %d is linked correctly", programObject);
		}

		//setp 7: free shader resources
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return programObject;
	}


	void SHADER_API shaderInit(ESContext *esContext){

		//query number of vertex uniform vector support


		//load Vertex and Fragment Shader and link them into program object
// 		esContext->userData = malloc(sizeof(UserData)); 
// 		esContext->shapeData = malloc(sizeof(ShapeData)); 
// 		esContext->backgroundData = malloc(sizeof(BackgroundData));

		esContext->userData = new UserData();
		esContext->shapeData = new ShapeData();
		esContext->backgroundData = new BackgroundData();
		
		UserData *userData = (UserData*)esContext->userData;


#define GPU_SKINNING 0
#if GPU_SKINNING 
//         glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVUniforms);
// 		SmartBody::util::log("Web browser supports %d number of Vertex uniform vectors", maxVUniforms);
// 		if(maxVUniforms >= MAX_VERTEX_UNIFORM_1024)
// 			userData->programObject = esLoadProgram(vShaderStr, fShaderStr);
// 		else
// 			userData->programObject = esLoadProgram(vShaderLimitedStr, fShaderStr);
		
#else
        maxVUniforms = 2048; // fake the maxVUniform for non-GPU skinning
		//userData->programObject = esLoadProgram(vShaderStaticMeshStr, fShaderStr);
        userData->programObject = esLoadProgram(vShaderNormalMapStr, fShaderNormalMapStr);
        //userData->programObject = esLoadProgram(vShaderNormalMapStr, fShaderStr);
		//SbmShaderProgram::printOglError("shaderInit #0 ");
		SbmShaderManager& shaderManager = SbmShaderManager::singleton();
		shaderManager.addShader("skinning_shader", vShaderSkinStr, fShaderSkinStr, false);
		skinningShader = shaderManager.getShader("skinning_shader");
		skinningShader->buildShader();
		//SbmShaderProgram::printOglError("shaderInit #1 ");		
#endif
		
		ShapeData *shapeData = (ShapeData*)esContext->shapeData;
		shapeData->programObject = esLoadProgram(vShaderStrSphere, fShaderStrSphere);
		
		BackgroundData* backData = (BackgroundData*)esContext->backgroundData;
		backData->programObject = esLoadProgram(vShaderBackgroundStr, fShaderBackgroundStr);
		
		glUseProgram( backData->programObject );
		backData->positionLoc = glGetAttribLocation(backData->programObject, "aPosition");
		backData->texCoordLoc = glGetAttribLocation(backData->programObject, "aTexCoord");
		backData->mvpLoc	  = glGetUniformLocation(backData->programObject, "uMVPMatrix");
		backData->samplerLoc  = glGetUniformLocation(backData->programObject, "sTexture");
		
		glUseProgram ( userData->programObject );
		//mesh part
		userData->positionLoc = glGetAttribLocation(userData->programObject, "aPosition");
		userData->normalLoc   = glGetAttribLocation(userData->programObject, "aNormal");
        userData->tangentLoc   = glGetAttribLocation(userData->programObject, "aTangent");
		userData->texCoordLoc = glGetAttribLocation(userData->programObject, "aTexCoord");
		userData->mvpLoc	  = glGetUniformLocation(userData->programObject, "uMVPMatrix");
		userData->mvLoc		  = glGetUniformLocation(userData->programObject, "uMVMatrix");
		userData->samplerLoc  = glGetUniformLocation(userData->programObject, "sTexture");
        userData->normalMapLoc = glGetUniformLocation(userData->programObject, "normalTexture");
        userData->specularMapLoc = glGetUniformLocation(userData->programObject, "specularTexture");
#if GPU_SKINNING
		userData->boneIDLoc   = glGetAttribLocation(userData->programObject, "BoneID1");
		userData->boneWeightLoc = glGetAttribLocation(userData->programObject, "BoneWeight1");
#endif
		userData->blendTextureLightLoc = glGetAttribLocation(userData->programObject, "blendTextureWithLight");

		if(maxVUniforms >= MAX_VERTEX_UNIFORM_1024){
			//material
			userData->mtrlAmbientLoc = glGetUniformLocation(userData->programObject, "uMtrl.ambient");
			userData->mtrlDiffuseLoc = glGetUniformLocation(userData->programObject, "uMtrl.diffuse");
			userData->mtrlShininessLoc = glGetUniformLocation(userData->programObject, "uMtrl.shininess");
			glUniform4f(userData->mtrlAmbientLoc, 0.2, 0.2, 0.2, 1.0);
			glUniform4f(userData->mtrlDiffuseLoc, 0.8, 0.8, 0.8, 1.0);
			glUniform1f(userData->mtrlShininessLoc, 4.0);
			
		}
#if GPU_SKINNING
		userData->QuaternionLoc	  = glGetUniformLocation(userData->programObject, "uQuaternion");
		userData->TranslationLoc  = glGetUniformLocation(userData->programObject, "uTranslation");
#endif
		userData->meshScaleLoc = glGetUniformLocation(userData->programObject, "uMeshScale");

		//generate buffers
		glGenBuffers(1, &userData->meshPosObject);
		glGenBuffers(1, &userData->meshNormalObject);
        glGenBuffers(1, &userData->meshTangentObject);
		glGenBuffers(1, &userData->meshTexCoordObject);
		glGenBuffers(1, &userData->subMeshTriObject);
		glGenBuffers(1, &userData->boneWeightObject);
		glGenBuffers(1, &userData->boneIdObject);


		// setup render target
		//SmartBody::util::log("Generate FBO object");
		SbmTextureManager& texManager = SbmTextureManager::singleton();
   		std::string fboName = "renderFBO";
   		std::string fboTexName = "fboTex";
      
      	SbmTexture* fboTex = texManager.createTexture(SbmTextureManager::TEXTURE_RENDER_TARGET, fboTexName.c_str());
		fboTex->createEmptyTexture(esContext->width, esContext->height, 4, GL_FLOAT);
		fboTex->buildTexture(false, true);
   		esContext->fboID  = texManager.createFBO(fboName.c_str());   
   		esContext->fboTexID = fboTex->getID();



   		glGenRenderbuffers(1, &esContext->fboDepthBuf);
   		glBindRenderbuffer(GL_RENDERBUFFER, esContext->fboDepthBuf);
   		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, esContext->width, esContext->height);

   		//SmartBody::util::log("esContext->width = %d, esContext->height = %d", esContext->width, esContext->height);
   		//SmartBody::util::log("fboID = %d, fboDepth = %d, fboTexID = %d", esContext->fboID, esContext->fboDepthBuf, esContext->fboTexID);

   		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, esContext->fboID);
   		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, esContext->fboDepthBuf);
   		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex->getID(), 0);
   		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


		//sphere part
		glUseProgram ( shapeData->programObject );
		shapeData->positionLoc = glGetAttribLocation(shapeData->programObject, "aPosition");
		shapeData->normalLoc   = glGetAttribLocation(shapeData->programObject, "aNormal");
		shapeData->colorLoc    = glGetUniformLocation(shapeData->programObject, "uColor");
		shapeData->mvpLoc	   = glGetUniformLocation(shapeData->programObject, "uMVPMatrix");
		shapeData->mvLoc	   = glGetUniformLocation(shapeData->programObject, "uMVMatrix");
		shapeData->lightEnabledLoc = glGetUniformLocation(shapeData->programObject, "uLightEnabled");

		glGenBuffers(1, &shapeData->posObject);
		glGenBuffers(1, &shapeData->indexObject);
		glGenBuffers(1, &shapeData->normalObject);
		glGenBuffers(1, &shapeData->jointPosObject);
		glGenBuffers(1, &shapeData->boneIdxObject);


		const std::vector<std::string>& pawnNames = SmartBody::SBScene::getScene()->getPawnNames();
		for (unsigned int i = 0; i < pawnNames.size(); i++)
		{
			SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnNames[i]);
			DeformableMeshInstance* meshInstance = pawn->getActiveMesh();
      SmartBody::util::log("creating mesh buffer for pawn = %s", pawn->getName().c_str());
			if (meshInstance)
			{
				if (!meshInstance->isStaticMesh())
				{
					SbmDeformableMeshGPUInstance* gpuMeshInstance = dynamic_cast<SbmDeformableMeshGPUInstance*>(meshInstance);
					SbmDeformableMeshGPU* gpuMesh = (SbmDeformableMeshGPU*)gpuMeshInstance->getDeformableMesh();
					if (gpuMesh && gpuMeshInstance)
					{
            SmartBody::util::log("pawn = %s, rebuildVertexBufferGPU", pawn->getName().c_str());
						gpuMesh->rebuildVertexBufferGPU(true);
						gpuMeshInstance->initBuffer();
					}
					//GPUMeshUpdate(meshInstance);
				}
			}
		}

		//glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
		glClearColor ( 0.6f, 0.6f, 0.6f, 1.0f );

	}

	
	void SHADER_API drawLights(ESContext *esContext)
	{
		//test for drawing a directional light
		UserData *userData = (UserData*)esContext->userData;
		ShapeData *shapeData = (ShapeData*)esContext->shapeData;

		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
		SrCamera& cam = *scene->getActiveCamera();
		SrVec eye = cam.getEye();

		const int numOfLights = 4;
		GLint loc;
		string str;

#if 1
		_lights.clear();
		int numLightsInScene = 0;
		const std::vector<std::string>& pawnNames =  SmartBody::SBScene::getScene()->getPawnNames();
		SrLight light;
		for (std::vector<std::string>::const_iterator iter = pawnNames.begin();
			iter != pawnNames.end();
			iter++)
		{
			SmartBody::SBPawn* sbpawn = SmartBody::SBScene::getScene()->getPawn(*iter);
			const std::string& name = sbpawn->getName();
			if (name.find("light") == 0)
			{
				numLightsInScene++;
				SmartBody::BoolAttribute* enabledAttr = dynamic_cast<SmartBody::BoolAttribute*>(sbpawn->getAttribute("enabled"));
				if (enabledAttr && !enabledAttr->getValue())
				{
					continue;
				}
				//position
				light.position = sbpawn->getPosition();
				SrQuat orientation = sbpawn->getOrientation();
				if (light.directional)
				{
					light.position = -SrVec(0, 1, 0) * orientation;
				}			
				//diffuse
				SmartBody::Vec3Attribute* diffuseColorAttr = dynamic_cast<SmartBody::Vec3Attribute*>(sbpawn->getAttribute("lightDiffuseColor"));
				if (diffuseColorAttr)
				{
					const SrVec& color = diffuseColorAttr->getValue();
					light.diffuse = SrColor( color.x, color.y, color.z );
				}
				else
				{
					light.diffuse = SrColor( 1.0f, 0.95f, 0.8f );
				}
				//ambient
				SmartBody::Vec3Attribute* ambientColorAttr = dynamic_cast<SmartBody::Vec3Attribute*>(sbpawn->getAttribute("lightAmbientColor"));
				if (ambientColorAttr)
				{
					const SrVec& color = ambientColorAttr->getValue();
					light.ambient = SrColor( color.x, color.y, color.z );
				}
				else
				{
					light.ambient = SrColor( 0.0f, 0.0f, 0.0f );
				}
				//specular
				SmartBody::Vec3Attribute* specularColorAttr = dynamic_cast<SmartBody::Vec3Attribute*>(sbpawn->getAttribute("lightSpecularColor"));
				if (specularColorAttr)
				{
					const SrVec& color = specularColorAttr->getValue();
					light.specular = SrColor( color.x, color.y, color.z );
				}
				else
				{
					light.specular = SrColor( 0.0f, 0.0f, 0.0f );
				}

				_lights.push_back(light);
			}
		}

		if (_lights.size() == 0 && numLightsInScene == 0)
		{
			SrLight light;		
			light.diffuse = SrColor( 1.0f, 1.0f, 1.0f );
			SrMat mat;
			sr_euler_mat_xyz (mat, SR_TORAD(0), SR_TORAD(0), SR_TORAD(135	));
			SrQuat orientation(mat);
			SrVec up(0,1,0);
			SrVec lightDirection = -up * orientation;
			light.position = SrVec( lightDirection.x, lightDirection.y, lightDirection.z);
			_lights.push_back(light);

			SrLight light2 = light;
			light2.diffuse = SrColor( 0.8f, 0.8f, 0.8f );
			sr_euler_mat_xyz (mat, SR_TORAD(0), SR_TORAD(0), SR_TORAD(-135));
			SrQuat orientation2(mat);
			lightDirection = -up * orientation2;
			light2.position = SrVec( lightDirection.x, lightDirection.y, lightDirection.z);
			_lights.push_back(light2);
		}
		//pass lights to mesh shader 
		glUseProgram ( userData->programObject );
		if(maxVUniforms >= MAX_VERTEX_UNIFORM_1024){
			for(int i = 0; i < _lights.size(); ++i){
				light = _lights[i];
				str = "uLights[" + boost::lexical_cast<std::string>(i) +  "].position";
				loc = glGetUniformLocation(userData->programObject, str.c_str());
				glUniform4f(loc, light.position.x, light.position.y, light.position.z, 0.0);			
				str = "uLights[" + boost::lexical_cast<std::string>(i) +  "].diffuse";
				loc = glGetUniformLocation(userData->programObject, str.c_str());
				glUniform4f(loc, (float)light.diffuse.r / 255.0f, (float)light.diffuse.g / 255.0f, (float)light.diffuse.b / 255.0f, 1.0f);
				str = "uLights[" + boost::lexical_cast<std::string>(i) +  "].ambient";
				loc = glGetUniformLocation(userData->programObject, str.c_str());
				glUniform4f(loc, (float)light.ambient.r / 255.0f, (float)light.ambient.g / 255.0f, (float)light.ambient.b / 255.0f, 1.0f);
			}
		}else{
			light = _lights[0];
			str = "uLights[" + boost::lexical_cast<std::string>(0) +  "].position";
			loc = glGetUniformLocation(userData->programObject, str.c_str());
			glUniform4f(loc, light.position.x, light.position.y, light.position.z, 0.0);			
			str = "uLights[" + boost::lexical_cast<std::string>(0) +  "].diffuse";
			loc = glGetUniformLocation(userData->programObject, str.c_str());
			glUniform4f(loc, (float)light.diffuse.r / 255.0f, (float)light.diffuse.g / 255.0f, (float)light.diffuse.b / 255.0f, 1.0f);

			light = _lights[1];
			str = "uLights[" + boost::lexical_cast<std::string>(1) +  "].position";
			loc = glGetUniformLocation(userData->programObject, str.c_str());
			glUniform4f(loc, light.position.x, light.position.y, light.position.z, 0.0);			
			str = "uLights[" + boost::lexical_cast<std::string>(1) +  "].diffuse";
			loc = glGetUniformLocation(userData->programObject, str.c_str());
			glUniform4f(loc, (float)light.diffuse.r / 255.0f, (float)light.diffuse.g / 255.0f, (float)light.diffuse.b / 255.0f, 1.0f);
		}
		
		glUseProgram ( shapeData->programObject );
		//pass lights to shape shader
		for(int i = 0; i < _lights.size(); ++i){
			light = _lights[i];
			str = "uLights[" + boost::lexical_cast<std::string>(i) +  "].position";
			loc = glGetUniformLocation(shapeData->programObject, str.c_str());
			glUniform4f(loc, light.position.x, light.position.y, light.position.z, 0.0);			
			str = "uLights[" + boost::lexical_cast<std::string>(i) +  "].diffuse";
			loc = glGetUniformLocation(shapeData->programObject, str.c_str());
			glUniform4f(loc, (float)light.diffuse.r / 255.0f, (float)light.diffuse.g / 255.0f, (float)light.diffuse.b / 255.0f, 1.0f);
			
			str = "uLights[" + boost::lexical_cast<std::string>(i) +  "].ambient";
			loc = glGetUniformLocation(shapeData->programObject, str.c_str());
			glUniform4f(loc, (float)light.ambient.r / 255.0f, (float)light.ambient.g / 255.0f, (float)light.ambient.b / 255.0f, 1.0f);
			
		}
#endif
	}
	
	void SHADER_API drawBackground(std::string backgroundName, ESContext *esContext)
	{
		//SmartBody::util::log("Shader drawBackground");
		SbmTexture* tex = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE,backgroundName.c_str());
		if (!tex)
		{
			//SmartBody::util::log("drawBackground : cannot find texture image .....");
			return; // no background image
		}
		
		// figure out the texture coordinate scale and offset
		float texW = tex->getWidth();
		float texH = tex->getHeight();
		float surfaceW = esContext->width;
		float surfaceH = esContext->height;
		SrVec2 texScale = SrVec2(1, 1), texOffset = SrVec2(0, 0);
		float texAspect = texW / texH;
		float surfaceAspect = surfaceW / surfaceH;
		if (texAspect > surfaceAspect)
		{
			texScale.x = surfaceAspect / texAspect; // this must be < 1
			texOffset.x = (1.0 - texScale.x)*0.5f;
		}
		else
		{
			texScale.y = texAspect/surfaceAspect; // this must be < 1
			texOffset.y = (1.0 - texScale.y)*0.5f;
		}

				
		drawBackgroundTexID(tex->getID(),texScale, texOffset, esContext);
	}

	void SHADER_API drawBackgroundTexID(GLuint backgroundTexID, SrVec2& texScale, SrVec2& texOffset, ESContext *esContext)
	{
		BackgroundData *backData = (BackgroundData*)esContext->backgroundData;
		glUseProgram ( backData->programObject );
		
		glm::mat4 projMat = glm::ortho<float>(-1,1,-1,1);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, backgroundTexID);
		glUniform1i (backData->samplerLoc, 0);
		//SmartBody::util::log("drawBackgroundTexID, after glBindTexture");
		float z_max = -(1.f - 0.0001f);//gwiz::epsilon8());
		SrVec4 quad[4] = { SrVec4(-1.0, 1.0f, z_max, 1.f), SrVec4(-1.0f, -1.0f, z_max, 1.f), SrVec4(1.0f, -1.0f, z_max, 1.f), SrVec4(1.0f, 1.0f, z_max, 1.f) };
		SrVec2 quadT[4] = { SrVec2(0.f, 1.f), SrVec2(0.f, 0.f), SrVec2(1.f, 0.f), SrVec2(1.f, 1.f) };
		for (int i = 0; i < 4; i++)
		{
			quadT[i].x = quadT[i].x*texScale.x + texOffset.x;
			quadT[i].y = quadT[i].y*texScale.y + texOffset.y;
		}

		unsigned short indices[] = {0,1,2, 0,2,3};

		GLuint pos_loc = backData->positionLoc;
		GLuint texcoord_loc = backData->texCoordLoc;
		GLuint mvp_loc = backData->mvpLoc;
		glEnableVertexAttribArray(pos_loc);
		glVertexAttribPointer(pos_loc,4,GL_FLOAT,0,0,quad[0].data());
		glEnableVertexAttribArray(texcoord_loc);
		glVertexAttribPointer(texcoord_loc,2,GL_FLOAT,0,0,quadT[0].data());
		//wes_matrix_mvp();
		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(projMat));
		//SmartBody::util::log("drawBackgroundTexID, before glDrawElements");
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
		//SmartBody::util::log("drawBackgroundTexID, after glDrawElements");
		glDisableVertexAttribArray(pos_loc);
		glDisableVertexAttribArray(texcoord_loc);
		
		glUseProgram(0);
		//SmartBody::util::log("drawBackgroundTexID, after glUseProgram(0)");
	}

	void SHADER_API testTransformFeedback()
	{

	}

	void SHADER_API GPUMeshUpdate(DeformableMeshInstance* meshInstance)
	{
		SbmDeformableMeshGPUInstance* gpuMeshInstance = (SbmDeformableMeshGPUInstance*)meshInstance;
		SbmDeformableMeshGPU* gpuMesh = (SbmDeformableMeshGPU*)gpuMeshInstance->getDeformableMesh();

		//SmartBody::util::log("gpuMeshInstance = %x, gpuMesh = %x, VBODeformPos = %x", gpuMeshInstance, gpuMesh, gpuMeshInstance->getVBODeformPos());
		if (!gpuMeshInstance->getVBODeformPos())
		{
			SbmShaderProgram::printOglError("GPUMeshUpdate #0 ");
			//SmartBody::util::log("gpuMesh doesn't have VBODeformPos, initBuffer");
			gpuMeshInstance->initBuffer();
			SbmShaderProgram::printOglError("GPUMeshUpdate #0.5 ");
		}

		// update blendshapes
		gpuMeshInstance->gpuBlendShape();

		static GLuint queryName = -1;
		if (queryName == -1)
			glGenQueries(1, &queryName);
		// setup transform feedback
		const char* attr[3] = { "deformPos", "deformNormal", "deformTangent" };

		SbmShaderManager& shaderManager = SbmShaderManager::singleton();
		skinningShader = shaderManager.getShader("skinning_shader");
		//SmartBody::util::log("Before start transform feedback");
		glTransformFeedbackVaryings(skinningShader->getShaderProgram(), 3, attr, GL_SEPARATE_ATTRIBS);
		glLinkProgram(skinningShader->getShaderProgram());
		SbmShaderProgram::printOglError("GPUMeshUpdate #1 ");
		//SmartBody::util::log("After skinningShader glLinkProgram");

		glEnableVertexAttribArray(0);
		//SmartBody::util::log("Before bind PosVBO, PosVBO = %x", gpuMesh->getPosVBO());
		//SmartBody::util::log("VBO = %x, VBOID = %d", gpuMesh->getPosVBO()->VBO(), gpuMesh->getPosVBO()->VBO()->m_iVBO_ID);;
		gpuMesh->getPosVBO()->VBO()->BindBuffer();
		SbmShaderProgram::printOglError("GPUMeshUpdate #2 ");
		//SmartBody::util::log("After bind PosVBO");
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		gpuMesh->getNormalVBO()->VBO()->BindBuffer();
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);
		gpuMesh->getTangentVBO()->VBO()->BindBuffer();
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);
		gpuMesh->getBoneIDVBO()->VBO()->BindBuffer();
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(4);
		gpuMesh->getBoneWeightVBO()->VBO()->BindBuffer();
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
		SbmShaderProgram::printOglError("GPUMeshUpdate #3 ");

		//SmartBody::util::log("After setup glVertexAttribPointer");
		glEnable(GL_RASTERIZER_DISCARD);
		SbmShaderProgram::printOglError("GPUMeshUpdate #3.0 ");
		//gpuMeshInstance->updateTransformBuffer();
		//SmartBody::util::log("After update transform buffer");
		std::vector<SrMat>& transBuffer = gpuMeshInstance->getTransformBuffer();
		//float meshScale = 
		SrVec meshScale = meshInstance->getMeshScale();
		//SmartBody::util::log("Before glUseProgram, program = %d", skinningShader->getShaderProgram());		
		glUseProgram(skinningShader->getShaderProgram());
		SbmShaderProgram::printOglError("GPUMeshUpdate #3.1 ");
		//SmartBody::util::log("Before set glUniformMatrix4fv");
		glUniformMatrix4fv(glGetUniformLocation(skinningShader->getShaderProgram(), "Transform"), transBuffer.size(), true, (GLfloat*)getPtr(transBuffer));
		SbmShaderProgram::printOglError("GPUMeshUpdate #3.2 ");
		glUniform1f(glGetUniformLocation(skinningShader->getShaderProgram(), "meshScale"), meshScale[0]);
		SbmShaderProgram::printOglError("GPUMeshUpdate #3.3 ");
		//SmartBody::util::log("After set glUniform");
		// bind transform feedback buffer
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, gpuMeshInstance->getVBODeformPos()->VBO()->m_iVBO_ID);
		SbmShaderProgram::printOglError("GPUMeshUpdate #4 ");
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, gpuMeshInstance->getVBODeformNormal()->VBO()->m_iVBO_ID);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, gpuMeshInstance->getVBODeformTangent()->VBO()->m_iVBO_ID);
		//SmartBody::util::log("After glBindBufferBase");

		//glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, queryName);
		glBeginTransformFeedback(GL_POINTS);
		SbmShaderProgram::printOglError("GPUMeshUpdate #5 ");
		//SmartBody::util::log("before glDrawArrays");
		glDrawArrays(GL_POINTS, 0, gpuMeshInstance->_deformPosBuf.size());
		//glDrawArrays(GL_POINTS, 0, 100);
		SbmShaderProgram::printOglError("GPUMeshUpdate #6 ");
		//SmartBody::util::log("After glDrawArrays");
		glEndTransformFeedback();
		SbmShaderProgram::printOglError("GPUMeshUpdate #7 ");
		//SmartBody::util::log("After glEndTrasnformFeedback");
		//glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);	
		//SbmShaderProgram::printOglError("GPUMeshUpdate #8 ");

		GLuint PrimitivesWritten = 100;
		//glGetQueryObjectuiv(queryName, GL_QUERY_RESULT, &PrimitivesWritten);
		SbmShaderProgram::printOglError("GPUMeshUpdate #9 ");
		//SmartBody::util::log("Query name = %d, Output transform feedback = %d", queryName, PrimitivesWritten);

		glUseProgram(0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glDisable(GL_RASTERIZER_DISCARD);
	}

	void SHADER_API drawMesh(DeformableMeshInstance *shape, ESContext *esContext, bool showSkinWeight)
	{
		UserData *userData = (UserData*)esContext->userData;
		SmartBody::SBScene *scene = SmartBody::SBScene::getScene();
		//SmartBody::util::log("Render Deformable Model");
		DeformableMesh *mesh = shape->getDeformableMesh();
		std::vector<SrVec4>          QuaternionBuf;
		std::vector<SrVec4>			 TranslationBuf;
		if(!mesh){
			//printf("no deformable mesh found!\n");
			return;
		}
		//get submesh
		std::vector<SbmSubMesh*>& subMeshList = mesh->subMeshList;

		glEnable(GL_CULL_FACE);
		if(scene->getBoolAttribute("enableAlphaBlend")){
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		}
		//glEnable( GL_MULTISAMPLE );
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		//SmartBody::util::log("meshScale = %f", shape->getMeshScale());
		glUniform1f(userData->meshScaleLoc, shape->getMeshScale()[0]);
		
		if (shape->_deformPosBuf.size() > 0)
		{
			glEnableVertexAttribArray(userData->positionLoc);
			glBindBuffer(GL_ARRAY_BUFFER, userData->meshPosObject);
			glBufferData(GL_ARRAY_BUFFER, shape->_deformPosBuf.size() * sizeof(GLfloat) * 3, shape->_deformPosBuf[0].data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(userData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
			//glDrawArrays(GL_POINTS, 0, shape->_deformPosBuf.size());
		}
		
		if (mesh->normalBuf.size () > 0)
		{
			glEnableVertexAttribArray(userData->normalLoc);
			glBindBuffer(GL_ARRAY_BUFFER, userData->meshNormalObject);
			glBufferData(GL_ARRAY_BUFFER, mesh->normalBuf.size () * sizeof(GLfloat) * 3, mesh->normalBuf[0].data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(userData->normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		//load the texture coordinates
		if (mesh->texCoordBuf.size() > 0)
		{
			glEnableVertexAttribArray(userData->texCoordLoc);
			glBindBuffer(GL_ARRAY_BUFFER, userData->meshTexCoordObject);
			glBufferData(GL_ARRAY_BUFFER, mesh->texCoordBuf.size () * sizeof(GLfloat) * 2, mesh->texCoordBuf[0].data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(userData->texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}
		//load transform
		if(shape->transformBuffer.size() > 0){
			
			//convert transform from 4x4 matrix to QT(Quaternion + Translation) representation
			for(size_t i = 0; i < shape->transformBuffer.size(); ++i){
				SrQuat temp = SrQuat(shape->transformBuffer[i].get_rotation());
				temp.normalize();
				QuaternionBuf.push_back(SrVec4(temp.getData(1), temp.getData(2), temp.getData(3), temp.getData(0)));
				SrVec tempT = SrVec(shape->transformBuffer[i].get_translation());
				TranslationBuf.push_back(SrVec4(tempT.getData(0), tempT.getData(1), tempT.getData(2), 0.0));
			}
			//submit QT to the Vertex shader
      // todo: this smells.
			glUniform4fv(userData->QuaternionLoc, QuaternionBuf.size(), QuaternionBuf[0].data());
			glUniform4fv(userData->TranslationLoc, TranslationBuf.size(), TranslationBuf[0].data());
				
			QuaternionBuf.clear();
			TranslationBuf.clear();
			
		}
		
		//load weight
		if(mesh->boneWeightBuf[0].size() > 0){
			glEnableVertexAttribArray(userData->boneWeightLoc);
			glBindBuffer(GL_ARRAY_BUFFER, userData->boneWeightObject);
			glBufferData(GL_ARRAY_BUFFER, mesh->boneWeightBuf[0].size() * sizeof(GLfloat) * 4, mesh->boneWeightBuf[0][0].data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(userData->boneWeightLoc,4,GL_FLOAT,0,0,0);
		}
		//load boneId
		if(mesh->boneIDBuf_f[0].size() > 0){
			glEnableVertexAttribArray(userData->boneIDLoc);
			glBindBuffer(GL_ARRAY_BUFFER, userData->boneIdObject);
			glBufferData(GL_ARRAY_BUFFER, mesh->boneIDBuf_f[0].size() * sizeof(GLfloat) * 4, mesh->boneIDBuf_f[0][0].data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(userData->boneIDLoc,4,GL_FLOAT,0,0,0);
		}

		for(unsigned int i = 0; i < subMeshList.size(); i++)
		{
			SbmSubMesh *subMesh = subMeshList[i];
			std::string texturesType = "static";
			if (shape->getCharacter())
				texturesType = shape->getCharacter()->getStringAttribute("texturesType");	
			//printf("textureType = %s", texturesType.c_str());

			
			if( texturesType == "static" || texturesType == "dynamic")
			{
				SbmTexture* tex = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE, subMesh->texName.c_str());		
				//printf("tex = %d", tex);
				if (tex && !showSkinWeight)
				{
					GLint activeTexture = -1;
					glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);

					if (activeTexture != GL_TEXTURE0)
						glActiveTexture(GL_TEXTURE0);

					//	If we are using blended textures
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR); 

					if(!shape->getCharacter())
					{
						glBindTexture(GL_TEXTURE_2D, tex->getID());					
					} 
					else if (texturesType == "dynamic")
					{
						if(shape->_tempTexPairs != NULL)
						{
							glBindTexture(GL_TEXTURE_2D, shape->_tempTexPairs[0]);
							//std::cerr << "Using tex: " << shape->_tempTexPairs[0] << "\n";
							//SmartBody::util::log("Use Blended texture");
						}
						else 
						{
							//SmartBody::util::log("*** WARNING: Blended texture shape->_tempTex not initialized. Using tex->getID() instead.");
							glBindTexture(GL_TEXTURE_2D, tex->getID());
						}

					}
					else 		//	If blended textures not used, use neutral appearance				
					{
						glBindTexture(GL_TEXTURE_2D, tex->getID());
						//printf("Use original texture, texID = %d", tex->getID());
					}	
					glUniform1i (userData->samplerLoc, 0);
				}
			}

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->subMeshTriObject);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, subMesh->triBuf.size() * sizeof(GLushort) * 3, subMesh->triBuf[0].unsignedShortData(), GL_DYNAMIC_DRAW);
			glDrawElements(GL_TRIANGLES, (GLsizei)(subMesh->triBuf.size() * 3), GL_UNSIGNED_SHORT, 0);
		}
		if(scene->getBoolAttribute("enableAlphaBlend")){
			glDisable(GL_BLEND);
		}
		glDisable(GL_CULL_FACE);
		//unbind buffers
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


	void SHADER_API drawMeshStaticVBO(DeformableMeshInstance *shape, ESContext *esContext, bool showSkinWeight)
	{
		UserData *userData = (UserData*)esContext->userData;
		glUseProgram(userData->programObject);
		SmartBody::SBScene *scene = SmartBody::SBScene::getScene();
		//SmartBody::util::log("Render Deformable Model");
		DeformableMesh *mesh = shape->getDeformableMesh();
		if (!mesh) {
			//printf("no deformable mesh found!\n");
			return;
		}

		SbmDeformableMeshGPUInstance* gpuMeshInstance = (SbmDeformableMeshGPUInstance*)shape;
		if (!gpuMeshInstance->getVBODeformPos())
			gpuMeshInstance->initBuffer();
		SbmDeformableMeshGPU* gpuMesh = (SbmDeformableMeshGPU*)gpuMeshInstance->getDeformableMesh();

		std::vector<SrVec4>          QuaternionBuf;
		std::vector<SrVec4>			 TranslationBuf;
		
		//get submesh
		std::vector<SbmSubMesh*>& subMeshList = mesh->subMeshList;

		glEnable(GL_CULL_FACE);
		if (scene->getBoolAttribute("enableAlphaBlend")) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		if (scene->getBoolAttribute("blendTexturesWithLighting"))
		{
			glUniform1i(userData->blendTextureLightLoc, 1);
		}
		else
		{
			glUniform1i(userData->blendTextureLightLoc, 0);
		}
		//SmartBody::util::log("meshScale = %f", shape->getMeshScale());
		glUniform1f(userData->meshScaleLoc, shape->getMeshScale()[0]);

		if (shape->_deformPosBuf.size() > 0)
		{
			glEnableVertexAttribArray(userData->positionLoc);
			gpuMeshInstance->getVBODeformPos()->VBO()->BindBuffer();
			//glBindBuffer(GL_ARRAY_BUFFER, userData->meshPosObject);
			//glBufferData(GL_ARRAY_BUFFER, shape->_deformPosBuf.size() * sizeof(GLfloat) * 3, shape->_deformPosBuf[0].data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(userData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
			//glDrawArrays(GL_POINTS, 0, shape->_deformPosBuf.size());
		}

		if (mesh->normalBuf.size() > 0)
		{
			glEnableVertexAttribArray(userData->normalLoc);
			gpuMeshInstance->getVBODeformNormal()->VBO()->BindBuffer();
			//glBindBuffer(GL_ARRAY_BUFFER, userData->meshNormalObject);
			//glBufferData(GL_ARRAY_BUFFER, mesh->normalBuf.size() * sizeof(GLfloat) * 3, mesh->normalBuf[0].data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(userData->normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}


		if (mesh->tangentBuf.size() > 0)
		{
			glEnableVertexAttribArray(userData->tangentLoc);
			gpuMeshInstance->getVBODeformTangent()->VBO()->BindBuffer();
			//glBindBuffer(GL_ARRAY_BUFFER, userData->meshTangentObject);
			//glBufferData(GL_ARRAY_BUFFER, mesh->tangentBuf.size() * sizeof(GLfloat) * 3, mesh->tangentBuf[0].data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(userData->tangentLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		//load the texture coordinates
		if (mesh->texCoordBuf.size() > 0)
		{
			glEnableVertexAttribArray(userData->texCoordLoc);
			gpuMesh->getTexCoordVBO()->VBO()->BindBuffer();
			//glBindBuffer(GL_ARRAY_BUFFER, userData->meshTexCoordObject);
			//glBufferData(GL_ARRAY_BUFFER, mesh->texCoordBuf.size() * sizeof(GLfloat) * 2, mesh->texCoordBuf[0].data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(userData->texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}

		std::vector<VBOVec3i*>& subMeshTris = gpuMesh->getVBOSubMeshTris();
		for (unsigned int i = 0; i < subMeshList.size(); i++)
		{
			SbmSubMesh* subMesh = subMeshList[i];
			VBOVec3i* subMeshVBO = subMeshTris[i];
			bool blendShapeMesh = false;
			std::map<std::string, std::vector<SrSnModel*> > ::iterator mi = mesh->blendShapeMap.find(subMesh->modelName);
			std::map<std::string, std::vector<SrSnModel*> > ::iterator endIt = mesh->blendShapeMap.end();
			int hitCount = mesh->blendShapeMap.count(subMesh->modelName);
			if (mi != endIt)
				blendShapeMesh = true;

			if (subMesh->material.useAlphaBlend)
			{
				glDisable(GL_CULL_FACE);
			}
			else
			{
				glEnable(GL_CULL_FACE);
			}
			std::string texturesType = "static";
			if (shape->getCharacter())
				texturesType = shape->getCharacter()->getStringAttribute("texturesType");
			//printf("textureType = %s", texturesType.c_str());

			float color[4];
			float shininess = subMesh->material.shininess;
			subMesh->material.diffuse.get(color);
			glUniform4f(userData->mtrlDiffuseLoc, color[0], color[1], color[2], 1.0);
			glUniform1f(userData->mtrlShininessLoc, shininess);

			if (texturesType == "static" || texturesType == "dynamic")
			{
				SbmTexture* tex = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE, subMesh->texName.c_str());
				SbmTexture* normalMap = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_NORMALMAP, subMesh->normalMapName.c_str());
				SbmTexture* specularMap = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_SPECULARMAP, subMesh->specularMapName.c_str());
				//SmartBody::util::log("Render StaticMesh texName = %s, tex = %d", subMesh->texName.c_str(), tex);
				if (tex && !showSkinWeight)
				{
					//SmartBody::util::log("Render StaticMesh texID = %d", tex->getID());
					GLint activeTexture = -1;
					glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);

					if (activeTexture != GL_TEXTURE0)
						glActiveTexture(GL_TEXTURE0);

					//	If we are using blended textures
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


					if (!shape->getCharacter())
					{
						glBindTexture(GL_TEXTURE_2D, tex->getID());
					}
					else if (texturesType == "dynamic")
					{
						if (shape->_tempTexPairs != NULL && blendShapeMesh)
						{
							glBindTexture(GL_TEXTURE_2D, shape->_tempTexPairs[0]);
							//std::cerr << "Using tex: " << shape->_tempTexPairs[0] << "\n";
							//SmartBody::util::log("Use Blended texture");
						}
						else
						{
							//SmartBody::util::log("*** WARNING: Blended texture shape->_tempTex not initialized. Using tex->getID() instead.");
							glBindTexture(GL_TEXTURE_2D, tex->getID());
						}
					}
					else 		//	If blended textures not used, use neutral appearance
					{
						glBindTexture(GL_TEXTURE_2D, tex->getID());
						//SmartBody::util::log("Use original texture, texID = %d", tex->getID());
					}
					//SmartBody::util::log("sTexture uniform loc = %d", userData->samplerLoc);
					glUniform1i(userData->samplerLoc, 0);
				}
				if (normalMap)
				{
					GLint activeTexture = -1;
					glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);

					if (activeTexture != GL_TEXTURE1)
						glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, normalMap->getID());
					glUniform1i(userData->normalMapLoc, 1);
				}
				if (specularMap)
				{
					GLint activeTexture = -1;
					glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);

					if (activeTexture != GL_TEXTURE2)
						glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, specularMap->getID());
					glUniform1i(userData->specularMapLoc, 2);
				}
			}

			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->subMeshTriObject);
			//glBufferData(GL_ELEMENT_ARRAY_BUFFER, subMesh->triBuf.size() * sizeof(GLushort) * 3, subMesh->triBuf[0].unsignedShortData(), GL_DYNAMIC_DRAW);
			subMeshVBO->VBO()->BindBuffer();
			glDrawElements(GL_TRIANGLES, subMesh->triBuf.size() * 3, GL_UNSIGNED_SHORT, 0);
			subMeshVBO->VBO()->UnbindBuffer();
		}
		if (scene->getBoolAttribute("enableAlphaBlend")) {
			glDisable(GL_BLEND);
		}
		glDisable(GL_CULL_FACE);
		//unbind buffers
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}


	void SHADER_API drawMeshStatic(DeformableMeshInstance *shape, ESContext *esContext, bool showSkinWeight)
	{
		UserData *userData = (UserData*)esContext->userData;
		glUseProgram ( userData->programObject );
		SmartBody::SBScene *scene = SmartBody::SBScene::getScene();
		//SmartBody::util::log("Render Deformable Model");
		DeformableMesh *mesh = shape->getDeformableMesh();
		std::vector<SrVec4>          QuaternionBuf;
		std::vector<SrVec4>			 TranslationBuf;
		if(!mesh){
			//printf("no deformable mesh found!\n");
			return;
		}
		//get submesh
		std::vector<SbmSubMesh*>& subMeshList = mesh->subMeshList;

		glEnable(GL_CULL_FACE);
		if(scene->getBoolAttribute("enableAlphaBlend")){
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		if (scene->getBoolAttribute("blendTexturesWithLighting"))
		{
			glUniform1i(userData->blendTextureLightLoc, 1);
		}
		else
		{
			glUniform1i(userData->blendTextureLightLoc, 0);
		}
		//SmartBody::util::log("meshScale = %f", shape->getMeshScale());
		glUniform1f(userData->meshScaleLoc, shape->getMeshScale()[0]);

		if (shape->_deformPosBuf.size() > 0)
		{
			glEnableVertexAttribArray(userData->positionLoc);
			glBindBuffer(GL_ARRAY_BUFFER, userData->meshPosObject);
			glBufferData(GL_ARRAY_BUFFER, shape->_deformPosBuf.size() * sizeof(float) * 3, shape->_deformPosBuf[0].data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(userData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
			//glDrawArrays(GL_POINTS, 0, shape->_deformPosBuf.size());
		}

		if (mesh->normalBuf.size () > 0)
		{
			glEnableVertexAttribArray(userData->normalLoc);
			glBindBuffer(GL_ARRAY_BUFFER, userData->meshNormalObject);
			glBufferData(GL_ARRAY_BUFFER, mesh->normalBuf.size () * sizeof(float) * 3, mesh->normalBuf[0].data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(userData->normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}


        if (mesh->tangentBuf.size () > 0)
        {
            glEnableVertexAttribArray(userData->tangentLoc);
            glBindBuffer(GL_ARRAY_BUFFER, userData->meshTangentObject);
            glBufferData(GL_ARRAY_BUFFER, mesh->tangentBuf.size () * sizeof(float) * 3, mesh->tangentBuf[0].data(), GL_DYNAMIC_DRAW);
            glVertexAttribPointer(userData->tangentLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }

		//load the texture coordinates
		if (mesh->texCoordBuf.size() > 0)
		{
			glEnableVertexAttribArray(userData->texCoordLoc);
			glBindBuffer(GL_ARRAY_BUFFER, userData->meshTexCoordObject);
			glBufferData(GL_ARRAY_BUFFER, mesh->texCoordBuf.size () * sizeof(float) * 2, mesh->texCoordBuf[0].data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(userData->texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}

		for(unsigned int i = 0; i < subMeshList.size(); i++)
		{
			SbmSubMesh* subMesh = subMeshList[i];
			bool blendShapeMesh = false;
			std::map<std::string, std::vector<SrSnModel*> > ::iterator mi = mesh->blendShapeMap.find(subMesh->modelName);
			std::map<std::string, std::vector<SrSnModel*> > ::iterator endIt =  mesh->blendShapeMap.end();
			int hitCount = mesh->blendShapeMap.count(subMesh->modelName);
			if (mi != endIt)
				blendShapeMesh = true;
					
			if (subMesh->material.useAlphaBlend)
			{
				glDisable(GL_CULL_FACE);
			}
			else
			{
				glEnable(GL_CULL_FACE);
			}
			std::string texturesType = "static";
			if (shape->getCharacter())
				texturesType = shape->getCharacter()->getStringAttribute("texturesType");
			//printf("textureType = %s", texturesType.c_str());

			float color[4];
			float shininess = subMesh->material.shininess;
			subMesh->material.diffuse.get(color);	
			glUniform4f(userData->mtrlDiffuseLoc, color[0], color[1], color[2], 1.0);
			glUniform1f(userData->mtrlShininessLoc, shininess);

			if( texturesType == "static" || texturesType == "dynamic")
			{
				SbmTexture* tex = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE, subMesh->texName.c_str());
                SbmTexture* normalMap = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_NORMALMAP, subMesh->normalMapName.c_str());
                SbmTexture* specularMap = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_SPECULARMAP, subMesh->specularMapName.c_str());
				//SmartBody::util::log("Render StaticMesh texName = %s, tex = %d", subMesh->texName.c_str(), tex);
				if (tex && !showSkinWeight)
				{
					//SmartBody::util::log("Render StaticMesh texID = %d", tex->getID());
					GLint activeTexture = -1;
					glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);

					if (activeTexture != GL_TEXTURE0)
						glActiveTexture(GL_TEXTURE0);

					//	If we are using blended textures
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

					
					if(!shape->getCharacter())
					{
						glBindTexture(GL_TEXTURE_2D, tex->getID());
					}
					else if (texturesType == "dynamic")
					{
						if(shape->_tempTexPairs != NULL && blendShapeMesh)
						{
							glBindTexture(GL_TEXTURE_2D, shape->_tempTexPairs[0]);
							//std::cerr << "Using tex: " << shape->_tempTexPairs[0] << "\n";
							//SmartBody::util::log("Use Blended texture");
						}
						else
						{
							//SmartBody::util::log("*** WARNING: Blended texture shape->_tempTex not initialized. Using tex->getID() instead.");
							glBindTexture(GL_TEXTURE_2D, tex->getID());
						}
					}
					else 		//	If blended textures not used, use neutral appearance
					{
						glBindTexture(GL_TEXTURE_2D, tex->getID());
						//SmartBody::util::log("Use original texture, texID = %d", tex->getID());
					}
                    //SmartBody::util::log("sTexture uniform loc = %d", userData->samplerLoc);
					glUniform1i (userData->samplerLoc, 0);
				}
                if (normalMap)
                {
                    GLint activeTexture = -1;
                    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);

                    if (activeTexture != GL_TEXTURE1)
                        glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, normalMap->getID());
                    glUniform1i (userData->normalMapLoc, 1);
                }
                if (specularMap)
                {
                    GLint activeTexture = -1;
                    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);

                    if (activeTexture != GL_TEXTURE2)
                        glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, specularMap->getID());
                    glUniform1i (userData->specularMapLoc, 2);
                }
			}

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->subMeshTriObject);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, subMesh->triBuf.size() * sizeof(unsigned short) * 3, subMesh->triBuf[0].unsignedShortData(), GL_DYNAMIC_DRAW);
			glDrawElements(GL_TRIANGLES, (GLsizei)(subMesh->triBuf.size() * 3), GL_UNSIGNED_SHORT, 0);
		}
		if(scene->getBoolAttribute("enableAlphaBlend")){
			glDisable(GL_BLEND);
		}
		glDisable(GL_CULL_FACE);
		//unbind buffers
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUseProgram (0);
	}

	void SHADER_API drawSphere(ESContext *esContext, SrVec pos, float r, int p, SrVec color){
		ShapeData *shapeData = (ShapeData*)esContext->shapeData;
		float theta1 = 0.0, theta2 = 0.0, theta3 = 0.0;

		float ex = 0.0f, ey = 0.0f, ez = 0.0f;
		float px = 0.0f, py = 0.0f, pz = 0.0f;
		GLfloat *vertices = new GLfloat[p*6+6];
		GLfloat *normals = new GLfloat[p*6+6];

		if( r < 0 )
			r = -r;

		if( p < 0 )
			p = -p;

		for(int i = 0; i < p/2; ++i)
		{
			theta1 = i * (M_PI*2) / p - M_PI_2;
			theta2 = (i + 1) * (M_PI*2) / p - M_PI_2;

			for(int j = 0; j <= p; ++j)
			{
				theta3 = j * (M_PI*2) / p;

				ex = cosf(theta2) * cosf(theta3);
				ey = sinf(theta2);
				ez = cosf(theta2) * sinf(theta3);
				px = pos.x + r * ex;
				py = pos.y + r * ey;
				pz = pos.z + r * ez;

				vertices[(6*j)+(0%6)] = px;
				vertices[(6*j)+(1%6)] = py;
				vertices[(6*j)+(2%6)] = pz;

				normals[(6*j)+(0%6)] = ex;
				normals[(6*j)+(1%6)] = ey;
				normals[(6*j)+(2%6)] = ez;

				ex = cosf(theta1) * cosf(theta3);
				ey = sinf(theta1);
				ez = cosf(theta1) * sinf(theta3);
				px = pos.x + r * ex;
				py = pos.y + r * ey;
				pz = pos.z + r * ez;

				vertices[(6*j)+(3%6)] = px;
				vertices[(6*j)+(4%6)] = py;
				vertices[(6*j)+(5%6)] = pz;

				normals[(6*j)+(3%6)] = ex;
				normals[(6*j)+(4%6)] = ey;
				normals[(6*j)+(5%6)] = ez;
			}
			//bind position buffer
			glEnableVertexAttribArray(shapeData->positionLoc);
			glBindBuffer(GL_ARRAY_BUFFER, shapeData->posObject);
			glBufferData(GL_ARRAY_BUFFER,  (p*6+6) * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(shapeData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
			//bind normal buffer
			glEnableVertexAttribArray(shapeData->normalLoc);
			glBindBuffer(GL_ARRAY_BUFFER, shapeData->normalObject);
			glBufferData(GL_ARRAY_BUFFER,  (p*6+6) * sizeof(GLfloat), normals, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(shapeData->normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
			//pass uniform color
			glUniform4f(shapeData->colorLoc, 1.0, 0.0, 0.0, 1.0);
			glUniform1i(shapeData->lightEnabledLoc, 1);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, (p+1)*2);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(shapeData->positionLoc);
		glDisableVertexAttribArray(shapeData->normalLoc);

		delete [] vertices;
		delete [] normals;
	}

	void drawBox(ESContext *esContext, SrVec pos, SrVec extent, SrVec color){
		ShapeData *shapeData = (ShapeData*)esContext->shapeData;
		GLfloat vertices[] = {-extent.x + pos.x, -extent.y + pos.y,  extent.z + pos.z,
			extent.x + pos.x, -extent.y + pos.y,  extent.z + pos.z,  
			extent.x + pos.x,  extent.y + pos.y,  extent.z + pos.z, 
			-extent.x + pos.x,  extent.y + pos.y,  extent.z + pos.z,
			-extent.x + pos.x, -extent.y + pos.y, -extent.z + pos.z,  
			extent.x + pos.x, -extent.y + pos.y, -extent.z + pos.z,  
			extent.x + pos.x,  extent.y + pos.y, -extent.z + pos.z, 
			-extent.x + pos.x,  extent.y + pos.y, -extent.z + pos.z
		};

		GLfloat normals[]  = {-1.0, -1.0,  1.0,  1.0, -1.0,  1.0,  1.0,  1.0,  1.0, -1.0,  1.0,  1.0,
			-1.0, -1.0, -1.0,  1.0, -1.0, -1.0,  1.0,  1.0, -1.0, -1.0,  1.0, -1.0
		};

		GLuint indices[]  = {0, 1, 2, 2, 3, 0, 3, 2, 6, 6, 7, 3, 7, 6, 5, 5, 4, 7, 
			4, 0, 3, 3, 7, 4, 5, 1, 0, 0, 4, 5, 1, 5, 6, 6, 2, 1
		}; 

		//vertex position buffer
		glEnableVertexAttribArray(shapeData->positionLoc);
		glBindBuffer(GL_ARRAY_BUFFER, shapeData->posObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(shapeData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(shapeData->normalLoc);
		glBindBuffer(GL_ARRAY_BUFFER, shapeData->normalObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(shapeData->normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glUniform4f(shapeData->colorLoc, 1.0, 0.0, 0.0, 1.0);
		glUniform1i(shapeData->lightEnabledLoc, 1);
		//index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapeData->indexObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(shapeData->positionLoc);
		glDisableVertexAttribArray(shapeData->normalLoc);

	}
	void drawGrid(ESContext *esContext){
		ShapeData *shapeData = (ShapeData*)esContext->shapeData;

		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
		float sceneScale = scene->getScale();
		GLfloat gridHeight = 0.0f + 0.001f/scene->getScale();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		GLint loc;
		string str;

		glLineWidth(3.f);
		float gridStep = 20.0, gridSize = 400.0;
		float adjustedGridStep = gridStep;
		float adjustGridSize = gridSize * .01f / sceneScale;

		GLfloat vertices[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
		GLfloat normals[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

		if (sceneScale > 0.f)
		{
			adjustedGridStep *= .01f / sceneScale;
		}
		//pass uniform color
		glUniform4f(shapeData->colorLoc, 0.7, 0.7, 0.7, 1.0);
		glUniform1i(shapeData->lightEnabledLoc, 0);
		glEnableVertexAttribArray(shapeData->positionLoc);
		glBindBuffer(GL_ARRAY_BUFFER, shapeData->posObject);

		for (float x = -adjustGridSize; x <= adjustGridSize + .001; x += adjustedGridStep)
		{
			vertices[0] = x; vertices[1] = gridHeight; vertices[2] = -adjustGridSize;
			vertices[3] = x; vertices[4] = gridHeight; vertices[5] = adjustGridSize;
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(shapeData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glDrawArrays(GL_LINES, 0, 2);
		}
		for (float x = -adjustGridSize; x <= adjustGridSize + .001; x += adjustedGridStep)
		{

			vertices[0] = -adjustGridSize; vertices[1] = gridHeight; vertices[2] = x;
			vertices[3] = adjustGridSize; vertices[4] = gridHeight; vertices[5] = x;
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(shapeData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glDrawArrays(GL_LINES, 0, 2);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(shapeData->positionLoc); 
		glDisable(GL_BLEND);

	}

	void drawSkeleton(ESContext *esContext){
		ShapeData *shapeData = (ShapeData*)esContext->shapeData;
		glUseProgram (shapeData->programObject);
		const std::vector<std::string>& chars = SmartBody::SBScene::getScene()->getCharacterNames();
		for (std::vector<std::string>::const_iterator charIter = chars.begin();
			charIter != chars.end();
			charIter++)
		{
			SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter((*charIter));
			
			SmartBody::SBSkeleton* sk = character->getSkeleton();
			sk->update_global_matrices();
			std::map<int,int> indexMap;
			size_t numJoints = sk->joints().size();

			GLfloat *jointPos = new GLfloat[3 * numJoints];
			GLushort *boneIdx = new GLushort[2 * numJoints];
			
			for (int i=0;i<sk->joints().size();i++)
			{
				SkJoint* joint = sk->joints()[i];
				SrVec pos = joint->gmat().get_translation();
				jointPos[i * 3 + 0] = pos.x;
				jointPos[i * 3 + 1] = pos.y;
				jointPos[i * 3 + 2] = pos.z;
				//printf("Joint[%d]:(%f, %f, %f)\n", i, pos.x, pos.y, pos.z);
				indexMap[joint->index()] = i;
				boneIdx[i*2+0] = joint->index();
				if (joint->parent())
					boneIdx[i*2+1] = joint->parent()->index();
				else
					boneIdx[i*2+1] = joint->index();
			}
			for (int i=0;i<sk->joints().size();i++)
			{
				boneIdx[i*2] = indexMap[boneIdx[i*2]];
				boneIdx[i*2+1] = indexMap[boneIdx[i*2+1]];
			}

			glUniform4f(shapeData->colorLoc, 0.0, 0.0, 0.0, 1.0);
			glUniform1i(shapeData->lightEnabledLoc, 0);

			glEnableVertexAttribArray(shapeData->positionLoc); //Enable position vertex array attribute
			glLineWidth(2.0f);

			//draw joints
			glBindBuffer(GL_ARRAY_BUFFER, shapeData->jointPosObject);
			glBufferData(GL_ARRAY_BUFFER, numJoints * sizeof(GLfloat) * 3, jointPos, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(shapeData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glDrawArrays(GL_POINTS, 0, (GLsizei)numJoints);

			glUniform4f(shapeData->colorLoc, 1.0, 1.0, 1.0, 1.0);
			//draw bones
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapeData->boneIdxObject);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numJoints * 2 * sizeof(GLushort), boneIdx, GL_DYNAMIC_DRAW);
			glDrawElements(GL_LINES, (GLsizei)(numJoints * 2), GL_UNSIGNED_SHORT, 0); 
			
			delete [] jointPos;
			delete [] boneIdx;
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
#if __cplusplus
}
#endif
