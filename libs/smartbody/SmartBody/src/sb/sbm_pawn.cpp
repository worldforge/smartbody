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

#include <sb/SBTypes.h>

#include "sbm_pawn.hpp"
#include <sb/SBSimulationManager.h>
#include <sb/SBScene.h>


#include <sb/SBSceneListener.h>
#include <sb/SBCollisionManager.h>
#include "SBUtilities.h"
#include <controllers/me_controller_tree_root.hpp>
#include <controllers/me_ct_channel_writer.hpp>
#include "sbm/sbm_constants.h"
#include <sb/SBPawn.h>


#include <iostream>
#include <utility>

#include <sb/SBSkeleton.h>
#include <sr/sr_sn_matrix.h>


using namespace std;


// Predeclare private functions defined below
inline bool parse_float_or_error( float& var, const char* str, const string& var_name );


/////////////////////////////////////////////////////////////
//  SbmPawn Constants
const char* SbmPawn::WORLD_OFFSET_JOINT_NAME = "world_offset";
SkChannelArray SbmPawn::WORLD_OFFSET_CHANNELS_P;

SkChannelArray& SbmPawn::getWorldOffsetChannels() {
	if( WORLD_OFFSET_CHANNELS_P.size()==0 ) {
		std::string world_offset_joint_name( WORLD_OFFSET_JOINT_NAME );
		WORLD_OFFSET_CHANNELS_P.add( world_offset_joint_name, SkChannel::XPos );
		WORLD_OFFSET_CHANNELS_P.add( world_offset_joint_name, SkChannel::YPos );
		WORLD_OFFSET_CHANNELS_P.add( world_offset_joint_name, SkChannel::ZPos );
		WORLD_OFFSET_CHANNELS_P.add( world_offset_joint_name, SkChannel::Quat );
	}
	return WORLD_OFFSET_CHANNELS_P;
}


//SbmPawn::SbmPawn() : SBObject()
//{
//	_skeleton = nullptr;
//	SbmPawn::initData();
//
//	std::string validName =  _scene.getValidName("object");
//	setName(validName);	
//	SbmPawn::initData();
//}

// Constructor
SbmPawn::SbmPawn(SmartBody::SBScene& scene, const char * name ) 
: SmartBody::SBObject(),
SmartBody::SBSceneOwned(scene),
_skeleton(new SmartBody::SBSkeleton(scene)),
blendMeshGroup(new SrSnGroup()),
ct_tree_p( MeControllerTreeRoot::create(*this) ),
//TODO: remove the need for this hideous cast
world_offset_writer_p( new MeCtChannelWriter(*(SmartBody::SBPawn*)this, getWorldOffsetChannels(), true)),
wo_cache_timestamp( -std::numeric_limits<float>::max() )
{
	SmartBody::SBObject::setName( name );
	std::string controllerName = this->getName();
	controllerName += "_worldOffsetWriter";
	world_offset_writer_p->setName( controllerName );

	ct_tree_p->add_controller( world_offset_writer_p.get() );


	collisionObjName = this->getName();
	collisionObjName += "_BV"; // bounding volume

	SmartBody::SBCollisionManager* colManager = _scene.getCollisionManager();
	SBGeomObject* geomObj = colManager->createCollisionObject(collisionObjName,"null",SrVec());
	geomObj->attachToObj(this);

	wo_cache.x = 0;
	wo_cache.y = 0;
	wo_cache.z = 0;
	wo_cache.h = 0;
	wo_cache.p = 0;
	wo_cache.r = 0;

	blendMeshGroup->add(new SrSnMatrix, 0);
}

const boost::intrusive_ptr<SkSkeleton>& SbmPawn::getSkeleton() const
{
	return _skeleton;
}

void SbmPawn::setSkeleton(boost::intrusive_ptr<SkSkeleton> sk)
{
	if (!sk)
	{
		SmartBody::util::log("Cannot set a null skeleton on object %s", getName().c_str());
		return;
	}
	 

	if (_skeleton)
	{		
		ct_tree_p->remove_skeleton( _skeleton->getName() );
	}
	_skeleton = std::move(sk);
	ct_tree_p->add_skeleton( _skeleton->getName(), _skeleton );

	//scene_p->init(_skeleton);
	//int err = mcu.add_scene(scene_p);	
		
	float height = _skeleton->getCurrentHeight();	
	setHeight(height);

	std::vector<SmartBody::SBSceneListener*>& listeners = _scene.getSceneListeners();
	for (auto & listener : listeners)
	{
		listener->OnCharacterUpdate( getName() );
	}
}

