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


#include "GL/glew.h"
#include "SBRenderer.h"
#include "fltk_viewer.h"

#include <sb/SBObject.h>
#include <sb/SBAttribute.h>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include <sb/SBPawn.h>
#include <sb/SBCharacter.h>
#include "SBUtilities.h"
#include <sr/sr_camera.h>
#include <sr/sr_mat.h>
#include <sr/sr_random.h>

#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <sbm/GPU/VBOData.h>

#include <algorithm>
#include <boost/lexical_cast.hpp>

#include "sbm/SBRenderScene.h"
#include "Session.h"


SBRenderer* SBRenderer::_singleton = nullptr;

SBRenderer::SBRenderer()
{
	rendererInitialized = false;
}


SBRenderer::~SBRenderer() = default;

SBRenderer& SBRenderer::singleton()
{
	if (!_singleton)
		_singleton = new SBRenderer();
	return *_singleton;
}

void SBRenderer::destroy_singleton()
{
	delete _singleton;
	_singleton = nullptr;
}


void SBRenderer::drawDebugFBO()
{
	std::string gbufferDebug = Session::current->scene.getStringAttribute("Renderer.gbufferDebug");
	if (gbufferDebug == "" || gbufferDebug == "none")
		return;
	std::shared_ptr<SbmTexture> debugTex;
	if (gbufferDebug == "posTex")
		debugTex = gbuffer.posTex;
	else if (gbufferDebug == "normalTex")
		debugTex = gbuffer.normalTex;
	else if (gbufferDebug == "diffuseTex")
		debugTex = gbuffer.diffuseTex;
	else if (gbufferDebug == "specularTex")
		debugTex = gbuffer.specularTex;
	else if (gbufferDebug == "glossyTex")
		debugTex = gbuffer.glossyTex;
	else if (gbufferDebug == "depthTex")
		debugTex = gbuffer.depthTex;
	else if (gbufferDebug == "ssaoTex")
		debugTex = ssaoOutput;
	else if (gbufferDebug == "ssaoBlurTex")
		debugTex = ssaoBlurOutput;
	else if (gbufferDebug == "lightTex")
		debugTex = lightPassOutput;
	else if (gbufferDebug == "envMapTex")
	{
		debugTex = getCurEnvMap();
	}
	else
		debugTex = gbuffer.posTex; // draw posTex by default

	if (!debugTex)
	{		
		return;
	}

	//SBFrameBufferObject::drawTextureQuad(debugTex);
	drawTextureQuadWithDepth(debugTex.get(), gbuffer.depthTex.get());
}

std::shared_ptr<SbmTexture> SBRenderer::getCurEnvMap(bool diffuseMap)
{
	std::string texName = Session::current->scene.getStringAttribute("Renderer.envMapName");
	if (diffuseMap)
		texName = Session::current->scene.getStringAttribute("Renderer.envDiffuseMapName");
	SbmTextureManager& texManager = SbmTextureManager::singleton();
	auto debugTex = texManager.findTexture(texName.c_str());
	if (!debugTex)
		SmartBody::util::log("Error ! Can't find tex name = %s", texName.c_str());		
	return debugTex;
}

void SBRenderer::initRenderer(int w, int h)
{
	SBBaseRenderer::initRenderer(w, h);
	width = w;
	height = h;
	gbuffer.initBuffer(w, h);

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

	std::string shaderPath = scene->getMediaPath() + "/shaders/";
	SbmShaderManager& shaderManager = SbmShaderManager::singleton();
	shaderManager.addShader("gbuffer_shader", shaderPath + "gbuffer.vert", shaderPath + "gbuffer.frag", true);
	gbufferShader = shaderManager.getShader("gbuffer_shader");

	SbmTextureManager& texManager = SbmTextureManager::singleton();
	lightPassFBO.initFBO("lightPassFBO");
	lightPassOutput = texManager.createTexture(SbmTextureManager::TEXTURE_RENDER_TARGET, "lightPassOutput");
	lightPassOutput->createEmptyTexture(w, h, 4, GL_FLOAT);
	lightPassOutput->buildTexture(false, true);
	lightPassFBO.attachTexture(lightPassOutput.get(), GL_COLOR_ATTACHMENT0);
	lightPassFBO.setDrawBufferDefault();

	shaderManager.addShader("lightPass_shader", shaderPath + "lighting.vert", shaderPath + "lighting.frag", true);
	lightPassShader = shaderManager.getShader("lightPass_shader");

	shaderManager.addShader("ibl_shader", shaderPath + "ibl.vert", shaderPath + "ibl.frag", true);
	iblShader = shaderManager.getShader("ibl_shader");

	shaderManager.addShader("depthQuad_shader", shaderPath + "depthQuad.vert", shaderPath + "depthQuad.frag", true);
	depthQuadShader = shaderManager.getShader("depthQuad_shader");
	texManager.updateEnvMaps();

	initSSAO(w, h);
	rendererInitialized = true;
}

