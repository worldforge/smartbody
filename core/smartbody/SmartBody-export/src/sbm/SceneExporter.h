/*
 Copyright (C) 2020 Erik Ogenvik

 SmartBody-lib is free software: you can redistribute it and/or
 modify it under the terms of the Lesser GNU General Public License
 as published by the Free Software Foundation, version 3 of the
 license.

 SmartBody-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 Lesser GNU General Public License for more details.

 You should have received a copy of the Lesser GNU General Public
 License along with SmarBody-lib.  If not, see:
     http://www.gnu.org/licenses/lgpl-3.0.txt
 */

#ifndef SMARTBODY_SCENEEXPORTER_H
#define SMARTBODY_SCENEEXPORTER_H

#include "SBABI.h"
#include <ostream>
#include <vector>

namespace SmartBody {
class SBRenderScene;
class SBSteerManager;

SBAPI void save(SBRenderScene& renderScene,
				SBSteerManager* steerManager,
				std::ostream& ostream,
				bool remoteSetup = false,
				const std::string& mediaPath = "");

SBAPI void exportScene(SBRenderScene& renderScene,
					   SBSteerManager* steerManager,
					   std::ostream& ostream,
					   const std::vector<std::string>& aspects,
					   const std::string& mediaPath = "",
					   bool remoteSetup = false);

SBAPI void saveSceneSettings(SBRenderScene& renderScene, std::ostream& ostream);

SBAPI void exportScenePackage(SBRenderScene& renderScene,
							  SBSteerManager* steerManager,
							  const std::string& outDir,
							  const std::string& outZipArchiveName = "");

SBAPI void exportCharacter(SBRenderScene& renderScene,
						   SBSteerManager* steerManager,
						   const std::string& charName,
						   std::string outDir);


void saveScene(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveAssets(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup, const std::string& mediaPath);

void saveCameras(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void savePawns(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveCharacters(SBRenderScene& renderScene, SBSteerManager* steerManager, std::ostream& ostream, bool remoteSetup);

void saveLights(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveRetargets(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveBlends(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveJointMaps(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveFaceDefinitions(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveGestureMaps(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveLipSyncing(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveServices(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void savePositions(SBRenderScene& renderScene,
				   SBSteerManager* steerManager,
				   std::ostream& ostream,
				   bool remoteSetup);

}


#endif //SMARTBODY_SCENEEXPORTER_H
