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

#include "SBHandConfigurationManager.h"
#include <sb/SBHandConfiguration.h>
#include <sb/SBScene.h>
#include <sb/SBUtilities.h>

namespace SmartBody{

SBAPI SBHandConfigurationManager::SBHandConfigurationManager()
{
}

SBAPI SBHandConfigurationManager::~SBHandConfigurationManager()
{
	std::map<std::string, SBHandConfiguration*>::iterator iter = _configMap.begin();
	for (; iter != _configMap.end(); iter++)
	{
		delete iter->second;
	}
	_configMap.clear();
}

SBAPI SBHandConfiguration* SBHandConfigurationManager::createHandConfiguration(std::string configName)
{
	SBScene* scene = SmartBody::SBScene::getScene();

	std::map<std::string, SBHandConfiguration*>::iterator iter = _configMap.find(configName);
	if (iter != _configMap.end())
	{
		delete iter->second;
		_configMap.erase(iter);
	}

	SBHandConfiguration* config = new SBHandConfiguration();
	_configMap.insert(std::pair<std::string, SBHandConfiguration*>(configName, config));
	return config;
}

SBAPI void SBHandConfigurationManager::removeHandConfiguration(std::string configName)
{
	std::map<std::string, SBHandConfiguration*>::iterator iter = _configMap.find(configName);
	if (iter != _configMap.end())
	{
		delete iter->second;
		_configMap.erase(iter);
	}
}

SBAPI int SBHandConfigurationManager::getNumHandConfigurations()
{
	return _configMap.size();
}

SBAPI void SBHandConfigurationManager::printHandConfiguration(std::string configName)
{
	std::map<std::string, SBHandConfiguration*>::iterator iter = _configMap.find(configName);
	if (iter != _configMap.end())
	{
		std::string name = iter->first;
		SmartBody::util::log("Name of the hand configuration is %s",name.c_str());
		SBHandConfiguration* config = iter->second;
		config->printMotionNames();
	}
}

SBAPI SBHandConfiguration* SBHandConfigurationManager::getHandConfiguration(std::string configName)
{
	std::map<std::string, SBHandConfiguration*>::iterator iter = _configMap.find(configName);
	if (iter != _configMap.end())
	{
		return iter->second;
	}else{
		SmartBody::util::log("Unable to find hand configuration named %s",configName.c_str());
		return NULL;
	}
}

}


