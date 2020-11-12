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

#pragma once
#include <sb/SBTypes.h>


#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/gtc/type_ptr.hpp"
#include "external/glm/glm/gtc/matrix_transform.hpp"

#if !defined(__FLASHPLAYER__) && !defined(__ANDROID__) && !defined(SB_IPHONE) && !defined(EMSCRIPTEN)
#include "external/glew/glew.h"
#include "TBOData.h"
#include <sbm/sbm_deformable_mesh.h>
#include "SbmShader.h"
#include "VBOData.h"
#elif defined(__ANDROID__) || defined(SB_IPHONE)
#include <sbm/sbm_deformable_mesh.h>
#include "SbmShader.h"
#endif

class SbmDeformableMeshGPU;
#if !defined(__ANDROID__) && !defined(SB_IPHONE) && !defined(EMSCRIPTEN)
class SbmBlendFace: public DeformableMesh
{
	public:
		SbmBlendFace();
		~SbmBlendFace();

		bool			buildVertexBufferGPU(int);
		void			addFace(SbmDeformableMeshGPU*);
		void			addFace(SrSnModel* newFace);

		void addFaceVertices( std::vector<SrVec> vertices );

		void			initShaderProgram();
		void			initShaderProgram_Dan();
		void			initShader();
		
		void			setDeformableMesh(DeformableMesh*);
		DeformableMesh* getDeformableMesh();
		
		VBOVec3f*		getVBOPos(int);
		VBOVec3f*		getVBONormal();
		VBOVec2f*		getVBOTexCoord();
		VBOVec3i*		getVBOTri();
		
		std::vector<VBOVec3i*> subMeshTris;

		GLuint			_vsID;
		GLuint			_fsID;
		GLuint			_programID;

	private:
		DeformableMesh*			_mesh;
		std::vector<VBOVec3f*>	_VBOPos;	
		VBOVec3f*				_VBONormal;
		VBOVec2f*				_VBOTexCoord;
		VBOVec3i*				_VBOTri;

		bool			_initGPUVertexBuffer;
		
		std::string		_shaderName;

		unsigned int	_faceCounter;				// How many faces have been loaded
	

};
#endif

#if !defined(__ANDROID__) && !defined(SB_IPHONE) && !defined(EMSCRIPTEN)
class SbmBlendTextures
{
	public:
		SbmBlendTextures();
		~SbmBlendTextures();

		static GLuint getShader(const std::string);
		static void BlendTwoFBO(GLuint, GLuint, GLuint, GLuint, float, GLuint, int, int);
		static void BlendAllAppearances(GLuint, GLuint, std::vector<float>, std::vector<GLuint>, GLuint, int, int);
		static void BlendAllAppearancesPairwise(GLuint *, GLuint *, std::vector<float>, std::vector<GLuint>,  std::vector<std::string>, GLuint, int, int);
		static void BlendGeometry(GLuint * FBODst, std::vector<float> weights, std::vector<GLuint> texIDs, std::vector<std::string> texture_names, DeformableMeshInstance* meshInstance/*_mesh*/, GLuint program);
		
		static void ReadMasks(GLuint * FBODst, GLuint * texDst, std::vector<float> weights, std::vector<GLuint> texIDs, std::vector<std::string>& texture_names, std::vector<std::string>& textureFileNames, GLuint program, int w, int h);
		static void BlendGeometryWithMasks(GLuint * FBODst, std::vector<float> weights, GLuint * texIDs, std::vector<std::string> texture_names, DeformableMeshInstance* meshInstance, GLuint program, glm::mat4x4 translation, glm::mat4x4 rotation);

		static void BlendGeometryWithMasksFeedback(GLuint * FBODst, std::vector<float> weights, GLuint * texIDs, std::vector<std::string> texture_names, DeformableMeshInstance* meshInstance, GLuint program, glm::mat4x4 translation, glm::mat4x4 rotation);
		static void BlendTextureWithMasks(GLuint FBODst, GLuint FBOTex, std::vector<float> weights, GLuint * texIDs, std::vector<std::string> texture_names, DeformableMeshInstance* meshInstance, GLuint program, int width, int height);
		static void RenderGeometryWithMasks(GLuint * FBODst, std::vector<float> weights, GLuint * texIDs, std::vector<std::string> texture_names, DeformableMeshInstance* meshInstance, GLuint program, glm::mat4x4 translation, glm::mat4x4 rotation);

		GLuint			_vsID;
		GLuint			_fsID;
		GLuint			_programID;

	private:
		std::string		_shaderName;
};
#elif defined(__ANDROID__) || defined(SB_IPHONE)
class SbmBlendTextures
{
public:
	SbmBlendTextures();
	~SbmBlendTextures();

	static GLuint getShader(const std::string);
	static void BlendAllAppearancesPairwise(GLuint *, GLuint *, std::vector<float>, std::vector<GLuint>,  std::vector<std::string>, GLuint, int, int);
	GLuint			_vsID;
	GLuint			_fsID;
	GLuint			_programID;

private:
	std::string		_shaderName;
};
#endif