void SBRenderer::initSSAO(int w, int h)
{
	// create ssao render target
	SbmTextureManager& texManager = SbmTextureManager::singleton();	
	ssaoFBO.initFBO("ssaoFBO");

	ssaoOutput = texManager.createTexture(SbmTextureManager::TEXTURE_RENDER_TARGET, "ssaoOutput");
	ssaoOutput->createEmptyTexture(w, h, 4, GL_FLOAT);
	ssaoOutput->buildTexture(false, true);
	ssaoFBO.attachTexture(ssaoOutput.get(), GL_COLOR_ATTACHMENT0);
	ssaoFBO.setDrawBufferDefault();

	ssaoBlurFBO.initFBO("ssaoBlurFBO");
	ssaoBlurOutput = texManager.createTexture(SbmTextureManager::TEXTURE_RENDER_TARGET, "ssaoBlurOutput");
	ssaoBlurOutput->createEmptyTexture(w, h, 4, GL_FLOAT);
	ssaoBlurOutput->buildTexture(false, true);
	ssaoBlurFBO.attachTexture(ssaoBlurOutput.get(), GL_COLOR_ATTACHMENT0);
	ssaoBlurFBO.setDrawBufferDefault();

	SrRandom random;
	random.seed(clock());
	ssaoNoiseSize = 4;
	ssaoKernelSize = 30;
	
	// create ssaoKernel
	ssaoKernel.resize(ssaoKernelSize);
	for (unsigned int i = 0; i < ssaoKernel.size(); i++)
	{
		// randomly sample a point inside a unit hemi-sphere
		ssaoKernel[i] = SrVec(random.get(-1.f, 1.f), random.get(-1.f, 1.f), random.get(0.f, 1.f));
		ssaoKernel[i].normalize();
		// favor sample points that are closer to the vertex position, rescale the samples based on a distance cut-off curve
		float ratio = (float)i / (float)ssaoKernel.size();
		float rr = ratio*ratio;
		float kernelWeight = 0.1f*(1.f - rr) + 1.f*rr;
		ssaoKernel[i] *= kernelWeight;
		SmartBody::util::log("Kernel %d, vec = %s", i, ssaoKernel[i].toString().c_str());
	}
	
	

	// create ssao noise texture
	int noiseSqr = ssaoNoiseSize*ssaoNoiseSize;
	ssaoNoise = texManager.createTexture(SbmTextureManager::TEXTURE_RENDER_TARGET, "ssaoNoise");
	ssaoNoise->createEmptyTexture(ssaoNoiseSize, ssaoNoiseSize, 3, GL_FLOAT);	
	std::vector<SrVec> noiseTexData(noiseSqr);	
	for (auto & i : noiseTexData)
	{
		i = SrVec(random.get(-1.f, 1.f), random.get(-1.f, 1.f), 0.f);
		i.normalize();
	}
	ssaoNoise->setBuffer((unsigned char*)&noiseTexData[0], noiseTexData.size() * sizeof(SrVec));
	ssaoNoise->buildTexture(false, true);

	

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::string shaderPath = scene->getMediaPath() + "/shaders/";
	SbmShaderManager& shaderManager = SbmShaderManager::singleton();
	shaderManager.addShader("ssao_shader", shaderPath + "ssao.vert", shaderPath + "ssao.frag", true);
	ssaoShader = shaderManager.getShader("ssao_shader");
	ssaoShader->buildShader();

	shaderManager.addShader("ssaoBlur_shader", shaderPath + "blur.vert", shaderPath + "blur.frag", true);
	ssaoBlurShader = shaderManager.getShader("ssaoBlur_shader");
	ssaoBlurShader->buildShader();


	glUseProgram(ssaoShader->getShaderProgram());
	glUniform1i(glGetUniformLocation(ssaoShader->getShaderProgram(), "uKernelSize"), ssaoKernelSize);
	glUniform3fv(glGetUniformLocation(ssaoShader->getShaderProgram(), "uKernelOffsets"), ssaoKernelSize, (GLfloat*)&ssaoKernel[0]);
	glUseProgram(0);	


	glUseProgram(ssaoBlurShader->getShaderProgram());
	glUniform1i(glGetUniformLocation(ssaoBlurShader->getShaderProgram(), "uBlurSize"), ssaoNoiseSize);
	glUseProgram(0);
}



