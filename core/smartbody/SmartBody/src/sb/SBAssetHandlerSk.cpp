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

#include "SBAssetHandlerSk.h"

#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include "SBUtilities.h"


#if defined(EMSCRIPTEN)
#include <emscripten.h>
#endif

namespace SmartBody {

SBAssetHandlerSk::SBAssetHandlerSk()
{
	assetTypes.push_back("sk");
}

SBAssetHandlerSk::~SBAssetHandlerSk()
{
}

std::vector<std::unique_ptr<SBAsset>> SBAssetHandlerSk::getAssets(const std::string& path)
{
	std::vector<std::unique_ptr<SBAsset>> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath.empty())
		return assets;
	
	boost::filesystem::path p(convertedPath);
	std::string fileName = boost::filesystem::basename( p );
	std::string extension =  boost::filesystem::extension( p );
	FILE* myfile = fopen(convertedPath.c_str(), "rt");
	SrInput input(myfile);
	auto skeleton = std::make_unique<SmartBody::SBSkeleton>();
	double scale = 1.0;
	if (SmartBody::SBScene::getScene()->getAttribute("globalSkeletonScale"))
		scale = SmartBody::SBScene::getScene()->getDoubleAttribute("globalSkeletonScale");
	if( skeleton->loadSk( input, scale) )
	{
		skeleton->ref();
		skeleton->setFileName(convertedPath);
		skeleton->setName(fileName + extension);
		assets.emplace_back(std::move(skeleton));
	}
	else
	{
		SmartBody::util::log("Could not load .sk file %s", convertedPath.c_str());
	}

	return assets;
}

};
