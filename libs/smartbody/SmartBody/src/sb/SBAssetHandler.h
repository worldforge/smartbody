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

#ifndef _SBASSETHANDLER_H_
#define _SBASSETHANDLER_H_

#include <sb/SBTypes.h>
#include <sb/SBAsset.h>
#include <vector>
#include <string>
#include <memory>

namespace SmartBody {

class SBAssetHandler
{
	public:
		SBAPI SBAssetHandler();
		SBAPI virtual ~SBAssetHandler();

		SBAPI virtual std::vector<std::string> getAssetTypes();
		SBAPI virtual std::vector<std::unique_ptr<SBAsset>> getAssets(SBScene& scene, const std::string& path);

	protected:
		std::string checkPath(const std::string& path);

	protected:
		std::vector<std::string> assetTypes;
};


}

#endif