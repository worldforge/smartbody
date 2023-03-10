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


#include <sr/sr_model.h>

#include <sk/sk_skeleton.h>
#include <sk/sk_posture.h>
#include <queue>
#include <sb/SBJoint.h>
#include "SBUtilities.h"

//============================ SkSkeleton ============================

SkSkeleton::SkSkeleton() : SmartBody::SBAsset(),
						   _root(nullptr),
						   _coldetid(-1),
						   _gmat_uptodate(true),
						   _channels(new SkChannelArray) {
	setName("noname");
	_com.set(0, 0, 0);
}

SkSkeleton::SkSkeleton(const SkSkeleton& rhs)  : SmartBody::SBAsset()
{
	_jointLookupFn = rhs._jointLookupFn;
//	jointMap = rhs.jointMap;
	setName(rhs.getName());
	_skfilename = rhs._skfilename;
	if (!rhs._root)
	{
		return;
	}
	_root = new SmartBody::SBJoint(this, nullptr, rhs.root()->rot_type(), rhs.root()->index());
	copy_joint(_root, rhs._root);
	_joints.emplace_back(_root);
	SkJoint* origParent = rhs._root;
	SkJoint* thisParent = _root;
	create_joints(origParent, thisParent);
	// reset the joint indices since the original skeleton might not match if 
	// it contained nodes not connected to the root
	for (size_t j = 0; j < _joints.size(); j++)
	{
		_joints[j]->set_index(j);
	}

	_gmat_uptodate = rhs._gmat_uptodate;
	//TODO: Use a copy ctor instead.
	_channels = new SkChannelArray;
	auto& origChannels = rhs.channels();
	for (int c = 0; c < origChannels.size(); c++)
	{
		auto& origChannel = origChannels.get(c);
		SkJoint* origJoint = origChannels.joint(c);
		if (origJoint)
		{
			SkJoint* joint = this->search_joint(origJoint->jointName().c_str());
			_channels->add(joint, origChannel.type, true);
		}
	}
	_channels->_jointLookupFn = origChannels._jointLookupFn;
	_channels->count_floats();
	_com = rhs._com;

	compress ();
	make_active_channels();
	updateJointMap();
}

SkSkeleton::~SkSkeleton ()
{
	//SmartBody::util::log("delete skeleton %s",getName().c_str());
	init ();
}

SkSkeleton& SkSkeleton::operator=(const SkSkeleton& rhs)
{
	if (&rhs == this) {
		return *this;
	}
	if (!rhs.root())
	{
		SmartBody::util::log("Original skeleton has no root joint, cannot be copied.");
		return *this;
	}
	setName(rhs.getName());
	_skfilename = rhs.skfilename();
	setName(rhs.getName());
	
	//	_root = new SkJoint(this, 0, rhs.root()->rot_type(), rhs.root()->index());
	_root = new SmartBody::SBJoint(this, nullptr, rhs.root()->rot_type(), rhs.root()->index());
	copy_joint(_root, rhs.root());
	_joints.emplace_back(_root);
	SkJoint* origParent = rhs.root();
	SkJoint* thisParent = _root;
	create_joints(origParent, thisParent);

	_gmat_uptodate = rhs.global_matrices_uptodate();
	_channels = new SkChannelArray;
	auto& origChannels = rhs.channels();
	for (int c = 0; c < origChannels.size(); c++)
	{
		auto& origChannel = origChannels.get(c);
		SkJoint* origJoint = origChannels.joint(c);
		if (!origJoint)
			continue;
		SkJoint* joint = this->search_joint(origJoint->jointName().c_str());
		_channels->add(joint, origChannel.type, true);
	}
	_channels->count_floats();
	_com = rhs.com();	
	_jointLookupFn = rhs._jointLookupFn;
	compress ();
	updateJointMap();
	return *this;
}

void SkSkeleton::init ()
{
	resetSearchJoint();
	_coldet_free_pairs.clear();
	_channels->init();
	_postures.clear();
/*
	while ( _joints.size()>0 ) 
	{
		SkJoint* joint = _joints[_joints.size() - 1];
		delete joint;
		_joints.pop_back();
	}
*/
	_joints.clear();
	_jointMap.clear();
	_root = nullptr;
	_gmat_uptodate = false;
}

