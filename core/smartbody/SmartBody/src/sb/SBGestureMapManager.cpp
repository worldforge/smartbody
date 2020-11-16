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

#include "SBGestureMapManager.h"
#include <sb/SBScene.h>
#include <sb/SBGestureMap.h>
#include <sb/SBSceneListener.h>
#include "SBUtilities.h"

namespace SmartBody {

SBGestureMapManager::SBGestureMapManager()
{
}

SBGestureMapManager::~SBGestureMapManager()
{
	std::map<std::string, SBGestureMap*>::iterator iter = _gestureMaps.begin();
	for (; iter != _gestureMaps.end(); iter++)
	{
		delete iter->second;
	}
	_gestureMaps.clear();
}

SBGestureMap* SBGestureMapManager::createGestureMap(std::string gestureName)
{
	
	SBScene* scene = SmartBody::SBScene::getScene();

	std::map<std::string, SBGestureMap*>::iterator iter = _gestureMaps.find(gestureName);
	if (iter != _gestureMaps.end())
	{
		delete iter->second;
		_gestureMaps.erase(iter);
	}
	
	SBGestureMap* map = new SBGestureMap(gestureName);
	_gestureMaps.insert(std::make_pair(gestureName, map));

	std::vector<SBSceneListener*>& listeners = SmartBody::SBScene::getScene()->getSceneListeners();
	for (size_t l = 0; l < listeners.size(); l++)
	{
		listeners[l]->OnObjectCreate(map);
	}

	return map;
}

void SBGestureMapManager::removeGestureMap(std::string gestureName)
{
	std::map<std::string, SBGestureMap*>::iterator iter = _gestureMaps.find(gestureName);
	if (iter != _gestureMaps.end())
	{
		std::vector<SBSceneListener*>& listeners = SmartBody::SBScene::getScene()->getSceneListeners();
		for (size_t l = 0; l < listeners.size(); l++)
		{
			listeners[l]->OnObjectDelete((*iter).second);
		}

		delete iter->second;
		_gestureMaps.erase(iter);
	}
	else
		SmartBody::util::log("Gesture map %s doesn't exist!", gestureName.c_str());
}

int SBGestureMapManager::getNumGestureMaps()
{
	return _gestureMaps.size();
}

std::vector<std::string> SBGestureMapManager::getGestureMapNames()
{
	std::vector<std::string> gestureMapNames;
	for (std::map<std::string, SBGestureMap*>::iterator iter = _gestureMaps.begin();
		 iter != _gestureMaps.end();
		 iter++)
	{
		gestureMapNames.push_back((*iter).first);
	}

	return gestureMapNames;
}



SBGestureMap* SBGestureMapManager::getGestureMap(std::string gestureName)
{
	std::map<std::string, SBGestureMap*>::iterator iter = _gestureMaps.find(gestureName);
	if (iter != _gestureMaps.end())
	{
		return (*iter).second;
	}
	else
	{
		return NULL;
	}
}

}
