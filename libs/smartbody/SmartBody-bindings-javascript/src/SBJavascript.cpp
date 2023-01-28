
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
//#include <sb/SBNavigationMesh.h>
#include <sb/SBFaceDefinition.h>
#include <sb/nvbg.h>
#include <sb/SBBehavior.h>
#include <sb/SBMotion.h>
#include <sb/SBParseNode.h>
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
typedef std::vector<SrVec> VecArray;

namespace SmartBody 
{
#ifndef SB_NO_JAVASCRIPT
	void javascriptFuncsAnimation();
	void javascriptFuncsAttribute();
	void javascriptFuncsCharacter();
	void javascriptFuncsMath();
	void javascriptFuncsMotion();
	void javascriptFuncsScene();
	void javascriptFuncsSimulation();
	void javascriptFuncsSkeleton();
	void javascriptFuncsMesh();
	void javascriptFuncsSystem();


	EMSCRIPTEN_BINDINGS(SmartBody)
	{
		emscripten::function("printlog", &printLog);
		//emscripten::docstring_options local_docstring_options(true, true, false);
		emscripten::register_vector<std::string>("StringVec");
		emscripten::register_vector<float>("FloatVec");
		emscripten::register_vector<double>("DoubleVec");
		emscripten::register_vector<int>("IntVec");
		emscripten::register_vector<SrVec>("VecArray");

		emscripten::class_<QuatMap>("QuatMap")
			.function("__len__", &QuatMap::size)
			.function("clear", &QuatMap::clear)
			.function("__getitem__", &map_item<QuatMap>::get)
			.function("__setitem__", &map_item<QuatMap>::set) // to let container keep value
			.function("__delitem__", &map_item<QuatMap>::del)
			;

		emscripten::class_<VecMap>("VecMap")
			.function("__len__", &VecMap::size)
			.function("clear", &VecMap::clear)
			.function("__getitem__", &map_item<VecMap>::get)
			.function("__setitem__", &map_item<VecMap>::set) // to let container keep value
			.function("__delitem__", &map_item<VecMap>::del)
			;

		emscripten::class_<StringMap>("StringMap")
			.function("__len__", &StringMap::size)
			.function("clear", &StringMap::clear)
			.function("__getitem__", &map_item<StringMap>::get)
			.function("__setitem__", &map_item<StringMap>::set) // to let container keep value
			.function("__delitem__", &map_item<StringMap>::del)
			;

		emscripten::register_vector<SBAsset*>("AssetVec");

		emscripten::function("createController", createController, emscripten::allow_raw_pointers());


		//emscripten::def("testGetNDArray",testGetNDArray);
		// potential APIs that can be useful:
		// 1. getPawnNames
		// 2. removePawn
		// 3. createPawn 
		// etc. 
		// 

		javascriptFuncsSystem();

		// class interface
		/*	emscripten::class_<Script>("Script")
		.function("printInfo", &Script::print)
		.function("run", &Script::run)
		.function("abort", &Script::abort)
		;
		*/

		emscripten::class_<SBSubject>("SBSubject")
			.function("notifyObservers", &SBSubject::notifyObservers)
			.function("registerObserver", &SBSubject::registerObserver, emscripten::allow_raw_pointers())
			.function("unregisterObserver", &SBSubject::unregisterObserver, emscripten::allow_raw_pointers())
			;
		emscripten::class_<SBObserver>("SBObserver")
			.function("addDependency", &SBObserver::addDependency, emscripten::allow_raw_pointers())
			.function("removeDependency", &SBObserver::removeDependency, emscripten::allow_raw_pointers())
			.function("hasDependency", &SBObserver::hasDependency, emscripten::allow_raw_pointers())
			.function("notify", &SBObserver::notify, emscripten::allow_raw_pointers())
			;

		javascriptFuncsAttribute();


		emscripten::class_<SBObject>("SBObject")
			.function("getName", &SBObject::getName)
			.function("setName", &SBObject::setName)
			.function("getNumAttributes", &SBObject::getNumAttributes)
			.function("getAttributeNames", &SBObject::getAttributeNames)
			.function("getAttribute", &SBObject::getAttribute, emscripten::allow_raw_pointers())
			.function("clearAttributes", &SBObject::clearAttributes)
			.function("createBoolAttribute", &SBObject::createBoolAttribute, emscripten::allow_raw_pointers())
			.function("createVec3Attribute", &SBObject::createVec3Attribute, emscripten::allow_raw_pointers())
			.function("createIntAttribute", &SBObject::createIntAttribute, emscripten::allow_raw_pointers())
			.function("createDoubleAttribute", &SBObject::createDoubleAttribute, emscripten::allow_raw_pointers())
			.function("createStringAttribute", &SBObject::createStringAttribute, emscripten::allow_raw_pointers())
			.function("createActionAttribute", &SBObject::createActionAttribute, emscripten::allow_raw_pointers())
			.function("setBoolAttribute", &SBObject::setBoolAttribute)
			.function("setIntAttribute", &SBObject::setIntAttribute)
			.function("setDoubleAttribute", &SBObject::setDoubleAttribute)
			.function("setStringAttribute", &SBObject::setStringAttribute)
			.function("setVec3Attribute", &SBObject::setVec3Attribute)
			.function("setMatrixAttribute", &SBObject::setMatrixAttribute)
			.function("setActionAttribute", &SBObject::setActionAttribute)
			.function("setBool", &SBObject::setBoolAttribute)
			.function("setInt", &SBObject::setIntAttribute)
			.function("setDouble", &SBObject::setDoubleAttribute)
			.function("setString", &SBObject::setStringAttribute)
			.function("setVec3", &SBObject::setVec3Attribute)
			.function("setMatrix", &SBObject::setMatrixAttribute)
			.function("setAction", &SBObject::setActionAttribute)
			.function("getBool", &SBObject::getBoolAttribute)
			.function("getInt", &SBObject::getIntAttribute)
			.function("getDouble", &SBObject::getDoubleAttribute)
			.function("getString", &SBObject::getStringAttribute)
			
			;


		emscripten::class_<SBService, emscripten::base<SBObject>>("SBService")
			.function("setEnable", &SBService::setEnable)
			.function("isEnable", &SBService::isEnable)
			;

		emscripten::class_<SBServiceManager>("SBServiceManager")
			.function("addService", &SBServiceManager::addService, emscripten::allow_raw_pointers())
			.function("removeService", &SBServiceManager::removeService)
			.function("getServiceNames", &SBServiceManager::getServiceNames)
			.function("getNumServices", &SBServiceManager::getNumServices)
			.function("getService", &SBServiceManager::getService, emscripten::allow_raw_pointers())
			;


		javascriptFuncsSimulation();


		emscripten::class_<SBBmlProcessor>("BmlProcessor")
			.function("execBML", &SBBmlProcessor::execBML)
			.function("execBMLFile", &SBBmlProcessor::execBMLFile)
			.function("execXML", &SBBmlProcessor::execXML)
			.function("execXMLFile", &SBBmlProcessor::execXMLFile)
			.function("execBMLAt", &SBBmlProcessor::execBMLAt)
			.function("execBMLFileAt", &SBBmlProcessor::execBMLFileAt)
			.function("execXMLAt", &SBBmlProcessor::execXMLAt)
			.function("interruptCharacter", &SBBmlProcessor::interruptCharacter)
			.function("interruptBML", &SBBmlProcessor::interruptBML)
			;

		javascriptFuncsAnimation();


		emscripten::class_<SBSteerManager, emscripten::base<SBService>>("SBSteerManager")
			.function("createSteerAgent", &SBSteerManager::createSteerAgent, emscripten::allow_raw_pointers())
			.function("removeSteerAgent", &SBSteerManager::removeSteerAgent)
			.function("getNumSteerAgents", &SBSteerManager::getNumSteerAgents)
			.function("getSteerAgent", &SBSteerManager::getSteerAgent, emscripten::allow_raw_pointers())
			.function("getSteerAgentNames", &SBSteerManager::getSteerAgentNames)
			.function("start", &SBSteerManager::start)
			.function("stop", &SBSteerManager::stop)
			;

		emscripten::class_<SBCollisionManager, emscripten::base<SBService>>("SBCollisionManager")
			.function("start", &SBCollisionManager::start)
			.function("stop", &SBCollisionManager::stop)
			;

		emscripten::class_<SBFaceShiftManager, emscripten::base<SBService>>("SBFaceShiftManager")
			.function("getCoeffValue", &SBFaceShiftManager::getCoeffValue)
			.function("getHeadRotation", &SBFaceShiftManager::getHeadRotation)
			;

		emscripten::class_<SBPhysicsManager, emscripten::base<SBService>>("SBPhysicsManager")
			.function("createPhysicsCharacter", &SBPhysicsManager::createPhysicsCharacter, emscripten::allow_raw_pointers())
			.function("createPhysicsPawn", &SBPhysicsManager::createPhysicsPawn, emscripten::allow_raw_pointers())
			.function("getPhysicsEngine", &SBPhysicsManager::getPhysicsSimulationEngine, emscripten::allow_raw_pointers())  
			.function("getPhysicsCharacter", &SBPhysicsManager::getPhysicsCharacter, emscripten::allow_raw_pointers())  
			.function("getPhysicsJoint", &SBPhysicsManager::getPhysicsJoint, emscripten::allow_raw_pointers())  
			.function("getJointObj", &SBPhysicsManager::getJointObj, emscripten::allow_raw_pointers()) 
			.function("getPhysicsPawn", &SBPhysicsManager::getPhysicsPawn, emscripten::allow_raw_pointers())
			.function("applyForceToPawn", &SBPhysicsManager::applyForceToPawn)
			.function("applyForceToCharacter", &SBPhysicsManager::applyForceToCharacter)
			.function("start", &SBPhysicsManager::start) 
			.function("stop", &SBPhysicsManager::stop)		
			;


		emscripten::class_<SBBoneBusManager, emscripten::base<SBService>>("SBBoneBusManager")
			;

		emscripten::class_<SBSteerAgent>("SBSteerAgent")
			.function("setSteerStateNamePrefix", &SBSteerAgent::setSteerStateNamePrefix)
			.function("getSteerStateNamePrefix", &SBSteerAgent::getSteerStateNamePrefix)
			.function("setSteerType", &SBSteerAgent::setSteerType)
			.function("getSteerType", &SBSteerAgent::getSteerType)
			.function("getCharacter", &SBSteerAgent::getCharacter, emscripten::allow_raw_pointers())		
			;


		emscripten::class_<SBPhonemeManager>("SBPhonemeManager")
			.function("createDiphone", &SBPhonemeManager::createDiphone, emscripten::allow_raw_pointers())
			.function("getDiphones", &SBPhonemeManager::getDiphones, emscripten::allow_raw_pointers())
			.function("getDiphone", &SBPhonemeManager::getDiphone, emscripten::allow_raw_pointers())
			.function("getNumDiphoneMap", &SBPhonemeManager::getNumDiphoneMap)
			.function("getNumDiphones", &SBPhonemeManager::getNumDiphones)
			.function("getDiphoneMapNames", &SBPhonemeManager::getDiphoneMapNames)
			.function("deleteDiphoneSet", &SBPhonemeManager::deleteDiphoneSet)
			.function("loadDictionary", &SBPhonemeManager::loadDictionary)
			.function("addDictionaryWord", &SBPhonemeManager::addDictionaryWord)
			.function("getDictionaryWord", &SBPhonemeManager::getDictionaryWord)
			.function("clearPhonemesRealtime", &SBPhonemeManager::clearPhonemesRealtime)
			.function("setPhonemesRealtime", &SBPhonemeManager::setPhonemesRealtime)
			.function("getPhonemesRealtime", &SBPhonemeManager::getPhonemesRealtime)
			.function("getPhonemesRealtimeTimings", &SBPhonemeManager::getPhonemesRealtimeTimings)
			;

		emscripten::class_<SBDiphone>("SBDiphone")
			.function("addKey", &SBDiphone::addKey)
			.function("getKeys", &SBDiphone::getKeys, emscripten::allow_raw_pointers()) //return reference?
			.function("getVisemeNames", &SBDiphone::getVisemeNames)
			.function("getNumVisemes", &SBDiphone::getNumVisemes)
			.function("getFromPhonemeName", &SBDiphone::getFromPhonemeName)
			.function("getToPhonemeName", &SBDiphone::getToPhonemeName)		
			;

		emscripten::class_<SBBehaviorSetManager>("SBBehaviorSetManager")
			.function("createBehaviorSet", &SBBehaviorSetManager::createBehaviorSet, emscripten::allow_raw_pointers())
			.function("getBehaviorSets", &SBBehaviorSetManager::getBehaviorSets, emscripten::allow_raw_pointers())
			.function("getBehaviorSet", &SBBehaviorSetManager::getBehaviorSet, emscripten::allow_raw_pointers())
			.function("getNumBehaviorSets", &SBBehaviorSetManager::getNumBehaviorSets)
			.function("removeBehaviorSet", &SBBehaviorSetManager::removeBehaviorSet)
			.function("removeAllBehaviorSets", &SBBehaviorSetManager::removeAllBehaviorSets)
			;

		//return by reference string?
		emscripten::class_<SBBehaviorSet>("SBBehaviorSet")
			.function("setName", &SBBehaviorSet::setName)
			.function("getName", &SBBehaviorSet::getName)
			.function("setScript", &SBBehaviorSet::setScript)
			.function("getScript", &SBBehaviorSet::getScript)
			;

		emscripten::class_<SBRetargetManager>("SBRetargetManager")
			.function("createRetarget", &SBRetargetManager::createRetarget, emscripten::allow_raw_pointers())
			.function("getRetarget", &SBRetargetManager::getRetarget, emscripten::allow_raw_pointers())
			.function("getRetargetNames", &SBRetargetManager::getRetargetNames)

			;


		emscripten::class_<SBNavigationMeshManager>("SBNavigationMeshManager")
			.function("createNavigationMesh", &SBNavigationMeshManager::createNavigationMesh, emscripten::allow_raw_pointers())
			.function("getNavigationMesh", &SBNavigationMeshManager::getNavigationMesh, emscripten::allow_raw_pointers())
			.function("getNavigationMeshNames", &SBNavigationMeshManager::getNavigationMeshNames)
			;


		emscripten::class_<SBRetarget>("SBRetarget")
			.function("initRetarget", &SBRetarget::initRetarget)
			.function("applyRetargetJointRotation", &SBRetarget::applyRetargetJointRotation)
			.function("applyRetargetJointTranslation", &SBRetarget::applyRetargetJointTranslation)
			.function("getEndJointNames", &SBRetarget::getEndJointNames)
			.function("getRelativeJointNames", &SBRetarget::getRelativeJointNames)
			;


		/*
		emscripten::class_<Viseme>("Viseme")		
		.function("getName", &Viseme::getVisemeName)
		.function("getCharacterName", &Viseme::getCharName)
		.function("setWeight", &Viseme::setWeight)
		.function("setCurve", &Viseme::setCurve)
		;
		*/

		emscripten::class_<SBFaceDefinition, emscripten::base<SBObject>>("SBFaceDefinition")
			//.function("getName", &SBFaceDefinition::getName)
			.function("getNumVisemes", &SBFaceDefinition::getNumVisemes)
			.function("getVisemeNames", &SBFaceDefinition::getVisemeNames)
			.function("setViseme", &SBFaceDefinition::setViseme)
			.function("getVisemeMotion", &SBFaceDefinition::getVisemeMotion, emscripten::allow_raw_pointers())
			.function("getNumAUs", &SBFaceDefinition::getNumAUs)
			.function("getAUNumbers", &SBFaceDefinition::getAUNumbers)
			.function("getAUSide", &SBFaceDefinition::getAUSide)
			.function("getAUMotion", &SBFaceDefinition::getAUMotion, emscripten::allow_raw_pointers())
			.function("setAU", &SBFaceDefinition::setAU)
			.function("setFaceNeutral", &SBFaceDefinition::setFaceNeutral)
			.function("save", &SBFaceDefinition::save)
			.function("setDefaultFacePose", &SBFaceDefinition::setDefaultFacePose)
			;

		javascriptFuncsMotion();


		emscripten::class_<SBController, emscripten::base<SBObject>>("SBController")
			.constructor()
			//.function("setName", &SBController::setName)
			//.function("getName", &SBController::getName)
			.function("addChannel", &SBController::addChannel)
			.function("getType", &SBController::getType)
			.function("setIgnore", &SBController::setIgnore)
			.function("isIgnore", &SBController::isIgnore)
			.function("setDebug", &SBController::setDebug)
			.function("isDebug", &SBController::isDebug)
			.function("getDuration", &SBController::getDuration)
			.function("getChannelValue", &SBController::getChannelValue )
			.function("getChannelPos", &SBController::getChannelPos )
			.function("getChannelQuat", &SBController::getChannelQuat )
			.function("setChannelValue", &SBController::setChannelValue )
			.function("setChannelPos", &SBController::setChannelPos )
			.function("setChannelQuat", &SBController::setChannelQuat)
			.function("setChannelQuatGlobal", &SBController::setChannelQuatGlobal)

			;
		//multiple inheritence actually is a chain of inheritance MeCtNewLocomotion->SBController->SBObject
		emscripten::class_<MeCtNewLocomotion, emscripten::base<SBController>>("SBNewLocomotion")
			.constructor()
			.function("reset", &MeCtNewLocomotion::reset)
			.function("setDesiredHeading", &MeCtNewLocomotion::setDesiredHeading)
			.function("loopMotion", &MeCtNewLocomotion::loopMotion)
			;

		emscripten::class_<MeCtScheduler2, emscripten::base<SBController>>("SchedulerController")
			.function("getNumTracks", &MeCtScheduler2::count_children)
			;

		emscripten::class_<MeCtMotion, emscripten::base<SBController>>("MotionController")
			//	.function("getMotion", &MeCtMotion::motion, emscripten::allow_raw_pointers())
			;

		emscripten::class_<MeCtEyeLid, emscripten::base<SBController>>("EyelidController")
			;

		emscripten::class_<MeCtGaze, emscripten::base<SBController>>("GazeController")
			;

		emscripten::class_<MeCtCurveWriter, emscripten::base<SBController>>("CurveWriterController")
			;


		javascriptFuncsSkeleton();

		javascriptFuncsMesh();

		//SB behavior is actually not inherited from SBObject
		emscripten::class_<SBBehavior>("SBBehavior")
			.function("getType", &SBBehavior::getType)
			;

		emscripten::class_<GazeBehavior, emscripten::base<SBBehavior>>("GazeBehavior")
			//.function(emscripten::init<std::string, std::string>())
			.function("getGazeTarget", &GazeBehavior::getGazeTarget)
			.function("isFadingIn", &GazeBehavior::isFadingIn)
			.function("isFadingOut", &GazeBehavior::isFadingOut)
			.function("isFadedOut", &GazeBehavior::isFadedOut)
			.function("getHandle", &GazeBehavior::getHandle)
			;

		emscripten::class_<LocomotionBehavior, emscripten::base<SBBehavior>>("LocomotionBehavior")
			//.function(emscripten::init<std::string, std::string>())
			.function("getLocomotionTarget", &LocomotionBehavior::getLocomotionTarget)
			;

		emscripten::class_<PostureBehavior, emscripten::base<SBBehavior>>("PostureBehavior")
			//.function(emscripten::init<std::string, std::string>())
			.function("getPosture", &PostureBehavior::getPosture)
			;

		emscripten::class_<SpeechBehavior, emscripten::base<SBBehavior>>("SpeechBehavior")
			//.function(emscripten::init<std::string, std::string>())
			.function("getUtterance", &SpeechBehavior::getUtterance)
			;


		javascriptFuncsCharacter();


		emscripten::class_<SBReach>("SBReach")
			.function("getCharacter", &SBReach::getCharacter, emscripten::allow_raw_pointers())
			.function("copy", &SBReach::copy, emscripten::allow_raw_pointers())
			.function("addMotion", &SBReach::addMotion, emscripten::allow_raw_pointers())
			.function("removeMotion", &SBReach::removeMotion, emscripten::allow_raw_pointers())
			.function("getNumMotions", &SBReach::getNumMotions)
			.function("getMotionNames", &SBReach::getMotionNames)
			.function("build", &SBReach::build, emscripten::allow_raw_pointers())
			.function("setGrabHandMotion", &SBReach::setGrabHandMotion, emscripten::allow_raw_pointers())
			.function("getGrabHandMotion", &SBReach::getGrabHandMotion, emscripten::allow_raw_pointers())
			.function("setReleaseHandMotion", &SBReach::setReleaseHandMotion, emscripten::allow_raw_pointers())
			.function("getReleaseHandMotion", &SBReach::getReleaseHandMotion, emscripten::allow_raw_pointers())
			.function("setReachHandMotion", &SBReach::setReachHandMotion, emscripten::allow_raw_pointers())
			.function("getReachHandMotion", &SBReach::getReachHandMotion, emscripten::allow_raw_pointers())
			.function("setPointHandMotion", &SBReach::setPointHandMotion, emscripten::allow_raw_pointers())
			.function("getPointHandMotion", &SBReach::getPointHandMotion, emscripten::allow_raw_pointers())
			.function("setInterpolatorType", &SBReach::setInterpolatorType)
			.function("getInterpolatorType", &SBReach::getInterpolatorType)
			.function("isPawnAttached", &SBReach::isPawnAttached)
			;

		emscripten::class_<SBReachManager>("SBReachManager")
			.function("createReach", &SBReachManager::createReach, emscripten::allow_raw_pointers())
			.function("createReachWithTag", &SBReachManager::createReachWithTag, emscripten::allow_raw_pointers())
			.function("removeReach", &SBReachManager::removeReach, emscripten::allow_raw_pointers())		
			.function("getNumReaches", &SBReachManager::getNumReaches)
			.function("getReach", &SBReachManager::getReach, emscripten::allow_raw_pointers())
			;



		emscripten::class_<SBGestureMap::GestureInfo >("GestureInfo")
			;

		emscripten::class_<SBGestureMap>("SBGestureMap")
			.function("addGestureMapping", &SBGestureMap::addGestureMapping)
			.function("getGestureByInfo", &SBGestureMap::getGestureByInfo)
			.function("getGestureByIndex", &SBGestureMap::getGestureByIndex, emscripten::allow_raw_pointers())
			.function("getGestureAnimation", &SBGestureMap::getGestureAnimation)
			.function("getNumMappings", &SBGestureMap::getNumMappings)
			.function("validate", &SBGestureMap::validate)
			;

		emscripten::class_<SBGestureMapManager>("SBGestureMapManager")
			.function("createGestureMap", &SBGestureMapManager::createGestureMap, emscripten::allow_raw_pointers())
			.function("removeGestureMap", &SBGestureMapManager::removeGestureMap)
			.function("getNumGestureMaps", &SBGestureMapManager::getNumGestureMaps)
			.function("getGestureMap", &SBGestureMapManager::getGestureMap, emscripten::allow_raw_pointers())
			;

		emscripten::class_<SBHandConfigurationManager>("SBHandConfigurationManager")
			.function("createHandConfiguration", &SBHandConfigurationManager::createHandConfiguration, emscripten::allow_raw_pointers())
			.function("removeHandConfiguration", &SBHandConfigurationManager::removeHandConfiguration)
			.function("getNumHandConfiguration", &SBHandConfigurationManager::getNumHandConfigurations)
			.function("printHandConfiguration", &SBHandConfigurationManager::printHandConfiguration)
			;

		emscripten::class_<SBHandConfiguration>("SBHandConfiguration")
			.function("addMotion", &SBHandConfiguration::addMotion)
			.function("removeMotion", &SBHandConfiguration::removeMotion)
			.function("getNumMotions", &SBHandConfiguration::getNumMotions)
			.function("getMotionNames", &SBHandConfiguration::getMotionNames)
			;

		emscripten::class_<SBJointMap>("SBJointMap")
			.function("setMapping", &SBJointMap::setMapping)
			.function("removeMapping", &SBJointMap::removeMapping)
			.function("getMapTarget", &SBJointMap::getMapTarget)
			.function("getMapSource", &SBJointMap::getMapSource)
			.function("getNumMappings", &SBJointMap::getNumMappings)
			.function("getSource", &SBJointMap::getSource)
			.function("getTarget", &SBJointMap::getTarget)
			.function("applyMotion", &SBJointMap::applyMotion, emscripten::allow_raw_pointers())
			.function("applyMotionRecurse", &SBJointMap::applyMotionRecurse)
			.function("applySkeleton", &SBJointMap::applySkeleton, emscripten::allow_raw_pointers())
			.function("applyMotionInverse", &SBJointMap::applyMotionInverse, emscripten::allow_raw_pointers())
			.function("applySkeletonInverse", &SBJointMap::applySkeletonInverse, emscripten::allow_raw_pointers())
			.function("guessMapping", &SBJointMap::guessMapping, emscripten::allow_raw_pointers())
			.function("getMappedMotions", &SBJointMap::getMappedMotions)
			.function("getMappedSkeletons", &SBJointMap::getMappedSkeletons)
			;

		emscripten::class_<SBJointMapManager>("SBJointMapManager")
			.function("getJointMap", &SBJointMapManager::getJointMap, emscripten::allow_raw_pointers())
			.function("createJointMap", &SBJointMapManager::createJointMap, emscripten::allow_raw_pointers())
			.function("getJointMapNames", &SBJointMapManager::getJointMapNames)
			.function("removeJointMap", &SBJointMapManager::removeJointMap)
			.function("removeAllJointMaps", &SBJointMapManager::removeAllJointMaps)
			;

		emscripten::class_<SBEvent>("SBEvent")
			.constructor()
			.function("getType", &SBEvent::getType)
			.function("setType", &SBEvent::setType)
			.function("getParameters", &SBEvent::getParameters)
			.function("setParameters", &SBEvent::setParameters)
			.function("getSource", &SBEvent::getSource)
			.function("setSource", &SBEvent::setSource)
			;

		emscripten::class_<SBEventManager>("SBEventManager")
			.function("handleEvent", &SBEventManager::handleEvent, emscripten::allow_raw_pointers())
			.function("addEventHandler", &SBEventManager::addEventHandler, emscripten::allow_raw_pointers())
			.function("removeEventHandler", &SBEventManager::removeEventHandler)
			.function("getNumHandlers", &SBEventManager::getNumEventHandlers)
			.function("getEventHandler", &SBEventManager::getEventHandler, emscripten::allow_raw_pointers())
			.function("createEvent", &SBEventManager::createEvent, emscripten::allow_raw_pointers())
			;

		emscripten::class_<SBParseNode>("SBParseNode")
			.function("getWord", &SBParseNode::getWord)
			.function("getTerm", &SBParseNode::getTerm)
			.function("isTerminal", &SBParseNode::isTerminal)
			.function("getNumChildren", &SBParseNode::getNumChildren)
			.function("getChild", &SBParseNode::getChild, emscripten::allow_raw_pointers())
			;

		emscripten::class_<SBParser>("SBParser")
			.function("parse", &SBParser::parse, emscripten::allow_raw_pointers())
			.function("initialize", &SBParser::initialize)
			.function("cleanUp", &SBParser::cleanUp, emscripten::allow_raw_pointers())
			.function("isInitialized", &SBParser::isInitialized)
			;


		javascriptFuncsMath();

		emscripten::class_<Nvbg>("Nvbg")
			.allow_subclass<NvbgWrapper>("NvbgWrapper")
			.function("objectEvent", emscripten::optional_override([](Nvbg &self, std::string character, std::string name, 
			bool isAnimate, SrVec charPosition, SrVec charVelocity, SrVec objPosition, SrVec objVelocity, SrVec relativePosition, SrVec relativeVelocity)
			{
				return self.Nvbg::objectEvent(character, name, isAnimate, charPosition, charVelocity, objPosition, objVelocity, relativePosition, relativeVelocity);
			}))
			.function("execute", emscripten::optional_override([](Nvbg &self, std::string character, std::string to, std::string messageId, std::string xml)
			{
				return self.Nvbg::execute(character, to, messageId, xml);
			}))
			.function("executeEvent", emscripten::optional_override([](Nvbg &self, std::string character, std::string messageId, std::string state)
			{
				return self.Nvbg::executeEvent(character, messageId, state);
			}))
			.function("executeSpeech", emscripten::optional_override([](Nvbg &self, std::string character, std::string speechStatus, std::string speechId, std::string speaker)
			{
				return self.Nvbg::executeSpeech(character, speechStatus, speechId, speaker);
			}))
			.function("executeSpeechRequest", emscripten::optional_override([](Nvbg &self, std::vector<std::string> behaviors, std::vector<std::string> types, 
			std::vector<float> times, std::vector<std::string> targets, std::vector<std::string> info)
			{
				return self.Nvbg::executeSpeechRequest(behaviors, types, times, targets, info);
			}))
			.function("notifyAction", emscripten::optional_override([](Nvbg &self, std::string name)
			{
				return self.Nvbg::notifyLocal(name);
			}))
			.function("notifyBool", emscripten::optional_override([](Nvbg &self, std::string name, bool val)
			{
				return self.Nvbg::notifyLocal(name);
			}))
			.function("notifyInt", emscripten::optional_override([](Nvbg &self, std::string name, int val)
			{
				return self.Nvbg::notifyLocal(name);
			}))
			.function("notifyDouble", emscripten::optional_override([](Nvbg &self, std::string name, double val)
			{
				return self.Nvbg::notifyLocal(name);
			}))
			.function("notifyString", emscripten::optional_override([](Nvbg &self, std::string name, std::string val)
			{
				return self.Nvbg::notifyLocal(name);
			}))
			.function("notifyVec3", emscripten::optional_override([](Nvbg &self, std::string name, SrVec val)
			{
				return self.Nvbg::notifyLocal(name);
			}))
			.function("notifyMatrix", emscripten::optional_override([](Nvbg &self, std::string name, SrVec SrMat)
			{
				return self.Nvbg::notifyLocal(name);
			}))
			;

		emscripten::class_<SBScript>("SBScript")
			.allow_subclass<SBScriptWrapper>("SBScriptWrapper")
			.function("start", emscripten::optional_override([](SBScript &self)
			{
				return self.SBScript::start();
			}))
			.function("stop", emscripten::optional_override([](SBScript &self)
			{
				return self.SBScript::stop();
			}))
			.function("beforeUpdate", emscripten::optional_override([](SBScript &self, double time)
			{
				return self.SBScript::beforeUpdate(time);
			}))
			.function("update", emscripten::optional_override([](SBScript &self, double time)
			{
				return self.SBScript::update(time);
			}))
			.function("afterUpdate", emscripten::optional_override([](SBScript &self, double time)
			{
				return self.SBScript::afterUpdate(time);
			}))
			;

		emscripten::class_<SBEventHandler>("SBEventHandler")
			.allow_subclass<SBEventHandlerWrapper>("SBEventHandlerWrapper")
			.function("executeAction", emscripten::optional_override([](SBEventHandler &self, SBEvent* event)
			{
				return self.SBEventHandler::executeAction(event);
			}), emscripten::allow_raw_pointers())
			;

		emscripten::class_<JavascriptController, emscripten::base<SBController>> ("JavascriptController")
			.allow_subclass<JavascriptControllerWrapper>("JavacriptControllerWrapper")
			.function("start", emscripten::optional_override([](JavascriptController &self)
			{
				return self.JavascriptController::start();
			}))
			.function("stop", emscripten::optional_override([](JavascriptController &self)
			{
				return self.JavascriptController::stop();
			}))
			.function("init", emscripten::optional_override([](JavascriptController &self)
			{
				return self.JavascriptController::init();
			}))
			.function("evaluate", emscripten::optional_override([](JavascriptController &self)
			{
				return self.JavascriptController::evaluate();
			}))
			;

		emscripten::class_<SBAnimationTransitionRule>("SBAnimationTransitionRule")
			.allow_subclass<TransitionRuleWrapper>("TransitionRuleWrapper")
			.function("check", emscripten::optional_override([](SBAnimationTransitionRule &self, SmartBody::SBCharacter* character, SmartBody::SBAnimationBlend* blend)
			{
				return self.SBAnimationTransitionRule::check(character, blend);
			}), emscripten::allow_raw_pointers())
			;

		/*
		emscripten::class_<SBSceneListener>("SBSceneListener")
			.function("OnCharacterCreate", &SBSceneListener::OnCharacterCreate)
			.function("OnCharacterDelete", &SBSceneListener::OnCharacterDelete)
			.function("OnCharacterUpdate", &SBSceneListener::OnCharacterUpdate)
			.function("OnPawnCreate", &SBSceneListener::OnPawnCreate)
			.function("OnPawnDelete", &SBSceneListener::OnPawnDelete)
			.function("OnViseme", &SBSceneListener::OnViseme)
			.function("OnEvent", &SBSceneListener::OnEvent)
			;
		*/
		emscripten::class_<SBSceneListener>("CharacterListener")
			.allow_subclass<CharacterListenerWrapper>("CharacterListenerWrapper")
			.function("OnCharacterCreate", emscripten::optional_override([](SBSceneListener &self, const std::string & name, const std::string & objectClass){
				return self.SBSceneListener::OnCharacterCreate(name, objectClass);
			}))
			.function("OnCharacterDelete", emscripten::optional_override([](SBSceneListener &self, const std::string & name){
				return self.SBSceneListener::OnCharacterDelete(name);
			}))
			.function("OnCharacterUpdate", emscripten::optional_override([](SBSceneListener &self, const std::string & name){
				return self.SBSceneListener::OnCharacterUpdate(name);
			}))
			.function("OnPawnCreate", emscripten::optional_override([](SBSceneListener &self, const std::string & name){
				return self.SBSceneListener::OnPawnCreate(name);
			}))
			.function("OnPawnDelete", emscripten::optional_override([](SBSceneListener &self, const std::string & name){
				return self.SBSceneListener::OnPawnDelete(name);
			}))
			.function("OnViseme", emscripten::optional_override([](SBSceneListener &self, const std::string & name, const std::string & 
			visemeName, const float weight, const float blendTime)
			{
				return self.SBSceneListener::OnViseme(name, visemeName, weight, blendTime);
			}))
			.function("OnChannel", emscripten::optional_override([](SBSceneListener &self, const std::string & name, const std::string & channelName, const float value)
			{
				return self.SBSceneListener::OnChannel(name, channelName, value);
			}))
			.function("OnLogMessage", emscripten::optional_override([](SBSceneListener &self, const std::string & message)
			{
				return self.SBSceneListener::OnLogMessage(message);
			}))
			;

		emscripten::class_<SBAssetManager, emscripten::base<SBObject>>("SBAssetManager")
			.function("addSkeletonDefinition", &SBAssetManager::addSkeletonDefinition, emscripten::allow_raw_pointers())
			.function("removeSkeletonDefinition", &SBAssetManager::removeSkeletonDefinition)//deprecated calling it from SBScene, call SBAssetManager instead
			.function("createMotion", &SBAssetManager::createMotion, emscripten::allow_raw_pointers())
			.function("createSkeleton", &SBAssetManager::createSkeleton, emscripten::allow_raw_pointers())
			.function("getSkeleton", &SBAssetManager::getSkeleton, emscripten::allow_raw_pointers())
			.function("addAssetPath", &SBAssetManager::addAssetPath)
			.function("removeAssetPath", &SBAssetManager::removeAssetPath)
			.function("removeAllAssetPaths", &SBAssetManager::removeAllAssetPaths)
			.function("getAssetPaths", &SBAssetManager::getAssetPaths)
			.function("getLocalAssetPaths", &SBAssetManager::getLocalAssetPaths)
			.function("loadAsset", &SBAssetManager::loadAsset)
			.function("loadAssets", &SBAssetManager::loadAssets)
			.function("loadAssetsFromPath", &SBAssetManager::loadAssetsFromPath)
			.function("addMotion", &SBAssetManager::addMotion, emscripten::allow_raw_pointers())
			.function("addMotionDefinition", &SBAssetManager::addMotionDefinition, emscripten::allow_raw_pointers())
			.function("removeMotion", &SBAssetManager::removeMotion, emscripten::allow_raw_pointers())
			.function("addMotions", &SBAssetManager::addMotions)
			.function("getMotion", &SBAssetManager::getMotion, emscripten::allow_raw_pointers())
			.function("getNumMotions", &SBAssetManager::getNumMotions)
			.function("getMotionNames", &SBAssetManager::getMotionNames)
			.function("getNumSkeletons", &SBAssetManager::getNumSkeletons)
			.function("getSkeletonNames", &SBAssetManager::getSkeletonNames)
			.function("addMesh", &SBAssetManager::addMesh, emscripten::allow_raw_pointers())
			.function("removeMesh", &SBAssetManager::removeMesh, emscripten::allow_raw_pointers())
			.function("getMeshNames", &SBAssetManager::getMeshNames)
			.function("getMesh", &SBAssetManager::getDeformableMesh, emscripten::allow_raw_pointers())
			;

		emscripten::class_<SBVHMsgManager, emscripten::base<SBService>>("SBVHMsgManager")
			.function("connect", &SBVHMsgManager::connect)
			.function("disconnect", &SBVHMsgManager::disconnect)
			.function("sendMessage", &SBVHMsgManager::sendMessage)
			.function("sendOpMessage", &SBVHMsgManager::sendOpMessage)
			.function("send", &SBVHMsgManager::send, emscripten::allow_raw_pointers())
			.function("poll", &SBVHMsgManager::poll)
			.function("setPort", &SBVHMsgManager::setPort)
			.function("getPort", &SBVHMsgManager::getPort)
			.function("setServer", &SBVHMsgManager::setServer)
			.function("getServer", &SBVHMsgManager::getServer)
			.function("setScope", &SBVHMsgManager::setScope)
			.function("getScope", &SBVHMsgManager::getScope)
			.function("setEnableLogging", &SBVHMsgManager::setEnableLogging)
			.function("isEnableLogging", &SBVHMsgManager::isEnableLogging)
			;

		javascriptFuncsScene();
	}
#endif
}

void setupJavascript()
{
#ifndef SB_NO_JAVASCRIPT
	//for those global variables do not use 'var' in Javascript
	EM_ASM(
		scene = Module.getScene();
		bml = scene.getBmlProcessor();
		sim = scene.getSimulationManager();
		assets = scene.getAssetManager();
		
		//wrapper for types, so that we could use SrVec in Javascript instead of Module.SrVec
		SrVec = Module.SrVec;
		StringVec = Module.StringVec;
		VecMap = Module.VecMap;
	);
#endif
}
#endif