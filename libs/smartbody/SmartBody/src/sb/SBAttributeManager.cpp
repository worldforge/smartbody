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

SBAttributeManager::~SBAttributeManager() = default;

void SBAttributeManager::addGroup(const std::string& name)
{
	auto I = m_groups.find(name);
	if (I != m_groups.end()) {
		return;
	}

	m_groups.emplace(name, std::make_unique<SBAttributeGroup>(name));
	resortGroups();
}

SBAttributeGroup* SBAttributeManager::getGroup(const std::string& name, bool createIfNotFound)
{
	auto I = m_groups.find(name);
	if (I != m_groups.end()) {
		return I->second.get();
	}

	if (createIfNotFound)
	{
		auto result = m_groups.emplace(name, std::make_unique<SBAttributeGroup>(name));
		if (result.second) {
			resortGroups();
			return result.first->second.get();
		}
	}
	return nullptr;
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
	m_groupsByPriority.reserve(m_groups.size());
	for (auto & m_group : m_groups)
	{
		m_groupsByPriority.emplace_back(m_group.second.get());
	}

	std::sort(m_groupsByPriority.begin(), m_groupsByPriority.end(), prioritySort());
	
}

std::map<std::string, std::unique_ptr<SBAttributeGroup>>& SBAttributeManager::getGroups()
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
