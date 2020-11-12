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

#ifndef _MINIBRAIN_H_
#define _MINIBRAIN_H_

#include <sr/sr_vec.h>
#include <map>
#include <string>
#include <list>

namespace SmartBody 
{

struct ObjectData
{
	SrVec velocity;
	SrVec position;
	std::list<SrVec> cachePositions;
	SrVec relativeVelocity;
	SrVec relativePosition;
	double startGazeTime;
	bool isAnimate;
};

class SBCharacter;

class MiniBrain
{
	public:
		MiniBrain();
		virtual ~MiniBrain();

		virtual void update(SBCharacter* character, double time, double dt);

	protected:
		std::map<std::string, ObjectData> _data;
		int _cacheLimit;
};

}

#endif
