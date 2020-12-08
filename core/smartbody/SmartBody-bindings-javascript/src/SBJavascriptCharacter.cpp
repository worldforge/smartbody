
#include "controllers/me_ct_scheduler2.h"
#include "controllers/me_ct_gaze.h"
#include "controllers/me_ct_eyelid.h"
#include "controllers/me_ct_curve_writer.hpp"
#include "controllers/me_ct_motion.h"
#include "controllers/me_ct_new_locomotion.h"

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
#include <sb/SBPhysicsManager.h>
#include <sb/SBReach.h>
#include <sb/SBReachManager.h>
#include <sb/SBGestureMap.h>
#include <sb/SBGestureMapManager.h>
#include <sb/SBVHMsgManager.h>
#include <sb/SBAssetManager.h>
#include <sb/SBJointMap.h>
#include <sb/SBJointMapManager.h>
#include <sb/SBParser.h>
#include <sb/SBBoneBusManager.h>
#include <sb/SBCollisionManager.h>
#include <sb/SBFaceShiftManager.h>
#include <sb/SBSteerAgent.h>
#include <sb/SBPhoneme.h>
#include <sb/SBPhonemeManager.h>
#include <sb/SBBehaviorSet.h>
#include <sb/SBBehaviorSetManager.h>
#include <sb/SBRetarget.h>
#include <sb/SBRetargetManager.h>
//#include <sb/SBNavigationMesh.h>
//#include <sb/SBNavigationMeshManager.h>
#include <sb/SBHandConfigurationManager.h>
#include <sb/SBHandConfiguration.h>
#include <sb/SBEvent.h>
#include <sb/SBSceneListener.h>
#include <sb/SBFaceDefinition.h>
#include <sb/nvbg.h>
#include <sb/SBBehavior.h>
#include <sb/SBMotion.h>
#include <sb/SBParseNode.h>
#include <sb/sbm_pawn.hpp>
#include <sb/SBRigNode.h>

#include <sbm/GenericViewer.h>

#include <sr/sr_box.h>
#include <sr/sr_camera.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include <stdlib.h>
#include <string>
#include <malloc.h>
#include <functional>

#if defined(EMSCRIPTEN)
#ifndef SB_NO_JAVASCRIPT
#include <emscripten/bind.h>
#include <emscripten.h>

#endif
#include "SBJavascriptInternal.h"
#include "SBJavascriptClass.h"

typedef std::map<std::string,SrQuat> QuatMap;
typedef std::map<std::string,SrVec> VecMap;
typedef std::map<std::string, std::string> StringMap;

