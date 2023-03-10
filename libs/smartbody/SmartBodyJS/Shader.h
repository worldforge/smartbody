#ifndef __SHADER_H__
#define __SHADER_H__

#include "esUtil.h"
#include "sbm/sbm_deformable_mesh.h"
#include "sb/SBSkeleton.h"
#include <Emscripten/emscripten.h>
#include <emscripten/bind.h>
#if __cplusplus
extern "C"
{
#endif

#define SHADER_API

	GLuint SHADER_API esLoadShader ( GLenum type, const char *shaderSrc );
	GLuint SHADER_API esLoadProgram ( const char *vertShaderSrc, const char *fragShaderSrc );

	void SHADER_API shaderInit(ESContext *esContext); 
	void SHADER_API drawLights(ESContext *esContext);
	void SHADER_API drawMesh(DeformableMeshInstance *shape, ESContext *esContext, bool showSkinWeight);
	void SHADER_API drawSphere(ESContext *esContext, SrVec pos, float r, int p, SrVec color);
	void SHADER_API drawBox(ESContext *esContext, SrVec pos, SrVec extent, SrVec color);
	void SHADER_API drawBBox(ESContext *esContext, SrVec pos, SrVec extent);
	void SHADER_API drawGrid(ESContext *esContext);
	void SHADER_API drawSkeleton(ESContext *esContext);
	void SHADER_API drawCoordinates(ESContext *esContext);
	void SHADER_API drawSkyBox(ESContext *esContext);
	void SHADER_API createTextureCubeMap(GLuint &textureId);
#if __cplusplus
}
#endif

#endif