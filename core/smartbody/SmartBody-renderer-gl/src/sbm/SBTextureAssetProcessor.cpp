/*
 Copyright (C) 2020 Erik Ogenvik

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

 */

#include "SBTextureAssetProcessor.h"

SBTextureAssetProcessor::SBTextureAssetProcessor(SbmTextureManager& textureManager)
		: _textureManager(textureManager) {

}

void SBTextureAssetProcessor::processAssets(std::vector<std::unique_ptr<SmartBody::SBAsset>>& assets) {
	for (auto& asset : assets) {
		if (asset) {
			auto texturePtr = dynamic_cast<SbmTexture*>(asset.get());
			std::shared_ptr<SbmTexture> sharedPtr(texturePtr);
			asset.release();
			//TOOD: check if we should add textures as different types, or if we just should just do away with the "texture type" feature.
			_textureManager.addTexture(texturePtr->getName(), SbmTextureManager::TEXTURE_DIFFUSE, sharedPtr);
		}
	}
}

