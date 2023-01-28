#pragma once
#include "controllers/me_ct_example_body_reach.hpp"

namespace SmartBody {
	class SBPawn;
	class SBScene;
}

class ReachTarget
{
public:
	SmartBody::SBScene& _scene;
	SRT      targetState;
	SRT      paraTargetState;
	//SbmPawn* targetPawn;
	std::string targetPawnName;
	SkJoint* targetJoint;
	bool     useTargetPawn, useTargetJoint;
public:
	explicit ReachTarget(SmartBody::SBScene& scene);
	~ReachTarget() = default;
	ReachTarget& operator= (const ReachTarget& rt);
	bool targetIsPawn();
	bool targetHasGeometry();
	bool targetIsJoint();
	void setTargetState(SRT& ts);
	void setTargetPawnName(std::string pawnName);
	void setTargetJoint(SkJoint* tjoint);
	SRT getTargetState(); // the return state is based on target's state
	SRT getParaTargetState(); // the return state is based on target's state
	SRT getGrabTargetState(SRT& naturalState, float offset = 0.f); 
	SmartBody::SBPawn* getTargetPawn();
	std::string getTargetPawnName() const { return targetPawnName; }
};

class EffectorState 
{
public:
	SmartBody::SBScene& _scene;
	std::string  effectorName;
	SRT startTargetState, curBlendState; 
	SRT curIKTargetState, ikTargetState;
	SRT curState; 
	SRT grabStateError;
	SrVec paraTarget;
	SrMat gmatZero;	
	//SbmPawn* attachedPawn;	
	std::string attachedPawnName;
	SrMat    attachMat;
public:
	explicit EffectorState(SmartBody::SBScene& scene);
	~EffectorState() = default;
	SmartBody::SBPawn* getAttachedPawn();
	void setAttachedPawn(ReachStateData* rd);
	void removeAttachedPawn(ReachStateData* rd);	
};

struct ReachHandAction // dafault hand behavior for "Touch"
{
	explicit ReachHandAction(SmartBody::SBScene& scene): _scene(scene) {}
	SmartBody::SBScene& _scene;
	virtual void reachPreCompleteAction(ReachStateData* rd) ;
	virtual void reachCompleteAction(ReachStateData* rd) ;	
	virtual void reachPreReturnAction(ReachStateData* rd);
	virtual void reachNewTargetAction(ReachStateData* rd) ;
	virtual void reachReturnAction(ReachStateData* rd) ;
	virtual void reachPostCompleteAction(ReachStateData* rd);
	virtual SRT getHandTargetStateOffset(ReachStateData* rd, SRT& naturalState);
	virtual bool isPickingUpNewPawn(ReachStateData* rd) ;
	

	void sendReachEvent(const std::string& etype, const std::string& cmd, const std::string& source, float time = 0.0);
	void pickUpAttachedPawn(ReachStateData* rd);
	void putDownAttachedPawn(ReachStateData* rd);
	virtual int getType();
	std::string generateGrabCmd(const std::string& charName, const std::string& targetName, const std::string& grabState, int type, float grabSpeed);
	std::string generateAttachCmd(const std::string& charName, const std::string& targetName, int type, float grabSpeed);
};

struct ReachHandPickUpAction : public ReachHandAction
{
	explicit ReachHandPickUpAction(SmartBody::SBScene& scene): ReachHandAction(scene) {}

	void reachCompleteAction(ReachStateData* rd) override;
	void reachNewTargetAction(ReachStateData* rd) override;
	void reachPreReturnAction(ReachStateData* rd) override {} ;
	void reachReturnAction(ReachStateData* rd) override; // do nothing when return
	void reachPostCompleteAction(ReachStateData* rd) override;
	int getType() override;
};

struct ReachHandPutDownAction : public ReachHandAction
{
	explicit ReachHandPutDownAction(SmartBody::SBScene& scene): ReachHandAction(scene) {}
	void reachPreCompleteAction(ReachStateData* rd) override {}; // do nothing when moving toward target
	void reachCompleteAction(ReachStateData* rd) override;
	void reachPreReturnAction(ReachStateData* rd) override;
	void reachReturnAction(ReachStateData* rd) override;
	SRT getHandTargetStateOffset(ReachStateData* rd, SRT& naturalState) override;
	int getType() override;
};

struct ReachHandPointAction : public ReachHandAction // dafault hand behavior for "Touch"
{
	explicit ReachHandPointAction(SmartBody::SBScene& scene): ReachHandAction(scene) {}
	void reachPreCompleteAction(ReachStateData* rd) override;
	void reachCompleteAction(ReachStateData* rd) override {};
	void reachPreReturnAction(ReachStateData* rd) override {};
	void reachNewTargetAction(ReachStateData* rd) override {};
	void reachPostCompleteAction(ReachStateData* rd) override {};
	SRT getHandTargetStateOffset(ReachStateData* rd, SRT& naturalState) override;
	bool isPickingUpNewPawn(ReachStateData* rd) override;
	int getType() override;
};

