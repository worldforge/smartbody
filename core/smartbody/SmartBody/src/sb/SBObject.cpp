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
#include "SBObject.h"
#include <sr/sr_vec.h>
#include <sr/sr_mat.h>
#include <sb/SBAttribute.h>
#include <sb/SBAttributeManager.h>

#include <utility>
#include "SBUtilities.h"


namespace SmartBody {


SBObject::SBObject() : SBSubject(), SBObserver()
{
}

SBObject::~SBObject()
{
	for (auto iter : _dependenciesReverse)
	{
		iter->onDependencyRemoval(this);
	}
}

void SBObject::setName(const std::string& name)
{
	m_name = name;
}


const std::string& SBObject::getName() const
{
	return m_name;
}


void SBObject::copyAllAttributes( SBObject* origObj )
{
	auto& attrList = origObj->getAttributeList();
	for (auto & vi : attrList)
	{
		std::string attrName = vi.first;
		auto& attr = vi.second;
		SBAttribute* existAttr = getAttribute(attrName);
		if (existAttr) // copy over the value
		{
			existAttr->copyAttrValue(attr.get()); // this ensure observer get notify
		}
		else // create a new copy 
		{
			std::unique_ptr<SBAttribute> attrCopy(attr->copy());
			attrCopy->registerObserver(this);
			addAttribute(std::move(attrCopy));
		}
	}	
}


void SBObject::addAttribute(std::unique_ptr<SBAttribute> attr)
{
	 // check for the existence of the attribute
	auto iter = m_attributeList.find(attr->getName());
	if (iter != m_attributeList.end()) // attribute exists, remove the old attribute 
	{
		auto& rmattr = iter->second;

		// notify the attribute manager of the change
		this->getAttributeManager()->notifyRemoveAttribute(rmattr.get());
		m_attributeList.erase(iter);		
	}

	auto result = m_attributeList.emplace(attr->getName(), std::move(attr));
	if (result.second) {
		result.first->second->setObject(this);
		// notify the attribute manager of the change
		this->getAttributeManager()->notifyCreateAttribute(result.first->second.get());
	}
}

void SBObject::addAttribute( std::unique_ptr<SBAttribute> attr, const std::string& groupName )
{
	// check for the existence of the attribute
	auto iter = m_attributeList.find(attr->getName());
	if (iter != m_attributeList.end()) // attribute exists, remove the old attribute 
	{
		auto& rmattr = iter->second;

		// notify the attribute manager of the change
		this->getAttributeManager()->notifyRemoveAttribute(rmattr.get());

		m_attributeList.erase(iter);
	}

	auto result = m_attributeList.emplace(attr->getName(), std::move(attr));
	if (result.second) {
		SBAttributeGroup* group = this->getAttributeManager()->getGroup(groupName, true);
		result.first->second->getAttributeInfo()->setGroup(group);
		result.first->second->setObject(this);
		// notify the attribute manager of the change
		this->getAttributeManager()->notifyCreateAttribute(result.first->second.get());
	}
}

 bool SBObject::hasAttribute(const std::string& name)
 {
	auto iter = m_attributeList.find(name);
	if (iter != m_attributeList.end()) // attribute exists, remove the old attribute 
	{
		return true;
	}
	else
	{
		return false;
	}
 }

 SBAttribute* SBObject::getAttribute(const std::string& name) const
 {
	auto iter = m_attributeList.find(name);
	if (iter != m_attributeList.end()) // attribute exists, remove the old attribute 
	{
		return iter->second.get();
	}
	else
	{
		return nullptr;
	}
 }

 void SBObject::clearAttributes()
 {
	 auto iter = m_attributeList.begin();
	 std::vector<std::string> attrNameList;
	 for ( iter  = m_attributeList.begin();
		   iter != m_attributeList.end();
		   iter++)
	 {
		 attrNameList.emplace_back(iter->first);
	 }		 
	 for (auto & i : attrNameList)
		 removeAttribute(i);
 }
 
int SBObject::getNumAttributes()
{
	return m_attributeList.size();
}

std::vector<std::string> SBObject::getAttributeNames()
{
	std::vector<std::string> attrNameList;
	for (auto & iter : m_attributeList)
	 {
		 attrNameList.emplace_back(iter.first);
	 }
	 return attrNameList;
}


 bool SBObject::removeAttribute(const std::string& name)
 {
	// check for the existence of the attribute
	auto iter = m_attributeList.find(name);
	if (iter != m_attributeList.end()) // attribute exists, remove the old attribute 
	{

		auto& attr = iter->second;
	
		// notify the attribute manager of the change
		this->getAttributeManager()->notifyRemoveAttribute(attr.get());
		
		m_attributeList.erase(iter);

		return true;
	}

	return false;
 }



