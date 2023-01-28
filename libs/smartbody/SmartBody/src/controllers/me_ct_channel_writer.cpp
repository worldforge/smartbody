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


#include <controllers/me_ct_channel_writer.hpp>

#include <sstream>
#include <sb/SBPawn.h>


std::string MeCtChannelWriter::TYPE = "MeCtChannelWriter";



MeCtChannelWriter::MeCtChannelWriter(SmartBody::SBPawn& pawn, SkChannelArray& channels, bool continuous)
:	SmartBody::SBController(pawn),
	_continuous( false ),
	_write_next( false )
{
	_channels.init();
	_channels.merge( channels );
//	_channels.compress();  // save memory
	_channels.rebuild_hash_table();

	_data.size( _channels.floats() );
	_data.setall(0.0);

	const int size = _channels.size();
	_local_ch_to_buffer.size( size );
	int index = 0;
	for( int i=0; i<size; ++ i ) {
		_local_ch_to_buffer[i] = index;
		index += SkChannel::size( _channels.type(i) );
	}

	_continuous = continuous;
	_write_next = false;  // Don't write until the data is set

}

const std::string& MeCtChannelWriter::controller_type() const {
	return MeCtChannelWriter::TYPE;
}


bool MeCtChannelWriter::set_data( SrBuffer<float> data ) {
	const int size = data.size();
	if( size != _data.size() )
		return false;
	for( int i=0; i<size; ++i )  // slow approach
		_data[i] = data[i];

	_write_next = true;
	
	return true;
}

void MeCtChannelWriter::set_data( const float data[] ) {
	const int size = _data.size();
	for( int i=0; i<size; ++i )  // slow approach
		_data[i] = data[i];

	_write_next = true;
}

SrBuffer<float>& MeCtChannelWriter::get_data() {
	return _data;
}

SkChannelArray& MeCtChannelWriter::controller_channels() {
	return _channels;
}

double MeCtChannelWriter::controller_duration() {
	return -1;
}

bool MeCtChannelWriter::controller_evaluate( double time, MeFrameData& frame ) {
	if( _write_next ) {  // Do we write this frame? First frame or continuous
		SkChannelArray& channels = controller_channels();
		SrBuffer<float>& frame_buffer = frame.buffer();
		const int size = channels.size();
		for( int i=0; i<size; ++i ) {          // i is the local channels[] index
			int context_ch = _toContextCh[i];  // frames.channels()[ index ]
			if( context_ch != -1 ) {           // Make sure channel exist in the context
#if DEBUG_CHANNELS   // Get a reference to the channel to inspect via debugger
				SkChannel::Type ch_type = channels.type( i );
				const char*     ch_name = (const char*)(channels.name( i ));
#endif
				int buff_index = _local_ch_to_buffer[i];   // Find the local buffer index
				int frame_buffer_index = frame.toBufferIndex( context_ch );  // find the matching context's buffer index

				int fSize = channels[i].size();
				for( int j=0; j<fSize; ++j ) {  // for each float in the channel
					float data = _data[ buff_index + j ];
					frame_buffer[ frame_buffer_index + j ] = data;
				}
				frame.channelUpdated( context_ch );
			} // else ignore
		}

		_write_next = _continuous;
	}

	return true;
}
