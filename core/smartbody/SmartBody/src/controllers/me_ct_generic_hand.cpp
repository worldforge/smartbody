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

#include "controllers/me_ct_generic_hand.h"
#include <sb/SBScene.h>
#include <sb/sbm_character.hpp>
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include <sb/SBJoint.h>
#include <sb/SBRetargetManager.h>
#include <sb/SBRetarget.h>
#include <sb/SBAssetManager.h>
#include "SBUtilities.h"
#include <algorithm>

#include <boost/config.hpp>
#include <iostream>
#include <fstream>

#include <boost/config.hpp>
 
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/properties.hpp>
 
#include <boost/property_map/property_map.hpp>

using namespace std;
using namespace boost;

/*
   These inital type defs are used to define the lists , graphs , vertices, edges etc
*/
typedef adjacency_list < listS, vecS, directedS,
no_property, property < edge_weight_t, int > > graph_t;
typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
typedef graph_traits < graph_t >::edge_descriptor edge_descriptor;
typedef std::pair<int, int> Edge;

typedef std::vector<std::pair<int,float> > CostList;

// TODO: Add to character and run data on hand skeleton (get skeleton too)


std::string MeCtGenericHand::_controller_type = "GenericHand";

MeCtGenericHand::MeCtGenericHand() : SmartBody::SBController()
{
	_valid = false;
	_lastTime = 0.0;
	_motionTime = 0.0;
	_handMotion = nullptr;
	_bodyMotion = nullptr;
	_handDatabase = nullptr;
	_bodyDatabase = nullptr;
	_finalMotion = nullptr;
	currentSegment = 0;
	_maxLevels = 100;
	_k = 5;
	_sk = nullptr;
	
	_enableHand = true;
	_enableBody = true;

	// hand attributes
	setDefaultAttributeGroupPriority("Hand", 800);
	addDefaultAttributeBool("hand.enableHand", true, "Hand");
	addDefaultAttributeBool("hand.enableBody", true, "Hand");
	addDefaultAttributeBool("hand.debug", false, "Hand");
	addDefaultAttributeString("hand.motionName", "", "Hand");
	addDefaultAttributeString("hand.config", "", "Hand");
	addDefaultAttributeInt("hand.levels", -1, "Hand");
	addDefaultAttributeBool("hand.useRandom", false, "Hand");
}

MeCtGenericHand::MeCtGenericHand( boost::intrusive_ptr<SmartBody::SBSkeleton> skeleton, SbmCharacter* c) : SmartBody::SBController()
{	
	_valid = false;
	_lastTime = 0.0;
	_motionTime = 0.0;
	_handMotion = nullptr;
	_bodyMotion = nullptr;
	_handDatabase = nullptr;
	_bodyDatabase = nullptr;
	_finalMotion = nullptr;
	currentSegment = 0;
	_maxLevels = 100;
	_k = 5;
	_sk = nullptr;

	_enableHand = true;
	_enableBody = true;

//	SmartBody::util::log("Trying to initalize hand controller here");

	// wrist joint names
	_wristJtRtName = "r_wrist";
	_wristJtLtName = "l_wrist";

	_skeletonRef = skeleton;
	// work on the copy of skeleton to avoid problems
	_sk = new SmartBody::SBSkeleton(*skeleton);
	
	// set the character
	_character = c;

	// initialize the final motion
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	//_finalMotion = assetManager->createMotion("finalMotion");
	
	// load the databsae motions for body and hand
	_bodyDatabase = SmartBody::SBScene::getScene()->getMotion("brad_body_motion.skm");
	//_handDatabase = SmartBody::SBScene::getScene()->getMotion("brad_hand_motion.skm");

	// try sbhandsynthesis here
	_handSynthesis = new SmartBody::SBHandSynthesis(_sk, _channels);

	// fill the end joints vector 
	_endJointsRt.emplace_back(_handSynthesis->getRightDb()->getJointName());
	_endJointsLt.emplace_back(_handSynthesis->getLeftDb()->getJointName());
	_endJoints.emplace_back(_handSynthesis->getRightDb()->getJointName());
	_endJoints.emplace_back(_handSynthesis->getLeftDb()->getJointName());
}

