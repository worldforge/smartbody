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

SBAPI static void save(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup = false, const std::string& mediaPath = "");

SBAPI static void exportScene(SmartBody::SBRenderScene& renderScene, const std::vector<std::string>& aspects, std::string mediaPath = "", bool remoteSetup = false);

SBAPI static void saveSceneSettings(SmartBody::SBRenderScene& renderScene, std::ostream& ostream);

SBAPI static void exportScenePackage(SmartBody::SBRenderScene& renderScene, std::string outDir, const std::string& outZipArchiveName = "");

SBAPI static void exportCharacter(SmartBody::SBRenderScene& renderScene, const std::string& charName, std::string outDir);


void saveScene(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveAssets(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup, const std::string& mediaPath);

void saveCameras(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void savePawns(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveCharacters(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveLights(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveRetargets(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveBlends(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveJointMaps(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveFaceDefinitions(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveGestureMaps(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveLipSyncing(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void saveServices(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

void savePositions(SmartBody::SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup);

}


#endif //SMARTBODY_SCENEEXPORTER_H