void SBRenderer::resize(int w, int h)
{
	gbuffer.resize(w, h);
	if (ssaoOutput)
	{
		ssaoOutput->createEmptyTexture(w, h, 4, GL_FLOAT);
		ssaoOutput->buildTexture(false, false);
	}

	if (ssaoBlurOutput)
	{
		ssaoBlurOutput->createEmptyTexture(w, h, 4, GL_FLOAT);
		ssaoBlurOutput->buildTexture(false, false);
	}

	if (lightPassOutput)
	{
		lightPassOutput->createEmptyTexture(w, h, 4, GL_FLOAT);
		lightPassOutput->buildTexture(false, false);
	}
	
	width = w;
	height = h;
}

void SBRenderer::draw(SmartBody::SBRenderScene& renderScene,  std::vector<SrLight>& lights, bool isDrawFloor)
{
	if (!isInitialized()) return;

	this->drawDeferredRendering(renderScene, lights, isDrawFloor);
}

void SBRenderer::drawDeferredRendering(SmartBody::SBRenderScene& renderScene, std::vector<SrLight>& lights, bool isDrawFloor)
{	
	// update skinning transform

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::vector<std::string> pawnNames = scene->getPawnNames();
#if 1
	for (auto& entry : renderScene.getRenderables()) {
		auto& renderable = entry.second;
		DeformableMeshInstance* meshInstance = renderable.getActiveMesh();
		if (meshInstance)
		{
			if (!meshInstance->isStaticMesh())
			{
				auto* gpuMeshInstance = dynamic_cast<SbmDeformableMeshGPUInstance*>(meshInstance);
				GPUMeshUpdate(meshInstance);
			}
		}
	}

#endif
	gbuffer.bindFBO();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(gbufferShader->getShaderProgram());	
	SrCamera* cam = renderScene.getActiveCamera();
	// if there is no active camera, then only show the blank screen
	if (!cam)
		return;	

	SrMat mat, modelViewMat, modelViewProjMat;
	//----- Set Projection ----------------------------------------------
	cam->setAspectRatio((float)width / (float)height);

	glViewport(0, 0, width, height);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#if 0
	glMatrixMode(GL_PROJECTION);
	glLoadMatrix(cam->get_perspective_mat(mat));

	//----- Set Visualisation -------------------------------------------
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrix(cam->get_view_mat(mat));
	glScalef(cam->getScale(), cam->getScale(), cam->getScale());
#endif
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

	cam->get_view_mat(modelViewMat);
	cam->get_perspective_mat(modelViewProjMat);
	modelViewProjMat = modelViewMat*modelViewProjMat;

	glUniformMatrix4fv(glGetUniformLocation(gbufferShader->getShaderProgram(), "modelViewMat"), 1, GL_FALSE, (GLfloat*)&modelViewMat);
	glUniformMatrix4fv(glGetUniformLocation(gbufferShader->getShaderProgram(), "modelViewProjMat"), 1, GL_FALSE, (GLfloat*)&modelViewProjMat);


	for (auto& entry : renderScene.getRenderables()) {
		auto& renderable = entry.second;
		DeformableMeshInstance* meshInstance = renderable.getActiveMesh();

		float alphaThreshold = (float)renderable.pawn->getDoubleAttribute("alphaThreshold");
		glUniform1f(glGetUniformLocation(gbufferShader->getShaderProgram(), "alphaThreshold"), alphaThreshold);
		if (meshInstance)
		{
			renderMesh(meshInstance, gbufferShader, true);
		}
	}

	if (isDrawFloor)
		drawFloor();
	glUseProgram(0);
	gbuffer.unbindFBO();
	std::string gbufferDebug = Session::current->scene.getStringAttribute("Renderer.gbufferDebug");
	drawSSAOPass(renderScene);
	//drawLightPass(lights);
	drawIBLPass(renderScene, lights);

	drawDebugFBO();
}


