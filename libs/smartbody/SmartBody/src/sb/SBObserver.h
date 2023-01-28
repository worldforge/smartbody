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

#ifndef DOBSERVER_H
#define DOBSERVER_H

#include <sb/SBTypes.h>
#include <set>

namespace SmartBody {

class SBSubject;

class SBObserver
{
	public:
		SBAPI SBObserver();
		SBAPI ~SBObserver();

		SBAPI virtual void addDependency(SBSubject* subject);
		SBAPI virtual void removeDependency(SBSubject* subject);
		SBAPI bool hasDependency(SBSubject* subject);

		SBAPI virtual void notify(SBSubject* subject);

	protected:
		std::set<SBSubject*> _subjects;
};

};

#endif