  BoolAttribute* SBObject::createBoolAttribute(const std::string& name, bool value, bool notifySelf, const std::string& groupName, int priority, 
											  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description)
 {

	 auto* boolAttr = new BoolAttribute();
	 boolAttr->setName(name);
	 boolAttr->setValue(value);
	 boolAttr->setDefaultValue(value);
	 boolAttr->getAttributeInfo()->setPriority(priority);
	 boolAttr->getAttributeInfo()->setReadOnly(isReadOnly);
	 boolAttr->getAttributeInfo()->setLocked(isLocked);
	 boolAttr->getAttributeInfo()->setHidden(isHidden);
	 boolAttr->getAttributeInfo()->setDescription(description);

	 if (notifySelf)
		 boolAttr->registerObserver(this);
	 this->addAttribute(std::unique_ptr<BoolAttribute>(boolAttr), groupName);
	 return boolAttr;

}

IntAttribute* SBObject::createIntAttribute(const std::string& name, int value, bool notifySelf, const std::string& groupName, int priority,
												  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description)
 {
	 auto* intAttr = new IntAttribute();
	 intAttr->setName(name);
	 intAttr->setValue(value);
	 intAttr->setDefaultValue(value);
	 intAttr->getAttributeInfo()->setPriority(priority);
	 intAttr->getAttributeInfo()->setReadOnly(isReadOnly);
	 intAttr->getAttributeInfo()->setLocked(isLocked);
	 intAttr->getAttributeInfo()->setHidden(isHidden);
	 intAttr->getAttributeInfo()->setDescription(description);

	 if (notifySelf)
		 intAttr->registerObserver(this);

	 this->addAttribute(std::unique_ptr<IntAttribute>(intAttr), groupName);

	  return intAttr;
}

 DoubleAttribute* SBObject::createDoubleAttribute(const std::string& name, double value, bool notifySelf, const std::string& groupName, int priority,
												  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description)
 {
	 auto* doubleAttr = new DoubleAttribute();
	 doubleAttr->setName(name);
	 doubleAttr->setValue(value);
	 doubleAttr->setDefaultValue(value);
	 doubleAttr->getAttributeInfo()->setPriority(priority);
	 doubleAttr->getAttributeInfo()->setReadOnly(isReadOnly);
	 doubleAttr->getAttributeInfo()->setLocked(isLocked);
	 doubleAttr->getAttributeInfo()->setHidden(isHidden);
	 doubleAttr->getAttributeInfo()->setDescription(description);

	 if (notifySelf)
		 doubleAttr->registerObserver(this);

	 this->addAttribute(std::unique_ptr<DoubleAttribute>(doubleAttr), groupName);

	  return doubleAttr;
}

 Vec3Attribute* SBObject::createVec3Attribute(const std::string& name, float val1, float val2, float val3, bool notifySelf, const std::string& groupName, int priority,
												  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description)
 {
	 auto* vec3Attr = new Vec3Attribute();
	 vec3Attr->setName(name);
	 SrVec vec(val1, val2, val3);
	 vec3Attr->setValue(vec);
	 vec3Attr->setDefaultValue(vec);
	 vec3Attr->getAttributeInfo()->setPriority(priority);
	 vec3Attr->getAttributeInfo()->setReadOnly(isReadOnly);
	 vec3Attr->getAttributeInfo()->setLocked(isLocked);
	 vec3Attr->getAttributeInfo()->setHidden(isHidden);
	 vec3Attr->getAttributeInfo()->setDescription(description);

	 if (notifySelf)
		 vec3Attr->registerObserver(this);

	 this->addAttribute(std::unique_ptr<Vec3Attribute>(vec3Attr), groupName);

	  return vec3Attr;
}


 StringAttribute* SBObject::createStringAttribute(const std::string& name, const std::string& value, bool notifySelf, const std::string& groupName, int priority,
												  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description)
 {
	 auto* strAttr = new StringAttribute();
	 strAttr->setName(name);
	 strAttr->setValue(value);
	 strAttr->setDefaultValue(value);
	 strAttr->getAttributeInfo()->setPriority(priority);
	 strAttr->getAttributeInfo()->setReadOnly(isReadOnly);
	 strAttr->getAttributeInfo()->setLocked(isLocked);
	 strAttr->getAttributeInfo()->setHidden(isHidden);
	 strAttr->getAttributeInfo()->setDescription(description);

	 if (notifySelf)
		 strAttr->registerObserver(this);

	 this->addAttribute(std::unique_ptr<StringAttribute>(strAttr), groupName);

	 return strAttr;
}

