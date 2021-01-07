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

#ifndef _ME_CT_MOTION_RECORDER_H
#define _ME_CT_MOTION_RECORDER_H

#include <sb/SBController.h>
#include <sb/SBCharacter.h>
#include <string>

class MeCtMotionRecorder : public SmartBody::SBController
{
public:
	static std::string CONTROLLER_TYPE;
	
public:
	explicit MeCtMotionRecorder(SmartBody::SBCharacter* c);
	~MeCtMotionRecorder() override;

	void startRecording(double frameRate);
	void stopRecording(const std::string& motionName, const std::string& type);
	void writeRecording(const std::string& motionName, const std::string& type);
	void controller_map_updated() override;
    SkChannelArray& controller_channels() override;
    double controller_duration() override;
	const std::string& controller_type() const override {return CONTROLLER_TYPE;}
	bool controller_evaluate( double t, MeFrameData& frame ) override;

protected:
	SkJoint* findRootJoint( SmartBody::SBSkeleton* skel );

	void getJointChannelValues(const std::string& jointName, MeFrameData& frame, SrQuat& outQuat, SrVec& outPos);
	void setJointChannelQuat(const std::string& jointName, MeFrameData& frame, SrQuat& outQuat);
	void setJointChannelPos(const std::string& jointName, MeFrameData& frame, SrVec& outPos);

protected:
	SmartBody::SBCharacter*		  character;	
	SkChannelArray				  channels;
	bool		                  recordStart;
	double                        prevTime, startTime;
	double                        recordDt;
};

#endif
