/*
 *  me_ct_scheduler2.h - part of SmartBody-lib's Motion Engine
 *  Copyright (C) 2008  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Andrew n marshall, USC
 */


#ifndef ME_CT_SCHEDULER2_H
#define ME_CT_SCHEDULER2_H

//=================================== MeCtScheduler2 =====================================
#include <sb/SBTypes.h>
#include <map>
#include <vector>


#include <controllers/me_ct_container.hpp>
#include <controllers/me_ct_unary.hpp>

// Use Boost Smart Point Impl until TR1 is finalized
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "sr/sr_shared_ptr.hpp"

/*! The scheduler maps each controller to its internal channel array, so that
    after evaluation, the values are copied and blended into the scheduler buffer */
class MeCtScheduler2;
typedef boost::shared_ptr<MeCtScheduler2> MeCtScheduler2Ptr;
typedef boost::weak_ptr<MeCtScheduler2>   MeCtScheduler2WeakPtr;

class MeCtScheduler2 : public MeCtContainer {
public:
	///////////////////////////////////////////////////////////////////////
	//  Public Constants
	static std::string type_name;

	//  4,194,303 (2^22-1), without a fractional component
	//  can be represented in 32-bit float without rounding
	//  (preserving at least 1 second of precision).
	//  4,194,303 seconds is more than 48 days.  Most likely safe.
	static const float MAX_TRACK_DURATION;


	///////////////////////////////////////////////////////////////////////
	//  Public Inner Classes Pre-Declaration
	class Track; 

protected:
	//////////////////////////////////////////////////////////////////////////
	//  Local per-track Context
	/**
	*  MeControllerContext implementation for child controller.
	*/
	class Context : public MeCtContainer::Context {
		friend class MeCtScheduler2;
	public:
		///////////////////////////////////////////////////////////////
		//  Public Constants
		static std::string CONTEXT_TYPE;

	protected:
		///////////////////////////////////////////////////////////////
		//  Private Data

		//MeCtScheduler2&   _schedule;     // use static_cast<MeCtScheduler2>(_container)
		MeFrameDataProxy  _local_frame;

		///////////////////////////////////////////////////////////////
		//  Constructor
		explicit Context( MeCtScheduler2* schedule );

	public:
		///////////////////////////////////////////////////////////////
		//  Public Methods
		void child_channels_updated( MeController* child ) override;

		const std::string& context_type() const override
		{	return CONTEXT_TYPE; }

		void remove_controller( MeController* child ) override;
	};

public:
	///////////////////////////////////////////////////////////////////////
	//  Public Inner Classes
	class Track {
		friend class MeCtScheduler2;
		friend class MeCtScheduler2::Context;

	///////////////////////////////////////////////////////////////
	//  Private Data
	private:
		boost::weak_ptr<MeCtScheduler2> _schedule_weak;     // Schedule which this track is a part of.

	protected:
		boost::intrusive_ptr<MeCtUnary>      _blending_ct;  // blending controller (usually MeCtBlend)
		boost::intrusive_ptr<MeCtUnary>      _timing_ct;    // timing controller (usually MeCtTimeShiftWarp)
		boost::intrusive_ptr<MeController>   _animation_ct; // source animation controller
		boost::intrusive_ptr<MeController>   _root;         // First controller (ordered blending, timing, animation) that isn't nullptr

    public:
		///////////////////////////////////////////////////////////////
		//  Public Methods

		// default constructor
		Track( boost::intrusive_ptr<MeCtUnary> blending,
		       boost::intrusive_ptr<MeCtUnary> timing,
			   boost::intrusive_ptr<MeController> animation );
		//Track( const Track& );  // copy constructor
		virtual ~Track();

		/*! Copy assignment opperator. */
		Track& operator=( const Track& other );

        /*! returns the blending controller for this Track, possibly nullptr */
		boost::intrusive_ptr<MeCtUnary> blending_ct()  { return _blending_ct; }
        /*! returns the timing controller for this Track, possibly nullptr. */
		boost::intrusive_ptr<MeCtUnary> timing_ct()    { return _timing_ct; }
        /*! returns the source animation controller for this Track */
		boost::intrusive_ptr<MeController> animation_ct() { return _animation_ct; }

		MeController* animation_parent_ct();

		boost::intrusive_ptr<MeController> root() { return _root; }

