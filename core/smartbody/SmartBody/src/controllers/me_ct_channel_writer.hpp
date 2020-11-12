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

#ifndef ME_CT_CHANNEL_WRITER_HPP
#define ME_CT_CHANNEL_WRITER_HPP

#include <sb/SBController.h>


/**
 *  Write the given data to the given channels.
 *
 *  If continuous writing is enabled, the data will be written every frame.
 *  If continuous writing is disabled, the data will only be written on the
 *  next frame.
 *
 *  This controller only works with the new controller API.
 */

namespace SmartBody {
	class SBPawn;
}

class MeCtChannelWriter : public SmartBody::SBController {
public:
	// Public Constants
	static std::string TYPE;

protected:
	// Data
	SkChannelArray  _channels;
	SrBuffer<float> _data;
	SrBuffer<int>   _local_ch_to_buffer;
	bool            _continuous;
	bool            _write_next;

public:
	/** Constructor */
	MeCtChannelWriter();

	const std::string& controller_type() const;

	/**
	 *  Initializes the controller with a set of channels to write.
	 */
	void init(SmartBody::SBPawn* pawn, SkChannelArray& channels, bool continuous );

	/**
	 *  Copies data into the current data buffer.
	 *  Returns false if the data length does not match expected.
	 *  Otherwise returns true if successful.
	 */
	bool set_data( SrBuffer<float> data );

	/**
	 *  Copies data into the current data buffer.
	 *  It is up to the caller to make sure the length of data is
	 *  at least this->controller_channels().floats().
	 */
	void set_data( float data[] );

	SrBuffer<float>& get_data();

	/**
	 *  Implements MeController::controller_channels().
	 */
	SkChannelArray& controller_channels();

	/**
	 *  Implements MeController::controller_duration()
	 *  Returns -1, undefined duration.
	 */
	double controller_duration();

	/**
	 *  Implements MeController::controller_evaluate(..).
	 */
	bool controller_evaluate( double time, MeFrameData& frame );
};


#endif // ME_CT_CHANNEL_WRITER_HPP