void SkSkeleton::refresh_joints()
{
	auto joints = std::move(_joints);

	SkJoint* j = _root;
	std::queue<SkJoint*> queue;
	if (_root)
		queue.push(j);

	int curIndex = 0;

	while (!queue.empty())
	{
		SkJoint* joint = queue.front();

		joint->set_index(curIndex);
		curIndex++;
		for (int c = 0; c < joint->num_children(); c++) {
			queue.push(joint->child(c));
		}

		auto I = std::find_if(joints.begin(), joints.end(), [&](const std::unique_ptr<SkJoint>& entry){ return entry.get() == joint;});
		if (I != joints.end()) {
			_joints.emplace_back(std::move(*I));
		} else {
			_joints.emplace_back(std::unique_ptr<SkJoint>(joint));
		}


		queue.pop();
	}
}


SkJoint* SkSkeleton::add_joint ( SkJoint::RotType rtype, int parentid )
{
	_jointMap.clear();

	SkJoint* parent=nullptr;
	if ( parentid < 0 ) {
#if 0   // is there a reason whey we want to attach unparent joint to the last joint ? it doesn't make sense and will most likely break the structure of skeleton.
		if( _joints.size()>0 )
			parent =_joints[_joints.size() - 1];
		else
#endif
			parent=nullptr;
	} 	
	else if (parentid > (int)_joints.size())
	{
		parent = nullptr;
	}
	else {
		parent = _joints[parentid].get();
	}

	auto j = std::make_unique<SmartBody::SBJoint> ( this, parent, rtype, _joints.size() );
	_joints.emplace_back(std::move(j));

	// should set the root explicitly
	//if ( parent==nullptr )
	//	_root=j;

	return _joints.back().get();
}

SkJoint* SkSkeleton::insert_new_root_joint ( SkJoint::RotType rtype )
{
	_jointMap.clear();
	SkJoint* old_root = _root;

	_root = new SmartBody::SBJoint ( this, 0, rtype, _joints.size() );
	_joints.emplace_back(_root);

	if( old_root ) {
		old_root->_parent = _root;

		/* thiebaux 6/19/2006 */
		_root->_children.emplace_back( old_root );
	}

	return _root;
}

SkJoint* SkSkeleton::linear_search_joint ( const char* n ) const
{
	std::string name(n);
	for (auto& _joint : _joints)
	{ 
		if (_joint->jointName() == name )
			return _joint.get();
	}
	return nullptr;
}

void SkSkeleton::resetSearchJoint()
{
	_jointMap.clear();
	_extJointMap.clear();
	_extIDJointMap.clear();
	_extSIDJointMap.clear();
}

SkJoint* SkSkeleton::search_joint ( const char* n )
{
	size_t jointSize = _joints.size();
	if ( jointSize > 0 && _jointMap.size() != jointSize)
	{
		//SmartBody::util::log("joint size = %d, joint map size = %d", jointSize, jointMapSize);
		updateJointMap();
	}

	if (_extJointMap.empty() &&
		!_joints.empty())
	{
		for (auto& joint : _joints)
		{
			_extJointMap.emplace(joint->extName(), joint.get());
		}
	}

	if (_extIDJointMap.empty() &&
		!_joints.empty())
	{
		for (auto& joint : _joints)
		{
			_extIDJointMap.emplace(joint->extID(), joint.get());
		}
	}

	if (_extSIDJointMap.empty() &&
		!_joints.empty())
	{
		for (auto& joint : _joints)
		{
			_extSIDJointMap.emplace(joint->extSID(), joint.get());
		}
	}

	auto iter = _jointMap.find(n);
	if (iter != _jointMap.end())
	{
		return (*iter).second;
	}
	else if ( _extJointMap.find(n) != _extJointMap.end())
	{
		return (_extJointMap.find(n)->second);
	}
	else if ( _extIDJointMap.find(n) != _extIDJointMap.end())
	{
		return (_extIDJointMap.find(n)->second);
	}
	else if ( _extSIDJointMap.find(n) != _extSIDJointMap.end())
	{
		return (_extSIDJointMap.find(n)->second);
	}
	else
	{
		return nullptr;
	}
}

void SkSkeleton::update_global_matrices ()
{
	if ( _gmat_uptodate ) return;
	if (!_root)
		return;
	_root->update_gmat();
	_gmat_uptodate = true;
}

