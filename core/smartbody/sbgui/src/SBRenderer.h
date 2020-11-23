#pragma once
#include <sbm/GPU/SbmShader.h>
#include <sbm/GPU/SbmTexture.h>
#include <sbm/sbm_deformable_mesh.h>
#include "SBFrameBufferObject.h"

#include <sr/sr_light.h>
#include "SBBaseRenderer.h"

namespace SmartBody {
class SBRenderScene;
}

class FltkViewerData;


class SBGBuffer : public SBFrameBufferObject
{
public:
	SBGBuffer();
	void initBuffer(int w, int h);
	void resize(int w, int h);	
protected:
	void createBufferTex(int w, int h, bool rebuild);

public:
	SbmTexture* posTex;
	SbmTexture* normalTex;
	SbmTexture* diffuseTex;
	SbmTexture* specularTex;
	SbmTexture* glossyTex;
	SbmTexture* depthTex;
	GLuint fboID;
};




class SBRenderer : public SBBaseRenderer
{
private:
	// for singleton
	static SBRenderer* _singleton;
	SBRenderer(void);
	~SBRenderer(void);
public:
	static SBRenderer& singleton();
	static void destroy_singleton();

	virtual void initRenderer(int w, int h);
	virtual void resize(int w, int h);
	virtual void draw(SmartBody::SBRenderScene& renderScene, std::vector<SrLight>& lights, bool isDrawFloor);
	

	SbmTexture* getCurEnvMap(bool diffuseMap = false);
	
	void initSSAO(int w, int h);

	void drawDeferredRendering(SmartBody::SBRenderScene& renderScene, std::vector<SrLight>& lights, bool isDrawFloor);
	void drawForwardRendering(SmartBody::SBRenderScene& renderScene, std::vector<SrLight>& lights, bool isDrawFloor);
	void drawSSAOPass();	
	void drawLightPass(std::vector<SrLight>& lights);
	void drawIBLPass(std::vector<SrLight>& lights);
	

	void registerGUI();


	void drawTextureQuadWithDepth(SbmTexture* tex, SbmTexture* depthTex);
	void drawDebugFBO();
protected:	
	

	SbmShaderProgram* gbufferShader;
	SBGBuffer gbuffer;	
	SbmShaderProgram* lightPassShader; // compute lighting and combine with ambient occlusion
	SbmShaderProgram* iblShader; // compute lighting and combine with ambient occlusion	
	SbmShaderProgram* depthQuadShader;

	SBFrameBufferObject lightPassFBO;
	SbmTexture* lightPassOutput;

	SbmShaderProgram* ssaoShader;
	SbmShaderProgram* ssaoBlurShader;
	SBFrameBufferObject ssaoFBO;
	SBFrameBufferObject ssaoBlurFBO;
	SbmTexture* ssaoNoise;	
	SbmTexture* ssaoOutput;
	SbmTexture* ssaoBlurOutput;
	std::vector<SrVec> ssaoKernel;
	int ssaoNoiseSize;
	int ssaoKernelSize;
};

