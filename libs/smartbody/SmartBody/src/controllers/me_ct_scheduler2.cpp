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



#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <sk/sk_skeleton.h>

#include <controllers/me_ct_examples.h>
#include <controllers/me_ct_scheduler2.h>
#include <controllers/me_ct_blend.hpp>
#include <controllers/me_ct_time_shift_warp.hpp>
#include <controllers/me_ct_motion.h>
#include <controllers/me_ct_interpolator.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBScene.h>
#include "SBUtilities.h"
#include <controllers/me_prune_policy.hpp>


using namespace std;


#define DEBUG_INSPECT_CHANNELS       (0)  // Enable the runtime debugger inspection of channel name/types


const float MeCtScheduler2::MAX_TRACK_DURATION = 4194303.0; // Calculated to give 1 second precision with floats



// Copied from http://www.boost.org/doc/libs/1_38_0/libs/smart_ptr/sp_techniques.html#static
// for http://www.boost.org/doc/libs/1_38_0/libs/smart_ptr/sp_techniques.html#weak_without_shared
// used by MeCtScheduler2::_self
struct null_deleter
{
    void operator()(void const *) const
    {
    }
};




//===== MeCtScheduler2::Context ==============================

std::string MeCtScheduler2::Context::CONTEXT_TYPE = "MeCtScheduler2::Context";

MeCtScheduler2::Context::Context( MeCtScheduler2* schedule )
:	MeCtContainer::Context( schedule ),
	_local_frame( this )   // Ignore Warning: No writes, just passing a reference
{}

void MeCtScheduler2::Context::child_channels_updated( MeController* child ) {
	if( _container == nullptr )
		return; // MeCtScheduler2 was deleted.

	auto* schedule = static_cast<MeCtScheduler2*>(_container);

	// Does Schedule have a context yet?
	MeControllerContext* parent_context = schedule->_context;
	//if( parent_context==nullptr )
	//	return;  // Ignore...

	// Verify it is a child...
	if( schedule->_child_to_track.find(child) ==schedule->_child_to_track.end() )
		return; // Not a known child.  Ignore...

	SkChannelArray& child_channels = child->controller_channels();
	SkChannelArray& sched_channels = schedule->_channels;
	int floats_before = sched_channels.floats();
	bool channelsAdded = sched_channels.merge( child_channels );

	if (channelsAdded)
	{
		child->remap();
		sched_channels.rebuild_hash_table();
		if( parent_context )
			parent_context->child_channels_updated( schedule );
	}
	else
	{
	
	}
//	if( sched_channels.floats() == floats_before ) {
//		child->remap();
//	} else {
//		sched_channels.rebuild_hash_table();
//		if( parent_context )
//			parent_context->child_channels_updated( schedule );
//	}
}

void MeCtScheduler2::Context::remove_controller( MeController* child ) {
	MeCtContainer::Context::remove_controller( child );
}

//======= MeCtScheduler2::Track ==============================

// Constructor
MeCtScheduler2::Track::Track( boost::intrusive_ptr<MeCtUnary> blending_ct,
							  boost::intrusive_ptr<MeCtUnary> timing_ct,
							  boost::intrusive_ptr<MeController> animation_ct )
:	_blending_ct( std::move(blending_ct) ),
	_timing_ct( std::move(timing_ct) ),
	_animation_ct( std::move(animation_ct) ),
	_root( _animation_ct )
{
	// _animation_ct should never be nullptr
	if( _timing_ct ) {
		_timing_ct->init( _root );
		_root = _timing_ct;
	}
	if( _blending_ct ) {
		_blending_ct->init( _root );
		_root = _blending_ct;
	}
}

// Destructor
MeCtScheduler2::Track::~Track() {
	// starting with shallowest, remove children and unref
	if( _blending_ct ) {
		_blending_ct = nullptr;
	}
	if( _timing_ct ) {
		_timing_ct = nullptr;
	}
	if( _animation_ct ) {
		//printf("delete track animation ct = %s\n",_animation_ct->name());
		_animation_ct = nullptr;
	}
	_root = nullptr;
}

