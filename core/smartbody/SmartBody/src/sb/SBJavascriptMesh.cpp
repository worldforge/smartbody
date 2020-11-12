
#include "SBJavascript.h"
#include <sbm/sbm_deformable_mesh.h>
#include <functional>
#if defined(EMSCRIPTEN)
#include <emscripten/bind.h>
#include <emscripten.h>


namespace SmartBody
{
#ifndef SB_NO_JAVASCRIPT
	void javascriptFuncsMesh()
	{

		emscripten::class_<DeformableMesh, emscripten::base<SBObject>>("SBMesh")
			.constructor()
			.function("isSkinnedMesh", &DeformableMesh::isSkinnedMesh)
			.function("saveToSmb", &DeformableMesh::saveToSmb)
			.function("saveToDmb", &DeformableMesh::saveToDmb)
			.function("scale", &DeformableMesh::scale)
			.function("translate", &DeformableMesh::translate)
			.function("rotate", &DeformableMesh::rotate)
			.function("centralize", &DeformableMesh::centralize)
			.function("computeNormals", &DeformableMesh::computeNormals)
			.function("copySkinWeights", &DeformableMesh::copySkinWeights, emscripten::allow_raw_pointers())
			.function("copyClosestSkinWeights", &DeformableMesh::copyClosestSkinWeights, emscripten::allow_raw_pointers())
			.function("getNumMeshes", &DeformableMesh::getNumMeshes)
			.function("getBoundingBox", &DeformableMesh::computeBoundingBox)
			;
	}
#endif
}
#endif