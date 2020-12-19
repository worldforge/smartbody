/*************************************************************
Copyright(C) 2017 University of Southern California

This file is part of Smartbody.

Smartbody is free software : you can redistribute it and / or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Smartbody is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Smartbody.If not, see <http://www.gnu.org/licenses/>.

**************************************************************/



#include <iostream>
#include <sstream>
#include <string>

#include "bml_gesture.hpp"

#include "sb/SBScene.h"
#include "bml_xml_consts.hpp"
#include <sb/SBGestureMap.h>
#include <sb/SBAssetManager.h>
#include <sb/SBGestureMapManager.h>
#include <sb/SBRetargetManager.h>
#include <sb/SBRetarget.h>
#include <sb/SBSteerManager.h>
#include <sb/SBSteerAgent.h>
#include <sb/SBBehavior.h>
#include <sb/SBSkeleton.h>
#include <sb/SBMotion.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBAnimationState.h>
#include <controllers/me_ct_motion.h>
#include <controllers/me_ct_scheduler2.h>
#include <sbm/PPRAISteeringAgent.h>
#include <sb/SBMotion.h>
#include "SBUtilities.h"

using namespace std;
using namespace BML;
using namespace xml_utils;

/*	Usage of bml gestures
 *	- if stroke and stroke_end is defined, poststroke_hold time would be calculated automatically even if there's poststroke_hold given by user
 *	- you can specify idle animation used for gesture holding period
 *	- you can specify joints, scale, frequency for perlin noise added to gesture holding period. premise is no idle animation is specified.
 */

