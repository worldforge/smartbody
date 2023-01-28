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

#include "SBAttribute.h"
#include "sb/SBObject.h"
#include "SBUtilities.h"

#include <sstream>
#include <limits>
#include <utility>

namespace SmartBody {

SBAttributeGroup::SBAttributeGroup(std::string name)
 : m_name(std::move(name)), m_priority(0)
{
}

SBAttributeGroup::~SBAttributeGroup() = default;

const std::string& SBAttributeGroup::getName() const
{
	return m_name;
}

void SBAttributeGroup::setPriority(int val)
{
	m_priority = val;
}

int SBAttributeGroup::getPriority() const
{
	return m_priority;
}


void SBAttributeGroup::addAttribute(SBAttribute* attr)
{
	auto iter = m_attributeMap.find(attr->getName());
	if (iter == m_attributeMap.end())
	{
		m_attributeMap.insert(std::pair<std::string, SBAttribute*>(attr->getName(), attr));	
	}
	else
	{
		SmartBody::util::log("Attribute info for %s already exists in attribute group %s!", attr->getName().c_str(), this->getName().c_str());
	}
}

void  SBAttributeGroup::removeAttribute(SBAttribute* attr)
{
	auto iter = m_attributeMap.find(attr->getName());
	if (iter == m_attributeMap.end())
	{
		SmartBody::util::log("Attribute info for %s does not exists in attribute group %s!", attr->getName().c_str(), this->getName().c_str());
	}
	else
	{
		m_attributeMap.erase(iter);
	}
}

std::map<std::string, SBAttribute*>& SBAttributeGroup::getAttributes()
{
	return m_attributeMap;
}

std::vector<std::string> SBAttributeGroup::getAttributeNames()
{
	std::vector<std::string> attributes;
	for (auto & iter : m_attributeMap)
	{
		attributes.emplace_back(iter.first);
	}
	return attributes;
}

SBAttribute* SBAttributeGroup::getAttribute(const std::string& name)
{
	auto iter = m_attributeMap.find(name);
	if (iter == m_attributeMap.end())
	{
		SmartBody::util::log("Attribute for %s does not exist in attribute group %s!", name.c_str(), this->getName().c_str());
		return nullptr;
	}
	else
	{
		return (*iter).second;
	}
}



// remove the min/max definition for windows systems
// so that there is no conflict between the min or max macros
// and the min() or max() functions in numeric_limits
SBAttributeInfo::SBAttributeInfo(SBAttribute& m_attr)
: 	m_priority(0),
	m_readOnly(false),
	m_locked(false),
	m_hidden(false),
	m_attr(m_attr),
	m_group(nullptr)
{
}

SBAttributeInfo::~SBAttributeInfo()
{
	if (m_group)
		m_group->removeAttribute(&m_attr);
}

SBAttribute* SBAttributeInfo::getAttribute()
{
	return &m_attr;
}

void SBAttributeInfo::setPriority(int val)
{
	m_priority = val;
	getAttribute()->notifyObservers();
}

int SBAttributeInfo::getPriority() const
{
	return m_priority;
}

void SBAttributeInfo::setReadOnly(bool val)
{
	m_readOnly = val;
	getAttribute()->notifyObservers();
}

bool SBAttributeInfo::getReadOnly() const
{
	return m_readOnly;
}

void SBAttributeInfo::setLocked(bool val)
{
	m_locked = val;
	getAttribute()->notifyObservers();
}

bool SBAttributeInfo::getLocked() const
{
	return m_locked;
}

void SBAttributeInfo::setHidden(bool val)
{
	m_hidden = val;
	getAttribute()->notifyObservers();
}

bool SBAttributeInfo::getHidden() const
{
	return m_hidden;
}

void SBAttributeInfo::setGroup(SBAttributeGroup* group)
{
	// remove from old group if any
	if (m_group)
	{
		m_group->removeAttribute(this->getAttribute());
	}
	m_group = group;
	if (group)
	{
		m_group->addAttribute(this->getAttribute());
	}

}

void SBAttributeInfo::setDescription(const std::string& description)
{
	m_description = description;
}

std::string SBAttributeInfo::getDescription()
{
	return m_description;
}

SBAttributeGroup* SBAttributeInfo::getGroup()
{
	return m_group;
}

std::string SBAttributeInfo::write()
{
	bool useReadOnly = getReadOnly();
	bool useHidden = getHidden();
	bool useLocked = getLocked();
	bool usePriority = getPriority() != 0;

	std::stringstream strstr;
	strstr << "\"attrinfo\", \"";
	strstr << this->getAttribute()->getName();
	strstr << "\", ";

	int numAttrs = 0;
	if (useReadOnly)
	{
		if (numAttrs > 0)
			strstr << ", ";
		strstr << "\"readonly\", ";
		if (getReadOnly())
			strstr << "\"true\"";
		else
			strstr << "\"false\"";
		numAttrs++;
	}

	if (useHidden)
	{
		if (numAttrs > 0)
			strstr << ", ";
		strstr << "\"hidden\", ";
		if (getHidden())
			strstr << "\"true\"";
		else
			strstr << "\"false\"";
		numAttrs++;
	}

	if (useLocked)
	{
		if (numAttrs > 0)
			strstr << ", ";
		strstr << "\"locked\", ";
		if (getLocked())
			strstr << "\"true\"";
		else
			strstr << "\"false\"";
		numAttrs++;
	}

	if (usePriority)
	{
		if (numAttrs > 0)
			strstr << ", ";
		strstr << "\"priority\", ";
		strstr << getPriority();
		numAttrs++;
	}

	return strstr.str();
}

SBAttribute::SBAttribute()
: m_info(*this),
m_object(nullptr)
{
}

SBAttribute::SBAttribute(std::string name)
: m_name(std::move(name)),
  m_info(*this),
  m_object(nullptr)
{
}

// by default any observers will no longer
// be notified by this subject.
// However, there is no event propagated indicating
// this fact. I might need to add an event lifecycle
// system to accomodate this.
SBAttribute::~SBAttribute() = default;

SBAttributeInfo* SBAttribute::getAttributeInfo()
{
	return &m_info;
}

void SBAttribute::setName(std::string name)
{
	m_name = std::move(name);
}

const std::string& SBAttribute::getName() const
{
	return m_name;
}

void SBAttribute::setObject(SBObject* object)
{
	m_object = object;
}

std::string SBAttribute::write()
{
	return "";
}

void SBAttribute::read()
{
}

SBAttribute* SBAttribute::copy()
{
	return nullptr;
}


SBObject* SBAttribute::getObject()
{
	return m_object;
}

void SBAttribute::copyAttrValue( SBAttribute* inAttr )
{

}

bool SBAttribute::isDefaultValue()
{
	return false;
}

BoolAttribute::BoolAttribute() : SBAttribute()
{
	m_value = false;
	m_defaultValue = false;
}

BoolAttribute::BoolAttribute(const std::string& name, bool val) : SBAttribute(name)
{
	m_value = val;
}

BoolAttribute::~BoolAttribute() = default;

bool BoolAttribute::getValue() const
{
	return m_value;
}

void BoolAttribute::setValue(bool val)
{
	m_value = val;
	notifyObservers();
}

void BoolAttribute::setValueFast(bool val)
{
	m_value = val;
}

void BoolAttribute::setDefaultValue(bool defaultVal)
{
	m_defaultValue = defaultVal;
}

bool BoolAttribute::getDefaultValue() const
{
	return m_defaultValue;
}

std::string BoolAttribute::write()
{
	SBAttributeInfo* info = this->getAttributeInfo();
	std::stringstream strstr;
	std::string boolValue = getValue() ? "True" : "False";
	strstr << "if obj.getAttribute(\""<< getName() << "\") != None :\n";
	strstr << "\tobj.setBoolAttribute(\"" << getName() << "\"," << boolValue << ")\n";
	strstr << "else:\n";
	strstr << "\tattr = obj.createBoolAttribute(\"" << getName() << "\"";
	getValue() ? strstr << ", True" :  strstr << ", False";
	SBObject* object = this->getObject();
	if (object->hasDependency(this))
	{
		strstr << ", True";
	}
	else
	{
		strstr << ", False";
	}
	SBAttributeGroup* group = info->getGroup();
	if (!group || group->getName() == "")
		strstr << ", \"\"";
	else
		strstr << ", \"" << group->getName() << "\"";
	strstr << ", " << info->getPriority();
	info->getReadOnly() ? strstr << ", True" : strstr << ", False";
	info->getLocked() ? strstr << ", True" : strstr << ", False";
	info->getHidden() ? strstr << ", True" : strstr << ", False";
	if (info->getDescription() == "")
		strstr << ", \"\"";
	else
		strstr << ", \"" << info->getDescription() << "\"";
	strstr << ")\n";

	bool val = getDefaultValue();
	bool curVal = getValue();
	strstr << "\tattr.setDefaultValue(";
	val? strstr << "True)\n" : strstr << "False)\n"; 
	strstr << "\tattr.setValue(";
	curVal? strstr << "True)\n" : strstr << "False)\n"; 

	return strstr.str();
}

void BoolAttribute::read()
{
}

SBAttribute* BoolAttribute::copy()
{
	BoolAttribute* a = new BoolAttribute();
	a->setName(getName());
	a->setValue(getValue());
	a->setDefaultValue(getDefaultValue());

	a->getAttributeInfo()->setPriority(this->getAttributeInfo()->getPriority());
	a->getAttributeInfo()->setReadOnly(this->getAttributeInfo()->getReadOnly());
	a->getAttributeInfo()->setLocked(this->getAttributeInfo()->getLocked());
	a->getAttributeInfo()->setHidden(this->getAttributeInfo()->getHidden());
	a->getAttributeInfo()->setDescription(this->getAttributeInfo()->getDescription());
	 
	return a;
}

void BoolAttribute::copyAttrValue( SBAttribute* inAttr )
{
	BoolAttribute* boolAttr = dynamic_cast<BoolAttribute*>(inAttr);
	if (!boolAttr) return; // do nothing
	setValue(boolAttr->getValue());
	setDefaultValue(boolAttr->getDefaultValue());
}

bool BoolAttribute::isDefaultValue()
{
	if (this->getValue() == this->getDefaultValue())
		return true;
	else
		return false;
}

//////////////////////////////////////////////////
IntAttribute::IntAttribute() : SBAttribute()
{
	m_min = std::numeric_limits<int>::lowest();
	m_max = std::numeric_limits<int>::max();
}

IntAttribute::IntAttribute(const std::string& name, int val) : SBAttribute(name)
{
	m_value = val;
	m_min = std::numeric_limits<int>::lowest();
	m_max = std::numeric_limits<int>::max();
}

IntAttribute::IntAttribute(const std::string& name, int val, int min, int max) : SBAttribute(name)
{
	m_value = val;
	m_min = min;
	m_max = max;
}

IntAttribute::~IntAttribute() = default;

int IntAttribute::getValue() const
{
	return m_value;
}

void IntAttribute::setValue(int val)
{
	if (val < getMin())
		m_value = getMin();
	else if (val > getMax())
		m_value = getMax();
	else
		m_value = val;
	
	notifyObservers();
}

void IntAttribute::setValueFast(int val)
{
	if (val < getMin())
		m_value = getMin();
	else if (val > getMax())
		m_value = getMax();
	else
		m_value = val;
}

int IntAttribute::getMin() const
{
	return m_min;
}

void IntAttribute::setMin(int val)
{
	m_min = val;
	notifyObservers();
}

int IntAttribute::getMax() const
{
	return m_max;
}

void IntAttribute::setMax(int val)
{
	m_max = val;
	notifyObservers();
}

void IntAttribute::setDefaultValue(int defaultVal)
{
	m_defaultValue = defaultVal;
}

int IntAttribute::getDefaultValue() const
{
	return m_defaultValue;
}

std::string IntAttribute::write()
{
	SBAttributeInfo* info = this->getAttributeInfo();
	std::stringstream strstr;
	strstr << "if obj.getAttribute(\""<< getName() << "\") != None :\n";
	strstr << "\tobj.setIntAttribute(\"" << getName() << "\"," << getValue() << ")\n";
	strstr << "else:\n";
	strstr << "\tattr = obj.createIntAttribute(\"" << getName() << "\", ";
	strstr << getValue();
	SBObject* object = this->getObject();
	if (object->hasDependency(this))
	{
		strstr << ", True";
	}
	else
	{
		strstr << ", False";
	}
	SBAttributeGroup* group = info->getGroup();
	if (!group || group->getName() == "")
		strstr << ", \"\"";
	else
		strstr << ", \"" << group->getName() << "\"";
	strstr << ", " << info->getPriority();
	info->getReadOnly() ? strstr << ", True" : strstr << ", False";
	info->getLocked() ? strstr << ", True" : strstr << ", False";
	info->getHidden() ? strstr << ", True" : strstr << ", False";
	if (info->getDescription() == "")
		strstr << ", \"\"";
	else
		strstr << ", \"" << info->getDescription() << "\"";
	strstr << ")\n";

	int val = getDefaultValue();
	int curVal = getValue();
	strstr << "\tattr.setDefaultValue(" << val << ")\n";
	strstr << "\tattr.setValue(" << curVal << ")\n";

	return strstr.str();
}

void IntAttribute::read()
{
}

SBAttribute* IntAttribute::copy()
{
	IntAttribute* a = new IntAttribute();
	a->setName(getName());
	a->setValue(getValue());
	a->setDefaultValue(getDefaultValue());
	a->setMin(getMin());
	a->setMax(getMax());

	a->getAttributeInfo()->setPriority(this->getAttributeInfo()->getPriority());
	a->getAttributeInfo()->setReadOnly(this->getAttributeInfo()->getReadOnly());
	a->getAttributeInfo()->setLocked(this->getAttributeInfo()->getLocked());
	a->getAttributeInfo()->setHidden(this->getAttributeInfo()->getHidden());
	a->getAttributeInfo()->setDescription(this->getAttributeInfo()->getDescription());
	 
	return a;
}

void IntAttribute::copyAttrValue( SBAttribute* inAttr )
{
	IntAttribute* intAttr = dynamic_cast<IntAttribute*>(inAttr);
	if (!intAttr) return; // do nothing
	setValue(intAttr->getValue());
	setDefaultValue(intAttr->getDefaultValue());
}


bool IntAttribute::isDefaultValue()
{
	if (this->getValue() == this->getDefaultValue())
		return true;
	else
		return false;
}


//////////////////////////////////////////////////

DoubleAttribute::DoubleAttribute() : SBAttribute()
{
	m_min = std::numeric_limits<double>::lowest();
	m_max = std::numeric_limits<double>::max();
}

DoubleAttribute::DoubleAttribute(const std::string& name, double val) : SBAttribute(name)
{
	m_value = val;
	m_min = std::numeric_limits<double>::lowest();
	m_max = std::numeric_limits<double>::max();
}


DoubleAttribute::DoubleAttribute(const std::string& name, double val, double min, double max) : SBAttribute(name)
{
	m_value = val;
	m_min = min;
	m_max = max;
}

DoubleAttribute::~DoubleAttribute() = default;

double DoubleAttribute::getValue() const
{
	return m_value;
}

void DoubleAttribute::setValue(double val)
{
	if (val < getMin())
		m_value = getMin();
	else if (val > getMax())
		m_value = getMax();
	else
		m_value = val;	
	notifyObservers();
}

void DoubleAttribute::setValueFast(double val)
{
	if (val < getMin())
		m_value = getMin();
	else if (val > getMax())
		m_value = getMax();
	else
		m_value = val;
	
}

void DoubleAttribute::setDefaultValue(double defaultVal)
{
	m_defaultValue = defaultVal;
}

double DoubleAttribute::getDefaultValue() const
{
	return m_defaultValue;
}

double DoubleAttribute::getMin() const
{
	return m_min;
}

void DoubleAttribute::setMin(double val)
{
	m_min = val;
	notifyObservers();
}

double DoubleAttribute::getMax() const
{
	return m_max;
}

void DoubleAttribute::setMax(double val)
{
	m_max = val;
	notifyObservers();
}

std::string DoubleAttribute::write()
{
	SBAttributeInfo* info = this->getAttributeInfo();
	std::stringstream strstr;	
	strstr << "if obj.getAttribute(\""<< getName() << "\") != None :\n";
	strstr << "\tobj.setDoubleAttribute(\"" << getName() << "\"," << getValue() << ")\n";
	strstr << "else:\n";
	strstr << "\tattr = obj.createDoubleAttribute(\"" << getName() << "\", ";
	strstr << getValue();
	SBObject* object = this->getObject();
	if (object->hasDependency(this))
	{
		strstr << ", True";
	}
	else
	{
		strstr << ", False";
	}
	SBAttributeGroup* group = info->getGroup();
	if (!group || group->getName() == "")
		strstr << ", \"\"";
	else
		strstr << ", \"" << group->getName() << "\"";
	strstr << ", " << info->getPriority();
	info->getReadOnly() ? strstr << ", True" : strstr << ", False";
	info->getLocked() ? strstr << ", True" : strstr << ", False";
	info->getHidden() ? strstr << ", True" : strstr << ", False";
	if (info->getDescription() == "")
		strstr << ", \"\"";
	else
		strstr << ", \"" << info->getDescription() << "\"";
	strstr << ")\n";

	double val = getDefaultValue();
	double curVal = getValue();
	strstr << "\tattr.setDefaultValue(" << val << ")\n";	
	strstr << "\tattr.setValue(" << curVal << ")\n";
	return strstr.str();
}

void DoubleAttribute::read()
{
}

SBAttribute* DoubleAttribute::copy()
{
	DoubleAttribute* a = new DoubleAttribute();
	a->setName(getName());
	a->setValue(getValue());
	a->setDefaultValue(getDefaultValue());
	a->setMin(getMin());
	a->setMax(getMax());

	a->getAttributeInfo()->setPriority(this->getAttributeInfo()->getPriority());
	a->getAttributeInfo()->setReadOnly(this->getAttributeInfo()->getReadOnly());
	a->getAttributeInfo()->setLocked(this->getAttributeInfo()->getLocked());
	a->getAttributeInfo()->setHidden(this->getAttributeInfo()->getHidden());
	a->getAttributeInfo()->setDescription(this->getAttributeInfo()->getDescription());
	
	return a;
}

void DoubleAttribute::copyAttrValue( SBAttribute* inAttr )
{
	DoubleAttribute* doubleAttr = dynamic_cast<DoubleAttribute*>(inAttr);
	if (!doubleAttr) return; // do nothing
	setValue(doubleAttr->getValue());
	setDefaultValue(doubleAttr->getDefaultValue());
}

bool DoubleAttribute::isDefaultValue()
{
	if (this->getValue() == this->getDefaultValue())
		return true;
	else
		return false;
}

//////////////////////////////////////////////////

StringAttribute::StringAttribute() : SBAttribute()
{
}

StringAttribute::StringAttribute(const std::string& name, std::string val) : SBAttribute(name)
{
	m_value = std::move(val);
}

StringAttribute::~StringAttribute() = default;

const std::string& StringAttribute::getValue()
{
	return m_value;
}

void StringAttribute::setValue(std::string val)
{
	m_value = std::move(val);
	notifyObservers();
}

void StringAttribute::setValueFast(std::string val)
{
	m_value = std::move(val);
}

void StringAttribute::setDefaultValue(const std::string& defaultVal)
{
	m_defaultValue = defaultVal;
}

const std::string& StringAttribute::getDefaultValue()
{
	return m_defaultValue;
}

std::string StringAttribute::write()
{
	SBAttributeInfo* info = this->getAttributeInfo();
	std::stringstream strstr;
	strstr << "if obj.getAttribute(\""<< getName() << "\") != None :\n";
	strstr << "\tobj.setStringAttribute(\"" << getName() << "\",\"" << getValue() << "\")\n";
	strstr << "else:\n";
	strstr << "\tattr = obj.createStringAttribute(\"" << getName() << "\", ";
	const std::string& val = getValue();
	if (val.empty())
		strstr << " \"\"";
	else
		strstr << " \"" << val << "\"";
	SBObject* object = this->getObject();
	if (object->hasDependency(this))
	{
		strstr << ", True";
	}
	else
	{
		strstr << ", False";
	}
	SBAttributeGroup* group = info->getGroup();
	if (!group || group->getName().empty())
		strstr << ", \"\"";
	else
		strstr << ", \"" << group->getName() << "\"";
	strstr << ", " << info->getPriority();
	info->getReadOnly() ? strstr << ", True" : strstr << ", False";
	info->getLocked() ? strstr << ", True" : strstr << ", False";
	info->getHidden() ? strstr << ", True" : strstr << ", False";
	if (info->getDescription().empty())
		strstr << ", \"\"";
	else
		strstr << ", \"" << info->getDescription() << "\"";
	strstr << ")\n";

	const std::string& defaultVal = getDefaultValue();
	const std::string& curVal = getValue();
	if (defaultVal == "")
	{
		strstr << "\tattr.setDefaultValue(\"\")\n";		
	}
	else
	{
		strstr << "\tattr.setDefaultValue(\"" << defaultVal << "\")\n";		
	}
	strstr << "\tattr.setValue(\"" << curVal << "\")\n";
	strstr << "\tvalidValues = StringVec()\n";
	const std::vector<std::string>& values = getValidValues();
	for (const auto & value : values)
	{
		strstr << "\tvalidValues.append(\"" << value << "\")\n";
	}
	strstr << "\tattr.setValidValues(validValues)\n";
	return strstr.str();
}

void StringAttribute::read()
{
}

void StringAttribute::setValidValues(const std::vector<std::string>& values)
{
	m_validValues.clear();
	for (const auto & value : values)
		m_validValues.emplace_back(value);
}

const std::vector<std::string>& StringAttribute::getValidValues()
{
	return m_validValues;
}

SBAttribute* StringAttribute::copy()
{
	auto* a = new StringAttribute(getName(), getValue());
	a->setDefaultValue(getDefaultValue());
	a->setValidValues(getValidValues());

	a->getAttributeInfo()->setPriority(this->getAttributeInfo()->getPriority());
	a->getAttributeInfo()->setReadOnly(this->getAttributeInfo()->getReadOnly());
	a->getAttributeInfo()->setLocked(this->getAttributeInfo()->getLocked());
	a->getAttributeInfo()->setHidden(this->getAttributeInfo()->getHidden());
	a->getAttributeInfo()->setDescription(this->getAttributeInfo()->getDescription());
		
	return a;
}

void StringAttribute::copyAttrValue( SBAttribute* inAttr )
{
	StringAttribute* strAttr = dynamic_cast<StringAttribute*>(inAttr);
	if (!strAttr) return; // do nothing
	setValue(strAttr->getValue());
	setDefaultValue(strAttr->getDefaultValue());
}

bool StringAttribute::isDefaultValue()
{
	if (this->getValue() == this->getDefaultValue())
		return true;
	else
		return false;
}

//////////////////////////////////////////////////

Vec3Attribute::Vec3Attribute() : SBAttribute()
{
}

Vec3Attribute::Vec3Attribute(const std::string& name) : SBAttribute(name)
{
}

Vec3Attribute::~Vec3Attribute() = default;

const SrVec& Vec3Attribute::getValue()
{
	return m_value;
}

void Vec3Attribute::setValue(const SrVec& val)
{
	m_value = val;
	notifyObservers();
}

void Vec3Attribute::setValueFast(const SrVec& val)
{
	m_value = val;
}

void Vec3Attribute::setDefaultValue(const SrVec& defaultVal)
{
	m_defaultValue = defaultVal;
}

const SrVec& Vec3Attribute::getDefaultValue()
{
	return m_defaultValue;
}

std::string Vec3Attribute::write()
{
	SBAttributeInfo* info = this->getAttributeInfo();
	std::stringstream strstr;
	const SrVec& vec = getValue();
	strstr << "if obj.getAttribute(\""<< getName() << "\") != None :\n";
	strstr << "\tobj.setVec3Attribute(\"" << getName() << "\"," << vec.x << ", " << vec.y << ", " << vec.z << ")\n";
	strstr << "else:\n";
	strstr << "\tattr = obj.createVec3Attribute(\"" << getName() << "\", ";
	strstr << " " << vec.x << ", " << vec.y << ", " << vec.z;
	SBObject* object = this->getObject();
	if (object->hasDependency(this))
	{
		strstr << ", True";
	}
	else
	{
		strstr << ", False";
	}
	SBAttributeGroup* group = info->getGroup();
	if (!group || group->getName() == "")
		strstr << ", \"\"";
	else
		strstr << ", \"" << group->getName() << "\"";
	strstr << ", " << info->getPriority();
	info->getReadOnly() ? strstr << ", True" : strstr << ", False";
	info->getLocked() ? strstr << ", True" : strstr << ", False";
	info->getHidden() ? strstr << ", True" : strstr << ", False";
	if (info->getDescription() == "")
		strstr << ", \"\"";
	else
		strstr << ", \"" << info->getDescription() << "\"";
	strstr << ")\n";

	const SrVec& val = getDefaultValue();
	const SrVec& curVal = getValue();
	strstr << "\tvec = SrVec()\n";
	strstr << "\tvec.setData(0, " << val.x << ")\n";
	strstr << "\tvec.setData(1, " << val.y << ")\n";
	strstr << "\tvec.setData(2, " << val.z << ")\n";
	strstr << "\tattr.setDefaultValue(vec)\n";
	strstr << "\tvec1 = SrVec(" << curVal.x << ", " << curVal.y << ", " << curVal.z << ")\n";
	strstr << "\tattr.setValue(vec1)\n";

	return strstr.str();
}

void Vec3Attribute::read()
{
}


SBAttribute* Vec3Attribute::copy()
{
	Vec3Attribute* a = new Vec3Attribute();
	a->setName(getName());
	a->setValue(getValue());
	a->setDefaultValue(getDefaultValue());


	a->getAttributeInfo()->setPriority(this->getAttributeInfo()->getPriority());
	a->getAttributeInfo()->setReadOnly(this->getAttributeInfo()->getReadOnly());
	a->getAttributeInfo()->setLocked(this->getAttributeInfo()->getLocked());
	a->getAttributeInfo()->setHidden(this->getAttributeInfo()->getHidden());
	a->getAttributeInfo()->setDescription(this->getAttributeInfo()->getDescription());
	 
	return a;
}

void Vec3Attribute::copyAttrValue( SBAttribute* inAttr )
{
	Vec3Attribute* vec3Attr = dynamic_cast<Vec3Attribute*>(inAttr);
	if (!vec3Attr) return; // do nothing
	setValue(vec3Attr->getValue());
	setDefaultValue(vec3Attr->getDefaultValue());
}

bool Vec3Attribute::isDefaultValue()
{
	if (this->getValue() == this->getDefaultValue())
		return true;
	else
		return false;
}

MatrixAttribute::MatrixAttribute()
{
	m_value.identity();
	m_defaultValue.identity();
}

MatrixAttribute::MatrixAttribute(const std::string& name)
{
	m_name = name;
	m_value.identity();
}

MatrixAttribute::~MatrixAttribute() = default;

const SrMat& MatrixAttribute::getValue()
{
	return m_value;
}

void MatrixAttribute::setValue(const SrMat& matrix)
{
	m_value = matrix;
	notifyObservers();
}

void MatrixAttribute::setValueFast(const SrMat& matrix)
{
	m_value = matrix;
}

void MatrixAttribute::setDefaultValue(const SrMat& matrix)
{
	m_defaultValue = matrix;
}

const SrMat& MatrixAttribute::getDefaultValue()
{
	return m_defaultValue;
}

std::string MatrixAttribute::write()
{
	SBAttributeInfo* info = this->getAttributeInfo();
	std::stringstream strstr;



	strstr << "attr = obj.createMatrixAttribute(\"" << getName() << "\", ";
	const SrMat& mat = getValue();
	strstr << "mat = SrMat()\n";
	for (int r = 0; r < 4; r++)
		for (int c = 0; c < 4; c++)
			strstr << "mat.setData(" << mat.getData(r, c) << ")\n";
	strstr << "attr.setValue(mat)\n";

	SBObject* object = this->getObject();
	if (object->hasDependency(this))
	{
		strstr << ", True";
	}
	else
	{
		strstr << ", False";
	}
	SBAttributeGroup* group = info->getGroup();
	if (!group || group->getName() == "")
		strstr << ", \"\"";
	else
		strstr << ", \"" << group->getName() << "\"";
	strstr << ", " << info->getPriority();
	info->getReadOnly() ? strstr << ", True" : strstr << ", False";
	info->getLocked() ? strstr << ", True" : strstr << ", False";
	info->getHidden() ? strstr << ", True" : strstr << ", False";
	if (info->getDescription() == "")
		strstr << ", \"\"";
	else
		strstr << ", \"" << info->getDescription() << "\"";
	strstr << ")\n";

	const SrMat& defMat = getDefaultValue();
	strstr << "defMat = SrMat()\n";
	for (int r = 0; r < 4; r++)
		for (int c = 0; c < 4; c++)
			strstr << "defMat.setData(" << defMat.getData(r, c) << ")\n";

	const SrMat& curMat = getDefaultValue();
	strstr << "curMat = SrMat()\n";
	for (int r = 0; r < 4; r++)
		for (int c = 0; c < 4; c++)
			strstr << "curMat.setData(" << curMat.getData(r, c) << ")\n";
	strstr << "attr.setDefaultValue(defMat)\n";
	strstr << "attr.setValue(curMat)\n";

	return strstr.str();
}

void MatrixAttribute::read()
{
}

SBAttribute* MatrixAttribute::copy()
{
	MatrixAttribute* a = new MatrixAttribute();
	a->setName(getName());
	a->setValue(getValue());
	a->setDefaultValue(getDefaultValue());

	a->getAttributeInfo()->setPriority(this->getAttributeInfo()->getPriority());
	a->getAttributeInfo()->setReadOnly(this->getAttributeInfo()->getReadOnly());
	a->getAttributeInfo()->setLocked(this->getAttributeInfo()->getLocked());
	a->getAttributeInfo()->setHidden(this->getAttributeInfo()->getHidden());
	a->getAttributeInfo()->setDescription(this->getAttributeInfo()->getDescription());
	 
	return a;
}

void MatrixAttribute::copyAttrValue( SBAttribute* inAttr )
{
	MatrixAttribute* matAttr = dynamic_cast<MatrixAttribute*>(inAttr);
	if (!matAttr) return; // do nothing
	setValue(matAttr->getValue());
	setDefaultValue(matAttr->getDefaultValue());
}

bool MatrixAttribute::isDefaultValue()
{
	if (this->getValue() == this->getDefaultValue())
		return true;
	else
		return false;
}


ActionAttribute::ActionAttribute() = default;

ActionAttribute::ActionAttribute(const std::string& name)
{
	// adding this 
	m_name = name;
}

ActionAttribute::~ActionAttribute() = default;

void ActionAttribute::setValue()
{
	notifyObservers();
}

void ActionAttribute::setValueFast()
{
}

std::string ActionAttribute::write()
{
	SBAttributeInfo* info = this->getAttributeInfo();
	std::stringstream strstr;
	strstr << "if obj.getAttribute(\""<< getName() << "\") == None :\n";
	strstr << "\tattr = obj.createActionAttribute(\"" << getName() << "\", ";
	SBObject* object = this->getObject();
	if (object->hasDependency(this))
	{
		strstr << " True";
	}
	else
	{
		strstr << " False";
	}
	SBAttributeGroup* group = info->getGroup();
	if (!group || group->getName() == "")
		strstr << ", \"\"";
	else
		strstr << ", \"" << group->getName() << "\"";
	strstr << ", " << info->getPriority();
	info->getReadOnly() ? strstr << ", True" : strstr << ", False";
	info->getLocked() ? strstr << ", True" : strstr << ", False";
	info->getHidden() ? strstr << ", True" : strstr << ", False";
	if (info->getDescription() == "")
		strstr << ", \"\"";
	else
		strstr << ", \"" << info->getDescription() << "\"";
	strstr << ")\n";

	return strstr.str();
}

void ActionAttribute::read()
{
}

SBAttribute* ActionAttribute::copy()
{
	ActionAttribute* a = new ActionAttribute();
	a->setName(getName());

	a->getAttributeInfo()->setPriority(this->getAttributeInfo()->getPriority());
	a->getAttributeInfo()->setReadOnly(this->getAttributeInfo()->getReadOnly());
	a->getAttributeInfo()->setLocked(this->getAttributeInfo()->getLocked());
	a->getAttributeInfo()->setHidden(this->getAttributeInfo()->getHidden());
	a->getAttributeInfo()->setDescription(this->getAttributeInfo()->getDescription());
	 
	return a;
}

};
