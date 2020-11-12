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

#include "SBReach.h"
#include <sb/SBCharacter.h>
#include <sb/SBMotion.h>
#include <sb/SBUtilities.h>
#include <controllers/MeCtBodyReachState.h>
#include <controllers/MeCtReachEngine.h>

namespace SmartBody {

SBReach::SBReach()
{
	_character = NULL;
	interpolatorType = "KNN";	
	currentReachType = -1;
	reachTag = "default";
}

SBReach::SBReach(SBCharacter* character, std::string reTag)
{
	_character = character;
	interpolatorType = "KNN";	
	currentReachType = -1;
	reachTag = reTag;
}

SBReach::~SBReach()
{
  std::map<int,MeCtReachEngine*>::iterator it;
  for (it  = reachEngineMap.begin();
       it != reachEngineMap.end();
       it++)
  {
    delete it->second;
  }
  reachEngineMap.clear();
}

SBCharacter* SBReach::getCharacter()
{
	return _character;
}

SBReach* SBReach::copy()
{
	// ?????
	// ...
	// ...
	return NULL;
}

void SBReach::addMotion(std::string type, SBMotion* motion)
{
	if (!_character)
	{
		SmartBody::util::log("No character present, motion %s was not added to reach.", motion->getName().c_str());
		return;
	}

	int reachType = MeCtReachEngine::getReachType(type);
	if (reachType != -1)
	{
		//_character->addReachMotion(reachType,motion);
		TagMotion tagMotion = TagMotion(reachType, motion);
		if (reachMotionData.find(tagMotion) == reachMotionData.end()) 
		{
			reachMotionData.insert(tagMotion);			
		}		
	}
	else
	{
		SmartBody::util::log("Please use 'LEFT' or 'RIGHT'");
		return;
	}
}

void SBReach::removeMotion(std::string type, SBMotion* motion)
{
	if (!_character)
	{
		SmartBody::util::log("No character present, motion %s was not removed from reach.", motion->getName().c_str());
		return;
	}

	int reachType = MeCtReachEngine::getReachType(type);
	if (reachType != -1)
	{
		TagMotion tagMotion = TagMotion(reachType, motion);
		if (reachMotionData.find(tagMotion) != reachMotionData.end()) 
		{
			reachMotionData.erase(tagMotion);			
		}		
	}	
	else
	{
		SmartBody::util::log("Please use 'LEFT' or 'RIGHT'");
		return;
	}
}

int SBReach::getNumMotions()
{
	if (!_character)
		return 0;

	return reachMotionData.size();
}

std::vector<std::string> SBReach::getMotionNames(std::string type)
{
	std::vector<std::string> motionNames;
	//MotionDataSet& motionSet = const_cast<MotionDataSet&>(_character->getReachMotionDataSet());
	MotionDataSet& motionSet = reachMotionData;
	MotionDataSet::iterator vi;
	for ( vi  = motionSet.begin();
		  vi != motionSet.end();
		  vi++)
	{
		SkMotion* motion = (*vi).second;
		motionNames.push_back(motion->getName());
	}	
	return motionNames;

}
void SBReach::build(SBCharacter* character)
{
	if (!character)
		return;

	// create the reach engine if the character does not have one.
	//character->createReachEngine(this);
	createReachEngineMap();
	//ReachEngineMap* reMapPtr = &reachEngineMap;//character->getReachEngineMap(this->getReachTag());
	for (ReachEngineMap::iterator mi = reachEngineMap.begin();
		mi != reachEngineMap.end();
		mi++)
	{
		MeCtReachEngine* re = mi->second;
		if (re)
		{
			re->updateMotionExamples(reachMotionData, interpolatorType);
		}
	}
	character->setReach(this);
}


bool SBReach::isPawnAttached( std::string pawnName )
{	
	if (!_character)
		return false;

	for (ReachEngineMap::iterator mi = reachEngineMap.begin();
		mi != reachEngineMap.end();
		mi++)
	{
		MeCtReachEngine* re = mi->second;
		if (re)
		{
			if (re->getReachData()->effectorState.attachedPawnName == pawnName)
			{
				return true;
			}
		}
	}
	return false;
}

void SBReach::setPointHandMotion( std::string type, SBMotion* pointMotion )
{
	if (!_character)
		return;
	MotionDataSet& pointMotionSet = pointHandData;
	int reachType = MeCtReachEngine::getReachType(type);
	if (reachType != -1)
	{
		TagMotion motion = TagMotion(reachType,pointMotion);
		pointMotionSet.insert(motion);
	}
	else
	{
		SmartBody::util::log("Please use 'LEFT' or 'RIGHT'");
		return;
	}	

}

SBMotion* SBReach::getPointHandMotion( std::string type )
{
	MotionDataSet& pointMotionSet = pointHandData;
	int reachType = MeCtReachEngine::getReachType(type);
	SBMotion* sbMotion = SBReach::findTagMotion(reachType,pointMotionSet);	
	return sbMotion;
}

void SBReach::setGrabHandMotion(std::string type, SBMotion* grabMotion)
{
	if (!_character)
		return;
	MotionDataSet& grabMotionSet = grabHandData;
	int reachType = MeCtReachEngine::getReachType(type);
	if (reachType != -1)
	{
		TagMotion motion = TagMotion(reachType,grabMotion);
		grabMotionSet.insert(motion);
	}
	else
	{
		SmartBody::util::log("Please use 'LEFT' or 'RIGHT'");
		return;
	}	
}

SBMotion* SBReach::getGrabHandMotion(std::string type)
{
	MotionDataSet& grabMotionSet = grabHandData;
	int reachType = MeCtReachEngine::getReachType(type);
	SBMotion* sbMotion = SBReach::findTagMotion(reachType,grabMotionSet);	
	return sbMotion;
}

void SBReach::setReleaseHandMotion(std::string type, SBMotion* releasebMotion)
{
	if (!_character)
		return;
	MotionDataSet& releaseMotionSet = releaseHandData;
	int reachType = MeCtReachEngine::getReachType(type);
	if (reachType != -1)
	{
		TagMotion motion = TagMotion(reachType,releasebMotion);
		releaseMotionSet.insert(motion);
	}
	else
	{
		SmartBody::util::log("Please use 'LEFT' or 'RIGHT'");
		return;
	}	
}

SBMotion* SBReach::getReleaseHandMotion(std::string type)
{
	MotionDataSet& releaseMotionSet = releaseHandData;
	int reachType = MeCtReachEngine::getReachType(type);
	SBMotion* sbMotion = SBReach::findTagMotion(reachType,releaseMotionSet);	
	return sbMotion;
}

void SBReach::setReachHandMotion(std::string type, SBMotion* reachMotion)
{
	if (!_character)
		return;
	MotionDataSet& reachMotionSet = reachHandData;
	int reachType = MeCtReachEngine::getReachType(type);
	if (reachType != -1)
	{
		TagMotion motion = TagMotion(reachType,reachMotion);
		reachMotionSet.insert(motion);
	}
	else
	{
		SmartBody::util::log("Please use 'LEFT' or 'RIGHT'");
		return;
	}	
}

SBMotion* SBReach::getReachHandMotion(std::string type)
{
	MotionDataSet& reachMotionSet = reachHandData;
	int reachType = MeCtReachEngine::getReachType(type);
	SBMotion* sbMotion  = SBReach::findTagMotion(reachType,reachMotionSet);	
	return sbMotion;
}

void SBReach::setInterpolatorType( std::string type )
{
	interpolatorType = type;
}

std::string& SBReach::getInterpolatorType()
{
	return interpolatorType;
}

SBMotion* SBReach::findTagMotion( int tag, const MotionDataSet& motionSet )
{
	MotionDataSet::const_iterator vi;
	for ( vi  = motionSet.begin();
		vi != motionSet.end();
		vi++)
	{
		TagMotion tagMotion = *vi;
		if (tagMotion.first == tag)
			return tagMotion.second;
	}
	return NULL;
}

void SBReach::createReachEngineMap()
{
	if (!_character)
		return;

	SmartBody::SBSkeleton* sbSkel = dynamic_cast<SmartBody::SBSkeleton*>(_character->getSkeleton());
	SmartBody::SBJoint* effector = sbSkel->getJointByMappedName("r_middle1");
	if (!effector) 
		effector = sbSkel->getJointByMappedName("r_index1");

	if (!effector)
		effector = sbSkel->getJointByMappedName("r_wrist");


	if (effector && reachEngineMap.find(MeCtReachEngine::RIGHT_ARM) == reachEngineMap.end())
	{
		MeCtReachEngine* rengine = new MeCtReachEngine(_character,sbSkel);
		rengine->init(MeCtReachEngine::RIGHT_ARM,effector);
		reachEngineMap[MeCtReachEngine::RIGHT_ARM] = rengine;	

		// 		MeCtReachEngine* rengineJump = new MeCtReachEngine(this,sbSkel);
		// 		rengineJump->init(MeCtReachEngine::RIGHT_JUMP,effector);
		// 		(*this->reachEngineMap)[MeCtReachEngine::RIGHT_JUMP] = rengineJump;	
	}	

	SmartBody::SBJoint* leftEffector = sbSkel->getJointByMappedName("l_middle1");

	if (!leftEffector) 
		leftEffector = sbSkel->getJointByMappedName("l_index1");

	if (!leftEffector)
		leftEffector = sbSkel->getJointByMappedName("l_wrist");
	if (leftEffector && reachEngineMap.find(MeCtReachEngine::LEFT_ARM) == reachEngineMap.end())
	{
		MeCtReachEngine* rengine = new MeCtReachEngine(_character,sbSkel);
		rengine->init(MeCtReachEngine::LEFT_ARM,leftEffector);
		reachEngineMap[MeCtReachEngine::LEFT_ARM] = rengine;	
		//MeCtReachEngine* rengineJump = new MeCtReachEngine(this,sbSkel);
		//rengineJump->init(MeCtReachEngine::LEFT_JUMP,effector);
		//(*this->reachEngineMap)[MeCtReachEngine::LEFT_JUMP] = rengineJump;
	}

}

MeCtReachEngine* SBReach::getReachEngine( const std::string& reachType )
{
	int reachID = MeCtReachEngine::getReachType(reachType);
	MeCtReachEngine* reachEngine = NULL;
	if (reachEngineMap.find(reachID) != reachEngineMap.end())
	{
		reachEngine = reachEngineMap[reachID];
	}
	return reachEngine;
}

}
