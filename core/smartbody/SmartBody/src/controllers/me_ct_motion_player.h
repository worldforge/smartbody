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
		Context( MeCtMotionPlayer* container, MeControllerContext* context = nullptr )
			:	MeCtContainer::Context( container, context )
		{}

		const std::string& context_type() const {	return CONTEXT_TYPE; }
		void child_channels_updated( MeController* child );
	};

public:
	MeCtMotionPlayer(SbmCharacter* c);
	~MeCtMotionPlayer();

	void init(SmartBody::SBPawn* pawn, std::string name, double n);

	void setFrameNum(double n);
	double getFrameNum();

	void setMotionName(const std::string& name);
	const std::string& getMotionName();

	void setActive(bool a);
	bool getActive();

	virtual void controller_map_updated();
    virtual SkChannelArray& controller_channels();
    virtual double controller_duration();
	virtual const std::string& controller_type() const {return CONTROLLER_TYPE;}
	virtual bool controller_evaluate( double t, MeFrameData& frame );

private:
	SbmCharacter*					character;
	MeController*					controller;		
	SkChannelArray					channels;

	double							frameNum;
	std::string						motionName;
	bool							isActive;
};

#endif
