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

#ifndef _SBCHARACTERFRAMEDATA_H_
#define _SBCHARACTERFRAMEDATA_H_


#ifdef __cplusplus
extern "C" {
#endif 

#include <cstddef>


// This struct is used by SBCharacter::GetFrameDataMarshalFriendly() and SBM_GetCharacter().
// It is useful for when you want a character's entire skeleton data for the current frame in one call.
// The data is laid out to be friendly for marshalling purposes when going over the managed barrier.

struct SBM_CharacterFrameDataMarshalFriendly
{
	char * m_name;
	float x;
	float y;
	float z;
	float rw;
	float rx;
	float ry;
	float rz;
	size_t m_numJoints;

	char ** jname;
	float * jx;
	float * jy;
	float * jz;
	float * jrw;
	float * jrx;
	float * jry;
	float * jrz;
};


#ifdef __cplusplus
}
#endif

#endif  // _SBCHARACTERFRAMEDATA_H_
