/*
 *  me_ct_param_animation.cpp - part of Motion Engine and SmartBody-lib
 *  Copyright (C) 2008  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 */

#include "controllers/me_ct_param_animation.h"

#include <sb/SBCharacter.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBAnimationTransitionRule.h>
#include <sb/SBMotionBlendBase.h>
#include <sb/SBSkeleton.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBScene.h>
#include <sb/SBSceneListener.h>
#include <sb/SBRetargetManager.h>
#include <sb/SBRetarget.h>
#include "SBUtilities.h"
#include <sr/sr_mat.h>

#include <utility>
#include "controllers/MotionAnalysis.h"

std::string MeCtParamAnimation::Context::CONTEXT_TYPE = "MeCtParamAnimation::Context";
std::string MeCtParamAnimation::CONTROLLER_TYPE = "MeCtParamAnimation";

#define debug 0

ScheduleType::ScheduleType()
{
	wrap = PABlendData::Loop;
	schedule = PABlendData::Queued;
	blend = PABlendData::Overwrite;
	jName = "";
	timeOffset = 0.0;
	stateTimeOffset = 0.0;
	stateTimeTrim = 0.0;
	transitionLen = -1.0;
	directPlay = false;
	playSpeed = 1.0;
	duration = -1.0;
}


void MeCtParamAnimation::Context::child_channels_updated( MeController* child )
{
}

//MeCtParamAnimation::MeCtParamAnimation() : MeCtContainer(new MeCtParamAnimation::Context(this))
//{
//	character = nullptr;
//	woWriter = nullptr;
//	smoothAngVel = 0.f;
//	smoothVel = SrVec();
//}

MeCtParamAnimation::MeCtParamAnimation(SbmCharacter* c, MeCtChannelWriter* wo) : MeCtContainer(*c, new MeCtParamAnimation::Context(this)), woWriter(wo)
{
	character = dynamic_cast<SmartBody::SBCharacter*>(c);
	baseJointName = "base";
	curStateData = nullptr;
	nextStateData = nullptr;
	transitionManager = nullptr;
	waitingList.clear();
	prevGlobalTime = character->_scene.getSimulationManager()->getTime();
	smoothAngVel = 0.f;
	smoothVel = SrVec();
}

MeCtParamAnimation::~MeCtParamAnimation()
{
	reset();
}

void MeCtParamAnimation::controller_map_updated()
{
}

SkChannelArray& MeCtParamAnimation::controller_channels()
{
	return channels;
}

double MeCtParamAnimation::controller_duration()
{
	return -1;
}


bool MeCtParamAnimation::isInTransition()
{
	if (transitionManager && transitionManager->active)
	{
		return true;
	}
	
	return false;
}

