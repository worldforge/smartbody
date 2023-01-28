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

#include "SBScript.h"
#include <sb/SBAttribute.h>
#include <sb/SBScene.h>

namespace SmartBody {

SBScript::SBScript() : SBAsset()
{
	this->createBoolAttribute("enable", true, true, "Script", 10, false, false, false, "Whether this event is enabled.");
	_enabled = true;
}

SBScript::~SBScript()
{
}

void SBScript::setEnable(bool val)
{
	_enabled = val;
	BoolAttribute* boolAttribute = dynamic_cast<BoolAttribute*>(this->getAttribute("enable"));
	boolAttribute->setValueFast(val);
}

bool SBScript::isEnable()
{
	return _enabled;
}

void SBScript::notify(SBSubject* subject)
{
	SBAttribute* attribute = dynamic_cast<SBAttribute*>(subject);
	if (attribute)
	{
		if (attribute->getName() == "enable")
		{
			BoolAttribute* boolAttr = dynamic_cast<BoolAttribute*>(attribute);
			if (boolAttr)
			{
				this->setEnable(boolAttr->getValue());
			}
		}
	}
}
}
