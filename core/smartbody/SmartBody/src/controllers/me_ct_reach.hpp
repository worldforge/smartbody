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

#pragma once
#include <sk/sk_skeleton.h>
#include <sb/SBController.h>
#include <sb/SBMotion.h>
#include "me_ct_limb.hpp"
#include "me_ct_reach_IK.hpp"
#include "sbm/gwiz_math.h"

typedef std::pair<int,SmartBody::SBMotion*> TagMotion; // pair the motion with a tag
typedef std::set<TagMotion> MotionDataSet;

class MeCtReach : public SmartBody::SBController
{
private:
	static std::string CONTROLLER_TYPE;

public:
	enum ReachMode { TARGET_POS = 1, TARGET_JOINT_POS };	
	enum ReachArm { REACH_LEFT_ARM = 1, REACH_RIGHT_ARM };

public:	
	MeCtReach();
	MeCtReach(SkSkeleton* skeleton);
	~MeCtReach(void);

protected:
	ReachMode            reach_mode;
	ReachArm             reach_arm;	
	MeCtReachIK          ik;
	MeCtLimb             limb;
	float                limb_length;	
	SrArray<const char*> joint_name;	
	SrArray<MeCtIKJointLimit> joint_limit;
	float 			_duration;
	SkChannelArray	_channels;
	SrVec   		target_pos;
	SkJoint        *target_joint_ref;
	SkJoint        *root_joint_ref; // root of target chain	
	SkSkeleton*     _skeleton;
	float           prev_time; // to get dt


public:	
	void set_target_pos(SrVec& target_pos);
	void set_target_joint(SkJoint* target_joint);	

	void init(SmartBody::SBPawn* pawn);
	virtual void controller_map_updated();
	virtual void controller_start();	
	virtual bool controller_evaluate( double t, MeFrameData& frame );
	virtual SkChannelArray& controller_channels()	{ return( _channels ); }
	virtual double controller_duration()			{ return( (double)_duration ); }
	void set_duration(float duration) { _duration = duration; }
	virtual const std::string& controller_type() const		{ return( CONTROLLER_TYPE ); }

	void setReachArm(MeCtReach::ReachArm val) { reach_arm = val; }
	MeCtReach::ReachArm getReachArm() { return reach_arm; }
	float getLimbLength() const { return limb_length; }
	SkJoint* getRootJoint() const { return root_joint_ref; }

protected:
	SrVec get_reach_target();
};