namespace SmartBody
{
#ifndef SB_NO_JAVASCRIPT
	void javascriptFuncsCharacter()
	{
		//need to add this additional part to expose getBoundingBox function
		//also we need to explicitly specify its base class
		emscripten::class_<SbmPawn, emscripten::base<SBObject>>("SbmPawn")
			.function("getBoundingBox", &SbmPawn::getBoundingBox)
			.function("getHeight", &SbmPawn::getHeight)
			.function("setHeight", &SbmPawn::setHeight);
		;
		//SBPawn is directly inherinted from sbm_pawn not sure if I could change to it...
		emscripten::class_<SBPawn, emscripten::base<SbmPawn>>("SBPawn")
			.function("getSkeleton", &SBPawn::getSkeleton, emscripten::allow_raw_pointers())
			.function("setSkeleton", &SBPawn::setSkeleton, emscripten::allow_raw_pointers())
			.function("setName", &SBPawn::setName)
			.function("getPosition", &SBPawn::getPosition)
			.function("getOrientation", &SBPawn::getOrientation)
			.function("setPosition", &SBPawn::setPosition)
			.function("setPositionSmooth", &SBPawn::setPositionSmooth)
			.function("setOrientation", &SBPawn::setOrientation)
			.function("setHPR", &SBPawn::setHPR)
			.function("getHPR", &SBPawn::getHPR)
			.function("setHPRSmooth", &SBPawn::setHPRSmooth)
			;

		emscripten::class_<SrCamera, emscripten::base<SBPawn>>("Camera")
			.function("print", &SrCamera::print)
			.function("reset", &SrCamera::reset)
			.function("setEye", &SrCamera::setEye)
			.function("getEye", &SrCamera::getEye)
			.function("setCenter", &SrCamera::setCenter)
			.function("getCenter", &SrCamera::getCenter)
			.function("setScale", &SrCamera::setScale)
			.function("getScale", &SrCamera::getScale)
			.function("setUpVector", &SrCamera::setUpVector)
			.function("getUpVector", &SrCamera::getUpVector)
			.function("setFov", &SrCamera::setFov)
			.function("getFov", &SrCamera::getFov)
			.function("setNearPlane", &SrCamera::setNearPlane)
			.function("getNearPlane", &SrCamera::getNearPlane)
			.function("setFarPlane", &SrCamera::setFarPlane)
			.function("getFarPlane", &SrCamera::getFarPlane)
			.function("setAspectRatio", &SrCamera::setAspectRatio)
			.function("getAspectRatio", &SrCamera::getAspectRatio)
			.function("setCameraParameterSmooth", &SrCamera::setCameraParameterSmooth)
			.function("apply_translation_from_mouse_motion", &SrCamera::apply_translation_from_mouse_motion)
			.function("setAdditionalXForm", &SrCamera::setAdditionalXForm)
			.function("getAdditionalXForm", &SrCamera::getAdditionalXForm)
			.function("view_all", &SrCamera::view_all); 
			;
		
		emscripten::class_<SbmCharacter, emscripten::base<SBPawn>>("SbmCharacter")
			.function("setSoftEyes", &SbmCharacter::setSoftEyes)
			.function("isSoftEyes", &SbmCharacter::isSoftEyes)
			.function("createStandardControllers", &SbmCharacter::createStandardControllers)
			.function("createMinimalControllers", &SbmCharacter::createMinimalControllers)
			.function("setNvbg", &SbmCharacter::setNvbg, emscripten::allow_raw_pointers())
			.function("getNvbg", &SbmCharacter::getNvbg, emscripten::allow_raw_pointers())
			.function("setMiniBrain", &SbmCharacter::setMiniBrain, emscripten::allow_raw_pointers())
			.function("getMiniBrain", &SbmCharacter::getMiniBrain, emscripten::allow_raw_pointers())
			.function("updateFaceDefinition", &SbmCharacter::updateFaceDefinition)
			;
		emscripten::class_<SBCharacter, emscripten::base<SbmCharacter>>("SBCharacter")
			.function("addJointTrajectoryConstraint", &SBCharacter::addJointTrajectoryConstraint)
			.function("isAutomaticPruning", &SBCharacter::isAutomaticPruning)
			.function("setAutomaticPruning", &SBCharacter::setAutomaticPruning)
			.function("pruneControllers", &SBCharacter::pruneControllers)
			.function("startMotionRecord", &SBCharacter::startMotionRecord)
			.function("stopMotionRecord", &SBCharacter::stopMotionRecord)
			.function("writeMotionRecord", &SBCharacter::writeMotionRecord)
			.function("setUseVisemeCurves", &SBCharacter::setUseVisemeCurves)
			.function("isUseVisemeCurves", &SBCharacter::isUseVisemeCurves)
			.function("setVisemeTimeOffset", &SBCharacter::setVisemeTimeOffset)
			.function("getVisemeTimeOffset", &SBCharacter::getVisemeTimeOffset)
			.function("getNumControllers", &SBCharacter::getNumControllers)
			.function("addController", &SBCharacter::addController, emscripten::allow_raw_pointers())
			.function("removeController", &SBCharacter::removeController, emscripten::allow_raw_pointers())
			.function("getControllerByIndex", &SBCharacter::getControllerByIndex, emscripten::allow_raw_pointers())
			.function("getControllerByName", &SBCharacter::getControllerByName, emscripten::allow_raw_pointers())
			.function("getControllerNames", &SBCharacter::getControllerNames)
			.function("setVoice", &SBCharacter::setVoice)
			.function("setVoiceCode", &SBCharacter::setVoiceCode)
			.function("setVoiceBackup", &SBCharacter::setVoiceBackup)
			.function("setVoiceBackupCode", &SBCharacter::setVoiceBackupCode)
			.function("getVoice", &SBCharacter::getVoice)
			.function("getVoiceCode", &SBCharacter::getVoiceBackupCode)
			.function("getVoiceBackup", &SBCharacter::getVoiceBackup)
			.function("setFaceDefinition", &SBCharacter::setFaceDefinition, emscripten::allow_raw_pointers())
			.function("getFaceDefinition", &SBCharacter::getFaceDefinition, emscripten::allow_raw_pointers())
			.function("getNumBehaviors", &SBCharacter::getNumBehaviors)
			.function("getBehavior", &SBCharacter::getBehavior, emscripten::allow_raw_pointers())
			.function("interruptFace", &SBCharacter::interruptFace)
			.function("hasSpeechBehavior", &SBCharacter::hasSpeechBehavior)
			.function("createBlendShapeChannel", &SBCharacter::createBlendShapeChannel)
			.function("setDeformableMeshName", &SBCharacter::setDeformableMeshName)
			.function("setMotionGraph", &SBCharacter::setMotionGraph)
			.function("startMotionGraph", &SBCharacter::startMotionGraph)
			.function("startMotionGraphRandomWalk", &SBCharacter::startMotionGraphRandomWalk)
			.function("startMotionGraphWithPath", &SBCharacter::startMotionGraphWithPath)
			.function("updateDefaultFacePose", &SBCharacter::updateDefaultFacePose)
			.function("getReach", &SBCharacter::getReach, emscripten::allow_raw_pointers())
			.function("setReach", &SBCharacter::setReach, emscripten::allow_raw_pointers())
			.function("getReachAttachedPawnName", &SBCharacter::getReachAttachedPawnName)
			;


		emscripten::class_<SBSetDrivenKeyNode>("SBSetDrivenKeyNode")
			.constructor()
			.function("setRange", &SBSetDrivenKeyNode::setRange)
			.function("getValue", &SBSetDrivenKeyNode::getValue)
			;

		emscripten::class_<SBSetDrivenKeySequenceNode>("SBSetDrivenKeySequenceNode")
			.constructor()
			.function("setRange", &SBSetDrivenKeySequenceNode::setRange)
			.function("getValue", &SBSetDrivenKeySequenceNode::getValue)
			;

		
	}
#endif
}
#endif