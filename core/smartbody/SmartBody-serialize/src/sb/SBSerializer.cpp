/*************************************************************
Copyright (C) 2020 Erik Ogenvik <erik@ogenvik.org>

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

#include "SBSerializer.h"
#include "sb/SBMotion.h"
#include "protocols/sbmotion.pb.h"
#include "SBUtilities.h"
#include "sb/SBSkeleton.h"
#include "sb/SBJoint.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <fstream>
#include <sr/sr_euler.h>

namespace SmartBody {

void SBSerializer::saveToSkm(SBMotion& motion, const std::string& fileName)
{
	SrOutput out(fileName.c_str(), "w");
	if (!out.valid())
	{
		SmartBody::util::log("Cannot write to file %s", fileName.c_str());
		return;
	}

	boost::filesystem::path p(fileName);
	std::string fname = boost::filesystem::basename(p);
	std::string extension = boost::filesystem::extension(p);

	std::string mname = fname + extension;
	motion.setName(mname);
	motion.save(out);

}

void SBSerializer::saveToSkb(SBMotion& motion, const std::string& fileName)
{

	std::string sName;
	int sNumChannels;
	std::vector<std::string> sChannelNames;
	std::vector<int> sChannelTypes;
	int sFrames;
	std::vector<float> sKeyTimes;
	std::vector<std::vector<float> > sKeyValues;
	std::vector<std::string> sMetaDataNames;
	std::vector<std::string> sMetaDataValues;
	std::vector<float> sSyncPoints;

	// write the data to the stream variables
	sName = motion.getName();

	sNumChannels = motion._channels.size();
	for (int c = 0; c < motion._channels.size(); c++)
	{
		sChannelNames.emplace_back(motion._channels.name(c));
		sChannelTypes.emplace_back(motion._channels.type(c));
	}

	sFrames = motion._frames.size();

	int size = motion._channels.floats();

	for (int f = 0; f < sFrames; f++)
	{
		float time = motion._frames[f].keytime;
		sKeyTimes.emplace_back(time);
		sKeyValues.emplace_back(std::vector<float>() );
		std::vector<float>& data = sKeyValues[f];
		for (int p = 0; p < size; p++)
		{
			data.emplace_back(motion._frames[f].posture[p]);
		}
	}

	std::vector<std::string> tags = motion.getMetaDataTags();
	for (auto & tag : tags)
	{
		std::string tagValue = motion.getMetaDataString(tag);
		sMetaDataNames.emplace_back(tag);
		sMetaDataValues.emplace_back(tagValue);
	}
	sSyncPoints.emplace_back((float) motion.synch_points.get_time( srSynchPoints::START ) );
	sSyncPoints.emplace_back((float) motion.synch_points.get_time( srSynchPoints::READY ) );
	sSyncPoints.emplace_back((float) motion.synch_points.get_time( srSynchPoints::STROKE_START ) );
	sSyncPoints.emplace_back((float) motion.synch_points.get_time( srSynchPoints::STROKE ) );
	sSyncPoints.emplace_back((float) motion.synch_points.get_time( srSynchPoints::STROKE_STOP ) );
	sSyncPoints.emplace_back((float) motion.synch_points.get_time( srSynchPoints::RELAX ) );
	sSyncPoints.emplace_back((float) motion.synch_points.get_time( srSynchPoints::STOP ) );



	SmartBodyBinary::Motion outputMotion;
	outputMotion.set_name(sName);
	outputMotion.set_numchannels(sNumChannels);
	for (int i = 0; i < sNumChannels; ++i)
	{
		outputMotion.add_channelname(sChannelNames[i]);
		outputMotion.add_channeltype(sChannelTypes[i]);
	}
	outputMotion.set_numframes(sFrames);
	for (int i = 0; i < sFrames; ++i)
	{
		outputMotion.add_keytimes(sKeyTimes[i]);
	}
	for (int i = 0; i < sFrames; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			outputMotion.add_keyvalues(sKeyValues[i][j]);
		}
	}
	for (float sSyncPoint : sSyncPoints)
		outputMotion.add_syncpoints(sSyncPoint);
	for (size_t i = 0; i < sMetaDataNames.size(); ++i)
	{
		SmartBodyBinary::Motion_MetaData* newMetaData = outputMotion.add_metadata();
		newMetaData->set_metadataname(sMetaDataNames[i]);
		newMetaData->set_metadatavalue(sMetaDataValues[i]);
	}

	std::fstream file(fileName.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!outputMotion.SerializeToOstream(&file))
	{
		SmartBody::util::log("Fail to write to binary file %s", fileName.c_str());
	}
	google::protobuf::ShutdownProtobufLibrary();

}

bool SBSerializer::readFromSkb(SBMotion& motion, const std::string& fileName)
{
	SmartBodyBinary::Motion inputMotion;
	std::fstream input(fileName.c_str(), std::ios::in | std::ios::binary);
	if (!inputMotion.ParseFromIstream(&input))
	{
		SmartBody::util::log("Failed to parse binary motion from file %s", fileName.c_str());
		return false;
	}
	motion._filename = fileName;

	// read the data
	motion.setName(inputMotion.name());

	for (int c = 0; c < inputMotion.numchannels(); c++)
	{
		motion._channels.add(inputMotion.channelname(c), (SkChannel::Type) inputMotion.channeltype(c));
	}

	motion._postsize = motion._channels.floats();
	int postureSize = motion._postsize;

	motion._frames.resize(inputMotion.numframes());
	for (int f = 0; f < inputMotion.numframes(); f++)
	{
		motion._frames[f].keytime = inputMotion.keytimes(f);
	}
	for (int f = 0; f < inputMotion.numframes(); f++)
	{
		motion._frames[f].posture.resize(postureSize);
		for (int p = 0; p < postureSize; p++)
		{
			motion._frames[f].posture[p] = inputMotion.keyvalues(f * postureSize + p);
		}
	}

	motion.synch_points.set_time( srSynchPoints::START, inputMotion.syncpoints(0) );
	motion.synch_points.set_time( srSynchPoints::READY, inputMotion.syncpoints(1) );
	motion.synch_points.set_time( srSynchPoints::STROKE_START, inputMotion.syncpoints(2) );
	motion.synch_points.set_time( srSynchPoints::STROKE, inputMotion.syncpoints(3) );
	motion.synch_points.set_time( srSynchPoints::STROKE_STOP, inputMotion.syncpoints(4) );
	motion.synch_points.set_time( srSynchPoints::RELAX, inputMotion.syncpoints(5) );
	motion.synch_points.set_time( srSynchPoints::STOP, inputMotion.syncpoints(6) );

	for (int t = 0; t < inputMotion.metadata_size(); t++)
	{
		motion.addMetaData(inputMotion.metadata(t).metadataname(), inputMotion.metadata(t).metadatavalue());
	}

	return true;
}


void SBSerializer::saveToSkmByFrames(SBMotion& motion, const std::string& fileName, int startFrame, int endFrame)
{
	SBMotion newMotion;
	SkChannelArray& ch = motion.channels();
	int numFrames = motion.frames();
	newMotion.setMotionSkeletonName(motion.getMotionSkeletonName());
	std::string filebase = boost::filesystem::basename(fileName);
	newMotion.setName(filebase);
	newMotion.filename(filebase.c_str());
	newMotion.init(ch);
	//float* baseP = motion.posture(0);
	for (int f = 0; f < (endFrame - startFrame + 1); f++)
	{
		newMotion.insert_frame(f, motion.keytime(f + startFrame) - motion.keytime(startFrame));
		float* ref_p = motion.posture(f + startFrame);
		float *new_p = newMotion.posture(f);
		memcpy(new_p,ref_p,sizeof(float)*motion.posture_size());
	}
	newMotion.setSyncPoint("start", 0);
	newMotion.setSyncPoint("ready", newMotion.duration() * 0.25);
	newMotion.setSyncPoint("stroke_start", newMotion.duration() * 0.5);
	newMotion.setSyncPoint("stroke", newMotion.duration() * 0.5);
	newMotion.setSyncPoint("stroke_stop", newMotion.duration() * 0.5);
	newMotion.setSyncPoint("relax", newMotion.duration() * 0.75);
	newMotion.setSyncPoint("stop", newMotion.duration());
	saveToSkm(newMotion, fileName);
}


void SBSerializer::saveToBVH(SBMotion& motion, const std::string& fileName, SBSkeleton& skel )
{
//	auto skel = SmartBody::SBScene::getScene()->getSkeleton(skelName);
//	if (!skel)
//	{
//		SmartBody::util::log("Error : skeleton '%s' does not exist.", skelName.c_str());
//		return;
//	}

	FILE* fp = fopen(fileName.c_str(),"wt");
	skel.updateJointMap();
	SmartBody::SBJoint* root = dynamic_cast<SmartBody::SBJoint*>(skel.root());
	std::vector<int> jointIdxs;
	fprintf(fp,"HIERARCHY\n");
	printHierarchy(fp,root,0,jointIdxs);

	// write buffers
	fprintf(fp,"MOTION\n");
	fprintf(fp,"Frames: %d\n",motion.getNumFrames());
	fprintf(fp,"Frame Time: %f\n",motion.getFrameRate());
	//SmartBody::util::log("Joint idxs size = %d", jointIdxs.size());

	for (int i=0;i<motion.getNumFrames();i++)
	{
		float* frameBuf = motion.posture(i);
		for (int jointIdx : jointIdxs)
		{
			SmartBody::SBJoint* joint = skel.getJoint(jointIdx);
			SrVec pos = joint->offset();
			for (int c=0;c<3;c++)
			{
				int chanID = motion._channels.search(joint->getMappedJointName(), (SkChannel::Type)(SkChannel::XPos+c));
				if (chanID != -1)
				{
					pos[c] = frameBuf[motion._channels.float_position(chanID)];
				}
			}

			SrQuat quat = joint->quat()->orientation()*joint->quat()->prerot()*joint->quat()->postrot(); // bake all joint pre-post rotations into motion first
			int quatID = motion._channels.search(joint->getMappedJointName(), SkChannel::Quat);
			if (quatID != -1)
			{
				//pos[c] = frameBuf[_channels.float_position(chanID)];
				int fidx = motion._channels.float_position(quatID);
				// bake the quaternion based on joint orientation/pre/post-rotation since BVH can't store pre-rotation in the skeleton
				//quat = SrQuat(frameBuf[fidx], frameBuf[fidx + 1], frameBuf[fidx + 2], frameBuf[fidx + 3]);
				quat = joint->quat()->orientation()*joint->quat()->prerot()*SrQuat(frameBuf[fidx], frameBuf[fidx+1], frameBuf[fidx+2], frameBuf[fidx+3])*joint->quat()->postrot();
			}
			SrMat rotMat;
			quat.get_mat(rotMat);
			float ex, ey, ez;
			sr_euler_angles_yxz( rotMat, ex, ey, ez );
// 			if (i == 0)
// 			{
// 				SmartBody::util::log("Joint name = %s, euler angle = %f %f %f", joint->getMappedJointName().c_str(), ex*57.295779513082323, ey*57.295779513082323, ez*57.295779513082323);
// 			}
			fprintf(fp," %f %f %f %f %f %f",pos[0],pos[1],pos[2], ez*57.295779513082323, ex*57.295779513082323, ey*57.295779513082323);
		}
		fprintf(fp,"\n");
	}

	fclose(fp);
}

void SBSerializer::printHierarchy(FILE* file, SmartBody::SBJoint* joint, int depth, std::vector<int>& jointIdxs)
{
	int i;

	if( joint == nullptr )	{
		return;
	}

	int num_child = joint->getNumChildren();
	if (num_child == 0)
	{
		fprintf(file, "End Site\n");
		//print_tabs( depth + 1 );
		//*_record_output << "{\n";
		fprintf(file, "{\n");

		// End Site OFFSET not used
		// This is the geometric vector of the final bone segment
		//print_tabs( depth + 2 );
		//*_record_output << "OFFSET 0.0 0.0 0.0\n";
		SrVec offset_v = joint->offset();

		fprintf(file, "OFFSET %f %f %f \n", offset_v.x,offset_v.y,offset_v.z);
		//fprintf(file, "OFFSET 0.0 0.0 0.0\n");

		//print_tabs( depth + 1 );
		//*_record_output << "}\n";
		fprintf(file,"}\n");
	}
	else
	{
		//print_tabs( depth );
		if( depth == 0 )	{
			//*_record_output << "ROOT " << joint_p->jointName().c_str() << "\n";
			fprintf(file,"ROOT %s\n",joint->getMappedJointName().c_str());
		}
		else	{
			//*_record_output << "JOINT " << joint_p->jointName().c_str() << "\n";
			fprintf(file,"JOINT %s\n",joint->getMappedJointName().c_str());
		}

		jointIdxs.emplace_back(joint->index());
		//print_tabs( depth );
		//*_record_output << "{\n";
		fprintf(file, "{\n");

		//print_tabs( depth + 1 );
		//*_record_output << "OFFSET ";
		fprintf(file,"OFFSET ");

		// STUPID-POLYTRANS ignores ROOT OFFSET: added to CHANNEL motion
		if( depth == 0 )	{
			//*_record_output << "0.0 0.0 0.0 \n";
			fprintf(file,"0.0 0.0 0.0 \n");
		}
		else	{
			SrVec offset_v = joint->offset();

			fprintf(file, "%f %f %f \n", offset_v.x,offset_v.y,offset_v.z);
			// STUPID-POLYTRANS subtracts OFFSET instead of adds
		}

		// CHANNELS:
		// Optimize: check
		//   SkJointQuat::_active, and
		//   SkJointPos:SkVecLimits::frozen()
		//print_tabs( depth + 1 );
		//*_record_output << "CHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n";
		fprintf(file, "CHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n");

		{
			for( i = 0; i < num_child; i++ )	{
				//SkJoint* child_p = joint_p->child( i );
				SmartBody::SBJoint* child_p = joint->getChild(i);
				//print_bvh_hierarchy( child_p, depth + 1 );
				printHierarchy(file,child_p, depth+1, jointIdxs);
			}
		}
		fprintf(file,"}\n");
	}

// 	if( num_child == 0 )	{
//
// 		//print_tabs( depth + 1 );
// 		//*_record_output << "End Site\n";
// 		fprintf(file, "End Site\n");
// 		//print_tabs( depth + 1 );
// 		//*_record_output << "{\n";
// 		fprintf(file, "{\n");
//
// 		// End Site OFFSET not used
// 		// This is the geometric vector of the final bone segment
// 		//print_tabs( depth + 2 );
// 		//*_record_output << "OFFSET 0.0 0.0 0.0\n";
// 		fprintf(file, "OFFSET 0.0 0.0 0.0\n");
//
// 		//print_tabs( depth + 1 );
// 		//*_record_output << "}\n";
// 		fprintf(file,"}\n");
// 	}
// 	else

	//print_tabs( depth );
	//*_record_output << "}\n";

}

}