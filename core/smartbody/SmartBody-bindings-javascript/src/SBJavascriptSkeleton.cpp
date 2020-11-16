
#include "SBJavascript.h"
#include "SBJavascriptClass.h"
#include <sb/SBJoint.h>
#include <sb/SBSkeleton.h>
#include <sk/sk_skeleton.h>
#include <sb/SBScript.h>
#include <sb/SBEvent.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBAnimationTransitionRule.h>
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
	void javascriptFuncsSkeleton()
	{
		emscripten::class_<SkJoint, emscripten::base<SBObject>>("SkJoint")
			.function("getJointType", &SkJoint::getJointType)
		;

		emscripten::class_<SBSkeleton, emscripten::base<SBObject>>("SBSkeleton")
			.constructor()
			.function("load", &SBSkeleton::load)
			.function("save", &SBSkeleton::save)
			.function("loadFromString",  &SBSkeleton::loadFromString)
			.function("rescale", &SBSkeleton::rescale)
			.function("getFileName", &SBSkeleton::getFileName)
			.function("getNumJoints", &SBSkeleton::getNumJoints)
			.function("getJointNames", &SBSkeleton::getJointNames)
			.function("getJointByName", &SBSkeleton::getJointByName, emscripten::allow_raw_pointers())
			.function("getJoint", &SBSkeleton::getJoint, emscripten::allow_raw_pointers())
			.function("getNumChannels", &SBSkeleton::getNumChannels)
			.function("getChannelType", &SBSkeleton::getChannelType)
			.function("getChannelSize", &SBSkeleton::getChannelSize)
			.function("createSkelWithoutPreRot", &SBSkeleton::createSkelWithoutPreRot, emscripten::allow_raw_pointers())	
			.function("orientJointsLocalAxesToWorld", &SBSkeleton::orientJointsLocalAxesToWorld)
			.function("createJoint", &SBSkeleton::createJoint, emscripten::allow_raw_pointers())
			.function("createStaticJoint", &SBSkeleton::createStaticJoint, emscripten::allow_raw_pointers())
			.function("createChannel", &SBSkeleton::createChannel, emscripten::allow_raw_pointers())		
			.function("update", &SBSkeleton::update)
			; 

		emscripten::class_<SBJoint, emscripten::base<SBObject>>("SBJoint")
			.constructor()
			.function("setName", &SBJoint::setName)
			.function("getName", &SBJoint::getName)
			.function("getParent", &SBJoint::getParent, emscripten::allow_raw_pointers())
			.function("setParent", &SBJoint::setParent, emscripten::allow_raw_pointers())
			.function("getNumChildren", &SBJoint::getNumChildren)
			.function("getChild", &SBJoint::getChild, emscripten::allow_raw_pointers())
			.function("getSkeleton", &SBJoint::getSkeleton, emscripten::allow_raw_pointers())
			.function("setSkeleton", &SBJoint::setSkeleton, emscripten::allow_raw_pointers())
			.function("getOffset", &SBJoint::getOffset) 
			.function("setOffset", &SBJoint::setOffset)
			.function("getIndex", &SBJoint::getIndex)
			.function("getPosition", &SBJoint::getPosition)
			.function("getQuat", &SBJoint::getQuaternion)
			.function("getMatrixGlobal", &SBJoint::getMatrixGlobal)
			.function("getMatrixLocal", &SBJoint::getMatrixLocal)
			.function("addChild", &SBJoint::addChild, emscripten::allow_raw_pointers())
			.function("setUseRotation", &SBJoint::setUseRotation)	
			.function("getUseRotation", &SBJoint::isUseRotation)	
			.function("setUsePosition", &SBJoint::setUsePosition)	
			.function("isUsePosition", &SBJoint::isUsePosition)	
			.function("getMass", &SBJoint::getMass)
			.function("setMass", &SBJoint::setMass)
			.function("getPrerotation", &SBJoint::getPrerotation)
			.function("setPrerotation", &SBJoint::setPrerotation)
			.function("getPostrotation", &SBJoint::getPostrotation)
			.function("setPostrotation", &SBJoint::setPostrotation)		
			;
	}
#endif
}
#endif
