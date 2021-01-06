#include <assert.h>
#include <sstream>
#include <algorithm>
#include <time.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <sr/sr_timer.h>
#include "MeCtReachEngine.h"

#include "controllers/me_ct_barycentric_interpolation.h"
#include "controllers/me_ct_inverse_interpolation.h"
#include <sb/SBEvent.h>
#include "MeCtBodyReachState.h"
#include <sb/sbm_character.hpp>
#include <sb/SBCharacter.h>
#include "SBUtilities.h"



/************************************************************************/
/* Reach Engine                                                         */
/************************************************************************/

const std::string lFootName[] = {"l_forefoot", "l_ankle" };
const std::string rFootName[] = {"r_forefoot", "r_ankle" };

std::string MeCtReachEngine::ReachTypeTag[REACH_TYPE_SIZE] = { "Right", "Left", "RightJump", "LeftJump" };

MeCtReachEngine::MeCtReachEngine( SbmCharacter* sbmChar, const boost::intrusive_ptr<SmartBody::SBSkeleton>& sk)
{
	character = sbmChar;
	skeletonCopy = new SmartBody::SBSkeleton(*sk);
	skeletonRef  = sk;
	dataInterpolator = nullptr;
	refMotion = nullptr;

	valid = false;
	ikInit = false;

	reachCompleteDuration = -1.0;
	fadingWeight = 0.f;	
	footIKFix = false;
	initStart = true;
	reachEndEffector = nullptr;
	curHandActionState  = TOUCH_OBJECT;		
	interpMotion = nullptr;
	motionParameter = nullptr;
	curReachState = nullptr;
}

MeCtReachEngine::~MeCtReachEngine( )
{

//	if (dataInterpolator)
//		delete dataInterpolator;
//	if (interpMotion)
//		delete interpMotion;
//	FREE_DATA(motionParameter);
}

