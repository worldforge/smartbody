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

#pragma once

#ifndef _SBHANDCONFIGURATIONMANAGER_H_
#define _SBHANDCONFIGURATIONMANAGER_H_

#include <sb/SBTypes.h>
#include <string>
#include <map>
#include <vector>

namespace SmartBody {

class SBHandConfiguration;

class SBHandConfigurationManager{

public:

	SBAPI SBHandConfigurationManager();
	SBAPI ~SBHandConfigurationManager();

	// create a hand configuration
	SBAPI SBHandConfiguration* createHandConfiguration(std::string configName);
	SBAPI SBHandConfiguration* getHandConfiguration(std::string configName);
	SBAPI void removeHandConfiguration(std::string configName);
	SBAPI int getNumHandConfigurations();
	SBAPI void printHandConfiguration(std::string configName);

private:

	// map of configurations
	std::map<std::string, SBHandConfiguration*> _configMap;
};



}
#endif