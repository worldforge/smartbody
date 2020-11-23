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

#include "SBObserver.h"
#include "SBSubject.h"

#include <vector>

namespace SmartBody {

SBObserver::SBObserver() = default;

SBObserver::~SBObserver()
{
	std::vector<SBSubject*> tmpSubjects;
	for (std::set<SBSubject*>::iterator iter = _subjects.begin(); 
		 iter != _subjects.end();
		 iter++)
	{
		tmpSubjects.push_back(*iter);
	}
	
	for (std::vector<SBSubject*>::iterator iter = tmpSubjects.begin(); 
		 iter != tmpSubjects.end();
		 iter++)
	{
		(*iter)->unregisterObserver(this);
	}
}

void SBObserver::addDependency(SBSubject* subject)
{
	if (_subjects.find(subject) == _subjects.end())
	{
		_subjects.insert(subject);
	}
}

void SBObserver::removeDependency(SBSubject* subject)
{
	std::set<SBSubject*>::iterator iter = _subjects.find(subject);
	if (iter != _subjects.end())
	{
		_subjects.erase(iter);
	}
}

void SBObserver::notify(SBSubject* subject)
{
}

bool SBObserver::hasDependency(SBSubject* subject)
{
	return (_subjects.find(subject) != _subjects.end());
}

};