void MeCtReachEngine::init(int rtype, SmartBody::SBJoint* effectorJoint)
{
	assert(skeletonRef);	
	assert(character);
	// root is "world_offset", so we use root->child to get the base joint.
	reachType = rtype;
	reachEndEffector = effectorJoint;
	SmartBody::SBJoint* rootJoint = findRootJoint(skeletonCopy.get());//findRootJoint(skeletonRef);//skeletonRef->root()->child(0);//skeletonCopy->root()->child(0);//skeletonRef->root()->child(0);
	std::vector<std::string> stopJoints;
	ikScenario.buildIKTreeFromJointRoot(rootJoint,stopJoints);
	ikCCDScenario.buildIKTreeFromJointRoot(rootJoint,stopJoints);	

	auto cons = std::make_unique<EffectorConstantConstraint>();
	cons->efffectorName = reachEndEffector->getMappedJointName().c_str();

	std::vector<std::string> consJointList;
	//consJointList.emplace_back("sternoclavicular");
	//consJointList.emplace_back("acromioclavicular");
	consJointList.emplace_back("shoulder");

	std::string preFix = "r_";	
	if (reachType == LEFT_ARM || reachType == LEFT_JUMP)
		preFix = "l_";	
	std::string consRootName = preFix + consJointList[0];	

	std::string thumbName = preFix+"thumb1";
	std::string fingerName = preFix + "finger1";
	std::string ringName = preFix + "ring1";
	std::string middle2Name = preFix + "middle2";

	SmartBody::SBJoint* thumbJoint = skeletonCopy->getJointByName(thumbName);
	SmartBody::SBJoint* effectorParent = effectorJoint->getParent();
	SmartBody::SBJoint* middle2Joint = skeletonCopy->getJointByName(middle2Name);
	SmartBody::SBJoint* fingerJoint = skeletonCopy->getJointByName(fingerName);
	SmartBody::SBJoint* ringJoint = skeletonCopy->getJointByName(ringName);

	SmartBody::SBJoint *pivotJoint = nullptr, *firstJoint = nullptr, *secondJoint = nullptr;
	if (effectorParent && ringJoint) // ideal configuration
	{
		pivotJoint = effectorParent;
		firstJoint = effectorJoint;
		secondJoint = ringJoint;
	}
	else if (thumbJoint && effectorParent)
	{
		pivotJoint = effectorParent;
		firstJoint = thumbJoint;
		secondJoint = effectorJoint;
	}

	SrVec effectorUp = SrVec(0,1,0);
	if (pivotJoint && firstJoint && secondJoint) // find up-axis for effector joint
	{
		SrVec v1 = firstJoint->gmatZero().get_translation() - pivotJoint->gmatZero().get_translation();
		SrVec v2 = secondJoint->gmatZero().get_translation() - pivotJoint->gmatZero().get_translation();
		effectorUp = cross(v2,v1); 
		effectorUp.normalize();
		if (preFix == "l_")
			effectorUp = -effectorUp;
	}

	//SmartBody::util::log("preFix = %s, effector up = %f %f %f", preFix.c_str(), effectorUp[0], effectorUp[1], effectorUp[2]);

	SrQuat offsetQuat = SrQuat(effectorUp,SrVec(0,1,0));
	SrMat offsetMat; offsetQuat.get_mat(offsetMat);
	
 	for (auto & i : consJointList)
 	{		
		consRootName = preFix + i;
		if (skeletonCopy->search_joint(consRootName.c_str()) != nullptr)
			break;
 	}
	
	cons->rootName = consRootName;
 	auto name = cons->efffectorName;
	reachPosConstraint[name] = std::move(cons);
	// if there is a child	
	if (reachEndEffector->num_children() > 0 && reachEndEffector->child(0))
	{
		auto rotCons = std::make_unique<EffectorConstantConstraint>();
		rotCons->efffectorName = reachEndEffector->getMappedJointName();
		rotCons->rootName = consRootName;
		reachRotConstraint[name] = std::move(rotCons);
	}	
	// setup foot constraint
	for (int i=0;i<2;i++)
	{
		auto lFoot = std::make_unique<EffectorConstantConstraint>();
		lFoot->efffectorName = lFootName[i];
		lFoot->rootName = "";
		leftFootConstraint[lFoot->efffectorName] = std::move(lFoot);

		auto rFoot = std::make_unique<EffectorConstantConstraint>();
		rFoot->efffectorName = rFootName[i];
		rFoot->rootName = "";
		rightFootConstraint[rFoot->efffectorName] = std::move(rFoot);
	}	

	ikScenario.ikPosEffectors = &reachPosConstraint;
	ikScenario.ikRotEffectors = &reachRotConstraint;

	const IKTreeNodeList& nodeList = ikScenario.ikTreeNodes;	
	idleMotionFrame.jointQuat.resize(nodeList.size());
	inputMotionFrame.jointQuat.resize(nodeList.size());
	ikMotionFrame.jointQuat.resize(nodeList.size());

	affectedJoints.clear();	
	for (auto node : nodeList)
	{
			SmartBody::SBJoint* joint = skeletonCopy->getJointByName(node->getNodeName());
		SkJointQuat* skQuat = joint->quat();		
		affectedJoints.emplace_back(joint);
	}		

	SmartBody::SBJoint* copyEffector = skeletonCopy->getJointByName(reachEndEffector->getMappedJointName());
	SmartBody::SBJoint* copyRoot = skeletonCopy->getJointByName(rootJoint->parent()->getMappedJointName());
	motionParameter = new ReachMotionParameter(skeletonCopy.get(),affectedJoints,copyEffector,copyRoot);
	motionExamples.initMotionExampleSet(motionParameter);	

	// initialize all parameters according to scale	
	float characterHeight = character->getHeight();
	ikReachRegion = characterHeight*0.02f;	
	ikDefaultVelocity = characterHeight*0.3f;
	ikDamp        = ikReachRegion*ikReachRegion*20.0;//characterHeight*0.1f;

	SbmCharacter* curCharacter = character;

	std::string rootName = ikScenario.ikTreeRoot->joint->parent()->jointName();
	SrVec initRootPosition = skeletonRef->getJointByName(rootName)->gmat().get_translation();

	reachData = std::make_unique<ReachStateData>(character->_scene);
	reachData->characterHeight = characterHeight;		
	reachData->charName = character->getName();
	reachData->centerOffset = (curCharacter->getBoundingBox().getMaximum()*0.7f+curCharacter->getBoundingBox().getMinimum()*0.3f) - initRootPosition;
	reachData->reachRegion = ikReachRegion;	
	reachData->linearVel = ikDefaultVelocity;
	reachData->curRefTime = 0.f;
	reachData->motionParameter = motionParameter;
	reachData->idleRefFrame = reachData->currentRefFrame = reachData->targetRefFrame = idleMotionFrame;	
	reachData->reachType = reachType;
	

	EffectorState& estate = reachData->effectorState;
	estate.effectorName = reachEndEffector->getMappedJointName().c_str();
	estate.curIKTargetState = reachData->getPoseState(idleMotionFrame);
	estate.gmatZero = copyEffector->gmatZero()*offsetMat;//copyEffector->gmatZero()*offsetMat;


	stateTable["Idle"] = std::make_unique<ReachStateIdle>();
	stateTable["Start"] = std::make_unique<ReachStateStart>();
	//stateTable["Move"] = new ReachStateMove();
	stateTable["Complete"] = std::make_unique<ReachStateComplete>();
	stateTable["NewTarget"] = std::make_unique<ReachStateNewTarget>();
	stateTable["PreReturn"] = std::make_unique<ReachStatePreReturn>();
	stateTable["Return"] = std::make_unique<ReachStateReturn>();

	handActionTable[PICK_UP_OBJECT] = new ReachHandPickUpAction(character->_scene);
	handActionTable[TOUCH_OBJECT] = new ReachHandAction(character->_scene); // default hand action
	handActionTable[PUT_DOWN_OBJECT] = new ReachHandPutDownAction(character->_scene);
	handActionTable[POINT_AT_OBJECT] = new ReachHandPointAction(character->_scene);

	curReachState = getState("Idle");
}

