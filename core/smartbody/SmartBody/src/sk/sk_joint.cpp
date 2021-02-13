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

#include <cmath>
#include <sr/sr_model.h>
#include <sk/sk_joint.h>
#include <sk/sk_skeleton.h>

#include <memory>
#include <utility>
#include "SBUtilities.h"
//============================= SkJoint ============================

SkJoint::SkJoint()
{
   _pos.init();

   _coldetid = -1;

   _parent = nullptr;

   _lmat_uptodate = 0;
   _name = "";
   _index = -1;
   _skeleton = nullptr;

   _pos._joint = this; // as we initialized it with null (to avoid a warning)
   _rtype =-1;
   rot_type(TypeQuat);
   _jointType = TypeJoint;

   _mass = 0;
}

SkJoint::SkJoint ( SkSkeleton* sk, SkJoint* parent, RotType rtype, int i )
 {
   _pos.init();

   _coldetid = -1;

   _parent = parent;

   _lmat_uptodate = 0;
   _name = "";
   _index = i;
   _skeleton = sk;

   _pos._joint = this; // as we initialized it with null (to avoid a warning)
   _rtype =-1;
   rot_type ( rtype );

   _mass = 0;
   _jointType = TypeJoint;

   // TODO: verify we are a parent/ancestor of the parent
   if( parent )
	   parent->_children.emplace_back(this);
 }

SkJoint::~SkJoint() = default;

void SkJoint::visgeo ( boost::intrusive_ptr<SrModel> m )
 { 
   _visgeo = std::move(m);
 }

void SkJoint::colgeo ( boost::intrusive_ptr<SrModel> m )
 { 
   _colgeo = std::move(m);
 }

void SkJoint::offset ( const SrVec& o )
 {
   if ( o==_offset ) return;
   _offset=o;
   set_lmat_changed ();
 }

void SkJoint::endEffectorOffset ( const SrVec& o )
 {
   if ( o==_endEffectorOffset ) return;
   _endEffectorOffset=o;
   set_lmat_changed ();
 }

void SkJoint::rot_type ( RotType t )
 {
   if ( char(t)==_rtype ) return; // no change required
   
   _rtype = (char)t;

   set_lmat_changed ();

   switch (_rtype)
    { case TypeEuler: _quat = std::make_unique<SkJointEuler>(this); break;
      case TypeSwingTwist: _quat = std::make_unique<SkJointSwingTwist>(this); break;
      default: _rtype = TypeQuat;
               _quat = std::make_unique<SkJointQuat>(this);
    }
 }

void SkJoint::init_pos ()
 {
   if ( !pos()->frozen(0) ) pos()->value(0,0);
   if ( !pos()->frozen(1) ) pos()->value(1,0);
   if ( !pos()->frozen(2) ) pos()->value(2,0);
 }

void SkJoint::init_rot ()
 {
   if ( !quat()->active() ) return;
   switch (_rtype)
    { case TypeEuler: if ( !euler()->frozen(0) ) euler()->value(0,0);
                      if ( !euler()->frozen(1) ) euler()->value(1,0);
                      if ( !euler()->frozen(2) ) euler()->value(2,0); break;
      case TypeSwingTwist: st()->swing ( 0, 0 );
                           if ( !st()->twist_frozen() ) st()->twist ( 0 ); break;
      default: quat()->value ( SrQuat::null ); break;
    }
 }

void SkJoint::update_lmat ()
 {
   if ( _lmat_uptodate ) return;
   _lmat_uptodate = 1;

   // update the 3x3 rotation submatrix if required:
   if ( !_quat->_jntsync )
    { _quat->_jntsync = 1;
      const SrQuat& q = _quat->value();

      float x2  = q.x+q.x;
      float x2x = x2*q.x;
      float x2y = x2*q.y;
      float x2z = x2*q.z;
      float x2w = x2*q.w;
      float y2  = q.y+q.y;
      float y2y = y2*q.y;
      float y2z = y2*q.z;
      float y2w = y2*q.w;
      float z2  = q.z+q.z;
      float z2z = z2*q.z;
      float z2w = z2*q.w;

      _lmat[0] = 1.0f - y2y - z2z; _lmat[1] = x2y + z2w;        _lmat[2]  = x2z - y2w;
      _lmat[4] = x2y - z2w;        _lmat[5] = 1.0f - x2x - z2z; _lmat[6]  = y2z + x2w;
      _lmat[8] = x2z + y2w;        _lmat[9] = y2z - x2w;        _lmat[10] = 1.0f - x2x - y2y;

      if (_lmat[0]==0 && _lmat[1]==0 && _lmat[2]==0) _lmat=SrMat::id; // to avoid a null matrix
    }

   // now update offset + translation:
   _lmat[12] = _pos.value(0) + _offset.x;
   _lmat[13] = _pos.value(1) + _offset.y;
   _lmat[14] = _pos.value(2) + _offset.z;
 }

