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


#ifndef _ME_CT_BASIC_LOCOMOTION_H_
#define _ME_CT_BASIC_LOCOMOTION_H_

#include <controllers/me_controller.h>
#include <sb/sbm_character.hpp>
#include <sb/SBController.h>

class MeCtBasicLocomotion : public SmartBody::SBController
{
	public:
		explicit MeCtBasicLocomotion(SbmCharacter* c);
		~MeCtBasicLocomotion() override;

		bool controller_evaluate(double t, MeFrameData& frame) override;
		SkChannelArray& controller_channels() override	{return(_channels);}
		double controller_duration() override			{return -1;}
		const std::string& controller_type() const override		{return(_type_name);}
		
	public:
		static std::string _type_name;
		void setScootSpd(float v) {scootSpd = v;}
		float getScootSpd() const {return scootSpd;}
		void setMovingSpd(float v) {movingSpd = v;}
		float getMovingSpd() const {return movingSpd;}
		void setTurningSpd(float v) {turningSpd = v;}
		float getTurningSpd() const {return turningSpd;}
		void setValid(bool v) {_valid = v;}
		void setDesiredHeading(float v) {desiredHeading = v;}
		float getDesiredHeading() const {return desiredHeading;}

	protected:
		void updateWorldOffset(MeFrameData& frame, SrQuat& rot, SrVec& pos);

	private:
		bool _valid;
		float scootSpd;
		float movingSpd;
		float turningSpd;
		SkChannelArray _channels;
		SbmCharacter* character;
		double _lastTime;
		float desiredHeading;
};

#endif
