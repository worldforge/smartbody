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

SBReachManager::SBReachManager()
{
}

SBReachManager::~SBReachManager()
{
}


SBAPI SBReach* SBReachManager::createReachWithTag( std::string characterName, std::string reachTag )
{
	SBScene* scene = SmartBody::SBScene::getScene();
	// get the character
	SBCharacter* character = scene->getCharacter(characterName);
	if (!character)
	{
		SmartBody::util::log("Character named %s does not exist.", characterName.c_str());
		return nullptr;
	}
	std::string charReachTag = characterName + "_" + reachTag;
	std::map<std::string, SBReach*>::iterator iter = _reaches.find(charReachTag);
	if (iter != _reaches.end())
	{
		// remove the old reach data
		SBReach* reach = (*iter).second;
    //return reach;
		removeReach(reach);
		_reaches.erase(iter);
	}

  
	SBReach* reach = new SBReach(character, reachTag);
	//_reaches.insert(pair<std::string, SBReach*>(characterName, reach));
	_reaches[charReachTag] = reach;
	return reach;
}


SBReach* SBReachManager::createReach(std::string characterName)
{
	return createReachWithTag(characterName,"default");
}

void SBReachManager::removeReach(SBReach* reach)
{
	SBCharacter* character = reach->getCharacter();
	 
	// clean up all the reach structures...
	// ...
	// ...
  delete reach;
}

int SBReachManager::getNumReaches()
{
	return _reaches.size();
}

SBReach* SBReachManager::getReach(std::string characterName, std::string reachTag)
{
	std::string charReachTag = characterName + "_" + reachTag;
	std::map<std::string, SBReach*>::iterator iter = _reaches.find(charReachTag);
	if (iter != _reaches.end())
	{
		return (*iter).second;
	}
	else
	{
		return nullptr;
	}
}



}