void SkSkeleton::compress ()
{
	/*
	_coldet_free_pairs.compress();
	_channels->compress();
	_postures.compress();
	_joints.compress();

	int i;
	for ( i=0; i<_joints.size(); i++ )
	_joints[i]->_children.compress();
	*/
	//updateGlobalMatricesZero();
}

void SkSkeleton::updateGlobalMatricesZero()
{
	this->update_global_matrices();
	for (auto & _joint : _joints)
	{
		_joint->updateGmatZero(_joint->gmat());
	}
}

void SkSkeleton::set_geo_local ()
{

	for (auto & _joint : _joints) _joint->init_values();
	update_global_matrices ();

	SrMat mat;
	for (auto & _joint : _joints)
	{
	mat = _joint->gmat();
	mat.invert();
	if ( _joint->_visgeo ) _joint->_visgeo->apply_transformation(mat);
	if ( _joint->_colgeo ) _joint->_colgeo->apply_transformation(mat);
	}


	update_global_matrices ();
}


float SkSkeleton::getBaseHeight( const std::string& baseName )
{
	SrBox boundingBox = getBoundingBox();
	SkJoint* baseJoint = search_joint(baseName.c_str());
	if (!baseJoint) return 0.f;
	SrVec basePos = baseJoint->gmat().get_translation();
	return basePos.y - boundingBox.a.y;
}

float SkSkeleton::getCurrentHeight()
{
	SrBox initialBoundingBox = getBoundingBox();	
	return initialBoundingBox.size().y;
}

SrBox SkSkeleton::getBoundingBox()
{
	SrBox initialBoundingBox;
	update_global_matrices();
	auto& joints = get_joint_array();
	for (auto & joint : joints)
	{
		if (joint->getJointType() != SkJoint::TypeJoint)
			continue;
		joint->update_gmat();
		const SrMat& gmat = joint->gmat();
		SrVec point(gmat.get(3, 0), gmat.get(3, 1), gmat.get(3, 2));
		initialBoundingBox.extend(point);
	}

	return initialBoundingBox;
}


void SkSkeleton::copy_joint(SkJoint* dest, SkJoint* src)
{
	src->copyTo(dest); // move copy joint to SkJoint as a member function
#if 0
	dest->name(src->jointName());
	dest->extName(src->extName());
	dest->extID(src->extID());
	dest->extSID(src->extSID());
	dest->visgeo(src->visgeo());
	dest->colgeo(src->colgeo());

	SkJointPos* srcPos = src->pos();
	SkJointPos* destPos = dest->pos();
	if (!srcPos->frozen(SkVecLimits::X))
	{
		destPos->limits(SkVecLimits::X, srcPos->limits(SkVecLimits::X));
		destPos->lower_limit(SkVecLimits::X, srcPos->lower_limit(SkVecLimits::X));
		destPos->upper_limit(SkVecLimits::X, srcPos->upper_limit(SkVecLimits::X));
	}
	if (!srcPos->frozen(SkVecLimits::Y))
	{
		destPos->limits(SkVecLimits::Y, srcPos->limits(SkVecLimits::Y));
		destPos->lower_limit(SkVecLimits::Y, srcPos->lower_limit(SkVecLimits::Y));
		destPos->upper_limit(SkVecLimits::Y, srcPos->upper_limit(SkVecLimits::Y));
	}
	if (!srcPos->frozen(SkVecLimits::Z))
	{
		destPos->limits(SkVecLimits::Z, srcPos->limits(SkVecLimits::Z));
		destPos->lower_limit(SkVecLimits::Z, srcPos->lower_limit(SkVecLimits::Z));
		destPos->upper_limit(SkVecLimits::Z, srcPos->upper_limit(SkVecLimits::Z));
	}



	if (src->quat()->active())
		dest->quat()->activate();

	dest->offset(src->offset());
	dest->updateGmatZero(src->gmatZero());	
	SkJointQuat* srcQuat = src->quat();
	SkJointQuat* destQuat = dest->quat();
	//destQuat->value(srcQuat->value());
	destQuat->value(srcQuat->rawValue()); // set the raw value here since value() may contain pre-rotation
	destQuat->prerot(srcQuat->prerot());
	destQuat->postrot(srcQuat->postrot());
	destQuat->orientation(srcQuat->orientation());	
	dest->mass(src->mass());	
#endif
}

