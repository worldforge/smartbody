
#include "SBRendererGLPython.h"
#include "sb/SBTypes.h"

#include "sb/SBRenderAssetManager.h"
#include "sbm/SBRenderScene.h"
#include "sbm/sbm_deformable_mesh.h"

#include "sr/sr_camera.h"

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/args.hpp>
#include <string>
#include <boost/python.hpp>

SBAPI void initPythonRenderer() {
	boost::python::class_<SmartBody::SBRenderScene , boost::noncopyable>("SBRenderScene", boost::python::no_init)
			// cameras
			.def("createCamera", &SmartBody::SBRenderScene::createCamera, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Creates a camera with a given name and returns it.")
			.def("removeCamera", &SmartBody::SBRenderScene::removeCamera, "Removes a camera.")
			.def("setActiveCamera", &SmartBody::SBRenderScene::setActiveCamera, "Sets the camera to be used in the viewer.")
			.def("getActiveCamera", &SmartBody::SBRenderScene::getActiveCamera, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Gets the camera currently being used in the viewer.")
			.def("getCamera", &SmartBody::SBRenderScene::getCamera, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns a camera by name.")
			.def("getNumCameras", &SmartBody::SBRenderScene::getNumCameras, "Returns the number of cameras available.")
			.def("getCameraNames", &SmartBody::SBRenderScene::getCameraNames, "Gets the names of all the cameras available.")	;

	boost::python::class_<SmartBody::SBRenderAssetManager, boost::noncopyable>("SBRenderAssetManager", boost::python::no_init)
			//.def("addMesh", &SmartBody::SBRenderAssetManager::addMesh, "Adds a mesh to the scene.")
			.def("removeMesh", &SmartBody::SBRenderAssetManager::removeMesh, "Removes a mesh from the scene.")
			.def("removeDeformableMesh", &SmartBody::SBRenderAssetManager::removeDeformableMesh, "Removes a mesh from the scene.")
			.def("getMeshNames", &SmartBody::SBRenderAssetManager::getMeshNames, "Returns a list of all mesh names.\n Input: NULL \nOutput: list of skeleton names")
			.def("getDeformableMesh", &SmartBody::SBRenderAssetManager::getDeformableMesh, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return a deformable mesh given name")
			.def("getMesh", &SmartBody::SBRenderAssetManager::getDeformableMesh, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Return a deformable mesh given name")
			.def("createMeshFromBlendMasks", &SmartBody::SBRenderAssetManager::createMeshFromBlendMasks, "Writes out a mesh and texture based on: neutralShapeFile, neutralTextureFile, expressiveShapeFile, expressiveTextureFile, maskTextureFile, outputMeshFile, outputTextureFile")
			.def("addModelToMesh", &SmartBody::SBRenderAssetManager::addModelToMesh, "Adds a submesh to a model.")
			.def("addBlendshapeToModel", &SmartBody::SBRenderAssetManager::addBlendshapeToModel, "Adds a blendshape to a mesh, or replaces the blendshape within the mesh.")
			.def("handlePenetrations", &SmartBody::SBRenderAssetManager::handlePenetrations, "Handles penetrations within a mesh given a base model and a penetrative model.");

	boost::python::class_<SrCamera, boost::python::bases<SmartBody::SBPawn>, boost::noncopyable >("Camera", boost::python::no_init)
			.def("print", &SrCamera::print, "Shows all the camera statistics. ")
			.def("reset", &SrCamera::reset, "Reset camera with camera eye (0 166 185), camera center (0 92 0). ")
			.def("setEye", &SrCamera::setEye, "Set camera eye position. \n Input: camera eye position(should only have three number in the input list) e.g. [0, 0, 0] \n Output: NULL")
			.def("getEye", &SrCamera::getEye, "Get camera eye position.")
			.def("setCenter", &SrCamera::setCenter, "Set camera center. \n Input: camera center position(should only have three number in the input list) e.g. [0, 0, 0] \n Output: NULL")
			.def("getCenter", &SrCamera::getCenter, "Get camera center.")
			.def("setScale", &SrCamera::setScale, "Set camera scale. \n camera scale: NULL \n Output: NULL")
			.def("getScale", &SrCamera::getScale, "Get camera scale.")
			.def("setUpVector", &SrCamera::setUpVector, "Set camera up vector.")
			.def("getUpVector", &SrCamera::getUpVector, "Returns the camera up vector.")
			.def("setFov", &SrCamera::setFov, "Set's the camera's field of view.")
			.def("getFov", &SrCamera::getFov, "Get's the camera's field of view.")
			.def("setNearPlane", &SrCamera::setNearPlane, "Set's the camera's near plane.")
			.def("getNearPlane", &SrCamera::getNearPlane, "Get's the camera's near plane.")
			.def("setFarPlane", &SrCamera::setFarPlane, "Set's the camera's far plane.")
			.def("getFarPlane", &SrCamera::getFarPlane, "Get's the camera's far plane.")
			.def("setAspectRatio", &SrCamera::setAspectRatio, "Set's the camera's aspect ratio.")
			.def("getAspectRatio", &SrCamera::getAspectRatio, "Get's the camera's aspect ratio.")
			.def("setCameraParameterSmooth", &SrCamera::setCameraParameterSmooth, "Smoothly interpolate the current camera to target camera within given time.")
			;
}