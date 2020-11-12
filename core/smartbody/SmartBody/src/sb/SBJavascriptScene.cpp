
#include "SBJavascript.h"
#include "SBJavascriptClass.h"
#include "controllers/me_ct_scheduler2.h"
#include "controllers/me_ct_gaze.h"
#include "controllers/me_ct_eyelid.h"
#include "controllers/me_ct_curve_writer.hpp"
#include "SBFaceDefinition.h"
#include <sb/nvbg.h>
#include "SBBehavior.h"
#include <sb/SBMotion.h>
#include <sb/SBParseNode.h>

#include <sb/SBScene.h>
#include <sb/SBScript.h>
#include <sb/SBService.h>
#include <sb/SBServiceManager.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBAnimationState.h>
#include <sb/SBMotionBlendBase.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBAnimationTransitionRule.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBSteerManager.h>
#include <sb/SBFaceShiftManager.h>
#include <sb/SBPhysicsManager.h>
#include <sb/SBReach.h>
#include <sb/SBReachManager.h>
#include <sb/SBGestureMap.h>
#include <sb/SBGestureMapManager.h>
#include <sb/SBAssetManager.h>
#include <sb/SBJointMap.h>
#include <sb/SBJointMapManager.h>
#include <sb/SBParser.h>
#include <sb/SBBoneBusManager.h>
#include <sb/SBVHMsgManager.h>
#include <sb/SBCollisionManager.h>
#include <sb/SBSteerAgent.h>
#include <sb/SBPhoneme.h>
#include <sb/SBPhonemeManager.h>
#include <sb/SBBehaviorSet.h>
#include <sb/SBBehaviorSetManager.h>
#include <sb/SBRetarget.h>
#include <sb/SBRetargetManager.h>
#include <sb/SBEvent.h>
#include <sb/SBSceneListener.h>
#include <sb/SBNavigationMesh.h>
#include <sb/SBSceneListener.h>
#include <sb/SBNavigationMeshManager.h>
#include <sb/SBHandConfigurationManager.h>
#include <sb/SBMotionGraph.h>
#include <sr/sr_box.h>
#include <sr/sr_camera.h>
#include <stdlib.h>
#include <sbm/GenericViewer.h>
#include <controllers/me_ct_motion.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include <functional>

#if defined(EMSCRIPTEN)
#ifndef SB_NO_JAVASCRIPT
#include <emscripten/bind.h>
#include <emscripten.h>
#endif


#include "SBJavascriptInternal.h"


typedef std::map<std::string,SrQuat> QuatMap;
typedef std::map<std::string,SrVec> VecMap;
typedef std::map<std::string, std::string> StringMap;