void SBRenderer::drawForwardRendering(SmartBody::SBRenderScene& renderScene, std::vector<SrLight>& lights, bool isDrawFloor)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

	std::vector<std::string> pawnNames = scene->getPawnNames();
#if 1

	for (auto& entry : renderScene.getRenderables()) {
		auto& renderable = entry.second;
		DeformableMeshInstance* meshInstance = renderable.getActiveMesh();
		if (meshInstance)
		{
			if (!meshInstance->isStaticMesh())
			{
				auto* gpuMeshInstance = dynamic_cast<SbmDeformableMeshGPUInstance*>(meshInstance);
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




	SrCamera* cam = renderScene.getActiveCamera();
	// if there is no active camera, then only show the blank screen
	if (!cam)
		return;

	SrMat mat, modelViewMat, modelViewProjMat;
	//----- Set Projection ----------------------------------------------
	cam->setAspectRatio((float)width / (float)height);
	glViewport(0, 0, width, height);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	cam->get_view_mat(modelViewMat);
	cam->get_perspective_mat(modelViewProjMat);
	modelViewProjMat = modelViewMat*modelViewProjMat;

	glUniformMatrix4fv(glGetUniformLocation(normalMapShader->getShaderProgram(), "uMVMatrix"), 1, GL_FALSE, (GLfloat*)&modelViewMat);
	glUniformMatrix4fv(glGetUniformLocation(normalMapShader->getShaderProgram(), "uMVPMatrix"), 1, GL_FALSE, (GLfloat*)&modelViewProjMat);


	for (auto& entry : renderScene.getRenderables()) {
		auto& renderable = entry.second;
		DeformableMeshInstance* meshInstance = renderable.getActiveMesh();
		if (meshInstance)
		{
			renderMesh(meshInstance, gbufferShader, true);
		}
	}
	if (isDrawFloor)
		drawFloor();
	glUseProgram(0);
}

void SBRenderer::drawLightPass(SmartBody::SBRenderScene& renderScene, std::vector<SrLight>& lights)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SrCamera* cam = renderScene.getActiveCamera();
	// if there is no active camera, then only show the blank screen
	if (!cam)
		return;

	lightPassFBO.bindFBO();	
	glUseProgram(lightPassShader->getShaderProgram());

	SrMat viewMat;
	cam->get_view_mat(viewMat);
	viewMat = viewMat.get_rotation();
	std::vector<SrVec> lightDirs, lightColors;
	float lcolor[4];
	// update light direction
	for (auto & light : lights)
	{
		SrVec ldir = light.position*viewMat;
		ldir.normalize();
		lightDirs.emplace_back(ldir);
		light.diffuse.get(lcolor);
		lightColors.emplace_back(SrVec(lcolor[0], lcolor[1], lcolor[2]));
		//lightColors.emplace_back(SrVec(1,1,1));
	}
	
	glUniform1i(glGetUniformLocation(lightPassShader->getShaderProgram(), "numLights"), lights.size());
	glUniform3fv(glGetUniformLocation(lightPassShader->getShaderProgram(), "lightDir"), lights.size(), (GLfloat*)&lightDirs[0]);
	glUniform3fv(glGetUniformLocation(lightPassShader->getShaderProgram(), "lightColor"), lights.size(), (GLfloat*)&lightColors[0]);

	glViewport(0, 0, width, height);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// binding the textures
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbuffer.normalTex->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ssaoBlurOutput->getID());
	//glBindTexture(GL_TEXTURE_2D, gbuffer.posTex->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gbuffer.diffuseTex->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);



	SBFrameBufferObject::drawScreenQuad();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);	


	glUseProgram(0);
	lightPassFBO.unbindFBO();	
}



