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

#include "SBSkeleton.h"
#include "SBJoint.h"
#include "SBPawn.h"
#include "SBAttribute.h"

#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBAssetManager.h>
#include <sb/SBSceneListener.h>
#include "SBUtilities.h"
#include <sr/sr_string.h>
#include <controllers/me_controller_tree_root.hpp>

namespace SmartBody {

SBSkeleton::SBSkeleton(SBScene& scene) : SkSkeleton(), SBSceneOwned(scene)
{
	_scale = 1.0f;
	_origRootChanged = false;
	createVec3Attribute("orientation", 0, 0, 0, true, "Basic", 100, false, false, false, "Change in orientation from initial loading of skeleton. Parameters are rotation in degrees of the X, then Y, then Z axes.");
	linkedPawnName = "";
}

SBSkeleton::SBSkeleton(SBScene& scene, const std::string& skelFile) : SkSkeleton(), SBSceneOwned(scene)
{
	_scale = 1.0f;
	_origRootChanged = false;
	createVec3Attribute("orientation", 0, 0, 0, true, "Basic", 100, false, false, false, "Change in orientation from initial loading of skeleton. Parameters are rotation in degrees of the X, then Y, then Z axes.");
	load(skelFile);
	linkedPawnName = "";
}

SBSkeleton::SBSkeleton(const SBSkeleton& rhs) : SkSkeleton(rhs), SBSceneOwned(rhs)
{
	_scale = rhs.getScale();
	_origRootChanged = false;
	linkedPawnName = "";
	//jointMap = copySkel->getJointMapName();
}

SBSkeleton::~SBSkeleton() = default;

SBSkeleton& SBSkeleton::operator=(const SBSkeleton& rhs){
	SkSkeleton::operator=(rhs);
	_scale = rhs._scale;
	_origRootChanged = rhs._origRootChanged;
	linkedPawnName = rhs.linkedPawnName;
	return *this;
}


SBAPI SBJoint* SBSkeleton::createChannel(const std::string& name)
{
	if (this->getJointByName(name))
	{
		SmartBody::util::log("Joint or channel %s already created.", name.c_str());
		return nullptr;
	}

	SmartBody::SBJoint* rootJoint = this->getJoint(0);
	if (!rootJoint)
	{
		SmartBody::util::log("Root joint not present. Please add root joint before adding channels.");
		return nullptr;
	}

	SBJoint* joint = new SBJoint();
	joint->setName(name);
	joint->name(name);
	joint->extName(name);
	joint->setJointType(SkJoint::TypeViseme);
	joint->skeleton(this);
	joint->setUsePosition(0, true);
	joint->pos()->limits(SkJointPos::X, 0, 200);
	
	rootJoint->addChild(joint);
	joint->setParent(rootJoint);
	
	_joints.emplace_back(joint);
	joint->index(_joints.size() - 1);

	if (this->getPawn())
	{
		this->getPawn()->ct_tree_p->child_channels_updated(nullptr);
		this->getPawn()->ct_tree_p->applySkeletonToBuffer();
	}
	
	return joint;
}

SBAPI SBJoint* SBSkeleton::createJoint(const std::string& name, SBJoint* parent)
{
	if (this->getJointByName(name))
	{
		SmartBody::util::log("Joint %s already created.", name.c_str());
		return nullptr;
	}
	SBJoint* joint = new SBJoint();
	joint->setName(name);
	joint->name(name);
	joint->extName(name);
	joint->skeleton(this);
	if (!parent) // enable translation only for the root joint
	{
		joint->rot_type ( SkJoint::TypeQuat );
		joint->quat()->activate();
		joint->pos()->limits(0, false);
		joint->pos()->limits(1, false);
		joint->pos()->limits(2, false);
		joint->pos()->value(0, 0);
		joint->pos()->value(1, 0);
		joint->pos()->value(2, 0);
	}
	else
	{
		joint->rot_type ( SkJoint::TypeQuat );
		joint->quat()->activate();
		joint->pos()->limits(0, false);
		joint->pos()->limits(1, false);
		joint->pos()->limits(2, false);
		joint->pos()->value(0, 0);
		joint->pos()->value(1, 0);
		joint->pos()->value(2, 0);
	}

	int parentId = -1;
	if (parent)
	{
		joint->setParent(parent);
		parent->addChild(joint);
	}
	else
	{
		// get the existing root joint and make it a child of the new root joint
		SkJoint* oldRoot = this->root();
		
		if (oldRoot)
		{
			joint->add_child(oldRoot);
		}
		this->root(joint);
		
	}
	
	_joints.emplace_back(joint);
	joint->index(_joints.size() - 1);

	if (this->getPawn())
	{
		this->getPawn()->ct_tree_p->child_channels_updated(nullptr);
		this->getPawn()->ct_tree_p->applySkeletonToBuffer();
	}
	
	return joint;
}

SBAPI SBJoint* SBSkeleton::createStaticJoint(const std::string& name, SBJoint* parent)
{
	if (this->getJointByName(name))
	{
		SmartBody::util::log("Joint %s already created.", name.c_str());
		return nullptr;
	}
	SBJoint* joint = new SBJoint();
	joint->setName(name);
	joint->name(name);
	joint->extName(name);
	joint->skeleton(this);
	if (!parent) // enable translation only for the root joint
	{
		joint->rot_type ( SkJoint::TypeQuat );
		//joint->quat()->activate();
		joint->pos()->limits(0, false);
		joint->pos()->limits(1, false);
		joint->pos()->limits(2, false);
		joint->pos()->value(0, 0);
		joint->pos()->value(1, 0);
		joint->pos()->value(2, 0);
	}
	else
	{
		joint->rot_type ( SkJoint::TypeQuat );
		//joint->quat()->activate();
		joint->pos()->limits(0, false);
		joint->pos()->limits(1, false);
		joint->pos()->limits(2, false);
		joint->pos()->value(0, 0);
		joint->pos()->value(1, 0);
		joint->pos()->value(2, 0);
	}

	int parentId = -1;
	if (parent)
	{
		joint->setParent(parent);
		parent->addChild(joint);
	}
	else
	{
		this->root(joint);
	}
	
	_joints.emplace_back(joint);
	joint->index(_joints.size() - 1);

	if (this->getPawn())
	{
		this->getPawn()->ct_tree_p->child_channels_updated(nullptr);
		this->getPawn()->ct_tree_p->applySkeletonToBuffer();
	}
	
	return joint;
}

void SBSkeleton::setFileName(const std::string& fname)
{
	skfilename(fname.c_str());
}

const std::string& SBSkeleton::getFileName()
{
	return skfilename();
}

bool SBSkeleton::load(const std::string& skeletonFile)
{
	auto skeleton = _scene.getAssetManager()->getSkeleton(skeletonFile);
	if (skeleton)
	{
		*this = *skeleton;
		update();
		return true;
	}
	else
	{
		FILE *fp = nullptr;
		fp = fopen(skeletonFile.c_str(), "rt");
		if (fp == nullptr )
		{
			SmartBody::util::log("No skeleton found for file %s.", skeletonFile.c_str());
			return false;
		}
		SrInput input(fp);
		if (input.valid())
		{
			input.filename(skeletonFile.c_str());
			if (!SkSkeleton::loadSk(input, 1.0f))
			{
				SmartBody::util::log("Problem loading skeleton from file %s.", skeletonFile.c_str());
				return false;
			} 
			else
			{
				update();
				return true;
			}
		}
		else
		{
			SmartBody::util::log("No skeleton found for file %s.", skeletonFile.c_str());
			return false;
		}
	}
}

bool SBSkeleton::save(const std::string& skeletonFile)
{
	FILE *fp = nullptr;
	fp = fopen(skeletonFile.c_str(), "w");
	if (fp == nullptr )
	{
		SmartBody::util::log("Can't create skeleton file %s.", skeletonFile.c_str());
		return false;
	}
	SrOutput output(fp);
	if (output.valid())
	{
		output.filename(skeletonFile.c_str());
		if (!SkSkeleton::save(output))
		{
			SmartBody::util::log("Problem saving skeleton to file %s.", skeletonFile.c_str());
			return false;
		} 
		else
		{
			SmartBody::util::log("Skeleton saved to file %s.", skeletonFile.c_str());
			return true;
		}
	}
	else
	{
		SmartBody::util::log("Problem saving skeleton to file %s.", skeletonFile.c_str());
		return false;
	}
}

std::string SBSkeleton::saveToString()
{
	std::string retString = "";
	SkSkeleton* myskel = dynamic_cast<SkSkeleton*>(this);
	SrString outputString;
	SrOutput output(outputString);
	if (myskel->save(output))
	{
		retString = (const char*)outputString;
	}
	return retString;
}

void SBSkeleton::loadFromString(const std::string& info )
{
	//SmartBody::util::log("loadFromString = %s",info.c_str());
	SrInput input(info.c_str());	
	SkSkeleton::loadSk(input);
}

int SBSkeleton::getNumJoints()
{
	return joints().size();
}

SBJoint* SBSkeleton::getJointByName(const std::string& jointName)
{
	return getJointByMappedName(jointName);
}

SBJoint* SBSkeleton::getJointByMappedName(const std::string& jointName)
{
	SkJoint* j = search_joint(jointName.c_str());
	if (j)
	{
		SBJoint* sbJoint = dynamic_cast<SBJoint*>(j);
		return sbJoint;
	}
	else
	{
		return nullptr;
	}
}


std::vector<std::string> SBSkeleton::getJointMappedNames()
{
	std::vector<std::string> jointNames;
	auto& alljoints = joints();
	for (auto& alljoint : alljoints)
	{
		jointNames.emplace_back(alljoint->getMappedJointName());
	}
	return jointNames;
}

std::vector<std::string> SBSkeleton::getJointNames()
{
	std::vector<std::string> jointNames;
	auto& alljoints = joints();
	for (auto& alljoint : alljoints)
	{
		jointNames.emplace_back(alljoint->jointName());
	}
	return jointNames;
}

std::vector<std::string> SBSkeleton::getJointOriginalNames()
{
	std::vector<std::string> jointNames;
	auto& alljoints = joints();
	for (auto& alljoint : alljoints)
	{
		jointNames.emplace_back(alljoint->extName());
	}
	return jointNames;
}

std::vector<std::string> SBSkeleton::getUpperBodyJointNames()
{
	std::vector<std::string> jointNames;
	SkJoint* spine1 = search_joint("spine1");
	if (!spine1)
	{
		SmartBody::util::log("SBSkeleton::getUpperBodyJointNames WARNING: cannot find spine1");
		return std::vector<std::string>();
	}
	std::vector<SkJoint*> alljoints;
	SkJoint::recursive_children(alljoints, spine1);
	for (auto & alljoint : alljoints)
		jointNames.emplace_back(alljoint->jointName());

	return jointNames;
}

SBJoint* SBSkeleton::getJoint(int index)
{
	auto& alljoints = joints();
	if (index >=0 && size_t(index) < alljoints.size())
	{
		SBJoint* sbJoint = dynamic_cast<SBJoint*>(alljoints[index].get());
		return sbJoint;
	}
	else
	{
		return nullptr;
	}
}

int SBSkeleton::getNumChannels()
{
	return channels().size();
}

std::string SBSkeleton::getChannelType(int index)
{
	if (index > 0 && index < channels().size())
	{
		return SkChannel::type_name(channels()[index].type);
	}
	else
	{
		return "";
	}
}

int SBSkeleton::getChannelSize(int index)
{
	if (index > 0 && index < channels().size())
	{
		return channels()[index].size();
	}
	else
	{
		return 0;
	}

}

SBPawn* SBSkeleton::getPawn()
{
	// determine which character uses this skeleton
	// NOTE: there should be back pointer between the skeleton and the pawn/character
	SmartBody::SBPawn* skelPawn = _scene.getPawn(linkedPawnName);
	if (skelPawn)
		return skelPawn;

	const std::vector<std::string>& pawns = _scene.getPawnNames();
	for (const auto & pawnIter : pawns)
	{
		SBPawn* pawn = _scene.getPawn(pawnIter);
		if (pawn->getSkeleton() == this)
		{
			setPawnName(pawn->getName());
			return pawn;
		}
}

	return nullptr;
}

void SBSkeleton::update()
{
	refresh_joints();
	make_active_channels();
	compress();
	updateGlobalMatricesZero();
	updateJointMap();
		
	SBPawn* pawn = getPawn();
	SBCharacter* character = dynamic_cast<SBCharacter*>(pawn);
	if (character)
	{

		std::vector<SBSceneListener*>& listeners = _scene.getSceneListeners();
		for (auto & listener : listeners)
		{
			listener->OnCharacterUpdate( character->getName().c_str() );
		}
	}
}


/* the following are designed to re-orient joints local axes. added by David Huang Jun 2012*/
/* Orient skeleton joints local axes to match world coordinate axes (Y-up Z-front)
// put skel into T-pose before running this! */
void SBSkeleton::orientJointsLocalAxesToWorld()
{
	auto& jnts = SkSkeleton::joints();
	update_global_matrices();
	for(const auto & j : jnts)
	{
		j->quat()->orientation(/*j->prerot() * */j->quat()->value());
		const SrMat& gmat = j->gmat();
		SrQuat grot; grot.set(gmat);
		j->quat()->prerot(grot.inverse());
		j->quat()->postrot(grot.inverse());
	}
	
	// now we can remove local rotations
	for(auto& jnt : jnts)
		jnt->init_rot();
	update_global_matrices();
}

/* Create a new standard T-pose skel with no pre-rotations from source (TposeSk)
// put source skel(TposeSk) into T-pose before running this!   */
void SBSkeleton::_createSkelWithoutPreRot(SBSkeleton* TposeSk, SBSkeleton* newSk, const char* new_name)
{
	TposeSk->invalidate_global_matrices();
	TposeSk->update_global_matrices();
	*newSk = *TposeSk; // first copy
	if(new_name) //  || strlen(new_name)<1)
	{
		newSk->setName(std::string(new_name));
		SrString fname(TposeSk->skfilename().c_str());
		fname.remove_file_name();
		fname.append(new_name); 
		//fname.append(".sk");
		newSk->skfilename(fname);
	}
	
	auto& new_jnts = newSk->joints();
	for(size_t i=1; i<new_jnts.size(); i++)
	{
		auto& new_j = new_jnts[i];
		if(!new_j->parent()) continue;
		SkJoint* src_j = TposeSk->search_joint(new_j->jointName().c_str());
		if(!src_j) continue;
		if(!src_j->parent()) continue;
		SrVec new_offset = src_j->gmat().get_translation() -
			src_j->parent()->gmat().get_translation();		
		new_j->offset(new_offset);
	}
	
	for(size_t i=1; i<new_jnts.size(); i++)
	{
		auto& new_j = new_jnts[i];
		new_j->init_values();
		new_j->quat()->prerot(SrQuat::null);
		new_j->quat()->postrot(SrQuat::null);
	}
}
// same as above but for Python interface
SBSkeleton* SBSkeleton::createSkelWithoutPreRot(const char* new_name)
{
	SBSkeleton* newSk = new SBSkeleton(_scene);
	_createSkelWithoutPreRot(this, newSk, new_name);
	return newSk;
}

void SBSkeleton::rescale( float scaleRatio )
{
	for (auto& i : joints())
	{
		SBJoint* joint = dynamic_cast<SBJoint*>(i.get());
		if (joint)
			joint->setOffset(joint->getOffset()*(float)scaleRatio / _scale);
	}
	_scale = scaleRatio;
}

float SBSkeleton::getScale() const
{
	return _scale;
}



void SBSkeleton::notify(SBSubject* subject)
{
	SBObject::notify(subject);

	SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute)
	{
		if (attribute->getName() == "orientation")
		{
			Vec3Attribute* orientationAttr = dynamic_cast<Vec3Attribute*>(attribute);
			// get the root node
			SkJoint* root = this->root();
			if (root)
			{
				if (root->quat())
				{
					SrQuat prerot = root->quat()->prerot();
					if (_origRootChanged)	
						prerot = _origRootPrerot;
					_origRootChanged = true;
					_origRootPrerot = prerot;
					SrQuat adjustX;
					SrQuat adjustY;
					SrQuat adjustZ;

					if (orientationAttr->getValue().x != 0)
					{
						SrVec xaxis(1, 0, 0);
						adjustX.set(xaxis, orientationAttr->getValue().x * 3.14159f / 180.0f);
					}
					if (orientationAttr->getValue().y != 0)
					{
						SrVec yaxis(0, 1, 0);
						adjustY.set(yaxis, orientationAttr->getValue().y * 3.14159f / 180.0f);
					}
					if (orientationAttr->getValue().z != 0)
					{	
						SrVec zaxis(0, 0, 1);
						adjustZ.set(zaxis, orientationAttr->getValue().z * 3.14159f / 180.0f);
					}
					
					SrQuat finalAdjust = adjustX * adjustY * adjustZ;
					SrQuat final = finalAdjust * prerot;
					root->quat()->prerot(final);
					root->offset(root->offset() * finalAdjust);
				}
			}
		}
	}

}

SBAPI void SBSkeleton::getJointPositions( const std::vector<std::string>& jointNames, std::vector<SrVec>& jointPositions, int startIdx )
{
	if (jointPositions.size() < jointNames.size()+startIdx)
		return;

	for (unsigned int i=0;i<jointNames.size();i++)
	{
		SmartBody::SBJoint* joint = getJointByName(jointNames[i]);
		if (!joint)
			continue;
		jointPositions[i+startIdx] = joint->gmat().get_translation();
	}
}

void SBSkeleton::setPawnName( const std::string& pawnName )
{
	linkedPawnName = pawnName;
}

}; //namespace
