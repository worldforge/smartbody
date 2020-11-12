/*************************************************************
Copyright (C) 2017 University of Southern California

This file is part of Smartbody.

Smartbody is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Smartbody is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Smartbody.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************/

#include "SBCharacter.h"
#include <sb/SBSkeleton.h>

#include <sbm/mcontrol_callbacks.h>
#include "sb/SBController.h"
#include "bml/bml_types.hpp"
#include "bml/bml_speech.hpp"
#include "sb/SBBehavior.h"
#include <sbm/PPRAISteeringAgent.h>
#include <sb/SBSteerAgent.h>
#include <sb/SBPhysicsManager.h>
#include <sb/SBSteerManager.h>
#include <sb/SBSteerAgent.h>
#include <sb/SBPhoneme.h>
#include <sb/SBPhonemeManager.h>
#include <sb/SBAssetManager.h>
#include <sb/SBScene.h>
#include <sb/SBSpeechManager.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBReach.h>
#include <sb/SBSceneListener.h>
#include <sb/SBMotionGraph.h>
#include <sb/SBParser.h>
#include <sb/SBUtilities.h>
#include <controllers/MeCtReachEngine.h>
#include <controllers/me_ct_motion_recorder.h>
#include <controllers/me_ct_scheduler2.h>
#include <controllers/me_ct_scheduler2.h>
#include <controllers/me_ct_gaze.h>
#include <controllers/me_controller_tree_root.hpp>
#include <controllers/me_ct_curve_writer.hpp>
#include <controllers/me_ct_channel_writer.hpp>
#include <controllers/me_ct_saccade.h>
#include <controllers/me_ct_motion_graph.hpp>
#include <controllers/me_ct_generic_hand.h>
#include <sbm/remote_speech.h>
#include <sbm/local_speech.h>
#include <sbm/text_speech.h>
#include <sbm/sbm_speech_audiofile.hpp>
#include <bml/bml_processor.hpp>
#include <sk/sk_channel_array.h>
#include <sr/sr_random.h>