void MeCtReachEngine::updateMotionExamples( const MotionDataSet& inMotionSet, std::string interpolatorType )
{
	if (inMotionSet.size() == 0)
		return;

	// set world offset to zero	
	std::string rootName = ikScenario.ikTreeRoot->joint->parent()->getMappedJointName();
	SkJoint* root = skeletonRef->search_joint(rootName.c_str());

	if (root)
	{
		root->quat()->value(SrQuat());
	}
	skeletonCopy->root()->quat()->value(SrQuat());
	for (int i=0;i<3;i++)
	{
		skeletonCopy->root()->pos()->value(i,0.f);
    if (root) {
      root->pos()->value(i,0.f);
    }
	}

	SmartBody::SBJoint* rootJoint = affectedJoints[0];
	BOOST_FOREACH(TagMotion tagMotion, inMotionSet)
	{
		if (tagMotion.first != reachType) // only process motion with correct tag 
			continue;

		SmartBody::SBMotion* motion = dynamic_cast<SmartBody::SBMotion*>(tagMotion.second);
		if (!motion)
			continue;
		if (motionData.find(tagMotion) != motionData.end())
			continue; // we do not process example motions that are already used for this controller instance
		if (!refMotion)
			refMotion = motion;

		motionData.insert(tagMotion);
		auto* ex = new MotionExample(character->_scene);
		ex->motion = motion;		
		ex->timeWarp = std::make_unique<SimpleTimeWarp>(refMotion->duration(),motion->duration());
		ex->motionParameterFunc = motionParameter;
		ex->motionProfile = new MotionProfile(motion);
		//ex->updateRootOffset(skeletonCopy,rootJoint);
#if 0
		ex->motionProfile->buildVelocityProfile(0.f,motion->duration()*0.999f,0.005f);
		ex->motionProfile->buildInterpolationProfile(0.f,(float)motion->time_stroke_emphasis(),0.005f);
		ex->motionProfile->buildEulerCurveProfile(0.f,(float)motion->time_stroke_emphasis(),0.005f);
#endif
		ex->getMotionParameter(ex->parameter);		
		// set initial index & weight for the motion example
		// by default, the index should be this motion & weight should be 1
		InterpWeight w;
		w.first = motionExamples.getExamples().size();
		w.second = 1.f;
		ex->weight.emplace_back(w);
		// add the example parameter for visualization purpose
		SrVec reachPos;
		for (int i=0;i<3;i++)
			reachPos[i] = (float)ex->parameter[i];
		examplePts.emplace_back(reachPos);
		motionExamples.addMotionExample(ex);
	}	

	if (motionExamples.getExamples().empty()) // no examples
	{
		//valid = false;
		return;
	}

	if (dataInterpolator)
		delete dataInterpolator;

	dataInterpolator = createInterpolator(interpolatorType);
	dataInterpolator->init(&motionExamples);
	dataInterpolator->buildInterpolator();		

	if (resampleData)
	{
		for (unsigned int i=0;i<resampleData->size();i++)
		{
			InterpolationExample* ex = (*resampleData)[i];
			SrVec reachPos;
			for (int k=0;k<3;k++)
				reachPos[k] = (float)ex->parameter[k];
			resamplePts.emplace_back(reachPos);
		}
	}

	if (interpMotion)
		delete interpMotion;
	interpMotion = createInterpMotion();

	// initialize the interpolation weights
	//dVector para; para.resize(3);
	//for (int i=0;i<3;i++)
	//	para[i] = curEffectorPos[i];
	//if (interpMotion && dataInterpolator)
	//	dataInterpolator->predictInterpWeights(para,interpMotion->weight);

	reachData->interpMotion = interpMotion;
	reachData->dataInterpolator = dataInterpolator;
	reachData->useExample = true;
}



