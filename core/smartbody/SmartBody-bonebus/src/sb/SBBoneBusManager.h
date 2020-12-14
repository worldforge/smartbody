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

#ifndef _SBBONEBUSMANAGER_
#define _SBBONEBUSMANAGER_

#include <sb/SBTypes.h>
#include <sb/SBService.h>
#include "bonebus.h"
#include <sbm/general_param_setting.h>
#include <memory>

namespace SmartBody {

	class SBSkeleton;

class SBBoneBusManager : public SmartBody::SBService
{
	public:

	struct BoneBusEntry {
		std::unique_ptr<bonebus::BoneBusCharacter> bonebusCharacter;
		SBCharacter* character;
	};

		SBAPI SBBoneBusManager();
		SBAPI ~SBBoneBusManager();

		SBAPI void setEnable(bool val) override;
		SBAPI void setHost(const std::string& host);
		SBAPI const std::string& getHost();

		SBAPI void start() override;
		SBAPI void beforeUpdate(double time) override;
		SBAPI void update(double time) override;
		SBAPI void afterUpdate(double time) override;
		SBAPI void stop() override;
		SBAPI bonebus::BoneBusClient& getBoneBus();
		SBAPI void NetworkSendSkeleton( bonebus::BoneBusCharacter * character, SmartBody::SBSkeleton* skeleton, GeneralParamMap * param_map );
		SBAPI void notify(SBSubject* subject) override;

		void onCharacterCreate(SBCharacter *character) override;
		void onCharacterDelete(SBCharacter *character) override;

	private:
		bonebus::BoneBusClient _boneBus;
		std::string _host;

		std::map<std::string, BoneBusEntry> _entries;

};

}

#endif