		bool evaluate( double time, MeFrameData& frame );

		//bool operator==( const MeCtScheduler2::Track& other ) const;

	protected:
		///////////////////////////////////////////////////////////////
		//  Private Methods
		/**  Remaps Track context and child controller.  */
		void remap();

    };
	typedef boost::shared_ptr<Track> TrackPtr;
	typedef std::vector<TrackPtr>            VecOfTrack;

	struct ScheduleData {
		double startAt;
		double readyAt;
		double strokeStartAt;
		double strokeAt;
		double strokeEndAt;
		double relaxAt;
		double endAt;
	};

protected:
	///////////////////////////////////////////////////////////////////////
	//  Private Data Types
	typedef std::map<MeController*,TrackPtr> MapOfCtToTrack;

	//  Private Data

	/**
	 * Reference to self, used to create weak_ptr references for Tracks.
	 *
	 * See: http://www.boost.org/doc/libs/1_38_0/libs/smart_ptr/sp_techniques.html#weak_without_shared
	 * Absolutely not thread safe.
	 */
	MeCtScheduler2Ptr _self;

	MapOfCtToTrack _child_to_track;
	MapOfCtToTrack _anim_to_track;

	/**
	 *  Local context for children.
	 */
	boost::intrusive_ptr<MeCtScheduler2::Context> const _sub_sched_context;

	/**
	 *  Lookup table of buffer values.
	 */
	std::vector<int> _local_ch_to_buffer;

	/**
	 *  Current tracks.
	 *
	 *  Chose list so that iterators remain consistent while adding and 
	 *  removing surrounding tracks.
	 */
    VecOfTrack _tracks;

	/**
	 *  Union of channels for all tracks.
	 */
    SkChannelArray _channels;

	/**
	 *  Option to keep this schedule controller active, even when empty.
	 *  False by default (parent may delete it automatically).
	 */
	bool _active_when_empty;


	///**
	// *  Count of tracks using each channel (maintains the same indices as
	// *  _channels).  Channel union is rebuilt if any ref count drops to 0.
	// */
	//std::vector<unsigned int> _channel_refs;


public:
	///////////////////////////////////////////////////////////////////////
	//  Public Methods

    /*! Constructor */
    explicit MeCtScheduler2(SmartBody::SBPawn& pawn);

    /*! Destructor is public but pay attention to the use of ref()/unref() */
    ~MeCtScheduler2() override;

	/** Accessor for _active_when_empty. */
	bool active_when_empty() const
	{ return _active_when_empty; }

	/** Mutator for _active_when_empty. */
	void active_when_empty( bool new_value )
	{ _active_when_empty = new_value; }

	/**
	 *  Returns number of child controllers
	 *  (which happens to also be the number of tracks).
	 *
	 *  Always equals tracks().
	 */
	size_t count_children() override { return (int)_tracks.size(); }

    /**
     *  Get Nth child
	 *
     *  Returns nullptr for invalid n.
     */
	MeController* child( size_t n ) override;

////  Bad design to share iterators to an encapsulated collection
//	/** Iterator for the first Track, or end() if there are no tracks. */
//	inline track_iterator begin() { return _tracks.begin(); }
//
//	/** Iterator for the position after the last Track */
//	inline track_iterator end() { return _tracks.end(); }

	/**
	 *  Returns the track_iterator for track with the given child controller.
	 *  Returns end() if no matching track is found.
	 */
	TrackPtr track_for_child( MeController* ct );

	/**
	 *  Returns the track_iterator for track with the given controller
	 *  as the animation source.
	 *  Returns end() if no matching track is found.
	 */
	TrackPtr track_for_anim_ct( MeController* ct );

	/**
	 *  Returns a current copy of the tracks.
	 */
	SBAPI VecOfTrack tracks();

	///** Returns a reference to the track index i (0 is at the bottom). Use with care. */
	//Track& track( int i ) { return _tracks.at(i); }

	///** Returns a reference to the top track. Use with care. */
	//Track& toptrack() { return *_tracks.end(); }

    /** Returns the channel array describing the output buffer of the controller */
    SkChannelArray& channels() { return _channels; }

	//  Implements MeController::context_updated()
	void context_updated() override;

