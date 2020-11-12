
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
	void javascriptFuncsMotion()
	{
		emscripten::class_<SBMotion, emscripten::base<SBObject>>("SBMotion")
			//.function(emscripten::init<std::string>())
			.function("addChannel", &SBMotion::addChannel)
			.function("addFrame", &SBMotion::addFrame)
			.function("addKeyFrameChannel", &SBMotion::addKeyFrameChannel)
			.function("addKeyFrameQuat", &SBMotion::addKeyFrameQuat)
			.function("bakeFrames", &SBMotion::bakeFrames)
			.function("getMotionFileName", &SBMotion::getMotionFileName)
			.function("getNumFrames", &SBMotion::getNumFrames)
			.function("getFrameData", &SBMotion::getFrameData)
			.function("getFrameSize", &SBMotion::getFrameSize)
			.function("getNumChannels", &SBMotion::getNumChannels)
			.function("getChannels", &SBMotion::getChannels)
			.function("checkSkeleton", &SBMotion::checkSkeleton)
			.function("connect", &SBMotion::connect, emscripten::allow_raw_pointers())
			.function("disconnect", &SBMotion::disconnect)
			.function("mirror", &SBMotion::mirror, emscripten::allow_raw_pointers())
			.function("getOffset", &SBMotion::getOffset, emscripten::allow_raw_pointers())
			.function("mirrorChildren", &SBMotion::mirrorChildren, emscripten::allow_raw_pointers())
			.function("retarget", &SBMotion::retarget, emscripten::allow_raw_pointers())
			.function("footSkateCleanUp", &SBMotion::footSkateCleanUp, emscripten::allow_raw_pointers())
			.function("constrain", &SBMotion::constrain, emscripten::allow_raw_pointers())
			.function("smoothCycle", &SBMotion::smoothCycle, emscripten::allow_raw_pointers())
			.function("alignToEnd", &SBMotion::alignToEnd)
			.function("alignToBegin", &SBMotion::alignToBegin)
			.function("duplicateCycle", &SBMotion::duplicateCycle, emscripten::allow_raw_pointers())
			.function("getJointSpeed", &SBMotion::getJointSpeed, emscripten::allow_raw_pointers())
			.function("getJointSpeedAxis", &SBMotion::getJointSpeedAxis, emscripten::allow_raw_pointers())
			.function("getJointAngularSpeed", &SBMotion::getJointAngularSpeed, emscripten::allow_raw_pointers())		
			.function("getJointAngularSpeedAxis", &SBMotion::getJointAngularSpeedAxis, emscripten::allow_raw_pointers())		
			.function("getJointTransition", &SBMotion::getJointTransition, emscripten::allow_raw_pointers())		
			.function("getJointPosition", &SBMotion::getJointPosition, emscripten::allow_raw_pointers())		
			.function("translate", &SBMotion::translate)		
			.function("rotate", &SBMotion::rotate)			
			.function("scale", &SBMotion::scale)		
			.function("trim", &SBMotion::trim)	
			.function("downsample", &SBMotion::downsample)	
			.function("removeMotionChannelsByEndJoints", &SBMotion::removeMotionChannelsByEndJoints)	
			.function("pertainMotionChannelsByEndJoints", &SBMotion::pertainMotionChannelsByEndJoints)	
			.function("removeMotionChannels", &SBMotion::removeMotionChannels)	
			.function("saveToSkm", &SBMotion::saveToSkm)	
			.function("saveToBVH", &SBMotion::saveToBVH)	
			.function("saveToSkmb", &SBMotion::saveToSkb)	
			.function("saveToSkmByFrames", &SBMotion::saveToSkmByFrames)	
			.function("getTimeStart", &SBMotion::getTimeStart)
			.function("getTimeReady", &SBMotion::getTimeReady)
			.function("getTimeStrokeStart", &SBMotion::getTimeStrokeStart)
			.function("getTimeStroke", &SBMotion::getTimeStroke)
			.function("getTimeStrokeEnd", &SBMotion::getTimeStrokeEnd)
			.function("getTimeRelax", &SBMotion::getTimeRelax)
			.function("getTimeStop", &SBMotion::getTimeStop)	
			.function("getDuration", &SBMotion::getDuration)
			.function("setSyncPoint", &SBMotion::setSyncPoint)
			.function("addEvent", &SBMotion::addEvent)
			.function("addMetaData", &SBMotion::addMetaData)
			.function("buildJointTrajectory", &SBMotion::buildJointTrajectory)
			.function("removeMetaData", &SBMotion::removeMetaData)
			.function("getMetaDataString", &SBMotion::getMetaDataString)
			.function("getMetaDataDouble", &SBMotion::getMetaDataDouble)
			.function("getMetaDataTags", &SBMotion::getMetaDataTags)
			.function("getMotionSkeletonName", &SBMotion::getMotionSkeletonName)
			.function("setMotionSkeletonName", &SBMotion::setMotionSkeletonName)
			.function("setEmptyMotion", &SBMotion::setEmptyMotion)
			.function("addSimilarPose", &SBMotion::addSimilarPose)
			.function("removeSimilarPose", &SBMotion::removeSimilarPose)
			.function("getSimilarPoses", &SBMotion::getSimilarPoses)
			.function("unrollPrerotation", &SBMotion::unrollPrerotation)		
			.function("addTemporalRotationOffset", &SBMotion::addTemporalRotationOffset)		
			;

		emscripten::class_<SBMotionNode>("SBMotionNode")
			.function("getName", &SBMotionNode::getName)
			.function("getAnimBlend", &SBMotionNode::getAnimBlend, emscripten::allow_raw_pointers())
			;

		emscripten::class_<SBMotionTransitionEdge>("SBMotionTransitionEdge")
			.function("getName", &SBMotionTransitionEdge::getName)
			.function("getSrcNode", &SBMotionTransitionEdge::getSrcNode, emscripten::allow_raw_pointers())
			.function("getTgtNode", &SBMotionTransitionEdge::getTgtNode, emscripten::allow_raw_pointers())
			;

		emscripten::class_<SBMotionGraph>("SBMotionGraph")
			.function("addMotionNodeFromBlend", &SBMotionGraph::addMotionNodeFromBlend, emscripten::allow_raw_pointers())
			.function("addMotionEdge", &SBMotionGraph::addMotionEdge, emscripten::allow_raw_pointers())
			.function("getMotionNode", &SBMotionGraph::getMotionNode, emscripten::allow_raw_pointers())
			.function("getMotionEdge", &SBMotionGraph::getMotionEdge, emscripten::allow_raw_pointers())
			.function("buildAutomaticMotionGraph", &SBMotionGraph::buildAutomaticMotionGraph)
			;
		emscripten::class_<SBMotionGraphManager>("SBMotionGraphManager")
			.function("createMotionGraph", &SBMotionGraphManager::createMotionGraph, emscripten::allow_raw_pointers())
			.function("getMotionGraph", &SBMotionGraphManager::getMotionGraph, emscripten::allow_raw_pointers())
			.function("getMotionGraphNames", &SBMotionGraphManager::getMotionGraphNames)
			;	

	}
#endif
}
#endif
