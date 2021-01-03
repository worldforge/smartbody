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

#ifndef DATTRIBUTEMANAGER_H
#define DATTRIBUTEMANAGER_H

#include <sb/SBTypes.h>
#include "SBSubject.h"
#include <map>
#include <vector>
#include <string>
#include <memory>

namespace SmartBody {

class SBAttribute;
class SBAttributeGroup;

class SBAttributeManager : public SBSubject
{
	public:
		SBAPI SBAttributeManager();
		SBAPI ~SBAttributeManager();

		SBAPI void addGroup(const std::string& name);
		SBAPI SBAttributeGroup* getGroup(const std::string& name, bool createIfNotFound = false);
		SBAPI std::map<std::string, std::unique_ptr<SBAttributeGroup>>& getGroups();
		SBAPI std::vector<SBAttributeGroup*>& getAttributeGroups();

		SBAPI void notifyCreateAttribute(SBAttribute* attr);
		SBAPI void notifyRemoveAttribute(SBAttribute* attr);
		SBAPI void notifyPrioritizeAttribute(SBAttribute* attr);
		SBAPI void notifyHideAttribute(SBAttribute* attr);

	protected:
		void resortGroups();

		std::map<std::string, std::unique_ptr<SBAttributeGroup>> m_groups;
		std::vector<SBAttributeGroup*> m_groupsByPriority;
};

};

#endif
