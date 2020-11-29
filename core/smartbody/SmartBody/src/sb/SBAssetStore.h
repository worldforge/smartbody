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

#ifndef SMARTBODY_SBASSETSTORE_H
#define SMARTBODY_SBASSETSTORE_H

#include "SBAssetHandler.h"

#include "sbm/sr_path_list.h"

#include <map>
#include <vector>
#include <string>
#include <memory>

namespace SmartBody {
class SBScene;

struct SBAssetsProcessor {
	virtual void processAssets(std::vector<std::unique_ptr<SBAsset>>&) = 0;
};

struct SBAssetsHistory {
	std::vector<std::string> _assetHistory;

	SBAPI std::vector<std::string>& getAssetHistory();
	SBAPI void clearAssetHistory();
	void addAssetHistory(const std::string& str);
};

class SBAssetStore {
public:

	explicit SBAssetStore(SBScene& scene);

	SBAPI void addAssetPath(const std::string& type, const std::string& path);
	SBAPI std::vector<std::string> getAssetPaths(const std::string& type);
	SBAPI std::vector<std::string> getLocalAssetPaths(const std::string& type);
	SBAPI void removeAssetPath(const std::string& type, const std::string& path);
	SBAPI void removeAllAssetPaths(const std::string& type);
	SBAPI void loadAssets();
	SBAPI std::vector<std::unique_ptr<SBAsset>> loadAsset(const std::string& assetPath);
	SBAPI void loadAssetsFromPath(const std::string& assetPath);
	SBAPI std::string findAsset(const std::string& type, const std::string& assetName);


	SBAPI void addAssetHandler(std::unique_ptr<SBAssetHandler> handler);
	SBAPI const std::vector<std::unique_ptr<SBAssetHandler>>& getAssetHandlers() const;
	//SBAPI std::vector<SBAssetHandler*> getAssetHandlers(const std::string& assetTypes) const;

	srPathList* getPathList(const std::string& type);
	std::string findFileName(const std::string& type, const std::string& filename);

	void addAssetProcessor(SBAssetsProcessor* processor);
	void removeAssetProcessor(SBAssetsProcessor* processor);

protected:

	std::string findAssetFromLocation(const std::string& filepath, const std::string& assetName);

	SBScene& _scene;

	std::vector<SBAssetsProcessor*> _assetProcessors;

	std::vector<std::unique_ptr<SBAssetHandler>> _assetHandlers;
	std::map<std::string, std::vector<SBAssetHandler*> > _assetHandlerMap;
	std::map<std::string, srPathList> _paths;


};
}

#endif //SMARTBODY_SBASSETSTORE_H