MeCtGenericHand::~MeCtGenericHand() = default;

// probably should change initialization to something more relevant
void MeCtGenericHand::init(SmartBody::SBMotion* m , int num_levels = -1)
{
	
	// initailzing controller
	SmartBody::util::log("initializing genric hand controller ");

	// initialize number of levels 
	_maxLevels = num_levels;

	// clear all the data
	
	// set valid to true
	_valid = true;
	
	// set body motion to this motion
	_bodyMotion = m;

	// run the synthesis
	bool useRandom = this->_character->getBoolAttribute("hand.useRandom");
	_handSynthesis->synthesizeHands(_bodyMotion, num_levels, useRandom);

	SmartBody::SBMotion* _leftMotion = _handSynthesis->getLeftDb()->getFinalMotion();
	SmartBody::SBMotion* _rightMotion = _handSynthesis->getRightDb()->getFinalMotion();

	//// print the duration of the hand motion
	SmartBody::util::log("The duration of left loaded motion is %f",_leftMotion->duration());
	SmartBody::util::log("The duration of right loaded motion is %f",_rightMotion->duration());
}


// this function is run per frame to update the data
bool MeCtGenericHand::controller_evaluate( double t, MeFrameData& frame )
{

	float dt = 1.0f / 60.0f;
	if (_lastTime > 0.0)
	{
		dt = float(t - _lastTime);
	}
	// if (dt < .016f) // update no faster than 60 fps
	// 	return true;
	_lastTime = t;

	// get the hand motion
	SmartBody::SBMotion* _leftMotion = _handSynthesis->getLeftDb()->getFinalMotion();
	SmartBody::SBMotion* _rightMotion = _handSynthesis->getRightDb()->getFinalMotion();
	
	// only if motion exists
	if (!_leftMotion || !_rightMotion)
		return true;

	// make sure they have the same duration
	float duration = std::min(_leftMotion->duration(),_rightMotion->duration());

	// get the motion time (always loop this motion)
	_motionTime = fmod((float)t, duration);
	
	// if this needs to be stopped
	if (_motionTime > duration)
	{
		_valid = false;
		return true;
	}

	if (_character && _valid){
		

		// this adds the body motion
		if (_enableBody)
			updateChannelBuffer(frame);

		// this adds the hand motion
		if (_enableHand)
		{
			updateChannelBuffer(frame, _endJointsRt, _rightMotion);
			updateChannelBuffer(frame, _endJointsLt, _leftMotion);
		}
	}

	return true;
}

// update the channel buffers
void MeCtGenericHand::updateChannelBuffer(MeFrameData& frame, std::vector<std::string>& endJoints, SmartBody::SBMotion* motion)
{
	SrBuffer<float>& buffer=frame.buffer();
	motion->connect(_sk.get());
	motion->apply(_motionTime);

	for (auto & endJoint : endJoints)
	{
		SmartBody::SBJoint* eJoint = _sk->getJointByName(endJoint);
		if (eJoint)
		{
			std::vector<SmartBody::SBJoint*> descendents = eJoint->getDescendants();
			//descendents.emplace_back(eJoint);

			for (auto joint : descendents)
			{
					int chanId = _context->channels().search(joint->getMappedJointName(), SkChannel::Quat);
				if (chanId < 0)
					continue;
				int index = _context->toBufferIndex(chanId);
				SrQuat quat= joint->quat()->rawValue();
				if(index<0)
					continue;

				buffer[index + 0] = quat.w;
				buffer[index + 1] = quat.x;
				buffer[index + 2] = quat.y;
				buffer[index + 3] = quat.z;
			}
		}
	}

	motion->disconnect();
}




