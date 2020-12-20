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

#include "SBAssetHandlerAmc.h"
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include <sb/SBAssetManager.h>
#include "SBUtilities.h"
#include <sbm/ParserASFAMC.h>

namespace SmartBody {

SBAssetHandlerAmc::SBAssetHandlerAmc()
{
	assetTypes.emplace_back("amc");
}

SBAssetHandlerAmc::~SBAssetHandlerAmc() = default;

std::vector<std::unique_ptr<SBAsset>> SBAssetHandlerAmc::getAssets(const std::string& path)
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
	if (SmartBody::SBScene::getScene()->getAttribute("globalMotionScale"))
		scale = SmartBody::SBScene::getScene()->getDoubleAttribute("globalMotionScale");

	boost::intrusive_ptr<SBSkeleton> skeleton;
	// find a skeleton with a prefix that matches the first part of this motion file
	int pos = fileName.find_first_of('_');
	if (pos != std::string::npos)
	{
		std::string skelPrefix = fileName.substr(0, pos);
		// find a skeleton with that name
		std::string skelName = skelPrefix + ".asf";
		skeleton = SmartBody::SBScene::getScene()->getAssetManager()->getSkeleton(skelName);
		if (!skeleton)
		{
			SmartBody::util::log("No skeleton with name %s could be find to accompany .asf file, %s not loaded.", skelName.c_str(), convertedPath.c_str());
			return assets;
		}
	}

	if (!skeleton)
	{
		SmartBody::util::log(".amc files need accompanying .asf skeleton, %s not loaded.", convertedPath.c_str());
		return assets;
	}

	
	auto motion = std::make_unique<SmartBody::SBMotion>();
	bool ok = ParserASFAMC::parseAmc(*motion, skeleton.get(), filestream, float(scale));
	if (ok)
	{
		motion->setName(fileName);
		assets.emplace_back(std::move(motion));
	}
	else
	{
		SmartBody::util::log("Could not load .amc file %s", convertedPath.c_str());
	}

	return assets;
}

};
