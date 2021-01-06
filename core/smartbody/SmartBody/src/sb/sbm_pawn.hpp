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

#ifndef SBM_PAWN_HPP
#define SBM_PAWN_HPP

#include <sb/SBTypes.h>
#include <sk/sk_scene.h>
#include <sk/sk_skeleton.h>
#include <sb/SBColObject.h>
#include <sb/SBPhysicsSim.h>
#include <sb/SBObject.h>
#include "sb/SBSceneOwned.h"
#include "sr/sr_shared_ptr.hpp"
#include <map>
#include <boost/noncopyable.hpp>


// Declare classes used (avoid circular references)
class srArgBuffer;

class MeCtChannelWriter;
class MeControllerTreeRoot;


#define SBM_PAWN_USE_WORLD_OFFSET_WRITER	(1)

class SbmPawn : public SmartBody::SBObject, public SBTransformObjInterface, public SmartBody::SBSceneOwned {
public:
	//  Public Constants
	static const char* WORLD_OFFSET_JOINT_NAME;

private:
	//  Private Constants
	static SkChannelArray WORLD_OFFSET_CHANNELS_P;

protected:
	//  Private Data
	struct {
		float x, y, z;  // translation (x,y,z)
		float h, p, r;  // rotation (heading, pitch, roll)
	} wo_cache;         // Caches values when setting world offset, because controller may not have been evaluated and skeleton value may not reflect the last call to set_world_offset (HACK-ish)
	double wo_cache_timestamp;

	boost::intrusive_ptr<MeCtChannelWriter>  world_offset_writer_p;
	float	_height;
	//SBGeomObject* _collisionObject;
	std::string collisionObjName;

public:  // TODO - properly encapsulate / privatize the following
	boost::intrusive_ptr<SkSkeleton>	_skeleton;  // MAY BE nullptr!!!
	SrSnGroup* blendMeshGroup;
	std::string _classType;
	
	// Temporarily, until there is a unified multi-skeleton controller tree
	boost::intrusive_ptr<MeControllerTreeRoot> ct_tree_p;
	SBTransform                globalTransform;

public:	
	//  Public Methods
//	SBAPI SbmPawn();
	SBAPI explicit SbmPawn(SmartBody::SBScene& scene, const char* name );
	SBAPI virtual ~SbmPawn();

	SBAPI virtual void copy(SbmPawn* orignalPawn);

	SBAPI const boost::intrusive_ptr<SkSkeleton>& getSkeleton() const;
	SBAPI void setSkeleton(boost::intrusive_ptr<SkSkeleton> sk);
	SBAPI virtual int init( boost::intrusive_ptr<SkSkeleton> skeleton_p );

	SBAPI const std::string& getGeomObjectName();
	SBAPI SBGeomObject* getGeomObject(); // get geometry object associated with the pawn
	//void setGeomObject(SBGeomObject* obj);

 	SBAPI void updateToColObject();

	SBAPI bool is_initialized() const;

	SBAPI void setHeight( float height )	{ _height = height; }
	SBAPI float getHeight( ) const 		{ return _height; }
	SBAPI SrBox getBoundingBox( );

	SBAPI virtual int prune_controller_tree();  // removes unused or overwritten controllers
	
	SBAPI void reset_all_channels();

	SBAPI const SkJoint* get_joint( const char* joint_name ) const;
	SBAPI const SkJoint* get_world_offset_joint() const
	{	return get_joint( WORLD_OFFSET_JOINT_NAME ); }

	SBAPI SBTransform& getGlobalTransform() override;
	SBAPI void setGlobalTransform(SBTransform& newGlobalTransform) override;

	SBAPI SrMat get_world_offset();
	SBAPI void get_world_offset( float& x, float& y, float& z,
		                   float& yaw, float& pitch, float& roll );

	SBAPI void setWorldOffset(const SrMat& newWorld);
	SBAPI void set_world_offset( float x, float y, float z,
		                   float yaw, float pitch, float roll );

	SBAPI virtual std::string getClassType();
	SBAPI virtual void setClassType(std::string classType);

	SBAPI void notify(SBSubject* subject) override;

protected:
	/*!
	 *  Initializes the static variable WORLD_OFFSET_CHANNELS_P.
	 *  TODO: When is this deleted?
	 */
	static void init_world_offset_channels();

	/*!
	 *   Modify skeleton, if necessary.
	 *
	 *   SbmPawn inserts world_offset joint above the existing root.
	 */
	virtual int init_skeleton();	

	/*!
	 *   Modify skeleton, if necessary.
	 *
	 *   SbmPawn inserts world_offset joint above the existing root.
	 */
	virtual int setup();

	void initData();


public:
	MeCtChannelWriter* get_world_offset_writer_p()	{return world_offset_writer_p.get();}

	void wo_cache_update();
};

#endif // SBM_PAWN_HPP