void SBRenderer::drawIBLPass(SmartBody::SBRenderScene& renderScene, std::vector<SrLight>& lights)
{
	std::string texName = Session::current->scene.getStringAttribute("Renderer.envMapName");
	SbmTextureManager& texManager = SbmTextureManager::singleton();
	auto envMap = texManager.findTexture(texName.c_str());
	if (!envMap) return;

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SrCamera* cam = renderScene.getActiveCamera();
	// if there is no active camera, then only show the blank screen
	if (!cam)
		return;

	lightPassFBO.bindFBO();
	glUseProgram(iblShader->getShaderProgram());

	SrMat viewMat;
	cam->get_view_mat(viewMat);
	viewMat = viewMat.get_rotation();
	SrMat invViewMat = viewMat.inverse();

	float tanHalfFov = tanf(cam->getFov() / 2.f);
	float aspectRatio = cam->getAspectRatio();

	auto curEnvMap = getCurEnvMap(false);
	auto curEnvDiffuseMap = getCurEnvMap(true);
	GLuint envMapTexID = -1, envDiffuseTexId = -1;
	if (curEnvMap)
		envMapTexID = curEnvMap->getID();
	if (curEnvDiffuseMap)
		envDiffuseTexId = curEnvDiffuseMap->getID();

	float roughness = 0.f;
	float metallic = 0.f;
	
	roughness = scene->getDoubleAttribute("Renderer.roughness");
	roughness = std::min(1.f, std::max(roughness, 0.f));
	metallic = scene->getDoubleAttribute("Renderer.metallic");
	metallic = std::min(1.f, std::max(metallic, 0.f));

	float exposure = 1.f;
	float gamma = 1.f;
	exposure = scene->getDoubleAttribute("Renderer.exposure");
	exposure = std::min(100.f, std::max(exposure, 0.f));
	//gamma = scene->getDoubleAttribute("Renderer.gamma");
	//gamma = std::min(10.f, std::max(gamma, 0.f));

	// update lights to shaders
	std::vector<SrVec> lightDirs, lightColors;
	float lcolor[4];
	// update light direction
	for (auto & light : lights)
	{
		SrVec ldir = light.position;
		ldir.normalize();
		lightDirs.emplace_back(ldir);
		light.diffuse.get(lcolor);
		lightColors.emplace_back(lcolor[0], lcolor[1], lcolor[2]);
	}

	glUniform1i(glGetUniformLocation(iblShader->getShaderProgram(), "showEnvMap"), scene->getBoolAttribute("Renderer.showEnvMap") ? 1 : 0);
	glUniform1i(glGetUniformLocation(iblShader->getShaderProgram(), "numLights"), lights.size());
	glUniform3fv(glGetUniformLocation(iblShader->getShaderProgram(), "lightDir"), lights.size(), (GLfloat*)&lightDirs[0]);
	glUniform3fv(glGetUniformLocation(iblShader->getShaderProgram(), "lightColor"), lights.size(), (GLfloat*)&lightColors[0]);
	

	glUniformMatrix4fv(glGetUniformLocation(iblShader->getShaderProgram(), "uViewToWorldMatrix"), 1, GL_FALSE, (GLfloat*)&invViewMat);
 	glUniform1f(glGetUniformLocation(iblShader->getShaderProgram(), "uTanHalfFov"), tanHalfFov);
	glUniform1f(glGetUniformLocation(iblShader->getShaderProgram(), "uAspectRatio"), aspectRatio);
	glUniform1f(glGetUniformLocation(iblShader->getShaderProgram(), "roughnessVal"), roughness);
	glUniform1f(glGetUniformLocation(iblShader->getShaderProgram(), "metallic"), metallic);

	glUniform1f(glGetUniformLocation(iblShader->getShaderProgram(), "exposure"), exposure);
	//glUniform1f(glGetUniformLocation(iblShader->getShaderProgram(), "gamma"), gamma);
	//glUniform1f(glGetUniformLocation(iblShader->getShaderProgram(), "shininess"), shininess);

	glViewport(0, 0, width, height);
	float bgColor[4];
	backgroundColor.get(bgColor);
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// binding the textures
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbuffer.normalTex->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, envMapTexID);
	//glBindTexture(GL_TEXTURE_2D, gbuffer.posTex->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, envDiffuseTexId);
	//glBindTexture(GL_TEXTURE_2D, gbuffer.posTex->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gbuffer.diffuseTex->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, ssaoBlurOutput->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, gbuffer.specularTex->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);



	SBFrameBufferObject::drawScreenQuad();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, 0);


	glUseProgram(0);
	lightPassFBO.unbindFBO();

}