// update the channel buffers
void MeCtGenericHand::updateChannelBuffer(MeFrameData& frame)
{
	SrBuffer<float>& buffer=frame.buffer();

	SmartBody::SBRetarget* retarget = nullptr;
	{
		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();		
		retarget = scene->getRetargetManager()->getRetarget(_bodyMotion->getMotionSkeletonName(),_character->getSkeleton()->getName());	
		
	}
	_bodyMotion->connect(_sk.get());
	_bodyMotion->apply(_motionTime,SkMotion::Linear,0,retarget);

	// get all the descendants
	std::vector<SmartBody::SBJoint*> removeJoints;
	for (auto & _endJoint : _endJoints)
	{
		SmartBody::SBJoint* joint = _sk->getJointByName(_endJoint);
		std::vector<SmartBody::SBJoint*> descendants = joint->getDescendants();
		removeJoints.insert(removeJoints.end(), descendants.begin(), descendants.end());
	}
		
	// find the joints to be keep
	vector<SmartBody::SBJoint*> keepJoints;
	for ( int i = 0 ; i < _sk->getJointNames().size() ; i++ )
	{
		SmartBody::SBJoint* joint = _sk->getJointByName(_sk->getJointNames()[i]);
		
		// check if joint is in the ones to be removed
		bool shouldRemove = false;
		for (auto & removeJoint : removeJoints)
		{
			if ( joint == removeJoint)
			{
				shouldRemove = true;
				break;
			}
		}

		// if remove , continue
		if (shouldRemove)
			continue;
		else
		{
			keepJoints.emplace_back(joint);
		}
	}


	int numJoints = _sk->getJointNames().size();

	// placing in those descendants
	for(auto joint : keepJoints)
	{
				int chanId = _context->channels().search(joint->getMappedJointName(), SkChannel::Quat);
			if (chanId < 0)
				continue;
			int index = _context->toBufferIndex(chanId);
			SrQuat quat= joint->quat()->rawValue();
			if(index<0)
				continue;
			buffer[index + 0] = quat.w;
			buffer[index + 1] = quat.x;
			buffer[index + 2] = quat.y;
			buffer[index + 3] = quat.z;
	}

	_bodyMotion->disconnect();
}

// might wanna do something when the controller starts
void MeCtGenericHand::controller_start()
{

}

void MeCtGenericHand::notify(SBSubject* subject)
{
	SmartBody::SBController::notify(subject);

	SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute)
	{
		const std::string& name = attribute->getName();
		if (name == "hand.enableHand")
		{
			SmartBody::BoolAttribute* boolAttr = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			if (boolAttr)
			{
				_enableHand = boolAttr->getValue();
			}
		}
		else if (name == "hand.enableBody")
		{
			SmartBody::BoolAttribute* boolAttr = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			if (boolAttr)
			{
				_enableBody = boolAttr->getValue();
			}
		}
		else if (name == "hand.debug")
		{
			SmartBody::BoolAttribute* boolAttr = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			if (boolAttr)
			{
				_handSynthesis->setDebug(boolAttr->getValue());
			}
		}
		else if (name == "hand.motionName")
		{
			if (!_character)
				return;
			std::string motionName = _character->getStringAttribute("hand.motionName");
			if (motionName == "")
			{
				SmartBody::util::log("No motion specified. Cannot initialize hand controller.");
				return;
			}
			int numLevels = _character->getIntAttribute("hand.levels");
			SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(motionName);
			if (!motion)
			{
				SmartBody::util::log("No motion named %s available. Cannot initialize hand controller.", motionName.c_str());
				return;
			}
			this->init(motion, numLevels);
		}
		else if (name == "hand.config")
		{
			std::string configName = _character->getStringAttribute("hand.config");
			_handSynthesis->setConfigurationName(configName);
			_handSynthesis->clearData();
			_handSynthesis->segmentDatabase();
		}

	}

}

