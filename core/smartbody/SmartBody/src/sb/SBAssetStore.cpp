/*************************************************************
 Copyright (C) 2020 Erik Ogenvik
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

#include "SBAssetStore.h"
#include "SBUtilities.h"
#include "SBScene.h"
#include "sbm/sbm_constants.h"

#include <boost/filesystem.hpp>

namespace SmartBody {
std::vector<std::string> SBAssetStore::getAssetPaths(const std::string& type) {
	std::vector<std::string> list;

	auto I = _paths.find(type);
	if (I != _paths.end()) {
		auto& path = I->second;
		path.reset();
		std::string nextPath = path.next_path(true);
		while (!nextPath.empty()) {
			list.emplace_back(nextPath);
			nextPath = path.next_path(true);
		}
	}

	return list;
}

std::vector<std::string> SBAssetStore::getLocalAssetPaths(const std::string& type) {
	return getAssetPaths(type);
}

void SBAssetStore::addAssetPath(const std::string& type, const std::string& path) {
	_paths[type].insert(path);
}

void SBAssetStore::removeAssetPath(const std::string& type, const std::string& path) {
	auto I = _paths.find(type);
	if (I != _paths.end()) {
		I->second.remove(path);
	}
}

void SBAssetStore::removeAllAssetPaths(const std::string& type) {
	_paths.erase(type);
}

std::string SBAssetStore::findAsset(const std::string& type, const std::string& assetName) {
	// load the assets from the mesh directories
	std::vector<std::string> meshPaths = this->getAssetPaths(type);
	for (auto& meshPath : meshPaths) {
		std::string location = findAssetFromLocation(meshPath, assetName);
		if (!location.empty())
			return location;
	}
	return "";
}

std::string SBAssetStore::findAssetFromLocation(const std::string& filepath, const std::string& assetName) {
	boost::filesystem::path path(filepath);

	const std::string& mediaPath = _scene.getMediaPath();
	if (!mediaPath.empty()) {
		// if the path already contains the media path, ignore it
		if (filepath.find(mediaPath) == 0) {
			// do nothing
		} else {
			if (boost::filesystem::exists(path)) {
				// do nothing
			} else {
				boost::filesystem::path finalPath(mediaPath);
				finalPath /= path;
				if (boost::filesystem::exists(finalPath))
					path = finalPath;
				else {
					SmartBody::util::log("Could not load assets from %s, does not exist", finalPath.string().c_str());
					return "";
				}
			}
		}

	}

	if (boost::filesystem::is_regular_file(path)) {
		std::string base = boost::filesystem::basename(path);
		std::string ext = boost::filesystem::basename(path);
		if (assetName == base + ext)
			return filepath;
	}

	std::vector<boost::filesystem::path> dirs;
	dirs.emplace_back(path);
	while (!dirs.empty()) {
		boost::filesystem::path curPath = dirs[0];
		dirs.erase(dirs.begin());
		for (boost::filesystem::directory_iterator it(curPath), eit; it != eit; ++it) {
			if (boost::filesystem::is_directory(it->path())) {
				// ignore directories that start with a '.'
				std::string basename = boost::filesystem::basename(it->path());
				std::string extension = boost::filesystem::extension(it->path());
				if (basename.empty() && extension.find('.') == 0)
					continue;
				dirs.emplace_back(it->path());
			} else if (boost::filesystem::is_regular_file(it->path())) {
				std::string base = boost::filesystem::basename(it->path());
				std::string ext = boost::filesystem::extension(it->path());
				if (assetName == base + ext)
					return filepath;
			}
			/*
			else if (boost::filesystem::is_symlink(p))
			{
				loadAsset(p.string());
			}
			*/
		}
	}
	for (auto& dir : dirs) {
		std::string location = findAssetFromLocation(dir.string(), assetName);
		if (!location.empty())
			return location;
	}
	return "";
}

