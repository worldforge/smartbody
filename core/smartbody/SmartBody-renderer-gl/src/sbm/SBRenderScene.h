/*
 Copyright (C) 2020 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef SMARTBODY_SBRENDERSCENE_H
#define SMARTBODY_SBRENDERSCENE_H

#include "SBABI.h"

#include <vector>
#include <string>
#include <map>

class SkJoint;
class SrCamera;
struct CameraTrack;

namespace SmartBody {
class SBScene;
class SBController;

class SBRenderScene {

public:
	explicit SBRenderScene(SBScene& scene);
	~SBRenderScene();

	std::vector<std::string> checkVisibility_current_view();
	std::vector<std::string> checkVisibility(const std::string& characterName);
	std::vector<std::string> occlusionTest( const std::vector<std::string>& testPawns);
	std::vector<std::string> frustumTest(const std::vector<std::string>& testPawnNames);
	void updateConeOfSight();
	bool setCameraConeOfSight(const std::string& characterName);
	bool hasConeOfSight() const;
	void removeConeOfSight();


	SBAPI SrCamera* createCamera(const std::string& name);
	SBAPI void removeCamera(SrCamera* camera);
	SBAPI void setActiveCamera(SrCamera* camera);
	SBAPI SrCamera* getActiveCamera();
	SBAPI SrCamera* getCamera(const std::string& name);
	SBAPI void SetCameraLocked(bool locked);
	SBAPI bool IsCameraLocked();
	SBAPI int getNumCameras();
	SBAPI std::vector<std::string> getCameraNames();
	SBAPI bool hasCameraTrack();
	SBAPI void setCameraTrack(const std::string& characterName, const std::string& jointName);
	SBAPI void removeCameraTrack();
	SBAPI void updateTrackedCameras();

	SBScene& mScene;

protected:

	bool _isCameraLocked;
	std::map<std::string, SrCamera*> _cameras;
	std::string _activeCamera;
	std::vector<CameraTrack*> _cameraTracking;

	bool		_coneOfSight;
	SkJoint *	_coneOfSight_leftEye;
	SkJoint *	_coneOfSight_rightEye;
	std::string	_coneOfSight_character;
};
}


#endif //SMARTBODY_SBRENDERSCENE_H
