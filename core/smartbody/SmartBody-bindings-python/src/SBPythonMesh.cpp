
#include "SBPython.h"
#include <sbm/sbm_deformable_mesh.h>
#include "sb/SBRenderAssetManager.h"

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/args.hpp>


#if 0//defined(_MSC_FULL_VER) && (_MSC_FULL_VER == 190024210 || _MSC_FULL_VER == 190024215)
namespace boost
{
	template<> const volatile DeformableMesh* get_pointer(const volatile DeformableMesh* p) { return p; }
}
#endif


namespace SmartBody
{
	void pythonFuncsMesh()
	{
		boost::python::class_<SkinWeight>("SBSkinWeight")
			.def("getInfluenceJointNames", &SkinWeight::getInfluenceJointNames, "Returns an array of ordered joint names")
			.def("getNumVertices", &SkinWeight::getNumVertices, "Returns the number of vertices.")
			.def("getInfluenceJoints", &SkinWeight::getInfluenceJoints, "Returns a list of influences for a specified vertex index.")
			.def("getInfluenceWeights", &SkinWeight::getInfluenceWeights, "Returns a list of influence weights for a specified vertex index.")
			.def("getBindShape", &SkinWeight::getBindShape, "Returns the bind pose for the overall shape.")
			.def("getBindJoint", &SkinWeight::getBindJoint, "Returns the bind pose for a specified joint.")
			.def("createCache", &SkinWeight::createCache, "Creates a fast lookup for skin weights.")
			.def("getInfluencesByJointIndex", &SkinWeight::getInfluencesByJointIndex, "Returns vertex numbers affected by joint index.")
			;

		boost::python::class_<DeformableMesh, boost::python::bases<SBObject> >("SBMesh")
			.def(boost::python::init<>())
			.def("isSkinnedMesh", &DeformableMesh::isSkinnedMesh, "Whether the current mesh is static or skinned/deformable")
			.def("saveToSmb", &DeformableMesh::saveToSmb, "Save the static mesh into a binary file with extension .smb")
			.def("saveToDmb", &DeformableMesh::saveToDmb, "Save the dynamic mesh into a binary file with extension .dmb")
			.def("saveToDae", &DeformableMesh::saveToDae, "Save the dynamic mesh into a binary file with extension .dmb")
			.def("scale", &DeformableMesh::scale, "Scale mesh")
			.def("translate", &DeformableMesh::translate, "Translate mesh")
			.def("rotate", &DeformableMesh::rotate, "Rotate mesh")
			.def("centralize", &DeformableMesh::centralize, "Make the center of the mesh (0,0,0)")
			.def("computeNormals", &DeformableMesh::computeNormals, "Recompute normals.")
			.def("copySkinWeights", &DeformableMesh::copySkinWeights, "Copies the skin weights from the input mesh to this mesh.")
			.def("copyClosestSkinWeights", &DeformableMesh::copyClosestSkinWeights, "Copies the skin weights from the input mesh to this mesh.")
			.def("getNumSubmeshes", &DeformableMesh::getNumMeshes, "Returns the number of submeshes.")
			.def("getSkinWeight", &DeformableMesh::getSkinWeight, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the skin weight object from mesh name.")
			.def("getSkinWeightIndex", &DeformableMesh::getSkinWeightIndex, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the skin weight object from index.")
			.def("rebuildVertexBuffer", &DeformableMesh::rebuildVertexBuffer, "Rebuilds the vertex buffer.")

			;
	}
}