namespace SmartBody
{ 

#ifndef SB_NO_JAVASCRIPT
	void javascriptFuncsScene()
	{
		emscripten::class_<SBNavigationMesh>("SBNavigationMesh")		
			.function("buildNavigationMesh", &SBNavigationMesh::buildNavigationMesh)		
			.function("findPath", &SBNavigationMesh::findPath)
			.function("queryMeshPointByRayCast", &SBNavigationMesh::queryMeshPointByRayCast)
			.function("queryFloorHeight", &SBNavigationMesh::queryFloorHeight)
			;

		emscripten::class_<SBScene, emscripten::base<SBObject>>("SBScene")
			.function("reset", &SBScene::reset)
			.function("update", &SBScene::update)
			.function("setProcessId", &SBScene::setProcessId)
			.function("getProcessId", &SBScene::getProcessId)
			.function("getStringFromObject", &SBScene::getStringFromObject, emscripten::allow_raw_pointers())
			.function("getObjectFromString", &SBScene::getObjectFromString, emscripten::allow_raw_pointers())
			.function("createCharacter", &SBScene::createCharacter, emscripten::allow_raw_pointers())
			.function("copyCharacter", &SBScene::copyCharacter, emscripten::allow_raw_pointers())
			.function("createPawn", &SBScene::createPawn, emscripten::allow_raw_pointers())
			.function("copyPawn", &SBScene::copyPawn, emscripten::allow_raw_pointers())
			.function("createFaceDefinition", &SBScene::createFaceDefinition, emscripten::allow_raw_pointers())
			.function("getFaceDefinition", &SBScene::getFaceDefinition, emscripten::allow_raw_pointers())
			.function("getNumFaceDefinitions", &SBScene::getNumFaceDefinitions)
			.function("getFaceDefinitionNames", &SBScene::getFaceDefinitionNames)
			.function("removeCharacter", &SBScene::removeCharacter)
			.function("removeAllCharacters", &SBScene::removeAllCharacters)
			.function("removePawn", &SBScene::removePawn)
			.function("removeAllPawns", &SBScene::removeAllPawns)
			.function("getNumPawns", &SBScene::getNumPawns)
			.function("getNumCharacters", &SBScene::getNumCharacters)
			.function("getPawn", &SBScene::getPawn, emscripten::allow_raw_pointers())
			.function("getCharacter", &SBScene::getCharacter, emscripten::allow_raw_pointers())
			.function("getPawnNames", &SBScene::getPawnNames)
			.function("getCharacterNames", &SBScene::getCharacterNames, emscripten::allow_raw_pointers())
			.function("getEventHandlerNames", &SBScene::getEventHandlerNames)
			.function("setMediaPath",&SBScene::setMediaPath)
			.function("getMediaPath",&SBScene::getMediaPath)
			.function("setDefaultCharacter", &SBScene::setDefaultCharacter)
			.function("setDefaultRecipient", &SBScene::setDefaultRecipient)
			.function("addSkeletonDefinition", &SBScene::addSkeletonDefinition, emscripten::allow_raw_pointers())
			.function("removeSkeletonDefinition", &SBScene::removeSkeletonDefinition)
			.function("createMotion", &SBScene::createMotion, emscripten::allow_raw_pointers())
			.function("addScript", &SBScene::addScript, emscripten::allow_raw_pointers())
			.function("removeScript", &SBScene::removeScript)
			.function("getNumScripts", &SBScene::getNumScripts)
			.function("getScriptNames", &SBScene::getScriptNames)
			.function("getScript", &SBScene::getScript, emscripten::allow_raw_pointers())
			.function("getScale", &SBScene::getScale)
			.function("setScale", &SBScene::setScale)
			.function("isRemoteMode", &SBScene::isRemoteMode)
			.function("setRemoteMode", &SBScene::setRemoteMode)
			.function("removePendingCommands", &SBScene::removePendingCommands)
			.function("addSceneListener", &SBScene::addSceneListener, emscripten::allow_raw_pointers())
			.function("removeSceneListener", &SBScene::removeSceneListener, emscripten::allow_raw_pointers())
			.function("removeSceneListener", &SBScene::addSceneListener, emscripten::allow_raw_pointers())
			.function("getSceneListeners", &SBScene::getSceneListeners, emscripten::allow_raw_pointers())
			.function("removeAllSceneListeners", &SBScene::removeAllSceneListeners)
			.function("save", &SBScene::save)
			.function("exportScene", &SBScene::exportScene)
			.function("setNavigationMesh", &SBScene::setNavigationMesh)
			.function("getNavigationMesh", &SBScene::getNavigationMesh, emscripten::allow_raw_pointers())
			.function("startFileLogging", &SBScene::startFileLogging)
			.function("stopFileLogging", &SBScene::stopFileLogging)

			// cameras
			.function("createCamera", &SBScene::createCamera, emscripten::allow_raw_pointers())
			.function("removeCamera", &SBScene::removeCamera, emscripten::allow_raw_pointers())
			.function("setActiveCamera", &SBScene::setActiveCamera, emscripten::allow_raw_pointers())
			.function("getActiveCamera", &SBScene::getActiveCamera, emscripten::allow_raw_pointers())
			.function("getCamera", &SBScene::getCamera, emscripten::allow_raw_pointers())
			.function("getNumCameras", &SBScene::getNumCameras)
			.function("getCameraNames", &SBScene::getCameraNames)

			// command processing
			.function("command", &SBScene::command)
			.function("commandAt", &SBScene::commandAt)
			.function("vhmsg", &SBScene::sendVHMsg)
			.function("vhmsg2", &SBScene::sendVHMsg2)
			.function("run", &SBScene::runScript)
			// managers
			.function("getEventManager", &SBScene::getEventManager, emscripten::allow_raw_pointers())
			.function("getSimulationManager", &SBScene::getSimulationManager, emscripten::allow_raw_pointers())
			.function("getProfiler", &SBScene::getProfiler, emscripten::allow_raw_pointers())
			.function("getBmlProcessor", &SBScene::getBmlProcessor, emscripten::allow_raw_pointers())
			.function("getStateManager", &SBScene::getBlendManager, emscripten::allow_raw_pointers())
			.function("getBlendManager", &SBScene::getBlendManager, emscripten::allow_raw_pointers())
			.function("getReachManager", &SBScene::getReachManager, emscripten::allow_raw_pointers())
			.function("getSteerManager", &SBScene::getSteerManager, emscripten::allow_raw_pointers())
			.function("getServiceManager", &SBScene::getServiceManager, emscripten::allow_raw_pointers())
			.function("getPhysicsManager", &SBScene::getPhysicsManager, emscripten::allow_raw_pointers())
			.function("getBoneBusManager", &SBScene::getBoneBusManager, emscripten::allow_raw_pointers())
			.function("getGestureMapManager", &SBScene::getGestureMapManager, emscripten::allow_raw_pointers())
			.function("getJointMapManager", &SBScene::getJointMapManager, emscripten::allow_raw_pointers())
			.function("getCollisionManager", &SBScene::getCollisionManager, emscripten::allow_raw_pointers())
			.function("getDiphoneManager", &SBScene::getDiphoneManager, emscripten::allow_raw_pointers())
			.function("getBehaviorSetManager", &SBScene::getBehaviorSetManager, emscripten::allow_raw_pointers())
			.function("getRetargetManager", &SBScene::getRetargetManager, emscripten::allow_raw_pointers())
			.function("getAssetManager", &SBScene::getAssetManager, emscripten::allow_raw_pointers())
			.function("getVHMsgManager", &SBScene::getVHMsgManager, emscripten::allow_raw_pointers())
			.function("getNavigationMeshManager", &SBScene::getNavigationMeshManager, emscripten::allow_raw_pointers())
			.function("getMotionGraphManager", &SBScene::getMotionGraphManager, emscripten::allow_raw_pointers())
			.function("getHandConfigurationManager", &SBScene::getHandConfigurationManager, emscripten::allow_raw_pointers())
			.function("getFaceShiftManager", &SBScene::getFaceShiftManager, emscripten::allow_raw_pointers())
			.function("getParser", &SBScene::getParser, emscripten::allow_raw_pointers())
			.class_function("setSystemParameter", &SBScene::setSystemParameter)
			.class_function("getSystemParameter", &SBScene::getSystemParameter)
			.class_function("removeSystemParameter", &SBScene::removeSystemParameter)
			.class_function("removeAllSystemParameters", &SBScene::removeSystemParameter)
			.class_function("getSystemParameterNames", &SBScene::getSystemParameterNames)

			// deprecated
			.function("createSkeleton", &SBScene::createSkeleton, emscripten::allow_raw_pointers())
			.function("getSkeleton", &SBScene::getSkeleton, emscripten::allow_raw_pointers())
			.function("addAssetPath", &SBScene::addAssetPath)
			.function("removeAssetPath", &SBScene::removeAssetPath)
			.function("removeAllAssetPaths", &SBScene::removeAllAssetPaths)
			.function("getAssetPaths", &SBScene::getAssetPaths)
			.function("getLocalAssetPaths", &SBScene::getLocalAssetPaths)
			.function("loadAssets", &SBScene::loadAssets)
			.function("loadAssetsFromPath", &SBScene::loadAssetsFromPath)
			.function("addMotions", &SBScene::addMotions)
			.function("addMotionDefinition", &SBScene::addMotionDefinition, emscripten::allow_raw_pointers())
			.function("getMotion", &SBScene::getMotion, emscripten::allow_raw_pointers())
			.function("getNumMotions", &SBScene::getNumMotions)
			.function("getMotionNames", &SBScene::getMotionNames)
			.function("getNumSkeletons", &SBScene::getNumSkeletons)
			.function("getSkeletonNames", &SBScene::getSkeletonNames)

			;
	}
#endif
}
#endif

