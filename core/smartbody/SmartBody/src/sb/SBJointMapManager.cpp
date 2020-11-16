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


#include "SBJointMapManager.h"
#include <sb/SBJointMap.h>
#include <sb/SBScene.h>
#include <sb/SBSceneListener.h>
#include "SBUtilities.h"

namespace SmartBody {

SBJointMapManager::SBJointMapManager()
{
}

SBJointMapManager::~SBJointMapManager()
{
}

SmartBody::SBJointMap* SBJointMapManager::getJointMap(const std::string& name)
{
	std::map<std::string, SmartBody::SBJointMap*>::iterator iter = _jointMaps.find(name);
	if (iter != _jointMaps.end())
		return (*iter).second;
	else
		return NULL;
}

SmartBody::SBJointMap* SBJointMapManager::createJointMap(const std::string& name)
{
	std::map<std::string, SmartBody::SBJointMap*>::iterator iter = _jointMaps.find(name);
	if (iter == _jointMaps.end())
	{
		SmartBody::SBJointMap* map = new SmartBody::SBJointMap();
		_jointMaps[name] = map;
		map->setName(name);

		std::vector<SBSceneListener*>& listeners = SmartBody::SBScene::getScene()->getSceneListeners();
		for (size_t l = 0; l < listeners.size(); l++)
		{
			listeners[l]->OnObjectCreate(map);
		}

		return map;
	}
	else
	{
		return NULL;
	}
}

std::vector<std::string> SBJointMapManager::getJointMapNames()
{
	std::vector<std::string> names;
	for (std::map<std::string, SmartBody::SBJointMap*>::iterator iter = _jointMaps.begin();
		 iter != _jointMaps.end();
		 iter++)
	{
		names.push_back((*iter).first);
	}

	return names;
}

void SBJointMapManager::removeJointMap(const std::string& name)
{
	std::map<std::string, SmartBody::SBJointMap*>::iterator iter = _jointMaps.find(name);
	if (iter != _jointMaps.end())
	{
		SmartBody::SBJointMap* map = (*iter).second;
		std::vector<SBSceneListener*>& listeners = SmartBody::SBScene::getScene()->getSceneListeners();
		for (size_t l = 0; l < listeners.size(); l++)
		{
			listeners[l]->OnObjectDelete(map);
		}

		_jointMaps.erase(iter);
		delete map;
		return;
	}
	

	SmartBody::util::log("Cannot find joint map %s, will not be erased.", name.c_str());
}

void SBJointMapManager::removeAllJointMaps()
{
	for (std::map<std::string, SmartBody::SBJointMap*>::iterator iter = _jointMaps.begin();
		 iter != _jointMaps.end();
		 iter++)
	{
		delete (*iter).second;
	}
	_jointMaps.clear();
}


}
