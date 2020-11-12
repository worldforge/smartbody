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

#ifndef _SBJOINTMAPMANAGER_
#define _SBJOINTMAPMANAGER_

#include <sb/SBTypes.h>
#include <string>
#include <vector>
#include <map>

namespace SmartBody {

class SBJointMap;

class SBJointMapManager 
{
	public:
		SBAPI SBJointMapManager();
		SBAPI ~SBJointMapManager();
	
		SBAPI SmartBody::SBJointMap* createJointMap(const std::string& name);
		SBAPI SmartBody::SBJointMap* getJointMap(const std::string& name);
		SBAPI std::vector<std::string> getJointMapNames();
		SBAPI void removeJointMap(const std::string& name);
		SBAPI void removeAllJointMaps();

	private:
		std::map<std::string, SmartBody::SBJointMap*> _jointMaps;

};


}

#endif
