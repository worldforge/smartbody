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

#include <assert.h>
#include <sstream>
#include <algorithm>
#include <time.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <sr/sr_timer.h>

#include "sb/SBScene.h"
#include <controllers/me_ct_motion_blend.hpp>
#include <controllers/me_ct_example_body_reach.hpp>
#include "me_ct_barycentric_interpolation.h"
#include <sb/SBEvent.h>
#include <sb/SBSteerManager.h>


using namespace boost;

std::string MeCtMotionBlend::CONTROLLER_TYPE = "Blend";

/************************************************************************/
/* Motion Blending Controller                                           */
/************************************************************************/

MeCtMotionBlend::MeCtMotionBlend( std::map<int,MeCtBlendEngine*>& blendMap )  : SmartBody::SBController()
{
	
	currentBlendEngine = NULL;

	_duration = -1.f;	
	

	blendEngineMap = blendMap;
	ReachEngineMap::iterator mi;	
}

MeCtMotionBlend::~MeCtMotionBlend( void )
{
	
}


void MeCtMotionBlend::setParameterTargetPawn( SbmPawn* targetPawn )
{
	
}

void MeCtMotionBlend::setParameterTargetJoint( SkJoint* targetJoint )
{
	
}

void MeCtMotionBlend::setParameterTargetPos( SrVec& targetPos )
{
	
}

void MeCtMotionBlend::setParameterVector( dVector& para )
{

}

bool MeCtMotionBlend::controller_evaluate( double t, MeFrameData& frame )
{	
	//updateDefaultVariables(frame);	
		
	updateDt((float)t);	
	updateChannelBuffer(frame,inputMotionFrame,true);	

	// blending the input frame with ikFrame based on current fading
	bool finishFadeOut = updateFading(dt);
	
	if (!currentBlendEngine)
	{
    return true;
  }
		//SmartBody::util::log("update reach");
  currentBlendEngine->updateBlend((float)t,dt,inputMotionFrame);
	
	//printf("blend weight = %f\n",blendWeight);
	BodyMotionFrame outMotionFrame;
	MotionExampleSet::blendMotionFrame(inputMotionFrame,currentBlendEngine->outputMotion(),blendWeight,outMotionFrame);		
	updateChannelBuffer(frame,outMotionFrame);
	
	return true;
}

void MeCtMotionBlend::init(SmartBody::SBPawn* pawn)
{	
	IKTreeNodeList& nodeList = currentBlendEngine->ikTreeNodes();
	MeCtIKTreeNode* rootNode = nodeList[0];
	for (int i=0;i<3;i++)
		_channels.add(rootNode->joint->jointName(), (SkChannel::Type)(SkChannel::XPos+i));
	affectedJoints.clear();
	for (unsigned int i=0;i<nodeList.size();i++)
	{
		MeCtIKTreeNode* node = nodeList[i];
		SkJoint* joint = node->joint;
		SkJointQuat* skQuat = joint->quat();				
		affectedJoints.push_back(joint);
		_channels.add(joint->jointName(), SkChannel::Quat);		
	}			
	MeController::init(pawn);	
}

void MeCtMotionBlend::updateChannelBuffer( MeFrameData& frame, BodyMotionFrame& motionFrame, bool bRead /*= false*/ )
{
	SrBuffer<float>& buffer = frame.buffer();
	int count = 0;
	// update root translation
	for (int i=0;i<3;i++)
	{
		int index = frame.toBufferIndex(_toContextCh[count++]);
		if (bRead)
		{
			motionFrame.rootPos[i] = buffer[index] ;
		}
		else
		{
			buffer[index] = motionFrame.rootPos[i];			
		}
	}

	if (motionFrame.jointQuat.size() != affectedJoints.size())
		motionFrame.jointQuat.resize(affectedJoints.size());

	for (unsigned int i=0;i<motionFrame.jointQuat.size();i++)
	{
		SrQuat& quat = motionFrame.jointQuat[i];		
		int index = frame.toBufferIndex(_toContextCh[count++]);	
		//printf("buffer index = %d\n",index);		
		if (index == -1)
		{
			if (bRead)
			{
				quat = SrQuat();
			}
		}
		else
		{
			if (bRead)
			{
				quat.w = buffer[index] ;
				quat.x = buffer[index + 1] ;
				quat.y = buffer[index + 2] ;
				quat.z = buffer[index + 3] ;			
			}
			else
			{
				buffer[index] = quat.w;
				buffer[index + 1] = quat.x;
				buffer[index + 2] = quat.y;
				buffer[index + 3] = quat.z;			
			}
		}				
	}
}


void MeCtMotionBlend::controller_start()
{
	//restart = true;
	controlRestart();
}

void MeCtMotionBlend::controller_map_updated()
{

}
