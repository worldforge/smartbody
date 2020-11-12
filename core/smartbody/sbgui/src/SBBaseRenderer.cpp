#if !defined(__FLASHPLAYER__) && !defined(__ANDROID__) && !defined(EMSCRIPTEN)
#include "external/glew/glew.h"
#endif

#include "SBBaseRenderer.h"
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include <sr/sr_camera.h>
#include <sr/sr_mat.h>
#include <sr/sr_random.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <sbm/GPU/VBOData.h>

char vTestShaderStr[] =
"#version 430                        \n"
"layout(location = 0) in vec3 aPosition;						\n"
"uniform   mat4 uMVPMatrix;						\n"
"uniform   mat4 uMVMatrix;						\n"
"void main()						\n"
"{									\n"
"   vec3 pos = vec3(aPosition.xyz)* 1.0;						\n"
"	vec4 skinPos = vec4(pos.xyz,1.0);		                        \n"
"	gl_Position = uMVPMatrix * skinPos;								\n"
"}																    \n"

;

char fTestShaderStr[] =
"#version 430                      \n"
"out  vec4 fragColor;\n"
"void main()														\n"
"{																	\n"
" fragColor  =  vec4(1.0, 0.0, 0.0 ,1.0);                   			     \n"
"}																	\n"

;

char vShaderNormalMapStr[] =
"#version 430                        \n"
"struct lightSource {							\n"
"	mediump vec4	position;							\n"
"	mediump vec4	ambient;							\n"
"	mediump vec4	diffuse;							\n"
"};												\n"
"const int numOfLights = 3;						\n"
"layout(location = 0) in vec3 aPosition;						\n"
"layout(location = 1) in vec3 aNormal; 			\n"
"layout(location = 2) in vec2 aTexCoord;						\n"
"layout(location = 3) in vec3 aTangent;			\n"
"uniform   mat4 uMVPMatrix;						\n"
"uniform   mat4 uMVMatrix;						\n"
"uniform   mat4 uModelMatrix;                   \n"
"uniform   lightSource uLights[numOfLights];    \n"
"out   vec2 vTexCoord;						\n"
"out   vec3 vNormal;                        \n"
"out   vec3 lightDir[numOfLights], halfVec[numOfLights];  \n"
"uniform   float uMeshScale;					\n"
"const float c_zero = 0.0;											\n"
"const float c_one  = 1.0;											\n"
"void main()						\n"
"{									\n"
"	gl_PointSize = 2.0;				\n"
"   vTexCoord = aTexCoord;			\n"
"   vec3 pos = vec3(aPosition.xyz)* 1.0;						\n"
"	vec4 skinPos = vec4(pos.xyz,1.0);		                        \n"
"	gl_Position = uMVPMatrix * uModelMatrix * skinPos;								\n"
"   vec3 vertexPos = -normalize(vec3(uMVMatrix * uModelMatrix* vec4(skinPos.xyz,1.0)));          \n"
"   mat3 tangentMat;                                                \n"
"   vec3 vTangent = aTangent;										\n"
"   vNormal = aNormal;										\n"
"	if (isnan(vTangent.x) || isnan(vTangent.y) || isnan(vTangent.z)) vTangent = vec3(1,0,0); \n"
"   tangentMat[0] = normalize((uMVMatrix * vec4(vTangent,0.0)).xyz);            \n"
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

char fShaderNormalMapStr[] =
"#version 430 compatibility                      \n"
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
"const int numOfLights = 3;						\n"
"in   vec3 lightDir[numOfLights], halfVec[numOfLights];\n"
"in   vec2 vTexCoord;											\n"
"in   vec3 vNormal;											\n"
"layout(binding=0) uniform sampler2D sTexture; \n"
"layout(binding=1) uniform sampler2D normalTexture; \n"
"layout(binding=2) uniform sampler2D specularTexture; \n"
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
//"   float specularIntensity = pow (max (dot (normalize(vHalfVec), normal), 0.0), 10.0 + 0.01);\n"
"   computedColor += (specularIntensity * specularColor);        \n"
//"   computedColor += (specularIntensity * vec4(1.0,1.0,1.0,1.0));        \n"
//"   computedColor += ndotl;        \n"
"   return computedColor;											\n"
"}																    \n"
"void main()														\n"
"{																	\n"
"  vec4 texColor = texture2D( sTexture, vTexCoord );				\n"
"  float alpha = texColor.a*uMtrl.diffuse.a;						\n"
"  if (alpha < .05) discard;											\n"
//"  float alpha = uMtrl.diffuse.a;"
// "  if (alpha < 0.8) discard;                                   \n"

