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

#include "SBServiceManager.h"
#include <sb/SBService.h>
#include "SBUtilities.h"

namespace SmartBody {

SBServiceManager::SBServiceManager() : SBObject()
{
}

SBServiceManager::~SBServiceManager()
{
	_services.clear();
}

int SBServiceManager::getNumServices()
{
	return _services.size();
}

std::vector<std::string> SBServiceManager::getServiceNames()
{
	std::vector<std::string> serviceNames;

	for (std::map<std::string, SBService*>::iterator iter = _services.begin();
		 iter != _services.end();
		 iter++)
	{
		serviceNames.emplace_back((*iter).first);
	}

	return serviceNames;
}

void SBServiceManager::addService(SBService* service)
{
	std::map<std::string, SBService*>::iterator iter = _services.find(service->getName());
	if (iter != _services.end())
	{
		SmartBody::util::log("Service named %s already exists. Service not added.", service->getName().c_str());
		return;
	}

	_services.insert(std::pair<std::string, SBService*>(service->getName(), service));
}

void SBServiceManager::removeService(const std::string& name)
{
	std::map<std::string, SBService*>::iterator iter = _services.find(name);
	if (iter == _services.end())
	{
		SmartBody::util::log("Service named %s does not exist. Service not removed.", name.c_str());
		return;
	}

	_services.erase(iter);
}

SBService* SBServiceManager::getService(const std::string& name)
{
	std::map<std::string, SBService*>::iterator iter = _services.find(name);
	if (iter == _services.end())
	{
		SmartBody::util::log("Service named %s does not exist.", name.c_str());
		return nullptr;
	}

	return (*iter).second;
}


std::map<std::string, SBService*>& SBServiceManager::getServices()
{
	return _services;
}


}