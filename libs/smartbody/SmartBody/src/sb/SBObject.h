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

#ifndef _SBOBJECT_H_
#define _SBOBJECT_H_

#include "SBAttributeManager.h"
#include "SBObserver.h"
#include "SBSubject.h"
#include "SBABI.h"
#include <map>
#include <string>
#include <vector>

#include <memory>
#include <boost/noncopyable.hpp>


class SrMat;
class SrVec;

namespace SmartBody {

class SBScene;
class SBAttribute;
class ActionAttribute;
class BoolAttribute;
class IntAttribute;
class StringAttribute;
class DoubleAttribute;
class Vec3Attribute;
class MatrixAttribute;
class SBAttributeManager;

class SBObject : public SBObserver, public SBSubject, public boost::noncopyable
{
	public:
		SBAPI SBObject();
		SBAPI ~SBObject();

		SBAPI virtual void start();
		SBAPI virtual void beforeUpdate(double time);
		SBAPI virtual void update(double time);
		SBAPI virtual void afterUpdate(double time);
		SBAPI virtual void stop();

		SBAPI virtual void setName(const std::string& name);
		SBAPI virtual const std::string& getName() const;
		SBAPI bool hasAttribute(const std::string& attrName);
		SBAPI SBAttribute* getAttribute(const std::string& attrName) const;
		SBAPI std::map<std::string, std::unique_ptr<SBAttribute>>& getAttributeList();
		SBAPI SBAttributeManager* getAttributeManager();
		SBAPI void addAttribute(std::unique_ptr<SBAttribute> attr);
		SBAPI void addAttribute(std::unique_ptr<SBAttribute> attr, const std::string& groupName);
		SBAPI bool removeAttribute(const std::string& name);
		SBAPI void clearAttributes();
		SBAPI int getNumAttributes();

		// copy all attributes from original object to this object
		SBAPI void copyAllAttributes(SBObject* origObj); 

		SBAPI std::vector<std::string> getAttributeNames();

		SBAPI BoolAttribute* createBoolAttribute(const std::string& name, bool value, bool notifySelf, const std::string& groupName, int priority, 
													  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description = "");
		SBAPI IntAttribute* createIntAttribute(const std::string& name, int value, bool notifySelf, const std::string& groupName, int priority,
													  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description = "");
		SBAPI DoubleAttribute* createDoubleAttribute(const std::string& name, double value, bool notifySelf, const std::string& groupName, int priority,
													  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description = "");
		SBAPI Vec3Attribute* createVec3Attribute(const std::string& name, float val1, float val2, float val3, bool notifySelf, const std::string& groupName, int priority,
													  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description = "");

		SBAPI StringAttribute* createStringAttribute(const std::string& name, const std::string& value, bool notifySelf, const std::string& groupName, int priority,
													  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description = "");
		SBAPI MatrixAttribute* createMatrixAttribute(const std::string& name, SrMat& value, bool notifySelf, const std::string& groupName, int priority,
													  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description = "");
		SBAPI ActionAttribute* createActionAttribute(const std::string& name, bool notifySelf, const std::string& groupName, int priority,
													  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description = "");

		SBAPI void setAttributeGroupPriority(const std::string& name, int value);
		SBAPI int getAttributeGroupPriority(const std::string& name);

		SBAPI void setBoolAttribute(const std::string& name, bool value) const;
		SBAPI void setIntAttribute(const std::string& name, int value) const;
		SBAPI void setDoubleAttribute(const std::string& name, double value) const;
		SBAPI void setVec3Attribute(const std::string& name, float val1, float val2, float val3) const;
		SBAPI void setStringAttribute(const std::string& name, std::string value) const;
		SBAPI void setMatrixAttribute(const std::string& name, SrMat& value) const;
		SBAPI void setActionAttribute(const std::string& name) const;

		SBAPI bool getBoolAttribute(const std::string& name) const;
		SBAPI int  getIntAttribute(const std::string& name) const;
		SBAPI double getDoubleAttribute(const std::string& name) const;
		SBAPI const SrVec& getVec3Attribute(const std::string& name) const;
		SBAPI const std::string& getStringAttribute(const std::string& name) const;
		SBAPI const SrMat& getMatrixAttribute(const std::string& name) const;

		SBAPI void notify(SBSubject* subject) override;
		
		SBAPI void addDependency(SBObject* object);
		SBAPI void removeDependency(SBObject* object);
		SBAPI void addDependencyOnly(SBObject* object);
		SBAPI void removeDependencyOnly(SBObject* object);
		SBAPI std::set<SBObject*>& getDependencies();
		SBAPI void addDependencyReverse(SBObject* object);
		SBAPI void removeDependencyReverse(SBObject* object);
		SBAPI std::set<SBObject*>& getDependenciesReverse();
		SBAPI void clearDependencies();
		SBAPI void clearReverseDependencies();

		SBAPI virtual void onDependencyRemoval(SBObject* object);

	protected:
		std::string m_name;
		SBAttributeManager m_attributeManager;
		std::map<std::string, std::unique_ptr<SBAttribute>> m_attributeList;
		std::set<SBObject*> _dependencies;
		std::set<SBObject*> _dependenciesReverse;

};

}

#endif