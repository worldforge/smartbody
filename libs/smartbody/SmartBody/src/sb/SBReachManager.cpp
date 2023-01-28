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

#include "SBReachManager.h"

#include <sb/SBScene.h>
#include <sb/SBReach.h>
#include "SBUtilities.h"

namespace SmartBody {

SBReachManager::SBReachManager(SBScene& scene) : SBSceneOwned(scene)
{
}

SBReachManager::~SBReachManager() = default;


SBAPI SBReach* SBReachManager::createReachWithTag( const std::string& characterName, const std::string& reachTag )
{
	// get the character
	SBCharacter* character = _scene.getCharacter(characterName);
	if (!character)
	{
		SmartBody::util::log("Character named %s does not exist.", characterName.c_str());
		return nullptr;
	}
	std::string charReachTag = characterName + "_" + reachTag;
	auto iter = _reaches.find(charReachTag);
	if (iter != _reaches.end())
	{
		// remove the old reach data
		SBReach& reach = (*iter).second;
		_reaches.erase(iter);
	}

  
	//_reaches.insert(pair<std::string, SBReach*>(characterName, reach));
	_reaches[charReachTag] = SBReach(character, reachTag);
	return &_reaches[charReachTag];
}


SBReach* SBReachManager::createReach(const std::string& characterName)
{
	return createReachWithTag(characterName,"default");
}

//void SBReachManager::removeReach(SBReach* reach)
//{
//	SBCharacter* character = reach->getCharacter();
//
//	// clean up all the reach structures...
//	// ...
//	// ...
//  delete reach;
//}

int SBReachManager::getNumReaches()
{
	return _reaches.size();
}

SBReach* SBReachManager::getReach(const std::string& characterName, const std::string& reachTag)
{
	std::string charReachTag = characterName + "_" + reachTag;
	auto iter = _reaches.find(charReachTag);
	if (iter != _reaches.end())
	{
		return &iter->second;
	}
	else
	{
		return nullptr;
	}
}



}