// Copy Assignment Operator
MeCtScheduler2::Track& MeCtScheduler2::Track::operator=( const MeCtScheduler2::Track& other ) {
	// Assume proper initialization from other controller
	if( _blending_ct != other._blending_ct ) {
		_blending_ct = other._blending_ct;
	}
	if( _timing_ct != other._timing_ct ) {
		_timing_ct = other._timing_ct;
	}
	if( _animation_ct != other._animation_ct ) {
		_animation_ct = other._animation_ct;
	}
	_root = other._root;

    return *this;
}

MeController* MeCtScheduler2::Track::animation_parent_ct() {
	MeCtScheduler2Ptr schedule = _schedule_weak.lock();

	MeController* parent = nullptr;
	if( schedule ) {
		parent = schedule.get();
		if( _blending_ct != nullptr )
			parent = _blending_ct.get();
		if( _timing_ct != nullptr )
			parent = _timing_ct.get();
	}

	return parent;
}

bool MeCtScheduler2::Track::evaluate( double time, MeFrameData& frame ) {
	_root->evaluate( time, frame );
	return _root->active();
}

//// No longer needed (Compare TrackPtrs instead)
//bool MeCtScheduler2::Track::operator==( const MeCtScheduler2::Track& other ) const {
//	return ( &_schedule_weak==&(other._schedule_weak) )
//		&& _blending_ct==other._blending_ct
//		&& _timing_ct==other._timing_ct
//		&& _animation_ct==other._animation_ct;
//}

void MeCtScheduler2::Track::remap() {
	if( _root ) {
		//  TODO: Remap Track's _context
		_root->remap();
	}
}

//=================================== MeCtScheduler2 =====================================

std::string MeCtScheduler2::type_name = "MeCtScheduler2";

MeCtScheduler2::MeCtScheduler2 (SmartBody::SBPawn& pawn)
:	MeCtContainer( pawn, new MeCtScheduler2::Context( this ) ),   // Ignore Warning: No writes, just passing a reference
	 _self( this, null_deleter() ), // See: http://www.boost.org/doc/libs/1_38_0/libs/smart_ptr/sp_techniques.html#weak_without_shared
	 _sub_sched_context( dynamic_cast<MeCtScheduler2::Context*>( _sub_context.get()) )
{
}

MeCtScheduler2::~MeCtScheduler2 () {

   //SmartBody::util::log("delete scheduler %s\n",this->getName().c_str());
   stop (getScene().getSimulationManager()->getTime());
   //clear();
   //remove_tracks(_tracks);
}

MeController* MeCtScheduler2::child( size_t n ) {
	if( n < _tracks.size() )
		return _tracks[ n ]->_root.get();
	else
		return nullptr;
}

void MeCtScheduler2::context_updated() {
	_sub_sched_context->context( _context );
}

MeCtScheduler2::TrackPtr MeCtScheduler2::create_track( MeCtUnary* blending,
                                                       MeCtUnary* timing,
                                                       MeController* animation,
                                                       const MeCtScheduler2::TrackPtr& before_track )
{
	VecOfTrack::iterator pos;
	if( before_track ) {
		pos = pos_of_track( before_track );
		if( pos == _tracks.end() )
			return TrackPtr();
	} else {
		pos = _tracks.end();
	}

	TrackPtr track( new Track( blending, timing, animation ) );
	track->_schedule_weak = MeCtScheduler2WeakPtr( _self );

	_tracks.insert( pos, track );

	auto& root = track->_root;

	_child_to_track.insert( make_pair( root.get(), track ) );
	if( animation )
		_anim_to_track.insert( make_pair( track->_animation_ct.get(), track ) );

	_sub_sched_context->add_controller( root.get() );
	_sub_sched_context->child_channels_updated( root.get() );

	return track;
}

