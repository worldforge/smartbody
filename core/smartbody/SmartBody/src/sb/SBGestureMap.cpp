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

#include "SBGestureMap.h"

#include <sb/SBCharacter.h>
#include <sb/SBScene.h>
#include <sb/SBMotion.h>
#include "SBUtilities.h"
#include <cstdlib>
#include <ctime> 
#include <cstring>

#ifndef WIN32
#define _stricmp strcasecmp
#endif

namespace SmartBody {

SBGestureMap::SBGestureMap(SBScene& scene) : SBObject(), SBSceneOwned(scene)
{
	defaultGestureInfo._animation = "";
	defaultGestureInfo._lexeme = "";
	defaultGestureInfo._type = "";
	defaultGestureInfo._hand = "";
	defaultGestureInfo._style = "";
	defaultGestureInfo._posture = "";
}

SBGestureMap::SBGestureMap(SBScene& scene, const std::string& name) : SBSceneOwned(scene)
{
	setName(name);

	defaultGestureInfo._animation = "";
	defaultGestureInfo._lexeme = "";
	defaultGestureInfo._type = "";
	defaultGestureInfo._hand = "";
	defaultGestureInfo._style = "";
	defaultGestureInfo._posture = "";

	srand((unsigned int)time(0));
	//srand(clock());
}

SBGestureMap::~SBGestureMap()
{
	_gestureMaps.clear();
}

SBGestureMap* SBGestureMap::copy()
{
	// TODO:
	return nullptr;
}

void SBGestureMap::addGestureMapping(const std::string& name, const std::string& lexeme, const std::string& type, const std::string& hand, const std::string& style, const std::string& posture)
{
	GestureInfo gInfo;
	gInfo._animation = name;
	gInfo._lexeme = lexeme;
	gInfo._type = type;
	gInfo._hand = hand;
	gInfo._style = style;
	gInfo._posture = posture;

	_gestureMaps.emplace_back(gInfo);

	SBMotion* motion = _scene.getMotion(name);
	if (!motion)
		return;
	motion->setMotionType(SBMotion::Gesture);
}

bool SBGestureMap::gestureInfoCompare(	const std::string& glexeme, const std::string& gtype, const std::string& ghand, const std::string& gstyle, const std::string& gposture,
										const std::string& lexeme, const std::string& type, const std::string& hand, const std::string& style, const std::string& posture, 
										std::string compType)
{
	if (compType == "lexeme")
		return (_stricmp(glexeme.c_str(), lexeme.c_str()) == 0);
	if (compType == "type")
		return (_stricmp(gtype.c_str(), type.c_str()) == 0);
	if (compType == "hand")
		return (_stricmp(ghand.c_str(), hand.c_str()) == 0);
	if (compType == "style")
		return (_stricmp(gstyle.c_str(), style.c_str()) == 0);
	if (compType == "posture")
		return (_stricmp(gposture.c_str(), posture.c_str()) == 0);
	return true;
}


std::vector<std::string> SBGestureMap::getGestureListByInfo(const std::string& lexeme, const std::string& type, const std::string& hand, const std::string& style, const std::string& posture)
{
	std::vector<std::string> compTypes;
	if (lexeme != "")
		compTypes.emplace_back("lexeme");
	if (type != "")
		compTypes.emplace_back("type");
	if (hand != "")
		compTypes.emplace_back("hand");
	if (style != "")
		compTypes.emplace_back("style");
	if (posture != "")
		compTypes.emplace_back("posture");


	std::vector<std::string> retAnimations;
	for (auto & _gestureMap : _gestureMaps)
	{
		bool hasMatching = true;
		for (auto & compType : compTypes)
		{
			if (!gestureInfoCompare(_gestureMap._lexeme, _gestureMap._type, _gestureMap._hand, _gestureMap._style, _gestureMap._posture, lexeme, type, hand, style, posture, compType))
			{
				hasMatching = false;
				break;
			}
		}

		if (hasMatching)
			retAnimations.emplace_back(_gestureMap._animation);
	}

	return retAnimations;
}

/* 
 *	policy can be "random" | "first"
 *  user can only input type and get the result
 *	posture always has to match
 */
std::string SBGestureMap::getGestureByInfo(const std::string& lexeme, const std::string& type, const std::string& hand, const std::string& style, const std::string& posture, const std::string& policy, int useIndex, int& appliedIndex)
{
	const std::vector<std::string>& retAnimations = getGestureListByInfo(lexeme, type, hand, style, posture);
	if (retAnimations.empty())
	{
		SmartBody::util::log("Gesture %s cannot find gesture with type %s, posture %s, hand %s.", getName().c_str(), type.c_str(), posture.c_str(), hand.c_str());
		return "";
	}
	else
	{
		if (useIndex > -1)
		{
			appliedIndex = useIndex;
			if (retAnimations.size() > useIndex)
				return retAnimations[useIndex];
			SmartBody::util::log("Index %d is out of range of animation list.", useIndex);
		}
		if (policy == "random")
		{
			int id = rand() % retAnimations.size();
			if (id == retAnimations.size())
				id = id - 1;
			appliedIndex = id;
			return retAnimations[id];
		}
		else if (policy == "first")
		{
			appliedIndex = 0;
			return retAnimations[0];
		}
		else
		{
			appliedIndex = 0;
			return retAnimations[0];
		}
	}
}

SBGestureMap::GestureInfo& SBGestureMap::getGestureByIndex(int i)
{
	if (i < 0 || i >= int(_gestureMaps.size()))
	{
		SmartBody::util::log("Index %d out of range of gesture map.", i);
		return defaultGestureInfo;
	}

	return _gestureMaps[i];
}


SBAPI std::string SBGestureMap::getGestureAnimation( int i )
{
	GestureInfo& gesture = getGestureByIndex(i);
	return gesture._animation;
}

int SBGestureMap::getNumMappings()
{
	return _gestureMaps.size();
}

void SBGestureMap::validate()
{
	auto iter = _gestureMaps.begin();
	for (; iter != _gestureMaps.end(); iter++)
	{
		SmartBody::util::log("Gesture: motion='%s' idle='%s' lexeme='%s', type='%s', hand='%s', style='%s'", 
			 iter->_animation.c_str(), iter->_posture.c_str(), iter->_lexeme.c_str(), iter->_type.c_str(), iter->_hand.c_str(), iter->_style.c_str());
		SBMotion* animation = _scene.getMotion(iter->_animation);
		if (!animation)
		{
			SmartBody::util::log("WARNING: Animation '%s' is not loaded.", iter->_animation.c_str());
		}
		SBMotion* idle = _scene.getMotion(iter->_posture);
		if (!idle)
		{
			SmartBody::util::log("WARNING: Idle '%s' is not loaded.", iter->_posture.c_str());
		}
	}

}


}
