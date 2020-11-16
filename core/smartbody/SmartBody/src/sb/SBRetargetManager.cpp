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

SBAPI SBRetargetManager::SBRetargetManager()
{

}

SBAPI SBRetargetManager::~SBRetargetManager()
{

}

SBAPI SBRetarget* SBRetargetManager::createRetarget( std::string sourceSk, std::string targetSk )
{	
	if (getRetarget(sourceSk, targetSk))
	{
		SmartBody::util::log("Retarget for skeleton pair (%s,%s) already exist.", sourceSk.c_str(), targetSk.c_str());
		return NULL;			
	}

	SmartBody::SBRetarget* retarget = new SmartBody::SBRetarget(sourceSk,targetSk);
	StringPair skNamePair = StringPair(sourceSk,targetSk);
	_retargets[skNamePair] = retarget;
	return retarget;
}

SBAPI SBRetarget* SBRetargetManager::getRetarget( std::string sourceSk, std::string targetSk )
{
	StringPair skNamePair = StringPair(sourceSk,targetSk);
	SmartBody::SBRetarget* retarget = NULL;
	if (_retargets.find(skNamePair) != _retargets.end())
	{
		retarget = _retargets[skNamePair];
	}
	return retarget;
}

SBAPI std::vector<StringPair> SBRetargetManager::getRetargetNames()
{
	std::vector<StringPair> retargetNames;
	std::map<StringPair, SBRetarget*>::iterator ri;
	for ( ri  = _retargets.begin();
		  ri != _retargets.end();
		  ri++)
	{
		retargetNames.push_back(ri->first);
	}
	return retargetNames;
}

}