//"  float alpha = texColor.a;									\n"
//"  if (alpha < 0.2) discard;                                   \n"
"  vec3 normal = 2.0 * texture2D(normalTexture, vTexCoord.st).rgb - 1.0;\n"
"  vec4 specularColor = texture2D( specularTexture, vTexCoord );				\n"
"  normal = normalize (normal);                                  \n"
"  vec4 vComputedLightColor = vec4(0.0,0.0,0.0,0.0);             \n"
"  for (int i=0;i<numOfLights; i++)                              \n"
"  {                                                             \n"
"       vComputedLightColor += calculateLighting(uLights[i].diffuse, specularColor, normal, lightDir[i], halfVec[i], texColor.xyz); \n"
"  }                                                             \n"
"  vComputedLightColor.a = alpha;                                  \n"
//"  gl_FragColor  = texColor * vComputedLightColor;				 \n"
"  gl_FragColor  = vComputedLightColor;				 \n"
//" gl_FragColor  =  vec4(alpha, alpha, alpha ,1.0);                   			     \n"
//" gl_FragColor  =  vec4(vNormal.r, vNormal.g, vNormal.b ,1.0);                   			     \n"
"}																	\n"

;

SBBaseRenderer* SBBaseRenderer::_singleton = NULL;

SBBaseRenderer& SBBaseRenderer::singleton()
{
	if (!_singleton)
		_singleton = new SBBaseRenderer();
	return *_singleton;
}

void SBBaseRenderer::destroy_singleton()
{
	delete _singleton;
	_singleton = NULL;
}

SBBaseRenderer::SBBaseRenderer(void)
{
	rendererInitialized = false;
	backgroundColor = SrColor(0.6f, 0.6f, 0.6f);
}


SBBaseRenderer::~SBBaseRenderer(void)
{

}


void SBBaseRenderer::draw(std::vector<SrLight>& lights, bool isDrawFloor)
{
	if (!isInitialized()) return;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::vector<std::string> pawnNames = scene->getPawnNames();
#if 1
	for (unsigned int i = 0; i < pawnNames.size(); i++)
	{
		SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnNames[i]);
		DeformableMeshInstance* meshInstance = pawn->getActiveMesh();
		if (meshInstance && meshInstance->getDeformableMesh())
		{
			GPUBlendShapeUpdate(meshInstance);
			if (!meshInstance->isStaticMesh())
			{
				SbmDeformableMeshGPUInstance* gpuMeshInstance = dynamic_cast<SbmDeformableMeshGPUInstance*>(meshInstance);
				GPUMeshUpdate(meshInstance);
			}			
		}
	}
