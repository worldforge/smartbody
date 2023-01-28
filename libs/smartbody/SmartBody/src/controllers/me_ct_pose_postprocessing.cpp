#include "controllers/me_ct_pose_postprocessing.hpp"
#include <boost/foreach.hpp>
#include <utility>
using namespace gwiz;

std::string MeCtPosePostProcessing::CONTROLLER_TYPE = "PostProcessing";


MeCtPosePostProcessing::MeCtPosePostProcessing(SmartBody::SBCharacter& pawn, boost::intrusive_ptr<SmartBody::SBSkeleton> skeleton ) : MeCtConstraint(pawn, std::move(skeleton))
{
	firstIKSolve = true;			
}

MeCtPosePostProcessing::~MeCtPosePostProcessing() = default;


void MeCtPosePostProcessing::updatePoseConstraint()
{
	std::vector<std::string> consNames = _sbChar->getJointConstraintNames();
	for (auto & consName : consNames)
	{
		SmartBody::TrajectoryRecord* trajRecord = _sbChar->getJointTrajectoryConstraint(consName);
		EffectorConstantConstraint* cons = nullptr;
		if (posConstraint.find(consName) == posConstraint.end())
		{
			cons = new EffectorConstantConstraint();
			posConstraint[consName].reset(cons);
			cons->efffectorName = consName;
			cons->rootName = trajRecord->refJointName;
			if (consName[0] == 'l')
				cons->rootName = "l_hip";
			else if (consName[0] == 'r')
				cons->rootName = "r_hip";
		}	
		else
		{
			cons = dynamic_cast<EffectorConstantConstraint*>(posConstraint[consName].get());
		}		
		cons->targetPos = trajRecord->jointTrajGlobalPos;
 		if (!trajRecord->isEnable)
 			cons->constraintWeight = 0.f;
 		else
			cons->constraintWeight = 1.f;
	}
}

bool MeCtPosePostProcessing::controller_evaluate( double t, MeFrameData& frame )
{	
	static int counter = 0;

	std::vector<SrQuat> tempQuatList; tempQuatList.resize(ik_scenario.ikTreeNodes.size());
	if (prev_time == -1.0) // first start
	{		
		// for first frame, update from frame buffer to joint quat in the limb
		// any future IK solving will simply use the joint quat from the previous frame.		
		updateChannelBuffer(frame,tempQuatList,true);			
		ik_scenario.setTreeNodeQuat(tempQuatList,QUAT_INIT);
		ik_scenario.setTreeNodeQuat(tempQuatList,QUAT_PREVREF);
		ik_scenario.setTreeNodeQuat(tempQuatList,QUAT_CUR);
	}

	updateDt((float)t);

	updateChannelBuffer(frame,tempQuatList,true);
	ik_scenario.setTreeNodeQuat(tempQuatList,QUAT_REF);

	prev_time = (float)t;	

	std::vector<std::string> consNames = _sbChar->getJointConstraintNames();
	if (consNames.empty()) // if the character doesn't have any constraints, don't solve IK
		return true;


	updatePoseConstraint(); // update the joint trajectory as constraints

	bool hasConstraint = false;
	ConstraintMap::iterator ci;
	//SmartBody::util::log("Counter = %d",counter);
	for (ci = posConstraint.begin(); ci != posConstraint.end(); ci++)
	{
		auto& cons = ci->second;//posConstraint[i];
		MeCtIKTreeNode* node = ik_scenario.findIKTreeNode(cons->efffectorName.c_str());
		if (node)
			node->targetPos = cons->getPosConstraint();				
		//if (counter < 10)
		//	SmartBody::util::log("node %s : target pos = %f %f %f", node->nodeName.c_str(),node->targetPos[0],node->targetPos[1],node->targetPos[2]);
	}	
	counter++;

	_skeleton->update_global_matrices();
	ik_scenario.ikTreeRoot->joint->parent()->update_gmat_up(); // update world offset global transformation	
	ik_scenario.ikGlobalMat = ik_scenario.ikTreeRoot->joint->parent()->gmat();
	for (int i=0;i<3;i++)
		ik_scenario.ikTreeRootPos[i] = ik_scenario.ikTreeRoot->joint->pos()->value(i);


	ik.setDt(dt);
	if (fadeMode == FADING_MODE_IN)
		useIKConstraint = true;

	bool finishFadeOut = updateFading(dt);	
	if (finishFadeOut)
		useIKConstraint = false;

	float transitionWeight = _sbChar->getJointTrajBlendWeight();
	if (transitionWeight > 1.f) transitionWeight = 1.f;
	//SmartBody::util::log("joint transition weight = %f",transitionWeight);
	if (useIKConstraint && transitionWeight > 0.0) //&& ik_scenario.ikEndEffectors.size() != 0)	
	{	
		if (firstIKSolve)
		{
			updateChannelBuffer(frame,tempQuatList,true);			
			ik_scenario.setTreeNodeQuat(tempQuatList,QUAT_INIT);
			ik_scenario.setTreeNodeQuat(tempQuatList,QUAT_PREVREF);
			ik_scenario.setTreeNodeQuat(tempQuatList,QUAT_CUR);
		}


		if (!firstIKSolve)
		{
			//sr_out << "global offset mat = " << ik_scenario.ikGlobalMat << srnl;
			ik.dampJ = ikDamp;
			ik.refDampRatio = 0.01;
			for (int i=0;i<1;i++)
			{
				//printf("IK Iteration %d\n",i);
				ik.update(&ik_scenario);
				//ikCCD.update(&ik_scenario);
				//ik_scenario.updateGlobalMat();
				ik_scenario.copyTreeNodeQuat(QUAT_CUR,QUAT_INIT);
			}

			//ik_scenario.ikInitQuatList = ik_scenario.ikQuatList;
			ik_scenario.getTreeNodeQuat(tempQuatList,QUAT_CUR);			
			for (unsigned int i=0;i<ik_scenario.ikTreeNodes.size();i++)
			{
				MeCtIKTreeNode* node = ik_scenario.ikTreeNodes[i];
				SrQuat qEval = node->getQuat(QUAT_CUR);//ik_scenario.ikQuatList[i];
				//SrQuat qEval = node->getQuat(QUAT_REF);
				SrQuat qInit = node->getQuat(QUAT_REF);
				qEval.normalize();
				qInit.normalize();
				//tempQuatList[i] = qEval;//slerp(qInit,qEval,blendWeight);	
				tempQuatList[i] = slerp(qInit,qEval,transitionWeight);	
				//tempQuatList[i].normalize();
				//node->setQuat(tempQuatList[i],QUAT_INIT);
			}	
			updateChannelBuffer(frame,tempQuatList);	
		}
		firstIKSolve = false;			
	}	
	return true;
}
