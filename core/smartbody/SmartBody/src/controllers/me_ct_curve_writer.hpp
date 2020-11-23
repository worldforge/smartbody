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

#ifndef ME_CT_CURVE_WRITER_HPP
#define ME_CT_CURVE_WRITER_HPP

#include <sb/SBController.h>
#include <sbm/sr_linear_curve.h>
#include <sbm/sr_curve_builder.h>

class MeCtCurveWriter : public SmartBody::SBController {

	public:

		static std::string TYPE;

		MeCtCurveWriter( void )	{
			curve_arr = nullptr;
			num_curves = 0;
			write_once_arr = nullptr;
			tail_bound_mode = 0;
		}
		~MeCtCurveWriter( void )	{
			if( curve_arr ) {
				delete [] curve_arr;
				curve_arr = nullptr;
			}
			if( write_once_arr ) {
				delete [] write_once_arr;
				write_once_arr = nullptr;
			}
		}
	
		void init( 
			SmartBody::SBPawn* pawn,
			SkChannelArray& channels,
			int left_bound = srLinearCurve::CROP, 
			int right_bound = srLinearCurve::CROP, 
			bool at_least_once = true 
		);
		
		void insert_key( int curve_index, double t, double v )	{
			if( curve_index < 0 ) return;
			if( curve_index >= num_curves ) return;
			curve_arr[ curve_index ].insert( t, v );
		}
		void insert_key( double t, double v )	{
			curve_arr[ 0 ].insert( t, v );
		}

		void insert_spline( int curve_index, srSplineCurve& spline, double segments_per_sec )	{
			if( curve_index < 0 ) return;
			if( curve_index >= num_curves ) return;

			double fr, to;
			spline.query_span( &fr, &to );
			int num_segs = (int)( ( to - fr ) * segments_per_sec );

			srCurveBuilder builder;
			builder.get_spline_curve( curve_arr + curve_index, spline, num_segs, true );
		}
		void insert_spline( srSplineCurve& spline, double segments_per_sec )	{
			insert_spline( 0, spline, segments_per_sec );
		}

	private:
	
		SkChannelArray  _channels;
		SrBuffer<float> _data;
		SrBuffer<int>   _local_ch_to_buffer;
		
		int 	num_curves;
		srLinearCurve	*curve_arr;
		bool	*write_once_arr;
		int 	tail_bound_mode;
		
	public:
		virtual const std::string& controller_type() const {
			return( MeCtCurveWriter::TYPE );
		}
	
		virtual SkChannelArray& controller_channels()	{
			return( _channels );
		}
		
		virtual double controller_duration();
		virtual bool controller_evaluate( double time, MeFrameData& frame );

};

#endif // ME_CT_CURVE_WRITER_HPP