// This schedule function define arbitary blending curve
// numKeys: number of Knots used to define the curve
// curveInfo: knots information, each knot is composed of:
//	two floats:  time weight
//	four floats:  time weight slope-in slope-out
MeCtScheduler2::TrackPtr MeCtScheduler2::schedule( 
	MeController* ct, 
	double tin, 
	const float* curveInfo,
	int numKeys,
	int numKeyParams
){	
	auto* timingCt   = new MeCtTimeShiftWarp(_pawn, ct );
	auto*         blendingCt = new MeCtBlend(_pawn, timingCt );

	const char* ct_name = ct->getName().c_str();

	srLinearCurve& blend_curve = blendingCt->get_curve();
	for (int i = 0; i < numKeys; i++)
	{
		float t = curveInfo[ i * numKeyParams + 0 ];
		float w = curveInfo[ i * numKeyParams + 1 ];
		blend_curve.insert( tin + t, w );		
	}

	double dur = curveInfo[ ( numKeys - 1 ) * numKeyParams ];

	if( ct_name && (ct_name[0]!='\0') ) {
		string blend_name( "blending for " );
		blend_name += ct_name;
		blendingCt->setName( blend_name );
	}

	// Configure time mapping
	srLinearCurve& time_warp = timingCt->get_curve();
	time_warp.insert( tin, 0.0 );
	time_warp.insert( tin + dur, dur );

	if( ct_name && (ct_name[0]!='\0') ) {
		string timing_name( "timing for " );
		timing_name += ct_name;
		timingCt->setName( timing_name );
	}
	return create_track( blendingCt, timingCt, ct );
}

MeCtScheduler2::TrackPtr MeCtScheduler2::schedule( MeController* ct, double tin, double tout, float indt, float outdt )
{
	double ct_dur = ct->controller_duration();
	bool dur_defined = (ct_dur >= 0);
	//double tout = tin + dur;  // Only used if duration is defined.  See below.

	auto* timingCt   = new MeCtTimeShiftWarp(_pawn, ct );
	auto*         blendingCt = new MeCtBlend(_pawn, timingCt );

	const char* ct_name = ct->getName().c_str();

	// Configure blend curve
	srLinearCurve& blend_curve = blendingCt->get_curve();
	if( indt>0 ) {
		blend_curve.insert( tin,        0.0 );
		blend_curve.insert( tin+indt,   1.0 );
	} else {
		blend_curve.insert( tin, 0.0 );
		blend_curve.insert( tin, 1.0 );
	}
	if( dur_defined ) {
		if( outdt > 0 ) {
			blend_curve.insert( tout - outdt, 1.0 );
			blend_curve.insert( tout,       0.0 );
		} else {
			blend_curve.insert( tout, 1.0 );
			blend_curve.insert( tout, 0.0 );
		}
	}

	if( ct_name && (ct_name[0]!='\0') ) {
		string blend_name( "blending for " );
		blend_name += ct_name;
		blendingCt->setName( blend_name );
	}

	// Configure time mapping
	srLinearCurve& time_warp = timingCt->get_curve();
	time_warp.insert( tin, 0.0 );
	if( dur_defined ) {
		time_warp.insert( tout, ct_dur);
	} else {
		time_warp.insert( MAX_TRACK_DURATION, MAX_TRACK_DURATION-tin );
	}

	if( ct_name && (ct_name[0]!='\0') ) {
		string timing_name( "timing for " );
		timing_name += ct_name;
		timingCt->setName( timing_name );
	}

	return create_track( blendingCt, timingCt, ct );
}

