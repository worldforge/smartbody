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

#include "SBService.h"
#include <sb/SBSubject.h>
#include <sb/SBAttribute.h>
#include <sb/SBPawn.h>
#include <sb/SBCharacter.h>

namespace SmartBody {

SBService::SBService() : SBObject()
{
	_enabled = false;

	createBoolAttribute("enable", false, true, "basic", 5, false, false, false, "Enables or disables the service.");
}

SBService::~SBService()
{
}

void SBService::setEnable(bool val)
{
	_enabled = val;
}

bool SBService::isEnable()
{
	return _enabled;
}

void SBService::notify(SBSubject* subject)
{
	SBAttribute* attribute = dynamic_cast<SBAttribute*>(subject);
	if (attribute)
	{
		if (attribute->getName() == "enable")
		{
			BoolAttribute* enableAttr = dynamic_cast<BoolAttribute*>(attribute);
			if (enableAttr)
			{
				setEnable(enableAttr->getValue());
			}
		}
	}
}

void SBService::onPawnCreate(SBPawn* pawn)
{
}

void SBService::onPawnDelete(SBPawn* pawn)
{
}

void SBService::onCharacterCreate(SBCharacter* character)
{
}

void SBService::onCharacterDelete(SBCharacter* character)
{
}


}
