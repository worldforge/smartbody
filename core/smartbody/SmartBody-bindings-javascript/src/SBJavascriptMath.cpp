
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
#include <sbm/GenericViewer.h>

#include <sr/sr_box.h>
#include <sr/sr_camera.h>
#include <sr/sr_euler.h>
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

namespace SmartBody
{
#ifndef SB_NO_JAVASCRIPT
	void javascriptFuncsMath()
	{

		emscripten::class_<SrVec>("SrVec")
			.constructor()
			.constructor<float, float, float>()
			.constructor<SrVec>()
			.function("__str__", &SrVec::toString)
			.function("getData", &SrVec::getData)
			.function("setData", &SrVec::setData)
			.function("len", &SrVec::norm)
			.function("normalize", &SrVec::normalize)
			.function("isZero", &SrVec::iszero)
			.function("rotY", &SrVec::rotY)
			.function("vecAngle", &SrVec::vecAngle)
			.function("vecYaw", &SrVec::vecYaw)
			.function("addVec", &SrVec::operator+=)
			.function("cross", &SrVec::cross)
			;
		emscripten::function("rotatePoint", &rotatePoint); 
		emscripten::class_<SrMat>("SrMat")
			.constructor()
			.function("__str__", &SrMat::toString)
			.function("getData", &SrMat::getData)
			.function("setData", &SrMat::setData)
			.function("identity", &SrMat::identity)
			.function("transpose", &SrMat::transpose)
			.function("multVec", &SrMat::multVec)
			.function("getTranslation", &SrMat::get_translation)
			;
		emscripten::function("srEulerMat", &sr_euler_mat);
		emscripten::class_<SrQuat>("SrQuat")
			.constructor()
			.constructor<float, float, float, float>()
			.constructor<SrVec, float>()
			.constructor<SrMat>()
			.function("__str__", &SrQuat::toString)
			.function("getAxis", &SrQuat::axis)
			.function("getAngle", &SrQuat::angle)
			.function("getData", &SrQuat::getData)
			.function("setData", &SrQuat::setData)
			.function("multVec", &SrQuat::multVec)
			.function("normalize", &SrQuat::normalize)
			.function("setAxisAngle", &SrQuat::setAxisAngle)
			;	

		emscripten::class_<SrBox>("SrBox")
			.constructor()
			.constructor<SrVec, SrVec>()
			.constructor<SrBox>()
			.function("setMinimum", &SrBox::setMinimum)
			.function("setMaximum", &SrBox::setMaximum)
			.function("getMinimum", &SrBox::getMinimum)
			.function("getMaximum", &SrBox::getMaximum)
			.function("getCenter", &SrBox::getCenter)
			.function("getMinSize", &SrBox::min_size)
			.function("getMaxSize", &SrBox::max_size)
			.function("getSize", &SrBox::getSize)
			.function("doesContain", &SrBox::contains)
			.function("doesIntersect", &SrBox::intersects)
			.function("getVolume", &SrBox::volume)
			.function("isEmpty", &SrBox::empty)
			.function("grows", &SrBox::grows)
			.function("extend", emscripten::select_overload<void(const SrBox &)>(&SrBox::extend));
			;

	}
#endif
}
#endif