MeCtScheduler2::TrackPtr MeCtScheduler2::schedule( MeController* ct, ScheduleData& scheduleData)
{
//	double startAt  = syncPoints.sync_start()->time();
//	double readyAt  = syncPoints.sync_ready()->time();
//	double strokeStartAt = syncPoints.sync_stroke_start()->time();
//	double strokeAt = syncPoints.sync_stroke()->time();
//	double strokeEndAt = syncPoints.sync_stroke_end()->time();
//	double relaxAt  = syncPoints.sync_relax()->time();
//	double endAt    = syncPoints.sync_end()->time();

	double startAt  = scheduleData.startAt;
	double readyAt  = scheduleData.readyAt;
	double strokeStartAt = scheduleData.strokeStartAt;
	double strokeAt = scheduleData.strokeAt;
	double strokeEndAt = scheduleData.strokeEndAt;
	double relaxAt  = scheduleData.readyAt;
	double endAt    = scheduleData.endAt;
	
	double now = getScene().getSimulationManager()->getTime();;

	// if any of the sync points begin before the current time, 
	// then offset the motion accordingly

	double indt  = readyAt - startAt;
	double outdt = endAt - relaxAt;

	double blendStartFirst = startAt;
	double blendStartSecond = readyAt;

	// if some sync points come before 'now', 
	// make sure that the indt fits with the next sync point
	if (startAt < now) {
		if (readyAt < now) {
			if (strokeStartAt < now) {
				if (strokeAt < now) {
					if (strokeEndAt < now) {
						if (relaxAt < now) {
							if (endAt < now) {
								// do not motion...
							} else {
								if (endAt - now < indt)
									indt = endAt - now;
								
							}
						} else {
							if (relaxAt - now < indt)
								indt = relaxAt - now;
						}
					} else {
						if (strokeEndAt - now < indt)
							indt = strokeEndAt - now;
					}
				} else {
					if (strokeAt - now < indt)
						indt = strokeAt - now;				
				}
			} else {
				if (strokeStartAt - now < indt)
					indt = strokeStartAt - now;				
			}
		} else {
			if (readyAt - now < indt)
					indt = readyAt - now;				
		}

		// if the motion has one or more sync points < now, change the blend times
		blendStartFirst = now;
		blendStartSecond = now + max(indt, readyAt - startAt);
	}

	double ct_dur = ct->controller_duration();
	bool dur_defined = (ct_dur >= 0);
	//double tout = tin + dur;  // Only used if duration is defined.  See below.

	auto* timingCt   = new MeCtTimeShiftWarp(_pawn, ct );
	auto*         blendingCt = new MeCtBlend(_pawn, timingCt );

	const char* ct_name = ct->getName().c_str();

	auto* motionController = dynamic_cast<MeCtMotion*>(ct);
	
	// Configure blend curve
	srLinearCurve& blend_curve = blendingCt->get_curve();
	if( ( indt > 0 ) || ( blendStartSecond > blendStartFirst ) ) {
		blend_curve.insert( blendStartFirst,   0.0 );
		blend_curve.insert( blendStartSecond,   1.0 );
	} else {
		blend_curve.insert(blendStartFirst - .001, 0.0);
		blend_curve.insert( blendStartFirst, 1.0 );
	}
	if( dur_defined ) {
		if (motionController && 
			motionController->isGesture())
		{
			if (fabs(endAt - relaxAt) < .001)
			{
				blend_curve.insert(relaxAt, 1.0);
				blend_curve.insert(relaxAt + .001, 0.0);
			}
			else
			{
				blend_curve.insert(relaxAt, 1.0);
				blend_curve.insert(endAt, 0.0);
			}
		}
		else if (outdt > 0 ) {
			blend_curve.insert( relaxAt, 1.0 );
			blend_curve.insert( endAt,    0.0 );
		} else {
			blend_curve.insert(endAt - .001, 1.0);
			blend_curve.insert( endAt, 0.0 );
		}
	}

	if( ct_name && (ct_name[0]!='\0') ) {
		string blend_name( "blending for " );
		blend_name += ct_name;
		blendingCt->setName( blend_name );
	}

	// Configure time mapping
	srLinearCurve& time_warp = timingCt->get_curve();

	if (motionController)
	{
		SkMotion* skMotion = motionController->motion();
		if (!dur_defined ) {
			time_warp.insert( startAt,	0.0 );
			time_warp.insert( MAX_TRACK_DURATION, MAX_TRACK_DURATION - startAt );
		} else {
			double time_scale = 1.0 / motionController->twarp();
			double time_offset = motionController->offset();
			// following piece of code detects if we need to truncate the motion 
			// note: you can only cut till stroke for now
			int counter = 0;
			double marker = startAt;
			double cut = 0;
			if (fabs(startAt - readyAt) < gwiz::epsilon4())
			{
				counter++;
				marker = strokeStartAt;
				cut = skMotion->time_ready();// - skMotion->time_start();
			}
			if (fabs(startAt - strokeStartAt) < gwiz::epsilon4())
			{
				counter++;
				marker = strokeAt;
				cut = skMotion->time_stroke_start();// - skMotion->time_start();
			}
			if (fabs(startAt - strokeAt) < gwiz::epsilon4())
			{
				counter++;
				marker = strokeEndAt;
				cut = skMotion->time_stroke_emphasis();// - skMotion->time_start();
			}
			if (motionController->isGesture())
			{
				// lay the gesture controller on the time track without any warping
				time_warp.insert(strokeStartAt - (skMotion->time_stroke_start() - skMotion->time_start()), skMotion->time_start());
				time_warp.insert(strokeStartAt - (skMotion->time_stroke_start() - skMotion->time_ready()), skMotion->time_ready());
				time_warp.insert(strokeStartAt, skMotion->time_stroke_start());
				time_warp.insert(strokeAt, skMotion->time_stroke_emphasis());
				time_warp.insert(strokeEndAt, skMotion->time_stroke_end());
				time_warp.insert(strokeEndAt + (skMotion->time_relax() - skMotion->time_stroke_end()), skMotion->time_relax());
				time_warp.insert(strokeEndAt + (skMotion->duration() - skMotion->time_stroke_end()), skMotion->duration());
			}
			else
			{
				if (counter > 0 && counter <= 3)
				{
					blend_curve.clear();
					blend_curve.insert(startAt, 0.0f);
					blend_curve.insert(marker, 1.0f);
					blend_curve.insert(relaxAt, 1.0f);
					blend_curve.insert(endAt, 0.0f);


					time_warp.insert(startAt, cut);
					if (counter <= 1)
					{
						time_warp.insert(strokeStartAt, skMotion->time_stroke_start());
					}
					if (counter <= 2)
					{
						time_warp.insert(strokeAt, skMotion->time_stroke_emphasis());
					}
					time_warp.insert(strokeEndAt, skMotion->time_stroke_end());
					time_warp.insert(relaxAt, skMotion->time_relax());
					time_warp.insert(endAt, skMotion->duration());

					motionController->offset(cut);
				}
				else
				{
					time_warp.insert(startAt, skMotion->time_start());
					//time_warp.insert( readyAt,			motionController->time_ready() );
					//time_warp.insert( strokeStartAt,	motionController->time_stroke_start() );
					//time_warp.insert( strokeAt,			motionController->time_stroke_emphasis() );
					//time_warp.insert( strokeEndAt,		motionController->time_stroke_end() );
					//time_warp.insert( relaxAt,			motionController->time_relax() );
					//time_warp.insert( endAt,	ct_dur );
					time_warp.insert(readyAt, skMotion->time_ready());
					time_warp.insert(strokeStartAt, skMotion->time_stroke_start());
					time_warp.insert(strokeAt, skMotion->time_stroke_emphasis());
					time_warp.insert(strokeEndAt, skMotion->time_stroke_end());
					time_warp.insert(relaxAt, skMotion->time_relax());
					time_warp.insert(endAt, skMotion->duration());
				}
			}
		}
	}
	else
	{
		auto* nod = dynamic_cast<MeCtSimpleNod*>(ct);
		if (nod)
		{
			time_warp.insert( startAt,	0.0 );
			time_warp.insert( endAt,	ct_dur );
		}
	}

	if( ct_name && (ct_name[0]!='\0') ) {
		string timing_name( "timing for " );
		timing_name += ct_name;
		timingCt->setName( timing_name );
	}

	//SmartBody::util::log("[%s] Blend curve:", this->getName().c_str());
	//blendingCt->get_curve().print();
	//SmartBody::util::log("[%s] Timewarp curve:", this->getName().c_str());
	//timingCt->get_curve().print();
	return create_track( blendingCt, timingCt, ct );
}