int SbmPawn::init( boost::intrusive_ptr<SkSkeleton> new_skeleton_p ) {
	if( _skeleton ) {
		ct_tree_p->remove_skeleton( _skeleton->getName() );
	}
	_skeleton = std::move(new_skeleton_p);
	if( _skeleton ) {
		if( init_skeleton()!=CMD_SUCCESS ) {
			return CMD_FAILURE; 
		}
		ct_tree_p->add_skeleton( _skeleton->getName(), _skeleton );
		
		std::vector<SmartBody::SBSceneListener*>& listeners = _scene.getSceneListeners();
		for (auto & listener : listeners)
		{
			listener->OnCharacterUpdate( getName() );
		}

	}

	// 	if (colObj_p)
	// 	{
	// 		initPhysicsObj();
	// 	}

	// Name the controllers
	string ct_name( getName() );
	ct_name += "'s world_offset writer";
	world_offset_writer_p->setName( ct_name );

	return CMD_SUCCESS;
}

int SbmPawn::setup() {
	// Verifiy the joint name is not already in use.
	if( _skeleton->search_joint( SbmPawn::WORLD_OFFSET_JOINT_NAME ) ) {
		std::stringstream strstr;
		strstr << "ERROR: SbmPawn::init_skeleton_offset: Skeleton already contains joint \"" << SbmPawn::WORLD_OFFSET_JOINT_NAME << "\".";
		SmartBody::util::log(strstr.str().c_str());
		return( CMD_FAILURE ); 
	}

	SkJoint* world_offset_joint = _skeleton->insert_new_root_joint( SkJoint::TypeQuat );
	world_offset_joint->name( SbmPawn::WORLD_OFFSET_JOINT_NAME );
	world_offset_joint->extName( SbmPawn::WORLD_OFFSET_JOINT_NAME );
	// Make sure the world_offset accepts new pos and quat values
	SkJointPos* world_offset_pos = world_offset_joint->pos();
	world_offset_pos->limits( SkVecLimits::X, false );
	world_offset_pos->limits( SkVecLimits::Y, false );
	world_offset_pos->limits( SkVecLimits::Z, false );
	world_offset_joint->quat()->activate();
	_skeleton->compress();
	_skeleton->make_active_channels();

	wo_cache.x = 0;
	wo_cache.y = 0;
	wo_cache.z = 0;
	wo_cache.h = 0;
	wo_cache.p = 0;
	wo_cache.r = 0;

	std::vector<SmartBody::SBSceneListener*>& listeners = _scene.getSceneListeners();
	for (auto & listener : listeners)
	{
		listener->OnCharacterUpdate( getName() );
	}

	return( CMD_SUCCESS ); 
}

int SbmPawn::init_skeleton() {
	// Verifiy the joint name is not already in use.
	if( _skeleton->search_joint( SbmPawn::WORLD_OFFSET_JOINT_NAME ) ) {
		std::stringstream strstr;
		strstr << "ERROR: SbmPawn::init_skeleton_offset: Skeleton already contains joint \"" << SbmPawn::WORLD_OFFSET_JOINT_NAME << "\".";
		SmartBody::util::log(strstr.str().c_str());
		return( CMD_FAILURE ); 
	}

	SkJoint* world_offset_joint = _skeleton->insert_new_root_joint( SkJoint::TypeQuat );
	world_offset_joint->name( SbmPawn::WORLD_OFFSET_JOINT_NAME );
	world_offset_joint->extName( SbmPawn::WORLD_OFFSET_JOINT_NAME );
	// Make sure the world_offset accepts new pos and quat values
	SkJointPos* world_offset_pos = world_offset_joint->pos();
	world_offset_pos->limits( SkVecLimits::X, false );
	world_offset_pos->limits( SkVecLimits::Y, false );
	world_offset_pos->limits( SkVecLimits::Z, false );
	world_offset_joint->quat()->activate();
	_skeleton->compress();

	wo_cache.x = 0;
	wo_cache.y = 0;
	wo_cache.z = 0;
	wo_cache.h = 0;
	wo_cache.p = 0;
	wo_cache.r = 0;

	return( CMD_SUCCESS ); 
}

void SbmPawn::reset_all_channels()
{
	if (!_skeleton)
		return;

	SkChannelArray& channels = _skeleton->channels();
	MeFrameData& frameData = ct_tree_p->getLastFrame();
	SrBuffer<float>& sr_fbuff = frameData.buffer();
	int n = channels.size();
	for (int c = 0; c < n; c++)
	{
		SkChannel& chan = channels[c];
		int buffIndex = ct_tree_p->toBufferIndex(c);
		if( buffIndex > -1 )	
		{
			// Assume only have Quat or X/Y/Z
			if (chan.type == SkChannel::Quat)
			{
				sr_fbuff[ buffIndex + 0 ] = 1.0f;
				sr_fbuff[ buffIndex + 1 ] = 0.0f;
				sr_fbuff[ buffIndex + 2 ] = 0.0f;
				sr_fbuff[ buffIndex + 3 ] = 0.0f;			
			}
			else
			{
				sr_fbuff[ buffIndex ] = 0.0f;
			}
		}
	}
}


