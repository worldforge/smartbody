/*

Shader code for Smartbody JS.
Author: Zengrui Wang @ USC ICT
Revised: 12/15/2015

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
#include "Shader.h"
#include "sb/SBScene.h"
#include "sb/SBAttribute.h"
#include "sbm/GPU/SbmTexture.h"
#include "sr/sr_camera.h"
#include "sr/sr_light.h"
#include "sr/sr_euler.h"
#include "sr/sr_quat.h"
#include "boost/lexical_cast.hpp"

#include <string>
#include <vector>
#include <stdlib.h>

#if defined(EMSCRIPTEN)
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <emscripten.h>
#endif

#define MAX_VERTEX_UNIFORM_1024 1024
#define MAX_NUM_OF_LIGHTS		5

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
		"const int numOfLights = 2;						\n"
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
		"uniform   vec3 uMeshScale_hasBone_lightEnabled;\n"
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
		"   vec4 shadeColor;												\n"
		"   vTexCoord = aTexCoord;			\n"
		"   vec3 pos = vec3(aPosition.xyz) * uMeshScale_hasBone_lightEnabled.x;	\n"
		"	if(uMeshScale_hasBone_lightEnabled.y == 0.0)					\n"
		"		gl_Position = uMVPMatrix * vec4(pos.xyz, 1.0);				\n"
		"	else{															\n"
		"		vec4 skinPos = TransformPos(pos.xyz,BoneID1,BoneWeight1);	\n"
		"		gl_Position = uMVPMatrix * skinPos;							\n"
		"	}																\n"
		"	if(uMeshScale_hasBone_lightEnabled.z == 1.0){					\n"
		"		shadeColor = vec4(c_zero, c_zero, c_zero, c_zero);		\n"
		"		for(int i = 0; i < numOfLights; i++){						\n"
		"			shadeColor += calculateDirectionalShading(i);			\n"
		"		}															\n"
		"	}																\n"
		"	else  shadeColor = vec4(c_one, c_one, c_one, c_one);			\n"
		"	vComputedLightColor = shadeColor;								\n"
		"}																    \n"

		;

	char vShaderLimitedStr[] =  
		"struct lightSource {												\n"
		"	vec4	position;												\n"
		"	vec4	diffuse;												\n"
		"};																	\n"
		"const int numOfLights = 2;											\n"
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
		"uniform   vec3 uMeshScale_hasBone_lightEnabled;					\n"
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
		"   vec4 shadeColor;												\n"
		"   vTexCoord = aTexCoord;			\n"
		"   vec3 pos = vec3(aPosition.xyz) * uMeshScale_hasBone_lightEnabled.x;	\n"
		"	if(uMeshScale_hasBone_lightEnabled.y == 0.0)					\n"
		"		gl_Position = uMVPMatrix * vec4(pos.xyz, 1.0);				\n"
		"	else{															\n"
		"		vec4 skinPos = TransformPos(pos.xyz,BoneID1,BoneWeight1);	\n"
		"		gl_Position = uMVPMatrix * skinPos;							\n"
		"	}																\n"
		"	if(uMeshScale_hasBone_lightEnabled.z == 1.0){					\n"
		"		shadeColor = vec4(c_zero, c_zero, c_zero, c_zero);		\n"
		"		for(int i = 0; i < numOfLights; i++){						\n"
		"			shadeColor += calculateDirectionalShading(i);			\n"
		"		}															\n"
		"	}																\n"
		"	else  shadeColor = vec4(c_one, c_one, c_one, c_one);			\n"
		"	vComputedLightColor = shadeColor;								\n"
		"}																    \n"

		;
	char fShaderStr[] = 
		"precision mediump float;											\n"
		"varying   vec2 vTexCoord;											\n"
		"uniform   sampler2D sTexture;										\n"
		"varying   vec4 vComputedLightColor;								\n"
		"void main()														\n"
		"{																	\n"
		"  vec4 texColor = texture2D( sTexture, vTexCoord );				\n"
		"  gl_FragColor  = texColor * vComputedLightColor;					\n"
		//"  gl_FragColor  = vec4(1.0, 0.0, 0.0, 1.0);					\n"
		"}																	\n"

		;
	char vShaderStrSphere[] = 
		"struct lightSource {				\n"
		"	vec4	position;				\n"
		"	vec4	ambient;				\n"
		"	vec4	diffuse;				\n"
		"};									\n"
		"precision mediump float;											\n"
		"attribute vec4 aPosition;											\n"
		"attribute vec3 aNormal;											\n"
		"attribute vec4 aColor;												\n"
		"uniform   mat4 uMVPMatrix;											\n"
		"uniform   mat4 uMVMatrix;											\n"
		"uniform   vec2 uLight_Tex;										\n"
		"const int numOfLights = 2;											\n"
		"const float c_zero = 0.0;											\n"
		"const float c_one  = 1.0;											\n"
		"uniform   lightSource uLights[numOfLights];						\n"
		"varying   vec4 vColor;												\n"
		"varying   vec3 vTexcoord;											\n"
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
		"	if(uLight_Tex.y == 1.0)											\n"
		"		vTexcoord = aNormal;										\n"
		"	if( uLight_Tex.x == 1.0 ){										\n"
		"		shadeColor = vec4(c_zero, c_zero, c_zero, c_zero);			\n"
		"		for(int i = 0; i < numOfLights; i++){						\n"
		"			shadeColor += calculateDirectionalShading(i);			\n"
		"		}															\n"
		"	}																\n"
		"	else															\n"
		"		shadeColor = vec4(c_one, c_one, c_one, c_one);				\n"														
		"   vColor		   = shadeColor * aColor;							\n"
		"}																	\n"

		;
	char fShaderStrSphere[] =  
		"precision mediump float;											\n"
		"varying   vec4 vColor;												\n"
		"varying   vec3 vTexcoord;											\n"
		"uniform samplerCube s_texture;										\n"
		"uniform   vec2 uLight_Tex;										\n"
		"void main()														\n"
		"{																	\n"
		"	if(uLight_Tex.y == 0.0)											\n"
		"		gl_FragColor  = vColor;										\n"
		"	else															\n"
		"		gl_FragColor = textureCube(s_texture, vTexcoord);			\n"
		"}																	\n"

		;

	std::vector<SrLight> _lights;
	GLint maxVUniforms = 0;

	GLuint SHADER_API esLoadShader(GLenum type, const char *shaderSrc)
	{
		GLuint shader;
		GLint compiled;

		//step 1: create the Shader object
		shader = glCreateShader(type);

		if(shader == NULL)
			return 0;
		//step 2: load the Shader source
		glShaderSource(shader, 1, &shaderSrc, NULL);

		//step 3: compile the Shader
		glCompileShader(shader);

		//step 4: check the compilation
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

		if(!compiled)
		{
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

			if(infoLen > 0){ 
				char* infoLog = (char*)malloc(sizeof(char) * infoLen);
				glGetShaderInfoSmartBody::util::log(shader, infoLen, NULL, infoLog);
				esLogMessage("Error compiling shader:\n%s\n", infoLog);
				free(infoLog);
			}

			glDeleteShader(shader);
			return 0;
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
		if(vertexShader == NULL)
		{
			glDeleteShader(vertexShader);
			return 0;
		}
		//step 2: load fragment shader
		fragmentShader		= esLoadShader(GL_FRAGMENT_SHADER, fragShaderSrc);
		if(fragmentShader ==  NULL)
		{
			glDeleteShader(fragmentShader);
			return 0;
		}
		//setp 3: create program object
		programObject = glCreateProgram();
		if(programObject == NULL)
			return 0;

		//step 4: attach vertexShader and fragmentShader to the program
		glAttachShader(programObject, vertexShader);
		glAttachShader(programObject, fragmentShader);

		//step 5: link the program
		glLinkProgram(programObject);

		//step 6: check linking result
		glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
		if(!linked)
		{
			GLint infoLen = 0;
			glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

			if(infoLen > 0)
			{
				char *infoLog = (char*)malloc(sizeof(char) * infoLen);

				glGetProgramInfoSmartBody::util::log(programObject, infoLen, NULL, infoLog);
				esLogMessage("Error linking program:\n%s\n", infoLog);
				free(infoLog);

			}

			glDeleteProgram(programObject);
			return 0;
		}

		//setp 7: free shader resources
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return programObject;
	}


	void SHADER_API shaderInit(ESContext *esContext){

		//query number of vertex uniform vector support
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVUniforms);
		SmartBody::util::log("Web browser supports %d number of Vertex uniform vectors", maxVUniforms);

		//load Vertex and Fragment Shader and link them into program object
		esContext->meshData = malloc(sizeof(MeshData)); 
		esContext->geometryData = malloc(sizeof(GeometryData)); 

		MeshData *meshData = (MeshData*)esContext->meshData; 
		if(maxVUniforms >= MAX_VERTEX_UNIFORM_1024)
			meshData->programObject = esLoadProgram(vShaderStr, fShaderStr);
		else
			meshData->programObject = esLoadProgram(vShaderLimitedStr, fShaderStr);

		GeometryData *geoData = (GeometryData*)esContext->geometryData;
		geoData->programObject = esLoadProgram(vShaderStrSphere, fShaderStrSphere);

		//Mesh part
		glUseProgram ( meshData->programObject );
		meshData->positionLoc	= glGetAttribLocation(meshData->programObject, "aPosition");
		meshData->normalLoc		= glGetAttribLocation(meshData->programObject, "aNormal");
		meshData->texCoordLoc	= glGetAttribLocation(meshData->programObject, "aTexCoord");
		meshData->boneIDLoc		= glGetAttribLocation(meshData->programObject, "BoneID1");
		meshData->boneWeightLoc = glGetAttribLocation(meshData->programObject, "BoneWeight1");
		meshData->mvpLoc		= glGetUniformLocation(meshData->programObject, "uMVPMatrix");
		meshData->mvLoc			= glGetUniformLocation(meshData->programObject, "uMVMatrix");
		meshData->samplerLoc	= glGetUniformLocation(meshData->programObject, "sTexture");
		meshData->QuaternionLoc	  = glGetUniformLocation(meshData->programObject, "uQuaternion");
		meshData->TranslationLoc  = glGetUniformLocation(meshData->programObject, "uTranslation"); 
		//some flag used in shader rendering
		meshData->meshScale_hasBone_lightEnabledLoc = glGetUniformLocation(meshData->programObject, "uMeshScale_hasBone_lightEnabled");

		if(maxVUniforms >= MAX_VERTEX_UNIFORM_1024){
			//hard coded material parameters for now, may be possible set it dynamically
			meshData->mtrlAmbientLoc = glGetUniformLocation(meshData->programObject, "uMtrl.ambient");
			meshData->mtrlDiffuseLoc = glGetUniformLocation(meshData->programObject, "uMtrl.diffuse");
			glUniform4f(meshData->mtrlAmbientLoc, 0.2, 0.2, 0.2, 1.0);
			glUniform4f(meshData->mtrlDiffuseLoc, 0.8, 0.8, 0.8, 1.0);
		}
		//generate buffers for attributes
		glGenBuffers(1, &meshData->meshPosObject);
		glGenBuffers(1, &meshData->meshNormalObject);
		glGenBuffers(1, &meshData->meshTexCoordObject);
		glGenBuffers(1, &meshData->subMeshTriObject);
		glGenBuffers(1, &meshData->boneWeightObject);
		glGenBuffers(1, &meshData->boneIdObject);


		//Geometry part
		glUseProgram ( geoData->programObject );
		geoData->positionLoc = glGetAttribLocation(geoData->programObject, "aPosition");
		geoData->normalLoc   = glGetAttribLocation(geoData->programObject, "aNormal");
		geoData->colorLoc    = glGetAttribLocation(geoData->programObject, "aColor");
		geoData->mvpLoc	     = glGetUniformLocation(geoData->programObject, "uMVPMatrix");
		geoData->mvLoc		 = glGetUniformLocation(geoData->programObject, "uMVMatrix");
		geoData->samplerLoc	 = glGetUniformLocation(geoData->programObject, "s_texture");
		geoData->light_texLoc = glGetUniformLocation(geoData->programObject, "uLight_Tex");

		glGenBuffers(1, &geoData->posObject);
		glGenBuffers(1, &geoData->indexObject);
		glGenBuffers(1, &geoData->vertexObject);
		glGenBuffers(1, &geoData->normalObject);
		glGenBuffers(1, &geoData->jointPosObject);
		glGenBuffers(1, &geoData->boneIdxObject);
		glGenTextures(1, &geoData->textureId);

	}
	void SHADER_API drawLights(ESContext *esContext)
	{
		/*
		Note: only support geometry lights for now
		*/
		MeshData *meshData = (MeshData*)esContext->meshData;
		GeometryData *geoData = (GeometryData*)esContext->geometryData;

		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
		SrCamera& cam = *scene->getActiveCamera();
		SrVec eye = cam.getEye();

		_lights.clear();
		
		int numLightsInScene = 0;
		SrLight light;
		
		//iterate through pawns and find lights
		const std::vector<std::string>& pawnNames =  SmartBody::SBScene::getScene()->getPawnNames();
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
		//no lights in the scene and no lights enabled
		if (_lights.size() == 0 && numLightsInScene == 0)
		{
			SrMat mat;

			SrLight light;		
			light.diffuse = SrColor( 1.0f, 1.0f, 1.0f );
			sr_euler_mat_xyz (mat, SR_TORAD(0), SR_TORAD(0), SR_TORAD(135));
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

		GLint loc;
		string str;
		glUseProgram ( geoData->programObject );
		//pass lights to shape shader
		for(int i = 0; i < _lights.size(); ++i){
			light = _lights[i];
			str = "uLights[" + boost::lexical_cast<std::string>(i) +  "].position";
			loc = glGetUniformLocation(geoData->programObject, str.c_str());
			glUniform4f(loc, light.position.x, light.position.y, light.position.z, 0.0);			
			str = "uLights[" + boost::lexical_cast<std::string>(i) +  "].diffuse";
			loc = glGetUniformLocation(geoData->programObject, str.c_str());
			glUniform4f(loc, (float)light.diffuse.r / 255.0f, (float)light.diffuse.g / 255.0f, (float)light.diffuse.b / 255.0f, 1.0f);

			str = "uLights[" + boost::lexical_cast<std::string>(i) +  "].ambient";
			loc = glGetUniformLocation(geoData->programObject, str.c_str());
			glUniform4f(loc, (float)light.ambient.r / 255.0f, (float)light.ambient.g / 255.0f, (float)light.ambient.b / 255.0f, 1.0f);

		}
		//pass lights to mesh shader 
		glUseProgram ( meshData->programObject );
		
		for(int i = 0; i < _lights.size(); ++i){
			if(maxVUniforms >= MAX_VERTEX_UNIFORM_1024 && i >= 2) break;
			light = _lights[i];
			str = "uLights[" + boost::lexical_cast<std::string>(i) +  "].position";
			loc = glGetUniformLocation(meshData->programObject, str.c_str());
			glUniform4f(loc, light.position.x, light.position.y, light.position.z, 0.0);			
			str = "uLights[" + boost::lexical_cast<std::string>(i) +  "].diffuse";
			loc = glGetUniformLocation(meshData->programObject, str.c_str());
			glUniform4f(loc, (float)light.diffuse.r / 255.0f, (float)light.diffuse.g / 255.0f, (float)light.diffuse.b / 255.0f, 1.0f);
			str = "uLights[" + boost::lexical_cast<std::string>(i) +  "].ambient";
			loc = glGetUniformLocation(meshData->programObject, str.c_str());
			glUniform4f(loc, (float)light.ambient.r / 255.0f, (float)light.ambient.g / 255.0f, (float)light.ambient.b / 255.0f, 1.0f);
		}
	}
	void SHADER_API drawMesh(DeformableMeshInstance *shape, ESContext *esContext, bool showSkinWeight)
	{
		MeshData *meshData = (MeshData*)esContext->meshData;
		SmartBody::SBScene *scene = SmartBody::SBScene::getScene();
		SrCamera& cam = *scene->getActiveCamera();
		
		SrMat matPerspective, additionalXform;
		cam.get_perspective_mat(matPerspective);
		cam.getAdditionalXForm(additionalXform);

		std::vector<SrVec4>          QuaternionBuf;
		std::vector<SrVec4>			 TranslationBuf;
		 
		DeformableMesh *mesh = shape->getDeformableMesh();
		if(!mesh)
			return;

		//if it is a static mesh, we attach it to the pawn so that pawn's transformation could be applied to the mesh
		if (shape->isStaticMesh())
		{
			SmartBody::SBSkeleton* skel = shape->getSkeleton();
			SmartBody::SBPawn* pawn		= skel->getPawn();
			SrMat mvMat = meshData->gMV, mvpMat = meshData->gMVP;
			const std::string& parentJoint = pawn->getStringAttribute("blendShape.parentJoint");
			if (parentJoint != "")
			{
				SmartBody::SBJoint* joint = skel->getJointByName(parentJoint);
				if (joint)
				{
					//joint to world matrix
					const SrMat& woMat = joint->gmat();
					mvMat = woMat * mvMat;		

					const SrVec& offsetTrans	= pawn->getVec3Attribute("blendShape.parentJointOffsetTrans");
					const SrVec& offsetRot		= pawn->getVec3Attribute("blendShape.parentJointOffsetRot");

					SrQuat quat;
					quat.set(offsetRot.x * M_PI / 180.0f, offsetRot.y * M_PI / 180.0f, offsetRot.z * M_PI / 180.0f);
					SrMat mat;
					quat.get_mat(mat);
					mat.set_translation(offsetTrans);
					//joint local transformation
					mvMat = mat * mvMat;	
				}
			}
			else
			{
				const SrMat& woMat = skel->root()->gmat();
				mvMat = woMat * mvMat;	 
			}
			 
			SrVec meshScale = shape->getMeshScale();
			SrMat scaleMat;
			//SmartBody::util::log("mesh scale: %f, %f, %f\n", meshScale[0], meshScale[1], meshScale[2]);
			scaleMat.scale(meshScale[0], meshScale[1], meshScale[2]); 
			//MV = scale * rotate * translation * root(joint->world) * MV
			mvMat = scaleMat * mvMat;	
			glUniformMatrix4fv(meshData->mvLoc, 1, GL_FALSE, (GLfloat *)mvMat.pt(0));
			mvpMat = additionalXform * mvMat * matPerspective;
			glUniformMatrix4fv(meshData->mvpLoc, 1, GL_FALSE, (GLfloat *)mvpMat.pt(0));
		}

		//get submesh
		std::vector<SbmSubMesh*>& subMeshList = mesh->subMeshList;
		glEnable(GL_CULL_FACE);
		if(scene->getBoolAttribute("enableAlphaBlend")){
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		//use this flag to disable all lights in the scene for now, need a better way to do it.
		//TODO: for example disable or enable a specific light
		//SmartBody::util::log("mesh scale: %f, %f, %f\n", shape->getMeshScale()[0], shape->getMeshScale()[1], shape->getMeshScale()[2]);
		glUniform3f(meshData->meshScale_hasBone_lightEnabledLoc, shape->getMeshScale()[0], 
			shape->transformBuffer.size() > 0 ? 1.0 : 0.0, scene->getBoolAttribute("lightEnabled") ? 1.0 : 0.0); 
		
		if (shape->_deformPosBuf.size() > 0)
		{
			glEnableVertexAttribArray(meshData->positionLoc);
			glBindBuffer(GL_ARRAY_BUFFER, meshData->meshPosObject);
			glBufferData(GL_ARRAY_BUFFER, shape->_deformPosBuf.size() * sizeof(GLfloat) * 3, (GLfloat*)&shape->_deformPosBuf[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(meshData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		if (mesh->normalBuf.size () > 0)
		{
			glEnableVertexAttribArray(meshData->normalLoc);
			glBindBuffer(GL_ARRAY_BUFFER, meshData->meshNormalObject);
			glBufferData(GL_ARRAY_BUFFER, mesh->normalBuf.size () * sizeof(GLfloat) * 3, (GLfloat*)&mesh->normalBuf[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(meshData->normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		
		if (mesh->texCoordBuf.size() > 0)
		{
			glEnableVertexAttribArray(meshData->texCoordLoc);
			glBindBuffer(GL_ARRAY_BUFFER, meshData->meshTexCoordObject);
			glBufferData(GL_ARRAY_BUFFER, mesh->texCoordBuf.size () * sizeof(GLfloat) * 2, (GLfloat*)&mesh->texCoordBuf[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(meshData->texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}
		
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
			glUniform4fv(meshData->QuaternionLoc, QuaternionBuf.size(), (GLfloat*)&QuaternionBuf[0]);
			glUniform4fv(meshData->TranslationLoc, TranslationBuf.size(), (GLfloat*)&TranslationBuf[0]);

			QuaternionBuf.clear();
			TranslationBuf.clear();

		}

		//load weight
		if(mesh->boneWeightBuf[0].size() > 0){
			glEnableVertexAttribArray(meshData->boneWeightLoc);
			glBindBuffer(GL_ARRAY_BUFFER, meshData->boneWeightObject);
			glBufferData(GL_ARRAY_BUFFER, mesh->boneWeightBuf[0].size() * sizeof(GLfloat) * 4, (GLfloat*)&mesh->boneWeightBuf[0][0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(meshData->boneWeightLoc,4,GL_FLOAT,0,0,0);
		}
		//load boneId
		if(mesh->boneIDBuf_f[0].size() > 0){
			glEnableVertexAttribArray(meshData->boneIDLoc);
			glBindBuffer(GL_ARRAY_BUFFER, meshData->boneIdObject);
			glBufferData(GL_ARRAY_BUFFER, mesh->boneIDBuf_f[0].size() * sizeof(GLfloat) * 4, (GLfloat*)&mesh->boneIDBuf_f[0][0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(meshData->boneIDLoc,4,GL_FLOAT,0,0,0);
		}

		for(unsigned int i = 0; i < subMeshList.size(); i++)
		{
			SbmSubMesh *subMesh = subMeshList[i];
			std::string texturesType = "static";
			if (shape->getCharacter())
				texturesType = shape->getCharacter()->getStringAttribute("texturesType");	
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
						}
						else 
						{
							glBindTexture(GL_TEXTURE_2D, tex->getID());
						}

					}
					else
					{
						glBindTexture(GL_TEXTURE_2D, tex->getID());
					}	
					glUniform1i (meshData->samplerLoc, 0);
				}
			}

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->subMeshTriObject);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, subMesh->triBuf.size() * sizeof(GLushort) * 3, (GLushort *)&subMesh->triBuf[0], GL_DYNAMIC_DRAW);
			glDrawElements(GL_TRIANGLES, subMesh->triBuf.size() * 3, GL_UNSIGNED_SHORT, 0);
		}
		if(scene->getBoolAttribute("enableAlphaBlend")){
			glDisable(GL_BLEND);
		}
		glDisable(GL_CULL_FACE);
		//unbind buffers
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SHADER_API drawSphere(ESContext *esContext, SrVec pos, float r, int p, SrVec color){
		GeometryData *geoData = (GeometryData*)esContext->geometryData;
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
			glEnableVertexAttribArray(geoData->positionLoc);
			glBindBuffer(GL_ARRAY_BUFFER, geoData->posObject);
			glBufferData(GL_ARRAY_BUFFER,  (p*6+6) * sizeof(GLfloat), (GLfloat*)&vertices[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(geoData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
			//bind normal buffer
			glEnableVertexAttribArray(geoData->normalLoc);
			glBindBuffer(GL_ARRAY_BUFFER, geoData->normalObject);
			glBufferData(GL_ARRAY_BUFFER,  (p*6+6) * sizeof(GLfloat), (GLfloat*)&normals[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(geoData->normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
			//constant color attribute
			glUniform2f(geoData->light_texLoc, 1.0, 0.0);
			//disable vertex attribute array to enable constant vertex attribute
			glDisableVertexAttribArray(geoData->colorLoc);
			glVertexAttrib4f(geoData->colorLoc, color.x, color.y, color.z, 1.0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, (p+1)*2);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(geoData->positionLoc);
		glDisableVertexAttribArray(geoData->normalLoc);

		delete [] vertices;
		delete [] normals;
	}

	void SHADER_API drawBox(ESContext *esContext, SrVec pos, SrVec extent, SrVec color){
		GeometryData *geoData = (GeometryData*)esContext->geometryData;
		//vertex attributes in one array
		GLint vtxStride = 3 + 3; //pos(x, y, z) + normal(x, y, z)
		GLfloat *vertexBuf = new GLfloat[vtxStride * 8];
		GLfloat p[]  = {-1.0, -1.0,  1.0,  1.0, -1.0,  1.0,  1.0,  1.0,  1.0, -1.0,  1.0,  1.0,
					    -1.0, -1.0, -1.0,  1.0, -1.0, -1.0,  1.0,  1.0, -1.0, -1.0,  1.0, -1.0
		               };
		for(int i = 0, j = 0; i < (3 + 3) * 8; i += 6, j +=3){
			//copy vertex position
			vertexBuf[i + 0] = p[j + 0] * extent.x + pos.x;
			vertexBuf[i + 1] = p[j + 1] * extent.y + pos.y;
			vertexBuf[i + 2] = p[j + 2] * extent.z + pos.z;
			vertexBuf[i + 3] = p[j + 0];
			vertexBuf[i + 4] = p[j + 1];
			vertexBuf[i + 5] = p[j + 2];
		}

		GLushort indices[]  = {0, 1, 2, 2, 3, 0, 3, 2, 6, 6, 7, 3, 7, 6, 5, 5, 4, 7, 
							   4, 0, 3, 3, 7, 4, 5, 1, 0, 0, 4, 5, 1, 5, 6, 6, 2, 1
							  }; 
		GLuint offset = 0;
		//bind vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, geoData->vertexObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vtxStride * 8, vertexBuf, GL_STATIC_DRAW);
		//bind index buffer object
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geoData->indexObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//position buffer
		glEnableVertexAttribArray(geoData->positionLoc);
		glVertexAttribPointer(geoData->positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * vtxStride, (const void*)offset);
		offset += 3 * sizeof(GLfloat);
		//normal buffer
		glEnableVertexAttribArray(geoData->normalLoc);
		glVertexAttribPointer(geoData->normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * vtxStride, (const void*)offset);
		//constant color attribute
		glDisableVertexAttribArray(geoData->colorLoc);
		glVertexAttrib4f(geoData->colorLoc, color.x, color.y, color.z, 1.0);
		
		glUniform2f(geoData->light_texLoc, 1.0, 0.0);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(geoData->positionLoc);
		glDisableVertexAttribArray(geoData->normalLoc);

		delete [] vertexBuf;
	}
	void SHADER_API drawBBox(ESContext *esContext, SrVec pos, SrVec extent){
		GeometryData *geoData = (GeometryData*)esContext->geometryData;
		GLfloat vertices[] = {-extent.x + pos.x, -extent.y + pos.y,  extent.z + pos.z,
							   extent.x + pos.x, -extent.y + pos.y,  extent.z + pos.z,  
							   extent.x + pos.x,  extent.y + pos.y,  extent.z + pos.z, 
							  -extent.x + pos.x,  extent.y + pos.y,  extent.z + pos.z,
							  -extent.x + pos.x, -extent.y + pos.y, -extent.z + pos.z,  
							   extent.x + pos.x, -extent.y + pos.y, -extent.z + pos.z,  
			                   extent.x + pos.x,  extent.y + pos.y, -extent.z + pos.z, 
							  -extent.x + pos.x,  extent.y + pos.y, -extent.z + pos.z
		};
		GLuint indices[]  = {0, 1, 1, 5, 5, 4, 4, 0, 0, 3, 1, 2, 5, 6, 4, 7, 3, 2, 2, 6, 6, 7, 7, 3}; 
		//position
		glEnableVertexAttribArray(geoData->positionLoc);
		glBindBuffer(GL_ARRAY_BUFFER, geoData->posObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), (GLfloat*)&vertices[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(geoData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//color
		glDisableVertexAttribArray(geoData->colorLoc);
		glVertexAttrib4f(geoData->colorLoc, 0.0, 1.0, 0.0, 1.0);
		//normal
		glDisableVertexAttribArray(geoData->normalLoc);
		glVertexAttrib3f(geoData->normalLoc, 0.0, 0.0, 0.0);
		
		glUniform2f(geoData->light_texLoc, 0.0, 0.0);
		//index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geoData->indexObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), (GLuint*)&indices[0], GL_DYNAMIC_DRAW);
		glDrawElements(GL_LINES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(geoData->positionLoc);

	}
	void SHADER_API drawCoordinates(ESContext *esContext){
		//printf("draw coordinates\n");
		GeometryData *geoData = (GeometryData*)esContext->geometryData;
		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
		GLfloat vertexBuff[] = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, //o
								0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, //x
								0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, //o
							    0.0, 0.5, 0.0, 0.0, 1.0, 0.0, 1.0, //y
								0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, //o
							    0.0, 0.0, 0.5, 0.0, 0.0, 1.0, 1.0 //z
								}; 
		int vertexStride = 3 + 4;
		GLuint offset = 0;

		
		//vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, geoData->vertexObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuff), (const GLfloat*)&vertexBuff[0], GL_STATIC_DRAW);
		
		//enable position buffer
		glEnableVertexAttribArray(geoData->positionLoc);
		glVertexAttribPointer(geoData->positionLoc, 3, GL_FLOAT, GL_FALSE, vertexStride * sizeof(GLfloat), (const void*)offset);
		offset += 3 * sizeof(GLfloat);

		//enable color buffer
		glEnableVertexAttribArray(geoData->colorLoc);
		glVertexAttribPointer(geoData->colorLoc, 4, GL_FLOAT, GL_FALSE, vertexStride * sizeof(GLfloat), (const void*)offset);

		//disable normal buffer
		glDisableVertexAttribArray(geoData->normalLoc);
		glVertexAttrib3f(geoData->normalLoc, 0.0, 0.0, 0.0);
		glDisable(GL_DEPTH_TEST);
		glUniform2f(geoData->light_texLoc, 0.0, 0.0);
		glDrawArrays(GL_LINES, 0, 6);
		glEnable(GL_DEPTH_TEST);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(geoData->positionLoc);
		glDisableVertexAttribArray(geoData->colorLoc);
		//glDisable(GL_BLEND);
	}
	void SHADER_API drawGrid(ESContext *esContext){
		GeometryData *geoData = (GeometryData*)esContext->geometryData;

		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
		float sceneScale = scene->getScale();
		GLfloat gridHeight = 0.0f + 0.001f/scene->getScale();
		float gridStep = 20.0, gridSize = 400.0;
		float adjustedGridStep = gridStep;
		float adjustGridSize = gridSize * .01f / sceneScale;
		if (sceneScale > 0.f)
		{
			adjustedGridStep *= .01f / sceneScale;
		}

		int numOfVertexPerEdge = (adjustGridSize * 2 / adjustedGridStep) + 1;
		GLfloat *vertexBuff = new GLfloat[numOfVertexPerEdge * 4 * 3];
		int indexVtx = 0;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform2f(geoData->light_texLoc, 0.0, 0.0);

		//vertical lines
		for (float x = -adjustGridSize; x <= adjustGridSize + .001; x += adjustedGridStep)
		{
			vertexBuff[indexVtx++] = x; vertexBuff[indexVtx++] = gridHeight; vertexBuff[indexVtx++] = -adjustGridSize;
			vertexBuff[indexVtx++] = x; vertexBuff[indexVtx++] = gridHeight; vertexBuff[indexVtx++] = adjustGridSize;	
		}
		//horizontal lines
		for (float x = -adjustGridSize; x <= adjustGridSize + .001; x += adjustedGridStep)
		{
			vertexBuff[indexVtx++] = -adjustGridSize; vertexBuff[indexVtx++] = gridHeight; vertexBuff[indexVtx++] = x;
			vertexBuff[indexVtx++] = adjustGridSize;  vertexBuff[indexVtx++] = gridHeight; vertexBuff[indexVtx++] = x;
		}
		
		//bind position buffer
		glBindBuffer(GL_ARRAY_BUFFER, geoData->posObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numOfVertexPerEdge * 4 * 3, vertexBuff, GL_STATIC_DRAW);
		glEnableVertexAttribArray(geoData->positionLoc);
		glVertexAttribPointer(geoData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//pass color attribute
		glDisableVertexAttribArray(geoData->colorLoc);
		glVertexAttrib4f(geoData->colorLoc, 0.7, 0.7, 0.7, 1.0);
		glDisableVertexAttribArray(geoData->normalLoc);
		glVertexAttrib3f(geoData->normalLoc, 0.0, 0.0, 0.0);

		glDrawArrays(GL_LINES, 0, numOfVertexPerEdge * 4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(geoData->positionLoc); 
		glDisable(GL_BLEND);
		delete [] vertexBuff;
	}

	void SHADER_API drawSkeleton(ESContext *esContext){
		GeometryData *geoData = (GeometryData*)esContext->geometryData;
		glUseProgram (geoData->programObject);
		const std::vector<std::string>& chars = SmartBody::SBScene::getScene()->getCharacterNames();
		for (std::vector<std::string>::const_iterator charIter = chars.begin();
			charIter != chars.end();
			charIter++)
		{
			SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter((*charIter));

			int numJoints;
			SmartBody::SBSkeleton* sk = character->getSkeleton();
			sk->update_global_matrices();
			std::map<int,int> indexMap;
			numJoints = sk->joints().size();

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

			glVertexAttrib4f(geoData->colorLoc, 0.0, 0.0, 0.0, 1.0);
			glUniform2f(geoData->light_texLoc, 0.0, 0.0);

			glEnableVertexAttribArray(geoData->positionLoc); //Enable position vertex array attribute
			glLineWidth(2.0f);

			//draw joints
			glBindBuffer(GL_ARRAY_BUFFER, geoData->jointPosObject);
			glBufferData(GL_ARRAY_BUFFER, numJoints * sizeof(GLfloat) * 3, (GLfloat*)&jointPos[0], GL_DYNAMIC_DRAW);
			glVertexAttribPointer(geoData->positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glDrawArrays(GL_POINTS, 0, numJoints);

			glVertexAttrib4f(geoData->colorLoc, 1.0, 1.0, 1.0, 1.0);
			//draw bones
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geoData->boneIdxObject);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numJoints * 2 * sizeof(GL_UNSIGNED_SHORT), (GLushort*)&boneIdx[0], GL_DYNAMIC_DRAW);
			glDrawElements(GL_LINES, numJoints * 2, GL_UNSIGNED_SHORT, 0); 

			delete [] jointPos;
			delete [] boneIdx;
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void SHADER_API drawSkyBox(ESContext *esContext){
		glDepthMask(GL_FALSE);

		GeometryData *geoData = (GeometryData*)esContext->geometryData;
		//vertex attributes in one array
		GLfloat vertexBuf[] = {-10.0, -10.0, 10.0,   -1.0, -1.0, 1.0, 
							   10.0, -10.0, 10.0,    1.0, -1.0, 1.0,
							   10.0, -10.0, -10.0,   1.0, -1.0, -1.0,
							  -10.0, -10.0, -10.0,  -1.0, -1.0, -1.0,
							  -10.0, 10.0, 10.0,  -1.0, 1.0, 1.0,
							   10.0, 10.0, 10.0,   1.0, 1.0, 1.0,
							   10.0, 10.0, -10.0,  1.0, 1.0, -1.0,
							  -10.0, 10.0, -10.0, -1.0, 1.0, -1.0};
		GLushort indices[] = {
								2, 5, 6, 2, 1, 5, //right face - posx
								0, 7, 4, 0, 3, 7, //left face - negz
								7, 5, 4, 7, 6, 5, //up face -posy
								0, 2, 3, 0, 1, 2, //bottom face - negy
								1, 4, 5, 1, 0, 4, //front face - posz
								3, 6, 7, 3, 2, 6  //back face - negz
							 };
		int offset = 0;
		GLsizei vtxStride = 3 + 3;
		//bind vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, geoData->vertexObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vtxStride * 8, vertexBuf, GL_STATIC_DRAW);

		//bind indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geoData->indexObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), (const GLvoid*)&indices[0], GL_STATIC_DRAW);

		//position buffer
		glEnableVertexAttribArray(geoData->positionLoc);
		glVertexAttribPointer(geoData->positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * vtxStride, (const void*)offset);
		offset += 3 * sizeof(GLfloat);
		//normal buffer
		glEnableVertexAttribArray(geoData->normalLoc);
		glVertexAttribPointer(geoData->normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * vtxStride, (const void*)offset);
		
		SbmCubeMapTexture* tex = SbmTextureManager::singleton().findCubeMapTexture("skybox");
		if(tex){
			geoData->textureId = tex->getID();
			glActiveTexture ( GL_TEXTURE0 );
			glBindTexture ( GL_TEXTURE_CUBE_MAP, geoData->textureId );
			glUniform1i (geoData->samplerLoc, 0);
		}

		//color
		glDisableVertexAttribArray(geoData->colorLoc);
		glVertexAttrib4f(geoData->colorLoc, 0.0, 1.0, 0.0, 1.0);

		glUniform2f(geoData->light_texLoc, 0.0, 1.0);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(geoData->positionLoc);
		glDepthMask(GL_TRUE);
	}
#if __cplusplus
}
#endif
