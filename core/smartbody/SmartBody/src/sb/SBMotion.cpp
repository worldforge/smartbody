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


#include "SBMotion.h"
#include <sb/SBScene.h>
#include <sr/sr_euler.h>
#include <sb/SBSkeleton.h>
#include <sb/SBJoint.h>
#include <sb/SBJointMap.h>
#include <sb/SBEvent.h>
#include <sb/SBJointMapManager.h>
#include <sb/SBAssetManager.h>
#include "SBUtilities.h"
#include <controllers/me_ct_jacobian_IK.hpp>
#include <controllers/me_ct_ccd_IK.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <sbm/sbm_constants.h>
#ifdef WIN_BUILD
#pragma warning(push)
#pragma warning(disable:4244)
#endif

#ifdef WIN_BUILD
#pragma warning(pop)
#endif
#include <fstream>


namespace SmartBody {

FootStepRecord::FootStepRecord() = default;

FootStepRecord::~FootStepRecord() = default;

FootStepRecord& FootStepRecord::operator=( const FootStepRecord& rt ) = default;

void FootStepRecord::updateJointAveragePosition( SBSkeleton* skel, SBMotion* motion )
{
	int steps = 0;
	posVec.resize(jointNames.size());
	for (auto & i : posVec) i = SrVec();

	float frameRate = (float)motion->getFrameRate();
	motion->connect(skel);
	//for (float t = startTime; t <= endTime; t += frameRate)
	float t = startTime;
	{
		motion->apply(t);
		skel->update_global_matrices();
		for (unsigned int k=0;k<jointNames.size();k++)
		{
			SBJoint* joint = skel->getJointByName(jointNames[k]);
			if (joint)
			{
				posVec[k] += joint->gmat().get_translation();				
			}
		}		
		steps++;
	}
	for (auto & i : posVec)
		i /= (float)steps;
	motion->disconnect();			
}


JointTrajectory::JointTrajectory() = default;

JointTrajectory::~JointTrajectory() = default;

JointTrajectory& JointTrajectory::operator=( const JointTrajectory& rt ) = default;

SBMotion::SBMotion() : SkMotion()
{
	_motionFile = "";
	alignIndex = 0; 
	transformDepth = 0;
	_motionType = Unknown;
	_scale = 1.f;
	_offsetMotion = nullptr;
	_offsetParent = nullptr;
	_maxFrameValue = 0.0;
}

SBMotion::SBMotion(const SBMotion& motion)
{
	//...
	alignIndex = 0;
	transformDepth = 0;
	_motionType = Unknown;
	_scale = 1.f;
	_offsetMotion = nullptr;
	_offsetParent = nullptr;

	_gestureSpeed = 0.0;
	_gestureDistance = 0.0;
	_startLocation = SrVec(0, 0, 0);
	_holdLocation = SrVec(0, 0, 0);

}

void SBMotion::setMotion(const SBMotion& motion)
{

	// copy everything here
	_postsize = motion.posture_size();
	//_skeleton = motion.connected_skeleton();
	_floatbuffer = motion.connected_buffer();
	_filename = motion.filename();
	_frames = motion.data_frames();
	_channels = motion.copy_channels();
	_last_apply_frame = motion.last_apply_frame();
	transformDepth = motion.getTransformDepth();
	_motionFile = motion.getMotionFileName();
	_emptyString = "";
	_motionSkeleton = "";
	alignIndex = motion.getAlignIndex();
	_similarPoses = motion.getSimilarPoses();
	_scale = motion.getScale();
	_motionType = motion.getMotionType();
}

SBMotion::SBMotion(std::string file) : SkMotion()
{
	_motionFile = file;
	alignIndex = 0;
	transformDepth = 0;
	_motionType = Unknown;
	_scale = 1.f;
	_offsetMotion = nullptr;
	_offsetParent = nullptr;
}

SBMotion::~SBMotion() = default;

void SBMotion::setMotionType(MotionType type)
{
	_motionType = type;
}

const std::string& SBMotion::getMotionFileName() const
{
	return filename();
}

int SBMotion::getNumFrames()
{
	return frames();
}

float SBMotion::getScale() const
{
	return _scale;
}

std::vector<float> SBMotion::getFrameData(int frameId)
{
	std::vector<float> ret;
	for (int i = 0; i < getFrameSize(); i++)
		ret.emplace_back(posture(frameId)[i]);
	return ret;
}

int SBMotion::getFrameSize()
{
	return posture_size();
}

int SBMotion::getNumChannels()
{
	return channels().size();
}

std::vector<std::string> SBMotion::getChannels()
{
	std::vector<std::string> ret;
	for (int i = 0; i < channels().size(); i++)
	{
		std::string chanName = channels().name(i);
		//if (channels()[i].joint)
		//	chanName = channels()[i].joint->name().c_str();
		int	chanType = channels()[i].type;
		std::string chanTypeString;
		switch (chanType)
		{
			case 0:
				chanTypeString = "XPos";
				break;
			case 1:	
				chanTypeString = "YPos";
				break;
			case 2:
				chanTypeString = "ZPos";
				break;
			case 6:
				chanTypeString = "Quat";
				break;
			default:
				chanTypeString = "Others";
		}
		std::string name = chanName + "_" + chanTypeString;
		ret.emplace_back(name);
	}
	return ret;
}

void SBMotion::addChannel(const std::string& channelName, const std::string& type)
{
	SkChannel::Type channelType = SkChannel::XPos;
	if (type == "XPos")
	{
		channelType = SkChannel::XPos;
	}
	else if (type == "YPos")
	{
		channelType = SkChannel::YPos;
	}
	else if (type == "ZPos")
	{
		channelType = SkChannel::ZPos;
	}
	else if (type == "Quat")
	{
		channelType = SkChannel::Quat;
	}
	else 
	{
		SmartBody::util::log("Channel type %s not valid, must be one of: XPos, YPos, ZPos, Quat. Channel %s not added.", type.c_str(), channelName.c_str());
		return;
	}

	int index = _channels.search(channelName, channelType);
	if (index != -1)
	{
		SmartBody::util::log("Channel %s of type %s already present in motion, duplicate channel not added.", channelName.c_str(), type.c_str());
		return;
	}

	_channels.add(channelName, channelType);
}

void SBMotion::addFrame(float frameTime, const std::vector<float>& frameData)
{
	// make sure that the frame data corresponds to the channel data
	int channelSize = _channels.floats();
	if (channelSize != frameData.size())
	{
		SmartBody::util::log("Motion %s needs %d values, only have %d values, data not added.", this->getName().c_str(), channelSize, frameData.size());
		return;
	}

	double duration = this->getDuration();
	if (duration <= frameTime)
	{
		int postureSize = _channels.floats();
		int frameSize = _frames.size();

		// don't add the frame if it isn't at a later time than the last frame
		if (frameSize > 0 && fabs(_frames[frameSize - 1].keytime - frameTime) < .0001)
		{
			SmartBody::util::log("Frame %d uses the same time as last frame %f, ignoring...", frameSize, frameTime);
			return;
		}

		_frames.resize(_frames.size() + 1);
		_frames[frameSize].keytime = frameTime;
		_frames[frameSize].posture = (float*) malloc ( sizeof(float)*postureSize );
		for (int p = 0; p < postureSize; p++)
		{
			_frames[frameSize].posture[p] = frameData[p];
		}
	}
	else
	{
		SmartBody::util::log("Frame must be added in time-order. Motion %s has duration %f, frame is at time %f. Frame not added.", this->getName().c_str(), duration, frameTime);
		return;
	}

}

void SBMotion::addKeyFrameChannel(const std::string& channelName, const std::string& channelType, float keyTime, float value)
{
	srLinearCurve* curve = nullptr;
	std::string key = channelName + "/" + channelType;
	auto iter = _channelFrameValues.find(key);
	if (iter == _channelFrameValues.end())
	{
		curve = new srLinearCurve();
		_channelFrameValues.insert(std::pair<std::string, srLinearCurve*>(key, curve));
	}
	else
	{
		curve = (*iter).second.get();
	}
	curve->insert(keyTime, value);
	if (keyTime > _maxFrameValue)
		_maxFrameValue = keyTime;
}

void SBMotion::addKeyFrameQuat(const std::string& channelName, const std::string& channelType, float keyTime, const SrQuat& value)
{
	rotationCurve* curve = nullptr;
	std::string key = channelName;
	auto iter = _quatFrameValues.find(key);
	if (iter == _quatFrameValues.end())
	{
		curve = new rotationCurve();
		curve->x = std::make_unique<srLinearCurve>();
		curve->y = std::make_unique<srLinearCurve>();
		curve->z = std::make_unique<srLinearCurve>();
		_quatFrameValues.insert(std::pair<std::string, rotationCurve*>(key, curve));
	}
	else
	{
		curve = (*iter).second.get();
	}
	// convert the quaternion to xyz rotations
	SrMat mat;
	value.get_mat(mat);
	SrVec euler;
	sr_euler_angles_xyz(mat, euler.x, euler.y, euler.z);
	curve->x->insert(keyTime, euler.x);
	curve->y->insert(keyTime, euler.y);
	curve->z->insert(keyTime, euler.z);
	if (keyTime > _maxFrameValue)
		_maxFrameValue = keyTime;

}

void SBMotion::bakeFrames(float fps)
{
	if (_channelFrameValues.size() == 0 &&
		_quatFrameValues.size() == 0)
	{
		SmartBody::util::log("No motion data to bake.");
		return;
	} 

	if (fabs(fps) < .0001)
	{
		SmartBody::util::log("FPS set to zero, resetting to 30.");
		fps = 30.0;
	}
	float step = float(1.0) / fps;
	if (step < .033f)
		step = .033f;
	int numChannels = _channels.size();
	for (float curTime = 0.0; curTime <= _maxFrameValue; curTime += step)
	{
		std::vector<float> data;
		for (int c = 0; c < numChannels; c++)
		{
			const std::string channelName = _channels.name(c);
			SkChannel& channel = _channels.get(c);
			SkChannel::Type channelType = _channels.type(c);
			std::string keyName = "";
			if (channelType == SkChannel::XPos)
			{
				keyName = channelName + "/" + "XPos";
			}
			else if (channelType == SkChannel::YPos)
			{
				keyName = channelName + "/" + "YPos";
			}
			else if (channelType == SkChannel::ZPos)
			{
				keyName = channelName + "/" + "ZPos";
			}
			else if (channelType == SkChannel::Quat)
			{
				keyName = channelName;
			}
			else
			{
				keyName = channelName;
			}

			if (channelType == SkChannel::XPos ||
				channelType == SkChannel::YPos ||
				channelType == SkChannel::ZPos)
			{
				auto iter = _channelFrameValues.find(keyName);
				if (iter == _channelFrameValues.end())
				{
					data.emplace_back(0.0);
					continue;
				}

				auto& curve = (*iter).second;
				double value = curve->evaluate(curTime);
				data.emplace_back((float) value);
			}
			else if (channelType == SkChannel::Quat)
			{
				auto iter = _quatFrameValues.find(keyName);
				if (iter == _quatFrameValues.end())
				{
					data.emplace_back(1.0);
					data.emplace_back(0.0);
					data.emplace_back(0.0);
					data.emplace_back(0.0);
					continue;
				}

				auto& curve = (*iter).second;
				// need to slerp between quaternions
				SrVec euler;
				euler.x = curve->x->evaluate(curTime);
				euler.y = curve->y->evaluate(curTime);
				euler.z = curve->z->evaluate(curTime);

				SrMat mat;
				sr_euler_mat_xyz(mat, euler.x, euler.y, euler.z);
				SrQuat quat(mat);
				data.emplace_back(quat.w);
				data.emplace_back(quat.x);
				data.emplace_back(quat.y);
				data.emplace_back(quat.z);
			}
		}
		this->addFrame(curTime, data);
	}

	this->setSyncPoint("start", 0.0);
	this->setSyncPoint("end", this->duration());
	if (this->duration() > .5)
	{
		this->setSyncPoint("ready", .25);
		this->setSyncPoint("relax", this->duration() - .25);
	}
	else
	{
		this->setSyncPoint("ready", 0.0);
		this->setSyncPoint("relax", this->duration());
	}
}

void SBMotion::checkSkeleton(std::string skel)
{
	 
	int chanSize;
	SkChannel chan;

	SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(getName());
	if (!motion)
	{
		SmartBody::util::log("checkSkeleton ERR: Motion %s NOT EXIST!", getName().c_str());
		return;
	}

	auto skSkel =  SmartBody::SBScene::getScene()->createSkeleton(skel);
	//load_skeleton(skel.c_str(), mcu.me_paths, SmartBody::SBScene::getScene()->getAssetManager()->getGlobalSkeletonScale());
	if (skSkel)
	{
		motion->connect(skSkel.get());	// connect and check for the joints
		SkChannelArray& mChanArray = motion->channels();
		int mChanSize = mChanArray.size();
		SkChannelArray& skelChanArray = skSkel->channels();
		int skelChanSize = skelChanArray.size();
		chanSize = mChanSize;
		SmartBody::util::log("Channels in skeleton %s's channel matching motion %s's channel are preceeded with '+'", skel.c_str(), getName().c_str());
		SmartBody::util::log("motion %s's Channel Info:", getName().c_str());
		SmartBody::util::log("Channel Size: %d", chanSize);
		for (int i = 0; i < chanSize; i++)
		{				
			std::stringstream outputInfo;
			chan = mChanArray[i];
			std::string jointName = chan.joint->jointName().c_str();
			int	chanType = chan.type;
			std::string chanTypeString;
			switch (chanType)
			{
				case 0:
					chanTypeString = "XPos";
					break;
				case 1:	
					chanTypeString = "YPos";
					break;
				case 2:
					chanTypeString = "ZPos";
					break;
				case 6:
					chanTypeString = "Quat";
					break;
				default:
					chanTypeString = "Others";
			}
			int pos;
			pos = skelChanArray.linear_search(chan.joint->jointName(), chan.type);
			if (pos != -1)
				outputInfo << "+ ";
			if (pos == -1)	
				outputInfo << "  ";
			outputInfo << i << ": " << jointName.c_str() << " (" << chanTypeString << ")";
			SmartBody::util::log("%s", outputInfo.str().c_str());
		}
	}
	else
		SmartBody::util::log("Skeleton %s does NOT exist!", skel.c_str());
}

int SBMotion::connect(SBSkeleton* skel)
{
	return SkMotion::connect(skel);
}

void SBMotion::disconnect()
{
	SkMotion::disconnect();
}


void SBMotion::alignToBegin(int numFrames)
{
	alignToSide(numFrames, 1);
}

void SBMotion::alignToEnd(int numFrames)
{
	alignToSide(numFrames, 0);
}

int SBMotion::getAlignIndex() const
{
	return alignIndex;
}

void SBMotion::recoverAlign()
{
	if (alignIndex > 0)
		alignToEnd(alignIndex);
	else if (alignIndex < 0)
		alignToBegin(-alignIndex);
}

void SBMotion::alignToSide(int numFrames, int direction)
{
	if (numFrames >= getNumFrames())
	{
		SmartBody::util::log("SBMotion::alignToSide WARNING: number of frames %d exceed the motion cycle.", numFrames);
		return;
	}

	if (direction != 0 && direction != 1)
	{
		SmartBody::util::log("SBMotion::alignToSide WARNING: direction %d not valid.", direction);
		return;
	}

	if (getNumFrames() == 1)
	{
		SmartBody::util::log("SBMotion::alignToSide WARNING: motion %s only has one frame, no need to align.", this->getName().c_str());
		return;
	}

	// register previous motion
	registerAnimation();
	
	// handle base joint
	// get adjust matrix
	SrMat adjustBaseMat;
	if (isRegistered())
	{
		int startFrameId = 0;
		int endFrameId = getNumFrames() - 1;

		SrMat matStart;
		_frameOrientation[startFrameId].get_mat(matStart);
		matStart.setl4(_frameOffset[startFrameId]);

		SrMat matEnd;
		_frameOrientation[endFrameId].get_mat(matEnd);
		matEnd.setl4(_frameOffset[endFrameId]);

		if (direction == 0)
		{
			SrMat mat0;
			_frameOrientation[0].get_mat(mat0);
			mat0.setl4(_frameOffset[0]);
			SrMat mat1;
			_frameOrientation[1].get_mat(mat1);
			mat1.setl4(_frameOffset[1]);
			SrMat mat01 = mat0.inverse() * mat1;

			adjustBaseMat = mat01 * (matStart.inverse() * matEnd);
		}
		else if (direction == 1)
		{
			SrMat mat1;
			_frameOrientation[getNumFrames() - 1].get_mat(mat1);
			mat1.setl4(_frameOffset[getNumFrames() - 1]);
			SrMat mat0;
			_frameOrientation[getNumFrames() - 2].get_mat(mat0);
			mat0.setl4(_frameOffset[getNumFrames() - 2]);
			SrMat mat10 = mat1.inverse() * mat0;

			adjustBaseMat = mat10 * (matEnd.inverse() * matStart);
		}
	}

	// calculate new matrix vector
	std::vector<SrMat> copyMatVec;
	copyMatVec.resize(getNumFrames());
	for (int i = 0; i < getNumFrames(); i++)
	{
		int origMotionId = 0;
		bool cycle = false;
		if (direction == 0)
		{
			if (i < (getNumFrames() - numFrames))
				origMotionId = i + numFrames;
			else
			{
				origMotionId = i - (getNumFrames() - numFrames);
				cycle = true;
			}
		}
		else if (direction == 1)
		{
			if (i < numFrames)
			{
				origMotionId = i + (getNumFrames() - numFrames);
				cycle = true;
			}
			else
				origMotionId = i - numFrames;
		}

		_frameOrientation[origMotionId].get_mat(copyMatVec[i]);
		copyMatVec[i].setl4(_frameOffset[origMotionId]);
		if (cycle)
		{
			SrMat temp = copyMatVec[i] * adjustBaseMat;
			copyMatVec[i] = temp;
		}
	}

	// copy it back
	for (int i = 0; i < getNumFrames(); i++)
	{
		/*
		gwiz::matrix_t actualMatrix;
		for (int r = 0; r < 4; r++)
			for (int c = 0; c < 4; c++)
			{
				actualMatrix.set(r, c, copyMatVec[i].get(r, c));
			}
		gwiz::quat_t retQuat = actualMatrix.quat();

		_frameOrientation[i] = SrQuat((float)retQuat.w(), (float)retQuat.x(), (float)retQuat.y(), (float)retQuat.z());
		*/
		_frameOrientation[i] = SrQuat(copyMatVec[i]);
		_frameOffset[i] = copyMatVec[i].get_translation();
	}

	// create a new motion
	SkChannelArray& mchan_arr = this->channels();
	SkMotion* copyMotion = new SmartBody::SBMotion();
	srSynchPoints sp(synch_points);
	copyMotion->synch_points = sp;
	copyMotion->init(mchan_arr);

	// insert frames into new motion
	for (int i = 0; i < getNumFrames(); i++)
	{
		int origMotionId = 0;
		if (direction == 0)
		{
			if (i < (getNumFrames() - numFrames))
				origMotionId = i + numFrames;
			else
				origMotionId = i - (getNumFrames() - numFrames);
		}
		else if (direction == 1)
		{
			if (i < numFrames)
				origMotionId = i + (getNumFrames() - numFrames);
			else
				origMotionId = i - numFrames;
		}
		copyMotion->insert_frame(i, this->keytime(i));
		float *ref_p = this->posture(origMotionId);
		float *new_p = copyMotion->posture(i);
		for (int k = 0; k < mchan_arr.size(); k++)
		{
			SkChannel& chan = mchan_arr[k];
			int index = mchan_arr.float_position(k);			
			for (int n = 0; n < chan.size(); n++)
				new_p[index + n] = ref_p[index + n];
		}
	}

	// assign back to the old motion
	for (int i = 0; i < getNumFrames(); i++)
	{
		float* orig_p = this->posture(i);
		float* copy_p = copyMotion->posture(i);
		for (int k = 0; k < mchan_arr.size(); k++)
		{
			SkChannel& chan = mchan_arr[k];
			int index = mchan_arr.float_position(k);			
			for (int n = 0; n < chan.size(); n++)
				orig_p[index + n] = copy_p[index + n];
		}
	}

	// unregister animation
	unregisterAnimation();

	delete copyMotion;
	copyMotion = nullptr;

	if (direction == 0)
		alignIndex -= numFrames;	
	else if (direction == 1)
		alignIndex += numFrames;
}

void SBMotion::addSimilarPose(const std::string& motionName)
{
	for (auto & _similarPose : _similarPoses)
	{
		if (_similarPose == motionName)
		{
			SmartBody::util::log("Pose named '%s' already similar to motion %s.", motionName.c_str(), this->getName().c_str());
			return;
		}
	}

	_similarPoses.emplace_back(motionName);
}

void SBMotion::removeSimilarPose(const std::string& motionName)
{
	for (auto iter = _similarPoses.begin();
		 iter != _similarPoses.end();
		 iter++)
	{
		if ((*iter) == motionName)
		{
			_similarPoses.erase(iter);
			return;
		}
	}
	SmartBody::util::log("Could not find similar pose '%s' for motion '%s'.", motionName.c_str(), this->getName().c_str());
}

std::vector<std::string> SBMotion::getSimilarPoses() const
{
	std::vector<std::string> poses;
	for (const auto & _similarPose : _similarPoses)
	{
		poses.emplace_back(_similarPose);
	}
	return poses;
}


SBMotion* SBMotion::duplicateCycle(int num, std::string newName)
{
	 
	registerAnimation();

	num++;

	// create a new motion
	SkChannelArray& mchan_arr = this->channels();
	auto copyMotion = std::make_unique<SmartBody::SBMotion>();
	std::string copyMotionName = newName;
	if (newName.empty())
	{
		std::stringstream ss;
		ss << this->getName() << "_duplicate" << (num - 1);
		copyMotionName = ss.str();
	}

	copyMotion->setName(copyMotionName);

	srSynchPoints sp(synch_points);
	copyMotion->synch_points = sp;
	copyMotion->init(mchan_arr);

	std::vector<SrVec>& frameOffset = getFrameOffset();
	std::vector<SrQuat>& frameOrientation = getFrameOrientation();
	SrMat adjustBaseMat;
	if (isRegistered())
	{
		int startFrameId = 0;
		int endFrameId = getNumFrames() - 1;

		SrMat matStart;
		frameOrientation[startFrameId].get_mat(matStart);
		matStart.setl4(frameOffset[startFrameId]);

		SrMat matEnd;
		frameOrientation[endFrameId].get_mat(matEnd);
		matEnd.setl4(frameOffset[endFrameId]);

		SrMat mat0;
		frameOrientation[0].get_mat(mat0);
		mat0.setl4(frameOffset[0]);
		SrMat mat1;
		frameOrientation[1].get_mat(mat1);
		mat1.setl4(frameOffset[1]);
		SrMat mat01 = mat0.inverse() * mat1;

		adjustBaseMat = mat01 * (matStart.inverse() * matEnd);
	}

	std::vector<SrQuat> copyFrameOrientation;
	std::vector<SrVec> copyFrameOffset;
	copyFrameOrientation.resize(getNumFrames() * num);
	copyFrameOffset.resize(getNumFrames() * num);
	for (int cycleId = 0; cycleId < num; cycleId++)
	{
		// handle base joints
		SrMat adjustMats;
		SrVec adjustVec;
		SrQuat adjustQuat;
		for (int i = 0; i < cycleId; i++)
		{
			SrMat temp = adjustMats * adjustBaseMat;
			adjustMats = temp;
		}

		for (int i = 0; i < getNumFrames(); i++)
		{
			// copy frame data without base joints
			copyMotion->insert_frame(i + cycleId * getNumFrames(), this->keytime(i) + float(cycleId * getDuration()));
			float* orig_p = posture(i);
			float* copy_p = copyMotion->posture(i + cycleId * getNumFrames());
			for (int k = 0; k < mchan_arr.size(); k++)
			{
				SkChannel& chan = mchan_arr[k];
				int index = mchan_arr.float_position(k);			
				for (int n = 0; n < chan.size(); n++)
					copy_p[index + n] = orig_p[index + n];
			}
			SrMat curMat;
			frameOrientation[i].get_mat(curMat);
			curMat.setl4(frameOffset[i]);
			SrMat newMat = curMat * adjustMats;
			copyFrameOffset[i + cycleId * getNumFrames()] = newMat.get_translation();
			copyFrameOrientation[i + cycleId * getNumFrames()] = SrQuat(newMat);
		}	
	}
	unregisterAnimation();
	copyMotion->setFrameOffset(copyFrameOffset);
	copyMotion->setFrameOrientation(copyFrameOrientation);
	copyMotion->unregisterAnimation();
	copyMotion->setTransformDepth(getTransformDepth()+1);
	auto motionPtr = copyMotion.get();

	if (SmartBody::SBScene::getScene()->getAssetManager()->addMotion(std::move(copyMotion))) {
		return motionPtr;
	} else {
		return nullptr;
	}
}

void SBMotion::removeMotionChannels(std::vector<std::string> channelNames)
{
	_channels.startChannelNameChange();
	for (auto & channelName : channelNames)
	{
		std::string newChanName = "deletedChan" + channelName;
		_channels.changeChannelName(channelName, newChanName);
	}
	_channels.rebuild_hash_table();
}



SBMotion* SBMotion::removeChannels(std::string motionName, bool isTranslation, std::vector<std::string> jointNames)
{
	SBMotion* newMotion = new SBMotion();
	newMotion->setName(motionName);

	SkChannelArray& ch = channels();
	int numFrames = frames();
	SmartBody::SBMotion* originalMotion = dynamic_cast<SmartBody::SBMotion*>(this);
	newMotion->setMotionSkeletonName(originalMotion->getMotionSkeletonName());
	std::string filebase = boost::filesystem::basename(this->getFullFilePath());

	newMotion->filename(filebase.c_str());

	std::set<std::string> toRemoveJoints;
	for (auto & jointName : jointNames)
	{
		toRemoveJoints.insert(jointName);
	}

	// preserve only the desired channels
	SkChannelArray ch2;
	for (int c = 0; c < ch.size(); c++)
	{
		SkChannel& channel = ch.get(c);
		SkJoint* curJoint = channel.joint;
		SkChannel::Type cType = channel.type;
		if (isTranslation) // removing translation channels
		{
			if (cType == SkChannel::XPos || cType == SkChannel::YPos || cType == SkChannel::ZPos)
			{
				auto iter = toRemoveJoints.find(curJoint->getName());
				if (iter != toRemoveJoints.end())
				{
					continue;
				}
			}
		}
		else // removing rotation channels
		{
			if (cType == SkChannel::Quat)
			{
				auto iter = toRemoveJoints.find(curJoint->getName());
				if (iter != toRemoveJoints.end())
				{
					continue;
				}
			}
		}

		ch2.add(curJoint->getName(), cType);
	}

	newMotion->init(ch2);

	float* data = new float[ch2.count_floats()];
	for (size_t f = 0; f < this->_frames.size(); f++)
	{
		float* curFrame = this->_frames[f].posture;
		int curPos = 0;
		for (int c = 0; c < ch2.size(); c++)
		{
			SkChannel& channel = ch2.get(c);
			SkChannel::Type channelType = ch2.type(c);
			int channelIndex = _channels.search(ch2.name(c), channelType);
			int dataIndex = _channels.float_position(channelIndex);
			if (channelType == SkChannel::XPos || channelType == SkChannel::YPos || channelType == SkChannel::ZPos)
			{
				data[curPos] = curFrame[dataIndex];
			}
			else if (channelType == SkChannel::Quat)
			{
				for (int p = 0; p < 4; p++)
				{
					data[curPos + p] = curFrame[dataIndex + p];
				}
			}
			else
			{
				int num = channel.size();
				for (int p = 0; p < num; p++)
				{
					data[curPos + p] = curFrame[dataIndex + p];
				}
			}
			curPos += channel.size();
		}
		float keytime = this->_frames[f].keytime;
		newMotion->insert_frame(f, keytime);
		float *new_p = newMotion->posture(f);
		memcpy(new_p, data, sizeof(float)*posture_size());
	}
	newMotion->setSyncPoint("start", this->getTimeStart());
	newMotion->setSyncPoint("ready", this->getTimeReady());
	newMotion->setSyncPoint("stroke_start", this->getTimeStrokeStart());
	newMotion->setSyncPoint("stroke", this->getTimeStroke());
	newMotion->setSyncPoint("stroke_stop", this->getTimeStrokeEnd());
	newMotion->setSyncPoint("relax", this->getTimeRelax());
	newMotion->setSyncPoint("stop", this->getTimeStop());

	delete [] data;

	return newMotion;
}



void SBMotion::pertainMotionChannelsByEndJoints( std::string skelName, std::vector<std::string>& endJoints )
{
	auto srcSkeleton = SmartBody::SBScene::getScene()->getSkeleton(skelName);
	if (!srcSkeleton)
	{
		SmartBody::util::log("No skeleton named %s found. Can not match joint names and descendents", skelName.c_str());
		return;
	}
	std::vector<std::string> pertainJoints;
	for (auto & endJoint : endJoints)
	{
		SmartBody::SBJoint* eJoint = srcSkeleton->getJointByName(endJoint);
		if (eJoint)
		{
			std::vector<SBJoint*> descendents = eJoint->getDescendants();
			for (auto & descendent : descendents)
			{
				pertainJoints.emplace_back(descendent->getName());
			}
			//pertainJoints.emplace_back(eJoint->getName());
		}		
	}
	std::vector<std::string> removeJoints;
	for (unsigned int i = 0; i < srcSkeleton->getJointNames().size(); ++i)
	{
		std::string jname = srcSkeleton->getJointNames()[i];
		bool shouldRemove = true;
		for (auto & pertainJoint : pertainJoints)
		{
			if (pertainJoint == jname)
			{
				shouldRemove = false;
				break;
			}
		}
		if (shouldRemove)
			removeJoints.emplace_back(jname);
	}
	this->removeMotionChannels(removeJoints);
}


void SBMotion::removeMotionChannelsByEndJoints(std::string skelName, std::vector<std::string>& endJoints)
{
	auto srcSkeleton = SmartBody::SBScene::getScene()->getSkeleton(skelName);
	if (!srcSkeleton)
	{
		SmartBody::util::log("No skeleton named %s found. Can not match joint names and descendents", skelName.c_str());
		return;
	}
	std::vector<std::string> removeJoints;
	for (auto & endJoint : endJoints)
	{
		SmartBody::SBJoint* eJoint = srcSkeleton->getJointByName(endJoint);
		if (eJoint)
		{
			std::vector<SBJoint*> descendents = eJoint->getDescendants();
			for (auto & descendent : descendents)
			{
				removeJoints.emplace_back(descendent->getName());
			}
			//removeJoints.emplace_back(eJoint->getName());
		}		
	}
	this->removeMotionChannels(removeJoints);
}

SBMotion* SBMotion::retarget( const std::string& name, std::string srcSkeletonName, std::string dstSkeletonName, std::vector<std::string>& endJoints, std::vector<std::string>& relativeJoints, std::map<std::string, SrVec>& offsetJointMap)
{
	 
	auto srcSkeleton = SmartBody::SBScene::getScene()->getSkeleton(srcSkeletonName);
	if (!srcSkeleton)
	{
		SmartBody::util::log("No retarget source skeleton named %s found.", srcSkeletonName.c_str());
		return nullptr;
	}
	auto dstSkeleton = SmartBody::SBScene::getScene()->getSkeleton(dstSkeletonName);
	if (!dstSkeleton)
	{
		SmartBody::util::log("No retarget destination skeleton named %s found.", dstSkeletonName.c_str());
		return nullptr;
	}
	srcSkeleton->clearJointValues();
	dstSkeleton->clearJointValues();
	if (!srcSkeleton || !dstSkeleton)
	{
		SmartBody::util::log("Skeleton %s or %s not found. Retargeted motion %s not built.",srcSkeletonName.c_str(),dstSkeletonName.c_str(),name.c_str());
		return nullptr;
	}
	
	//SkMotion* motion = buildRetargetMotion(srcSkeleton,dstSkeleton, endJoints, relativeJoints, offsetJointMap);
	SkMotion* motion = buildRetargetMotionV2(srcSkeleton.get(),dstSkeleton.get(), endJoints, relativeJoints, offsetJointMap);
	
	
	auto* sbmotion = dynamic_cast<SBMotion*>(motion);
	if (sbmotion)
	{
		std::string motionName;
		if (name.empty())
		{
			motionName = sbmotion->getName();
			if (motionName.empty())
				motionName = getName() + "_retarget";
		}
		else
			motionName = name;
		sbmotion->setName(motionName);
		//mcu.motion_map.insert(std::pair<std::string, SkMotion*>(motionName, motion));
		if (SmartBody::SBScene::getScene()->getAssetManager()->addMotion(std::unique_ptr<SBMotion>(sbmotion))) {
			return sbmotion;
		} else {
			return nullptr;
		}
	}
	return sbmotion;
}

SBMotion* SBMotion::buildConstraintMotion( SBSkeleton* sourceSk, SBSkeleton* targetSk, SBMotion* targetMotion, std::vector<std::string>& endJoints, std::vector<std::string>& endJointRoots )
{
	SkChannelArray& mchan_arr = this->channels();
	SBSkeleton interSk(*targetSk); // copy for an intermediate skeleton
	SBSkeleton tempSrcSk(*sourceSk);

	tempSrcSk.invalidate_global_matrices();
	tempSrcSk.update_global_matrices();
	interSk.invalidate_global_matrices();
	interSk.update_global_matrices();

	auto* rootJoint = dynamic_cast<SBJoint*>(interSk.root());
	if (interSk.getJointByMappedName("base"))
		rootJoint = interSk.getJointByMappedName("base");
	auto* constraintMotion = dynamic_cast<SBMotion*>(targetMotion->copyMotion()); // copy the motion first
	MeCtIKTreeScenario ikScenario;
	std::vector<std::string> stopJoints;
	ikScenario.buildIKTreeFromJointRoot(rootJoint,stopJoints);	
	MeCtJacobianIK ikJacobian;
	float sceneScale = (float)1.f/SBScene::getScene()->getScale();	
	float heightRatio = (interSk.getBaseHeight("base")/tempSrcSk.getBaseHeight("base"));//*0.99f;

	ikJacobian.maxOffset = 0.05f*sceneScale;
	ikJacobian.dampJ = 1.5f*sceneScale;

	std::map<std::string, float> jointScaleMap;
	ConstraintMap consMap;
	ConstraintMap noRotConstraint;
	for (unsigned int i=0;i<endJoints.size();i++)
	{
		std::string jname = endJoints[i];
		std::string jrootName;
		if (endJointRoots.size() > i)
			jrootName = endJointRoots[i];

		SBJoint* srcJoint = tempSrcSk.getJointByName(jname);
		SBJoint* tgtJoint = interSk.getJointByName(jname);
		if (srcJoint && tgtJoint) // a valid effector
		{
			auto constraint = std::make_unique<EffectorConstantConstraint>();
			constraint->efffectorName = jname;
			//SBJoint* pjoint = dynamic_cast<SBJoint*>(tgtJoint->getParent()->getParent()->getParent());			
			if (interSk.getJointByName(jrootName))
			{
				constraint->rootName = jrootName;
			}
			else
			{
				SBJoint* pjoint = tgtJoint->getParent();
				while (pjoint->getParent() != rootJoint)
					pjoint = pjoint->getParent();
				constraint->rootName = pjoint->jointName();
			}			
			consMap[jname] = std::move(constraint);
		}
	}

	SrVec prevPos;
	SrVec prevOffsetPos;
	for (int iframe=0; iframe<frames(); iframe++)
	{
		int chanID = mchan_arr.search(rootJoint->jointName(),SkChannel::XPos);
		float* cur_p = constraintMotion->posture(iframe);	
		float* orig_p = this->posture(iframe);
		int index = mchan_arr.float_position(chanID);
		SrVec curPos;
		for (int k=0;k<3;k++)
		{
			cur_p[index+k] = orig_p[index+k]*heightRatio;
			curPos[k] = orig_p[index+k];
		}
		SrVec baseOffset = curPos - prevPos;
		prevPos = curPos;
		SrVec curScalePos = curPos*heightRatio;
		if (iframe > 0)
		{
			curScalePos = prevOffsetPos + baseOffset*heightRatio;
			for (int k=0;k<3;k++)
			{
				cur_p[index+k] = curScalePos[k];
			}
		}
		prevOffsetPos = curScalePos;
	}

	constraintMotion->connect(&interSk);
	this->connect(&tempSrcSk);
	bool prevConstraint = false;

	constraintMotion->apply_frame(0);
	this->apply_frame(0);
	constraintMotion->connected_skeleton()->update_global_matrices();
	this->connected_skeleton()->update_global_matrices();
	std::map<std::string, SrVec> prevPosMap;
	ConstraintMap::iterator mi;
	SBJoint* srcRoot = tempSrcSk.getJointByName(rootJoint->jointName());
	SBJoint* tgtRoot = rootJoint;
	for ( mi  = consMap.begin();
		  mi != consMap.end();
		  mi++)
	{
		auto* con = dynamic_cast<EffectorConstantConstraint*>(mi->second.get());
		SBJoint* srcJoint = tempSrcSk.getJointByName(con->efffectorName);
		SBJoint* tgtJoint = interSk.getJointByName(con->efffectorName);
		if (tgtJoint)
		{
			SrVec offset = srcJoint->getMatrixGlobal().get_translation() - srcRoot->getMatrixGlobal().get_translation();
			con->targetPos = tgtRoot->getMatrixGlobal().get_translation() + offset*heightRatio;//tgtJoint->getMatrixGlobal().get_translation(); // initialize the frame 
		}					
		if (srcJoint)
		{
			prevPosMap[con->efffectorName] = srcJoint->getMatrixGlobal().get_translation();
		}
	}



	for (int iframe=0; iframe<frames(); iframe++)
	{			
		constraintMotion->apply_frame(iframe);
		this->apply_frame(iframe);
		constraintMotion->connected_skeleton()->update_global_matrices();
		this->connected_skeleton()->update_global_matrices();
		float* cur_p = constraintMotion->posture(iframe);		
		if (!prevConstraint)
		{
			ikScenario.setTreeNodeQuat(&interSk,QUAT_INIT);
			prevConstraint = true;
		}		
		ikScenario.copyTreeNodeQuat(QUAT_INIT,QUAT_CUR);
		ikScenario.setTreeNodeQuat(&interSk,QUAT_REF);
		// update root transformation
		if (rootJoint->pos())
		{
			SkJointPos* jpos = rootJoint->pos();
			ikScenario.ikTreeRootPos = SrVec(jpos->value(0),jpos->value(1),jpos->value(2));
		}		
		if (rootJoint->getParent())
			ikScenario.ikGlobalMat = rootJoint->getParent()->gmat();

		// update joint constraint
		ConstraintMap::iterator mi;
		for ( mi  = consMap.begin();
			  mi != consMap.end();
			  mi++)
		{
			EffectorConstantConstraint* con = dynamic_cast<EffectorConstantConstraint*>(mi->second.get());
			SBJoint* srcJoint = tempSrcSk.getJointByName(con->efffectorName);
			if (srcJoint)
			{
				//con->targetPos = srcJoint->getMatrixGlobal().get_translation();
				SrVec offset = srcJoint->getMatrixGlobal().get_translation() - prevPosMap[con->efffectorName];
				con->targetPos += offset*heightRatio;
				prevPosMap[con->efffectorName] = srcJoint->getMatrixGlobal().get_translation();
			}			
		}

		// update IK for each constraint at this frame	
		ikScenario.ikPosEffectors = &consMap;
		ikScenario.ikRotEffectors = &noRotConstraint;
		for (int k=0;k<20;k++)
		{
			ikJacobian.update(&ikScenario);
			ikScenario.copyTreeNodeQuat(QUAT_CUR,QUAT_INIT);	
		}
		for (auto ikNode : ikScenario.ikTreeNodes)
		{
				SBJoint* ikJoint = interSk.getJointByName(ikNode->getNodeName());
			int chanID = mchan_arr.search(ikNode->getNodeName(),SkChannel::Quat);
			SrQuat& nq = ikNode->getQuat(QUAT_CUR);
			SrQuat kq; 
			if (ikJoint) kq = ikJoint->quat()->rawValue();			
			if (chanID != -1)
			{
				int floatIdx = mchan_arr.float_position(chanID);
				//SmartBody::util::log("old quat = %f %f %f %f",cur_p[ floatIdx + 0 ],cur_p[ floatIdx + 1 ],cur_p[ floatIdx + 2 ],cur_p[ floatIdx + 3 ] );
				//SmartBody::util::log("new quat = %f %f %f %f",nq.w,nq.x,nq.y,nq.z );
				//SmartBody::util::log("skel quat = %f %f %f %f",kq.w,kq.x,kq.y,kq.z );
				cur_p[ floatIdx + 0 ] = nq.w;
				cur_p[ floatIdx + 1 ] = nq.x;
				cur_p[ floatIdx + 2 ] = nq.y;
				cur_p[ floatIdx + 3 ] = nq.z;
			}
		}				
	}
	constraintMotion->disconnect();
	this->disconnect();
	return constraintMotion;
}


SBMotion* SBMotion::autoFootSkateCleanUp( std::string name, std::string srcSkeletonName, std::string rootName, std::vector<FootStepRecord>& footStepRecords )
{
	auto origSkel = SBScene::getScene()->getSkeleton(srcSkeletonName);
	if (!origSkel) return nullptr;
	boost::intrusive_ptr<SBSkeleton> skel(new SBSkeleton(*origSkel));
	if (!skel) return nullptr;
	SBJoint* rootJoint = skel->getJointByName(rootName);
	if (!rootJoint) return nullptr;

	SBMotion* cleanMotion = dynamic_cast<SBMotion*>(this->copyMotion()); // copy the motion first	
	MeCtIKTreeScenario ikScenario;
	std::vector<std::string> stopJoints;
	ikScenario.buildIKTreeFromJointRoot(rootJoint,stopJoints);
	MeCtCCDIK ikCCD;
	MeCtJacobianIK ikJacobian;
	float sceneScale = (float)1.f/SBScene::getScene()->getScale();
	ikJacobian.maxOffset = 0.1f*sceneScale;
	ikJacobian.dampJ = 1.5f*sceneScale;
	std::map<int, std::vector<int> > frameConstraintMap;

	//ConstraintMap cons;
	std::vector<ConstraintMap> constrainMapList;
	constrainMapList.resize(footStepRecords.size());

	SkChannelArray& mchan_arr = cleanMotion->channels();
	ConstraintMap noRotConstraint;
	for (auto & rec : footStepRecords)
	{
			if (rec.jointNames.size() == 1)
		{
			SBJoint* joint = skel->getJointByName(rec.jointNames[0]);
			if (joint && joint->child(0))
				rec.jointNames.emplace_back(joint->child(0)->jointName());
// 			if (joint && joint->child(0) && joint->child(0)->child(0))
// 				rec.jointNames.emplace_back(joint->child(0)->child(0)->name());
		}

	}

	cleanMotion->connect(skel.get());
	for (unsigned int i=0;i<footStepRecords.size();i++)
	{
		FootStepRecord& rec = footStepRecords[i];
		rec.updateJointAveragePosition(skel.get(), this);
		ConstraintMap& cons = constrainMapList[i];		
		// setup constraint
		for (unsigned int k=0;k<rec.jointNames.size();k++)
		{
			SBJoint* conJoint = skel->getJointByName(rec.jointNames[k]);
			if (conJoint)
			{
				auto constraint = std::make_unique<EffectorConstantConstraint>();
				constraint->efffectorName = rec.jointNames[k];
				constraint->targetPos = rec.posVec[k];
				auto* pjoint = dynamic_cast<SBJoint*>(conJoint->getParent()->getParent()->getParent());
				constraint->rootName = pjoint->jointName();
				//SmartBody::util::log("effector = %s, root = %s, target pos = %f %f %f",rec.jointNames[k].c_str(),constraint->rootName.c_str(), rec.posVec[k][0],rec.posVec[k][1],rec.posVec[k][2]);
				cons[rec.jointNames[k]] = std::move(constraint);
			}				
		}
		int curFrame = -1;
		for (float t=rec.startTime; t<=rec.endTime; t+= (float)getFrameRate())
		{
			int nextFrame = (int)(t/(float)getFrameRate());
			if (nextFrame == curFrame)
				continue;
			curFrame = nextFrame;
			if (frameConstraintMap.find(curFrame) == frameConstraintMap.end())
				frameConstraintMap[curFrame] = std::vector<int>();
			frameConstraintMap[curFrame].emplace_back(i);
		}
	}

	bool prevConstraint = false;
	for (int iframe=0; iframe<frames(); iframe++)
	{
 		if (frameConstraintMap.find(iframe) == frameConstraintMap.end())
		{
			//prevConstraint = false;
 			continue;
		}

		std::vector<int>& recIdxList = frameConstraintMap[iframe];
		cleanMotion->apply_frame(iframe);
		float* cur_p = cleanMotion->posture(iframe);		
		if (!prevConstraint)
		{
			ikScenario.setTreeNodeQuat(skel.get(),QUAT_INIT);
			//prevConstraint = true;
		}		
		ikScenario.copyTreeNodeQuat(QUAT_INIT,QUAT_CUR);
		ikScenario.setTreeNodeQuat(skel.get(),QUAT_REF);
		if (rootJoint->pos())
		{
			SkJointPos* jpos = rootJoint->pos();
			ikScenario.ikTreeRootPos = SrVec(jpos->value(0),jpos->value(1),jpos->value(2));
		}		
		if (rootJoint->getParent())
			ikScenario.ikGlobalMat = rootJoint->getParent()->gmat();

		// update IK for each constraint at this frame
		
// 		for (unsigned int k=0; k<recIdxList.size(); k++)
// 		{
// 			FootStepRecord& rec = footStepRecords[recIdxList[k]];
// 			ConstraintMap& cons = constrainMapList[recIdxList[k]];
// 			ikScenario.ikPosEffectors = &cons;
// 			ikScenario.ikRotEffectors = &noRotConstraint;
// 			ikCCD.update(&ikScenario);
// 		}
		
// 		ConstraintMap combineCons;
// 		for (unsigned int k=0; k<recIdxList.size(); k++)
// 		{
// 			ConstraintMap& cons = constrainMapList[recIdxList[k]];
// 			combineCons.insert(cons.begin(),cons.end());
// 		}
// 		ikScenario.ikPosEffectors = &combineCons;
// 		ikScenario.ikRotEffectors = &noRotConstraint;
// 		for (int k=0;k<10;k++)
// 		{
// 			ikJacobian.update(&ikScenario);
// 			ikScenario.copyTreeNodeQuat(QUAT_CUR,QUAT_INIT);	
// 		}
		

			
		for (auto ikNode : ikScenario.ikTreeNodes)
		{
				SBJoint* ikJoint = skel->getJointByName(ikNode->getNodeName());
			int chanID = mchan_arr.search(ikNode->getNodeName(),SkChannel::Quat);
			SrQuat& nq = ikNode->getQuat(QUAT_CUR);
			SrQuat kq; 
			if (ikJoint) kq = ikJoint->quat()->rawValue();
			//SrQuat& kq = 
			if (chanID != -1)
			{
				int floatIdx = mchan_arr.float_position(chanID);
				//SmartBody::util::log("old quat = %f %f %f %f",cur_p[ floatIdx + 0 ],cur_p[ floatIdx + 1 ],cur_p[ floatIdx + 2 ],cur_p[ floatIdx + 3 ] );
				//SmartBody::util::log("new quat = %f %f %f %f",nq.w,nq.x,nq.y,nq.z );
				//SmartBody::util::log("skel quat = %f %f %f %f",kq.w,kq.x,kq.y,kq.z );
				cur_p[ floatIdx + 0 ] = nq.w;
				cur_p[ floatIdx + 1 ] = nq.x;
				cur_p[ floatIdx + 2 ] = nq.y;
				cur_p[ floatIdx + 3 ] = nq.z;
			}
		}				
	}
	cleanMotion->disconnect();

	return cleanMotion;
}

SBMotion* SBMotion::mirror2(std::string name, std::string skeletonName, std::vector<std::string> from, std::vector<std::string> to)
{

	auto skeleton = SmartBody::SBScene::getScene()->getSkeleton(skeletonName);
	if (!skeleton)
	{
		SmartBody::util::log("Skeleton %s not found. Mirror motion %s not built.", skeletonName.c_str(), name.c_str());
		return nullptr;
	}
	SkMotion* motion = buildMirrorMotion(skeleton.get(), from, to);
	auto* sbmotion = dynamic_cast<SBMotion*>(motion);
	if (sbmotion)
	{
		std::string motionName = "";
		if (name == "")
		{
			motionName = sbmotion->getName();
			if (motionName == EMPTY_STRING)
				motionName = getName() + "_mirror";
		}
		else
			motionName = name;
		sbmotion->setName(motionName.c_str());

		bool success = SmartBody::SBScene::getScene()->getAssetManager()->addMotion(std::unique_ptr<SBMotion>(sbmotion));
		if (!success)
		{
			return nullptr;
		}

    // create a trail indicating that this motion was mirrored
    sbmotion->createStringAttribute("mirrorMotion", this->getName(), false, "mirroring", 110, false, false, false, "Which motion has this motion been mirrored from");
    sbmotion->createStringAttribute("mirrorSkeleton", skeletonName, false, "mirroring", 120, false, false, false, "Which skeleton has this motion been mirrored from");

    sbmotion->setTransformDepth(getTransformDepth() + 1); // increment the depth counter
  }
	return sbmotion;
}



SBMotion* SBMotion::mirror(std::string name, std::string skeletonName)
{
	 
	auto skeleton = SmartBody::SBScene::getScene()->getSkeleton(skeletonName);
	if (!skeleton)
	{
		SmartBody::util::log("Skeleton %s not found. Mirror motion %s not built.",skeletonName.c_str(),name.c_str());
		return nullptr;
	}
	std::vector<std::string> from;
	from.emplace_back("l_");
	std::vector<std::string> to;
	to.emplace_back("r_");
	SkMotion* motion = buildMirrorMotion(skeleton.get(), from, to);
	SBMotion* sbmotion = dynamic_cast<SBMotion*>(motion);
	if (sbmotion)
	{
		std::string motionName = "";
		if (name == "")
		{
			motionName = sbmotion->getName();
			if (motionName == EMPTY_STRING)
				motionName = getName() + "_mirror";
		}
		else
			motionName = name;
		sbmotion->setName(motionName.c_str());

		if (!SmartBody::SBScene::getScene()->getAssetManager()->addMotion(std::unique_ptr<SBMotion>(sbmotion))) {
			return nullptr;
		}

    // create a trail indicating that this motion was mirrored
    sbmotion->createStringAttribute("mirrorMotion", this->getName(), false, "mirroring", 110, false, false, false, "Which motion has this motion been mirrored from");
    sbmotion->createStringAttribute("mirrorSkeleton", skeletonName, false, "mirroring", 120, false, false, false, "Which skeleton has this motion been mirrored from");

    sbmotion->setTransformDepth(getTransformDepth()+1); // increment the depth counter
  }
	return sbmotion;
}


SBMotion* SBMotion::mirrorChildren( std::string name, std::string skeletonName, std::string parentJointName )
{
	 
	auto skeleton = SmartBody ::SBScene::getScene()->getSkeleton(skeletonName);
	if (!skeleton)
	{
		SmartBody::util::log("Skeleton %s not found. Mirror motion %s not built.",skeletonName.c_str(),name.c_str());
		return nullptr;
	}
	std::map<std::string,bool> jointNameMap;
	SBJoint* pjoint = skeleton->getJointByName(parentJointName);
	std::vector<SBJoint*> childJoints = pjoint->getDescendants();
	for (auto & childJoint : childJoints)
	{
		jointNameMap[childJoint->getMappedJointName()] = true;
	}
	// add the parent joint as well
	jointNameMap[pjoint->getMappedJointName()] = true;
	
	std::vector<std::string> from;
	from.emplace_back("l_");
	std::vector<std::string> to;
	to.emplace_back("r_");

	SkMotion* motion = buildMirrorMotionJoints(skeleton.get(),jointNameMap, from, to);
	SBMotion* sbmotion = dynamic_cast<SBMotion*>(motion);
	if (sbmotion)
	{
		std::string motionName = "";
		if (name == "")
		{
			motionName = sbmotion->getName();
			if (motionName == EMPTY_STRING)
				motionName = getName() + "_mirror";
		}
		else
			motionName = name;
		sbmotion->setName(motionName.c_str());

		if (!SmartBody::SBScene::getScene()->getAssetManager()->addMotion(std::unique_ptr<SBMotion>(sbmotion))) {
			return nullptr;
		}
		sbmotion->setTransformDepth(getTransformDepth()+1); // increment the depth counter
	}
	return sbmotion;
}

SBMotion* SBMotion::smoothCycle( std::string name, float timeInterval )
{
	bool smoothBase = false; 	
	return motionSmoothCycle(timeInterval, smoothBase, name);

}

SBAPI SBMotion* SBMotion::smoothCycleWithBase(std::string name, float timeInterval)
{
	bool smoothBase = true;
	return motionSmoothCycle(timeInterval, smoothBase, name);
}

SBAPI SBMotion* SBMotion::motionSmoothCycle(float timeInterval, bool smoothBase, std::string name)
{
	SkMotion* motion = buildSmoothMotionCycle(timeInterval, smoothBase);
	SBMotion* sbmotion = dynamic_cast<SBMotion*>(motion);
	if (sbmotion)
	{
		std::string motionName = "";
		if (name == "")
		{
			motionName = sbmotion->getName();
			if (motionName == EMPTY_STRING)
				motionName = getName() + "_smoothCycle";
		}
		else
			motionName = name;
		sbmotion->setName(motionName.c_str());
		if (!SmartBody::SBScene::getScene()->getAssetManager()->addMotion(std::unique_ptr<SBMotion>(sbmotion))) {
			return nullptr;
		}
		sbmotion->setTransformDepth(getTransformDepth() + 1); // increment the depth counter 
		sbmotion->createStringAttribute("smoothMotion", this->getName(), false, "smoothing", 110, false, false, false, "Which motion has this motion been smoothed from");
		sbmotion->createDoubleAttribute("smoothInterval", timeInterval, false, "smoothing", 120, false, false, false, "how long is the smooth time interval");
	}

	return sbmotion;
}


float SBMotion::getJointSpeed(SBJoint* joint, float startTime, float endTime)
{
	if (!joint)
		return 0.f;
	if (connected_skeleton() == nullptr)
	{
		SmartBody::util::log("Motion %s is not connected to any skeleton, cannot retrieve parameter speed.", getName().c_str());
		return 0;
	}

	float dt = duration() / float(frames() - 1);
	int minFrameId = int(startTime / dt);
	int maxFrameId = int(endTime / dt);
	float distance = 0;
	for (int i = minFrameId; i < maxFrameId + 1; i++)
	{
		apply_frame(i);
		connected_skeleton()->update_global_matrices();
		const SrMat& srcMat = joint->gmat();
		SrVec srcPt = SrVec(srcMat.get(12), srcMat.get(13), srcMat.get(14));
		apply_frame(i + 1);
		connected_skeleton()->update_global_matrices();
		const SrMat& destMat = joint->gmat();
		SrVec destPt = SrVec(destMat.get(12), destMat.get(13), destMat.get(14));
		distance += dist(srcPt, destPt);
	}
	float accSpd = distance / (endTime - startTime);
	connected_skeleton()->clearJointValues(); // reset the joint quat/pos
	return accSpd;
}

float SBMotion::getJointSpeedAxis(SBJoint* joint, const std::string& axis, float startTime, float endTime)
{
	int axisIndex = 0;
	if (axis == "X" || axis == "x")
		axisIndex = 0;
	else if (axis == "Y" || axis == "y")
		axisIndex = 1;
	else if (axis == "Z" || axis == "z")
		axisIndex = 2;
	else
	{
		SmartBody::util::log("Bad axis specified '%s', defaulting to use the X-axis.", axis.c_str());
	}

	if (!joint)
		return 0.f;
	if (connected_skeleton() == nullptr)
	{
		SmartBody::util::log("Motion %s is not connected to any skeleton, cannot retrieve parameter speed.", getName().c_str());
		return 0;
	}

	float dt = duration() / float(frames() - 1);
	int minFrameId = int(startTime / dt);
	int maxFrameId = int(endTime / dt);
	float distance = 0;
	for (int i = minFrameId; i < maxFrameId + 1; i++)
	{
		apply_frame(i);
		connected_skeleton()->update_global_matrices();
		const SrMat& srcMat = joint->gmat();
		SrVec srcPt = SrVec(srcMat.get(12), srcMat.get(13), srcMat.get(14));
		apply_frame(i + 1);
		connected_skeleton()->update_global_matrices();
		const SrMat& destMat = joint->gmat();
		SrVec destPt = SrVec(destMat.get(12), destMat.get(13), destMat.get(14));
		distance += destPt[axisIndex] - srcPt[axisIndex];
	}
	float accSpd = distance / (endTime - startTime);
	connected_skeleton()->clearJointValues(); // reset the joint quat/pos
	return accSpd;
}



float SBMotion::getJointAngularSpeed(SBJoint* joint, float startTime, float endTime)
{
	if (!joint)
	{
		return 0.f;
	}
	if (connected_skeleton() == nullptr)
	{
		SmartBody::util::log("Motion %s is not connected to any skeleton, cannot retrieve parameter angular speed.", getName().c_str());
		return 0;
	}
	float dt = duration() / float(frames() - 1);
	int minFrameId = int(startTime / dt);
	int maxFrameId = int(endTime / dt);
	float diffRotY = 0.0f;
	for (int i = minFrameId; i < maxFrameId + 1; i++)
	{
		apply_frame(i);
		connected_skeleton()->update_global_matrices();
		SrMat srcMat = joint->gmat();
		float rx, ry, rz;
		
		//sr_euler_angles(132, srcMat, rx, ry, rz);
		//float srcRotY = ry;
		apply_frame(i + 1);
		connected_skeleton()->update_global_matrices();
		SrMat destMat = joint->gmat();
		//sr_euler_angles(132, destMat, rx, ry, rz);
		//float destRotY = ry;
		float diff;
		//if (destRotY * srcRotY < 0 && fabs(destRotY) > 1.0f)
		//	diff = - destRotY - srcRotY;
		//else
		//	diff = destRotY - srcRotY;
		sr_euler_angles(132, destMat*srcMat.inverse(), rx, ry, rz);
		diff = ry;
		diffRotY += diff;
	}
	float accAngularSpd = diffRotY / (endTime - startTime);
	accAngularSpd *= (180.0f/ float(M_PI));
	connected_skeleton()->clearJointValues(); // reset the joint quat/pos
	return accAngularSpd;
}

float SBMotion::getJointAngularSpeedAxis(SBJoint* joint, const std::string& axis, float startTime, float endTime)
{	
	int axisIndex = 0;
	if (axis == "X" || axis == "x")
		axisIndex = 0;
	else if (axis == "Y" || axis == "y")
		axisIndex = 1;
	else if (axis == "Z" || axis == "z")
		axisIndex = 2;
	else
	{
		SmartBody::util::log("Bad axis specified '%s', defaulting to use the X-axis.", axis.c_str());
	}

	if (!joint)
	{
		return 0.f;
	}
	if (connected_skeleton() == nullptr)
	{
		SmartBody::util::log("Motion %s is not connected to any skeleton, cannot retrieve parameter angular speed.", getName().c_str());
		return 0;
	}
	float dt = duration() / float(frames() - 1);
	int minFrameId = int(startTime / dt);
	int maxFrameId = int(endTime / dt);
	float diffRot = 0.0f;
	for (int i = minFrameId; i < maxFrameId + 1; i++)
	{
		apply_frame(i);
		connected_skeleton()->update_global_matrices();
		const SrMat& srcMat = joint->gmat();
		float rx, ry, rz;
		sr_euler_angles(132, srcMat, rx, ry, rz);
		float srcRot = rx;
		if (axisIndex == 0)
			srcRot = rx;
		else if (axisIndex == 1)
			srcRot = ry;
		else if (axisIndex == 2)
			srcRot = rz;
		
		apply_frame(i + 1);
		connected_skeleton()->update_global_matrices();
		const SrMat& destMat = joint->gmat();
		sr_euler_angles(132, destMat, rx, ry, rz);

		float destRot = rx;
		if (axisIndex == 0)
			destRot = rx;
		else if (axisIndex == 1)
			destRot = ry;
		else if (axisIndex == 2)
			destRot = rz;
		float diff;
		if (destRot * srcRot < 0 && fabs(destRot) > 1.0f)
			diff = - destRot - srcRot;
		else
			diff = destRot - srcRot;
		diffRot += diff;
	}
	float accAngularSpd = diffRot / (endTime - startTime);
	accAngularSpd *= (180.0f/ float(M_PI));
	connected_skeleton()->clearJointValues(); // reset the joint quat/pos
	return accAngularSpd;
}

// Added by Adil . gets angular velocity
SrVec SBMotion::getJointAngularVelocity(SBJoint* joint, float startTime, float endTime)
{	

	if (!joint)
	{
		SmartBody::util::log("Joint is nullptr");
		return SrVec();
	}
	if (connected_skeleton() == nullptr)
	{
		SmartBody::util::log("Motion %s is not connected to any skeleton, cannot retrieve parameter angular speed.", getName().c_str());
		return SrVec();
	}
	float dt = duration() / float(frames() - 1);
	int minFrameId = int(startTime / dt);
	int maxFrameId = int(endTime / dt);
	SrVec diff;
	for (int i = minFrameId; i < maxFrameId + 1; i++)
	{
		apply_frame(i);
		connected_skeleton()->update_global_matrices();
		const SrMat& srcMat = joint->gmat();
		SrQuat srcQuat(srcMat);
		
		apply_frame(i + 1);
		connected_skeleton()->update_global_matrices();
		const SrMat& destMat = joint->gmat();
		SrQuat destQuat(srcMat);
		
		SrQuat diffQuat = destQuat.inverse()*srcQuat;
		diffQuat.normalize();
		SrVec diffAngle = diffQuat.axisAngle();

		diff += diffAngle;
	}

	SrVec accAngularVelocity = diff/(endTime-startTime);

	connected_skeleton()->clearJointValues(); // reset the joint quat/pos

	return accAngularVelocity;
}

std::vector<float> SBMotion::getJointTransition(SBJoint* joint, float startTime, float endTime)
{
	std::vector<float> transitions;
	if (!joint)
	{
		SmartBody::util::log("No joint found when determining joint transitions.");
		transitions.emplace_back(0);
		transitions.emplace_back(0);
		transitions.emplace_back(0);
		return transitions;
	}

	if (connected_skeleton() == nullptr)
	{
		SmartBody::util::log("Motion %s is not connected to any skeleton, cannot retrieve parameter angular speed.", getName().c_str());
		transitions.emplace_back(0);
		transitions.emplace_back(0);
		transitions.emplace_back(0);
		return transitions;
	}
	float dt = duration() / float(frames() - 1);
	int minFrameId = int(startTime / dt);
	int maxFrameId = int(endTime / dt);
	apply_frame(minFrameId);
	connected_skeleton()->update_global_matrices();
	const SrMat& srcMat = joint->gmat();
	const SrMat& srcMat0 = joint->gmatZero();
	SrVec srcPnt = SrVec(srcMat.get(12), srcMat.get(13), srcMat.get(14));
	float rx, ry, rz;
	float rx0, ry0, rz0;
	sr_euler_angles(132, srcMat, rx, ry, rz);
	sr_euler_angles(132, srcMat0, rx0, ry0, rz0);
	apply_frame(maxFrameId);
	connected_skeleton()->update_global_matrices();
	const SrMat& destMat = joint->gmat();
	SrVec destPnt = SrVec(destMat.get(12), destMat.get(13), destMat.get(14));
	SrVec transitionVec = destPnt - srcPnt;
	SrVec heading = SrVec(sin(ry - ry0 - 1.57f), 0, cos(ry - ry0 - 1.57f));
	float x = dot(transitionVec, heading);
	transitions.emplace_back(x);
	float y = destMat.get(14) - srcMat.get(14);
	transitions.emplace_back(y);
	heading = SrVec(sin(ry - ry0), 0, cos(ry - ry0));
	float z = dot(transitionVec, heading);
	transitions.emplace_back(z);
	connected_skeleton()->clearJointValues(); // reset the joint quat/pos
	return transitions;
}

SrVec SBMotion::getJointPosition(SBJoint* joint, float time)
{
	if (!joint)
		return SrVec();
	if (connected_skeleton() == nullptr)
	{
		SmartBody::util::log("Motion %s is not connected to any skeleton, cannot retrieve parameter speed.", getName().c_str());
		return SrVec();
	}

	float dt = duration() / float(frames() - 1);
	int frameId = int(time / dt);
	float distance = 0;
	
	apply_frame(frameId);
	connected_skeleton()->update_global_matrices();
	const SrMat& srcMat = joint->gmat();
	SrVec point = SrVec(srcMat.get(12), srcMat.get(13), srcMat.get(14));
	connected_skeleton()->clearJointValues(); // reset the joint quat/pos
	
	return point;
}

SrVec SBMotion::getJointPositionFromBase(SBJoint* joint, SBJoint* baseJoint, float time)
{
	if (!joint || !baseJoint)
		return SrVec();
	if (connected_skeleton() == nullptr)
	{
		SmartBody::util::log("Motion %s is not connected to any skeleton, cannot retrieve parameter speed.", getName().c_str());
		return SrVec();
	}

	float dt = duration() / float(frames() - 1);
	int frameId = int(time / dt);
	float distance = 0;
	
	apply_frame(frameId);
	connected_skeleton()->update_global_matrices();
	
	const SrMat& srcMat = joint->gmat();
	const SrMat& baseMat = baseJoint->gmat();

	// find the different
	const SrMat& diffMat = srcMat * baseMat.inverse();
	
	SrVec point = SrVec(diffMat.get(12), diffMat.get(13), diffMat.get(14));
	connected_skeleton()->clearJointValues(); // reset the joint quat/pos
	
	return point;
}

SrQuat SBMotion::getJointRotation(SBJoint* joint, float time)
{
	if (!joint)
		return SrVec();
	if (connected_skeleton() == nullptr)
	{
		SmartBody::util::log("Motion %s is not connected to any skeleton, cannot retrieve parameter speed.", getName().c_str());
		return SrVec();
	}

	float dt = duration() / float(frames() - 1);
	int frameId = int(time / dt);
	float distance = 0;
	
	apply_frame(frameId);
	connected_skeleton()->update_global_matrices();
	const SrMat& srcMat = joint->gmat();
	SrQuat rot = SrQuat(srcMat);
	connected_skeleton()->clearJointValues(); // reset the joint quat/pos
	
	return rot;	
}

SBJointMap* SBMotion::getJointMap()
{
	SBJointMapManager* jointMapManager = SBScene::getScene()->getJointMapManager();
	const std::vector<std::string>& jointMapNames = jointMapManager->getJointMapNames();
	for (size_t i = 0; i < jointMapNames.size(); ++i)
	{
		SBJointMap* jointMap = jointMapManager->getJointMap(jointMapNames[i]);
		if (jointMap->isAppliedToMotion(this->getName()))
			return jointMap;
	}
	return nullptr;
}

/*
// subtract current motion with base motion 
SBAPI SBMotion* SBMotion::getOffset(SBMotion* baseMotion, std::string motionName)
{
	if (baseMotion == nullptr)
		return nullptr;

	if (baseMotion->frames() == 0)
		return nullptr;

	SkChannelArray& ch = channels();
	SkChannelArray& baseCh = baseMotion->channels();
	int numFrames = frames();
	SmartBody::SBMotion* originalMotion = dynamic_cast<SmartBody::SBMotion*>(this);
	SmartBody::SBMotion* offsetMotion = new SmartBody::SBMotion();
	offsetMotion->setMotionSkeletonName(originalMotion->getMotionSkeletonName());	
	if (motionName == "")
		motionName = originalMotion->getName() + "_offset_" + baseMotion->getName();
	offsetMotion->setName(motionName);
	srSynchPoints sp(synch_points);
	offsetMotion->synch_points = sp;
	offsetMotion->init(ch);
	float* baseP = baseMotion->posture(0);
	for (int f = 0; f < numFrames; f++)
	{
		offsetMotion->insert_frame(f, this->keytime(f));
		float* p = posture(f);
		for (int c = 0; c < ch.size(); c++)
		{
			//if (ch.mappedName(c) == "base")
			//	continue;

			SkChannel& channel = ch[c];
			// find corresponding channel from base motion
			int baseC = baseCh.linear_search(ch.name(c), channel.type);
			if (baseC >= 0)
			{
				int floatPos = ch.float_position(c);
				int baseFloatPos = baseCh.float_position(baseC);
				if (channel.type == SkChannel::XPos ||
					channel.type == SkChannel::YPos ||
					channel.type == SkChannel::ZPos)
				{
					float v = p[floatPos] - baseP[baseFloatPos];
					offsetMotion->posture(f)[floatPos] = v;
				}
				else
				{
					SrQuat q = SrQuat(p[floatPos + 0], p[floatPos + 1], p[floatPos + 2], p[floatPos + 3]);
					SrQuat baseQ = SrQuat(baseP[baseFloatPos + 0], baseP[baseFloatPos + 1], baseP[baseFloatPos + 2], baseP[baseFloatPos + 3]);
					SrQuat diff = q.inverse() * baseQ;
					//SrQuat diff = baseQ.inverse() * q;
					offsetMotion->posture(f)[floatPos + 0] = diff.w;
					offsetMotion->posture(f)[floatPos + 1] = diff.x;
					offsetMotion->posture(f)[floatPos + 2] = diff.y;
					offsetMotion->posture(f)[floatPos + 3] = diff.z;
				}
			}
		}
	}
	if (offsetMotion != nullptr)
	{
		SmartBody::SBScene::getScene()->getAssetManager()->addMotion(offsetMotion);
		SmartBody::util::log("Add motion %s to scene", offsetMotion->getName().c_str());
	}

	return offsetMotion;
}
*/

SBMotion* SBMotion::getOffsetParent()
{
	return _offsetParent;
}

SBMotion* SBMotion::getOffset(std::string motionName, int index)
{
	if (_offsetMotions.find(index) != _offsetMotions.end())
	{
		if (_offsetMotions[index] != nullptr)
			return _offsetMotions[index];
	}

	SkChannelArray& ch = channels();
	int numFrames = frames();
	SmartBody::SBMotion* originalMotion = dynamic_cast<SmartBody::SBMotion*>(this);
	SmartBody::SBMotion* offsetMotion = new SmartBody::SBMotion();
	offsetMotion->setMotionSkeletonName(originalMotion->getMotionSkeletonName());
	std::stringstream ss;
	if (motionName == "")
		ss << originalMotion->getName() << "_offset_" << index;
	offsetMotion->setName(ss.str().c_str());
	srSynchPoints sp(synch_points);
	offsetMotion->synch_points = sp;
	offsetMotion->init(ch);
	float* baseP = this->posture(index);
	for (int f = 0; f < numFrames; f++)
	{
		offsetMotion->insert_frame(f, this->keytime(f));
		float* p = posture(f);
		for (int c = 0; c < ch.size(); c++)
		{
			SkChannel& channel = ch[c];
			
			int floatPos = ch.float_position(c);
			if (channel.type == SkChannel::XPos ||
				channel.type == SkChannel::YPos ||
				channel.type == SkChannel::ZPos)
			{
				float v = p[floatPos] - baseP[floatPos];
				offsetMotion->posture(f)[floatPos] = v;
			}
			else
			{
				SrQuat q = SrQuat(p[floatPos + 0], p[floatPos + 1], p[floatPos + 2], p[floatPos + 3]);
				SrQuat baseQ = SrQuat(baseP[floatPos + 0], baseP[floatPos + 1], baseP[floatPos + 2], baseP[floatPos + 3]);
				//SrQuat diff = q.inverse() * baseQ;
				SrQuat diff = baseQ.inverse() * q;
				offsetMotion->posture(f)[floatPos + 0] = diff.w;
				offsetMotion->posture(f)[floatPos + 1] = diff.x;
				offsetMotion->posture(f)[floatPos + 2] = diff.y;
				offsetMotion->posture(f)[floatPos + 3] = diff.z;
			}

		}
	}

	if (offsetMotion )
	{
		if (!SmartBody::SBScene::getScene()->getAssetManager()->addMotion(std::unique_ptr<SBMotion>(offsetMotion))) {
			return nullptr;
		}
		SmartBody::util::log("Add motion %s to scene", offsetMotion->getName().c_str());
		// insert into the map
		if (_offsetMotions.find(index) != _offsetMotions.end())
		{
			_offsetMotions[index] = offsetMotion;
		}
		else
		{
			_offsetMotions.insert(std::make_pair(index, offsetMotion));
		}
		_offsetMotions[index]->_offsetParent = this;
		return _offsetMotions[index];
	} else {
		return nullptr;
	}


}

bool SBMotion::translate(float x, float y, float z, const std::string& baseJointName)
{
	SrVec offset(x, y, z);
	// get the base x, y, z
	SkChannelArray& ch = channels();
	int pos[3];
	pos[0] = ch.search(baseJointName, SkChannel::XPos);
	pos[1] = ch.search(baseJointName, SkChannel::YPos);
	pos[2] = ch.search(baseJointName, SkChannel::ZPos);
	if (pos[0] == -1 || pos[1] == -1 || pos[2] == -1)
	{
		SmartBody::util::log("No joint named '%s' found in motion %s, cannot translate.", baseJointName.c_str(), getName().c_str());
		return false;
	}

	int numFrames = frames();
	for (int f = 0; f < numFrames; f++)
	{
		float* curFrame = posture(f);
		for (int p = 0; p < 3; p++)
		{
			curFrame[pos[p]] = curFrame[pos[p]] + offset[p];
		}

	}

	SmartBody::util::log("Motion %s with %d frames offset by (%f, %f, %f)", getName().c_str(), frames(), offset[0], offset[1], offset[2]);
	return true;
}

bool SBMotion::rotate(float xaxis, float yaxis, float zaxis, const std::string& baseJointName)
{
	SrVec rotation;
	rotation[0] = xaxis;
	rotation[1] = yaxis;
	rotation[2] = zaxis;

	// get the base quaternion
	SkChannelArray& ch = channels();
	int pos = -1;
	pos = ch.search(baseJointName.c_str(), SkChannel::Quat);
	if (pos == -1)
	{
		SmartBody::util::log("No joint named '%s' found in motion %s, cannot rotate.", baseJointName.c_str(), getName().c_str());
		return false;
	}

	SrMat xRot;
	xRot.rotx(rotation[0] * (float) M_PI / 180.0f);
	SrMat yRot;
	yRot.roty(rotation[1] * (float) M_PI / 180.0f);
	SrMat zRot;
	zRot.rotz(rotation[2] * (float) M_PI / 180.0f);
	SrMat finalMat = xRot * yRot * zRot;

	int numFrames = frames();
	for (int f = 0; f < numFrames; f++)
	{
		float* curFrame = posture(f);
		SrQuat curQuat(curFrame[pos], curFrame[pos + 1], curFrame[pos + 2], curFrame[pos + 3]); 
		SrMat currentMat;
		curQuat.get_mat(currentMat);
		currentMat *= finalMat;
		SrQuat newQuat(currentMat);
		curFrame[pos + 0] = newQuat.w;
		curFrame[pos + 1] = newQuat.x;
		curFrame[pos + 2] = newQuat.y;
		curFrame[pos + 3] = newQuat.z;
	}

	SmartBody::util::log("Motion %s with %d frames rotated by (%f, %f, %f)", getName().c_str(), frames(), rotation[0], rotation[1], rotation[2]);
	return true;
}

bool SBMotion::scale(float factor)
{
	SkChannelArray& ch = channels();
	int numFrames = frames();
	for (int f = 0; f < numFrames; f++)
	{
		float* p = posture(f);
		int index = 0;
		for (int c = 0; c < ch.size(); c++)
		{
			SkChannel& channel = ch[c];
			// only scale the positions
			if (channel.type == SkChannel::XPos ||
				channel.type == SkChannel::YPos ||
				channel.type == SkChannel::ZPos)
			{
				p[index] *= factor;
			}
			index += channel.size();
		}
	}
	SmartBody::util::log("Motion %s with %d frames scaled by a factor of %f", getName().c_str(), frames(), factor);
	return true;
}

bool SBMotion::retime(float factor)
{
	for (int f = 0; f < frames(); f++)
	{
		keytime(f, keytime(f) * factor);
	}

	//SmartBody::util::log("Motion %s with %d frames retimed by a factor of %f", getName().c_str(), frames(), factor);
	return true;
}


bool SBMotion::trim(int numFramesFromFront, int numFramesFromBack)
{
	int newFrames = frames() - numFramesFromFront - numFramesFromBack;
	if (numFramesFromFront < 0 || numFramesFromBack < 0)
	{
		SmartBody::util::log("trim frames can not be negative");
		return false;
	}
	if (newFrames < 1)
	{
		SmartBody::util::log("Motion %s has only %d frames, can not be trimmed by %d frames at front and %d frames at back",getName().c_str(), frames(), numFramesFromFront, numFramesFromBack);
		return false;
	}
	for (int i=0;i<newFrames;i++)
	{
		int idx = i + numFramesFromFront;
		Frame& srcFrame = _frames[idx];
		Frame& tgtFrame = _frames[i];
		// copy over the posture at that frame, but reserved the key time
		memcpy(tgtFrame.posture,srcFrame.posture,sizeof(float)*posture_size());
	}
	// remove all frames from the back
	for (int i=frames()-1; i>= newFrames; i--)
	{
		Frame& delFrame = _frames[i];
		//free(delFrame.posture);
		delete [] delFrame.posture;
		_frames.pop_back();
	}

	return true;
}

double SBMotion::getTimeStart()
{
	return time_start();
}

double SBMotion::getTimeReady()
{
	return time_ready();
}

double SBMotion::getTimeStrokeStart()
{
	return time_stroke_start();
}

double SBMotion::getTimeStroke()
{
	return time_stroke_emphasis();
}

double SBMotion::getTimeStrokeEnd()
{
	return time_stroke_end();
}

double SBMotion::getTimeRelax()
{
	return time_relax();
}

double SBMotion::getTimeStop()
{
	return time_stop();
}

void SBMotion::setTimeStart(double time)
{
	setSyncPoint("start", time);
}

void SBMotion::setTimeReady(double time)
{
	setSyncPoint("ready", time);
}

void SBMotion::setTimeStrokeStart(double time)
{
	setSyncPoint("stroke_start", time);
}

void SBMotion::setTimeStroke(double time)
{
	setSyncPoint("stroke", time);
}

void SBMotion::setTimeStrokeEnd(double time)
{
	setSyncPoint("stroke_stop", time);
}

void SBMotion::setTimeRelax(double time)
{
	setSyncPoint("relax", time);
}

void SBMotion::setTimeStop(double time)
{
	setSyncPoint("stop", time);
}

double SBMotion::getDuration()
{
	return duration();
}


bool SBMotion::setSyncPoint( const std::string& syncTag, double time )
{
	int tagIndex = synch_points.tag_index(syncTag.c_str());
	return synch_points.set_time(tagIndex, time);
}

void SBMotion::validateSyncPoint(const std::string& syncTag)
{
	if (syncTag == "start")
	{
		if (getTimeStart() > getTimeReady())
			setSyncPoint(syncTag, getTimeReady());
	}
	if (syncTag == "ready")
	{
		if (getTimeReady() < getTimeStart())
			setSyncPoint(syncTag, getTimeStart());
		if (getTimeReady() > getTimeStrokeStart())
			setSyncPoint(syncTag, getTimeStrokeStart());
	}
	if (syncTag == "stroke_start")
	{
		if (getTimeStrokeStart() < getTimeReady())
			setSyncPoint(syncTag, getTimeReady());
		if (getTimeStrokeStart() > getTimeStroke())
			setSyncPoint(syncTag, getTimeStroke());
	}
	if (syncTag == "stroke")
	{
		if (getTimeStroke() < getTimeStrokeStart())
			setSyncPoint(syncTag, getTimeStrokeStart());
		if (getTimeStroke() > getTimeStrokeEnd())
			setSyncPoint(syncTag, getTimeStrokeEnd());
	}
	if (syncTag == "stroke_stop")
	{
		if (getTimeStrokeEnd() < getTimeStroke())
			setSyncPoint(syncTag, getTimeStroke());
		if (getTimeStrokeEnd() > getTimeRelax())
			setSyncPoint(syncTag, getTimeRelax());
	}
	if (syncTag == "relax")
	{
		if (getTimeRelax() < getTimeStrokeEnd())
			setSyncPoint(syncTag, getTimeStrokeEnd());
		if (getTimeRelax() > getTimeStop())
			setSyncPoint(syncTag, getTimeStop());
	}
	if (syncTag == "stop")
	{
		if (getTimeStop() < getTimeRelax())
			setSyncPoint(syncTag, getTimeRelax());
	}
}

double SBMotion::getFrameRate()
{
	return getDuration() / double(getNumFrames() - 1);
}

void SBMotion::addEvent(double time, const std::string& type, const std::string& parameters, bool onceOnly)
{
	SBMotionEvent* motionEvent = new SBMotionEvent();
	motionEvent->setIsOnceOnly(onceOnly);
	motionEvent->setTime(time);
	motionEvent->setType(type);
	motionEvent->setParameters(parameters);
	addMotionEvent(motionEvent);
}

bool SBMotion::kMeansClustering1D(int num, std::vector<double>& inputPoints, std::vector<double>& outMeans)
{
	if ((int)inputPoints.size() < num)
	{
		SmartBody::util::log("PAAutoFootStepsEditor::kMeansClustering1D Warning: Input points are less than number of means");
		return false;
	}

	// pick initial point
	int step = inputPoints.size() / num;
	for (int i = 0; i < num; i++)
		outMeans.emplace_back(inputPoints[i * step]);

	double convergence = 0.1;
	calculateMeans(inputPoints, outMeans, convergence);

	outMeans.resize(num);
	return true;
}


void SBMotion::calculateMeans(std::vector<double>&inputPoints, std::vector<double>& means, double convergentValue)
{
	std::vector<std::vector<double> > partitionBin;
	partitionBin.resize(means.size());

	// partition
	for (size_t i = 0; i < inputPoints.size(); i++)
	{
		double minDist = 9999;
		int minDistId = -1;
		for (size_t j = 0; j < means.size(); j++)
		{
			double dist = fabs(inputPoints[i] - means[j]);
			if (dist < minDist)
			{
				minDist = dist;
				minDistId = j;
			}
		}
		if (minDistId >= 0)
		{
			partitionBin[minDistId].emplace_back(inputPoints[i]);
		}
	}

	// get new means
	std::vector<double> newMeans;
	for (size_t i = 0; i < means.size(); i++)
	{
		double newMean = 0;
		for (size_t j = 0; j < partitionBin[i].size(); j++)
			newMean += partitionBin[i][j];
		newMean /= double(partitionBin[i].size());
		newMeans.emplace_back(newMean);
	}

	double diff = 0.0f;
	for (size_t i = 0; i < means.size(); i++)
	{
		diff = diff + (means[i] - newMeans[i]) * (means[i] - newMeans[i]);
	}
	diff = sqrt(diff);

	if (diff < convergentValue)
		return;
	else
	{
		means = newMeans;
		calculateMeans(inputPoints, means, convergentValue);
	}
}


SBMotion* SBMotion::constrain( std::string name, std::string srcSkeletonName, std::string tgtSkeletonName, std::string tgtMotionName, std::vector<std::string>& endJoints, std::vector<std::string>& endJointRoots )
{
	auto origSkel = SBScene::getScene()->getSkeleton(srcSkeletonName);
	if (!origSkel) return nullptr;
	SBMotion* tgtMotion = SBScene::getScene()->getMotion(tgtMotionName);
	if (!tgtMotion) return nullptr;
	auto tgtSkel  = SBScene::getScene()->getSkeleton(tgtSkeletonName);
	if (!tgtSkel) return nullptr;

	SBMotion* constraintMotion = buildConstraintMotion(origSkel.get(), tgtSkel.get(), tgtMotion, endJoints, endJointRoots);

	if (constraintMotion)
	{
		std::string motionName = "";
		if (name == "")
		{
			motionName = constraintMotion->getName();
			if (motionName == EMPTY_STRING)
				motionName = getName() + "_cleanup";
		}
		else
			motionName = name;
		constraintMotion->setName(motionName.c_str());
		
		//mcu.motion_map.insert(std::pair<std::string, SkMotion*>(motionName, cleanUpMotion));
		if (!SmartBody::SBScene::getScene()->getAssetManager()->addMotion(std::unique_ptr<SBMotion>(constraintMotion))) {
			return nullptr;
		}
	}
	return constraintMotion;

}


SBMotion* SBMotion::footSkateCleanUp( std::string name, std::vector<std::string>& footJoints, std::string srcSkeletonName, 
									  std::string srcMotionName, std::string tgtSkeletonName, std::string tgtRootName, 
									  float floorHeight, float heightThreshold, float speedThreshold )
{
	SmartBody::util::log("foot skate cleanup for motion %s.", this->getName().c_str());

	auto origSkel = SBScene::getScene()->getSkeleton(srcSkeletonName);
	if (!origSkel) return nullptr;
	SBMotion* origMotion = SBScene::getScene()->getMotion(srcMotionName);
	if (!origMotion) return nullptr;

	SmartBody::util::log("foot skate cleanup for motion %s.", this->getName().c_str());

	SBSkeleton skelCopy(*origSkel);
	std::vector<FootStepRecord> footStepRecords;
	bool hasFootPlant = origMotion->autoFootPlantDetection(origSkel.get(), footJoints, floorHeight, heightThreshold, speedThreshold,footStepRecords);

	for (auto & record : footStepRecords)
	{
			SmartBody::util::log("Footstep joint = %s, start frame = %f, end frame = %f",record.jointNames[0].c_str(), record.startTime, record.endTime);
	}

	SBMotion* cleanUpMotion = autoFootSkateCleanUp(name,tgtSkeletonName, tgtRootName, footStepRecords);
	
	if (cleanUpMotion)
	{
		std::string motionName = "";
		if (name == "")
		{
			motionName = cleanUpMotion->getName();
			if (motionName == EMPTY_STRING)
				motionName = getName() + "_cleanup";
		}
		else
			motionName = name;
		cleanUpMotion->setName(motionName.c_str());
		if (!SmartBody::SBScene::getScene()->getAssetManager()->addMotion(std::unique_ptr<SBMotion>(cleanUpMotion))) {
			return nullptr;
		}
	}
	return cleanUpMotion;
}

bool SBMotion::autoFootPlantDetection( SBSkeleton* srcSk, std::vector<std::string>& footJoints, float floorHeight, float heightThreshold, float speedThreshold, std::vector<FootStepRecord>& footStepRecords )
{
	int speedWindow = 3;
	footStepRecords.clear();	
	std::vector<std::vector<float> > vecTiming;
	vecTiming.resize(footJoints.size());	
	std::vector<std::vector<SrVec> > vecPos;
	vecPos.resize(footJoints.size());
	this->connect(srcSk);
	for(int f = 0; f < frames(); f++)
	{
		this->apply_frame(f);
		this->connected_skeleton()->update_global_matrices();
		for (size_t jointId = 0; jointId < footJoints.size(); jointId ++)
		{
			std::string jointName = footJoints[jointId];
			SBJoint* joint = srcSk->getJointByName(jointName);
			if (!joint)
				continue;			
			// get height
			const SrMat& gMat = joint->gmat();
			SrVec gPos = SrVec(gMat.get(12), gMat.get(13), gMat.get(14));			
			// get speed
			int startFrame = f - speedWindow / 2;
			int endFrame = f + speedWindow / 2;
			float startTime = startFrame * (float)getFrameRate();
			float endTime = endFrame * (float)getFrameRate();
			float speed = getJointSpeed(joint, startTime, endTime);		

			std::vector<SBJoint*> descendants = joint->getDescendants();
			for (auto des : descendants)
			{
					gPos += des->gmat().get_translation();
				speed += getJointSpeed(des, startTime, endTime);
			}
			gPos /= (float)(descendants.size()+1);
			speed /= (float)(descendants.size()+1);
			// filter for height
			if (gPos.y < floorHeight || gPos.y > (floorHeight + heightThreshold))
				continue;
			// filter speed
			if (speed <= speedThreshold)
			{
				vecTiming[jointId].emplace_back((float)f);
				vecPos[jointId].emplace_back(gPos); // also push back the joint's current position
			}
		}
	}	

	// merging the constraints
	float sceneScale = 1.f/(float)SBScene::getScene()->getScale();
	float Fmax = 10.f;
	float dmax = 0.1f * sceneScale;
	for (unsigned int i=0;i<footJoints.size();i++)
	{
		std::string jointName = footJoints[i];
		std::vector<float>& plantFrames = vecTiming[i];
		std::vector<SrVec>& plantPos = vecPos[i];
		if (plantFrames.size() == 0)
			continue;
		std::vector<float> mergeFrames;
		float curFrame = plantFrames[0];
		SrVec curPos = plantPos[0];
		mergeFrames.emplace_back(curFrame); // init the constraint
		for (unsigned int k=1;k<plantFrames.size();k++)
		{
			float nextFrame = plantFrames[k];
			SrVec nextPos = plantPos[k];
			float curDist = (nextPos - curPos).norm();
			float ftol = Fmax*exp(-curDist*log(Fmax)/dmax);
			if (ftol > fabs(nextFrame-curFrame)) // merge constraint
			{
				mergeFrames.emplace_back(nextFrame);
				curFrame = nextFrame;
				curPos = nextPos;
			}
			else // finalize the current constraint
			{
				FootStepRecord footPlant;
				footPlant.jointNames.emplace_back(jointName);
				SBJoint* joint = srcSk->getJointByName(jointName);				
				footPlant.startTime = mergeFrames[0]*(float)getFrameRate();
				footPlant.endTime   = mergeFrames[mergeFrames.size()-1]*(float)getFrameRate();	
				footStepRecords.emplace_back(footPlant);

				curFrame = nextFrame;
				curPos = nextPos;
				mergeFrames.clear();
				mergeFrames.emplace_back(curFrame);
			}
		}

		// push again if there are values in mergeFrames
		if (mergeFrames.size() > 0)
		{
			FootStepRecord footPlant;
			footPlant.jointNames.emplace_back(jointName);
			footPlant.startTime = mergeFrames[0]*(float)getFrameRate();
			footPlant.endTime   = mergeFrames[mergeFrames.size()-1]*(float)getFrameRate();	
			footStepRecords.emplace_back(footPlant);
		}		
	}
	this->disconnect();
	return true;
}


bool SBMotion::autoFootStepDetection( std::vector<double>& outMeans, int numStepsPerJoint, int maxNumSteps, SBSkeleton* skeleton, 
									  std::vector<std::string>& selectedJoints, float floorHeight, float floorThreshold, 
									  float speedThreshold, int speedWindow, bool isPrintDebugInfo  )
{
	std::vector<double> possibleTiming;

	// divided
	std::vector<std::vector<double> > vecOutMeans;
	vecOutMeans.resize(selectedJoints.size());
	std::vector<std::vector<double> > vecTiming;
	vecTiming.resize(selectedJoints.size());	
	this->connect(skeleton);
	for(int f = 0; f < frames(); f++)
	{
		this->apply_frame(f);
		this->connected_skeleton()->update_global_matrices();

		for (size_t jointId = 0; jointId < selectedJoints.size(); jointId ++)
		{
			std::string jointName = selectedJoints[jointId];
			SBJoint* joint = skeleton->getJointByName(jointName);
			if (!joint)
				continue;

			// get height
			const SrMat& gMat = joint->gmat();
			SrVec gPos = SrVec(gMat.get(12), gMat.get(13), gMat.get(14));

			// get speed
			int startFrame = f - speedWindow / 2;
			int endFrame = f + speedWindow / 2;
			float startTime = startFrame * (float)getFrameRate();
			float endTime = endFrame * (float)getFrameRate();
			float speed = getJointSpeed(joint, startTime, endTime);

			// print info
			if (isPrintDebugInfo)
				SmartBody::util::log("motion %s at time %f-> speed is %f, height is %f, joint is %s", getName().c_str(), f * getFrameRate(), speed, gPos.y, jointName.c_str());

			// filter for height
			if (gPos.y < floorHeight || gPos.y > (floorHeight + floorThreshold))
				continue;

			// filter speed
			if (speed <= speedThreshold)
			{
				vecTiming[jointId].emplace_back(f * (float)getFrameRate());
				possibleTiming.emplace_back(f * (float)getFrameRate());
			}
		}
	}	
	/*
	int numSteps = footStepEditor->stateEditor->getCurrentState()->getNumKeys();
	isConvergent = footStepEditor->kMeansClustering1D(numSteps, possibleTiming, outMeans);
	*/
	int maxSteps = maxNumSteps;
	int stepsPerJoint = numStepsPerJoint;
	bool isConvergent = true;
	for (size_t jointId = 0; jointId < selectedJoints.size(); jointId++)
	{
		if (jointId == (selectedJoints.size() - 1) )
		{
			int mod = maxSteps % selectedJoints.size();
			stepsPerJoint += mod;
		}

		bool retBoolean = kMeansClustering1D(stepsPerJoint, vecTiming[jointId], vecOutMeans[jointId]);
		if (!retBoolean)
		{
			isConvergent = false;
			break;
		}
	}
	if (isConvergent)
	{
		outMeans.clear();
		for (size_t joinId = 0; joinId < selectedJoints.size(); joinId++)
		{
			for (size_t meanId = 0; meanId < vecOutMeans[joinId].size(); meanId++)
				outMeans.emplace_back(vecOutMeans[joinId][meanId]);
		}
		std::sort(outMeans.begin(), outMeans.end());
	}

	// apply it to corresponding points
	// also appending starting and ending corresponding points
	/*
	int motionIndex = currentState->getMotionId(selectedMotions[m]);
	if (isConvergent)
	{
		std::stringstream ss;
		ss << "[" << motion->getName() << "]detected ";
		for (size_t i = 0; i < outMeans.size(); i++)
			ss << outMeans[i] << " ";
		SmartBody::util::log("%s", ss.str().c_str());
		finalMessage << ss.str() << "\n";
		currentState->keys[motionIndex].clear();
		if (footStepEditor->isProcessAll)
			currentState->keys[motionIndex].emplace_back(0);
		for (size_t i = 0; i < outMeans.size(); i++)
			currentState->keys[motionIndex].emplace_back(outMeans[i]);
		if (footStepEditor->isProcessAll)
			currentState->keys[motionIndex].emplace_back(motion->getDuration());
	}
	else
	{
		motionsNeedManualAdjusting.emplace_back(motion->getName());
		std::stringstream ss;
		ss << "[" << motion->getName() << "]NOT detected(evenly distrubted): ";
		int actualNum = maxNumSteps;
		currentState->keys[motionIndex].clear();
		if (footStepEditor->isProcessAll)
			actualNum += 2;
		for (int i = 0; i < actualNum; i++)
		{
			double step = motion->getDuration() / double(actualNum - 1);
			currentState->keys[motionIndex].emplace_back(step * i);
			ss << step * i << " ";
		}
		SmartBody::util::log("%s", ss.str().c_str());
		finalMessage << ss.str() << "\n";
	}
	*/
	disconnect();
	return isConvergent;

}

bool SBMotion::addMetaData( const std::string& tagName, const std::string& strValue )
{
	bool newData = false;
	if (tagAttrMap.find(tagName) == tagAttrMap.end())
	{		
		newData = true;		
	}
	tagAttrMap[tagName] = strValue;	
	return newData;
}

bool SBMotion::hasMetaData(const std::string& tagName)
{
	if (tagAttrMap.find(tagName) == tagAttrMap.end())
		return false;
	return true;
}

bool SBMotion::removeMetaData( const std::string& tagName )
{
	if (tagAttrMap.find(tagName) != tagAttrMap.end())
	{
		tagAttrMap.erase(tagName);
		return true;
	}
	else
	{
		SmartBody::util::log("Tag %s not found !", tagName.c_str());
	}
	return false;
}

double SBMotion::getMetaDataDouble( const std::string& tagName )
{
	std::string strValue = getMetaDataString(tagName);
#if defined(__ANDROID__) || defined(SB_IPHONE)
	return atof(strValue.c_str());
#elif __native_client__
	return atof(strValue.c_str());
#else
	return boost::lexical_cast<double>(strValue);
#endif
}

std::string SBMotion::getMetaDataString( const std::string& tagName )
{
	std::string strValue = "";
	if (tagAttrMap.find(tagName) != tagAttrMap.end())
	{
		strValue = tagAttrMap[tagName];		
	}
	else
	{
		SmartBody::util::log("Tag %s not found !", tagName.c_str());
	}
	return strValue;
}

std::vector<std::string> SBMotion::getMetaDataTags()
{
	std::vector<std::string> tagList;
	std::map<std::string, std::string >::iterator mi;
	for ( mi  = tagAttrMap.begin();
		  mi != tagAttrMap.end();
		  mi++)
	{
		tagList.emplace_back(mi->first);
	}
	return tagList;
}

SBAPI void SBMotion::setMotionSkeletonName( std::string skelName)
{
	_motionSkeleton = skelName;
}

SBAPI const std::string& SBMotion::getMotionSkeletonName()
{
	return _motionSkeleton;
}

SBAPI void SBMotion::buildJointTrajectory( const std::string& effectorName, const std::string& refJointName /*= "base" */ )
{
	auto motionSkel = SBScene::getScene()->getSkeleton(getMotionSkeletonName());
	if (!motionSkel)
	{
		SmartBody::util::log("Motion skeleton doesn't exist. Cannot compute effector trajectory");
		return;
	}
	SBSkeleton skelCopy(*motionSkel);
	if (!skelCopy.getJointByName(effectorName) || !skelCopy.getJointByName(refJointName))
	{
		SmartBody::util::log("Effector joint '%s' or reference joint '%s' does not exist.", effectorName.c_str(), refJointName.c_str());
	}
	auto traj = std::make_unique<JointTrajectory>();
	traj->effectorName = effectorName;
	traj->refJointName = refJointName;
	connect(&skelCopy);
	SmartBody::SBJoint* effector = skelCopy.getJointByName(effectorName); // find the global position of end effector
	SmartBody::SBJoint* refJoint = skelCopy.getJointByName(refJointName); // compute offset relative to ref joint
	SmartBody::SBJoint* baseJoint = skelCopy.getJointByName("base"); // use base rotation as local frame
	for (int i=0;i<getNumFrames();i++)
	{
		float keyTime = keytime(i);
		apply(keyTime);		
		skelCopy.update_global_matrices();
		SrVec effPos = effector->gmat().get_translation();
		SrVec refPos = refJoint->gmat().get_translation();
		SrMat baseRot = baseJoint->gmat().get_rotation();		
		SrVec trajPos = (effPos-refPos)*baseRot.inverse();
		traj->jointTrajectory.emplace_back(trajPos);
	}
	disconnect();	
	trajMap[effectorName] = std::move(traj);
}

SBAPI JointTrajectory* SBMotion::getJointTrajectory( const std::string& effectorName )
{
	if (trajMap.find(effectorName) == trajMap.end())
	{
		return nullptr;
	}	
	return trajMap[effectorName].get();
}


SBAPI bool SBMotion::getTrajPosition(const std::string& effectorName, float time, SrVec& outPos )
{	
	JointTrajectory* traj = getJointTrajectory(effectorName);
	if (!traj) return false;
	int f1,f2;
	float weight;
	bool validTime = getInterpolationFrames(time, f1,f2,weight);
	if (!validTime) return false;
	if (f1 >= (int) traj->jointTrajectory.size() || f2 >= (int) traj->jointTrajectory.size()) return false;
	
	outPos = traj->jointTrajectory[f1]*(1.0f - weight) + traj->jointTrajectory[f2]*weight;
	return true;
}

bool SBMotion::getInterpolationFrames( float time, int& f1, int& f2, float& weight )
{
	if ( time!= 0.f && time<=_frames[0].keytime )	{
		return false;
	}

	float dt = duration() / float(getNumFrames() - 1);
	int frameId = int(time / dt) % getNumFrames();	
	float t1,t2;
	if (frameId >= getNumFrames() -1 ) // last frame
	{
		time = _frames[getNumFrames()-1].keytime;		
		t1 = time;
		t2 = time+1.f;
		f1 = f2 = frameId;
	}
	else
	{
		t1 = _frames[frameId].keytime;	
		t2 = _frames[frameId+1].keytime;		
		f1 = frameId;
		f2 = frameId+1;
	}	
	weight = (time - t1)/(t2-t1);
	return true;	
}
bool motionComp(const SBMotion *a, const SBMotion *b)
{
	return a->getTransformDepth() < b->getTransformDepth();
}


void SBMotion::setEmptyMotion(float duration, int numFrames)
{
	if (duration <= 0.0 || numFrames <= 0)
	{
		SmartBody::util::log("Duration for motion %s should be > 0 and frames > 0", this->getName().c_str());
		return;
	}

	if (numFrames <= 2 && duration > 0)
	{
		this->insert_frame(0,0.f);
		this->insert_frame(1,(float)duration);
	}	
	else if (numFrames > 2 && duration > 0)
	{
		float deltaT = (float)duration/(numFrames-1);
		for (int i=0;i<numFrames;i++)
		{
			this->insert_frame(i,deltaT*i);			
		}
	}

}


SrQuat SBMotion::getChannelQuat( const std::string& channelName, float t )
{
	int idx = _channels.search(channelName, SkChannel::Quat);
	if (idx < 0) return SrQuat();
	int floatIdx = _channels.float_position(idx);
	if (floatIdx < 0) return SrQuat();

	int frame1 = getKeyFrameFromTime(t, 0, _frames.size()-1); // get closest key frame
	int frame2 = frame1+1; // next frame
	if (frame2 >= (int) _frames.size()-1) // last frame
	{
		frame2 = _frames.size()-1; 		
	}
	
	float* fp1 = _frames[frame1].posture;
	float* fp2 = _frames[frame2].posture;
	float keyDuration = (frame1 != frame2) ? _frames[frame2].keytime - _frames[frame1].keytime : 1.f;
	float interpWeight = (t - _frames[frame1].keytime)/keyDuration;

	float q[4];
	_channels[idx].interp(q,&fp1[floatIdx], &fp2[floatIdx], interpWeight);
	return SrQuat(q[0],q[1],q[2],q[3]);	
}


SrMat SBMotion::getChannelMat( const std::string& channelName, float t )
{
	SrQuat rot = getChannelQuat(channelName,t);
	SrVec  pos = getChannelPos(channelName,t);
	SrMat mat;
	rot.get_mat(mat);
	mat.set(12, pos[0]);
	mat.set(13, pos[1]);
	mat.set(14, pos[2]);

	return mat;
}


SrVec SBMotion::getChannelPos( const std::string& channelName, float t )
{
	SrVec pos;
	int frame1 = getKeyFrameFromTime(t, 0, _frames.size()-1); // get closest key frame
	int frame2 = frame1+1; // next frame
	if (frame2 >= (int) _frames.size()-1) // last frame
	{
		frame2 = _frames.size()-1; 		
	}

	float* fp1 = _frames[frame1].posture;
	float* fp2 = _frames[frame2].posture;
	float keyDuration = (frame1 != frame2) ? _frames[frame2].keytime - _frames[frame1].keytime : 1.f;
	float interpWeight = (t - _frames[frame1].keytime)/keyDuration;

	for (unsigned int i=0;i<3;i++)
	{
		int chanType = SkChannel::XPos + i; 	
		int idx = _channels.search(channelName, (SkChannel::Type) chanType);
		if (idx < 0) continue;
		int floatIdx = _channels.float_position(idx);
		if (floatIdx < 0) continue;
		float q[4];
		_channels[idx].interp(q,&fp1[floatIdx], &fp2[floatIdx], interpWeight);
		pos[i] = q[0];
	}
	return pos;
}

SrQuat SBMotion::getChannelQuatFrame(const std::string& channelName, int f)
{
	int idx = _channels.search(channelName, SkChannel::Quat);
	if (idx < 0)
	{
		SmartBody::util::log("Motion %s does not have channel %s.", this->getName().c_str(), channelName.c_str());
		return SrQuat();
	}
	int floatIdx = _channels.float_position(idx);
	if (floatIdx < 0)
	{
		SmartBody::util::log("Motion %s can't locate float position for channel %s.", this->getName().c_str(), channelName.c_str());
		return SrQuat();
	}

	if (f < 0 ||
		f >= this->getNumFrames())
	{
		SmartBody::util::log("Motion %s only has %d frames.", this->getName().c_str(), this->getNumFrames());
		return SrQuat();
	}

	float* p = _frames[f].posture;

	return SrQuat(p[idx], p[idx + 1], p[idx + 2], p[idx + 3]);
}

SrVec SBMotion::getChannelPosFrame(const std::string& channelName, int f)
{
	int idx = _channels.search(channelName, SkChannel::XPos);
	if (idx < 0)
	{
		SmartBody::util::log("Motion %s does not have channel %s.", this->getName().c_str(), channelName.c_str());
		return SrVec();
	}
	int floatIdx = _channels.float_position(idx);
	if (floatIdx < 0)
	{
		SmartBody::util::log("Motion %s can't locate float position for channel %s.", this->getName().c_str(), channelName.c_str());
		return SrVec();
	}

	if (f < 0 ||
		f >= this->getNumFrames())
	{
		SmartBody::util::log("Motion %s only has %d frames.", this->getName().c_str(), this->getNumFrames());
		return SrVec();
	}

	float* p = _frames[f].posture;

	return SrVec(p[idx], p[idx + 1], p[idx + 2]);
}

SrMat SBMotion::getChannelMatFrame(const std::string& channelName, int f)
{
	int idx = _channels.search(channelName, SkChannel::Quat);
	if (idx < 0)
	{
		SmartBody::util::log("Motion %s does not have channel %s.", this->getName().c_str(), channelName.c_str());
		return SrMat();
	}
	int floatIdx = _channels.float_position(idx);
	if (floatIdx < 0)
	{
		SmartBody::util::log("Motion %s can't locate float position for channel %s.", this->getName().c_str(), channelName.c_str());
		return SrMat();
	}

	if (f < 0 ||
		f >= this->getNumFrames())
	{
		SmartBody::util::log("Motion %s only has %d frames.", this->getName().c_str(), this->getNumFrames());
		return SrMat();
	}

	SrQuat rot = getChannelQuatFrame(channelName, f);
	SrVec  pos = getChannelPosFrame(channelName, f);
	SrMat mat;
	rot.get_mat(mat);
	mat.set(12, pos[0]);
	mat.set(13, pos[1]);
	mat.set(14, pos[2]);

	return mat;
}

void SBMotion::setChannelQuatFrame(const std::string& channelName, int frame, SrQuat quat)
{
	int idx = _channels.search(channelName, SkChannel::Quat);
	if (idx < 0)
	{
		SmartBody::util::log("Motion %s does not have channel %s.", this->getName().c_str(), channelName.c_str());
		return;
	}
	int floatIdx = _channels.float_position(idx);
	if (floatIdx < 0)
	{
		SmartBody::util::log("Motion %s can't locate float position for channel %s.", this->getName().c_str(), channelName.c_str());
		return;
	}

	if (frame < 0 ||
		frame >= this->getNumFrames())
	{
		SmartBody::util::log("Motion %s only has %d frames.", this->getName().c_str(), this->getNumFrames());
		return;
	}

	float* p = _frames[frame].posture;
	p[idx] = quat.w;
	p[idx + 1] = quat.x;
	p[idx + 2] = quat.y;
	p[idx + 3] = quat.z;
}

void SBMotion::setChannelPosFrame(const std::string& channelName, int frame, SrVec vec)
{
	int idx = _channels.search(channelName, SkChannel::XPos);
	if (idx < 0)
	{
		SmartBody::util::log("Motion %s does not have channel %s.", this->getName().c_str(), channelName.c_str());
		return;
	}
	int floatIdx = _channels.float_position(idx);
	if (floatIdx < 0)
	{
		SmartBody::util::log("Motion %s can't locate float position for channel %s.", this->getName().c_str(), channelName.c_str());
		return;
	}

	if (frame < 0 ||
		frame >= this->getNumFrames())
	{
		SmartBody::util::log("Motion %s only has %d frames.", this->getName().c_str(), this->getNumFrames());
		return;
	}

	float* p = _frames[frame].posture;

	p[idx] = vec.x;
	p[idx + 1] = vec.y;
	p[idx + 2] = vec.z;
}

void SBMotion::setChannelMatFrame(const std::string& channelName, int frame, SrMat mat)
{
	int idxq = _channels.search(channelName, SkChannel::Quat);
	if (idxq < 0)
	{
		SmartBody::util::log("Motion %s does not have channel %s.", this->getName().c_str(), channelName.c_str());
		return;
	}
	int floatIdxq = _channels.float_position(idxq);
	if (floatIdxq < 0)
	{
		SmartBody::util::log("Motion %s can't locate float position for channel %s.", this->getName().c_str(), channelName.c_str());
		return;
	}

	int idxt = _channels.search(channelName, SkChannel::XPos);
	if (idxt < 0)
	{
		SmartBody::util::log("Motion %s does not have channel %s.", this->getName().c_str(), channelName.c_str());
		return;
	}
	int floatIdxt = _channels.float_position(idxt);
	if (floatIdxt < 0)
	{
		SmartBody::util::log("Motion %s can't locate float position for channel %s.", this->getName().c_str(), channelName.c_str());
		return;
	}

	if (frame < 0 ||
		frame >= this->getNumFrames())
	{
		SmartBody::util::log("Motion %s only has %d frames.", this->getName().c_str(), this->getNumFrames());
		return;
	}

	float* p = _frames[frame].posture;
	p[floatIdxt] = mat[13];
	p[floatIdxt + 1] = mat[14];
	p[floatIdxt + 2] = mat[15];

	// convert mat to quat
	SrQuat quat(mat);
	p[floatIdxq] = quat.w;
	p[floatIdxq + 1] = quat.x;
	p[floatIdxq + 2] = quat.y;
	p[floatIdxq + 3] = quat.z;
}

// find the closest key frame with key time < t
int SBMotion::getKeyFrameFromTime( float t, int firstFrame, int lastFrame )
{
	int f1 = 0, f2 = 0;
	float weight;
	getInterpolationFrames(t,f1,f2,weight);
	return f1;
//
//  if (firstFrame+1>=lastFrame) // narrow down to the last frame
//    return firstFrame;  
//  unsigned int index = (firstFrame+lastFrame)/2;
//  if (_frames[index].keytime==t)
//    return index;
//  if (_frames[index].keytime < t)
//    return getKeyFrameFromTime(t, index, lastFrame);
//  return getKeyFrameFromTime(t, firstFrame, index);
}

void SBMotion::getAllChannelPos( const std::vector<std::string>& channeNames, float t, std::vector<SrVec>& outPosList )
{
	if (outPosList.size() != channeNames.size())
		outPosList.resize(channeNames.size());

	int frame1 = getKeyFrameFromTime(t, 0, _frames.size()-1); // get closest key frame
	int frame2 = frame1+1; // next frame
	if (frame2 >= (int) _frames.size()-1) // last frame
	{
		frame2 = _frames.size()-1; 		
	}

	float* fp1 = _frames[frame1].posture;
	float* fp2 = _frames[frame2].posture;
	float keyDuration = (frame1 != frame2) ? _frames[frame2].keytime - _frames[frame1].keytime : 1.f;
	float interpWeight = (t - _frames[frame1].keytime)/keyDuration;

	for (unsigned int idx=0;idx<channeNames.size();idx++)
	{
		const std::string channelName = channeNames[idx];
		outPosList[idx] = SrVec();
		SrVec& pos = outPosList[idx];
		for (int i=0;i<3;i++)
		{
			int chanType = SkChannel::XPos + i; 	
			int cidx = _channels.search(channelName, (SkChannel::Type) chanType);
			if (cidx < 0) continue;
			int floatIdx = _channels.float_position(cidx);
			if (floatIdx < 0) continue;
			float q[4];
			_channels[cidx].interp(q,&fp1[floatIdx], &fp2[floatIdx], interpWeight);
			pos[i] = q[0];
		}
	}
}

void SBMotion::getAllChannelQuat( const std::vector<std::string>& channeNames, float t, std::vector<SrQuat>& outQuatList )
{
	if (outQuatList.size() != channeNames.size())
		outQuatList.resize(channeNames.size());

	int frame1 = getKeyFrameFromTime(t, 0, _frames.size()-1); // get closest key frame
	int frame2 = frame1+1; // next frame
	if (frame2 >= (int) _frames.size()-1) // last frame
	{
		frame2 = _frames.size()-1; 		
	}

	float* fp1 = _frames[frame1].posture;
	float* fp2 = _frames[frame2].posture;
	float keyDuration = (frame1 != frame2) ? _frames[frame2].keytime - _frames[frame1].keytime : 1.f;
	float interpWeight = (t - _frames[frame1].keytime)/keyDuration;
	for (unsigned int i=0;i<channeNames.size();i++)
	{
		const std::string channelName = channeNames[i];
		outQuatList[i] = SrQuat();

		int idx = _channels.search(channelName, SkChannel::Quat);
		if (idx < 0) continue;
		int floatIdx = _channels.float_position(idx);
		if (floatIdx < 0) continue;

		float q[4];
		_channels[idx].interp(q,&fp1[floatIdx], &fp2[floatIdx], interpWeight);
		outQuatList[i] = SrQuat(q[0],q[1],q[2],q[3]);
	}
}

SBAPI bool SBMotion::downsample( int factor )
{
	
	if (factor <= 1) return false;
	int newNumFrames = getNumFrames()/factor;

	std::vector<Frame> newFrames;
	newFrames.resize(newNumFrames);
	for (int i=0;i<newNumFrames;i++)
	{
		int oldFrameIdx = i*factor;
		newFrames[i].keytime = _frames[oldFrameIdx].keytime;
		newFrames[i].posture = _frames[oldFrameIdx].posture;
	}
	_frames = newFrames;

	return true;
}

SBAPI void SBMotion::unrollPrerotation( const std::string& skelName )
{
	auto skel = SBScene::getScene()->getSkeleton(skelName);
	for (auto & _frame : _frames)
	{
		float* fbuffer = _frame.posture;
		for (int j=0;j<_channels.size();j++)
		{
			if (_channels[j].type == SkChannel::Quat)
			{
				SmartBody::SBJoint* joint = skel->getJointByMappedName(_channels.mappedName(j));
				if (joint)
				{
					int fidx = _channels.float_position(j);				
					SrQuat chanQuat = SrQuat(fbuffer[fidx], fbuffer[fidx+1], fbuffer[fidx+2], fbuffer[fidx+3]);
					SrQuat prerot = joint->getPrerotation();
					SrQuat newQuat = prerot.inverse()*chanQuat;
					newQuat.normalize();
					fbuffer[fidx] = newQuat.w;
					fbuffer[fidx+1] = newQuat.x;
					fbuffer[fidx+2] = newQuat.y;
					fbuffer[fidx+3] = newQuat.z;
				}
			}		
		}
	}
}


SBAPI void SBMotion::addTemporalRotationOffset( const std::string& chanName, SrQuat& startQuat, SrQuat& endQuat )
{
	int chanID = _channels.search(chanName, SkChannel::Quat); // search for the channel index	
	if (chanID == -1 || _frames.size() < 1) return;
	float ratio = 1.f/_frames.size();
	int fidx = _channels.float_position(chanID);
	for (size_t i=0;i<_frames.size();i++)
	{
		float* fbuffer = _frames[i].posture;
		SrQuat chanQuat = SrQuat(fbuffer[fidx], fbuffer[fidx+1], fbuffer[fidx+2], fbuffer[fidx+3]);
		SrQuat offsetQuat = slerp(startQuat,endQuat, ratio*(i+1));
		SrQuat newQuat = chanQuat*offsetQuat;
		fbuffer[fidx] = newQuat.w;
		fbuffer[fidx+1] = newQuat.x;
		fbuffer[fidx+2] = newQuat.y;
		fbuffer[fidx+3] = newQuat.z;
	}
}


SBMotion* SBMotion::copy(const std::string& motionName)
{
	SBMotion* newMotion = new SBMotion();
	newMotion->setName(motionName);

	SkChannelArray& ch = channels();
	int numFrames = frames();
	SmartBody::SBMotion* originalMotion = dynamic_cast<SmartBody::SBMotion*>(this);
	newMotion->setMotionSkeletonName(originalMotion->getMotionSkeletonName());
	std::string filebase = boost::filesystem::basename(this->getFullFilePath());
	
	newMotion->filename(filebase.c_str());
	newMotion->init(ch);
	//float* baseP = this->posture(0);
	for (size_t f = 0; f < this->_frames.size(); f++)
	{
		newMotion->insert_frame(f, this->_frames[f].keytime);
		float* ref_p = posture(f);
		float *new_p = newMotion->posture(f);
		memcpy(new_p, ref_p, sizeof(float)*posture_size());
	}
	newMotion->setSyncPoint("start", this->getTimeStart());
	newMotion->setSyncPoint("ready", this->getTimeReady());
	newMotion->setSyncPoint("stroke_start", this->getTimeStrokeStart());
	newMotion->setSyncPoint("stroke", this->getTimeStroke());
	newMotion->setSyncPoint("stroke_stop", this->getTimeStrokeEnd());
	newMotion->setSyncPoint("relax", this->getTimeRelax());
	newMotion->setSyncPoint("stop", this->getTimeStop());
	
	return newMotion;
}

bool SBMotion::speed(float factor)
{
	if (factor <= 0.0)
	{
		SmartBody::util::log("Cannot speed up motion '%s'to a factor of %f, ignoring...", this->getName().c_str(), factor);
		return false;
	}

	for (size_t f = 0; f < _frames.size(); f++)
	{
		_frames[f].keytime = _frames[f].keytime / factor;
	}
	
	// modify the syncpoints according to the speed factor

	this->setSyncPoint("start", getTimeStart() / factor);
	this->setSyncPoint("ready", getTimeReady() / factor);
	this->setSyncPoint("stroke_start", getTimeStrokeStart() / factor);
	this->setSyncPoint("stroke", getTimeStroke() / factor);
	this->setSyncPoint("stroke_stop", getTimeStrokeEnd() / factor);
	this->setSyncPoint("relax", getTimeRelax() / factor);
	this->setSyncPoint("stop", getTimeStop() / factor);

	return true;
}

/*
This function can be made generic later on
*/
SBMotion* SBMotion::buildPoststrokeHoldMotion(float holdTime, std::vector<std::string>& joints, float scale, float freq, SBMotion* idleMotion)
{
	bool insertIdleMotion = (idleMotion != nullptr) ? true : false;

	int numHoldFrames = int(holdTime * this->frames() / this->duration());
	int strokeEndFrameId = int(synch_points.get_time(srSynchPoints::STROKE_STOP) * this->frames() / this->duration());

	SkChannelArray& mchan_arr = this->channels();
	SBMotion* newMotion = new SmartBody::SBMotion();
	newMotion->synch_points.set_time(synch_points.get_time(srSynchPoints::START),
		synch_points.get_time(srSynchPoints::READY),
		synch_points.get_time(srSynchPoints::STROKE_START),
		synch_points.get_time(srSynchPoints::STROKE),
		synch_points.get_time(srSynchPoints::STROKE_STOP),
		synch_points.get_time(srSynchPoints::RELAX) + holdTime,
		synch_points.get_time(srSynchPoints::STOP) + holdTime
	);

	newMotion->init(mchan_arr);
	int num_f = this->frames();
	for (int i = 0; i < (num_f + numHoldFrames); ++i)
	{
		newMotion->insert_frame(i, float(i) * this->duration() / float(this->frames() - 1));

		int refPosId = i;
		bool inHoldingPeriod = false;
		if (i >= strokeEndFrameId && i <= (strokeEndFrameId + numHoldFrames))
		{
			if (insertIdleMotion)	// insert idle motion frames
			{
				int numIdleFrames = idleMotion->frames();
				refPosId = (i - strokeEndFrameId) % numIdleFrames;
			}
			else
				refPosId = strokeEndFrameId;
			inHoldingPeriod = true;
		}
		else if (i < strokeEndFrameId)
			refPosId = i;
		else
			refPosId = i - numHoldFrames;

		float* ref_p;
		if (inHoldingPeriod && insertIdleMotion)
			ref_p = idleMotion->posture(refPosId);
		else
			ref_p = this->posture(refPosId);

		float* new_p = newMotion->posture(i);
		memcpy(new_p, ref_p, sizeof(float)*posture_size());
	}
	std::vector<int> toSmoothIds;
	int wide = 5;
	for (int i = strokeEndFrameId - wide; i <= strokeEndFrameId + wide; i++)
		toSmoothIds.emplace_back(i);


	// handle the base joints
	if (strokeEndFrameId == 0)
	{
		SmartBody::util::log("SkMotion::buildPoststrokeHoldMotion ERR: please check if the stroke end time is set correctly!");
		return newMotion;
	}

	if (insertIdleMotion)
	{
		float *ref_p = newMotion->posture(strokeEndFrameId - 1);
		for (int i = strokeEndFrameId; i <= strokeEndFrameId + numHoldFrames; ++i)
		{
			for (int k = 0; k<mchan_arr.size(); k++)
			{
				SkChannel& chan = mchan_arr[k];
				const std::string& jointName = mchan_arr.name(k);
				int index = mchan_arr.float_position(k);
				bool isPos = chan.type == SkChannel::XPos;
				if (jointName == "base" && isPos)
				{
					newMotion->posture(i)[index] = ref_p[index];
					newMotion->posture(i)[index + 1] = ref_p[index + 1];
					newMotion->posture(i)[index + 2] = ref_p[index + 2];
					break;
				}
			}
		}
		newMotion->smoothAtFrame(strokeEndFrameId, 1, 5);
		newMotion->smoothAtFrame(strokeEndFrameId + numHoldFrames, 1, 5);
	}
	else if (joints.size() > 0)	// joint specified, add perlin noise
	{
		newMotion->addPerlinNoise(joints, strokeEndFrameId, strokeEndFrameId + numHoldFrames, scale, freq);
		//newMotion->smoothAtFrame(strokeEndFrameId, 3, 5);
		//newMotion->smoothAtFrame(strokeEndFrameId + numHoldFrames, 3, 5);
	}


	return newMotion;
}

/*
This function can be made generic later on
*/
SBMotion* SBMotion::buildPrestrokeHoldMotion(float holdTime, SBMotion* idleMotion)
{
	int numHoldFrames = int(holdTime * this->frames() / this->duration());
	int strokeStartFrameId = int(synch_points.get_time(srSynchPoints::STROKE_START) * this->frames() / this->duration());

	SkChannelArray& mchan_arr = this->channels();
	SBMotion* newMotion = new SmartBody::SBMotion();
	newMotion->synch_points.set_time(synch_points.get_time(srSynchPoints::START),
		synch_points.get_time(srSynchPoints::READY),
		synch_points.get_time(srSynchPoints::STROKE_START) + holdTime,
		synch_points.get_time(srSynchPoints::STROKE) + holdTime,
		synch_points.get_time(srSynchPoints::STROKE_STOP) + holdTime,
		synch_points.get_time(srSynchPoints::RELAX) + holdTime,
		synch_points.get_time(srSynchPoints::STOP) + holdTime
	);

	newMotion->init(mchan_arr);
	int num_f = this->frames();
	for (int i = 0; i < (num_f + numHoldFrames); ++i)
	{
		newMotion->insert_frame(i, float(i) * this->duration() / float(this->frames()));

		int refPosId = i;
		if (i >= strokeStartFrameId && i <= (strokeStartFrameId + numHoldFrames))
			refPosId = strokeStartFrameId;
		else if (i < strokeStartFrameId)
			refPosId = i;
		else
			refPosId = i - numHoldFrames;
		float *ref_p = this->posture(refPosId);
		float *new_p = newMotion->posture(i);
		memcpy(new_p, ref_p, sizeof(float)*posture_size());
	}
	return newMotion;
}

bool SBMotion::isGestureSpeedCalculated()
{
	return _isGestureSpeedCalculated;

}

void SBMotion::calculateGestureSpeed()
{
	_isGestureSpeedCalculated = false;
	_isGestureSpeedValid = true;
	_startLocation = SrVec(0, 0, 0);
	_holdLocation = SrVec(0, 0, 0);
	std::string skeletonName = this->getMotionSkeletonName();
	if (skeletonName == "")
	{
		SmartBody::util::log("Gesture %s has motion skeleton, cannot calculate gesture speed...", this->getName().c_str());
		return;
	}
	auto skeleton = SmartBody::SBScene::getScene()->createSkeleton(skeletonName);
	if (skeleton == nullptr)
	{
		SmartBody::util::log("Gesture %s has motion skeleton %s but does not exist, cannot calculate gesture speed...", this->getName().c_str(), skeletonName.c_str());
		return;
	}

	this->connect(skeleton.get());
	SBJoint* lWrist = skeleton->getJointByMappedName("l_wrist");
	SBJoint* rWrist = skeleton->getJointByMappedName("r_wrist");
	if (!lWrist || !rWrist)
	{
		// no wrists to check for speed, so can't do coarticulation
		SmartBody::util::log("Gesture %s has no wrist information to test for coarticulation, cannot calculate gesture speed...", this->getName().c_str());
		this->disconnect();
		return;
	}

	float startTime = (float) this->time_stroke_start();
	float endTime = (float) this->time_stroke_end();
	float gestureTime = endTime - startTime;
	if (fabs(gestureTime) < .001 || gestureTime < 0.0)
	{
		SmartBody::util::log("Gesture %s has no stroke speed, cannot calculate speed...", this->getName().c_str());
		this->disconnect();
		_isGestureSpeedValid = false;
		return;
	}

	SrVec lWristPosStart = this->getJointPosition(lWrist, startTime);
	SrVec rWristPosStart = this->getJointPosition(rWrist, startTime);

	SrVec lWristPosEnd = this->getJointPosition(lWrist, endTime);
	SrVec rWristPosEnd = this->getJointPosition(rWrist, endTime);
				
	this->disconnect();
				
	SrVec leftDistanceVec = lWristPosEnd - lWristPosStart;
	SrVec rightDistanceVec = rWristPosEnd - rWristPosStart;

	double leftDistance = leftDistanceVec.len(); 
	double leftSpeed = leftDistance / gestureTime;
	double rightDistance = rightDistanceVec.len();
	double rightSpeed = rightDistance / gestureTime;

	if (leftSpeed > rightSpeed)
	{
		_gestureSpeed = leftSpeed;
		_holdLocation = lWristPosEnd;
		_startLocation = lWristPosStart;
	}
	else
	{
		_gestureSpeed = rightSpeed;
		_holdLocation = rWristPosEnd;
		_startLocation = rWristPosStart;
	}

	_isGestureSpeedCalculated = true;
	_isGestureSpeedValid = true;
}

double SBMotion::getGestureSpeed()
{
	return _gestureSpeed;
}

void SBMotion::setGestureSpeed(double speed)
{
	_gestureSpeed = speed;
	_isGestureSpeedCalculated = true;
	_isGestureSpeedValid = true;
}

SrVec SBMotion::getGestureHoldLocation()
{
	return _holdLocation;
}

SrVec SBMotion::getGestureStartLocation()
{
	return _startLocation;
}

void SBMotion::setGestureHoldLocation(SrVec vec)
{
	_holdLocation = vec;
}

void SBMotion::setGestureStartLocation(SrVec vec)
{
	_startLocation = vec;
}


};