	/**
	 *  Schedules a new track at before Track before_pos.
	 *
	 *  If before_pos is nullptr, created track will be appended after all existing tracks.
	 *
	 *  If before_pos is not nullptr and not an active track of this schedule,
	 *  create_track(..) will return a nullptr TrackPtr.
	 */
	TrackPtr create_track( MeCtUnary* blending, MeCtUnary* timing, MeController* ct, const TrackPtr& before_pos );

	/**
	 *  Schedules a new track at position pos.
	 */
	TrackPtr create_track( MeCtUnary* blending, MeCtUnary* timing, MeController* ct )
	{	return create_track( blending, timing, ct, TrackPtr() ); }

    /** Backwards compatible schedule function: (sans Static/Once track type)
	 *
	 *  Schedule controller c with the given parameters.
     *  c - the controller to be scheduled.
     *  tin - the time to activate (start) the controller.
     *  indt - ease-in duration for blending with the prior controller in the stack.
     *  outdt - ease-out duration for blending with the next controller in the stack.
     *  type - Once will automatically remove c after completion, Static will not remove.
	 */
	TrackPtr schedule( MeController* c, double tin, double tout, float indt, float outdt );

	/** Backwards compatible schedule function: (sans Static/Once track type)
	 *
	 *  Schedule controller c with the given parameters.
     *  c - the controller to be scheduled.
     *  tin - the time to activate (start) the controller.
     *  curve - key information that defines the blending curve.
     *  numKeys - number of the knots.
     *  numKeyParams - number of values per key.
	 *	Current Usage for this schedule is for arbitary viseme curve, can be applied to any other blending curve
	 */	
	TrackPtr schedule( MeController* c, double tin, const float* curve, int numKeys, int numKeyParams );

    /** Backwards compatible schedule function: (sans Static/Once track type)
	 *
	 *  Schedule controller c with the given parameters.
     *  c - the controller to be scheduled.
     *  behavior syn points
	 */
	TrackPtr schedule( MeController* c, ScheduleData& scheduleData);

    /** Backwards compatible schedule function: (sans Static/Once track type)
	 *
	 *  Schedule controller c with the given parameters.
     *  ct1, ct2 - the motion controllers to be scheduled into interpolator controller.
	 *  value - the blending weight for ct1 and ct2
     *  type - Once will automatically remove c after completion, Static will not remove.
	 */
	TrackPtr schedule( MeController* ct1, MeController* ct2, float value, bool loop, ScheduleData& scheduleData);

	/**
	 *  Removes the given track from schedule.
	 *  If the track_iterator is recognized and removed successfully,
	 *  the track_iterator is set to end() and the set of requested channels
	 *  is recalculated from remaining children.
	 */
	bool remove_track( const MeCtScheduler2::TrackPtr& track );

	/**
	 *  Removes the given tracks from schedule.
	 *  Each track_iterator that is recognized and removed successfully is set to end().
	 *  If any tracks are removed, the set of requested channels is recalculated from remaining children.
	 *
	 *  Not only does this provide a convience interface for removing several channels,
	 *  it minimizes the number of changes to the channels request, which can propogate
	 *  to other controllers of the controller tree.
	 */
	void remove_tracks( std::vector< MeCtScheduler2::TrackPtr >& tracks );

    /*! Removes all tracks and make the scheduler empty */
    void clear();
    
    /*! Returns the track number of the controller c, or -1 if c is not
        currently scheduled to play */
    int track( MeController* c );
    
protected:
	///////////////////////////////////////////////////////////////////////
	//  Private Methods
	VecOfTrack::iterator pos_of_track( const TrackPtr& track );

	bool remove_child_impl( MeController *child );
	bool remove_track_impl( const MeCtScheduler2::TrackPtr& track );

	void recalc_channels_requested();

	/** 
	 *  Implements MeController::controller_map_updated().
	 *
	 *  Call remap on each track _root.
	 *  Overrides MeCtContainer for efficiency (sort of).
	 */
	void controller_map_updated() override;

	//! Implements MeCtContainer::remove_child(..)
	bool remove_child( MeController* child ) override;

    bool controller_evaluate( double t, MeFrameData& frame ) override;
    SkChannelArray& controller_channels() override;
    double controller_duration() override;
    const std::string& controller_type() const override;


	//
};

//======================================= EOF =====================================

# endif // ME_CT_SCHEDULER2_H


