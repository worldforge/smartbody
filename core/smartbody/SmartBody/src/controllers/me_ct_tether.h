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


#ifndef ME_CT_TETHER_H
#define ME_CT_TETHER_H

#include <sk/sk_skeleton.h>
#include <sb/SBController.h>

//////////////////////////////////////////////////////////////////////////////////

class MeCtTether : public SmartBody::SBController	{ 

	private:

		double              _duration;  // the time-warped duration
		SkChannelArray		_channels; // override motion channels, to include world_offset
		SkSkeleton* 	    _skeleton_ref_p;
		
		struct joint_state_t	{
//			gwiz::vector_t	parent_pos; // world coord of immediate parent joint
//			quat_t		parent_rot;
			gwiz::vector_t	local_pos;
			gwiz::quat_t		local_rot;
			gwiz::vector_t	world_pos;
			gwiz::quat_t		world_rot;
		};

	public:
		static std::string type_name;

		/*! Constructor */
		MeCtTether( void );

		/*! Destructor is public but pay attention to the use of ref()/unref() */
		virtual ~MeCtTether( void );
		
		void clear( void );

		void init(SmartBody::SBPawn* pawn,  char *channel_name );
		
		// will work with or without a source joint (otherwise world coord offset)
		void set_source_joint( char *joint_name );
		void set_source_joint( SkJoint* joint_p );

		void set_offset_pos( float x, float y, float z );
		void set_offset_euler( float p, float h, float r );
		void set_offset_swing( float sw_p, float sw_h, float tw = 0.0 ); // swing-twist: pitch, heading, roll
		void set_offset_polar( float d, float a, float r = 0.0 );        // polar-coord: direction, radial angle, roll

	private:

		SkJoint*		get_joint( char *joint_str, SkJoint *joint_p );
		SkJoint*		source_ref_joint( void );
		joint_state_t	capture_joint_state( SkJoint *joint_p );
		joint_state_t	calc_channel_state( MeCtTether::joint_state_t source );

		char*		source_ref_joint_str;
		SkJoint*	source_ref_joint_p;

		gwiz::vector_t	offset_pos;
		gwiz::quat_t		offset_rot;

		// callbacks for the base class
		virtual void context_updated( void );
		virtual void controller_map_updated();
		virtual void controller_start();
		virtual bool controller_evaluate ( double t, MeFrameData& frame );
		virtual SkChannelArray& controller_channels ();
		virtual double controller_duration ();
		virtual const std::string& controller_type () const;
};

//////////////////////////////////////////////////////////////////////////////////
#endif // ME_CT_TETHER_H