bool MeCtReachEngine::hasEffectorRotConstraint( ReachStateData* rd )
{	
	// 	if (rd->curHandAction == handActionTable[PICK_UP_OBJECT] || rd->curHandAction == handActionTable[PUT_DOWN_OBJECT])
	// 		return true;
	// 	else if (rd->curHandAction == handActionTable[TOUCH_OBJECT])
	// 		return false;
	// 	return false;
	return true;
}

void MeCtReachEngine::solveIK( ReachStateData* rd, BodyMotionFrame& outFrame )
{	
	BodyMotionFrame& refFrame = !motionData.empty() ? rd->currentRefFrame : inputMotionFrame;
	if (!ikInit)
	{			
		ikScenario.setTreeNodeQuat(refFrame.jointQuat,QUAT_INIT);
		ikScenario.setTreeNodeQuat(refFrame.jointQuat,QUAT_CUR);	
		ikInit = true;
	}		
	EffectorState& estate = rd->effectorState;

	auto cons = dynamic_cast<EffectorConstantConstraint*>(reachPosConstraint[reachEndEffector->getMappedJointName()].get());
	cons->targetPos = estate.curIKTargetState.tran;	
	//SmartBody::util::log("constraint position = %f %f %f",cons->targetPos[0],cons->targetPos[1], cons->targetPos[2]);
	
// 	if (curReachState->curStateName() == "Start" || curReachState->curStateName() == "Complete" )
// 	{
// 		SrVec ikTarget = rd->reachTarget.getTargetState().tran;
// 		//SmartBody::util::log("state time = %f",rd->stateTime);
// 		SmartBody::util::log("ikTarget = %f %f %f",ikTarget[0],ikTarget[1],ikTarget[2]);
// 		//SmartBody::util::log("targetPos = %f %f %f",cons->targetPos[0],cons->targetPos[1],cons->targetPos[2]);	
// 	}
	//SmartBody::util::log("gmat %s",rd->gmat.toString().c_str());
	//SmartBody::util::log("rootPos = %s", refFrame.rootPos.toString().c_str());
	ikScenario.ikGlobalMat = rd->gmat;//skeletonRef->search_joint(rootName)->gmat();//ikScenario.ikTreeRoot->joint->parent()->gmat();	
	ikScenario.ikTreeRootPos = refFrame.rootPos;
	ikScenario.setTreeNodeQuat(refFrame.jointQuat,QUAT_REF);		
	ikScenario.ikPosEffectors = &reachPosConstraint;

	
	{
		auto cons = dynamic_cast<EffectorConstantConstraint*>(reachRotConstraint[reachEndEffector->getMappedJointName().c_str()].get());
		if (!cons)
			return;		
		cons->targetRot = estate.curIKTargetState.rot;//ikRotTrajectory;//ikRotTarget;//motionParameter->getMotionFrameJoint(interpMotionFrame,reachEndEffector->name().get_string())->gmat();//ikRotTarget;	
		cons->constraintWeight = 0.f;//1.f - rd->blendWeight;	
		cons->gmatZero = estate.gmatZero;

		//if (rd->curHandAction == handActionTable[PICK_UP_OBJECT] || rd->curHandAction == handActionTable[PUT_DOWN_OBJECT])
		if (hasEffectorRotConstraint(rd) && motionData.size() != 0)
			ikScenario.ikRotEffectors = &reachRotConstraint;
		//else if (rd->curHandAction == handActionTable[TOUCH_OBJECT])
		else
			ikScenario.ikRotEffectors = &reachNoRotConstraint;
	}

	//sr_out << "target pos = " << estate.curTargetState.tran << " , target rot = " << estate.curTargetState.rot << srnl;
	if (curHandActionState != POINT_AT_OBJECT)
	{
		ik.maxOffset = ikMaxOffset;
		ik.dampJ = ikDamp*0.5f;
		ik.refDampRatio = 0.05;
		for (int i=0;i<10;i++)
		{
			ik.update(&ikScenario);		
			ikScenario.copyTreeNodeQuat(QUAT_CUR,QUAT_INIT);		
		}
	}	
	else
	{
		ikScenario.setTreeNodeQuat(refFrame.jointQuat,QUAT_INIT);
		ikScenario.setTreeNodeQuat(refFrame.jointQuat,QUAT_CUR);
	}
	//ikCCD.update(&ikScenario);

	
	if (footIKFix)
	{
		bool hasIKJoints = true;
		if (!skeletonCopy->search_joint(lFootName[0].c_str()) || !skeletonCopy->search_joint(lFootName[0].c_str()))
		{
			// does not have foot IK joints, do not solve for foot IK
			hasIKJoints = false;
		}

		if (hasIKJoints)
		{
			for (int i=0;i<2;i++)
			{			
				auto* lfoot = dynamic_cast<EffectorConstantConstraint*>(leftFootConstraint[lFootName[i]].get());
				lfoot->targetPos = motionParameter->getMotionFrameJoint(inputMotionFrame,lFootName[i].c_str())->gmat().get_translation();
				auto* rfoot = dynamic_cast<EffectorConstantConstraint*>(rightFootConstraint[rFootName[i]].get());
				rfoot->targetPos = motionParameter->getMotionFrameJoint(inputMotionFrame,rFootName[i].c_str())->gmat().get_translation();				
			} 			
			ikScenario.ikPosEffectors = &leftFootConstraint;
			ikCCD.update(&ikScenario);
			ikScenario.ikPosEffectors = &rightFootConstraint;
			ikCCD.update(&ikScenario);	
			ikScenario.copyTreeNodeQuat(QUAT_CUR,QUAT_INIT);
		}		
	}	

	outFrame = refFrame;
	ikScenario.getTreeNodeQuat(outFrame.jointQuat,QUAT_CUR); 	
}

