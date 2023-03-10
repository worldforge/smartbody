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

#include "SBAssetHandlerSkm.h"

#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include "SBUtilities.h"

namespace SmartBody {

SBAssetHandlerSkm::SBAssetHandlerSkm()
{
	assetTypes.emplace_back("skm");
}

SBAssetHandlerSkm::~SBAssetHandlerSkm() = default;

std::vector<std::unique_ptr<SBAsset>> SBAssetHandlerSkm::getAssets(SBScene& scene, const std::string& path)
{
	std::vector<std::unique_ptr<SBAsset>> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath.empty())
		return assets;

	auto motion = std::make_unique<SmartBody::SBMotion>(scene);
	bool parseSuccessful = false;

	SrInput in( convertedPath.c_str(), "rt" );
	SrString fullin_string;
	in.getall( fullin_string );
	SrInput fullin( (const char *)fullin_string );
	fullin.filename( convertedPath.c_str() ); // copy filename for error message
	double scale = 1.0;
	if (scene.getAttribute("globalMotionScale"))
		scale = scene.getDoubleAttribute("globalMotionScale");
	parseSuccessful = motion->load( fullin, scale );
	if (parseSuccessful)
	{
		motion->filename(convertedPath.c_str());
		assets.emplace_back(std::move(motion));
	}
	else
		SmartBody::util::log("Could not read .skm file %s", convertedPath.c_str());

	return assets;
}

};
