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

#ifndef _SBCONTROLLER_
#define _SBCONTROLLER_

#include <sb/SBTypes.h>
#include "controllers/me_controller.h"


namespace SmartBody {

class SBControllerModifier;


class SBController : public MeController
{
	public:
		SBAPI explicit SBController(SmartBody::SBPawn& pawn);
		SBAPI SBController(const SBController& controller);
		SBAPI ~SBController() override;

		SBAPI const std::string& getType();

//		SBAPI SBController* getParent();			// how to get parent?
//		SBAPI SBController* getChild(int index);
//		SBAPI int getNumChildren();
		SBAPI void addChannel(std::string channelName, std::string type);

		SBAPI SBPawn* getPawn();
		SBAPI const std::string& getCharacterName();

		SBAPI void setIgnore(bool val);
		SBAPI bool isIgnore();
		SBAPI void setDebug(bool val);
		SBAPI bool isDebug();

//		SBAPI void printInfo();
//		SBAPI void startRecordSkm(int maxFrame);
//		SBAPI void startRecordBvh(int maxFrame);
//		SBAPI void writeRecord(std::string prefix);
//		SBAPI void stopRecord();
//		SBAPI void clearRecord();
		SBAPI double getDuration();

//		SBAPI void setTiming(float indt, float outdt, float empht);

		SBAPI void getJointChannelValues(const std::string& jointName, MeFrameData& frame, SrMat& outMat);
		SBAPI void getJointChannelValues(const std::string& jointName, MeFrameData& frame, SrQuat& outQuat, SrVec& outPos);
		SBAPI void setJointChannelQuat(const std::string& jointName, MeFrameData& frame, SrQuat& outQuat);
		SBAPI void setJointChannelPos(const std::string& jointName, MeFrameData& frame, SrVec& outPos);

		SBAPI double getChannelValue(const std::string& channelName);
		SBAPI SrVec getChannelPos(const std::string& channelName);
		SBAPI SrQuat getChannelQuat(const std::string& channelName);

		SBAPI void setChannelValue(const std::string& channelName, double val);
		SBAPI void setChannelPos(const std::string& channelName, SrVec pos);
		SBAPI void setChannelQuat(const std::string& channelName, SrQuat quat);
		SBAPI void setChannelQuatGlobal(const std::string& channelName, SrQuat quat);
		SBAPI SrQuat convertChannelQuatGlobal(const std::string& channelName, SrQuat quat);
		
		SBAPI bool controller_evaluate ( double t, MeFrameData& frame ) override { return true;}
		SBAPI double controller_duration () override { return 0.0; }
		SBAPI SkChannelArray& controller_channels () override { return channelArray; }
		SBAPI const std::string& controller_type () const override { return controllerType; }

		SBAPI void addControllerModifier(SBControllerModifier* modifier);
 		SBAPI void removeControllerModifier(SBControllerModifier* modifier);
		SBAPI void removeAllControllerModifiers();
		SBAPI SmartBody::SBControllerModifier* getControllerModifier(const std::string& name);
		SBAPI std::vector<SmartBody::SBControllerModifier*>& getControllerModifiers();


	private:
		SkChannelArray channelArray;
		std::string controllerType;
		std::vector<SmartBody::SBControllerModifier*> _modifiers;

};

}

#endif
