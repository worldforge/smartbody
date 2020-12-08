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

#ifndef _SBNAVIGATIONMESHMANAGER_H_
#define _SBNAVIGATIONMESHMANAGER_H_

#include <sb/SBTypes.h>
#include <string>
#include <map>
#include <vector>

namespace SmartBody {

class SBNavigationMesh;

typedef std::pair<std::string,std::string> StringPair;

class SBNavigationMeshManager
{
	public:
		SBAPI SBNavigationMeshManager();
		SBAPI ~SBNavigationMeshManager();	

		SBAPI SBNavigationMesh* createNavigationMesh(const std::string& naviMeshName);
		SBAPI SBNavigationMesh* getNavigationMesh(const std::string& naviMeshName);		
		SBAPI std::vector<std::string> getNavigationMeshNames();

	protected:
		std::map<std::string, SBNavigationMesh*> _navigationMeshMap;
};

}
#endif