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

#include "SBNavigationMeshManager.h"
#include <sb/SBNavigationMesh.h>
#include "SBUtilities.h"

namespace SmartBody {

	
SBAPI SBNavigationMeshManager::SBNavigationMeshManager()
{

}

SBAPI SBNavigationMeshManager::~SBNavigationMeshManager()
{

}

SBNavigationMesh* SBNavigationMeshManager::createNavigationMesh( const std::string& naviMeshName )
{
	SBNavigationMesh* naviMesh = NULL;
	if (_navigationMeshMap.find(naviMeshName) != _navigationMeshMap.end())
	{
		SmartBody::util::log("Navigation mesh '%s' already exists. Returns the copy in system. ", naviMeshName.c_str());
		return _navigationMeshMap[naviMeshName];
	}

	naviMesh = new SBNavigationMesh();
	naviMesh->setName(naviMeshName);
	_navigationMeshMap[naviMeshName] = naviMesh;
	return naviMesh;
}

SBNavigationMesh* SBNavigationMeshManager::getNavigationMesh( const std::string& naviMeshName )
{
	std::map<std::string, SBNavigationMesh*>::iterator iter = _navigationMeshMap.find(naviMeshName);
	SBNavigationMesh* naviMesh = NULL;
	if (iter != _navigationMeshMap.end())
		naviMesh = iter->second;
	return naviMesh;
}

std::vector<std::string> SBNavigationMeshManager::getNavigationMeshNames()
{
	std::vector<std::string> ret;

	for(std::map<std::string, SBNavigationMesh*>::iterator iter = _navigationMeshMap.begin();
		iter != _navigationMeshMap.end();
		iter++)
	{
		ret.push_back(std::string(iter->first));
	}

	return ret;	
}

}
