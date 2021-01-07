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
#ifndef _ME_MOTION_BLEND_CONTROLLER_
#define _ME_MOTION_BLEND_CONTROLLER_

#include "me_ct_data_interpolation.h"
#include "me_ct_barycentric_interpolation.h"
#include "me_ct_motion_parameter.h"
#include "me_ct_jacobian_IK.hpp"
#include "me_ct_ccd_IK.hpp"
#include "me_ct_constraint.hpp"

#include <controllers/MeCtBlendEngine.h>
#include <sb/SBController.h>

class ReachStateData;
class ReachStateInterface;
class ReachHandAction;

using namespace std;
// used when we want exact control for an end effector

typedef std::map<int,MeCtBlendEngine*> BlendEngineMap; 

class MeCtMotionBlend :
	public SmartBody::SBController, public FadingControl
{
private:
	static std::string CONTROLLER_TYPE;
public:	
	//enum HandActionState { PICK_UP_OBJECT = 0, TOUCH_OBJECT, PUT_DOWN_OBJECT };

protected:	
	int                   defaultBlendType;
	BlendEngineMap        blendEngineMap;
	MeCtBlendEngine*      currentBlendEngine;
	std::string           characterName;						
	vector<SkJoint*>      affectedJoints;		
	BodyMotionFrame       inputMotionFrame;		
	float 			      _duration;
	SkChannelArray	      _channels;

public:	
	MeCtMotionBlend(SmartBody::SBPawn& pawn, std::map<int,MeCtBlendEngine*>& blendMap);
	~MeCtMotionBlend() override;
	void controller_map_updated() override;
	void controller_start() override;
	bool controller_evaluate( double t, MeFrameData& frame ) override;

	SkChannelArray& controller_channels() override	{ return( _channels ); }
	double controller_duration() override			{ return( (double)_duration ); }
	const std::string& controller_type() const override		{ return( CONTROLLER_TYPE ); }

	MeCtBlendEngine* getBlendEngine() const { return currentBlendEngine; }	
	void set_duration(float duration) { _duration = duration; }

	void setParameterTargetPawn(SbmPawn* targetPawn);
	void setParameterTargetJoint(SkJoint* targetJoint);
	void setParameterTargetPos(SrVec& targetPos);
	void setParameterVector(dVector& para);
protected:
	void updateChannelBuffer(MeFrameData& frame, BodyMotionFrame& motionFrame, bool bRead = false);		
};

#endif





