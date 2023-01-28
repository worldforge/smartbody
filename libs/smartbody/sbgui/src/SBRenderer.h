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
	std::shared_ptr<SbmTexture> posTex;
	std::shared_ptr<SbmTexture> normalTex;
	std::shared_ptr<SbmTexture> diffuseTex;
	std::shared_ptr<SbmTexture> specularTex;
	std::shared_ptr<SbmTexture> glossyTex;
	std::shared_ptr<SbmTexture> depthTex;
	GLuint fboID;
};




class SBRenderer : public SBBaseRenderer
{
private:
	// for singleton
	static SBRenderer* _singleton;
	SBRenderer();
	~SBRenderer();
public:
	static SBRenderer& singleton();
	static void destroy_singleton();

	void initRenderer(int w, int h) override;
	void resize(int w, int h) override;
	virtual void draw(SmartBody::SBRenderScene& renderScene, std::vector<SrLight>& lights, bool isDrawFloor);


	std::shared_ptr<SbmTexture> getCurEnvMap(bool diffuseMap = false);
	
	void initSSAO(int w, int h);

	void drawDeferredRendering(SmartBody::SBRenderScene& renderScene, std::vector<SrLight>& lights, bool isDrawFloor);
	void drawForwardRendering(SmartBody::SBRenderScene& renderScene, std::vector<SrLight>& lights, bool isDrawFloor);
	void drawSSAOPass(SmartBody::SBRenderScene& renderScene);
	void drawLightPass(SmartBody::SBRenderScene& renderScene, std::vector<SrLight>& lights);
	void drawIBLPass(SmartBody::SBRenderScene& renderScene, std::vector<SrLight>& lights);
	

	void registerGUI();


	void drawTextureQuadWithDepth(SbmTexture* tex, SbmTexture* depthTex);
	void drawDebugFBO();
protected:	
	

	SbmShaderProgram* gbufferShader{};
	SBGBuffer gbuffer;	
	SbmShaderProgram* lightPassShader{}; // compute lighting and combine with ambient occlusion
	SbmShaderProgram* iblShader{}; // compute lighting and combine with ambient occlusion
	SbmShaderProgram* depthQuadShader{};

	SBFrameBufferObject lightPassFBO;
	std::shared_ptr<SbmTexture> lightPassOutput;

	SbmShaderProgram* ssaoShader{};
	SbmShaderProgram* ssaoBlurShader{};
	SBFrameBufferObject ssaoFBO;
	SBFrameBufferObject ssaoBlurFBO;
	std::shared_ptr<SbmTexture> ssaoNoise;
	std::shared_ptr<SbmTexture> ssaoOutput;
	std::shared_ptr<SbmTexture> ssaoBlurOutput;
	std::vector<SrVec> ssaoKernel;
	int ssaoNoiseSize{};
	int ssaoKernelSize{};
};

