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
	assetTypes.push_back("asf");
}

SBAssetHandlerAsf::~SBAssetHandlerAsf()
{
}

std::vector<SBAsset*> SBAssetHandlerAsf::getAssets(const std::string& path)
{
	std::vector<SBAsset*> assets;

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
	if (SmartBody::SBScene::getScene()->getAttribute("globalSkeletonScale"))
		scale = SmartBody::SBScene::getScene()->getDoubleAttribute("globalSkeletonScale");

	SmartBody::SBSkeleton* skeleton = new SmartBody::SBSkeleton();
	bool ok = ParserASFAMC::parseAsf(*skeleton, filestream, float(scale));
	if (ok)
	{
		skeleton->setName(fileName + extension);
		skeleton->setFileName(convertedPath);
		assets.push_back(skeleton);
	}
	else
	{
		delete skeleton;
		SmartBody::util::log("Could not load skeleton from file %s", convertedPath.c_str());
	}

	return assets;
}

};
