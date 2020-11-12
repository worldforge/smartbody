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

#ifndef _KINECT_PROCESSOR_H_
#define _KINECT_PROCESSOR_H_

#include <string>
#include <vector>
#include <list>
#include <map>
#include <sr/sr_quat.h>
class KinectProcessor
{
	public:
		KinectProcessor();
		~KinectProcessor();
		int getNumBones();
		const char* getSBJointName(int i);
		void setSBJointName(int i, const char* jName);
		static void processGlobalRotation(std::vector<SrQuat>& quats);
		void processRetargetRotation(std::string targetSkelName, std::vector<SrQuat>& inQuats, std::vector<SrQuat>& outQuat);
		void processRetargetPosition(std::string targetSkelName, SrVec& inPos, SrVec& outPos);
		void filterRotation(std::vector<SrQuat>& quats);
		// routines to reconstruct a kinect skeleton from joint data
		void inferJointOffsets(std::vector<SrVec>& gPos, std::vector<SrQuat>& gRot, std::vector<SrVec>& out, std::map<int, SrQuat>& bonePreRotMap);
		void initKinectSkeleton(std::vector<SrVec>& gPos, std::vector<SrQuat>& gRot);
		void refineKinectSkeleton(std::vector<SrVec>& gPos, std::vector<SrQuat>& gRot);

	private:
		std::vector<std::string>			boneMapping;
		std::vector<std::list<SrQuat> >		rotationBuffer;
		std::map<int,int>                   parentIndexMap;
		int									filterSize;
		int                                 numOfKinectJoints;
};
#endif
