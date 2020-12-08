
#include "SBJavascript.h"
#include "SBJavascriptClass.h"
#include "controllers/me_ct_scheduler2.h"
#include "controllers/me_ct_gaze.h"
#include "controllers/me_ct_eyelid.h"
#include "controllers/me_ct_curve_writer.hpp"
#include "controllers/me_ct_motion.h"

#include <sb/SBFaceDefinition.h>
#include <sb/nvbg.h>
#include <sb/SBBehavior.h>
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


typedef std::map<std::string,SrQuat> QuatMap;
typedef std::map<std::string,SrVec> VecMap;
typedef std::map<std::string, std::string> StringMap;

namespace SmartBody
{
#ifndef SB_NO_JAVASCRIPT
	void javascriptFuncsAnimation()
	{
		emscripten::class_<SBAnimationBlend>("SBAnimationBlend")
			.function("addCorrespondencePoints", &SBAnimationBlend::addCorrespondencePoints)
			.function("addCorrespondancePoints", &SBAnimationBlend::addCorrespondencePoints)
			.function("setIncrementWorldOffsetY", &SBAnimationBlend::setIncrementWorldOffsetY)
			.function("getNumMotions", &SBAnimationBlend::getNumMotions)
			.function("getMotion", &SBAnimationBlend::getMotion)
			.function("getMotionObject", &SBAnimationBlend::getSBMotion, emscripten::allow_raw_pointers())
			.function("getNumCorrespondancePoints", &SBAnimationBlend::getNumCorrespondencePoints)
			.function("getNumCorrespondencePoints", &SBAnimationBlend::getNumCorrespondencePoints)
			.function("getCorrespondancePoints", &SBAnimationBlend::getCorrespondencePoints)
			.function("getCorrespondencePoints", &SBAnimationBlend::getCorrespondencePoints)
			.function("setCorrespondencePoints", &SBAnimationBlend::setCorrespondencePoints)
			.function("getDimension", &SBAnimationBlend::getDimension)
			.function("addEvent", &SBAnimationBlend::addEvent)
			.function("removeAllEvents", &SBAnimationBlend::removeAllEvents)
			.function("getNumEvents", &SBAnimationBlend::getNumEvents)
			.function("getEvent", &SBAnimationBlend::getEvent, emscripten::allow_raw_pointers())
			.function("removeEvent", &SBAnimationBlend::removeEvent) 
			//.function("buildVisSurfaces", &SBAnimationBlend::buildVisSurfaces)
			.function("createMotionVectorFlow", &SBAnimationBlend::createMotionVectorFlow)
			.function("clearMotionVectorFlow", &SBAnimationBlend::clearMotionVectorFlow)
			.function("plotMotion", &SBAnimationBlend::plotMotion)
			.function("plotMotionFrameTime", &SBAnimationBlend::plotMotionFrameTime)
			.function("plotMotionJointTrajectory", &SBAnimationBlend::plotMotionJointTrajectory)
			.function("clearPlotMotion", &SBAnimationBlend::clearPlotMotion)
			.function("addKeyTagValue", &SBAnimationBlend::addKeyTagValue)
			.function("buildMotionAnalysis", &SBAnimationBlend::buildMotionAnalysis)
			.function("setBlendSkeleton", &SBAnimationBlend::setBlendSkeleton)
			.function("offsetMotion", &SBAnimationBlend::offsetMotion)
			.function("unoffsetMotions", &SBAnimationBlend::unoffsetMotions)
			.function("backupMotionKey", &SBAnimationBlend::backupMotionKey)
			.function("getNumDimensions",&SBAnimationBlend::getParameterDimension)
			;

		emscripten::class_<SBAnimationBlend0D, emscripten::base<SBAnimationBlend>>("SBAnimationBlend0D")
			.function("addMotion", &SBAnimationBlend0D::addMotion)
			;

		emscripten::class_<SBAnimationBlend1D, emscripten::base<SBAnimationBlend>>("SBAnimationBlend1D")
			.function("addMotion", &SBAnimationBlend1D::addMotion)
			.function("setParameter", &SBAnimationBlend1D::setParameter)
			;

		emscripten::class_<SBAnimationBlend2D, emscripten::base<SBAnimationBlend>>("SBAnimationBlend2D")
			.function("addMotion", &SBAnimationBlend2D::addMotion)
			.function("setParameter", &SBAnimationBlend2D::setParameter)
			.function("addTriangle", &SBAnimationBlend2D::addTriangle)
			;

		emscripten::class_<SBAnimationBlend3D, emscripten::base<SBAnimationBlend>>("SBAnimationBlend3D")
			.function("addMotion", &SBAnimationBlend3D::addMotion)
			.function("setParameter", &SBAnimationBlend3D::setParameter)
			.function("addTetrahedron", &SBAnimationBlend3D::addTetrahedron)
			;

		emscripten::class_<SBMotionBlendBase, emscripten::base<SBAnimationBlend>>("SBMotionBlendBase")
			.function("addMotion", &SBMotionBlendBase::addMotion)
			.function("setParameter", &SBMotionBlendBase::setMotionParameter)
			.function("getParameter", &SBMotionBlendBase::getMotionParameter)
			.function("buildBlendBase", &SBMotionBlendBase::buildBlendBase)
			.function("addTetrahedron", &SBMotionBlendBase::addTetrahedron)
			.function("getSkeleton", &SBMotionBlendBase::getSkeleton)
			.function("getInterpType", &SBMotionBlendBase::getInterpType)
			.function("getBlendType", &SBMotionBlendBase::getBlendType)
			;

		emscripten::class_<SBAnimationTransition>("SBAnimationTransition")
			.function("set", &SBAnimationTransition::set, emscripten::allow_raw_pointers())
			.function("setSourceState", &SBAnimationTransition::setSourceBlend, emscripten::allow_raw_pointers())
			.function("getSourceState", &SBAnimationTransition::getSourceBlend, emscripten::allow_raw_pointers())
			.function("setSourceBlend", &SBAnimationTransition::setSourceBlend, emscripten::allow_raw_pointers())
			.function("getSourceBlend", &SBAnimationTransition::getSourceBlend, emscripten::allow_raw_pointers())
			.function("setDestinationState", &SBAnimationTransition::setDestinationBlend, emscripten::allow_raw_pointers())
			.function("getDestinationState", &SBAnimationTransition::getDestinationBlend, emscripten::allow_raw_pointers())
			.function("setDestinationBlend", &SBAnimationTransition::setDestinationBlend, emscripten::allow_raw_pointers())
			.function("getDestinationBlend", &SBAnimationTransition::getDestinationBlend, emscripten::allow_raw_pointers())
			.function("getSourceMotionName", &SBAnimationTransition::getSourceMotionName)
			.function("setEaseInInterval", &SBAnimationTransition::setEaseInInterval)
			.function("addEaseOutInterval", &SBAnimationTransition::addEaseOutInterval)
			.function("removeEaseOutInterval", &SBAnimationTransition::removeEaseOutInterval)
			.function("getNumEaseOutIntervals", &SBAnimationTransition::getNumEaseOutIntervals)
			.function("getEaseOutInterval", &SBAnimationTransition::getEaseOutInterval)
			.function("getDestinationMotionName", &SBAnimationTransition::getDestinationMotionName)
			.function("getEaseInStart", &SBAnimationTransition::getEaseInStart)
			.function("getEaseInEnd", &SBAnimationTransition::getEaseInEnd)
			.function("setEaseInStart", &SBAnimationTransition::setEaseInStart)
			.function("setEaseInEnd", &SBAnimationTransition::setEaseInEnd)
			.function("getTransitionRule", &SBAnimationTransition::getTransitionRule, emscripten::allow_raw_pointers())
			.function("setTransitionRule", &SBAnimationTransition::setTransitionRule, emscripten::allow_raw_pointers())
			;

		emscripten::class_<SBAnimationBlendManager>("SBAnimationBlendManager")
			.function("createState0D", &SBAnimationBlendManager::createBlend0D, emscripten::allow_raw_pointers())
			.function("createState1D", &SBAnimationBlendManager::createBlend1D, emscripten::allow_raw_pointers())
			.function("createState2D", &SBAnimationBlendManager::createBlend2D, emscripten::allow_raw_pointers())
			.function("createState3D", &SBAnimationBlendManager::createBlend3D, emscripten::allow_raw_pointers())
			.function("createBlend0D", &SBAnimationBlendManager::createBlend0D, emscripten::allow_raw_pointers())
			.function("createBlend1D", &SBAnimationBlendManager::createBlend1D, emscripten::allow_raw_pointers())
			.function("createBlend2D", &SBAnimationBlendManager::createBlend2D, emscripten::allow_raw_pointers())
			.function("createBlend3D", &SBAnimationBlendManager::createBlend3D, emscripten::allow_raw_pointers())
			.function("createMotionBlendBase", &SBAnimationBlendManager::createMotionBlendBase, emscripten::allow_raw_pointers())
			.function("createTransition", &SBAnimationBlendManager::createTransition, emscripten::allow_raw_pointers())
			.function("getState", &SBAnimationBlendManager::getBlend, emscripten::allow_raw_pointers())
			.function("getBlend", &SBAnimationBlendManager::getBlend, emscripten::allow_raw_pointers())
			.function("getNumStates", &SBAnimationBlendManager::getNumBlends)
			.function("getNumBlends", &SBAnimationBlendManager::getNumBlends)
			.function("getStateNames", &SBAnimationBlendManager::getBlendNames)
			.function("getBlendNames", &SBAnimationBlendManager::getBlendNames)
			.function("getTransition", &SBAnimationBlendManager::getTransition, emscripten::allow_raw_pointers())
			.function("getTransitionByIndex", &SBAnimationBlendManager::getTransitionByIndex, emscripten::allow_raw_pointers())
			.function("getNumTransitions", &SBAnimationBlendManager::getNumTransitions)
			.function("getTransitionNames", &SBAnimationBlendManager::getTransitionNames)
			.function("getCurrentState", &SBAnimationBlendManager::getCurrentBlend)
			.function("getCurrentStateParameters", &SBAnimationBlendManager::getCurrentBlendParameters)
			.function("getCurrentBlend", &SBAnimationBlendManager::getCurrentBlend)
			.function("getCurrentBlendParameters", &SBAnimationBlendManager::getCurrentBlendParameters)
			.function("isStateScheduled", &SBAnimationBlendManager::isBlendScheduled)
			.function("isBlendScheduled", &SBAnimationBlendManager::isBlendScheduled)
			;

	}
#endif
}
#endif