void MeCtReachEngine::updateSkeletonCopy()
{
	skeletonCopy->root()->quat()->value(skeletonRef->root()->quat()->value());
	for (int i=0;i<3;i++)
		skeletonCopy->root()->pos()->value(i,skeletonRef->root()->pos()->value(i));
}

ReachStateInterface* MeCtReachEngine::getState( const std::string& stateName )
{
	auto I = stateTable.find(stateName);
	if (I != stateTable.end()) {
		return I->second.get();
	}
	return nullptr;
}

SmartBody::SBJoint* MeCtReachEngine::findRootJoint( SmartBody::SBSkeleton* sk )
{

	auto* rootJoint = dynamic_cast<SmartBody::SBJoint*>(sk->root()->child(0)); // skip world offset
// 	if (sk->search_joint("base"))
// 	{
// 		rootJoint = sk->search_joint("base");
// 		return rootJoint;
// 	}
	
	bool bStop = false;
	while (!bStop)
	{
		if (rootJoint->num_children() == 0)
			return dynamic_cast<SmartBody::SBJoint*>(rootJoint);
		SmartBody::SBJoint* child = rootJoint->getChild(0);
		SkJointPos* skRootPos = rootJoint->pos();		
		SkJointPos* skPos = child->pos();
		bool rootFrozen = (skRootPos->frozen(0) && skRootPos->frozen(1) && skRootPos->frozen(2));
		bool childFrozen = (skPos->frozen(0) && skPos->frozen(1) && skPos->frozen(2));
		if (!rootFrozen) // find the top joint (excluding world offset) that has both translation + rotation
		{
			bStop = true;
		}
		else if (child)
		{
			rootJoint = child;
		}
		else
		{
			// error ? 
			rootJoint = dynamic_cast<SmartBody::SBJoint*>(sk->root()->child(0));
			bStop = true;
		}
	}
	SmartBody::util::log("ReachEngine Root Name = %s\n",rootJoint->getName().c_str());
	return dynamic_cast<SmartBody::SBJoint*>(rootJoint);
}

