
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
#include <sb/SBNavigationMesh.h>
#include <sb/SBNavigationMeshManager.h>
#include <sb/SBHandConfigurationManager.h>
#include <sb/SBHandConfiguration.h>
#include <sb/SBEvent.h>
#include <sb/SBSceneListener.h>
#include <sb/SBNavigationMesh.h>
#include <sb/SBFaceDefinition.h>
#include <sb/nvbg.h>
#include <sb/SBBehavior.h>
#include <sb/SBMotion.h>
#include <sb/SBParseNode.h>
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
	void javascriptFuncsAttribute()
	{

		emscripten::class_<SBAttributeInfo>("SBAttributeInfo")
			.function("getPriority", &SBAttributeInfo::getPriority)
			.function("setPriority", &SBAttributeInfo::setPriority)
			.function("getReadOnly", &SBAttributeInfo::getReadOnly)
			.function("setReadOnly", &SBAttributeInfo::setReadOnly)
			.function("getHidden", &SBAttributeInfo::getHidden)
			.function("setHidden", &SBAttributeInfo::setHidden)
			.function("setDescription", &SBAttributeInfo::setDescription)
			.function("getDescription", &SBAttributeInfo::getDescription)
			;

		emscripten::class_<SBAttribute, emscripten::base<SBSubject>>("SBAttribute")
			.function("getName", &SBAttribute::getName)
			.function("getAttributeInfo", &SBAttribute::getAttributeInfo, emscripten::allow_raw_pointers())
			;

		emscripten::class_<ActionAttribute, emscripten::base<SBAttribute>>("ActionAttribute")
			.function("setValue", &ActionAttribute::setValue)
			;

		emscripten::class_<BoolAttribute, emscripten::base<SBAttribute>>("BoolAttribute")
			.function("getValue", &BoolAttribute::getValue)
			.function("setValue", &BoolAttribute::setValue)
			.function("getDefaultValue", &BoolAttribute::getDefaultValue)
			.function("setDefaultValue", &BoolAttribute::setDefaultValue)
			.function("setValueFast", &BoolAttribute::setValueFast)
			;

		emscripten::class_<StringAttribute, emscripten::base<SBAttribute>>("StringAttribute")
			.function("getValue", &StringAttribute::getValue)
			.function("setValue", &StringAttribute::setValue)
			.function("getDefaultValue", &StringAttribute::getDefaultValue)
			.function("setDefaultValue", &StringAttribute::setDefaultValue)
			.function("setValueFast", &StringAttribute::setValueFast)
			.function("setValidValues", &StringAttribute::setValidValues)
			.function("getValidValues", &StringAttribute::getValidValues)
			;

		emscripten::class_<IntAttribute, emscripten::base<SBAttribute>>("IntAttribute")
			.function("getValue", &IntAttribute::getValue)
			.function("setValue", &IntAttribute::setValue)
			.function("getDefaultValue", &IntAttribute::getDefaultValue)
			.function("setDefaultValue", &IntAttribute::setDefaultValue)
			.function("setValueFast", &IntAttribute::setValueFast)
			;

		emscripten::class_<DoubleAttribute, emscripten::base<SBAttribute>>("DoubleAttribute")
			.function("getValue", &DoubleAttribute::getValue)
			.function("setValue", &DoubleAttribute::setValue)
			.function("getDefaultValue", &DoubleAttribute::getDefaultValue)
			.function("setDefaultValue", &DoubleAttribute::setDefaultValue)
			.function("setValueFast", &DoubleAttribute::setValueFast)
			;

		emscripten::class_<Vec3Attribute, emscripten::base<SBAttribute>>("Vec3Attribute")
			.function("getValue", &Vec3Attribute::getValue)
			.function("setValue", &Vec3Attribute::setValue)
			.function("getDefaultValue", &Vec3Attribute::getDefaultValue)
			.function("setDefaultValue", &Vec3Attribute::setDefaultValue)
			.function("setValueFast", &Vec3Attribute::setValueFast)
			;

		emscripten::class_<MatrixAttribute, emscripten::base<SBAttribute>>("MatrixAttribute")
			.function("getValue", &MatrixAttribute::getValue)
			.function("setValue", &MatrixAttribute::setValue)
			.function("getDefaultValue", &MatrixAttribute::getDefaultValue)
			.function("setDefaultValue", &MatrixAttribute::setDefaultValue)
			.function("setValueFast", &MatrixAttribute::setValueFast)
			;
	}
#endif
}
#endif