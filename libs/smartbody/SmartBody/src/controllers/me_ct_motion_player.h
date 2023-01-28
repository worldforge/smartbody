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

#ifndef _ME_CT_MOTION_PLAYER_H
#define _ME_CT_MOTION_PLAYER_H

#include <controllers/me_ct_container.hpp>
#include <controllers/me_ct_motion.h>
#include <sb/sbm_character.hpp>

#include <string>

class MeCtMotionPlayer : public MeCtContainer
{
public:
	static std::string CONTROLLER_TYPE;

	class Context : public MeCtContainer::Context 
	{
	protected:
		static std::string CONTEXT_TYPE;
	public:
		explicit Context( MeCtMotionPlayer* container, MeControllerContext* context = nullptr )
			:	MeCtContainer::Context( container, context )
		{}

		const std::string& context_type() const override {	return CONTEXT_TYPE; }
		void child_channels_updated( MeController* child ) override;
	};

public:
	explicit MeCtMotionPlayer(SmartBody::SBCharacter& c, const std::string& name, double n);
	~MeCtMotionPlayer() override;

	void init(const std::string& name, double n);

	void setFrameNum(double n);
	double getFrameNum() const;

	void setMotionName(const std::string& name);
	const std::string& getMotionName();

	void setActive(bool a);
	bool getActive() const;

	void controller_map_updated() override;
    SkChannelArray& controller_channels() override;
    double controller_duration() override;
	const std::string& controller_type() const override {return CONTROLLER_TYPE;}
	bool controller_evaluate( double t, MeFrameData& frame ) override;

private:
	SmartBody::SBCharacter&					character;
	boost::intrusive_ptr<MeController>					controller;
	SkChannelArray					channels;

	double							frameNum;
	std::string						motionName;
	bool							isActive;
};

#endif