bool MeCtParamAnimation::controller_evaluate(double t, MeFrameData& frame)
{	
	double timeStep = t - prevGlobalTime;
	prevGlobalTime = t;

	// auto scheduling
	autoScheduling(t);
	
	// make sure there is always a pseudo idle state running in the system
	if ((!curStateData && !nextStateData) ||
		!curStateData)
	{
		if (!hasPABlend(PseudoIdleState))
		{			
			std::vector<double> weights;
			schedule(nullptr, weights);
			return true;
		}
	}	
	if (curStateData)
	{
		if (curStateData->state && // the state exist
			curStateData->state->stateName != PseudoIdleState &&  // not a idle state
			nextStateData == nullptr && (curStateData->wrapMode == PABlendData::Once) && // only played once
			curStateData->timeManager->getNormalizeLocalTime() >= (curStateData->timeManager->getDuration() - curStateData->transitionLength)) // about to finish
		{			
			if (!hasPABlend(PseudoIdleState))
			{
				//SmartBody::util::log("no current state, and the state is about to finish, scedule nullptr state");
				std::vector<double> weights;
				ScheduleType scType;
				scType.transitionLen = curStateData->transitionLength;
				schedule(nullptr, weights, scType);
			}
		}
			
	}
	//------

	// dealing with transition
	
	if (transitionManager)
	{
		if (curStateData == nullptr || nextStateData == nullptr) {
			std::string errorInfo;
			errorInfo = character->getName() + "'s animation state transition warning. ";
			if (curStateData) {
				if (curStateData->state)
					errorInfo += "current state: " + curStateData->state->stateName;
				else
					errorInfo += "current state: null ";
			}
			if (nextStateData) {
				if (nextStateData->state)
					errorInfo += "next state: " + nextStateData->state->stateName;
				else
					errorInfo += "next state: null ";
			}
			SmartBody::util::log("%s", errorInfo.c_str());

			if (curStateData == nullptr && nextStateData != nullptr) {
				if (nextStateData->state)
					SmartBody::util::log("would start state %s now", nextStateData->state->stateName.c_str());
				curStateData = std::move(nextStateData);
				curStateData->active = true;
				transitionManager.reset();
				return true;
			}
			if (curStateData != nullptr && nextStateData == nullptr) {
				SmartBody::util::log("scheduling problem, please check the corresponding time marks for two states.");
				reset();
				return false;
			}
		}

		if (!transitionManager->blendingMode)
		{
			//SmartBody::util::log("Transition align");
			transitionManager->align(curStateData.get(), nextStateData.get());
		}
		else
		{
			if (transitionManager->active)
			{
				//SmartBody::util::log("Start transition");
				SrBuffer<float> buffer1;
				buffer1.size(frame.buffer().size());
				buffer1 = frame.buffer();
				SrBuffer<float> buffer2;
				buffer2.size(frame.buffer().size());
				buffer2 = frame.buffer();
				SrMat transformMat;		

				SrMat curBaseMat;
				SrMat nextBaseMat;
				double playSpeed = max(curStateData->playSpeed, nextStateData->playSpeed);
				bool directPlay = (curStateData->directPlay || nextStateData->directPlay);
				bool pseudoTransition = (curStateData->getStateName() == PseudoIdleState || nextStateData->getStateName() == PseudoIdleState);				
				if (pseudoTransition && !directPlay && (!curStateData->isPartialBlending() && !nextStateData->isPartialBlending())) // first time && not partial blending case
				{
					bool transitionIn = (curStateData->getStateName() == PseudoIdleState);
					curStateData->evaluateTransition(timeStep * transitionManager->getSlope(), buffer1, transitionIn);
					nextStateData->evaluateTransition(timeStep, buffer2, transitionIn);
					curBaseMat = curStateData->woManager->getBaseTransformMat();
					nextBaseMat = nextStateData->woManager->getBaseTransformMat();

					if (!transitionIn) // update base rotation & translation
					{						
						SrMat origBase = curStateData->woManager->getBaseMatFromBuffer(buffer1);//combineMat(curStateData->woManager->getBaseMatFromBuffer(buffer1),curBaseMat);
						SrMat newBuffBase = origBase;
						newBuffBase = origBase*nextStateData->woManager->getCurrentBaseTransformMat();
						curStateData->woManager->setBufferByBaseMat(newBuffBase,buffer1);	
						curBaseMat = SrMat();//nextBaseMat.inverse();//SrMat();//curBaseMat*nextStateData->woManager->getBaseTransformMat().inverse();
						nextBaseMat = SrMat();//nextBaseMat.inverse();//SrMat();//nextBaseMat.inverse();
					}

					if (!transitionManager->startTransition)
					{
						SrVec pos;
						if (transitionIn)
						{
							// update base transform mat
							curBaseMat = curStateData->woManager->getCurrentBaseTransformMat();
							pos = curBaseMat.get_translation();
							//SmartBody::util::log("startTransistion In, curBase = %f %f %f",pos[0],pos[1],pos[2]);
						}
						else
						{							
							curBaseMat = curBaseMat*nextStateData->woManager->getCurrentBaseTransformMat().inverse();//SrMat();//nbMat.inverse();							
							pos = curBaseMat.get_translation();
							SrVec origPos = nextStateData->woManager->getCurrentBaseTransformMat().get_translation();
						}
						transitionManager->startTransition = true;
					}					
				}
				else // proceed as usual						
				{
					curStateData->evaluate(timeStep * transitionManager->getSlope(), buffer1);
					nextStateData->evaluate(timeStep, buffer2);
					curBaseMat = curStateData->woManager->getBaseTransformMat();
					nextBaseMat = nextStateData->woManager->getBaseTransformMat();
				}			
				
				if (directPlay)
				{
					// clean up base mat if cur or next state is pseudo idle state
					if (curStateData->getStateName() == PseudoIdleState)
					{
						curBaseMat = SrMat::id;												
					}
					else if (nextStateData->getStateName() == PseudoIdleState)
					{
						nextBaseMat = SrMat::id;							
					}
				}
			
				transitionManager->blending(frame.buffer(), buffer1, buffer2, transformMat, curBaseMat, nextBaseMat, timeStep, _context, directPlay);
				transitionManager->step(timeStep*playSpeed);

				// a hack to also transition the IK post-processing results from/to pseudo-idle pose
				float currentTrajBlendWeight = character->getJointTrajBlendWeight();
				bool useJointConstraint = character->getUseJointConstraint();
				if (curStateData->getStateName() == PseudoIdleState && !useJointConstraint) // transition from idle pose, and poseture doesn't provide IK traj
				{	
					character->setJointTrajBlendWeight(1.f - transitionManager->getCurrentTransitionWeight());										
					updateJointTrajectory(nextStateData.get());
				}
				else if (nextStateData->getStateName() == PseudoIdleState && !useJointConstraint) // transition to idle pose, and poseture doesn't provide IK traj
				{
					character->setJointTrajBlendWeight(transitionManager->getCurrentTransitionWeight());	
					updateJointTrajectory(curStateData.get());
				}		
				else // blend the joint trajectory from current state to next state
				{
					std::vector<SrVec> poseStatePosList; 
					std::vector<SrVec> curStatePosList, nextStatePostList;

					getCharacterJointTrajectory(poseStatePosList);
				
					if (curStateData->getStateName() != PseudoIdleState)
					{
						updateJointTrajectory(curStateData.get());
						getCharacterJointTrajectory(curStatePosList);
					}										
					else
					{
						curStatePosList = poseStatePosList;
					}
					
					if (nextStateData->getStateName() != PseudoIdleState)
					{
						updateJointTrajectory(nextStateData.get());
						getCharacterJointTrajectory(nextStatePostList);
					}
					else
					{
						nextStatePostList = poseStatePosList;
					}
					

					std::vector<std::string> jointConsNames = character->getJointConstraintNames();
					float transitionWeight = transitionManager->getCurrentTransitionWeight();
					for (unsigned int i=0;i<jointConsNames.size();i++)
					{
						SmartBody::TrajectoryRecord* traj = character->getJointTrajectoryConstraint(jointConsNames[i]);
						traj->jointTrajGlobalPos = nextStatePostList[i]*(1.f-transitionWeight) + curStatePosList[i]*transitionWeight;
					}
				}


				if (!curStateData->isPartialBlending() && !nextStateData->isPartialBlending())
					updateWo(transformMat, woWriter, frame.buffer());
				return true;
			}
			else // finish transition
			{
				//if (curStateData->getStateName() == PseudoIdleState)
				//	character->setJointTrajBlendWeight(1.f);
				//else if (nextStateData->getStateName() == PseudoIdleState)
				//	character->setJointTrajBlendWeight(0.f);

				transitionManager.reset();
				curStateData = std::move(nextStateData);
			}
		}
	} 
	

	// if there's only one state, update current state
	if (curStateData && curStateData->state && curStateData->state->stateName != PseudoIdleState)
	{
		///////////////////////////////////////////////////////////////////////
		// check automatic transitions
		auto& scene = getScene();
		SmartBody::SBAnimationBlendManager* blendManager = scene.getBlendManager();
		PABlend* blend = curStateData->state;
		auto* sbstate = dynamic_cast<SmartBody::SBAnimationBlend*>(blend);
		std::vector<std::string> blends = blendManager->getTransitionBlends(blend->stateName);
		for (auto & iter : blends)
		{
			SmartBody::SBAnimationTransition* transition = blendManager->getTransition(blend->stateName, iter);
			if (transition)
			{
				SmartBody::SBAnimationTransitionRule* rule = transition->getTransitionRule();
				if (rule)
				{
					auto* sbchar = dynamic_cast<SmartBody::SBCharacter*>(character);
					auto* sbblend = dynamic_cast<SmartBody::SBAnimationBlend*>(blend);
					bool ret = rule->check(sbchar, sbblend);
					if (ret)
					{
						schedule(transition->getDestinationBlend(), 0, 0, 0);
					}
				}

			}
		}
		///////////////////////////////////////////////////////////////////////

		
		if (curStateData->active)
		{
			curStateData->evaluate(timeStep, frame.buffer());	
			if (!curStateData->isPartialBlending())
				updateWo(curStateData->woManager->getBaseTransformMat(), woWriter, frame.buffer());
#if 1 // disable IK post processing for now, until it is stable enough.
			if (character && character->getBoolAttribute("ikPostFix") && curStateData->getStateName().find("Locomotion") != std::string::npos)
			{
				SrMat mat = character->get_world_offset();
				updateIK(curStateData.get(), mat, curStateData->woManager->getBaseTransformMat(), (float)timeStep,frame.buffer());
				
				character->setWorldOffset(ikScenario.ikGlobalMat);
				float woYOffset = ikScenario.ikGlobalMat.get_translation().y;

				JointChannelId baseChanID, baseBuffId;
				baseChanID.y = _context->channels().search(SbmPawn::WORLD_OFFSET_JOINT_NAME, SkChannel::YPos);	
				baseBuffId.y = _context->toBufferIndex(baseChanID.y);	
				frame.buffer()[baseBuffId.y] = woYOffset;	

					
			}

			// update IK trajectory			
			updateJointTrajectory(curStateData.get());
#endif				
			return true;
		}
		else
		{
			curStateData.reset();
		}
	}
	return true;
}