namespace SmartBody {

SBCharacter::SBCharacter() : SbmCharacter()
{
	
}

SBCharacter::SBCharacter(const std::string& name, const std::string& type) : SbmCharacter(name.c_str(), type)
{
	setAttributeGroupPriority("Gestures", 80);
	setAttributeGroupPriority("Display", 110);
	setAttributeGroupPriority("Voice", 170);
	setAttributeGroupPriority("Lip Sync", 180);
	setAttributeGroupPriority("Gestures", 190);
	setAttributeGroupPriority("Behaviors", 200);
	setAttributeGroupPriority("Speech", 205);
	setAttributeGroupPriority("Retargeting", 210);
	setAttributeGroupPriority("Reaching", 220);
	setAttributeGroupPriority("Remote", 800);
	setAttributeGroupPriority("Baldi Lip Sync", 2000);


	createBoolAttribute("blendShape.mergeBoundary", false, true, "Blend Shape", 0, false, false, false, "Whether to merge the boundary of blendshape faces with body mesh.");


	createBoolAttribute("posture.useDifference", false, true, "Posture", 90, false, false, false, "Whether to use body posture difference"); 

	createBoolAttribute("useCustomizedLipSyncIfPresent", true, true, "Lip Sync", 60, false, false, false, "If motion name exists inside pre-recorded audio file bml, use it by default"); 
	
	createBoolAttribute("reach.useLocomotion", false, true, "Reaching", 110, false, false, false, "Whether to use locomotion for reach by default.");

	createBoolAttribute("gestureSimple", false, true, "Gestures", 65, false, false, false, "Uses simple gesture handling; gestures map to an animation, but otherwise, no special handling.");
	createBoolAttribute("gestureUseBlends", false, true, "Gestures", 88, false, false, false, "Whether SmartBody should use blend as gesture.");
	createBoolAttribute("gestureRequest.autoGestureTransition", true, true, "Gestures", 89, false, false, false, "Whether SmartBody should filter gestures behaviors according to priority."); 
	createBoolAttribute("gestureRequest.matchingHandness", true, true, "Gestures", 90, false, false, false, "Whether SmartBody should filter gestures behaviors according to priority."); 
	createBoolAttribute("gestureRequest.enableTransitionToStroke", false, true, "Gestures", 91, false, false, false, "Enable Transition to stroke posture directly if time is too limited."); 
	createBoolAttribute("gestureRequest.gestureLog", false, true, "Gestures", 92, false, false, false, "Toggle for logging the gesture request process."); 
	createBoolAttribute("gestureRequest.matchingSpeed", true, true, "Gestures", 93, false, false, false, "Holding previous gesture if necessary to match up with current gesture stroke speed."); 
	createDoubleAttribute("gestureRequest.gestureSpeedThreshold", 1.5, true, "Gestures", 95, false, false, false, "The speed threshold used to determine whether it's suitable to transition from one gesture to another"); 
	createDoubleAttribute("gestureRequest.gestureWristActiveThreshold", 0.15, true, "Gestures", 96, false, false, false, "The speed threshold used to determine if this hand moving."); 
	createBoolAttribute("gestureRequest.experimentalCoarticulation", false, true, "Gestures", 97, false, false, false, "Experimental coarticulation code.");
	createBoolAttribute("gestureRequest.experimentalPriorityCoarticulation", false, true, "Gestures", 107, false, false, false, "Experimental coarticulation code.");
	createDoubleAttribute("gestureRequest.experimentalCoarticulationOverlap", 0.0, true, "Gestures", 108, false, false, false, "Experimental coarticulation code.");
	createBoolAttribute("gestureRequest.experimentalIgnoreTransitionTime", false, true, "Gestures", 109, false, false, false, "Experimental coarticulation code.");
	createDoubleAttribute("gestureRequest.transitionTimeMultiplier", 1.0, true, "Gestures", 98, false, false, false, "Experimental coarticulation code; transition time adjustment.");
	createBoolAttribute("gestureRequest.experimentalTransitions", false, true, "Gestures", 99, false, false, false, "Experimental transition code.");
	createBoolAttribute("gestureRequest.coarticulateFace", false, true, "Gestures", 101, false, false, false, "Coarticulate facial action units via BML.");
	createDoubleAttribute("gestureRequest.coarticulateFaceProximity", .5, true, "Gestures", 102, false, false, false, "Coarticulate facial action units via BML when within this time of each other.");
	createBoolAttribute("gestureRequest.coarticulateRandomPriority", false, true, "Gestures", 103, false, false, false, "Randomize gesture priority.");
	createStringAttribute("gestureRequest.lastGestureRandom", "", true, "Gestures", 104, false, false, false, "Last set of randomized gestures choices. Used to replay the BML file exactly as before.");
	createStringAttribute("gestureRequest.curGestureRandom", "", true, "Gestures", 104, false, false, false, "Current set of randomized gestures choices. Used to replay the BML file exactly as before.");
	createIntAttribute("gestureRequest.lastGestureRandomIndex", 0, true, "Gestures", 105, false, false, false, "Last set of randomized gestures choices. Used to replay the BML file exactly as before.");
	createBoolAttribute("gestureRequest.useLastRandomGesture", false, true, "Gestures", 106, false, false, false, "Determines if the last gesture random selections should be applied to the next gesture choices.");
	createIntAttribute("gestureRequest.randomizePriorities", 0, true, "Gestures", 107, false, false, false, "Randomizes the priorites by a given amount.");
	createBoolAttribute("gestureRequest.useDiscreteBlends", false, true, "Gestures", 108, false, false, false, "Determines if the last gesture random selections should be applied to the next gesture choices.");
	
	createStringAttribute("gestureMap", "", true, "Gestures", 50, false, false, false, "Name of the gesture map to use.");

	std::vector<std::string> gesturePolicyVec;
	gesturePolicyVec.push_back("random");
	gesturePolicyVec.push_back("first");
	StringAttribute* gesturePolicyAttr = createStringAttribute("gesturePolicy", "random", true, "Gestures", 60, false, false, false, "Gesture policy to be used");
	gesturePolicyAttr->setValidValues(gesturePolicyVec);

	/*
	SmartBody::DoubleAttribute* motionAttr = createDoubleAttribute("emotion", 0, true, "Basic", 70, false, false, false, "Current emotion state, 0 is neutral, 1 is extreme angry");
	motionAttr->setMin(0.0);
	createBoolAttribute("emotion_automatic", 0, false, "Basic", 71, false, false, false, "");
	*/

	SmartBody::DoubleAttribute* bmlDelayAttr = createDoubleAttribute("bmlscheduledelay", 0.3, true, "Behaviors", 97, false, false, false, "Delay all bml schedules by a fixed amount.");
	bmlDelayAttr->setMin(0.0);
	SmartBody::DoubleAttribute* visemeDelayAttr = createDoubleAttribute("visemetimedelay", 0.0, true, "Behaviors", 98, false, false, false, "Delay visemes by a fixed amount.");
	visemeDelayAttr->setMin(0.0);
	createBoolAttribute("controllerPruning", true, true, "Behaviors", 99, false, false, false, "Disables controller pruning.");

	createBoolAttribute("visemecurve", false, true, "Lip Sync", 100, false, false, false, "Use curve-based visemes instead of discrete visemes.");
	createBoolAttribute("phoneBigramAnimation", true, true, "Lip Sync", 101, false, false, false, "Use phone bigram animation as opposed to diphone animation");
	createDoubleAttribute("diphoneTransition", 0.1, true, "Lip Sync", 102, false, false, false, "transition overlap between diphone");
	createBoolAttribute("usePhoneBigram", true, true, "Lip Sync", 150, false, false, false, "Use phone bigram method.");
	createBoolAttribute("lipSyncConstraint", true, true, "Lip Sync", 151, false, false, false, "Use constrains on facial shapes.");
	createBoolAttribute("constrainFV", true, true, "Lip Sync", 152, false, false, false, "Constrain facial shape FV.");
	createBoolAttribute("constrainW", true, true, "Lip Sync", 152, false, false, false, "Constrain facial shape W.");
	createBoolAttribute("constrainWide", false, true, "Lip Sync", 152, false, false, false, "Constrain facial shape Wide.");
	createBoolAttribute("constrainPBM", true, true, "Lip Sync", 153, false, false, false, "Constrain facial shape PBM.");
	createBoolAttribute("constrainShCh", true, true, "Lip Sync", 153, false, false, false, "Constrain facial shape ShCh.");
	createDoubleAttribute("openConstraintByPBM", 0.5, true, "Lip Sync", 154, false, false, false, "Open facial shape constraint by PBM facial shape.");
	createDoubleAttribute("wideConstraintByPBM", 0.5, true, "Lip Sync", 155, false, false, false, "Wide facial shape constraint by PBM facial shape.");
	createDoubleAttribute("shchConstraintByPBM", 0.5, true, "Lip Sync", 155, false, false, false, "ShCh facial shape constraint by PBM facial shape.");
	createDoubleAttribute("openConstraintByFV", 0.5, true, "Lip Sync", 155, false, false, false, "Open facial shape constraint by FV facial shape.");
	createDoubleAttribute("wideConstraintByFV", 0.5, true, "Lip Sync", 155, false, false, false, "Wide facial shape constraint by FV facial shape.");
	createDoubleAttribute("openConstraintByShCh", 0.7, true, "Lip Sync", 155, false, false, false, "Open facial shape constraint by ShCh facial shape.");
	createDoubleAttribute("openConstraintByW", 0.5, true, "Lip Sync", 155, false, false, false, "Open facial shape constraint by W facial shape.");
	createDoubleAttribute("openConstraintByWide", 0.5, true, "Lip Sync", 155, false, false, false, "Open facial shape constraint by Wide facial shape.");
	createDoubleAttribute("constrainPressedLips", 0.0, true, "Lip Sync", 165, false, false, false, "Extra time to keep lips pressed together.");
	createDoubleAttribute("lipSyncScale", 1, true, "Lip Sync", 156, false, false, false, "Scale factor for lip sync curves.");
	createStringAttribute("lipSyncSetName", "", true, "Lip Sync", 157, false, false, false, "Name of the lip sync set to be used when using phone bigram method.");
	createBoolAttribute("lipSyncSplineCurve", true, true, "Lip Sync", 158, false, false, false, "Use spline/linear curve.");
	createDoubleAttribute("lipSyncSmoothWindow", .18, true, "Lip Sync", 159, false, false, false, "Smooth window size. If it's less than 0, don't do smooth.");
	setDiphoneSmoothWindow(0.18f);
	createDoubleAttribute("lipSyncSmoothWindow-PBM", .1, true, "Lip Sync", 160, false, false, false, "Smooth window size for PBM. If it's less than 0, don't do smooth.");
	createDoubleAttribute("lipSyncSmoothWindow-FV", .1, true, "Lip Sync", 161, false, false, false, "Smooth window size for PBM. If it's less than 0, don't do smooth.");
	createDoubleAttribute("lipSyncSpeedLimit", 6.0, true, "Lip Sync", 162, false, false, false, "Speed Limit of facial shape movement");
	createDoubleAttribute("ja", .5, true, "Lip Sync", 170, false, false, false, "Jaw opening");
	createDoubleAttribute("li", .5, true, "Lip Sync", 180, false, false, false, "Lip intensity");
	createDoubleAttribute("maxVisemeWeight", 1, true, "Lip Sync", 190, false, false, false, "Maximum weighting of visemes. Default is 1.");
	setDiphoneSpeedLimit(6.0f);
	
	// Dominance curve attributes (other attributes will be created if the "dominancecurve" attribute is selected
	createBoolAttribute("dominancecurve", false, true, "Baldi Lip Sync", 500, false, false, false, "Use dominance curve instead of predefined curves for lip syncing.");

//-------------------

	createBoolAttribute("ikPostFix", false, true, "Retargeting", 200, false, false, false, "Post-Processing IK to fix foot sliding.");
	createBoolAttribute("terrainWalk", false, true, "Retargeting", 200, false, false, false, "Post-Processing to adjust the character for different terrain height. ikPostFix must be on for this to work.");
	createBoolAttribute("onlineRetarget", true, true, "Retargeting", 200, false, false, false, "Use on-line retargeting to adjust joint angles when playing animation blend.");

	SmartBody::StringAttribute* displayAttribute = createStringAttribute("displayType", "bones", true, "Display", 210, false, false, false, "How to display the character: bones, colgeo, visgeo, axis, mesh, GPUmesh");
	std::vector<std::string> displayVec;
	displayVec.push_back("bones");
	displayVec.push_back("colgeo");
	displayVec.push_back("visgeo");
	displayVec.push_back("axis");
	displayVec.push_back("skinWeight");
	displayVec.push_back("wireframeMesh");
	displayVec.push_back("mesh");
	displayVec.push_back("GPUmesh");
	displayAttribute->setValidValues(displayVec);

	createVec3Attribute("deformableMeshScale", 1.0, 1.0, 1.0, true, "Display", 220, false, false, false, "Scale factor when loading mesh.");
	createStringAttribute("deformableMesh", "", true, "Display", 230, false, false, false, "Directory that contains mesh information.");
	createBoolAttribute("showSelected", true, true, "Display", 250, false, false, false, "Shows selection indicator when selected.");
	createDoubleAttribute("alphaThreshold", .5, true, "Display", 260, false, false, false, "Alpha threshold for transparent textures.");

	createStringAttribute("receiverName", "kinect1", true, "Remote", 300, false, false, false, "Name to respond to when receiving joint positions and orientations remotely.");
	createDoubleAttribute("receiverScale", 100.0, true, "Remote", 300, false, false, false, "Scaling factor when receiving a joint position.");

	createVec3Attribute("leftShoulderOffset",0.f,0.f,0.f,true, "Retargeting", 320, false, false, false, "rotation offset added on left shoulder joint during retargeting.");
	createVec3Attribute("rightShoulderOffset",0.f,0.f,0.f,true, "Retargeting", 340, false, false, false, "rotation offset added on left shoulder joint during retargeting.");
	createVec3Attribute("retargetOffset.l_hip", 0.f, 0.f, 0.f, true, "Retargeting", 351, false, false, false, "rotation offset added on left shoulder joint during retargeting.");
	createVec3Attribute("retargetOffset.l_knee", 0.f, 0.f, 0.f, true, "Retargeting", 352, false, false, false, "rotation offset added on left shoulder joint during retargeting.");
	createVec3Attribute("retargetOffset.l_ankle", 0.f, 0.f, 0.f, true, "Retargeting", 353, false, false, false, "rotation offset added on left shoulder joint during retargeting.");
	createVec3Attribute("retargetOffset.r_hip", 0.f, 0.f, 0.f, true, "Retargeting", 354, false, false, false, "rotation offset added on left shoulder joint during retargeting.");
	createVec3Attribute("retargetOffset.r_knee", 0.f, 0.f, 0.f, true, "Retargeting", 355, false, false, false, "rotation offset added on left shoulder joint during retargeting.");
	createVec3Attribute("retargetOffset.r_ankle", 0.f, 0.f, 0.f, true, "Retargeting", 356, false, false, false, "rotation offset added on left shoulder joint during retargeting.");

	createBoolAttribute("isReaching", false, true, "Reaching", 158, false, false, false, "Whether a character is reaching.");


	createDoubleAttribute("headNodGain", 1.0, true, "BML", 400, false, false, false, "Scaling factor for head nods.");
	createDoubleAttribute("headShakeGain", 1.0, true, "BML", 401, false, false, false, "Scaling factor for head shakes.");
	createDoubleAttribute("headTossGain", 1.0, true, "BML", 402, false, false, false, "Scaling factor for head tosses.");
	createDoubleAttribute("headWiggleGain", 1.0, true, "BML", 403, false, false, false, "Scaling factor for wiggles.");
	createDoubleAttribute("headWaggleGain", 1.0, true, "BML", 404, false, false, false, "Scaling factor for waggles.");


	std::vector<std::string> voiceTypes;
	voiceTypes.push_back("");
	voiceTypes.push_back("remote");
	voiceTypes.push_back("audiofile");	
	voiceTypes.push_back("local");	
	voiceTypes.push_back("text");

	StringAttribute* voiceAttribute = createStringAttribute("voice", "remote", true, "Voice", 400, false, false, false, "How the voice is created - local (uses local festival voice), remote (uses a speech relay), or audiofile (voice generated from prerecorded audio).");
	voiceAttribute->setValidValues(voiceTypes);

	createStringAttribute("voiceCode", "voice_kal_diphone", true, "Voice", 410, false, false, false, "For local and remote voices, the name of the voice to be used. For audiofile, the path to the audiofile when combined with the media path.");
	StringAttribute* voiceBackupAttribute = createStringAttribute("voiceBackup", "audiofile", true, "Voice", 420, false, false, false, "How the voice is created if the primary voice fails. local (uses local festival voice), remote (uses a speech relay), or audiofile (voice generated from prerecorded audio).");
	voiceBackupAttribute->setValidValues(voiceTypes);
	createStringAttribute("voiceBackupCode", ".", true, "Voice", 430, false, false, false, "For local and remote voices, the name of the backup voice to be used. For audiofile, the path to the audiofile when combined with the media path.");
	createStringAttribute("lastAudioFile", ".", true, "Voice", 440, false, false, false, "Last audio file used for speech.");
	
	std::vector<std::string> utterancePolicyTypes;
	utterancePolicyTypes.push_back("none");
	utterancePolicyTypes.push_back("ignore");
	utterancePolicyTypes.push_back("queue");
	utterancePolicyTypes.push_back("interrupt");	
	StringAttribute* utterancePolicyAttribute = createStringAttribute("utterancePolicy", "none", true, "Speech", 500, false, false, false, "How utterances are handled when the character is already performing an utterance. Valid values are: ignore (ignores the new utterance and any associated behaviors), queue (plays the new utterance and associated behavior when the old utterance has finished), interrupt (stops the existing utterance and associated behaviors and plays the new one)");
	utterancePolicyAttribute->setValidValues(utterancePolicyTypes);

	std::vector<std::string> saccadePolicy;
	saccadePolicy.push_back("stopinutterance");
	saccadePolicy.push_back("alwayson");
	StringAttribute* saccadePolicyAttribute = createStringAttribute("saccadePolicy", "stopinutterance", true, "Speech", 501, false, false, false, "How saccade is handled during utterance");
	saccadePolicyAttribute->setValidValues(saccadePolicy);
	createDoubleAttribute("saccadeTurnOnDelay", 2, true, "Speech", 502, false, false, false, "delay saccade turn on after utterance.");
	jointTrajBlendWeight = 0.f;
	useJointConstraint = false;
	_reach = NULL;

	// since this is a character, show the deformable mesh by default
	setBoolAttribute("showStaticMesh",false);

	frameDataMarshalFriendly = NULL;
	frameDataMarshalFriendly = new SBM_CharacterFrameDataMarshalFriendly();
	InitFrameDataMarshalFriendly();
	
	SmartBody::StringAttribute* textureAttribute = createStringAttribute("texturesType", "static", true, "Display", 210, false, false, false, "How to display textures");
	std::vector<std::string> textureVec;
	textureVec.push_back("none");
	textureVec.push_back("static");
	textureVec.push_back("dynamic");
	textureAttribute->setValidValues(textureVec);
	
	createBoolAttribute("useOptimizedBlendShapes", true, true, "Display", 720, false, false, false, "Enables faster blend shape computation by caching blend shape data.");
	
	_parserListener = NULL;
	//setUseBlendFaceTextures(false);
}


SBAPI SBCharacter::~SBCharacter()
{
	//FreeFrameDataMarshalFriendly();
	delete frameDataMarshalFriendly;
	frameDataMarshalFriendly = NULL;
}


void SBCharacter::startMotionRecord( double frameRate )
{
	if (record_ct)
	{
		record_ct->startRecording(frameRate);
	}
}


void SBCharacter::stopMotionRecord( const std::string& motionName, const std::string& type)
{
	if (record_ct)
	{
		record_ct->stopRecording(motionName, type);
	}
}

void SBCharacter::writeMotionRecord(const std::string& motionName, const std::string& type)
{
	if (record_ct)
	{
		record_ct->writeRecording(motionName, type);
	}
}

int SBCharacter::setup()
{
	return SbmCharacter::setup();
}

const std::string& SBCharacter::getName()
{
	return SbmCharacter::getName();
}

void SBCharacter::setName(const std::string& name)
{
	SBPawn::setName(name);

	SmartBody::SBScene::getScene()->updateCharacterNames();

}

void SBCharacter::setType(const std::string& type)
{
	setClassType(type);
}

std::string SBCharacter::getType()
{
	return getClassType();
}

int SBCharacter::getNumControllers()
{
	MeControllerTreeRoot* controllerTree = ct_tree_p;
	if (controllerTree)
		return controllerTree->count_controllers();
	else
		return 0;
}

void SBCharacter::addController(int index, SBController* controller)
{
  if (!controller) return;
  
	int numControllers = this->getNumControllers();
	for (int c = 0; c < numControllers; c++)
	{
		if (this->getControllerByIndex(c) == controller)
		{
			SmartBody::util::log("Controller %s already exists in controller tree for character %s.", controller->getName().c_str(), this->getName().c_str());
			return;
		}
	}
	// make sure the controller doesn't already exist in the tree
	controller->init(this);
	ct_tree_p->add_controller(controller, index);
}

void SBCharacter::removeController(SBController* controller)
{
	ct_tree_p->remove_controller(controller);
}

void SBCharacter::setAutomaticPruning(bool val)
{
	_isControllerPruning = val;
}

bool SBCharacter::isAutomaticPruning()
{
	return _isControllerPruning;
}

void SBCharacter::pruneControllers()
{
	prune_controller_tree();
}

void SBCharacter::setUseVisemeCurves(bool val)
{
	set_viseme_curve_mode(val);
}

bool SBCharacter::isUseVisemeCurves()
{
	return get_viseme_curve_mode();
}

float SBCharacter::getVisemeTimeOffset()
{
	return get_viseme_time_delay();
}

void SBCharacter::setVisemeTimeOffset(float val)
{
	set_viseme_time_delay(val);
}

SBController* SBCharacter::getControllerByIndex(int index)
{
	if (!ct_tree_p)
		return NULL;

	if (index < 0 || index >= (int)ct_tree_p->count_controllers())
	{
		SmartBody::util::log("Index %d out of range.", index);
		return NULL;
	}

	SBController* controller = getControllerByName(ct_tree_p->controller(index)->getName());
	return controller;
}

SBController* SBCharacter::getControllerByName(const std::string& name)
{
	if (!ct_tree_p)
		return NULL;
	
	for (int i = 0; i < (int)ct_tree_p->count_controllers(); i++)
	{
		SBController* controller = dynamic_cast<SBController*>(ct_tree_p->controller(i));
		const std::string& cName = controller->getName();
		if (name == cName)
		{
			return controller;
		}
	}
	return NULL;
}

std::vector<std::string> SBCharacter::getControllerNames()
{
	std::vector<std::string> ret;
	for (int i = 0; i < (int)ct_tree_p->count_controllers(); i++)
	{
		const std::string& cName = ct_tree_p->controller(i)->getName();
		ret.push_back(cName);
	}	
	return ret;
}

void SBCharacter::setVoice(const std::string& type)
{
	if (type == "")
	{
		set_speech_impl(NULL);
	}
	else if (type == "remote")
	{
		set_speech_impl(SmartBody::SBScene::getScene()->getSpeechManager()->speech_rvoice());
	}
	else if (type == "audiofile")
	{
		set_speech_impl(SmartBody::SBScene::getScene()->getSpeechManager()->speech_audiofile());
	}
	else if (type == "text")
	{
		set_speech_impl(SmartBody::SBScene::getScene()->getSpeechManager()->speech_text());
	}
	else if (type == "local")
	{
		set_speech_impl(SmartBody::SBScene::getScene()->getSpeechManager()->speech_localvoice());
	}
	else
	{
		SmartBody::util::log("Unknown voice setting '%s'.", type.c_str());
	}

	StringAttribute* attr = dynamic_cast<StringAttribute*>(getAttribute("voice"));
	if (attr)
		attr->setValueFast(type);

}

const std::string SBCharacter::getVoice()
{
	/*
	
	std::string type = "";
	if (mcu.speech_rvoice())
		type = "remote";
	if (mcu.speech_audiofile())
		type = "audiofile";
	if (mcu.speech_text())
		type = "text";
	if (mcu.speech_localvoice())
		type = "local";
	*/
	// through attribute system...
	std::string type = "";
	StringAttribute* attr = dynamic_cast<StringAttribute*>(getAttribute("voice"));
	if (attr)
		type = attr->getValue();
	return type;
}

void SBCharacter::setVoiceCode(const std::string& param)
{
	set_voice_code(param);
	StringAttribute* attr = dynamic_cast<StringAttribute*>(getAttribute("voiceCode"));
	if (attr)
		attr->setValueFast(param);
}

const std::string& SBCharacter::getVoiceCode()
{
	return get_voice_code();
}

void SBCharacter::setVoiceBackup(const std::string& type)
{
	

	if (type == "")
	{
		set_speech_impl_backup(NULL);
	}
	else if (type == "remote")
	{
		set_speech_impl_backup(SmartBody::SBScene::getScene()->getSpeechManager()->speech_rvoice());
	}
	else if (type == "audiofile")
	{
		set_speech_impl_backup(SmartBody::SBScene::getScene()->getSpeechManager()->speech_audiofile());
	}
	else if (type == "text")
	{
		set_speech_impl_backup(SmartBody::SBScene::getScene()->getSpeechManager()->speech_text());
	}
	else if (type == "local")
	{
		set_speech_impl_backup(SmartBody::SBScene::getScene()->getSpeechManager()->speech_localvoice());
	}
	else
	{
		SmartBody::util::log("Unknown voice backup setting '%s'.", type.c_str());
	}

	StringAttribute* attr = dynamic_cast<StringAttribute*>(getAttribute("voiceBackup"));
	if (attr)
		attr->setValueFast(type);
}

const std::string& SBCharacter::getVoiceBackup()
{
	return get_voice_code_backup();
}

void SBCharacter::setVoiceBackupCode(const std::string& param)
{
	set_voice_code_backup(param);
	StringAttribute* attr = dynamic_cast<StringAttribute*>(getAttribute("voiceCodeBackup"));
	if (attr)
		attr->setValueFast(param);
}

const std::string& SBCharacter::getVoiceBackupCode()
{
	return get_voice_code_backup();
}

int SBCharacter::getNumBehaviors()
{
	std::vector<SBBehavior*>& behaviors = getBehaviors();
	return behaviors.size();
}

SBBehavior* SBCharacter::getBehavior(int num)
{
	if (num < (int) _curBehaviors.size())
		return _curBehaviors[num];
	else
		return NULL;
}

double SBCharacter::getLastScheduledSpeechBehavior()
{
	double lastTime =-1.0;

	BML::MapOfBmlRequest bmlRequestMap = SmartBody::SBScene::getScene()->getBmlProcessor()->getBMLProcessor()->getBMLRequestMap();
	for (BML::MapOfBmlRequest::iterator iter = bmlRequestMap.begin(); 
		 iter != bmlRequestMap.end();
		 iter ++
		)
	{
		std::string requestName = iter->first;
		BML::BmlRequestPtr bmlRequestPtr = iter->second;
		if (bmlRequestPtr->actor->getName() == this->getName())
		{
			if (bmlRequestPtr->speech_request)
			{
				if (lastTime < bmlRequestPtr->speech_request.get()->behav_syncs.sync_end()->time())
					lastTime = bmlRequestPtr->speech_request.get()->behav_syncs.sync_end()->time();

			}
		}
	}
	return lastTime;
}

std::string SBCharacter::hasSpeechBehavior()
{
	BML::MapOfBmlRequest bmlRequestMap = SmartBody::SBScene::getScene()->getBmlProcessor()->getBMLProcessor()->getBMLRequestMap();
	for (BML::MapOfBmlRequest::iterator iter = bmlRequestMap.begin(); 
		 iter != bmlRequestMap.end();
		 iter ++
		)
	{
		std::string requestName = iter->first;
		BML::BmlRequestPtr bmlRequestPtr = iter->second;
		if (bmlRequestPtr->actor->getName() == this->getName())
		{
			if (bmlRequestPtr->speech_request)
			{
				return (*bmlRequestPtr).msgId;
			}
		}
	}

	return "";
}

std::vector<SBBehavior*>& SBCharacter::getBehaviors()
{
	for (size_t b = 0; b < _curBehaviors.size(); b++)
	{
		delete _curBehaviors[b];
	}
	_curBehaviors.clear();

	// speech
	BML::MapOfBmlRequest bmlRequestMap = SmartBody::SBScene::getScene()->getBmlProcessor()->getBMLProcessor()->getBMLRequestMap();
	for (BML::MapOfBmlRequest::iterator iter = bmlRequestMap.begin(); 
		 iter != bmlRequestMap.end();
		 iter ++
		)
	{
		std::string requestName = iter->first;
		BML::BmlRequestPtr bmlRequestPtr = iter->second;
		if (bmlRequestPtr->actor->getName() == this->getName())
		{
			if (bmlRequestPtr->speech_request)
			{
				SpeechBehavior* speechBehavior = new SpeechBehavior();
				// what information do we need here?
				speechBehavior->setId((*bmlRequestPtr).msgId);
				speechBehavior->setUtterance(bmlRequestPtr->speech_request->getSpeechText());

				_curBehaviors.push_back(speechBehavior);
			}
		}
	}

	// posture
	if (this->posture_sched_p)
	{
		MeCtScheduler2::VecOfTrack tracks = posture_sched_p->tracks();
		for (size_t t = 0; t < tracks.size(); t++)
		{
			MeCtMotion* motionCt = dynamic_cast<MeCtMotion*>(tracks[t]->animation_ct());
			if (motionCt)
			{
				const std::string& motionName = motionCt->motion()->getName();
				PostureBehavior* postureBehavior = new PostureBehavior();
				postureBehavior->setPosture(motionName);
				_curBehaviors.push_back(postureBehavior);
			}
		}
	}

	// locomotion
	SmartBody::SBSteerManager* manager = SmartBody::SBScene::getScene()->getSteerManager();
	SmartBody::SBSteerAgent* steerAgent = manager->getSteerAgent(this->getName());
	if (steerAgent)
	{
		LocomotionBehavior* locoBehavior = new LocomotionBehavior();
		PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
		if (ppraiAgent->getAgent())
		{
			const SteerLib::AgentGoalInfo& goal = ppraiAgent->getAgent()->currentGoal();
			Util::Point goalTarget = goal.targetLocation;
		
			SrVec target(goalTarget.x, 0.f, goalTarget.z);
			bool reachTarget = _reachTarget && !_lastReachStatus;
			locoBehavior->setLocomotionTarget(target);
			locoBehavior->setReachTarget(reachTarget);
		}
		_curBehaviors.push_back(locoBehavior);
	}

	// gaze
	if (this->gaze_sched_p)
	{
		MeCtScheduler2::VecOfTrack tracks = gaze_sched_p->tracks();
		for (size_t t = 0; t < tracks.size(); t++)
		{
			MeCtGaze* gazeCt = dynamic_cast<MeCtGaze*>(tracks[t]->animation_ct());
			if (gazeCt)
			{
				float x, y, z;
				SkJoint* joint = gazeCt->get_target_joint(x, y, z);
				if (joint)
				{
					SkSkeleton* skeleton = joint->skeleton();
					SBSkeleton* sbSkel = dynamic_cast<SBSkeleton*> (skeleton);
					SBPawn* sbPawn = sbSkel->getPawn();
					
					GazeBehavior* gazeBehavior = new GazeBehavior();
					gazeBehavior->setGazeTarget(sbPawn->getName());
					gazeBehavior->setFadingIn(gazeCt->isFadingIn());
					gazeBehavior->setFadingOut(gazeCt->isFadingOut());
					gazeBehavior->setFadedOut(gazeCt->isFadedOut());
					gazeBehavior->setHandle(gazeCt->handle());

					_curBehaviors.push_back(gazeBehavior);
					break;
				}
			}
		}
	}

	return _curBehaviors;
}


SBFaceDefinition* SBCharacter::getFaceDefinition()
{
	return SbmCharacter::getFaceDefinition();
}

void SBCharacter::setFaceDefinition(SBFaceDefinition* face)
{
	SbmCharacter::setFaceDefinition(face);
}

void SBCharacter::updateDefaultFacePose()
{
	if (!_faceDefinition)
		return;

	interruptFace(0);
}

void SBCharacter::notify(SBSubject* subject)
{
	SBAttribute* attribute = dynamic_cast<SBAttribute*>(subject);
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	if (attribute)
	{
		if (attribute->getName() == "updateChannel")
		{
			if (!getSkeleton())
				return;

			// clear out all the TypeBlendShape joint channel
			removeAllBlendShapeChannels();
			
			// add channel
			int shapesAdded = 0;
			std::vector<std::string> attributeNames = this->getAttributeNames();
			std::vector<std::string> shapeNames;
			for (size_t i = 0; i < attributeNames.size(); ++i)
			{
				std::string attrName = attributeNames[i];
				if (attrName.find("blendShape.channelName.") != std::string::npos)
				{
					std::string channelName = getStringAttribute(attrName);
					if (channelName != "")
					{
						int startPos = std::string("blendShape.channelName").size();
						std::string shapeName = attrName.substr(startPos + 1);
						if (getSkeleton()->getJointByName(channelName))
						{
							//SmartBody::util::log("Blend shape channel %s already existed. Skipping add shape %s...", channelName.c_str(), shapeName.c_str());
							continue;
						}
						shapeNames.push_back(channelName);
						//SmartBody::util::log("Blend shape channel %s added for shape %s ...", channelName.c_str(), shapeName.c_str());
					}
				}
			}
			if (shapeNames.size() > 0)
				addBlendShapeChannels(shapeNames);

		}
		else if (attribute->getName() == "createPhysics")
		{
			SmartBody::SBPhysicsManager* manager = SmartBody::SBScene::getScene()->getPhysicsManager();
			manager->createPhysicsCharacter(this->getName());
			return;
		}

		const std::string& attrName = attribute->getName();
		if (attrName == "posture.useDifference")
		{
			SmartBody::BoolAttribute* postureDiff = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			if (this->posture_sched_p)
			{
				MeCtScheduler2::VecOfTrack tracks = this->posture_sched_p->tracks();
				for (size_t t = 0; t < tracks.size(); t++)
				{
					MeCtMotion* motionCt = dynamic_cast<MeCtMotion*>(tracks[t]->animation_ct());
					if (motionCt)
					{
						motionCt->useOffset(postureDiff->getValue());
					}
				}
			}
		}
		else if (attrName == "visemecurve")
		{
			SmartBody::BoolAttribute* curveAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			set_viseme_curve_mode(curveAttribute->getValue());
		}
		else if (attrName == "visemetimedelay")
		{
			SmartBody::DoubleAttribute* timeDelayAttribute = dynamic_cast<SmartBody::DoubleAttribute*>(attribute);
			set_viseme_time_delay((float) timeDelayAttribute->getValue());
		}
		else if (attrName == "usePhoneBigram")
		{
			SmartBody::BoolAttribute* diphoneAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			this->setDiphone(diphoneAttribute->getValue());
		}
		else if (attrName == "lipSyncScale")
		{
			SmartBody::DoubleAttribute* diphoneScaleAttribute = dynamic_cast<SmartBody::DoubleAttribute*>(attribute);
			this->setDiphoneScale((float)diphoneScaleAttribute->getValue());
		}
		else if (attrName == "lipSyncSplineCurve")
		{
			SmartBody::BoolAttribute* splineCurveAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			this->setDiphoneSplineCurve(splineCurveAttribute->getValue());
		}
		else if (attrName == "lipSyncSmoothWindow")
		{
			SmartBody::DoubleAttribute* smoothWindowAttribute = dynamic_cast<SmartBody::DoubleAttribute*>(attribute);
			setDiphoneSmoothWindow((float)smoothWindowAttribute->getValue());
		}
		else if (attrName == "lipSyncSpeedLimit")
		{
			SmartBody::DoubleAttribute* speedLimitAttribute = dynamic_cast<SmartBody::DoubleAttribute*>(attribute);
			setDiphoneSpeedLimit((float)speedLimitAttribute->getValue());
		}
		else if (attrName == "deformableMesh")
		{
	
		}
		else if (attrName == "voice")
		{
			SmartBody::StringAttribute* strAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
			this->setVoice(strAttribute->getValue());
		}
		else if (attrName == "voiceCode")
		{
			SmartBody::StringAttribute* strAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
			this->setVoiceCode(strAttribute->getValue());
		}
		else if (attrName == "voiceBackup")
		{
			SmartBody::StringAttribute* strAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
			this->setVoiceBackup(strAttribute->getValue());
		}
		else if (attrName == "voiceBackupCode")
		{
			SmartBody::StringAttribute* strAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
			this->setVoiceBackupCode(strAttribute->getValue());
		}
		else if (attrName == "dominancecurve")
		{
			SmartBody::BoolAttribute* boolAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			if (boolAttribute->getValue())
			{
				if (!this->getAttribute("jaw_rot_min"))
				{
					// create the attributes for Baldi lip syncing
					int startingPos = 500;
					createDoubleAttribute("jaw_rot_min", -4.0, true, "Baldi Lip Sync", startingPos + 0, false, false, false, "");
					createDoubleAttribute("jaw_rot_max", 13, true, "Baldi Lip Sync", startingPos + 1, false, false, false, "");
					createDoubleAttribute("jaw_rot_default", 0.21, true, "Baldi Lip Sync", startingPos + 2, false, false, false, "");

					createDoubleAttribute("lower_lip_ftuck_min", -15, true, "Baldi Lip Sync", startingPos + 3, false, false, false, "");
					createDoubleAttribute("lower_lip_ftuck_max", 10, true, "Baldi Lip Sync", startingPos + 4, false, false, false, "");

					createDoubleAttribute("upper_lip_raise_min", -30, true, "Baldi Lip Sync", startingPos + 5, false, false, false, "");
					createDoubleAttribute("upper_lip_raise_max", 20, true, "Baldi Lip Sync", startingPos + 6, false, false, false, "");
					createDoubleAttribute("upper_lip_raise_default", -2.96, true, "Baldi Lip Sync", startingPos + 7, false, false, false, "");

					createDoubleAttribute("cheek_hollow_min", -1.5, true, "Baldi Lip Sync", startingPos + 8, false, false, false, "");
					createDoubleAttribute("cheek_hollow_max", 1.5, true, "Baldi Lip Sync", startingPos + 9, false, false, false, "");

					createDoubleAttribute("lower_lip_roll_min", -20, true, "Baldi Lip Sync", startingPos + 10, false, false, false, "");
					createDoubleAttribute("lower_lip_roll_max", 40, true, "Baldi Lip Sync", startingPos + 11, false, false, false, "");

					createDoubleAttribute("jaw_thrust_min", -10, true, "Baldi Lip Sync", startingPos + 12, false, false, false, "");
					createDoubleAttribute("jaw_thrust_max", 30, true, "Baldi Lip Sync", startingPos + 13, false, false, false, "");

					createDoubleAttribute("lip_corner_zip_min", -20, true, "Baldi Lip Sync", startingPos + 14, false, false, false, "");
					createDoubleAttribute("lip_corner_zip_max", 30, true, "Baldi Lip Sync", startingPos + 15, false, false, false, "");

					createDoubleAttribute("lower_lip_raise_min", -25, true, "Baldi Lip Sync", startingPos + 16, false, false, false, "");
					createDoubleAttribute("lower_lip_raise_max", 15, true, "Baldi Lip Sync", startingPos + 17, false, false, false, "");

					createDoubleAttribute("lip_rounding_min", 0, true, "Baldi Lip Sync", startingPos + 18, false, false, false, "");
					createDoubleAttribute("lip_rounding_max", 20, true, "Baldi Lip Sync", startingPos + 19, false, false, false, "");
					createDoubleAttribute("lip_rounding_default", 6.106, true, "Baldi Lip Sync", startingPos + 20, false, false, false, "");

					createDoubleAttribute("lip_retraction_min", 0, true, "Baldi Lip Sync", startingPos + 21, false, false, false, "");
					createDoubleAttribute("lip_retraction_max", 15, true, "Baldi Lip Sync", startingPos + 22, false, false, false, "");
					createDoubleAttribute("lip_retraction_default", 2.809, true, "Baldi Lip Sync", startingPos + 23, false, false, false, "");

					createDoubleAttribute("tongue_tip_y_min", -2, true, "Baldi Lip Sync", startingPos + 24, false, false, false, "");
					createDoubleAttribute("tongue_tip_y_max", 2, true, "Baldi Lip Sync", startingPos + 25, false, false, false, "");

					createDoubleAttribute("tongue_tip_z_min", -2, true, "Baldi Lip Sync", startingPos + 26, false, false, false, "");
					createDoubleAttribute("tongue_tip_z_max", 2, true, "Baldi Lip Sync", startingPos + 27, false, false, false, "");
					createBoolAttribute("handtune", true, true, "Basic", startingPos + 28, false, false, false, "");
				}
			}

		}
		if (attrName.find("steering.") == 0)
		{
			// update the steering params on the next evaluation cycle
			SmartBody::SBSteerManager* steerManager = SmartBody::SBScene::getScene()->getSteerManager();
			SmartBody::SBSteerAgent* steerAgent = steerManager->getSteerAgent(getName());
			if (steerAgent)
			{
				PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
				ppraiAgent->setSteerParamsDirty(true);
			}
		}
#if 1 // rocket box test		
		if (attrName == "_1testHead")
		{
			scene->run("testHead('"+getName()+"',0.0)");		
		}
		else if (attrName == "_2testGaze")
		{
			scene->run("testGaze('"+getName()+"',0.0)");		
		}
		else if (attrName == "_3testGesture")
		{
			scene->run("testGesture('"+getName()+"',0.0)");		
		}
		else if (attrName == "_4testReach")
		{
			scene->run("testReach('"+getName()+"',0.0)");		
		}
		else if (attrName == "_5testLocomotion")
		{
			scene->run("testLocomotion('"+getName()+"',0.0)");		
		}
		
#endif

		if (attrName.find("saccade.listening.") == 0)
		{
			if (saccade_ct)
				saccade_ct->setBehaviorMode(MeCtSaccade::Listening);
		}
		if (attrName.find("saccade.talking.") == 0)
		{
			if (saccade_ct)
				saccade_ct->setBehaviorMode(MeCtSaccade::Talking);
		}
		if (attrName.find("saccade.thinking.") == 0)
		{
			if (saccade_ct)
				saccade_ct->setBehaviorMode(MeCtSaccade::Thinking);
		}
	}

	SbmCharacter::notify(subject);
	SBPawn::notify(subject);
}

void SBCharacter::interruptFace(double seconds)
{
	if (!head_sched_p)
		return;
	
	SmartBody::SBFaceDefinition* faceDefinition = this->getFaceDefinition();
	if (!faceDefinition)
		return;

	// retrieve any action units being executed
	std::vector<MeCtScheduler2::TrackPtr> tracks = head_sched_p->tracks();

	std::vector<MeCtScheduler2::TrackPtr> tracksToRemove;

	for (unsigned int t = 0; t < tracks.size(); t++)
	{
		MeCtScheduler2::TrackPtr track = tracks[t];
		MeController* controller = track->animation_ct();
		// determine the facial unit associated with this track
		MeCtCurveWriter* curveController = dynamic_cast<MeCtCurveWriter*>(controller);
		if (!curveController)
		{
			MeCtChannelWriter* channelWriterController = dynamic_cast<MeCtChannelWriter*>(controller);
			if (!channelWriterController)
				continue;
		}
		const std::string& controllerName = controller->getName();
		// find a face definition match
		bool visemeExists = faceDefinition->hasViseme(controllerName);
		if (visemeExists)
		{
			tracksToRemove.push_back(track);
			//this->schedule_viseme_trapezoid(controllerName.c_str(), SmartBody::SBScene::getScene()->getSimulationManager()->getTime(), 0, 0, float(seconds), float(seconds));
			//this->pruneControllers();
		}
		else
		{
			ActionUnit* au = faceDefinition->getAUByName(controllerName);
			if (au)
			{
				tracksToRemove.push_back(track);
				//this->schedule_viseme_trapezoid(controllerName.c_str(), SmartBody::SBScene::getScene()->getSimulationManager()->getTime(), 0, 0, float(seconds), float(seconds));
				//this->pruneControllers();
			}
		}
	}
	if (tracksToRemove.size() > 0)
		head_sched_p->remove_tracks(tracksToRemove);

	// reset the default face
	// get any default face poses and set them
	std::vector<std::string> poses = _faceDefinition->getDefaultFacePoses();
	std::vector<float> values = _faceDefinition->getDefaultFaceValues();

	for (unsigned int x = 0; x < poses.size(); x++)
	{
		this->schedule_viseme_trapezoid( poses[x].c_str(), SmartBody::SBScene::getScene()->getSimulationManager()->getTime(), values[x], 9999999.9f, 0, 0 );
	}
}

void SBCharacter::copy( SmartBody::SBCharacter* origChar )
{
	SbmCharacter::copy(origChar);
	if (origChar->getReach()) // original character has reach set
	{
		SmartBody::util::log("Build reach for character %s",getName().c_str());
		origChar->getReach()->build(this);
	}
	SBObject::copyAllAttributes(origChar);	
}

void SBCharacter::setReach( SmartBody::SBReach* reach )
{
	if (!reach) return;
	if (reach->getReachEngineMap().size() != 0)
	{
		_reach = reach;
	}
	else
	{
		SmartBody::util::log("Can't set SBReach '%s', the corresponding reach engine is not initialized. Do reach.build() before set to character.", reach->getReachTag().c_str());
	}
}

SmartBody::SBReach* SBCharacter::getReach()
{
	return _reach;
}

void SBCharacter::setDeformableMeshScale( double meshScale )
{
	SmartBody::Vec3Attribute* meshScaleAttribute = dynamic_cast<SmartBody::Vec3Attribute*>(getAttribute("deformableMeshScale"));
	if (meshScaleAttribute)
	{
		meshScaleAttribute->setValue(SrVec((float) meshScale, (float) meshScale, (float) meshScale));
	}
}

std::string SBCharacter::getPostureName()
{
	MeCtScheduler2::VecOfTrack tracks = posture_sched_p->tracks();
	for (unsigned int i=0;i<tracks.size();i++)
	{
		MeCtScheduler2::TrackPtr tr = tracks[i];
		MeCtMotion* motionCt = dynamic_cast<MeCtMotion*>(tr->animation_ct());
		if (motionCt)
		{
			return motionCt->motion()->getName();
		}
	}
	return "";
}

SBAPI void SBCharacter::addJointTrajectoryConstraint( const std::string& jointName, const std::string& refJointName )
{
	if (jointTrajMap.find(jointName) == jointTrajMap.end())
	{
		jointTrajMap[jointName] = new TrajectoryRecord();
	}
	TrajectoryRecord* trajRecord = jointTrajMap[jointName];
	trajRecord->effectorName = jointName;
	trajRecord->refJointName = refJointName;
	trajRecord->isEnable = true;
}

SBAPI TrajectoryRecord* SBCharacter::getJointTrajectoryConstraint( const std::string& jointName )
{
	if (jointTrajMap.find(jointName) == jointTrajMap.end())
	{
		return NULL;
	}
	return jointTrajMap[jointName];
}

SBAPI std::vector<std::string> SBCharacter::getJointConstraintNames()
{
	std::map<std::string, TrajectoryRecord*>::iterator mi;
	std::vector<std::string> jointConsNames;
	for ( mi  = jointTrajMap.begin();
		  mi != jointTrajMap.end();
		  mi++)
	{
		jointConsNames.push_back(mi->first);
	}
	return jointConsNames;
}


void SBCharacter::createBlendShapeChannel(const std::string& channelName)
{
	this->addBlendShapeChannel(channelName);
}

bool SBCharacter::getUseJointConstraint()
{
	//return false;
	return useJointConstraint;
}

void SBCharacter::setUseJointConstraint( bool bUseConstraint )
{
	useJointConstraint = bUseConstraint;
}

void SBCharacter::setDeformableMeshName(const std::string& meshName)
{
	this->setStringAttribute("deformableMesh", meshName);
}

SBAPI void SBCharacter::setMotionGraph( const std::string& moGraphName )
{
	SBMotionGraphManager* mographManager = SmartBody::SBScene::getScene()->getMotionGraphManager();
	SBMotionGraph* mograph = mographManager->getMotionGraph(moGraphName);
	if (mograph)
		_curMotionGraph = mograph;
}


SBAPI void SBCharacter::startMotionGraphRandomWalk()
{
	if (!_curMotionGraph)
	{
		SmartBody::util::log("Cannot start motion graph. No motion graph available.");
		return;
	}
	std::vector<std::string> nodeNames = _curMotionGraph->getMotionNodeNames();
	SrRandom random;
	random.limits(0, nodeNames.size()-1);
	int nodeIdx = random.geti();
	startMotionGraph(nodeNames[nodeIdx]);
}

SBAPI void SBCharacter::startMotionGraph( const std::string& moNodeName )
{
	if (!motiongraph_ct)
	{
		SmartBody::util::log("Motion Graph Controller is not initialized.");
		return;
	}

	SBMotionNodeState* motionState = motiongraph_ct->getMotionNodeState();
	if (!motionState)
	{
		SmartBody::util::log("Motion Graph Controller is not initialized.");
		return;
	}

	if (!_curMotionGraph)
	{
		SmartBody::util::log("Cannot start motion graph. No motion graph available.");
		return;
	}
	SBMotionNode* startNode = _curMotionGraph->getMotionNode(moNodeName);
	if (!startNode)
	{
		SmartBody::util::log("Cannot start motion graph. The motion node '%s' doesn't exist.", moNodeName.c_str());
		return;
	}

	if (motionState->isRunning())
	{
		SmartBody::util::log("Motion Graph is running. Stop the motion graph before restarting it.");
		return;
	}

	std::vector<float> startWeights;
	startNode->getRandomBlendWeights(startWeights);
	motionState->startMotionState(startNode,startWeights);	
	motiongraph_ct->controller_start();
}

SBAPI void SBCharacter::startMotionGraphWithPath( const std::vector<SrVec>& pathList )
{
	if (!_curMotionGraph)
	{
		SmartBody::util::log("Cannot start motion graph. No motion graph available.");
		return;
	}

	SBMotionNodeState* motionState = motiongraph_ct->getMotionNodeState();
	if (!motionState)
	{
		SmartBody::util::log("Motion Graph Controller is not initialized.");
		return;
	}

	SteerPath moGraphPath;
	std::vector<std::pair<std::string,std::string> > graphEdges;
	moGraphPath.initPath(pathList, 0.1f);

	trajectoryGoalList.clear();
	trajectoryGoalList.resize(pathList.size()*3);
	for (unsigned int i=0;i<pathList.size();i++)
	{
		for (int k=0;k<3;k++)
			trajectoryGoalList[i*3+k] = pathList[i][k];
	}
	_curMotionGraph->synthesizePath(moGraphPath, getSkeleton()->getName(), graphEdges);
	if (graphEdges.size() == 0)
	{
		SmartBody::util::log("Error, the resulting graph traversal is empty.");
		return;
	}
	//SmartBody::util::log("Finish synthesize the paths, graph edge size = %d", graphEdges.size());
	std::string startNodeName = graphEdges[0].first;
	startMotionGraph(startNodeName);	

	for (unsigned int i=0;i<graphEdges.size();i++)
	{
		std::pair<std::string,std::string>& edge = graphEdges[i];			
		//SmartBody::util::log("Edge %d, name = %s %s", i, edge.first.c_str(), edge.second.c_str());
		SBMotionTransitionEdge* motionEdge = _curMotionGraph->getMotionEdge(edge.first,edge.second);
		if (!motionEdge)
		{
			SmartBody::util::log("Cannot find edge %s %s",edge.first.c_str(), edge.second.c_str());
		}
		//SmartBody::util::log("motion edge %d, name = %s", motionEdge, motionEdge->getName().c_str());
		SBMotionNode* node = _curMotionGraph->getMotionNode(edge.second);	
		//SmartBody::util::log("Motion node = %d, namae = %s", node, node->getName().c_str());
		std::vector<float> weights;
		node->getRandomBlendWeights(weights);
		//SmartBody::util::log("Node weights size = %d", weights.size());
		motionState->addNextTransition(motionEdge, weights);
		//SmartBody::util::log("After add next transition");
	}

		
}


void SBCharacter::InitFrameDataMarshalFriendly()
{
   frameDataMarshalFriendly->m_name = new char[ strlen(getName().c_str()) + 1 ];
   strcpy( frameDataMarshalFriendly->m_name, getName().c_str() );

   frameDataMarshalFriendly->x  = 0;
   frameDataMarshalFriendly->y  = 0;
   frameDataMarshalFriendly->z  = 0;
   frameDataMarshalFriendly->rw = 0;
   frameDataMarshalFriendly->rx = 0;
   frameDataMarshalFriendly->ry = 0;
   frameDataMarshalFriendly->rz = 0;
   frameDataMarshalFriendly->m_numJoints = 0;

   frameDataMarshalFriendly->jname = NULL;
   frameDataMarshalFriendly->jx  = NULL;
   frameDataMarshalFriendly->jy  = NULL;
   frameDataMarshalFriendly->jz  = NULL;
   frameDataMarshalFriendly->jrw = NULL;
   frameDataMarshalFriendly->jrx = NULL;
   frameDataMarshalFriendly->jry = NULL;
   frameDataMarshalFriendly->jrz = NULL;
}


void SBCharacter::FreeFrameDataJointsMarshalFriendly()
{
   for ( size_t i = 0; i < frameDataMarshalFriendly->m_numJoints; i++ )
   {
      delete [] frameDataMarshalFriendly->jname[ i ];
   }

   frameDataMarshalFriendly->x  = 0;
   frameDataMarshalFriendly->y  = 0;
   frameDataMarshalFriendly->z  = 0;
   frameDataMarshalFriendly->rw = 0;
   frameDataMarshalFriendly->rx = 0;
   frameDataMarshalFriendly->ry = 0;
   frameDataMarshalFriendly->rz = 0;
   frameDataMarshalFriendly->m_numJoints = 0;

   delete [] frameDataMarshalFriendly->jname;
   delete [] frameDataMarshalFriendly->jx;
   delete [] frameDataMarshalFriendly->jy;
   delete [] frameDataMarshalFriendly->jz;
   delete [] frameDataMarshalFriendly->jrw;
   delete [] frameDataMarshalFriendly->jrx;
   delete [] frameDataMarshalFriendly->jry;
   delete [] frameDataMarshalFriendly->jrz;

   frameDataMarshalFriendly->jname = NULL;
   frameDataMarshalFriendly->jx  = NULL;
   frameDataMarshalFriendly->jy  = NULL;
   frameDataMarshalFriendly->jz  = NULL;
   frameDataMarshalFriendly->jrw = NULL;
   frameDataMarshalFriendly->jrx = NULL;
   frameDataMarshalFriendly->jry = NULL;
   frameDataMarshalFriendly->jrz = NULL;
}


void SBCharacter::FreeFrameDataMarshalFriendly()
{
   FreeFrameDataJointsMarshalFriendly();

   delete [] frameDataMarshalFriendly->m_name;
   frameDataMarshalFriendly->m_name = NULL;
}


SBAPI const SBM_CharacterFrameDataMarshalFriendly & SBCharacter::GetFrameDataMarshalFriendly()
{
   const SkJoint * joint = get_world_offset_joint();

   const SkJointPos * pos = joint->const_pos();
   float x = pos->value( SkJointPos::X );
   float y = pos->value( SkJointPos::Y );
   float z = pos->value( SkJointPos::Z );

   SkJoint::RotType rot_type = joint->rot_type();
   if ( rot_type != SkJoint::TypeQuat )
   {
      //cerr << "ERROR: Unsupported world_offset rotation type: " << rot_type << " (Expected TypeQuat, "<<SkJoint::TypeQuat<<")"<<endl;
   }

   // const_cast because the SrQuat does validation (no const version of value())
   const SrQuat & q = ((SkJoint *)joint)->quat()->value();

   frameDataMarshalFriendly->x = x;
   frameDataMarshalFriendly->y = y;
   frameDataMarshalFriendly->z = z;
   frameDataMarshalFriendly->rw = q.w;
   frameDataMarshalFriendly->rx = q.x;
   frameDataMarshalFriendly->ry = q.y;
   frameDataMarshalFriendly->rz = q.z;

   const std::vector<SkJoint *> & joints  = getSkeleton()->joints();

   if (frameDataMarshalFriendly->m_numJoints == 0 || frameDataMarshalFriendly->m_numJoints != joints.size())
   {
      FreeFrameDataJointsMarshalFriendly();

      frameDataMarshalFriendly->m_numJoints = joints.size();
      frameDataMarshalFriendly->jname = new char * [ frameDataMarshalFriendly->m_numJoints ];
      frameDataMarshalFriendly->jx = new float [ frameDataMarshalFriendly->m_numJoints ];
      frameDataMarshalFriendly->jy = new float [ frameDataMarshalFriendly->m_numJoints ];
      frameDataMarshalFriendly->jz = new float [ frameDataMarshalFriendly->m_numJoints ];
      frameDataMarshalFriendly->jrw = new float [ frameDataMarshalFriendly->m_numJoints ];
      frameDataMarshalFriendly->jrx = new float [ frameDataMarshalFriendly->m_numJoints ];
      frameDataMarshalFriendly->jry = new float [ frameDataMarshalFriendly->m_numJoints ];
      frameDataMarshalFriendly->jrz = new float [ frameDataMarshalFriendly->m_numJoints ];

      for ( size_t i = 0; i < frameDataMarshalFriendly->m_numJoints; i++ )
      {
         frameDataMarshalFriendly->jname[ i ] = new char[ joints[ i ]->getName().length() + 1 ];
         strcpy( frameDataMarshalFriendly->jname[ i ], joints[ i ]->getName().c_str() );
      }
   }


   for ( size_t i = 0; i < joints.size(); i++ )
   {
      SkJoint * j = joints[i];
      SrQuat q = j->quat()->value();

      //printf( "%s %f %f %f %f\n", (const char *)j->name(), q.w, q.x, q.y, q.z );

      float posx = j->pos()->value( 0 );
      float posy = j->pos()->value( 1 );
      float posz = j->pos()->value( 2 );
//      if ( false )
//      {
//         posx += j->offset().x;
//         posy += j->offset().y;
//         posz += j->offset().z;
//      }

      std::string jointName;
      if (j->extName() != "")
         jointName = j->extName();
      else
         jointName = j->jointName();

      frameDataMarshalFriendly->jx[i] = posx;
      frameDataMarshalFriendly->jy[i] = posy;
      frameDataMarshalFriendly->jz[i] = posz;
      frameDataMarshalFriendly->jrw[i] = q.w;
      frameDataMarshalFriendly->jrx[i] = q.x;
      frameDataMarshalFriendly->jry[i] = q.y;
      frameDataMarshalFriendly->jrz[i] = q.z;
   }

   return *frameDataMarshalFriendly;
}

SBAPI std::string SBCharacter::getReachAttachedPawnName(const std::string& reachType)
{
	std::string result = "null";
	//ReachEngineMap* reachEngineMap = NULL;
	//getCurrentReachEngineMap();
	if (getReach())
	{
		MeCtReachEngine* reachEngine = getReach()->getReachEngine(reachType);
		if (reachEngine)
			result = reachEngine->getAttachedPawnName();
		
	}

	return result;
}


/*
SBAPI bool SBCharacter::getUseBlendFaceTextures()
{
	return getBoolAttribute("useBlendFaceTextures");
}

SBAPI void SBCharacter::setUseBlendFaceTextures(bool useBlendFaceTextures)
{
	setBoolAttribute("useBlendFaceTextures",useBlendFaceTextures);
}

SBAPI bool SBCharacter::getUseFaceTextures()
{
	return getBoolAttribute("useFaceTextures");
}

SBAPI void SBCharacter::setUseFaceTextures(bool useFaceTextures)
{
	setBoolAttribute("useFaceTextures",useFaceTextures);

	cerr << "Here";

	if(!useFaceTextures)
		setUseBlendFaceTextures(!useFaceTextures);
}
*/

void SBCharacter::addParserListener(SBParserListener* parserListener)
{
	_parserListener = parserListener;
}

void SBCharacter::removeParserListener()
{
	_parserListener = NULL;
}

SBParserListener* SBCharacter::getParserListener()
{
	return _parserListener;
}


};