bool SbmPawn::is_initialized() const {
	return _skeleton != nullptr;
}

int SbmPawn::prune_controller_tree()
{
	// Unimplemented...
	//  TODO: walk the controller tree for excessive world offset raw writers
	return CMD_SUCCESS;
}

SrBox SbmPawn::getBoundingBox()
{ 
	SBGeomObject* geo = getGeomObject();
	if (geo)
	{
		auto* nullObject = dynamic_cast<SBGeomNullObject*>(geo);
		if (nullObject)
		{
			if (_skeleton) 
				return _skeleton->getBoundingBox(); 
			else
				return SrBox();
		}
		SrVec geo_size		= geo->getGeomSize();
		SrBox box = geo->getBoundingBox();
		return box;
	}
	else
	{
		if (_skeleton) 
			return _skeleton->getBoundingBox(); 
		else
			return SrBox();
	}
}

//  Destructor
SbmPawn::~SbmPawn()
{
	ct_tree_p->clear();  // Because controllers within reference back to tree root context

	SmartBody::SBCollisionManager* colManager = _scene.getCollisionManager();
	colManager->removeCollisionObject(collisionObjName);
}



const SkJoint* SbmPawn::get_joint( const char* joint_name ) const {
	if (_skeleton)
		return _skeleton->search_joint( joint_name );
	else
		return nullptr;
}


void SbmPawn::get_world_offset( float& x, float& y, float& z,
							   float& yaw, float& pitch, float& roll ) {
								   if( _scene.getSimulationManager()->getTime() != wo_cache_timestamp )
									   wo_cache_update();

								   x = wo_cache.x;
								   y = wo_cache.y;
								   z = wo_cache.z;
								   yaw = wo_cache.h;
								   pitch = wo_cache.p;
								   roll = wo_cache.r;
}

SrMat SbmPawn::get_world_offset()
{	
	float x,y,z,h,r,p;
	get_world_offset(x,y,z,h,p,r);
	gwiz::quat_t q = gwiz::euler_t(p,h,r);	
	SrQuat pawnQ = SrQuat((float)q.w(), (float)q.x(), (float)q.y(), (float)q.z());
	SrMat gmat;
	pawnQ.get_mat(gmat);
	gmat.e41() = x;
	gmat.e42() = y;
	gmat.e43() = z;
	return gmat;
}

SBTransform& SbmPawn::getGlobalTransform()
{	
	globalTransform.gmat(get_world_offset());
	return globalTransform;
}

void SbmPawn::setGlobalTransform( SBTransform& newGlobalTransform )
{
	SrMat gmat = globalTransform.gmat();
	setWorldOffset(gmat);
}

void SbmPawn::setWorldOffset( const SrMat& newWorld )
{	
	SrQuat quat = SrQuat(newWorld);
	gwiz::quat_t q = gwiz::quat_t(quat.w,quat.x,quat.y,quat.z);
	gwiz::euler_t e = gwiz::euler_t(q);	
	SrVec tran = newWorld.get_translation();
	set_world_offset(tran[0],tran[1],tran[2],(float)e.h(),(float)e.p(),(float)e.r());		
}

void SbmPawn::set_world_offset( float x, float y, float z,
							   float yaw, float pitch, float roll )
{
	// Store values since they are not written to the joint immediately
	wo_cache.x = x;
	wo_cache.y = y;
	wo_cache.z = z;
	wo_cache.h = yaw;
	wo_cache.p = pitch;
	wo_cache.r = roll;
	wo_cache_timestamp = _scene.getSimulationManager()->getTime();

	gwiz::quat_t q = gwiz::euler_t(pitch,yaw,roll);
	float data[7] = { x, y, z, (float)q.w(), (float)q.x(), (float)q.y(), (float)q.z() };
	if (!world_offset_writer_p)
		return;
	world_offset_writer_p->set_data( data );
	// should also write to the joint ?
	SkJoint* woj = _skeleton->search_joint( WORLD_OFFSET_JOINT_NAME );
	if (!woj) return;

	SkJointPos* woj_pos = woj->pos();
	woj_pos->value( SkJointPos::X, x );
	woj_pos->value( SkJointPos::Y, y );
	woj_pos->value( SkJointPos::Z, z );


	std::stringstream strstr;
	switch( woj->rot_type() ) {
		case SkJoint::TypeEuler: {
			SkJointEuler* joint_euler = woj->euler();
			joint_euler->value( SkJointPos::X, pitch );
			joint_euler->value( SkJointPos::Y, yaw );
			joint_euler->value( SkJointPos::Z, roll );
								 }
								 break;
		case SkJoint::TypeQuat: {
			SkJointQuat* joint_quat = woj->quat();
			gwiz::quat_t q_joint = gwiz::euler_t(pitch, yaw, roll);
			float q_data[4] = {(float)q_joint.w(), (float)q_joint.x(), (float)q_joint.y(), (float)q_joint.z() };
			joint_quat->value( q_data );
								}
								break;
		case SkJoint::TypeSwingTwist:
			strstr << "ERROR: SbmPawn::set_world_offset(..): Unsupported joint rotation type SwingTwist.";
			SmartBody::util::log(strstr.str().c_str());
			break;
		default:
			strstr << "ERROR: SbmPawn::set_world_offset(..): Unknown joint rotation type: " << woj->rot_type();
			SmartBody::util::log(strstr.str().c_str());
			break;
	}	
}