void MeCtParamAnimation::setBaseJointName(const std::string& name)
{
	baseJointName = name;
}

const std::string& MeCtParamAnimation::getBaseJointName()
{
	return baseJointName;
}

void MeCtParamAnimation::schedule(PABlend* state, double x, double y, double z, PABlendData::WrapMode wrap, PABlendData::ScheduleMode schedule, PABlendData::BlendMode blend, std::string jName, double timeOffset, double stateTimeOffset, double stateTimeTrim, double transitionLen, bool directPlay )
{
	std::vector<double> weights;
	weights.resize(state->getNumMotions());
	auto* state0D = dynamic_cast<SmartBody::SBAnimationBlend0D*>(state);
	if (state0D)
	{
		if (state->getNumMotions() > 0)
			weights[0] = 1.0f;
	}
	else
	{
		auto* state1D = dynamic_cast<SmartBody::SBAnimationBlend1D*>(state);
		if (state1D)
		{
			state->getWeightsFromParameters(x, weights);
		}
		else
		{
			auto* state2D = dynamic_cast<SmartBody::SBAnimationBlend2D*>(state);
			if (state2D)
			{
				state->getWeightsFromParameters(x, y, weights);
			}
			else
			{
				auto* state3D = dynamic_cast<SmartBody::SBAnimationBlend3D*>(state);
				if (state3D)
				{
					state->getWeightsFromParameters(x, y, z, weights);
				}
				else
				{
					auto* blendBase = dynamic_cast<SmartBody::SBMotionBlendBase*>(state);
					if (blendBase)
					{
						state->getWeightsFromParameters(x, y, z, weights);
					}
					else
					{
						SmartBody::util::log("Unknown state type. What is this?");
					}
				}
			}
		}
	}
	this->schedule(state, weights, wrap, schedule, blend, std::move(jName), timeOffset, stateTimeOffset, stateTimeTrim, transitionLen, directPlay);
}