 MatrixAttribute* SBObject::createMatrixAttribute(const std::string& name, SrMat& value, bool notifySelf, const std::string& groupName, int priority,
												  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description)
 {
	 auto* matrixAttr = new MatrixAttribute();
	 matrixAttr->setName(name);
	 matrixAttr->setValue(value);
	 matrixAttr->setDefaultValue(value);
	 matrixAttr->getAttributeInfo()->setPriority(priority);
	 matrixAttr->getAttributeInfo()->setReadOnly(isReadOnly);
	 matrixAttr->getAttributeInfo()->setLocked(isLocked);
	 matrixAttr->getAttributeInfo()->setHidden(isHidden);
	 matrixAttr->getAttributeInfo()->setDescription(description);

	 if (notifySelf)
		 matrixAttr->registerObserver(this);

	 this->addAttribute(std::unique_ptr<MatrixAttribute>(matrixAttr), groupName);

	  return matrixAttr;
}

 ActionAttribute* SBObject::createActionAttribute(const std::string& name, bool notifySelf, const std::string& groupName, int priority,
											  bool isReadOnly, bool isLocked, bool isHidden, const std::string& description)
 {
	 auto* actionAttr = new ActionAttribute();
	 actionAttr->setName(name);

	 actionAttr->getAttributeInfo()->setPriority(priority);
	 actionAttr->getAttributeInfo()->setReadOnly(isReadOnly);
	 actionAttr->getAttributeInfo()->setLocked(isLocked);
	 actionAttr->getAttributeInfo()->setHidden(isHidden);
	 actionAttr->getAttributeInfo()->setDescription(description);

	 if (notifySelf)
		 actionAttr->registerObserver(this);

	 this->addAttribute(std::unique_ptr<ActionAttribute>(actionAttr), groupName);

	 return actionAttr;
 }

void SBObject::setAttributeGroupPriority(const std::string& name, int value)
{
	 SBAttributeGroup* group = this->getAttributeManager()->getGroup(name, true);
	 group->setPriority(value);
}

int SBObject::getAttributeGroupPriority(const std::string& name)
{
	SBAttributeGroup* group = this->getAttributeManager()->getGroup(name, false);
	if (!group)
	{
		SmartBody::util::log("No group named '%s' found, cannot set priority.", name.c_str());
		return 0;
	}
	else
	{
		return group->getPriority();
	}
	
}

  std::map<std::string, std::unique_ptr<SBAttribute>>& SBObject::getAttributeList()
 {
	return m_attributeList;
 }


  SBAttributeManager* SBObject::getAttributeManager()
{
	return &m_attributeManager;
}

  void SBObject::notify(SBSubject* subject)
{
	
  }

  void SBObject::setBoolAttribute( const std::string& name, bool value ) const
  {
	  SBAttribute* attr = getAttribute(name);
	  auto battr = dynamic_cast<BoolAttribute*>(attr);
	  if (battr)
	  {
		  battr->setValue(value);
	  }
	  else
	  {
		  SmartBody::util::log("Warning, Bool Attribute %s does not exist.",name.c_str());
	  }
  }

  void SBObject::setIntAttribute( const std::string& name, int value ) const
  {
	  SBAttribute* attr = getAttribute(name);
	  auto iattr = dynamic_cast<IntAttribute*>(attr);
	  if (iattr)
	  {
		  iattr->setValue(value);
	  }
	  else
	  {
		  SmartBody::util::log("Warning, Int Attribute %s does not exist.",name.c_str());
	  }
  }

  void SBObject::setDoubleAttribute( const std::string& name, double value ) const
  {
	  SBAttribute* attr = getAttribute(name);
	  auto dattr = dynamic_cast<DoubleAttribute*>(attr);
	  if (dattr)
	  {
		  dattr->setValue(value);
	  }
	  else
	  {
		  SmartBody::util::log("Warning, Double Attribute %s does not exist.",name.c_str());
	  }
  }

  void SBObject::setVec3Attribute( const std::string& name, float val1, float val2, float val3 ) const
  {
	  SBAttribute* attr = getAttribute(name);
	  auto vattr = dynamic_cast<Vec3Attribute*>(attr);
	  if (vattr)
	  {
		  vattr->setValue(SrVec(val1,val2,val3));
	  }
	  else
	  {
		  SmartBody::util::log("Warning, Vec3 Attribute %s does not exist.",name.c_str());
	  }
  }

  void SBObject::setStringAttribute( const std::string& name, std::string value ) const
  {
	  SBAttribute* attr = getAttribute(name);
	  auto sattr = dynamic_cast<StringAttribute*>(attr);
	  if (sattr)
	  {
		  sattr->setValue(std::move(value));
	  }
	  else
	  {
		  SmartBody::util::log("Warning, String Attribute %s does not exist.",name.c_str());
	  }
  }

