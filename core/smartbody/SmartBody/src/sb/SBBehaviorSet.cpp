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

#include "SBBehaviorSet.h"

namespace SmartBody {

SBBehaviorSet::SBBehaviorSet()
{
}

SBBehaviorSet::~SBBehaviorSet()
{
}

void SBBehaviorSet::setName(const std::string& name)
{
	_name = name;
}

const std::string& SBBehaviorSet::getName()
{
	return _name;
}

void SBBehaviorSet::setScript(const std::string& name)
{
	_script = name;
}

const std::string& SBBehaviorSet::getScript()
{
	return _script;
}

}