void SBRenderer::drawSSAOPass(SmartBody::SBRenderScene& renderScene)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SrCamera* cam = renderScene.getActiveCamera();
	// if there is no active camera, then only show the blank screen
	if (!cam)
		return;

	ssaoFBO.bindFBO();
	float tanHalfFov = tanf(cam->getFov()/2.f);
	float aspectRatio = cam->getAspectRatio();
	float radius = scene->getDoubleAttribute("Renderer.ssaoRadius");
	float power = scene->getDoubleAttribute("Renderer.ssaoPower");
	glUseProgram(ssaoShader->getShaderProgram());

	SrMat projMatrix;
	cam->get_perspective_mat(projMatrix);
	//SmartBody::util::log("Proj Mat = %s", projMatrix.toString().c_str());
	glUniformMatrix4fv(glGetUniformLocation(ssaoShader->getShaderProgram(), "uProjectionMatrix"), 1, GL_FALSE, (GLfloat*)&projMatrix);
	glUniform1f(glGetUniformLocation(ssaoShader->getShaderProgram(), "uTanHalfFov"), tanHalfFov);
	glUniform1f(glGetUniformLocation(ssaoShader->getShaderProgram(), "uAspectRatio"), aspectRatio);
	glUniform1f(glGetUniformLocation(ssaoShader->getShaderProgram(), "farPlane"), cam->getFarPlane());
	glUniform1f(glGetUniformLocation(ssaoShader->getShaderProgram(), "nearPlane"), cam->getNearPlane());

	glUniform1f(glGetUniformLocation(ssaoShader->getShaderProgram(), "uRadius"), radius);
	glUniform1f(glGetUniformLocation(ssaoShader->getShaderProgram(), "uPower"), power);

	glViewport(0, 0, width, height);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// binding the textures
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbuffer.normalTex->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gbuffer.depthTex->getID());
	//glBindTexture(GL_TEXTURE_2D, gbuffer.posTex->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ssaoNoise->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#if 1
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gbuffer.diffuseTex->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#endif
	

	SBFrameBufferObject::drawScreenQuad();
#if 0
	glBegin(GL_QUADS);	
#if 1
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
#else
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
#endif	
	glEnd();
#endif
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);


	glUseProgram(0);
	ssaoFBO.unbindFBO();


	ssaoBlurFBO.bindFBO();
	glUseProgram(ssaoBlurShader->getShaderProgram());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssaoOutput->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SBFrameBufferObject::drawScreenQuad();
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	ssaoBlurFBO.unbindFBO();
}



void SBRenderer::registerGUI()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

	std::vector<std::string> GBufferOptions;
	GBufferOptions.emplace_back("none");
	GBufferOptions.emplace_back("posTex");
	GBufferOptions.emplace_back("normalTex");
	GBufferOptions.emplace_back("diffuseTex");
	GBufferOptions.emplace_back("specularTex");
	GBufferOptions.emplace_back("glossyTex");
	GBufferOptions.emplace_back("depthTex");
	GBufferOptions.emplace_back("ssaoTex");
	GBufferOptions.emplace_back("ssaoBlurTex");
	GBufferOptions.emplace_back("lightTex");
	GBufferOptions.emplace_back("envMapTex");

	SmartBody::StringAttribute* gbufferOptionsAttr = scene->createStringAttribute("Renderer.gbufferDebug", "none", true, "Renderer", 60, false, false, false, "Debug gbuffer");
	gbufferOptionsAttr->setValidValues(GBufferOptions);

	scene->createStringAttribute("Renderer.envMapName", "noname", true, "Renderer", 60, false, false, false, "Name of env texture for background and IBL lighting");
	scene->createStringAttribute("Renderer.envDiffuseMapName", "noname", true, "Renderer", 60, false, false, false, "Name of diffuse convoluted env texture for background and IBL lighting");

		
	SmartBody::DoubleAttribute* ssaoRadiusAttr = scene->createDoubleAttribute("Renderer.ssaoRadius", 0.15, true, "Renderer", 60, false, false, false, "SSAO Kernel Size");
	SmartBody::DoubleAttribute* ssaoPowerAttr = scene->createDoubleAttribute("Renderer.ssaoPower", 1.0, true, "Renderer", 60, false, false, false, "SSAO Kernel Size");

	SmartBody::DoubleAttribute* roughnessAttr = scene->createDoubleAttribute("Renderer.roughness", 0.0, true, "Renderer", 60, false, false, false, "SSAO Kernel Size");
	SmartBody::DoubleAttribute* metallicAttr = scene->createDoubleAttribute("Renderer.metallic", 0.0, true, "Renderer", 60, false, false, false, "SSAO Kernel Size");
	SmartBody::DoubleAttribute* exposureAttr = scene->createDoubleAttribute("Renderer.exposure", 5.0, true, "Renderer", 60, false, false, false, "SSAO Kernel Size");
	SmartBody::BoolAttribute* showEnvMap = scene->createBoolAttribute("Renderer.showEnvMap", true, true, "Renderer", 60, false, false, false, "SSAO Kernel Size");

	//SmartBody::DoubleAttribute* gammaAttr = scene->createDoubleAttribute("Renderer.gamma", 1.0, true, "Renderer", 60, false, false, false, "SSAO Kernel Size");



	//SmartBody::DoubleAttribute* roughnessAttr = scene->createDoubleAttribute("Renderer.shininess", 0.0, true, "Renderer", 60, false, false, false, "SSAO Kernel Size");
}



