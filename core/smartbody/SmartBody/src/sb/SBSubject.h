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

#ifndef DSUBJECT_H
#define DSUBJECT_H

#include <set>
#include <sb/SBTypes.h>

namespace SmartBody {

class SBObserver;

class SBSubject
{
	public:
		SBAPI SBSubject();
		SBAPI ~SBSubject();
		SBAPI virtual void registerObserver(SBObserver* observer);
		SBAPI virtual void unregisterObserver(SBObserver* observer);
		SBAPI virtual void notifyObservers();	
	protected:
		std::set<SBObserver*> m_observers;

};

};

#endif