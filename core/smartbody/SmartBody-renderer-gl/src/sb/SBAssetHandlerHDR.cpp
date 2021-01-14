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
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include "sbm/GPU/SbmTexture.h"
#include <sb/SBScene.h>
#include <sb/SBAttribute.h>

namespace SmartBody {

SBAssetHandlerHdr::SBAssetHandlerHdr() {
	assetTypes.emplace_back("hdr");
}

SBAssetHandlerHdr::~SBAssetHandlerHdr() = default;

std::vector<std::unique_ptr<SBAsset>> SBAssetHandlerHdr::getAssets(SBScene& scene, const std::string& path) {
	std::vector<std::unique_ptr<SBAsset>> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath.empty())
		return assets;

	boost::filesystem::path p(convertedPath);
	std::string fileName = boost::filesystem::basename(p);
	std::string extension = boost::filesystem::extension(p);

	std::string textureName = fileName + extension;

	auto texture = std::make_unique<SbmTexture>();
	texture->data.textureName = textureName;
	if (!texture->loadImage(fileName.c_str())) {
		SmartBody::util::log("ERROR: Can't load image %s from %s. Invalid path? Is it an 8-bit image?", textureName.c_str(), fileName.c_str());
	} else {
		assets.emplace_back(std::move(texture));
	}

	return assets;
}


};
