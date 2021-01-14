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

#ifndef _SBHANDCONFIGURATION_H_
#define _SBHANDCONFIGURATION_H_

#include <sb/SBScene.h>
#include <string>
#include "SBSceneOwned.h"

namespace SmartBody{


class SBHandConfiguration : public SBSceneOwned {

public:
	SBAPI explicit SBHandConfiguration(SBScene& scene);
	SBAPI ~SBHandConfiguration();

	SBAPI void addMotion(const std::string& motionName);
	SBAPI void removeMotion(const std::string& motionName);
	SBAPI int getNumMotions();
	SBAPI void printMotionNames();
	SBAPI SBMotion* getMotion(int i);
	SBAPI std::vector<std::string> getMotionNames();

private:
	
	// loaded motions
	std::vector<SBMotion*> _motions;
};

}

#endif