MeCtScheduler2::TrackPtr MeCtScheduler2::schedule( MeController* ct1, MeController* ct2, float value, bool loop, ScheduleData& scheduleData)
{
//	double startAt  = syncPoints.sync_start()->time();
//	double readyAt  = syncPoints.sync_ready()->time();
//	double strokeStartAt = syncPoints.sync_stroke_start()->time();
//	double strokeAt = syncPoints.sync_stroke()->time();
//	double strokeEndAt = syncPoints.sync_stroke_end()->time();
//	double relaxAt  = syncPoints.sync_relax()->time();
//	double endAt    = syncPoints.sync_end()->time();

	double startAt  = scheduleData.startAt;
	double readyAt  = scheduleData.readyAt;
	double strokeStartAt = scheduleData.strokeStartAt;
	double strokeAt = scheduleData.strokeAt;
	double strokeEndAt = scheduleData.strokeEndAt;
	double relaxAt  = scheduleData.readyAt;
	double endAt    = scheduleData.endAt;

	// if any of the sync points begin before the current time, 
	// then offset the motion accordingly

	double indt  = readyAt - startAt;
	double outdt = endAt - relaxAt;

	auto* interpolator = new MeCtInterpolator(_pawn, ct1, ct2, getScene().getSimulationManager()->getTime(), double(value), loop);
	double ct_dur = interpolator->controller_duration();
	bool dur_defined = (ct_dur >= 0);

	auto* timingCt   = new MeCtTimeShiftWarp(_pawn, interpolator );
	auto*         blendingCt = new MeCtBlend(_pawn, timingCt );

	srLinearCurve& blend_curve = blendingCt->get_curve();
	if( indt>0 ) {
		blend_curve.insert( startAt,        0.0 );
		blend_curve.insert( readyAt,		1.0 );
	} else {
		blend_curve.insert( startAt, 1.0 );
	}
	if( dur_defined ) {
		if( outdt>0 ) {
			blend_curve.insert( startAt + ct_dur - outdt,		 1.0 );
			blend_curve.insert( startAt + ct_dur,				 0.0 );
		} else {
			blend_curve.insert( startAt + ct_dur, 0.0 );
		}
	}

	const char* ct_name = interpolator->getName().c_str();
	if( ct_name && (ct_name[0]!='\0') ) {
		string blend_name( "blending for " );
		blend_name += ct_name;
		blendingCt->setName( blend_name );
	}

	// Configure time mapping
	srLinearCurve& time_warp = timingCt->get_curve();
	if (!dur_defined ) 
	{
		time_warp.insert( startAt, 0.0 );
		time_warp.insert( MAX_TRACK_DURATION, MAX_TRACK_DURATION - startAt );
	} 
	else 
	{
		time_warp.insert( startAt, 0.0 );
		time_warp.insert( startAt + ct_dur, ct_dur );
	}

	if( ct_name && (ct_name[0]!='\0') ) {
		std::string timing_name( "timing for " );
		timing_name += ct_name;
		timingCt->setName( timing_name );
	}

	return create_track( blendingCt, timingCt, interpolator );
}



