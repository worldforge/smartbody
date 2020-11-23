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

#ifndef _SBASSETHANDLERSKM_H_
#define _SBASSETHANDLERSKM_H_

#include <sb/SBTypes.h>
#include <vector>
#include <sb/SBAsset.h>
#include <sb/SBAssetHandler.h>

namespace SmartBody {

class SBAssetHandlerSkm : public SBAssetHandler
{
	public:
		SBAPI SBAssetHandlerSkm();
		SBAPI ~SBAssetHandlerSkm() override;

		SBAPI std::vector<std::unique_ptr<SBAsset>> getAssets(const std::string& path) override;

};


}

#endif