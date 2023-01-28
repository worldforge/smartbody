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

#include "SBBehaviorSetManager.h"


namespace SmartBody {

SBBehaviorSetManager::SBBehaviorSetManager()
{

}

SBBehaviorSetManager::~SBBehaviorSetManager()
{
	removeAllBehaviorSets();
}

SBBehaviorSet* SBBehaviorSetManager::createBehaviorSet(const std::string& name)
{
	removeBehaviorSet(name);
	SBBehaviorSet* b = new SBBehaviorSet();
	b->setName(name);
	addBehaviorSet(name, b);
	return b;
}

int SBBehaviorSetManager::getNumBehaviorSets()
{
	return _behaviorSets.size();
}

std::map<std::string, SBBehaviorSet*>& SBBehaviorSetManager::getBehaviorSets()
{
	return _behaviorSets;
}

SBBehaviorSet* SBBehaviorSetManager::getBehaviorSet(const std::string& name)
{
	auto iter = _behaviorSets.find(name);
	if (iter == _behaviorSets.end())
		return nullptr;
	return (*iter).second;
}

void SBBehaviorSetManager::addBehaviorSet(const std::string& name, SBBehaviorSet* set)
{
	auto iter = _behaviorSets.find(name);
	if (iter != _behaviorSets.end())
	{
		delete (*iter).second;
	}
	_behaviorSets[name] = set;

}

void SBBehaviorSetManager::removeBehaviorSet(const std::string& name)
{
	auto iter = _behaviorSets.find(name);
	if (iter != _behaviorSets.end())
	{
		delete (*iter).second;
		_behaviorSets.erase(iter);
	}
	

}

void SBBehaviorSetManager::removeAllBehaviorSets()
{
	for (auto & _behaviorSet : _behaviorSets)
	{
		delete _behaviorSet.second;
	}
	_behaviorSets.clear();

}

}

