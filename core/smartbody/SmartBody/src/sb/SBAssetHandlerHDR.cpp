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

#include "SBAssetHandlerHDR.h"
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sbm/GPU/SbmTexture.h>
#include <sb/SBScene.h>
#include <sb/SBAttribute.h>

namespace SmartBody {

SBAssetHandlerHdr::SBAssetHandlerHdr()
{
	assetTypes.push_back("hdr");
}

SBAssetHandlerHdr::~SBAssetHandlerHdr()
{
}

std::vector<SBAsset*> SBAssetHandlerHdr::getAssets(const std::string& path)
{
	std::vector<SBAsset*> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath == "")
		return assets;

	boost::filesystem::path p(convertedPath);
	std::string fileName = boost::filesystem::basename( p );
	std::string extension =  boost::filesystem::extension( p );

	SbmTextureManager& texManager = SbmTextureManager::singleton();
	std::string textureName = fileName + extension;
	texManager.loadTexture(SbmTextureManager::TEXTURE_HDR_MAP, textureName.c_str(), convertedPath.c_str());
	SbmTexture* tex = texManager.findTexture(SbmTextureManager::TEXTURE_HDR_MAP, textureName.c_str());
	if (tex)
	{		
		assets.push_back(tex);		
	}

	texManager.updateEnvMaps();

	return assets;
}


};