void MeCtParamAnimation::schedule( PABlend* state, const std::vector<double>& weights, const ScheduleType& scType )
{
	//schedule(state,weights,scType.wrap,scType.schedule,scType.blend,scType.jName, scType.timeOffset,scType.stateTimeOffset, scType.stateTimeTrim, scType.transitionLen, scType.directPlay);
	PABlendData::WrapMode wrap = scType.wrap;
	PABlendData::ScheduleMode scheduleMode = scType.schedule;
	PABlendData::BlendMode blend = scType.blend;
	std::string jName = scType.jName;
	double timeOffset = scType.timeOffset;
	double stateTimeOffset = scType.stateTimeOffset;
	double stateTimeTrim = scType.stateTimeTrim;
	double transitionLen = scType.transitionLen;
	double playSpeed = scType.playSpeed;
	bool directPlay = scType.directPlay;
	double desireDuration = scType.duration;

	

	ScheduleUnit unit;
	auto* animState = dynamic_cast<SmartBody::SBAnimationBlend*>(state);
	if (animState)
	{
		animState->validateState(); // to make sure the animaion state is valid before schedule it
	}

	

	// schedule
	unit.weights = weights;
	unit.data = state;
	unit.wrap = wrap;
	unit.schedule = scheduleMode;
	unit.blend = blend;
	unit.partialJoint = jName;
	unit.time = getScene().getSimulationManager()->getTime() + timeOffset;
	unit.stateTimeOffset = (float)stateTimeOffset;
	unit.stateTimeTrim = (float)stateTimeTrim;
	unit.directPlay = directPlay;
	unit.playSpeed = (float)playSpeed;
	unit.duration = (float)desireDuration;

	//SmartBody::util::log("unit schedule mode = %d",unit.schedule);

	auto unitTransitionLen = (float)defaultTransition; // set this to initial default
	// override if the default attribute exists
	if (character->hasAttribute("defaultTransition") && character->getDoubleAttribute("defaultTransition") >= 0)
	{
		unitTransitionLen = (float)character->getDoubleAttribute("defaultTransition");
	}
	// override if specified in the blend scheduling
	if (transitionLen >= 0)
	{
		unitTransitionLen = (float)transitionLen;
	}
	unit.transitionLength = unitTransitionLen;
	// make sure the weights are valid for non-pseudoidle state
	if (unit.weights.empty() && unit.data != nullptr)
	{
		if (unit.data->stateName != PseudoIdleState)
		{
			SmartBody::util::log("MeCtParamAnimation::schedule Warning: state %s has no weights assigned.", unit.data->stateName.c_str());
			unit.weights.resize(unit.data->getNumMotions());
			for (int i = 0; i < unit.data->getNumMotions(); i++)
			{
				if (i == 0)
					unit.weights[i] = 1.0;
				else
					unit.weights[i] = 0.0;
			}
		}
	}
	// make sure there's motion for non-pseudoidle state
	if (unit.data != nullptr)
	{
		if (unit.data->stateName != PseudoIdleState && unit.data->getNumMotions() == 0)
		{
			SmartBody::util::log("MeCtParamAnimation::schedule ERR: state %s has no motions attached.", unit.data->stateName.c_str());
			return;
		}
	}
	waitingList.emplace_back(unit);
}

void MeCtParamAnimation::schedule(PABlend* blendData, const std::vector<double>& weights, PABlendData::WrapMode wrap, PABlendData::ScheduleMode scheduleMode, PABlendData::BlendMode blend, std::string jName, double timeOffset, double stateTimeOffset, double stateTimeTrim, double transitionLen, bool directPlay)
{
	// Feng : rewrite this part to use the object scType so we don't need to indefinitely change this schedule function everytime we add another paramter
	ScheduleType scType;
	scType.wrap = wrap;
	scType.schedule = scheduleMode;
	scType.blend = blend;
	scType.jName = std::move(jName);
	scType.timeOffset = timeOffset;
	scType.stateTimeOffset = stateTimeOffset;
	scType.stateTimeTrim = stateTimeTrim;
	scType.transitionLen = transitionLen;
	scType.directPlay = directPlay;

	schedule(blendData,weights,scType);

#if 0
	ScheduleUnit unit;
	SmartBody::SBAnimationBlend* animState = dynamic_cast<SmartBody::SBAnimationBlend*>(blendData);
	if (animState)
	{
		animState->validateState(); // to make sure the animaion state is valid before schedule it
	}

	// schedule
	unit.weights = weights;
	unit.data = blendData;
	unit.wrap = wrap;
	unit.schedule = scheduleMode;
	unit.blend = blend;
	unit.partialJoint = jName;
	unit.time = mcuCBHandle::singleton().time + timeOffset;
	unit.stateTimeOffset = (float)stateTimeOffset;
	unit.stateTimeTrim = (float)stateTimeTrim;
	unit.directPlay = directPlay;

	//SmartBody::util::log("unit schedule mode = %d",unit.schedule);
	
	float unitTransitionLen = (float)defaultTransition; // set this to initial default	
	// override if the default attribute exists
	if (character->hasAttribute("defaultTransition") && character->getDoubleAttribute("defaultTransition") >= 0)
	{
		unitTransitionLen = (float)character->getDoubleAttribute("defaultTransition");
	}
	// override if specified in the blend scheduling
	if (transitionLen >= 0)
	{
		unitTransitionLen = (float)transitionLen;
	}
	unit.transitionLength = unitTransitionLen;
	
	
	
	

	// make sure the weights are valid for non-pseudoidle state
	if (unit.weights.size() == 0 && unit.data != nullptr)
	{
		if (unit.data->stateName != PseudoIdleState)
		{
			SmartBody::util::log("MeCtParamAnimation::schedule Warning: state %s has no weights assigned.", unit.data->stateName.c_str());
			unit.weights.resize(unit.data->getNumMotions());
			for (int i = 0; i < unit.data->getNumMotions(); i++)
			{
				if (i == 0)
					unit.weights[i] = 1.0;
				else
					unit.weights[i] = 0.0;
			}
		}
	}

	// make sure there's motion for non-pseudoidle state
	if (unit.data != nullptr)
	{
		if (unit.data->stateName != PseudoIdleState && unit.data->getNumMotions() == 0)
		{
			SmartBody::util::log("MeCtParamAnimation::schedule ERR: state %s has no motions attached.", unit.data->stateName.c_str());
			return;
		}
	}

	waitingList.emplace_back(unit);
#endif
}

