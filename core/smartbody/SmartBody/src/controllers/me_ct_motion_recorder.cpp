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

#include <controllers/me_ct_motion_recorder.h>
#include <sb/SBSkeleton.h>
#include <sb/SBScene.h>
#include "SBUtilities.h"

std::string MeCtMotionRecorder::CONTROLLER_TYPE = "MeCtMotionRecorder";

MeCtMotionRecorder::MeCtMotionRecorder(SmartBody::SBCharacter* c) : SmartBody::SBController(*c), character(c)
{	
	prevTime = -1.0;
	startTime = -1.0;
	_record_mode = RECORD_NULL;
	_record_max_frames = -1;
	recordDt = 0.03;
	recordStart = false;
}

MeCtMotionRecorder::~MeCtMotionRecorder() = default;

void MeCtMotionRecorder::startRecording( double frameRate )
{
	// make sure that all the channels are available for recording
	auto& scene = getScene();
	//auto skel = scene.getSkeleton(_pawn.getSkeleton()->getName());
	auto skel = _pawn.getSkeleton();
	if (!skel) 
		return;

	channels.clear();

	SkChannelArray& skelChannels = character->getSkeleton()->channels();
	for (int c = 0; c < skelChannels.size(); c++)
	{
		SkChannel& channel = skelChannels.get(c);
		channels.add(channel);
	}

	if (frameRate > 0)
	{
		recordDt = 1.0/frameRate;
		_record_dt = recordDt;
	}
	stop_record();
	recordStart = true;
	prevTime = -1.0;
	startTime = -1.0;
	_record_mode = RECORD_MOTION;	
	_record_max_frames = -1;
}

void MeCtMotionRecorder::stopRecording(const std::string& motionName, const std::string& type)
{
	if (!recordStart)
	{
		return; // haven't started recording yet. Nothing to save.
	}
	recordStart = false;
	if (type == "bvh" || type == "BVH")
	{
		_record_mode = RECORD_BVH_MOTION;
	}
	else if (type == "skm" || type == "SKM")
	{
		_record_mode = RECORD_MOTION;
	}
	else
	{
		SmartBody::util::log("Exported file format %s not supported, export to skm instead", type.c_str());
	}
	saveMotionRecord(motionName.c_str());
	stop_record();
}

void MeCtMotionRecorder::writeRecording(const std::string& motionName, const std::string& type)
{
	if (!recordStart)
	{
		return;
	}
	if (type == "bvh" || type == "BVH")
	{
		_record_mode = RECORD_BVH_MOTION;
	}
	else if (type == "skm" || type == "SKM")
	{
		_record_mode = RECORD_MOTION;
	}
	else
	{
		SmartBody::util::log("Exported file format %s not supported, export to skm instead", type.c_str());
	}
	saveMotionRecord(motionName);
}

void MeCtMotionRecorder::setJointChannelPos( const std::string& jointName, MeFrameData& frame, SrVec& outPos )
{
	bool hasTranslation = true;
	int positionChannelID = _context->channels().search(jointName, SkChannel::XPos);
	if (positionChannelID < 0) hasTranslation = false;
	int posBufferID = frame.toBufferIndex(positionChannelID);
	if (posBufferID < 0) hasTranslation = false;
	//SmartBody::util::log("MotionRecorder : jointname = %s, posChannelID = %d, posBufferID = %d",jointName.c_str(), positionChannelID, posBufferID);
	if (hasTranslation)
	{		
		frame.buffer()[posBufferID + 0] = outPos[0];
		frame.buffer()[posBufferID + 1] = outPos[1];
		frame.buffer()[posBufferID + 2] = outPos[2];				
	}
}

