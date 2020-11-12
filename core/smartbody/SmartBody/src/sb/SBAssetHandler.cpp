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

#include "SBAssetHandler.h"
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBUtilities.h>

namespace SmartBody {

SBAssetHandler::SBAssetHandler()
{
}

SBAssetHandler::~SBAssetHandler()
{
}

std::vector<std::string> SBAssetHandler::getAssetTypes()
{
	return assetTypes;
}

std::vector<SBAsset*> SBAssetHandler::getAssets(const std::string& path)
{
	return std::vector<SBAsset*>();
}

std::string SBAssetHandler::checkPath(const std::string& path)
{
	boost::filesystem::path pathname(path);
	if( !boost::filesystem::exists( pathname ) )
	{
#if (BOOST_VERSION > 104400)
		SmartBody::util::log("Asset path \"%s\" not found.",  pathname.string().c_str());
#else
		SmartBody::util::log("Asset path \"%s\" not found.", pathname.native_file_string().c_str());
#endif
		return "";
	}

	if( boost::filesystem::is_directory( pathname ) ) // path indicates a directory
	{
		#if (BOOST_VERSION > 104400)
		SmartBody::util::log("Asset path \"%s\" is a directory.",  pathname.string().c_str());
#else
		SmartBody::util::log("Asset path \"%s\" is a directory.", pathname.native_file_string().c_str());
#endif
		return "";
	}

	std::string convertedPath = pathname.string();
#ifdef WIN32
	boost::replace_all(convertedPath, "\\", "/");
#endif

	return convertedPath;
}

};