#endif
	glUseProgram(normalMapShader->getShaderProgram());
	// setup lighting
	const int numOfLights = 4;
	GLint loc;
	SrLight light;
	std::string str;
	for (int i = 0; i < lights.size(); ++i) {
		light = lights[i];
		str = "uLights[" + boost::lexical_cast<std::string>(i) + "].position";
		loc = glGetUniformLocation(normalMapShader->getShaderProgram(), str.c_str());
		glUniform4f(loc, light.position.x, light.position.y, light.position.z, 0.0);
		str = "uLights[" + boost::lexical_cast<std::string>(i) + "].diffuse";
		loc = glGetUniformLocation(normalMapShader->getShaderProgram(), str.c_str());
		glUniform4f(loc, (float)light.diffuse.r / 255.0f, (float)light.diffuse.g / 255.0f, (float)light.diffuse.b / 255.0f, 1.0f);
		str = "uLights[" + boost::lexical_cast<std::string>(i) + "].ambient";
		loc = glGetUniformLocation(normalMapShader->getShaderProgram(), str.c_str());
		glUniform4f(loc, (float)light.ambient.r / 255.0f, (float)light.ambient.g / 255.0f, (float)light.ambient.b / 255.0f, 1.0f);
	}
	//material
	SrCamera* cam = SmartBody::SBScene::getScene()->getActiveCamera();
	// if there is no active camera, then only show the blank screen
	if (!cam)
		return;

	SrMat mat, modelViewMat, modelViewProjMat;
	//----- Set Projection ----------------------------------------------
	cam->setAspectRatio((float)width / (float)height);
	glViewport(0, 0, width, height);
	float bgColor[4];
	backgroundColor.get(bgColor);
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	cam->get_view_mat(modelViewMat);
	cam->get_perspective_mat(modelViewProjMat);
	modelViewProjMat = modelViewMat*modelViewProjMat;

	glUniformMatrix4fv(glGetUniformLocation(normalMapShader->getShaderProgram(), "uMVMatrix"), 1, GL_FALSE, (GLfloat*)&modelViewMat);
	glUniformMatrix4fv(glGetUniformLocation(normalMapShader->getShaderProgram(), "uMVPMatrix"), 1, GL_FALSE, (GLfloat*)&modelViewProjMat);

	static GLfloat mat_emissin[] = { 0.f,  0.f,    0.f,    1.f };
	static GLfloat mat_ambient[] = { 0.f,  0.f,    0.f,    1.f };
	static GLfloat mat_diffuse[] = { 0.1f,  0.1f,    0.1f,    1.f };
	static GLfloat mat_speclar[] = { 0.f,  0.f,    0.f,    1.f };

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//SmartBody::util::log("Render Forward shading");
	for (unsigned int i = 0; i < pawnNames.size(); i++)
	{
		SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnNames[i]);
		DeformableMeshInstance* meshInstance = pawn->getActiveMesh();
		if (meshInstance)
		{
			renderMesh(meshInstance, normalMapShader, false);
		}
	}
	if (isDrawFloor)
		drawFloor(false);
	glUseProgram(0);
}