void SBAssetStore::loadAssetsFromPath(const std::string& assetPath) {
	SmartBody::util::log("Loading [%s]", assetPath.c_str());

	boost::filesystem::path path(assetPath);

	const std::string& mediaPath = _scene.getMediaPath();
	if (!mediaPath.empty()) {
		// if the path already contains the media path, ignore it
		if (assetPath.find(mediaPath) == 0) {
			// do nothing
		} else {
			if (boost::filesystem::exists(path) ||
				boost::filesystem::is_directory(path)) {
				// do nothing
			} else {
				boost::filesystem::path finalPath(mediaPath);
				finalPath /= path;
				if (boost::filesystem::exists(finalPath))
					path = finalPath;
				else {
					SmartBody::util::log("Could not load assets from %s, does not exist", finalPath.string().c_str());
					return;
				}
			}
		}

	}
	std::vector<boost::filesystem::path> dirs;

	if (boost::filesystem::is_directory(path)) {
		dirs.emplace_back(path);
		while (!dirs.empty()) {
			boost::filesystem::path curPath = dirs[0];
			dirs.erase(dirs.begin());
			for (boost::filesystem::directory_iterator it(curPath), eit; it != eit; ++it) {
				if (boost::filesystem::is_directory(it->path())) {
					// ignore directories that start with a '.'
					std::string basename = boost::filesystem::basename(it->path());
					std::string extension = boost::filesystem::extension(it->path());
					if (basename.empty() && extension.find('.') == 0)
						continue;
					dirs.emplace_back(it->path());
				} else if (boost::filesystem::is_regular_file(it->path())) {
					loadAsset(it->path().string());
				}
				/*
				else if (boost::filesystem::is_symlink(p))
				{
					loadAsset(p.string());
				}
				*/
			}
		}
		for (auto& dir : dirs) {
			loadAssetsFromPath(dir.string());
		}
	} else {
		loadAsset(assetPath);
	}


/*
	load_motions(assetPath.c_str(), true);
	load_skeletons(assetPath.c_str(), true);
*/
}

void SBAssetStore::loadAssets() {
	//TODO: Is this used? Should we do this selection in the store? Should we load all paths?
	auto I = _paths.find("motion");
	if (I != _paths.end()) {
		auto& pathList = I->second;
		pathList.reset();

		std::string path = pathList.next_path(true);
		while (!path.empty()) {
			loadAssetsFromPath(path);
//		load_motions(path.c_str(), true);
//		load_skeletons(path.c_str(), true);
			path = pathList.next_path(true);
		}

	}
}


std::vector<std::unique_ptr<SBAsset>> SBAssetStore::loadAsset(const std::string& assetPath) {
	//SmartBody::util::log("Loading asset [%s]", assetPath.c_str());
	const std::string& mediaPath = _scene.getMediaPath();
	boost::filesystem::path p(mediaPath);
	boost::filesystem::path assetP(assetPath);

	boost::filesystem::path abs_p = boost::filesystem::absolute(assetP);
	if (boost::filesystem::exists(abs_p)) {
		p = assetP;
	} else {
		p /= assetP;
	}
	boost::filesystem::path final = boost::filesystem::absolute(p);


	std::string finalPath = p.string();
#ifdef WIN32
	finalPath = SmartBody::util::replace(finalPath, "\\", "/");
#endif

	std::vector<std::unique_ptr<SBAsset>> allAssets;

	//SmartBody::util::log("Asset loading from final path = %s", finalPath.c_str());

	// make sure the file exists and is readable
	std::ifstream file(finalPath.c_str());
	if (!file.good()) {
		SmartBody::util::log("File %s cannot be read, asset will not be loaded.", finalPath.c_str());
		return allAssets;
	}

	if (boost::filesystem::is_directory(p)) {
		SmartBody::util::log("%s is a directory, cannot load asset.", finalPath.c_str());
		return allAssets;
	}


	std::string ext = boost::filesystem::extension(finalPath);
	std::string baseName = boost::filesystem::basename(finalPath);
	std::string fileName = baseName + ext;

	std::string extNoDot;
	if (!ext.empty())
		extNoDot = ext.substr(1);

	// get all the asset handlers for that extension
	auto I = _assetHandlerMap.find(extNoDot);
	if (I != _assetHandlerMap.end()) {
		auto& assetHandlers = I->second;
		for (auto assetHandler : assetHandlers) {
				std::vector<std::unique_ptr<SBAsset>> assets = assetHandler->getAssets(finalPath);
			for (auto & asset : assets) {
				asset->setFullFilePath(finalPath);
				allAssets.emplace_back(std::move(asset));
			}
		}

		for (auto& assetProcessor : _assetProcessors) {
			assetProcessor->processAssets(allAssets);
		}

//		// place the assets in their proper place
//		for (size_t x = 0; x < allAssets.size(); x++) {
//			SBAsset* asset = allAssets[x];
//			SmartBody::SBMotion* motion = dynamic_cast<SmartBody::SBMotion*>(asset);
//			if (motion) {
//				SBMotion* existingMotion = this->getMotion(motion->getName());
//				if (existingMotion) {
//					std::string name = this->getAssetNameVariation(existingMotion);
//					SmartBody::util::log("Motion named %s already exist, changing name to %s", motion->getName().c_str(), name.c_str());
//					motion->setName(name);
//				}
//				this->addMotion(motion);
//				addAssetHistory("MOTION " + motion->getName());
//				continue;
//			}
//			auto skeleton = dynamic_cast<SmartBody::SBSkeleton*>(asset);
//			if (skeleton) {
//				SBSkeleton* existingSkeleton = this->getSkeleton(skeleton->getName());
//				if (existingSkeleton) {
//					std::string name = this->getAssetNameVariation(existingSkeleton);
//					SmartBody::util::log("Skeleton named %s already exist, changing name to %s", skeleton->getName().c_str(), name.c_str());
//					skeleton->setName(name);
//				}
//				this->addSkeleton(skeleton);
//				skeleton->ref();
//				addAssetHistory("SKELETON " + skeleton->getName());
//				continue;
//			}
//			DeformableMesh* mesh = dynamic_cast<DeformableMesh*>(asset);
//			if (mesh) {
//				DeformableMesh* existingMesh = this->getDeformableMesh(mesh->getName());
//				if (existingMesh) {
//					std::string name = this->getAssetNameVariation(existingMesh);
//					SmartBody::util::log("Mesh named %s already exist, changing name to %s", existingMesh->getName().c_str(), name.c_str());
//					existingMesh->setName(name);
//				}
//				this->addMesh(mesh);
//				addAssetHistory("MESH " + mesh->getName());
//				continue;
//			}
//			SbmTexture* texture = dynamic_cast<SbmTexture*>(asset);
//			if (texture) {
//				addAssetHistory("ENVMAP " + texture->getName());
//				continue;
//			}
//
//			SmartBody::util::log("Unknown asset type for file %s", assetPath.c_str());
//		}
	}

	return allAssets;
}