void SkJoint::update_gmat ()
 {
   const SrMat& pmat = _parent? _parent->_gmat : SrMat::id;

   update_lmat ();

   _gmat.mult ( _lmat, pmat );

   for (auto & i : _children)
    { i->update_gmat();
    }
 }

void SkJoint::update_gmat ( std::vector<SkJoint*>& end_joints )
 {
   const SrMat& pmat = _parent? _parent->_gmat : SrMat::id;

   update_lmat ();

   _gmat.mult ( _lmat, pmat );

   for (auto & end_joint : end_joints)
    if ( this==end_joint ) return;

   for (auto & i : _children)
    { i->update_gmat(end_joints);
    }
 }

void SkJoint::update_gmat_local ()
 {
   update_lmat ();
   _gmat.mult ( _lmat, _parent? _parent->_gmat : SrMat::id );
 }

void SkJoint::update_gmat_up ( SkJoint* stop_joint )
 {
   SrArray<SkJoint*> joints;
   joints.capacity ( 64 );
   
   SkJoint* j = this;
   do { joints.push() = j;
        j = j->_parent;
      } while ( j!=nullptr && j!=stop_joint );
      
   while ( !joints.empty() )
    { joints.pop()->update_gmat_local();
    }
 }

void SkJoint::set_lmat_changed ()
 {
   _lmat_uptodate = 0;
   if (_skeleton)
	_skeleton->invalidate_global_matrices();
 }

static void _unite ( const SkJoint* j, SrModel& m, SrModel& tmp, bool ifvisgeo )
 {
   const SrModel* geo=nullptr;

   if (  ifvisgeo && j->visgeo() ) geo = j->visgeo();
   if ( !ifvisgeo && j->colgeo() ) geo = j->colgeo();

   if ( geo )
    { tmp = *geo;
      tmp.apply_transformation ( j->gmat() );
      m.add_model ( tmp );
    }

   int i;
   for ( i=0; i<j->num_children(); i++ )
    { _unite ( j->child(i), m, tmp, ifvisgeo );
    }
 }

void SkJoint::unite_visgeo ( SrModel& m )
 {
   update_gmat();
   m.init ();
   SrModel tmp;
   _unite ( this, m, tmp, true );
 }

void SkJoint::unite_colgeo ( SrModel& m )
 {
   update_gmat();
   m.init ();
   SrModel tmp;
   _unite ( this, m, tmp, false );
 }

void SkJoint::add_child( SkJoint* child )
 { 
	 _children.emplace_back(child);
	 child->set_parent(this);
	 child->skeleton(this->skeleton());
}

void SkJoint::remove_child(SkJoint* child)
{
	for (size_t i = 0; i < _children.size(); ++i)
	{
		if (_children[i]->getName() == child->getName())
		{
			_children.erase(_children.begin() + i);
			return;
		}
	}
	SmartBody::util::log("SkJoint::remove_child WARNING: cannot find child joint %s", child->getName().c_str());
}

void SkJoint::updateGmatZero(const SrMat& gmatZero)
{
	_gmatZero = gmatZero;
}

SrVec SkJoint::localGlobalAxis(int i)
{
	SrVec axis;
	for (int k=0;k<3;k++)
	{
		axis[k] = _gmatZero[i+k*4];
	}
	return axis;
}

void SkJoint::recursive_children(std::vector<SkJoint*>& joints, SkJoint* root)
{
	joints.emplace_back(root);
	for (int i = 0; i < root->num_children(); i++)
	{
		SkJoint::recursive_children(joints, root->child(i));
	}
}

const std::string& SkJoint::getMappedJointName() const
{
	return _sbName;
}


const std::string& SkJoint::jointName() const
{
	return _name;
}

void SkJoint::copyTo( SkJoint* dest )
{
	dest->name(_name);	
	dest->extName(_extName);
	dest->extID(_extID);
	dest->extSID(_extSID);
//	dest->visgeo(_visgeo);
//	dest->colgeo(_colgeo);

	SkJointPos* srcPos = pos();
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



	if (quat()->active())
		dest->quat()->activate();

	dest->offset(offset());
	dest->updateGmatZero(gmatZero());	
	SkJointQuat* srcQuat = this->quat();
	SkJointQuat* destQuat = dest->quat();
	//destQuat->value(srcQuat->value());
	destQuat->value(srcQuat->rawValue()); // set the raw value here since value() may contain pre-rotation
	destQuat->prerot(srcQuat->prerot());
	destQuat->postrot(srcQuat->postrot());
	destQuat->orientation(srcQuat->orientation());	
	dest->mass(mass());		

}
//============================ End of File ============================