void SBBaseRenderer::drawFloor(bool deferredRender)
{
	//if (viewerData->showFloor)
	{

		GLint mtrlAmbientLoc, mtrlDiffuseLoc, mtrlShininessLoc;
		if (!deferredRender)
		{
			mtrlAmbientLoc = glGetUniformLocation(normalMapShader->getShaderProgram(), "uMtrl.ambient");
			mtrlDiffuseLoc = glGetUniformLocation(normalMapShader->getShaderProgram(), "uMtrl.diffuse");
			mtrlShininessLoc = glGetUniformLocation(normalMapShader->getShaderProgram(), "uMtrl.shininess");
			//SmartBody::util::log("Ambient Loc = %d, diffuse Loc = %d, shininess Loc = %d", mtrlAmbientLoc, mtrlDiffuseLoc, mtrlShininessLoc);
			glUniform4f(mtrlAmbientLoc, 0.0, 0.0, 0.0, 1.0);
			glUniform4f(mtrlDiffuseLoc, 0.8, 0.8, 0.8, 1.0);
			glUniform1f(mtrlShininessLoc, 100.0);
		}

		SrMat modelMat = SrMat::id;		
		glUniformMatrix4fv(glGetUniformLocation(normalMapShader->getShaderProgram(), "uModelMatrix"), 1, GL_FALSE, (GLfloat*)&modelMat);

		std::string defaultTexName = "gray_tex";
		std::string defaultNormalTex = "defaultNormal_tex";
		std::string defaultGlossyTex = "gray_tex";
		SbmTexture* tex = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE, defaultTexName.c_str());
		SbmTexture* blueTex = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE, defaultNormalTex.c_str());
		SbmTexture* grayTex = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE, defaultGlossyTex.c_str());
		//	If we are using blended textures
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		if (tex)
			glBindTexture(GL_TEXTURE_2D, tex->getID());

		glActiveTexture(GL_TEXTURE1);
		if (blueTex)
			glBindTexture(GL_TEXTURE_2D, blueTex->getID());

		glActiveTexture(GL_TEXTURE2);
		if (grayTex)
			glBindTexture(GL_TEXTURE_2D, grayTex->getID());


		SrVec upVec = SrVec(0.f, 1.f, 0.01f);
		float floorSize = 1200;
		float planeY = -0.0f;
		glBegin(GL_QUADS);
		glVertexAttrib2f(2, 0, 0);
		glVertexAttrib3f(1, upVec[0], upVec[1], upVec[2]);
		glVertex3f(-floorSize, planeY, floorSize);
		glVertexAttrib2f(2, 0, 1);
		glVertexAttrib3f(1, upVec[0], upVec[1], upVec[2]);
		glVertex3f(floorSize, planeY, floorSize);
		glVertexAttrib2f(2, 1, 1);
		glVertexAttrib3f(1, upVec[0], upVec[1], upVec[2]);
		glVertex3f(floorSize, planeY, -floorSize);
		glVertexAttrib2f(2, 1, 0);
		glVertexAttrib3f(1, upVec[0], upVec[1], upVec[2]);
		glVertex3f(-floorSize, planeY, -floorSize);
		glEnd();
		// unbind texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void SBBaseRenderer::GPUMeshUpdate(DeformableMeshInstance* meshInstance)
{
	SbmDeformableMeshGPUInstance* gpuMeshInstance = (SbmDeformableMeshGPUInstance*)meshInstance;
	SbmDeformableMeshGPU* gpuMesh = (SbmDeformableMeshGPU*)gpuMeshInstance->getDeformableMesh();

	if (!gpuMesh) return;

	static GLuint queryName = -1;
	if (queryName == -1)
		glGenQueries(1, &queryName);
	// setup transform feedback
	const char* attr[3] = { "deformPos", "deformNormal", "deformTangent" };
	glTransformFeedbackVaryings(skinningShader->getShaderProgram(), 3, attr, GL_SEPARATE_ATTRIBS);
	glLinkProgram(skinningShader->getShaderProgram());

	glEnableVertexAttribArray(0);
	gpuMesh->getPosVBO()->VBO()->BindBuffer();
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

	glEnable(GL_RASTERIZER_DISCARD);
	gpuMeshInstance->updateTransformBuffer();
	std::vector<SrMat>& transBuffer = gpuMeshInstance->getTransformBuffer();
	//float meshScale = 
	SrVec meshScale = meshInstance->getMeshScale();

	glUseProgram(skinningShader->getShaderProgram());
	glUniformMatrix4fv(glGetUniformLocation(skinningShader->getShaderProgram(), "Transform"), transBuffer.size(), true, (GLfloat*)getPtr(transBuffer));
	glUniform1f(glGetUniformLocation(skinningShader->getShaderProgram(), "meshScale"), meshScale[0]);
	// bind transform feedback buffer
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, gpuMeshInstance->getVBODeformPos()->VBO()->m_iVBO_ID);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, gpuMeshInstance->getVBODeformNormal()->VBO()->m_iVBO_ID);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, gpuMeshInstance->getVBODeformTangent()->VBO()->m_iVBO_ID);


	//glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, queryName);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, gpuMeshInstance->_deformPosBuf.size());
	glEndTransformFeedback();
	//glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);	

	//GLuint PrimitivesWritten = 0;
	//glGetQueryObjectuiv(queryName, GL_QUERY_RESULT, &PrimitivesWritten);
	//SmartBody::util::log("Output transform feedback = %d", PrimitivesWritten);

	glUseProgram(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glDisable(GL_RASTERIZER_DISCARD);
}


void SBBaseRenderer::resize(int w, int h)
{
	width = w;
	height = h;
}


void SBBaseRenderer::resetRenderer()
{
	skinningShader = normalMapShader = NULL;
	rendererInitialized = false;
}