bool MeCtScheduler2::remove_child( MeController *child ) {
	bool result = remove_child_impl( child );
	if( result )
	{
		recalc_channels_requested();
	}

	return result;
}

bool MeCtScheduler2::remove_track( const MeCtScheduler2::TrackPtr& track ) {
	bool result = remove_track_impl( track );
	if( result )
		recalc_channels_requested();

	return result;
}

void MeCtScheduler2::remove_tracks( vector< MeCtScheduler2::TrackPtr >& tracks ) {
	auto it = tracks.begin();
	auto end = tracks.end();

	bool did_remove = false;

	for( ; it != end; ++it ) {
		did_remove |= remove_track_impl( *it );
	}

	if( did_remove )
		recalc_channels_requested();
}

// protected
MeCtScheduler2::VecOfTrack::iterator MeCtScheduler2::pos_of_track( const TrackPtr& track ) {
	return find( _tracks.begin(), _tracks.end(), track );
}

bool MeCtScheduler2::remove_child_impl( MeController *child ) {
	auto map_it = _child_to_track.find( child );
	bool result = (map_it != _child_to_track.end());
	if( result ) {
		TrackPtr track( map_it->second );
		
		auto pos = pos_of_track( track );
		if( pos != _tracks.end() )
			_tracks.erase( pos );

		_child_to_track.erase( child );
		_anim_to_track.erase( track->_animation_ct.get() );

		// Call this last, because it may recurse via the MeControllerContext
		_sub_sched_context->remove_controller( track->_root.get() );
	}

	return result;
}

