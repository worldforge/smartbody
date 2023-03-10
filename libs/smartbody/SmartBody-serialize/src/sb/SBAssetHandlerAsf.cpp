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

#include "SBAssetHandlerAsf.h"
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include "SBUtilities.h"
#include <sbm/ParserASFAMC.h>

namespace SmartBody {

SBAssetHandlerAsf::SBAssetHandlerAsf()
{
	assetTypes.emplace_back("asf");
}

SBAssetHandlerAsf::~SBAssetHandlerAsf() = default;

std::vector<std::unique_ptr<SBAsset>> SBAssetHandlerAsf::getAssets(SBScene& scene, const std::string& path)
{
	std::vector<std::unique_ptr<SBAsset>> assets;

	boost::filesystem::path pathname(path);
	if( !boost::filesystem::exists( pathname ) )
	{
#if (BOOST_VERSION > 104400)
		SmartBody::util::log("Asset path \"%s\" not found.",  pathname.string().c_str());
#else
		SmartBody::util::log("Asset path \"%s\" not found.", pathname.native_file_string().c_str());
#endif
		return assets;
	}

	if( boost::filesystem::is_directory( pathname ) ) // path indicates a directory
	{
		#if (BOOST_VERSION > 104400)
		SmartBody::util::log("Asset path \"%s\" is a directory.",  pathname.string().c_str());
#else
		SmartBody::util::log("Asset path \"%s\" is a directory.", pathname.native_file_string().c_str());
#endif
		return assets;
	}

	std::string convertedPath = pathname.string();
#ifdef WIN32
	boost::replace_all(convertedPath, "\\", "/");
#endif

	boost::filesystem::path p(convertedPath);
	std::string fileName = boost::filesystem::basename( p );
	std::string extension =  boost::filesystem::extension( p );
	std::ifstream filestream(convertedPath.c_str());

	double scale = 1.0;
	if (scene.getAttribute("globalSkeletonScale"))
		scale = scene.getDoubleAttribute("globalSkeletonScale");

	auto skeleton = std::make_unique<SmartBody::SBSkeleton>(scene);
	bool ok = ParserASFAMC::parseAsf(*skeleton, filestream, float(scale));
	if (ok)
	{
		skeleton->setName(fileName + extension);
		skeleton->setFileName(convertedPath);
		assets.emplace_back(std::move(skeleton));
	}
	else
	{
		SmartBody::util::log("Could not load skeleton from file %s", convertedPath.c_str());
	}

	return assets;
}

};