void SBBaseRenderer::initRenderer(int w, int h)
{
	width = w;
	height = h;

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::string shaderPath = scene->getMediaPath() + "/shaders/";
	SbmShaderManager& shaderManager = SbmShaderManager::singleton();
	
	skinningShader = shaderManager.getShader("skinning_shader");
	if (skinningShader)
	{
		skinningShader = shaderManager.getShader("skinning_shader");
		normalMapShader = shaderManager.getShader("normalMap_shader");
		rendererInitialized = true;
		return;
	}

	shaderManager.addShader("skinning_shader", shaderPath + "skinning.vert", "", true);
	skinningShader = shaderManager.getShader("skinning_shader");

	SmartBody::util::log("Building normalMap_shader\n");
	shaderManager.addShader("normalMap_shader", vShaderNormalMapStr, fShaderNormalMapStr, false);
	//shaderManager.addShader("normalMap_shader", vTestShaderStr, fTestShaderStr, false);
	normalMapShader = shaderManager.getShader("normalMap_shader");

	SbmTextureManager& texManager = SbmTextureManager::singleton();
	texManager.createColorTexture("white_tex", SrColor::white);
	texManager.createColorTexture("black_tex", SrColor::black);
	texManager.createColorTexture("gray_tex", SrColor(0.2f, 0.2f, 0.2f, 1.0f));
	texManager.createColorTexture("defaultSpecular_tex", SrColor(0.2f, 0.2f, 0.2f, 0.2f));
	texManager.createColorTexture("defaultNormal_tex", SrColor(0.5f, 0.5f, 1.0f));
	rendererInitialized = true;

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void SBBaseRenderer::renderMesh(DeformableMeshInstance* meshInstance, SbmShaderProgram* shaderProgram, bool deferredRender)
{
	DeformableMesh* mesh = meshInstance->getDeformableMesh();
	if (!mesh)
	{
		//SmartBody::util::log("SBRenderer::renderMesh ERR: no deformable mesh found!");
		return; // no deformable mesh
	}

	SbmTextureManager& texManager = SbmTextureManager::singleton();
	SbmTexture* whiteTex = texManager.findTexture(SbmTextureManager::TEXTURE_DIFFUSE, "white_tex");
	SbmTexture* blackTex = texManager.findTexture(SbmTextureManager::TEXTURE_DIFFUSE, "black_tex");
	SbmTexture* grayTex = texManager.findTexture(SbmTextureManager::TEXTURE_DIFFUSE, "gray_tex");
	SbmTexture* defaultSpecularTex = texManager.findTexture(SbmTextureManager::TEXTURE_DIFFUSE, "defaultSpecular_tex");
	SbmTexture* defaultNormalTex = texManager.findTexture(SbmTextureManager::TEXTURE_DIFFUSE, "defaultNormal_tex");

#if 0
	if (meshInstance->_deformPosBuf.size() > 0)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)&meshInstance->_deformPosBuf[0]);
	}
	if (mesh->normalBuf.size() > 0)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, (GLfloat*)&mesh->normalBuf[0]);
	}


	if (mesh->texCoordBuf.size() > 0)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, (GLfloat*)&mesh->texCoordBuf[0]);
	}	

	if (mesh->tangentBuf.size() > 0)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat*)&mesh->tangentBuf[0]);
	}
#else // use vertex attribute array
	
	SbmDeformableMeshGPUInstance* gpuMeshInstance = (SbmDeformableMeshGPUInstance*)meshInstance;
	if (!gpuMeshInstance->getVBODeformPos())
		gpuMeshInstance->initBuffer();

	SrMat modelMat = SrMat::id;
	if (gpuMeshInstance->isStaticMesh())
	{
		modelMat = gpuMeshInstance->getSkeleton()->root()->gmat();
	}

	glUniformMatrix4fv(glGetUniformLocation(normalMapShader->getShaderProgram(), "uModelMatrix"), 1, GL_FALSE, (GLfloat*)&modelMat);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	SbmDeformableMeshGPU* gpuMesh = (SbmDeformableMeshGPU*)gpuMeshInstance->getDeformableMesh();
	//VBOVec3f* posVBO = gpuMeshInstance->getVBODeformPos();
	//posVBO->VBO()->UpdateWithData(meshInstance->_deformPosBuf);


	glEnableVertexAttribArray(0);
	gpuMeshInstance->getVBODeformPos()->VBO()->BindBuffer();
	//gpuMesh->getPosVBO()->VBO()->BindBuffer();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	gpuMeshInstance->getVBODeformNormal()->VBO()->BindBuffer();
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(2);
	gpuMesh->getTexCoordVBO()->VBO()->BindBuffer();
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(3);
	gpuMeshInstance->getVBODeformTangent()->VBO()->BindBuffer();
	//gpuMesh->getTangentVBO()->VBO()->BindBuffer();
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

//  	glEnableVertexAttribArray(4);
//  	gpuMesh->getBiNormalVBO()->VBO()->BindBuffer();
//  	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);