void SbmPawn::wo_cache_update() {
	const SkJoint* joint = get_world_offset_joint();
	if( joint==nullptr )
	{
		//std::stringstream strstr;
		//strstr << "ERROR: SbmPawn::wo_cache_update(..): \"" << getName() << "\" does not have a " << WORLD_OFFSET_JOINT_NAME << " joint.";
		//SmartBody::util::log(strstr.str().c_str());
		return;
	}
	const SkJointPos* pos = joint->pos();
	float x = pos->value( SkJointPos::X );
	float y = pos->value( SkJointPos::Y );
	float z = pos->value( SkJointPos::Z );
	this->wo_cache.x = x;
	this->wo_cache.y = y;
	this->wo_cache.z = z;

	SkJoint::RotType rot_type = joint->rot_type();
	if( rot_type != SkJoint::TypeQuat ) {
		std::stringstream strstr;
		strstr << "ERROR: SbmPawn::wo_cache_update(..): Unsupported world_offset rotation type: " << rot_type << " (Expected TypeQuat, "<<SkJoint::TypeQuat<<")";
		SmartBody::util::log(strstr.str().c_str());
		return;
	}

	// const_cast because the SrQuat does validation (no const version of value())
	const SrQuat& quat = (const_cast<SkJoint*>(joint))->quat()->value();
	gwiz::euler_t euler( gwiz::quat_t( quat.w, quat.x, quat.y, quat.z ) );
	// Marcus's mappings
	auto p = (float)euler.x();
	auto h = (float)euler.y();
	auto r = (float)euler.z();
	this->wo_cache.p = p;
	this->wo_cache.h = h;
	this->wo_cache.r = r;
}

const std::string& SbmPawn::getGeomObjectName()
{
	return collisionObjName;
}

SBGeomObject* SbmPawn::getGeomObject()
{	
	//return _collisionObject;
	SmartBody::SBCollisionManager* colManager = _scene.getCollisionManager();
	SBGeomObject* geomObj = colManager->getCollisionObject(collisionObjName);
	return geomObj;
} 

// void SbmPawn::setGeomObject(SBGeomObject* object)
// {	
// 	if (_collisionObject)
// 		delete _collisionObject;
// 
// 	_collisionObject = object;
// 	_collisionObject->attachToObj(this);
// }


void SbmPawn::updateToColObject()
{
	auto* sbpawn = dynamic_cast<SmartBody::SBPawn*>(this);
	SmartBody::SBPhysicsObj* phyObj = sbpawn->getPhysicsObject();
	if (phyObj)
	{
		SRT newWorldState; 
		newWorldState.gmat(get_world_offset());		
		phyObj->setGlobalTransform(newWorldState);		
		phyObj->updatePhySim();					
	}
}

// void SbmPawn::setPhysicsSim( bool enable )
// {
// 	if (!phyObj_p)
// 		return;
// 
// 	phyObj_p->enablePhysicsSim(enable);	
// }
// 
// bool SbmPawn::hasPhysicsSim()
// {
// 	if (!phyObj_p)
// 		return false;
// 
// 	return phyObj_p->hasPhysicsSim();
// }
// 
// void SbmPawn::setCollision( bool enable )
// {
// 	if (!phyObj_p)
// 		return;
// 
// 	phyObj_p->enableCollisionSim(enable);
// }

///////////////////////////////////////////////////////////////////////////
//  Private sbm_pawn functions



std::string SbmPawn::getClassType()
{
	return _classType;
}

void SbmPawn::setClassType(std::string classType)
{
	_classType = std::move(classType);
}


void SbmPawn::notify(SBSubject* subject)
{

}

SBAPI void SbmPawn::copy( SbmPawn* orignalPawn )
{
	globalTransform = orignalPawn->globalTransform;
	_classType = orignalPawn->_classType;
}