void MeCtParamAnimation::unschedule()
{
	reset();
}

void MeCtParamAnimation::updateWeights(std::vector<double>& w)
{
	if (curStateData == nullptr)
		return;
	if (curStateData->state->getNumMotions() != w.size())
		return;

	double wCheck = 0.0;
	for (double i : w)
		wCheck += i;
	if (fabs(wCheck - 1.0) > 0.1)
	{
		for (size_t i = 0; i < w.size(); i++)
			if (i == 0) w[i] = 1.0;
			else		w[i] = 0.0;
	}

	curStateData->weights = w;
	curStateData->timeManager->updateWeights();
	
	if (transitionManager)
		transitionManager->update();
}

void MeCtParamAnimation::updateWeights()
{
	if (curStateData == nullptr)
		return;
	std::vector<double>& w = curStateData->weights;
	updateWeights(w);
}

int MeCtParamAnimation::getNumWeights()
{
	if (curStateData)
		return curStateData->interpolator->getNumMotions();
	else
		return 0;
}

const std::string& MeCtParamAnimation::getNextStateName()
{
	static std::string emptyString;
	if (nextStateData)
		return nextStateData->state->stateName;
	else
		return emptyString;
}

const std::string& MeCtParamAnimation::getCurrentStateName()
{
	static std::string emptyString;
	if (curStateData)
		if (curStateData->state)
			return curStateData->state->stateName;
		else
			return emptyString;
	else
		return emptyString;
}

PABlendData* MeCtParamAnimation::getCurrentPABlendData()
{
	if (curStateData)
		return curStateData.get();
	else
		return nullptr;
}

bool MeCtParamAnimation::hasPABlend(const std::string& name)
{
	if (getCurrentStateName() == name)
		return true;
	if (getNextStateName() == name)
		return true;
	auto iter = waitingList.begin();
	for (; iter!= waitingList.end(); iter++)
	{
		if (iter->data)
		{
			if (iter->data->stateName == name)
				return true;
		}
		else
		{
			if (name == PseudoIdleState)
				return true;
		}

	}
	return false;
}

bool MeCtParamAnimation::isIdle()
{
	if (getCurrentPABlendData() && 
		getCurrentPABlendData()->state &&
		getCurrentPABlendData()->state->stateName != PseudoIdleState)
		return false;
	if (!getNextStateName().empty())
		return false;
	if (!waitingList.empty())
		return false;
	return true;
}

void MeCtParamAnimation::autoScheduling(double time)
{
	if (waitingList.empty())
		return;	

	ScheduleUnit nextUnit = waitingList.front();

	if (time < nextUnit.time)
		return;
	
	if (transitionManager)
		return;

	// if current state is pseudo idle & next state is also pseudo idle, ignore
	if (curStateData != nullptr)
	{
		if (curStateData->getStateName() == PseudoIdleState)
		{
			if (nextUnit.data == nullptr)
			{
				waitingList.pop_front();
				return;
			}
			else if (nextUnit.data->stateName == PseudoIdleState)
			{
				waitingList.pop_front();
				return;
			}
		}
	}

	if (curStateData == nullptr)
	{
		nextStateData.reset();
		curStateData = createStateModule(nextUnit);
		if (!curStateData)
		{
			return;
		}
		curStateData->active = true;
		waitingList.pop_front();
	}
	else
	{

		transitionManager.reset();
		SmartBody::SBAnimationTransition* data = nullptr;
		if (nextUnit.data)
			data = getScene().getBlendManager()->getTransition(curStateData->state->stateName, nextUnit.data->stateName);
		nextStateData = createStateModule(nextUnit);
		nextStateData->active = false;
		if (!data) // no existing transition data
		{
			if (curStateData->state->stateName == PseudoIdleState)
				transitionManager = std::make_unique<PATransitionManager>(nextStateData->transitionLength);
			else
			{
				double transitionLen = nextStateData->transitionLength;				
				if (nextStateData->scheduleMode == PABlendData::Now)
				{
					transitionManager = std::make_unique<PATransitionManager>((float)transitionLen);
				}
				else
				{
					// check to see if the current local time cannot afford the defaultTransition time					
					double actualTransitionTime = transitionLen;

					if (curStateData->timeManager->getNormalizeLocalTime() >= (curStateData->timeManager->getDuration() - transitionLen))
					{
						actualTransitionTime = curStateData->timeManager->getDuration() - curStateData->timeManager->getNormalizeLocalTime();
						//SmartBody::util::log("adjust transition duration for %s and %s to %f", curStateData->getStateName().c_str(), nextStateData->getStateName().c_str(), actualTransitionTime);
					}

					double easeOutStart = curStateData->timeManager->getDuration() - actualTransitionTime;
					double easeOutDur = actualTransitionTime;

					if (actualTransitionTime < 0)
					{
						SmartBody::util::log("transition warning, current state %s is too short(%f), next state is %s", curStateData->getStateName().c_str(), curStateData->timeManager->getDuration(), nextStateData->getStateName().c_str());
					}

					//SmartBody::util::log("easeOutStart = %f, duration = %f",easeOutStart, easeOutDur);
					transitionManager = std::make_unique<PATransitionManager>(easeOutStart, easeOutDur);
					//transitionManager = new PATransitionManager(curStateData->timeManager->getDuration(), 0.0);
				}
			}
		}
		else
		{
			//SmartBody::util::log("state transition from = %s, to = %s, easeOut = %f, easeIn = %f",curStateData->getStateName().c_str(), nextStateData->getStateName().c_str(),data->easeOutStart[0], data->easeInStart);
			transitionManager = std::make_unique<PATransitionManager>(data, curStateData.get(), nextStateData.get());
			nextStateData->timeManager->updateLocalTimes(transitionManager->s2);
		}
		waitingList.pop_front();
	}
}