bool MeCtScheduler2::remove_track_impl( const TrackPtr& track ) {
	bool result = false;

	if( track ) {
		auto pos = pos_of_track( track );
		if( pos != _tracks.end() ) {
			result = true;
			_tracks.erase( pos );

			if( track->_root != nullptr ) {
				_child_to_track.erase( track->_root.get() );
				_anim_to_track.erase( track->_animation_ct.get() );

				// Call this last, because it may recurse via the MeControllerContext
				_sub_sched_context->remove_controller( track->_root.get() );
			}

			// Clear the back reference to the scheduler
			track->_schedule_weak.reset();
		}
	}

	return result;
}

// protected
void MeCtScheduler2::recalc_channels_requested() {
	if( _tracks.empty() ) {
		_channels.init();
	} else {
		// TODO: recalculate channels requested
	}
}


void MeCtScheduler2::clear () {
	auto it = _tracks.begin();
	auto end = _tracks.end();

	for( ; it != end; ++it ) {
		TrackPtr track( *it );

		if( track->_root != nullptr )
			remove_child( track->_root.get() );

		track->_schedule_weak.reset();
	}

	_tracks.clear();
	_child_to_track.clear();
	_anim_to_track.clear();

	recalc_channels_requested();
}

MeCtScheduler2::TrackPtr MeCtScheduler2::track_for_child( MeController* ct )
{
	auto i = _child_to_track.find(ct);
	if( i==_child_to_track.end() )
		return TrackPtr();
	else
		return i->second;
}

MeCtScheduler2::TrackPtr MeCtScheduler2::track_for_anim_ct( MeController* ct )
{
	auto i = _anim_to_track.find(ct);
	if( i==_anim_to_track.end() )
		return TrackPtr();
	else
		return i->second;
}

MeCtScheduler2::VecOfTrack MeCtScheduler2::tracks() {
	return VecOfTrack( _tracks );
}


//----- virtuals -----

void MeCtScheduler2::controller_map_updated() {
	// Clear previous data
	_local_ch_to_buffer.assign( _channels.size(), -1 );

	// Map logged channels
	if( _context ) {

		SkChannelArray& local_channels = controller_channels();
		const int num_channels = local_channels.size();
		for( int i=0; i<num_channels; ++i ) {
			int parent_index = _toContextCh[i];

			if( parent_index >= 0 ) {
				
				_local_ch_to_buffer[i] = _context->toBufferIndex( parent_index );

			}
		}
	}

	// remap children
	for(const auto& track : _tracks) {
			// TrackPtr in _tracks should not be null
		auto& ct = track->_root;
		if( ct )
			ct->remap();
	}
}


////  Predicate for uniniting past Tracks
//class TrackFinalizer : public std::unary_function<MeCtScheduler2::Track,bool> {
//    const double time; // time
//public:
//    TrackFinalizer( const double time )
//        : time( time )
//    {}
//
//    bool operator() ( MeCtScheduler2::Track& tr ) const {
//        if( tr._type==MeCtScheduler2::Once && 
//            tr._tout>=0 && 
//            time>tr._tout+tr._ext )
//        {
//            tr.controller()->stop();
//            tr.uninit();
//            return true;
//        } else {
//            return false;
//        }
//    }
//};



bool MeCtScheduler2::controller_evaluate( double time, MeFrameData& frame ) {
	vector< TrackPtr > to_remove( 0 );  // zero size because it is rarely used (don't allocate for array until needed)

	auto end = _tracks.end();

	for( auto i = _tracks.begin(); i != end; ++i )
	{
		TrackPtr track = *i;
		bool result = track->evaluate( time, frame );
		if (!result) {
			to_remove.emplace_back(track);
		}
	}

	if( !to_remove.empty() )
		remove_tracks( to_remove );
	
	return _active_when_empty || !_tracks.empty();
}

SkChannelArray& MeCtScheduler2::controller_channels ()
{
   return _channels;
}

double MeCtScheduler2::controller_duration () {
	double total_dur=0;

	auto end = _tracks.end();
	for( auto it = _tracks.begin(); it != end; ++it ) {
		TrackPtr track = (*it);

		double ct_dur = track->_root->controller_duration();
		if ( ct_dur<0 ) {
			total_dur = -1;

			break;
		} else {
			total_dur = max( ct_dur, total_dur );
		}
	}
	return total_dur;
}

const std::string& MeCtScheduler2::controller_type () const
 {
   return type_name;
 }


//======================================= EOF =====================================
