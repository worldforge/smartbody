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


#include <controllers/me_ct_curve_writer.hpp>
#include <sb/SBPawn.h>
#include <sb/SBUtilities.h>

std::string MeCtCurveWriter::TYPE = "MeCtCurveWriter";

///////////////////////////////////////////////////////////////////////////////

void MeCtCurveWriter::init(						   
	SmartBody::SBPawn* pawn,
	SkChannelArray& channels,
	int left_bound , 
	int right_bound, 
	bool at_least_once
)	{
	
	_channels.init();
	_channels.merge( channels );
//	_channels.compress();
	_channels.rebuild_hash_table();
	
	num_curves = _channels.floats();
	curve_arr = new srLinearCurve[ num_curves ];
	write_once_arr = new bool[ num_curves ];

	for( int i=0; i<num_curves; i++ )	{
		curve_arr[ i ].set_boundary_mode( left_bound, right_bound );
		write_once_arr[ i ] = at_least_once;
	}
	tail_bound_mode = right_bound;
	
	const int size = _channels.size();
	_local_ch_to_buffer.size( size );	// ends up being a 1 to 1 mapping...
	int index = 0;
	for( int i=0; i<size; ++ i ) {
		_local_ch_to_buffer[i] = index;
		int chan_size = SkChannel::size( _channels.type( i ) );
		if( chan_size > 1 ) {
			SmartBody::util::log( "MeCtCurveWriter::init ERR: channel '%s' not a scalar", _channels.name( i ).c_str() );
		}
		index += chan_size;
	}

	MeController::init (pawn);
}

bool MeCtCurveWriter::controller_evaluate( double time, MeFrameData& frame )	{

	if( time < 0.0 )	{
		return( true );
	}
	if( num_curves < 1 )	{
		return( true );
	}
	
	SkChannelArray& channels = controller_channels();
	SrBuffer<float>& frame_buffer = frame.buffer();

	for( int i=0; i<num_curves; i++ )	{

		int context_ch = _toContextCh[i];
		if( context_ch != -1 ) {

			int buff_index = _local_ch_to_buffer[i];
			int frame_buffer_index = frame.toBufferIndex( context_ch );

			bool cropped = false;
			float val = (float)curve_arr[ buff_index ].evaluate( time, & cropped );
			if( cropped )	{
				if( time > curve_arr[ i ].get_tail_param() )	{
					if( write_once_arr[ i ] )	{
						frame_buffer[ frame_buffer_index ] = val;
						write_once_arr[ i ] = false;
					}
				}
			}
			else	{
				frame_buffer[ frame_buffer_index ] = val;
				write_once_arr[ i ] = false;
			}
		}
	}
	return( true );
}

double MeCtCurveWriter::controller_duration()	{

	if( tail_bound_mode == srLinearCurve::CROP ) {
		double max_dur = 0.0;
		for( int i=0; i<num_curves; i++ )	{
			double dur = curve_arr[ i ].get_tail_param();
			if( dur > max_dur ) {
				max_dur = dur;
			}
		}
		return( max_dur );
	}
	return( -1.0 );
}

///////////////////////////////////////////////////////////////////////////////
