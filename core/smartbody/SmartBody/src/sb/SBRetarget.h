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

#ifndef _SBRETARGET_H_
#define _SBRETARGET_H_

#include <sb/SBTypes.h>
#include <sr/sr_quat.h>
#include <string>
#include <vector>
#include <map>



namespace SmartBody {

typedef std::pair<SrQuat,SrQuat> QuatPair;
typedef std::pair<std::string,std::string> StringPair;
class TrajectoryRecord;

class SBRetarget
{
	public:
		SBAPI SBRetarget();
		SBAPI SBRetarget(std::string srcName, std::string tgtName);		
		SBAPI ~SBRetarget();	
		SBAPI bool initRetarget(std::vector<std::string>& endJoints, std::vector<std::string>& relativeJoints);
		SBAPI SrQuat applyRetargetJointRotation(std::string jointName, SrQuat& inQuat);
		SBAPI SrQuat applyRetargetJointRotationInverse(std::string jointName, SrQuat& inQuat);
		SBAPI float  applyRetargetJointTranslation(std::string jointName, float inPos);		
		SBAPI SrVec  applyRetargetJointTranslationVec(std::string jointName, SrVec inVec);		

		SBAPI SrVec  applyRetargetJointTrajectory(TrajectoryRecord& trajRecord, SrMat& baseGmat);
		SBAPI std::vector<std::string> getEndJointNames();
		SBAPI std::vector<std::string> getRelativeJointNames();
		SBAPI float getHeightRatio();
		SBAPI void addJointRotOffset(std::string jointName, SrQuat& inQuat);
	protected:
		void computeJointLengthRatio(std::string jointName, std::string refJointName);
	protected:
		std::string srcSkName;
		std::string tgtSkName;
		std::map<std::string, QuatPair> jointPrePostRotMap;
		std::map<std::string, bool> jointSkipMap;
		std::map<std::string, SrQuat> jointAddRotMap;
		std::map<std::string, SrQuat> jointRotOffsetMap;
		std::vector<std::string> retargetEndJoints;
		std::vector<std::string> retargetRelativeJoints;		
		float heightRatio;
		std::map<StringPair, float> jointLengthRatioMap;
};

}


#endif