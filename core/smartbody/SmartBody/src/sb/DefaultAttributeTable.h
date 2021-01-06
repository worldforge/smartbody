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

#ifndef DEFAULT_ATTRIBUTE_TABLE_H
#define DEFAULT_ATTRIBUTE_TABLE_H

#include <sb/SBAttribute.h>
#include <vector>
#include <map>
#include <string>

class VariablePointer
{
public:
	enum { TYPE_BOOL, TYPE_INT, TYPE_FLOAT, TYPE_DOUBLE, TYPE_STRING, TYPE_VEC3, TYPE_MATRIX, TYPE_ACTION };
	int varType;
	void* varPtr;
public:
	VariablePointer& operator= (const VariablePointer& rt);
	void updateVariableFromAttribute(SmartBody::SBAttribute* attr);
};

typedef std::pair<SmartBody::SBAttribute*,VariablePointer> AttributeVarPair;

class DefaultAttributeTable
{
protected:
	std::vector<AttributeVarPair> _defaultAttributes;	
	std::map<std::string, SmartBody::SBAttributeGroup*> _defaultGroups;
public:
	DefaultAttributeTable();
	~DefaultAttributeTable();

	void setDefaultAttributeGroupPriority(const std::string& groupName, int priority);
	int setDefaultAttributeGroupPriority(const std::string& groupName);

	void addDefaultAttributeDouble(const std::string& name, double defaultValue, const std::string& attributeGroup, double* varPtr = nullptr);
	void addDefaultAttributeFloat(const std::string& name, float defaultValue, const std::string& attributeGroup, float* varPtr = nullptr);
	void addDefaultAttributeInt(const std::string& name, int defaultValue, const std::string& attributeGroup, int* varPtr = nullptr);
	void addDefaultAttributeBool(const std::string& name, bool defaultValue, const std::string& attributeGroup, bool* varPtr = nullptr);
	void addDefaultAttributeString(const std::string& name, const std::string& defaultValue, const std::string& attributeGroup, std::string* varPtr = nullptr);
	void addDefaultAttributeVec3(const std::string&name, SrVec& defaultValue, const std::string& attributeGroup, SrVec* varPtr = nullptr);
	void addDefaultAttributeMatrix(const std::string& name, SrMat& defaultValue, const std::string& attributeGroup, SrMat* varPtr = nullptr);
	void addDefaultAttributeAction(const std::string& name, const std::string& attributeGroup, bool* varPtr = nullptr);

	std::vector<AttributeVarPair>& getDefaultAttributes();
	std::map<std::string, SmartBody::SBAttributeGroup*>& getDefaultGroups();	
};

#endif