void SkSkeleton::create_joints(SkJoint* origParent, SkJoint* parent)
{
	for (int i = 0; i < origParent->num_children(); i++)
	{
		SkJoint* newJoint = new SmartBody::SBJoint(this, parent, origParent->child(i)->rot_type(), origParent->child(i)->index());
		// ??? SkJoint* newJoint = new SkJoint(this, parent, origParent->child(i)->rot_type(), _joints.size());
		_joints.emplace_back(newJoint);

		copy_joint(newJoint, origParent->child(i));
		create_joints(origParent->child(i), newJoint);
	}
}

SrVec SkSkeleton::boneGlobalDirection(const std::string& srcName, const std::string& dstName)
{
	SrVec dir = SrVec(0,0,0);
	SkJoint* srcJoint = search_joint(srcName.c_str());
	SkJoint* targetJoint = search_joint(dstName.c_str());	
	if (!srcJoint || !targetJoint) return dir;	
	srcJoint->update_gmat();
	targetJoint->update_gmat();
	dir = targetJoint->gmat().get_translation() - srcJoint->gmat().get_translation();
	dir.normalize();
	return dir;
}

void SkSkeleton::clearJointValues()
{
	for (auto& joint : _joints)
	{
			joint->quat()->value(SrQuat()); // reset rotation
		for (int k=0;k<3;k++)
			joint->pos()->value(k,0.f);
	}
	invalidate_global_matrices();
	update_global_matrices();
}

void SkSkeleton::updateJointMap()
{
	//SmartBody::util::log("Update Joint Map : %s, jointMap size = %d", getJointMapName().c_str(), _jointMap.size());
	_jointMap.clear();
	//SmartBody::util::log("After cleanup jointMap");
	int jointSize = _joints.size();	
	for (int i = 0; i < jointSize; i++)
	{
		auto& joint = _joints[i];
		auto jname = joint->jointName();

		if (_jointLookupFn)
		{			
			auto& mappedName = _jointLookupFn(jname);

			//SmartBody::util::log("Has JointMap, origName = %s, mappedName = %s", jname.c_str(), mappedName.c_str());

			if (!mappedName.empty()) {
				jname = mappedName;
			}
			//SmartBody::util::log("After mapped name, jname = %s", jname.c_str());
		}
		joint->_sbName = jname;
		//SmartBody::util::log("Before search jname = %s", jname.c_str());
		auto iter = _jointMap.find(joint->getMappedJointName());
		//SmartBody::util::log("After search jname = %s", jname.c_str());
		//if (iter != _jointMap.end())
		//	SmartBody::util::log("Found duplicate joint name %s", jname.c_str());
		//SmartBody::util::log("Insert joint name %s, _joint[%d] = %s", jname.c_str(), i, _joints[i]->getName().c_str());
		_jointMap.emplace(joint->getMappedJointName(), _joints[i].get());

	}
	//nSmartBody::util::log("Done update Joint Map : %s", getJointMapName().c_str());
}
SrVec SkSkeleton::getFacingDirection()
{
	SrVec defaultDir = SrVec(0,0,1);
	invalidate_global_matrices();
	update_global_matrices();
	
	SkJoint *base, *l_hip, *r_hip;
	base = search_joint("base");
	l_hip = search_joint("l_hip");
	r_hip = search_joint("r_hip");
	
	if (!base || !l_hip || !r_hip)
		return defaultDir;

	SrVec lhipPos,rhipPos, basePos;
	lhipPos = l_hip->gmatZero().get_translation();
	rhipPos = r_hip->gmatZero().get_translation();
	basePos = base->gmatZero().get_translation();

	defaultDir = cross(rhipPos-basePos,lhipPos-basePos);
	sr_out << "l_hip = " << lhipPos << srnl;
	sr_out << "r_hip = " << rhipPos << srnl;
	sr_out << "base = " << basePos << srnl;
	sr_out << "defaultDir = " << defaultDir << srnl;
	defaultDir.y = 0.0;
	defaultDir.normalize();
	return defaultDir;
}

//void SkSkeleton::setJointMapName( const std::string& jointMapName )
//{
//	jointMap = jointMapName;
//}
//
//const std::string& SkSkeleton::getJointMapName() const
//{
//	return jointMap;
//}


//============================ End of File ============================
