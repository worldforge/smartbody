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

#include "SBRetargetManager.h"
#include <sb/SBRetarget.h>
#include "SBUtilities.h"


namespace SmartBody {

SBAPI SBRetargetManager::SBRetargetManager() = default;

SBAPI SBRetargetManager::~SBRetargetManager() = default;

SBAPI SBRetarget* SBRetargetManager::createRetarget( const std::string& sourceSk, const std::string& targetSk )
{	
	if (getRetarget(sourceSk, targetSk))
	{
		SmartBody::util::log("Retarget for skeleton pair (%s,%s) already exist.", sourceSk.c_str(), targetSk.c_str());
		return nullptr;
	}

	StringPair skNamePair = StringPair(sourceSk,targetSk);
	_retargets[skNamePair] = SmartBody::SBRetarget(sourceSk,targetSk);
	return &_retargets[skNamePair];
}

SBAPI SBRetarget* SBRetargetManager::getRetarget( const std::string& sourceSk, const std::string& targetSk )
{
	auto I = _retargets.find(StringPair(sourceSk,targetSk));
	if (I != _retargets.end())
	{
		return &I->second;
	}
	return nullptr;
}

SBAPI std::vector<StringPair> SBRetargetManager::getRetargetNames()
{
	std::vector<StringPair> retargetNames;
	for (auto & _retarget : _retargets)
	{
		retargetNames.emplace_back(_retarget.first);
	}
	return retargetNames;
}

}
