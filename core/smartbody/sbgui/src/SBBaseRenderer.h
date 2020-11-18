#pragma once
#include <sbm/GPU/SbmShader.h>
#include <sbm/GPU/SbmTexture.h>
#include <sbm/sbm_deformable_mesh.h>
#include <sr/sr_light.h>
#include "sbm/Heightfield.h"


class SBBaseRenderer
{
private:
	static SBBaseRenderer* _singleton;
public:
	static SBBaseRenderer& singleton();
	static void destroy_singleton();

	SBBaseRenderer(void);
	~SBBaseRenderer(void);
	virtual void draw(std::vector<SrLight>& lights, bool isDrawFloor);
	virtual void resize(int w, int h);
	virtual void initRenderer(int w, int h);
	void renderMesh(DeformableMeshInstance* meshInstance, SbmShaderProgram* shaderProgram, bool deferredRender = true);
	bool isInitialized() { return rendererInitialized;  }
	void resetRenderer();
	void setBackgroundColor(SrColor& bg) { backgroundColor = bg;  }

	void render(Heightfield& heightfield, int renderMode) const;

protected:
	void drawFloor(bool deferrredRender = true);
	void GPUMeshUpdate(DeformableMeshInstance* meshInstance);
	void GPUBlendShapeUpdate(DeformableMeshInstance* meshInstance);
protected:
	SbmShaderProgram* skinningShader;
	SbmShaderProgram* normalMapShader;
	int width, height;
	SrColor backgroundColor;
	bool rendererInitialized;
};