void SBRenderer::drawTextureQuadWithDepth(SbmTexture* tex, SbmTexture* depthTex)
{
	if (!tex || !depthTex) return;

	glUseProgram(depthQuadShader->getShaderProgram());
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glColor3f(1.f, 1.f, 1.f);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, tex->getID());

	glActiveTexture(GL_TEXTURE1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, depthTex->getID());

	//drawScreenQuad();
	SBFrameBufferObject::drawScreenQuad();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	glEnable(GL_LIGHTING);
}

SBGBuffer::SBGBuffer()
{
	posTex = normalTex = diffuseTex = specularTex = glossyTex = depthTex = nullptr;
}

void SBGBuffer::initBuffer(int w, int h)
{
	SmartBody::util::log("Init SBGBuffer");
	initFBO("gbuffer_fbo");

	SbmTextureManager& texManager = SbmTextureManager::singleton();
	posTex = texManager.createTexture(SbmTextureManager::TEXTURE_RENDER_TARGET, "gbuffer_posTex");
	normalTex = texManager.createTexture(SbmTextureManager::TEXTURE_RENDER_TARGET, "gbuffer_normalTex");
	diffuseTex = texManager.createTexture(SbmTextureManager::TEXTURE_RENDER_TARGET, "gbuffer_diffuseTex");
	specularTex = texManager.createTexture(SbmTextureManager::TEXTURE_RENDER_TARGET, "gbuffer_specularTex");
	glossyTex = texManager.createTexture(SbmTextureManager::TEXTURE_RENDER_TARGET, "gbuffer_glossyTex");
	depthTex = texManager.createTexture(SbmTextureManager::TEXTURE_RENDER_TARGET, "gbuffer_depthTex");

	createBufferTex(w, h, true);	
	attachTexture(posTex.get(), GL_COLOR_ATTACHMENT0);
	attachTexture(normalTex.get(), GL_COLOR_ATTACHMENT1);
	attachTexture(diffuseTex.get(), GL_COLOR_ATTACHMENT2);
	attachTexture(specularTex.get(), GL_COLOR_ATTACHMENT3);
	attachTexture(glossyTex.get(), GL_COLOR_ATTACHMENT4);
	attachTexture(depthTex.get(), GL_DEPTH_ATTACHMENT);
	setDrawBufferDefault();	
}

void SBGBuffer::resize(int w, int h)
{
	//SmartBody::util::log("Resize SBGBuffer");
	createBufferTex(w, h, false);
}


void SBGBuffer::createBufferTex(int w, int h, bool rebuild)
{
	if (!posTex)
		return;
	
	posTex->createEmptyTexture(w, h, 3, GL_FLOAT);
	normalTex->createEmptyTexture(w, h, 3, GL_FLOAT);
	diffuseTex->createEmptyTexture(w, h, 4, GL_FLOAT);
	specularTex->createEmptyTexture(w, h, 4, GL_FLOAT);
	glossyTex->createEmptyTexture(w, h, 3, GL_FLOAT);
	depthTex->createEmptyTexture(w, h, 1, GL_FLOAT);

	posTex->buildTexture(false, rebuild);
	normalTex->buildTexture(false, rebuild);
	diffuseTex->buildTexture(false, rebuild);
	specularTex->buildTexture(false, rebuild);
	glossyTex->buildTexture(false, rebuild);
	depthTex->buildTexture(false, rebuild);
}