DataInterpolator* MeCtReachEngine::createInterpolator(std::string interpolatorType)
{
	DataInterpolator* interpolator = nullptr;
	if (interpolatorType == "KNN")
	{
		KNNInterpolator* knnInterpolator = new KNNInterpolator(500,ikReachRegion*1.f);
		resampleData = &knnInterpolator->resampleData;	
		interpolator = knnInterpolator;
	}
	else if (interpolatorType == "Inverse")
	{
		InverseInterpolation* ibinterpolator = new InverseInterpolation(); // Inverse Blending
		interpolator = ibinterpolator;
		resampleData = nullptr;
	}
	else if (interpolatorType == "RBF")
	{
		RBFInterpolator* rbfinterpolator = new RBFInterpolator();
		interpolator = rbfinterpolator;
		resampleData = nullptr;
	}
  if (interpolator) {
    interpExampleData = interpolator->getInterpExamples();
  }
	return interpolator;
}

ResampleMotion* MeCtReachEngine::createInterpMotion()
{
	ResampleMotion* ex = new ResampleMotion(motionExamples.getMotionData());
	ex->motionParameterFunc = motionParameter;
	ex->motionProfile = nullptr;
	return ex;
}

void MeCtReachEngine::updateReach(float t, float dt, BodyMotionFrame& inputFrame, float blendWeight)
{
	float du = 0.0;
	//SmartBody::util::log("reach type = %s, dt = %f",getReachTypeTag().c_str(),dt);
	if (initStart) // first start
	{		
		idleMotionFrame = inputFrame;
		reachData->idleRefFrame = reachData->currentRefFrame = reachData->targetRefFrame = idleMotionFrame;
		curReachState = getState("Idle");
		ikMotionFrame = idleMotionFrame;		
		initStart = false;
	}		

	inputMotionFrame = inputFrame;
	reachData->idleRefFrame = inputFrame;

	SmartBody::SBCharacter* curCharacter = dynamic_cast<SmartBody::SBCharacter*>(character);
	skeletonRef->update_global_matrices();	
	updateSkeletonCopy();	
	// update reach data
	std::string rootName = ikScenario.ikTreeRoot->joint->parent()->getMappedJointName();
	reachData->curTime = (float)t;
	reachData->dt = dt;	
	reachData->stateTime += dt;
	reachData->curHandAction = handActionTable[curHandActionState];	
	reachData->updateReachState(skeletonRef->search_joint(rootName.c_str())->gmat(),ikMotionFrame);
	//reachData->shoulderPosition;
	std::string shoulderJoint = (reachType == LEFT_ARM) ? "l_sternoclavicular" : "r_sternoclavicular";
	reachData->shoulderPosition = reachData->gmat.get_translation()+reachData->centerOffset;
	if (skeletonRef->search_joint(shoulderJoint.c_str()))
		reachData->shoulderPosition = skeletonRef->search_joint(shoulderJoint.c_str())->gmat().get_translation();
	
	if (curCharacter)
	{		
		reachData->locomotionComplete = (curCharacter->_reachTarget && !curCharacter->_lastReachStatus);		
	}
	
	//reachData->hasSteering = (getScene()->getSteerManager()->isInitialized());

	curReachState->updateEffectorTargetState(reachData.get());
	curReachState->update(reachData.get());
	ReachStateInterface* nextState = getState(curReachState->nextState(reachData.get()));
	if (nextState != curReachState)
	{
		//SmartBody::util::log("engine type = %s,  cur State = %s\n",this->getReachTypeTag().c_str(), nextState->curStateName().c_str());
		reachData->stateTime = 0.f;
		reachData->effectorState.startTargetState = reachData->effectorState.curIKTargetState;
		reachData->effectorState.curBlendState = reachData->effectorState.startTargetState;
		reachData->startRefFrame = reachData->currentRefFrame;
		curReachState = nextState;
	}

	ikMaxOffset = ikDefaultVelocity*3.f*dt;	
 	if (footIKFix)
 	{
 		solveIK(reachData.get(),ikMotionFrame);
 	}
 	else
	{
		ikMotionFrame = reachData->currentRefFrame;
	}
	
	//ikMotionFrame = reachData->currentRefFrame;
}