class ReachStateData
{
public:
	SmartBody::SBScene& _scene;
	std::string     charName; // character name
	int             reachType;
	float           curTime, dt;
	float           stateTime; // how much time has been in a state 
	float           curRefTime, du;	
	float           blendWeight;
	float           retimingFactor;
	SrMat           gmat;
	SrVec           centerOffset;
	SrVec           shoulderPosition;
	// reference motion frames ( as motion interpolation output, or IK reference pose )
	BodyMotionFrame idleRefFrame, startRefFrame, targetRefFrame, currentRefFrame;		
	BodyMotionFrame curMotionFrame; 
	EffectorState   effectorState;
	ReachTarget     reachTarget;
	SRT             desireHandState;	
	SrVec           HandUpVector;
	// flags for state transition
	float           autoReturnTime;
	float           characterHeight;
	bool            startReach, endReach;
	bool            ikReachTarget;
	bool            useExample;	
	bool            locomotionComplete;
	bool            hasSteering;
	bool            useProfileInterpolation;
	bool            useRetiming;
	bool            newTarget;

	// for pick-up/put-down action
	ReachHandAction* curHandAction;

	// for motion interpolation
	ResampleMotion* interpMotion;
	MotionParameter* motionParameter;
	DataInterpolator* dataInterpolator;
	MeCtExampleBodyReach* reachControl;
public:
	float linearVel;
	float grabSpeed;
	float reachRegion;
public:	
	explicit ReachStateData(SmartBody::SBScene& scene);
	~ReachStateData();
	void updateReachState(const SrMat& worldOffset, BodyMotionFrame& motionFrame); // update corresponding parameters according to current body frame
	void updateBlendWeight(SrVec paraPos);
	void getInterpFrame(float refTime, BodyMotionFrame& outFrame);
	SRT getBlendPoseState(SrVec paraPos, float refTime);
	SRT getPoseState(BodyMotionFrame& frame);
	bool useInterpolation();	
	float XZDistanceToTarget(SrVec& pos);
	bool hasAttachedPawn();
};

class ReachStateInterface
{	
public:
	virtual ~ReachStateInterface() = default;
	virtual void update(ReachStateData* rd) = 0;
	virtual void updateEffectorTargetState(ReachStateData* rd) = 0;	
	virtual float curStatePercentTime(ReachStateData* rd, float refTime)  { return 0.f; };
	virtual std::string nextState(ReachStateData* rd) = 0;
	virtual std::string curStateName() = 0;
protected:
	void updateReturnToIdle(ReachStateData* rd);
	void updateReachToTarget(ReachStateData* rd);
	void updateMotionIK(ReachStateData* rd);
	void updateMotionPoseInterpolation(ReachStateData* rd);
	void updateMotionInterp(ReachStateData* rd);
	bool ikTargetReached(ReachStateData* rd, float ratio = 0.1f);
	bool poseTargetReached(ReachStateData* rd, float ratio = 0.1f);
	bool interpTargetReached(ReachStateData* rd);	
};

class ReachStateIdle : public ReachStateInterface
{
public:
	virtual void update(ReachStateData* rd);
	virtual void updateEffectorTargetState(ReachStateData* rd);	
	virtual std::string nextState(ReachStateData* rd);	
	virtual std::string curStateName() { return "Idle"; }
};

class ReachStateStart : public ReachStateInterface
{
public:
	virtual void update(ReachStateData* rd);
	virtual void updateEffectorTargetState(ReachStateData* rd);	
	virtual float curStatePercentTime(ReachStateData* rd, float refTime);
	virtual std::string nextState(ReachStateData* rd);	
	virtual std::string curStateName() { return "Start"; }
};

class ReachStateComplete : public ReachStateInterface
{
protected:
	float completeTime;
public:
	ReachStateComplete() { completeTime = 0.f; }
	virtual void update(ReachStateData* rd);
	virtual void updateEffectorTargetState(ReachStateData* rd);	
	virtual std::string nextState(ReachStateData* rd);	
	virtual std::string curStateName() { return "Complete"; }
};

class ReachStateNewTarget : public ReachStateInterface
{
public:
	virtual void update(ReachStateData* rd);
	virtual void updateEffectorTargetState(ReachStateData* rd);	
	virtual std::string nextState(ReachStateData* rd);
	virtual std::string curStateName() { return "NewTarget"; }
};

class ReachStatePreReturn : public ReachStateComplete
{
public:
	ReachStatePreReturn() : ReachStateComplete() {}
	virtual void update(ReachStateData* rd) {}
	virtual void updateEffectorTargetState(ReachStateData* rd) {} // do nothing
	virtual std::string nextState(ReachStateData* rd);	
	virtual std::string curStateName() { return "PreReturn"; }
};

class ReachStateReturn : public ReachStateInterface
{
public:
	virtual void update(ReachStateData* rd);
	virtual void updateEffectorTargetState(ReachStateData* rd);	
	virtual float curStatePercentTime(ReachStateData* rd, float refTime);
	virtual std::string nextState(ReachStateData* rd);
	virtual std::string curStateName() { return "Return"; }
};