#endif
	GLint mtrlAmbientLoc, mtrlDiffuseLoc, mtrlShininessLoc;
	//if (!deferredRender)
	{
		mtrlAmbientLoc = glGetUniformLocation(shaderProgram->getShaderProgram(), "uMtrl.ambient");
		mtrlDiffuseLoc = glGetUniformLocation(shaderProgram->getShaderProgram(), "uMtrl.diffuse");
		mtrlShininessLoc = glGetUniformLocation(shaderProgram->getShaderProgram(), "uMtrl.shininess");
		//SmartBody::util::log("Ambient Loc = %d, diffuse Loc = %d, shininess Loc = %d", mtrlAmbientLoc, mtrlDiffuseLoc, mtrlShininessLoc);
		glUniform4f(mtrlAmbientLoc, 0.0, 0.0, 0.0, 1.0);
		glUniform4f(mtrlDiffuseLoc, 0.8, 0.8, 0.8, 1.0);
		glUniform1f(mtrlShininessLoc, 100.0);
	}

	

	std::vector<SbmSubMesh*>& subMeshList = mesh->subMeshList;
	std::vector<VBOVec3i*>& subMeshTris = gpuMesh->getVBOSubMeshTris();
	for (unsigned int i = 0; i < subMeshList.size(); i++)
	{
		SbmSubMesh* subMesh = subMeshList[i];	
		VBOVec3i* subMeshVBO = subMeshTris[i];
		glMaterial(subMesh->material);		
// 		if (subMesh->material.useAlphaBlend)
// 		{
// 			glDisable(GL_CULL_FACE);
// 		}
// 		else
// 		{
// 			glEnable(GL_CULL_FACE);
// 		}

		SmartBody::SBSkeleton* skel = meshInstance->getSkeleton();
		SmartBody::SBPawn* pawn = skel->getPawn();		
		
		SbmTexture* tex = texManager.findTexture(SbmTextureManager::TEXTURE_DIFFUSE, subMesh->texName.c_str());
		SbmTexture* normalTex = texManager.findTexture(SbmTextureManager::TEXTURE_NORMALMAP, subMesh->normalMapName.c_str());	
		SbmTexture* specularTex = texManager.findTexture(SbmTextureManager::TEXTURE_SPECULARMAP, subMesh->specularMapName.c_str());
		//SbmTexture* glossyTex = texManager.findTexture(SbmTextureManager::TEXTURE_SPECULARMAP, subMesh->specularMapName.c_str());

		if (!tex)
			tex = whiteTex;
		if (!normalTex)
		{
			//SmartBody::util::log("No normal texture name = %s", subMesh->normalMapName.c_str());
			normalTex = defaultNormalTex; // to use only original normal
		}

		if (!specularTex)
			specularTex = blackTex;

		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, tex->getID());		

		glActiveTexture(GL_TEXTURE1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, normalTex->getID());

		glActiveTexture(GL_TEXTURE2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, specularTex->getID());

		//if (!deferredRender)
		{
			float color[4];
			float shininess = subMesh->material.shininess;
			subMesh->material.diffuse.get(color);
			glUniform4f(mtrlDiffuseLoc, color[0], color[1], color[2], color[3]);
			glUniform1f(mtrlShininessLoc, shininess);
		}

		
		subMeshVBO->VBO()->BindBuffer();
		glDrawElements(GL_TRIANGLES, subMesh->triBuf.size() * 3, GL_UNSIGNED_INT, 0);
		subMeshVBO->VBO()->UnbindBuffer();
	}

	gpuMeshInstance->getVBODeformPos()->VBO()->UnbindBuffer();
	gpuMeshInstance->getVBODeformNormal()->VBO()->UnbindBuffer();
	gpuMesh->getTexCoordVBO()->VBO()->UnbindBuffer();
	//gpuMeshInstance->getVBODeformTangent()->VBO()->UnbindBuffer();
	gpuMesh->getTangentVBO()->VBO()->BindBuffer();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);	
}

void SBBaseRenderer::GPUBlendShapeUpdate(DeformableMeshInstance* meshInstance)
{
	SbmDeformableMeshGPUInstance* gpuMeshInstance = (SbmDeformableMeshGPUInstance*)meshInstance;
	SbmDeformableMeshGPU* gpuMesh = (SbmDeformableMeshGPU*)gpuMeshInstance->getDeformableMesh();

	if (!gpuMesh) return;

	if (!gpuMeshInstance->getVBODeformPos())
		gpuMeshInstance->initBuffer();
	// update blendshapes	
	gpuMeshInstance->blendShapeStaticMesh();
	gpuMeshInstance->gpuBlendShape(); // copy the static blendshape results to VBO buffer
}