bool MeCtReachEngine::addHandConstraint( SmartBody::SBJoint* targetJoint, const char* effectorName )
{
	MeCtIKTreeNode* node = ikScenario.findIKTreeNode(effectorName);
	if (!node || !targetJoint)
		return false;

	std::string str = effectorName;		
	auto ci = handConstraint.find(str);
	if (ci != handConstraint.end())
	{		
		auto* cons = dynamic_cast<EffectorJointConstraint*>((*ci).second.get());
		cons->targetJoint = targetJoint;
	}
	else // add effector-joint pair
	{
		// initialize constraint
		auto cons = std::make_unique<EffectorJointConstraint>();
		cons->efffectorName = effectorName;
		cons->targetJoint = targetJoint;
		handConstraint[str] = std::move(cons);
	}
	return true;
}

std::string MeCtReachEngine::getReachTypeTag() const
{
	return ReachTypeTag[reachType];
}

int MeCtReachEngine::getReachTypeID() const
{
	return reachType;
}

int MeCtReachEngine::getReachType( const std::string& tag )
{
	for (int i=0;i<REACH_TYPE_SIZE;i++)
	{
		if (boost::iequals(tag,ReachTypeTag[i]))
			return i;
	}
	return -1;
}

std::string MeCtReachEngine::getAttachedPawnName()
{
	SmartBody::SBPawn* pawn = reachData->effectorState.getAttachedPawn();
	if (pawn)
	{
		return pawn->getName();
	}
	else
	{
		return "null";
	}
}