void SBAssetStore::addAssetProcessor(SBAssetsProcessor* processor) {
	_assetProcessors.emplace_back(processor);
}

void SBAssetStore::removeAssetProcessor(SBAssetsProcessor* processor) {
	auto I = std::find(_assetProcessors.begin(), _assetProcessors.end(), processor);
	if (I != _assetProcessors.end()) {
		_assetProcessors.erase(I);
	}
}

std::string SBAssetStore::findFileName(const std::string& type, const std::string& filename)
{
	auto I = _paths.find(type);
	if (I != _paths.end()) {
		char buffer[ MAX_FILENAME_LEN ];
		
		auto& path = I->second;

		path.reset();
		std::string curFilename = path.next_filename( buffer, filename.c_str() );
		while (!curFilename.empty())
		{
			SmartBody::util::log("next curFilename = '%s'", curFilename.c_str());
			FILE* file = fopen(curFilename.c_str(), "r");
			SmartBody::util::log("file = %d", file);
			if (file)
			{
				fclose(file);
				return curFilename;
			}
			else
			{
				curFilename = path.next_filename( buffer, filename.c_str() );
			}
		}

		// check absolute path
		FILE* file = fopen(filename.c_str(), "r");
		if (file)
		{
			fclose(file);
			return filename;
		}
	} else {
		SmartBody::util::log("findFileName(): type name needs to be 'script', 'motion', 'mesh' or 'audio'");

	}
	

	return "";
}

void SBAssetStore::addAssetHandler(std::unique_ptr<SBAssetHandler> handler) {
	if (!handler)
		return;

	// make sure the handler doesn't exist already
	auto iter = std::find(_assetHandlers.begin(), _assetHandlers.end(), handler);
	if (iter != _assetHandlers.end())
		return;

	std::vector<std::string> assetTypes = handler->getAssetTypes();

	// add the asset types to the map
	_assetHandlers.emplace_back(std::move(handler));
	auto& emplacedHandler = _assetHandlers.back();
	for (auto& assetType : assetTypes) {
		_assetHandlerMap[assetType].emplace_back(emplacedHandler.get());
	}
}

const std::vector<std::unique_ptr<SBAssetHandler>>& SBAssetStore::getAssetHandlers() const{
	return _assetHandlers;
}

SBAssetStore::SBAssetStore(SBScene& scene) : _scene(scene){

}

srPathList* SBAssetStore::getPathList(const std::string& type) {
	auto I = _paths.find(type);
	if (I != _paths.end()) {
		return &I->second;
	}
	return nullptr;
}


void SBAssetsHistory::addAssetHistory(const std::string& str)
{
	_assetHistory.emplace_back(str);
}

std::vector<std::string>& SBAssetsHistory::getAssetHistory()
{
	return _assetHistory;
}

void SBAssetsHistory::clearAssetHistory()
{
	_assetHistory.clear();
}
//
//std::vector<SBAssetHandler*> SBAssetStore::getAssetHandlers(const std::string& assetTypes)  const {
//	// search for file extensions in lower case only
//	std::string lowerCaseAssetTypes = SmartBody::util::toLower(assetTypes);
//	auto mapIter = _assetHandlerMap.find(lowerCaseAssetTypes);
//	if (mapIter == _assetHandlerMap.end()) {
//		return {};
//	} else {
//
//	}
//
//		return (*mapIter).second;
//}


}