void MeCtMotionRecorder::setJointChannelQuat( const std::string& jointName, MeFrameData& frame, SrQuat& inQuat )
{
	bool hasRotation = true;
	int channelId = _context->channels().search(jointName, SkChannel::Quat);
	if (channelId < 0)	hasRotation = false;
	int bufferId = frame.toBufferIndex(channelId);
	if (bufferId < 0)	hasRotation = false;	

	if (hasRotation)
	{
		frame.buffer()[bufferId + 0] = inQuat.w;
		frame.buffer()[bufferId + 1] = inQuat.x;;
		frame.buffer()[bufferId + 2] = inQuat.y;;
		frame.buffer()[bufferId + 3] = inQuat.z;;
	}
}

void MeCtMotionRecorder::getJointChannelValues( const std::string& jointName, MeFrameData& frame, SrQuat& outQuat, SrVec& outPos )
{
	bool hasRotation = true;
	int channelId = _context->channels().search(jointName, SkChannel::Quat);
	if (channelId < 0)	hasRotation = false;
	int bufferId = frame.toBufferIndex(channelId);
	if (bufferId < 0)	hasRotation = false;	

	bool hasTranslation = true;
	int positionChannelID = _context->channels().search(jointName, SkChannel::XPos);
	if (positionChannelID < 0) hasTranslation = false;
	int posBufferID = frame.toBufferIndex(positionChannelID);
	if (posBufferID < 0) hasTranslation = false;		
	// input reference pose
	if (hasRotation)
	{
		outQuat.w = frame.buffer()[bufferId + 0];
		outQuat.x = frame.buffer()[bufferId + 1];
		outQuat.y = frame.buffer()[bufferId + 2];
		outQuat.z = frame.buffer()[bufferId + 3];
	}
	if (hasTranslation)
	{
		outPos.x = frame.buffer()[posBufferID + 0];
		outPos.y = frame.buffer()[posBufferID + 1];
		outPos.z = frame.buffer()[posBufferID + 2];				
	}
}


void MeCtMotionRecorder::controller_map_updated()
{
	
}

SkChannelArray& MeCtMotionRecorder::controller_channels()
{
	return channels;
}

double MeCtMotionRecorder::controller_duration()
{
	return -1;
}

bool MeCtMotionRecorder::controller_evaluate(double t, MeFrameData& frame)
{	
	if (!recordStart || !character->getSkeleton()) // do nothing if not recording
		return true;	
	bool recordFrame = false;
	if (prevTime < 0.0)
	{
		prevTime = t;
		startTime = t;
		recordFrame = true;
	}

	if (t - prevTime >= recordDt) // start recording frame
	{
		recordFrame = true;	
		prevTime = t;
	}

	if (recordFrame)
	{
		// swap joint rotation + joint translation	
		auto skel = character->getSkeleton();
		skel->update_global_matrices();
		
		SmartBody::SBJoint* rootJoint = skel->getJoint(0);
		SrQuat oldQuat;
		SrVec oldPos;	
		getJointChannelValues(rootJoint->getName(),frame, oldQuat, oldPos);
		if (rootJoint) // set the frame buffer value from current global mat
		{		
			SrMat lMat = rootJoint->gmat()*rootJoint->gmatZero().inverse();
			SrVec pos = lMat.get_translation();		
			SrQuat temp(lMat);
			setJointChannelQuat(rootJoint->getName(),frame,temp);
			SrVec trans = lMat.get_translation();
			setJointChannelPos(rootJoint->getName(),frame,trans);
		}
		// record the current joint values into motion buffer
		
		cont_record(t-startTime,frame);

		// restore the root quat/pos so it does not interfere with current simulation
		if (rootJoint)
		{
			setJointChannelQuat(rootJoint->getName(),frame,oldQuat);
			setJointChannelPos(rootJoint->getName(),frame,oldPos);
		}

	}	
	return true;
}

SkJoint* MeCtMotionRecorder::findRootJoint( SmartBody::SBSkeleton* skel )
{
	SkJoint* rootJoint = skel->root();
	bool bStop = false;
	while (!bStop)
	{
		if (rootJoint->num_children() == 0)
			return rootJoint;
		SkJoint* child = rootJoint->child(0);
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
			rootJoint = skel->root()->child(0);
			bStop = true;
		}
	}
	return rootJoint;
}