BML::BehaviorRequestPtr BML::parse_bml_gesture( DOMElement* elem, const std::string& unique_id, BehaviorSyncPoints& behav_syncs, bool required, BmlRequestPtr request, SmartBody::SBScene* scene ) 
{
	const XMLCh* animName = elem->getAttribute( BMLDefs::ATTR_NAME );
	const XMLCh* id = elem->getAttribute(BMLDefs::ATTR_ID);
	const XMLCh* lexemeAttr = elem->getAttribute(BMLDefs::ATTR_LEXEME);
	const XMLCh* typeAttr = elem->getAttribute(BMLDefs::ATTR_TYPE);
	const XMLCh* modeAttr = elem->getAttribute(BMLDefs::ATTR_MODE);
	const XMLCh* styleAttr = elem->getAttribute(BMLDefs::ATTR_STYLE);
	const XMLCh* priorityAttr = elem->getAttribute(BMLDefs::ATTR_PRIORITY_JOINT);

	const XMLCh* xVal = elem->getAttribute(BMLDefs::ATTR_X);
	const XMLCh* yVal = elem->getAttribute(BMLDefs::ATTR_Y);
	const XMLCh* zVal = elem->getAttribute(BMLDefs::ATTR_Z);

	std::string animationName;
	std::string localId;
	std::string lexeme;
	std::string type;
	std::string mode;
	std::string style;

	std::string xvalStr;
	std::string yvalStr;
	std::string zvalStr;

	xml_utils::xml_translate(&localId, id);
	xml_utils::xml_translate(&animationName, animName);
	xml_utils::xml_translate(&lexeme, lexemeAttr);
	xml_utils::xml_translate(&type, typeAttr);
	xml_utils::xml_translate(&mode, modeAttr);
	xml_utils::xml_translate(&style, styleAttr);

	xml_utils::xml_translate(&xvalStr, xVal);
	xml_utils::xml_translate(&yvalStr, yVal);
	xml_utils::xml_translate(&zvalStr, zVal);

	bool isAdditive = false;
	std::string additiveStr = xml_parse_string( BMLDefs::ATTR_ADDITIVE, elem, "false" );	
	if( stringICompare(additiveStr,"false") ) 
	{			
		isAdditive = false;
	}
	else //if (stringICompare(attrFootIK,"false"))
	{			
		isAdditive = true;
	}

	std::string emotion = xml_utils::xml_parse_string(BMLDefs::ATTR_EMOTION, elem, "");

	SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*>(request->actor);
	std::string gestureMapName = "";
	std::string mapNameLower = SmartBody::util::toLower(emotion);
	// capitalize the first letter 
	if (mapNameLower.size() > 0)
		mapNameLower[0] = toupper(mapNameLower[0]);
	std::string finalGestureMapName = "gestureMap" + mapNameLower;
	gestureMapName = character->getStringAttribute(finalGestureMapName);

	std::vector<std::string> animationList;
	if (animationName == "")	// If you have assigned the animation name, do not look for the map
	{
		SmartBody::SBGestureMap* gestureMap = SmartBody::SBScene::getScene()->getGestureMapManager()->getGestureMap(gestureMapName);
		if (!gestureMap)
		{
			//SmartBody::util::log("WARNING: BML::parse_bml_gesture(): gesture map '%s' for emotion '%s' on character %s doesn't exist.", gestureMapName.c_str(), finalGestureMapName.c_str(), request->actor->getName().c_str());
			// get the default gesture map if the emotional one isn't available
			if (gestureMapName != "gestureMap")
			{
				gestureMap = SmartBody::SBScene::getScene()->getGestureMapManager()->getGestureMap(character->getStringAttribute("gestureMap"));
			}
			if (!gestureMap)
			{
				SmartBody::util::log("No default gesture map. Gesture will not be played.");
				return BehaviorRequestPtr();
			}
		}

		// Get current posture
		std::string posture = "";
		if (request->actor->posture_sched_p)
		{
			MeCtScheduler2::VecOfTrack tracks = request->actor->posture_sched_p->tracks();
			for (size_t t = 0; t < tracks.size(); t++)
			{
				MeCtMotion* motionCt = dynamic_cast<MeCtMotion*>(tracks[t]->animation_ct());
				if (motionCt)
				{
					posture = motionCt->motion()->getName();
				}
			}
		}
		if (isAdditive)			// if is additive, do not take in consideration of current posture.
		{
			posture = "";
		}
		int useIndex = -1;
		if (request->actor->getBoolAttribute("gestureRequest.useLastRandomGesture"))
		{
			int nextIndex = request->actor->getIntAttribute("gestureRequest.lastGestureRandomIndex");
			std::string pastGestureChoicesStr = request->actor->getStringAttribute("gestureRequest.curGestureRandom");
			std::vector<std::string> tokens;
			SmartBody::util::tokenize(pastGestureChoicesStr, tokens, "|");
			if (tokens.size() > nextIndex)
				useIndex = atoi(tokens[nextIndex].c_str());
			request->actor->setIntAttribute("gestureRequest.lastGestureRandomIndex", nextIndex + 1);
		}
		int appliedIndex = 0;
		animationName = gestureMap->getGestureByInfo(lexeme, type, mode, style, posture, request->actor->getStringAttribute("gesturePolicy"), useIndex, appliedIndex);
			
		std::string str = request->actor->getStringAttribute("gestureRequest.lastGestureRandom");
		if (str != "")
			str += "|";
		str += std::to_string(appliedIndex);
		request->actor->setStringAttribute("gestureRequest.lastGestureRandom", str);

		animationList = gestureMap->getGestureListByInfo(lexeme, type, mode, style, posture);
		if (animationName == "")
		{
			SmartBody::util::log("Could not find animation for: %s %s %s %s %s", lexeme.c_str(), type.c_str(), mode.c_str(), style.c_str(), posture.c_str());
			return BehaviorRequestPtr();
		}
		else
		{
			//SmartBody::util::log("bml_gesture gesture retrieval: %s", animationName.c_str());
		}
	}

	if (animationName == "")
	{
		SmartBody::util::log("WARNING: BML::parse_bml_gesture(): invalid animation name");
		return BehaviorRequestPtr();
	}

	// play the animation
	if (!request->actor->motion_sched_p)
	{
		SmartBody::util::log("Character %s does not have a motion scheduler, so cannot schedule motion.", request->actor->getName().c_str());
		return BehaviorRequestPtr();
	}
	SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getAssetManager()->getMotion(animationName);

	if (!motion) // gesture might be a blend, and not a simple motion
	{
		SmartBody::SBAnimationBlend* blend = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(animationName);
		if (!blend)
		{
			SmartBody::util::log("Could not find blend named '%s' for gestures. Gesture will not be played.", animationName.c_str());
			return BehaviorRequestPtr();
		}

		// compose a new motion consisting of the motion blend 
		// including new sync points. Use that new motion as input into the gesture pipeline.
		SrVec params(atof(xvalStr.c_str()), atof(yvalStr.c_str()), atof(zvalStr.c_str()));

		if (request->actor->getBoolAttribute("useDiscreteBlends"))
		{
			for (int i = 0; i < 3; i++)
			{
				if (params[0] < .6)
					params[0] = 0.0;
				else
					params[i] = 1.0;
			}
		}
		// optimization; if the blend parameters dictate no blending needed, return the motion directly
		SmartBody::SBAnimationBlend1D* blend1D = dynamic_cast<SmartBody::SBAnimationBlend1D*>(blend);
		if (blend1D)
		{
			int numMotions = blend1D->getNumMotions();
			for (int m = 0; m < numMotions; m++)
			{
				std::string motionName = blend1D->getMotion(m);
				double val;
				blend1D->getParameter(motionName, val);
				if (fabs(val - params[0]) < .01)
				{
					motion = scene->getMotion(motionName);
					if (motion)
						break;
				}
			}
		}

		if (!motion)
		{
			std::stringstream strstr;
			strstr.precision(1);
			strstr << params[0] << "_" << params[1] << "_" << "_" << params[2];
			std::string tempString = strstr.str();
			std::string nameStr = SmartBody::util::replace(tempString, ".", "_");
			std::stringstream strstr2;
			strstr2 << blend->getName() << "_" << nameStr;
			std::string blendedMotionName = strstr2.str();

			motion = SmartBody::SBScene::getScene()->getMotion(blendedMotionName);
			if (!motion)
			{
				motion = blend->createMotionFromBlend(params, character, blendedMotionName, 30.0f);
				if (!motion)
				{
					SmartBody::util::log("Blend %s triggered from gesture is missing animation.", blend->getName().c_str());
					return BehaviorRequestPtr();
				}
				// perform a retargeting if needed
				std::string blendSkeleton = blend->getBlendSkeleton();
				if (blendSkeleton != "")
				{
					motion->setMotionSkeletonName(blendSkeleton);
				}
			}
		}
	}

	if (motion)
	{
		if (request->actor->getBoolAttribute("gestureSimple"))
		{
			// don't perform any specialized gesture handling, simply play the mapped animation

			double twarp = 1.0;
			MeCtMotion* motionCt = new MeCtMotion();

			// Name controller with behavior unique_id
			ostringstream name;
			name << unique_id << ' ' << motion->getName();
			motionCt->setName(name.str().c_str());  // TODO: include BML act and behavior ids
			motionCt->init(const_cast<SbmCharacter*>(request->actor), motion, 0.0, 1.0 / twarp);
			// remove all syncs except for stroke to prevent gesture scaling
			if (behav_syncs.sync_stroke()->sync())
			{
				behav_syncs.sync_start()->set_time(TIME_UNSET);
				behav_syncs.sync_start()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_ready()->set_time(TIME_UNSET);
				behav_syncs.sync_ready()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_stroke_start()->set_time(TIME_UNSET);
				behav_syncs.sync_stroke_start()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_stroke_end()->set_time(TIME_UNSET);
				behav_syncs.sync_stroke_end()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_relax()->set_time(TIME_UNSET);
				behav_syncs.sync_relax()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_end()->set_time(TIME_UNSET);
				behav_syncs.sync_end()->sync()->parent = SyncPointPtr();
			}
			else if (behav_syncs.sync_start()->sync())
			{
				behav_syncs.sync_ready()->set_time(TIME_UNSET);
				behav_syncs.sync_ready()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_stroke_start()->set_time(TIME_UNSET);
				behav_syncs.sync_stroke_start()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_stroke_end()->set_time(TIME_UNSET);
				behav_syncs.sync_stroke_end()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_relax()->set_time(TIME_UNSET);
				behav_syncs.sync_relax()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_end()->set_time(TIME_UNSET);
				behav_syncs.sync_end()->sync()->parent = SyncPointPtr();
			}
			else if (behav_syncs.sync_ready()->sync())
			{
				behav_syncs.sync_start()->set_time(TIME_UNSET);
				behav_syncs.sync_start()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_stroke_start()->set_time(TIME_UNSET);
				behav_syncs.sync_stroke_start()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_stroke_end()->set_time(TIME_UNSET);
				behav_syncs.sync_stroke_end()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_relax()->set_time(TIME_UNSET);
				behav_syncs.sync_relax()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_end()->set_time(TIME_UNSET);
				behav_syncs.sync_end()->sync()->parent = SyncPointPtr();
			}
			else if (behav_syncs.sync_relax()->sync())
			{
				behav_syncs.sync_start()->set_time(TIME_UNSET);
				behav_syncs.sync_start()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_ready()->set_time(TIME_UNSET);
				behav_syncs.sync_ready()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_stroke_start()->set_time(TIME_UNSET);
				behav_syncs.sync_stroke_start()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_stroke_end()->set_time(TIME_UNSET);
				behav_syncs.sync_end()->set_time(TIME_UNSET);
				behav_syncs.sync_end()->sync()->parent = SyncPointPtr();
			}
			else if (behav_syncs.sync_stroke_start()->sync())
			{
				behav_syncs.sync_start()->set_time(TIME_UNSET);
				behav_syncs.sync_start()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_ready()->set_time(TIME_UNSET);
				behav_syncs.sync_ready()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_stroke_end()->set_time(TIME_UNSET);
				behav_syncs.sync_stroke_end()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_relax()->set_time(TIME_UNSET);
				behav_syncs.sync_relax()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_end()->set_time(TIME_UNSET);
				behav_syncs.sync_end()->sync()->parent = SyncPointPtr();
			}
			else if (behav_syncs.sync_stroke_end()->sync())
			{
				behav_syncs.sync_start()->set_time(TIME_UNSET);
				behav_syncs.sync_start()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_ready()->set_time(TIME_UNSET);
				behav_syncs.sync_ready()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_stroke_start()->set_time(TIME_UNSET);
				behav_syncs.sync_stroke_start()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_relax()->set_time(TIME_UNSET);
				behav_syncs.sync_relax()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_end()->set_time(TIME_UNSET);
				behav_syncs.sync_end()->sync()->parent = SyncPointPtr();
			}
			else if (behav_syncs.sync_end()->sync())
			{
				behav_syncs.sync_start()->set_time(TIME_UNSET);
				behav_syncs.sync_start()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_ready()->set_time(TIME_UNSET);
				behav_syncs.sync_ready()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_stroke_start()->set_time(TIME_UNSET);
				behav_syncs.sync_stroke_start()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_stroke_end()->set_time(TIME_UNSET);
				behav_syncs.sync_stroke_end()->sync()->parent = SyncPointPtr();
				behav_syncs.sync_relax()->set_time(TIME_UNSET);
				behav_syncs.sync_relax()->sync()->parent = SyncPointPtr();
			}
			//behav_syncs.printSyncTimes();
			BehaviorRequestPtr behavPtr(new MotionRequest(unique_id, localId, motionCt, request->actor->motion_sched_p, behav_syncs));
			return behavPtr;
		}




		SmartBody::SBMotion* mForCt = motion;
		MeCtMotion* motionCt = new MeCtMotion();
		if (isAdditive)
		{
			mForCt = motion->getOffset();
			motionCt->isAdditive(true);
		}
		else
			mForCt = motion;

		// Name controller with behavior unique_id
		ostringstream name;
		name << unique_id << ' ' << motion->getName();
		motionCt->setName(name.str().c_str());  // TODO: include BML act and behavior ids
		
		// pre stroke hold
		float prestrokehold = (float)xml_utils::xml_parse_double(BMLDefs::ATTR_PRESTROKE_HOLD, elem, -1.0);
		std::string prestrokehold_idlemotion = xml_utils::xml_parse_string(BMLDefs::ATTR_PRESTROKE_HOLD_IDLEMOTION, elem);
		SmartBody::SBMotion* preIdleMotion = SmartBody::SBScene::getScene()->getMotion(prestrokehold_idlemotion);
		if (prestrokehold > 0)
			mForCt = dynamic_cast<SmartBody::SBMotion*>(motion->buildPrestrokeHoldMotion(prestrokehold, preIdleMotion));
		float poststrokehold = (float)xml_utils::xml_parse_double(BMLDefs::ATTR_POSTSTROKE_HOLD, elem, -1.0);
		std::string poststrokehold_idlemotion = xml_utils::xml_parse_string(BMLDefs::ATTR_POSTSTROKE_HOLD_IDLEMOTION, elem);
		SmartBody::SBMotion* postIdleMotion = SmartBody::SBScene::getScene()->getMotion(poststrokehold_idlemotion);
		
		// post stroke hold (it's alternative to setting stroke and relax time)
		std::string joints = xml_utils::xml_parse_string(BMLDefs::ATTR_JOINT_RANGE, elem);
		float scale = (float)xml_utils::xml_parse_double(BMLDefs::ATTR_SCALE, elem, 0.02f);
		float freq = (float)xml_utils::xml_parse_double(BMLDefs::ATTR_FREQUENCY, elem, 0.03f);
		std::string strokeString = xml_utils::xml_parse_string(BMLDefs::ATTR_STROKE, elem);
		std::string relaxString = xml_utils::xml_parse_string(BMLDefs::ATTR_RELAX, elem);
		if (poststrokehold > 0 && (strokeString == "" || relaxString == ""))
		{
			std::vector<std::string> jointVec;
			SmartBody::util::tokenize(joints, jointVec);
			mForCt = dynamic_cast<SmartBody::SBMotion*>(mForCt->buildPoststrokeHoldMotion(poststrokehold, jointVec, scale, freq, postIdleMotion));
			SmartBody::SBMotion* sbMForCT = dynamic_cast<SmartBody::SBMotion*>(mForCt);
			if (sbMForCT)
				sbMForCT->setMotionSkeletonName(motion->getMotionSkeletonName());
		}

		// gesture + locomotion
		SmartBody::SBCharacter* sbCharacter = dynamic_cast<SmartBody::SBCharacter*>(request->actor);
		bool isInLocomotion = false;
		SmartBody::SBSteerManager* steerManager = SmartBody::SBScene::getScene()->getSteerManager();
		SmartBody::SBSteerAgent* steerAgent = steerManager->getSteerAgent(request->actor->getName());
		if (steerAgent)
		{
			for (size_t i = 0; i < request->behaviors.size(); ++i)
			{
				if ((request->behaviors)[i]->local_id == "locomotion")
				{
					isInLocomotion = true;
					break;
				}
			}
			PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
			if (ppraiAgent->isInLocomotion())
				isInLocomotion = true;
		}
		if (isInLocomotion)
		{
			std::vector<std::string> jointNames = sbCharacter->getSkeleton()->getUpperBodyJointNames();
			motionCt->init( const_cast<SbmCharacter*>(request->actor), mForCt, jointNames);
		}
		else
		{
			motionCt->init( const_cast<SbmCharacter*>(request->actor), mForCt, 0.0, 1.0);
		}

		int priority = xml_utils::xml_parse_int(BMLDefs::ATTR_PRIORITY, elem, 0);
		BehaviorRequestPtr behavPtr(new GestureRequest( unique_id, localId, motionCt, request->actor->motion_sched_p, behav_syncs, animationList, joints, scale, freq, priority) );
		return behavPtr; 
	} 
	else 
	{
		SmartBody::util::log("WARNING: BML::parse_bml_gesture(): behavior \"%s\": name=\"%s\" not loaded; ignoring behavior.", unique_id.c_str(), animationName.c_str());
		return BehaviorRequestPtr();
	}
}