  void SBObject::setMatrixAttribute( const std::string& name, SrMat& value ) const
  {
	  SBAttribute* attr = getAttribute(name);
	  auto* mattr = dynamic_cast<MatrixAttribute*>(attr);
	  if (mattr)
	  {
		  mattr->setValue(value);
	  }
	  else
	  {
		  SmartBody::util::log("Warning, Matrix Attribute %s does not exist.",name.c_str());
	  }
  }

  void SBObject::setActionAttribute(const std::string& name) const
  {
	  SBAttribute* attr = getAttribute(name);
	  auto* aattr = dynamic_cast<ActionAttribute*>(attr);
	  if (aattr)
	  {
		  aattr->setValue();
	  }
	  else
	  {
		  SmartBody::util::log("Warning, Action Attribute %s does not exist.",name.c_str());
	  }
  }

  bool SBObject::getBoolAttribute( const std::string& name ) const
  {
	  SBAttribute* attr = getAttribute(name);
	  auto* battr = dynamic_cast<BoolAttribute*>(attr);
	  if (battr)
	  {
		  return battr->getValue();
	  }	  
	  return true;
  }

  int SBObject::getIntAttribute( const std::string& name ) const
  {
	  SBAttribute* attr = getAttribute(name);
	  auto* iattr = dynamic_cast<IntAttribute*>(attr);
	  if (iattr)
	  {
		  return iattr->getValue();
	  }
	  return 0;
  }

  double SBObject::getDoubleAttribute( const std::string& name ) const
  {
	  SBAttribute* attr = getAttribute(name);
	  auto* dattr = dynamic_cast<DoubleAttribute*>(attr);
	  if (dattr)
	  {
		 return dattr->getValue();
	  }
	  return 0;
  }

  const SrVec& SBObject::getVec3Attribute( const std::string& name ) const
  {
	  SBAttribute* attr = getAttribute(name);
	  auto* vattr = dynamic_cast<Vec3Attribute*>(attr);
	  if (vattr)
	  {
		  return vattr->getValue();
	  }
	  static SrVec defaultVec;

	  return defaultVec;
  }

  const std::string& SBObject::getStringAttribute( const std::string& name ) const
  {
	  SBAttribute* attr = getAttribute(name);
	  auto* sattr = dynamic_cast<StringAttribute*>(attr);
	  if (sattr)
	  {
		  return sattr->getValue();
	  }
	  static std::string defaultString;
	  return defaultString;
  }

  const SrMat& SBObject::getMatrixAttribute( const std::string& name ) const
  {
	  SBAttribute* attr = getAttribute(name);
	  auto* mattr = dynamic_cast<MatrixAttribute*>(attr);
	  if (mattr)
	  {
		  return mattr->getValue();
	  }
	  static SrMat defaultMatrix;
	  return defaultMatrix;
  }

void SBObject::start()
{
}

void SBObject::beforeUpdate(double time)
{
}

void SBObject::update(double time)
{
}

void SBObject::afterUpdate(double time)
{
}

void SBObject::stop()
{
}


void SBObject::addDependency(SBObject* object)
{
	if (object == nullptr)
		return;
	addDependencyOnly(object);
	object->addDependencyReverse(this);
}

void SBObject::addDependencyOnly(SBObject* object)
{
	if (object == nullptr)
		return;
	auto iter = _dependencies.find(object);
	if (iter == _dependencies.end())
		_dependencies.insert(object);
}


void SBObject::removeDependency(SBObject* object)
{
	if (object == nullptr)
		return;
	removeDependencyOnly(object);
	this->onDependencyRemoval(object);
	object->removeDependencyReverse(this);
}

void SBObject::removeDependencyOnly(SBObject* object)
{
	if (object == nullptr)
		return;
	auto iter = _dependencies.find(object);
	if (iter != _dependencies.end())
		_dependencies.erase(iter);
}

void SBObject::clearDependencies()
{
	_dependencies.clear();
}

void SBObject::clearReverseDependencies()
{
	_dependenciesReverse.clear();
}

std::set<SBObject*>& SBObject::getDependencies()
{
	return _dependencies;
}

void SBObject::addDependencyReverse(SBObject* object)
{
	auto iter = _dependencies.find(object);
	if (iter == _dependencies.end())
		_dependencies.insert(object);

}

void SBObject::removeDependencyReverse(SBObject* object)
{
	auto iter = _dependenciesReverse.find(object);
	if (iter != _dependenciesReverse.end())
		_dependenciesReverse.erase(iter);
}

std::set<SBObject*>& SBObject::getDependenciesReverse()
{
	return _dependenciesReverse;
}

void SBObject::onDependencyRemoval(SBObject* object)
{
}


}
  