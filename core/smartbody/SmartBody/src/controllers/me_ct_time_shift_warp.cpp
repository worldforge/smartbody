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

#include <controllers/me_ct_time_shift_warp.hpp>


#include <iostream>
#include <iomanip>
#include <sstream>


std::string MeCtTimeShiftWarp::CONTROLLER_TYPE = "MeCtTimeShiftWarp";


MeCtTimeShiftWarp::MeCtTimeShiftWarp(SmartBody::SBPawn& pawn, MeController* child )
:	MeCtUnary(pawn,  new MeCtUnary::Context(this), child )
{
	if( child ) {
		_sub_context->add_controller( child );
	}
}

const std::string& MeCtTimeShiftWarp::controller_type() const {
	return CONTROLLER_TYPE;
}

double MeCtTimeShiftWarp::controller_duration() {
	return( _curve.get_tail_param() );
}

bool MeCtTimeShiftWarp::controller_evaluate( double t, MeFrameData & frame ) {
	if( child() ) {
		child()->evaluate( _curve.evaluate( t ), frame );
//		SmartBody::util::log("MeCtTimeShiftWarp %s,  %f	%f	%f", this->getName().c_str(), t, _curve.evaluate( t ), controller_duration());
		return true;
	} else {
		return false;
	}
}
