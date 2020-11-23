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

#include "SBAttributeManager.h"
#include <algorithm>
#include "SBAttribute.h"

namespace SmartBody {

SBAttributeManager::SBAttributeManager() : SBSubject()
{
}

SBAttributeManager::~SBAttributeManager()
{
	for (std::map<std::string, SBAttributeGroup*>::iterator iter = m_groups.begin();
		 iter != m_groups.end();
		 iter++)
	{
		delete (*iter).second;
	}
}

void SBAttributeManager::addGroup(const std::string& name)
{
	for (std::map<std::string, SBAttributeGroup*>::iterator iter = m_groups.begin();
		 iter != m_groups.end();
		 iter++)
	{
		if (name == (*iter).second->getName())
			return;
	}

	SBAttributeGroup* group = new SBAttributeGroup(name);
	m_groups[name] = group;
	resortGroups();
}

SBAttributeGroup* SBAttributeManager::getGroup(const std::string& name, bool createIfNotFound)
{
	for (std::map<std::string, SBAttributeGroup*>::iterator iter = m_groups.begin();
		 iter != m_groups.end();
		 iter++)
	{
		if (name == (*iter).second->getName())
		{
			return (*iter).second;
		}
	}

	if (createIfNotFound)
	{
		SBAttributeGroup* group = new SBAttributeGroup(name);
		m_groups[name] = group;
		resortGroups();
		return group;
	}
	else
	{
		return nullptr;
	}
}

struct prioritySort
{
  bool operator()(SBAttributeGroup* a, SBAttributeGroup* b)
  {
	  return a->getPriority() < b->getPriority();
  }
};

void SBAttributeManager::resortGroups()
{
	m_groupsByPriority.clear();
	for (std::map<std::string, SBAttributeGroup*>::iterator iter = m_groups.begin();
		iter != m_groups.end();
		iter++)
	{
		m_groupsByPriority.push_back((*iter).second);
	}

	std::sort(m_groupsByPriority.begin(), m_groupsByPriority.end(), prioritySort());
	
}

std::map<std::string, SBAttributeGroup*>& SBAttributeManager::getGroups()
{
	return m_groups;
}

std::vector<SBAttributeGroup*>& SBAttributeManager::getAttributeGroups()
{
	return m_groupsByPriority;
}

void SBAttributeManager::notifyCreateAttribute(SBAttribute* attr)
{
	notifyObservers();
}

void SBAttributeManager::notifyRemoveAttribute(SBAttribute* attr)
{
	notifyObservers();
}

void SBAttributeManager::notifyPrioritizeAttribute(SBAttribute* attr)
{
	notifyObservers();
}

void SBAttributeManager::notifyHideAttribute(SBAttribute* attr)
{
	notifyObservers();
}

};