std::unique_ptr<PABlendData> MeCtParamAnimation::createStateModule(ScheduleUnit su)
{
	std::unique_ptr<PABlendData> module;
	if (su.data)
	{
		module = std::make_unique<PABlendData>(this, su.data, su.weights, su.blend, su.wrap, su.schedule, su.stateTimeOffset, su.stateTimeTrim, su.directPlay);
		auto* animBlend = dynamic_cast<SmartBody::SBAnimationBlend*>(su.data);
		if (animBlend) // set retarget information if it is available
		{
			SmartBody::SBRetargetManager* retargetManager = getScene().getRetargetManager();
			SmartBody::SBRetarget* retarget = retargetManager->getRetarget(animBlend->getBlendSkeleton(),character->getSkeleton()->getName());
			if (retarget && character->getBoolAttribute("onlineRetarget"))
				module->retarget = retarget;
			else
				module->retarget = nullptr;
		}

		module->blendStartOffset = su.stateTimeOffset;
		module->blendEndTrim = su.stateTimeTrim;
		module->transitionLength = su.transitionLength;
		module->playSpeed = su.playSpeed;
		module->baseJointName = baseJointName;
		if (su.duration > 0.f) // the user set a state duration, adjust playSpeed
		{
			module->playSpeed = float(module->timeManager->getDuration()/su.duration);
		}
		if (su.blend == PABlendData::Additive)
		{
			if (su.partialJoint.empty() || su.partialJoint == "null")
				su.partialJoint = "base";
		}
		std::vector<std::string> joints;
		std::vector<std::string> jointNameVec;
		SmartBody::util::tokenize(su.partialJoint, jointNameVec);
		for (auto & jointId : jointNameVec)
		{
			SkJoint* j = character->getSkeleton()->search_joint(jointId.c_str());
			if (j)
			{
				std::vector<SkJoint*> jVec;
				SkJoint::recursive_children(jVec, j);
				for (auto & i : jVec)
				{
					joints.emplace_back(i->getMappedJointName());//jointName());
				}
			}
		}
		module->interpolator->setBlendingJoints(joints);
	}
	else
	{
		module = std::make_unique<PABlendData>(this, PseudoIdleState, su.weights, su.blend, su.wrap, su.schedule, su.stateTimeOffset, su.stateTimeTrim, su.directPlay);
		module->transitionLength = su.transitionLength;
	}
	if (_context)
	{
		module->interpolator->setMotionContextMaps(_context);
		module->interpolator->initChanId(_context, baseJointName);
		SkJoint* baseJoint = character->getSkeleton()->search_joint(baseJointName.c_str());
		if (!baseJoint)
		{
			return nullptr;
		}
		module->interpolator->initPreRotation(baseJoint->quat()->orientation()*baseJoint->quat()->prerot());
		module->woManager->setMotionContextMaps(_context);
		module->woManager->initChanId(_context, baseJointName);		
		//SrQuat preRot = character->getSkeleton()->search_joint(baseJointName.c_str())->quat()->prerot();
		SrQuat preRot = baseJoint->quat()->orientation()*baseJoint->quat()->prerot();
		module->woManager->initPreRotation(preRot);
	}
	else
	{
		return nullptr;
	}

	/*
	for (int i = 0; i < module->state->getNumMotions(); ++i)
	{
		SmartBody::SBMotion* sbMotion = module->state->motions[i];
		if (su.blend == PABlendData::Additive)
		{
			if (!sbMotion->getOffsetParent())
				module->state->motions[i] = sbMotion->getOffset();
		}
		else
		{
			if (sbMotion->getOffsetParent())
				module->state->motions[i] = sbMotion->getOffsetParent();
		}
	}
	*/

	return module;
}

