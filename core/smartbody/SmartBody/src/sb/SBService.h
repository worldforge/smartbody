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

#ifndef _SBSERVICE_H_
#define _SBSERVICE_H_

#include <sb/SBTypes.h>
#include <sb/SBObject.h>
#include "sb/SBSceneOwned.h"
namespace SmartBody {

class SBSubject;
class SBPawn;

class SBCharacter;
class SBScene;

class SBService : public SBObject, public SBSceneOwned
{
	public:
		SBAPI explicit SBService(SBScene& scene);
		SBAPI ~SBService();

		SBAPI virtual void setEnable(bool val);
		SBAPI virtual bool isEnable();

		SBAPI virtual void onPawnCreate(SBPawn* pawn);
		SBAPI virtual void onPawnDelete(SBPawn* pawn);
		SBAPI virtual void onCharacterCreate(SBCharacter* character);
		SBAPI virtual void onCharacterDelete(SBCharacter* character);

		SBAPI void notify(SBSubject* subject) override;

	protected:
		bool _enabled;
};

}


#endif