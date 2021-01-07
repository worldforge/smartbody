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
#ifndef _ME_BODY_REACH_CONTROLLER_
#define _ME_BODY_REACH_CONTROLLER_

#include "me_ct_data_interpolation.h"
#include "me_ct_barycentric_interpolation.h"
#include "me_ct_motion_parameter.h"
#include "me_ct_jacobian_IK.hpp"
#include "me_ct_ccd_IK.hpp"
#include "me_ct_constraint.hpp"

#include <sr/planner/sk_pos_planner.h>
#include <sr/planner/sk_blend_planner.h>
#include <sb/SBColObject.h>
#include <sb/sbm_pawn.hpp>

#include <controllers/MeCtReachEngine.h>
#include <sb/SBController.h>
#include <sb/SBEvent.h>
#include <sb/SBReach.h>

class ReachStateData;
class ReachStateInterface;
class ReachHandAction;

using namespace std;
// used when we want exact control for an end effector

typedef std::map<int,MeCtReachEngine*> ReachEngineMap; 

class MeCtExampleBodyReach :
	public SmartBody::SBController, public FadingControl, public SmartBody::SBEventHandler
{
private:
	static std::string CONTROLLER_TYPE;
public:	
	//enum HandActionState { PICK_UP_OBJECT = 0, TOUCH_OBJECT, PUT_DOWN_OBJECT };

protected:	
	int                   defaultReachType;
	ReachEngineMap        reachEngineMap;
	MeCtReachEngine*      currentReachEngine;
	std::string           characterName;		
	bool                  footIKFix;	
	bool                  useLocomotion;
	bool                  useProfileInterpolation;
	bool                  useRetiming;
	bool                  isMoving;
	bool                  startReach;
	bool                  endReach;
	bool                  locomotionReachTarget;
	float                 autoReturnDuration;
	float                 reachVelocityScale;	
	float                 desireLinearVel;
	float                 desireGrabSpeed;
	vector<SkJoint*>      affectedJoints;		
	BodyMotionFrame       inputMotionFrame;		
	float 			      _duration;
	SkChannelArray	      _channels;
	SmartBody::SBReach*   _reach;

public:	
	ReachStateData*       currentReachData;		

public:	
	explicit MeCtExampleBodyReach(SmartBody::SBReach* reach);

	void setReach( SmartBody::SBReach* reach );
	~MeCtExampleBodyReach() override;
	void controller_map_updated() override;
	void controller_start() override;
	bool controller_evaluate( double t, MeFrameData& frame ) override;

	SkChannelArray& controller_channels() override	{ return( _channels ); }
	double controller_duration() override			{ return( (double)_duration ); }
	const std::string& controller_type() const override		{ return( CONTROLLER_TYPE ); }

	MeCtReachEngine* getReachEngine() const { return currentReachEngine; }	
	void set_duration(float duration) { _duration = duration; }

	void setHandActionState(MeCtReachEngine::HandActionState newState);
	MeCtReachEngine::HandActionState getHandActionState();
	void setLinearVelocity(float vel);
	void setGrabSpeed( float vel );
	void setReachCompleteDuration(float duration);	
	bool addHandConstraint(SkJoint* targetJoint, const char* effectorName);
	SbmPawn* getAttachedPawn();
	void setReachTargetPawn(SbmPawn* targetPawn);
	void setReachTargetJoint(SkJoint* targetJoint);
	void setReachTargetPos(SrVec& targetPos);
	void setFinishReaching(bool isFinish);
	void setFootIK(bool useIK);
	void setUseLocomotion(bool useLoco);
	void setDefaultReachType(const std::string& reachTypeName);
	void notify(SBSubject* subject) override;

	SBAPI void executeAction(SmartBody::SBEvent* event) override;

protected:			
	void updateChannelBuffer(MeFrameData& frame, BodyMotionFrame& motionFrame, bool bRead = false);
	bool updateLocomotion(); // return true if locomotion is finished
	void updateReachType(SrVec& targetPos); // update the correct reach engine 
	int  determineReachType(SrVec& targetPos);
	int  getReachTypeWithAttachedPawn();
	void setNewReachEngine(MeCtReachEngine* newReachEngine);
	bool isValidReachEngine(int reachType);	
};

#endif