void MeCtParamAnimation::reset()
{
	curStateData.reset();
	nextStateData.reset();
	transitionManager.reset();
	waitingList.clear();
}

void MeCtParamAnimation::updateWo(SrMat& mat, MeCtChannelWriter* woWriter, SrBuffer<float>& buffer)
{
	// get current woMat
	SrBuffer<float>& woValue = woWriter->get_data();
	SrMat currentWoMat;
	SrQuat quat;
	quat.w = woValue[3];
	quat.x = woValue[4];
	quat.y = woValue[5];
	quat.z = woValue[6];
	quat.get_mat(currentWoMat);
	currentWoMat.set(12, woValue[0]);
	currentWoMat.set(13, woValue[1]);
	currentWoMat.set(14, woValue[2]);	

	// separate transform matrix to rotation and transltion matrix
	SrQuat q = SrQuat(mat);
	SrMat rot;
	q.get_mat(rot);
	SrMat translate;
	translate.set(12, mat.get(12));
	translate.set(14, mat.get(14));

	// apply rotation and transition matrix perspectively
	SrQuat woQuat = SrQuat(currentWoMat);
	SrMat currentRot;
	woQuat.get_mat(currentRot);		

	SkJoint* baseJ = character->_skeleton->search_joint(baseJointName.c_str());
	SrVec offset;
	if (baseJ) offset = baseJ->offset(); offset.y = 0.f;
	SrMat negOffset; negOffset.set_translation(-offset);
	SrMat posOffset; posOffset.set_translation(offset);
	//SrMat newWoMat =  rot * currentRot;
	//SrMat newTranslate = translate * currentRot;	
	//SrMat newTranslate = translate * newWoMat;	
	SrMat nextMat = negOffset * mat * posOffset * currentWoMat;
	SrVec newTran = nextMat.get_translation();

	// set new wo mat back to skeleton and woWriter
	SrVec bufferTran = newTran;
	quat = SrQuat(nextMat);
	woValue[3] = quat.w;
	woValue[4] = quat.x;
	woValue[5] = quat.y;
	woValue[6] = quat.z;
	woValue[0] = bufferTran[0];
	woValue[1] = bufferTran[1];
	woValue[2] = bufferTran[2];
	gwiz::quat_t skelQ = gwiz::quat_t(woValue[3], woValue[4], woValue[5], woValue[6]);
	gwiz::euler_t skelE = gwiz::euler_t(skelQ);	
	character->set_world_offset(woValue[0], woValue[1], woValue[2], (float)skelE.h(), (float)skelE.p(), (float)skelE.r());

	JointChannelId baseChanID, baseBuffId;
	baseChanID.x = _context->channels().search(SbmPawn::WORLD_OFFSET_JOINT_NAME, SkChannel::XPos);
	baseChanID.y = _context->channels().search(SbmPawn::WORLD_OFFSET_JOINT_NAME, SkChannel::YPos);
	baseChanID.z = _context->channels().search(SbmPawn::WORLD_OFFSET_JOINT_NAME, SkChannel::ZPos);
	baseChanID.q = _context->channels().search(SbmPawn::WORLD_OFFSET_JOINT_NAME, SkChannel::Quat);

	baseBuffId.x = _context->toBufferIndex(baseChanID.x);
	baseBuffId.y = _context->toBufferIndex(baseChanID.y);
	baseBuffId.z = _context->toBufferIndex(baseChanID.z);	
	baseBuffId.q = _context->toBufferIndex(baseChanID.q);	

	buffer[baseBuffId.x] = bufferTran[0];
	buffer[baseBuffId.y] = bufferTran[1];
	buffer[baseBuffId.z] = bufferTran[2];	
	for (int k = 0; k < 4; k++)
		buffer[baseBuffId.q + k] = quat.getData(k);
}

void MeCtParamAnimation::updateIK( PABlendData* curBlendData, SrMat& woMat, SrMat& woDeltaMat, float dt, SrBuffer<float>& buff )
{
	auto* blend = dynamic_cast<SmartBody::SBAnimationBlend*>(curBlendData->state);
	if (!blend) return;
	MotionAnalysis* moAnalysis = blend->getMotionAnalysis();
	if (!moAnalysis) return;
	
	if (ikScenario.ikTreeNodes.empty()) // initialize ik nodes
	{
		std::vector<std::string> stopJoint;
		stopJoint.emplace_back("spine1");
		ikScenario.buildIKTreeFromJointRoot(character->getSkeleton()->getJointByMappedName("base"),stopJoint);
	}	
	
	float rotSpeed = SrQuat(woDeltaMat.get_rotation()).axisAngle().y*180.f/(dt*(float)M_PI);
	SrVec deltaTrans = woDeltaMat.get_translation()*woMat.get_rotation(); // rotate translation to global frame
	deltaTrans.y = 0.f; 
	SrVec velocity = deltaTrans/dt;		
	float smoothWeight = dt*10.f;
	smoothAngVel = smoothAngVel*(1.f-smoothWeight) + rotSpeed*smoothWeight;
	smoothVel = smoothVel*(1.f-smoothWeight) + velocity*smoothWeight;

	updateMotionFrame(inputFrame, ikScenario, buff, true); // read data from frame

	//moAnalysis->applyIKFix(ikScenario, character, curBlendData->weights, curBlendData->timeManager, woMat, velocity, rotSpeed, inputFrame, outputFrame);
	moAnalysis->applyIKFix(ikScenario, character, curBlendData->weights, curBlendData->timeManager, woMat, smoothVel, smoothAngVel, inputFrame, outputFrame);

	updateMotionFrame(outputFrame, ikScenario, buff, false);	
}

