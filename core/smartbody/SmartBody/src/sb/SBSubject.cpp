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

#include "SBSubject.h"
#include "SBObserver.h"

namespace SmartBody {

SBSubject::SBSubject()
{
}

SBSubject::~SBSubject()
{
	for (std::set<SBObserver*>::iterator iter = m_observers.begin();
		 iter !=  m_observers.end();
		 iter++)
	{
		(*iter)->removeDependency(this);
	}
}

void SBSubject::registerObserver(SBObserver* observer)
{
	std::set<SBObserver*>::iterator iter = m_observers.find(observer);
	if (iter == m_observers.end())
	{
		m_observers.insert(observer);
		observer->addDependency(this);
	}
}

void SBSubject::unregisterObserver(SBObserver* observer)
{
	std::set<SBObserver*>::iterator iter = m_observers.find(observer);
	if (iter != m_observers.end())
	{
		m_observers.erase(iter);
		observer->removeDependency(this);
	}
}

void SBSubject::notifyObservers()
{
	for (std::set<SBObserver*>::iterator iter = m_observers.begin();
		 iter != m_observers.end();
		 iter++)
	{
		(*iter)->notify(this);
	}
}

};
