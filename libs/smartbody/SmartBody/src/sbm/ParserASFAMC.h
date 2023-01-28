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

#ifndef ASFAMCPARSER_H
#define ASFAMCPARSER_H


#include <fstream>
#include <sb/SBSkeleton.h>
#include <sb/SBMotion.h>

struct JointInfo
{
	std::string name;
	SrVec direction;
	float length;
	SrVec axis;
	std::vector<std::string> dof;
};

class ParserASFAMC
{
	public:
		static bool parseAsf(SmartBody::SBSkeleton& skeleton, std::ifstream& metaFile, float scale);
		static bool parseAmc(SmartBody::SBMotion& motion, SmartBody::SBSkeleton* skeleton, std::ifstream& dataFile, float scale);
	
	private:
		static int getMotionChannelId(SkChannelArray& channels, std::string channelType, std::string jointName);
		static JointInfo* getJointInfo(std::string jointName, std::vector<JointInfo*>& jointInfoList);
};

#endif