void MeCtParamAnimation::updateJointTrajectory( PABlendData* blendData )
{
  if (!character) return;
	std::vector<std::string> jointConsNames = character->getJointConstraintNames();
	SmartBody::SBJoint* baseJoint = character->getSkeleton()->getJointByMappedName("base");
	SrMat baseGmat;
	character->getSkeleton()->update_global_matrices();
	if (baseJoint && baseJoint->getParent())
	{		
		baseGmat = baseJoint->gmat();//baseJoint->getParent()->gmat();		
	}
//	auto& scene = getScene();

	SmartBody::SBAnimationBlend* animBlend = dynamic_cast<SmartBody::SBAnimationBlend*>(blendData->state);

	SmartBody::SBRetarget* retarget = nullptr;
	if (animBlend)
	{
		SmartBody::SBRetargetManager* retargetManager = getScene().getRetargetManager();
		retarget = retargetManager->getRetarget(animBlend->getBlendSkeleton(),character->getSkeleton()->getName());
	}
	
	if (!retarget)
		return;

	for (unsigned int i=0;i<jointConsNames.size();i++)
	{
		SmartBody::TrajectoryRecord* trajRecord = character->getJointTrajectoryConstraint(jointConsNames[i]);
		if (!trajRecord)
			continue;

		SmartBody::SBJoint* refJoint = character->getSkeleton()->getJointByName(trajRecord->refJointName);
		if (!refJoint)
			continue;			
		SrVec trajOffset;
		bool hasTraj = blendData->getTrajPosition(jointConsNames[i],0.f,trajOffset);
		if (!hasTraj)
		{	
			trajRecord->isEnable = false;
			continue;
		}
		trajRecord->isEnable = true;
		trajRecord->jointTrajLocalOffset = trajOffset;
		trajRecord->refJointGlobalPos = baseGmat.get_translation();//refJoint->gmat().get_translation(); 
		if (retarget)
			retarget->applyRetargetJointTrajectory(*trajRecord,baseGmat);			
	}
}

void MeCtParamAnimation::updateMotionFrame( BodyMotionFrame& motionFrame, MeCtIKTreeScenario& ikScenario, SrBuffer<float>& buff, bool readData /*= true*/ )
{
	IKTreeNodeList& nodeList = ikScenario.ikTreeNodes;
	if (motionFrame.jointQuat.size() != nodeList.size())
		motionFrame.jointQuat.resize(nodeList.size());

	MeCtIKTreeNode* rootNode = ikScenario.ikTreeRoot;
	if (rootNode)
	{
		int chanId[3];
		int buffId[3];
		chanId[0] = _context->channels().search(rootNode->getNodeName().c_str(), SkChannel::XPos);
		chanId[1] = _context->channels().search(rootNode->getNodeName().c_str(), SkChannel::YPos);
		chanId[2] = _context->channels().search(rootNode->getNodeName().c_str(), SkChannel::ZPos);
		for (int k=0;k<3;k++)
		{
			if (chanId[k] < 0)
				continue;
			buffId[k] = _context->toBufferIndex(chanId[k]);
			if (buffId[k] < 0)
				continue;
			if (readData)
				motionFrame.rootPos[k] = buff[buffId[k]];
			else
				buff[buffId[k]] = motionFrame.rootPos[k];
		}		
	}

	for (unsigned int i=0;i<nodeList.size();i++)
	{
		MeCtIKTreeNode* node = nodeList[i];
		JointChannelId chanId;
		JointChannelId buffId;
		chanId.q = _context->channels().search(node->getNodeName().c_str(), SkChannel::Quat);
		if (chanId.q < 0)
			continue;
		buffId.q = _context->toBufferIndex(chanId.q);
		if (buffId.q < 0)
			continue;
		SrQuat& origQ = motionFrame.jointQuat[i];
		if (readData)
		{
			origQ.w = buff[buffId.q + 0];
			origQ.x = buff[buffId.q + 1];
			origQ.y = buff[buffId.q + 2];
			origQ.z = buff[buffId.q + 3];
		}
		else
		{
			buff[buffId.q + 0] = origQ.w ;
			buff[buffId.q + 1] = origQ.x;
			buff[buffId.q + 2] = origQ.y;
			buff[buffId.q + 3] = origQ.z;
		}
	}	
}


SrMat MeCtParamAnimation::combineMat( SrMat& mat1, SrMat& mat2 )
{
	SrMat newMat = mat1*mat2;
	SrVec newTranslation = mat1.get_translation() + mat2.get_translation();
	newMat.set_translation(newTranslation);
	return newMat;	
}

void MeCtParamAnimation::getCharacterJointTrajectory( std::vector<SrVec> &curStatePosList )
{
	std::vector<std::string> jointConsNames = character->getJointConstraintNames();					
	for (unsigned int i=0;i<jointConsNames.size();i++)
	{
		SmartBody::TrajectoryRecord* traj = character->getJointTrajectoryConstraint(jointConsNames[i]);
		curStatePosList.emplace_back(traj->jointTrajGlobalPos);
	}
}
