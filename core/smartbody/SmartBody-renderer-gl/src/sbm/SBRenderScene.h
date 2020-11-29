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
#include "sbm_deformable_mesh.h"

#include "sr/sr_shared_ptr.hpp"

#include <vector>
#include <string>
#include <map>

class SkJoint;
class SrCamera;
struct CameraTrack;

namespace SmartBody {
class SBScene;
class SBController;
class SBPawn;
class SBRenderSceneListener;
class SBRenderAssetManager;

class SBRenderScene {
public:
	friend class SBRenderSceneListener;

	struct Renderable {
		SBPawn* pawn;
		boost::intrusive_ptr<SkScene> scene_p;
		std::unique_ptr<DeformableMeshInstance> meshInstance;
		std::unique_ptr<DeformableMeshInstance> staticMeshInstance;
		bool useStaticMesh;

		DeformableMeshInstance* getActiveMesh() const {
			if (useStaticMesh) {
				if (staticMeshInstance) {
					return staticMeshInstance.get();
				}
			} else {
				if (meshInstance){
					return meshInstance.get();
				}
			}
			return nullptr;
		}
	};

	explicit SBRenderScene(SBScene& scene, SBRenderAssetManager& renderAssetManager);
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

	SBAPI void rescalePartialMeshSkeleton(const std::string& meshName,
										  const std::string& skelName,
										  const std::string& rootJointName,
										  const std::vector<std::string>& skipMeshNames,
										  float scaleRatio,
										  float blendThreshold = 0.025f);

	SBRenderAssetManager& _renderAssetManager;
	SBScene& mScene;

	const Renderable* getRenderable(const std::string& name) const;
	Renderable* getRenderable(const std::string& name);

	const std::map<std::string, Renderable>& getRenderables() const {
		return mRenderables;
	}

protected:


	bool _isCameraLocked;
	std::map<std::string, SrCamera*> _cameras;
	std::string _activeCamera;
	std::vector<CameraTrack*> _cameraTracking;

	bool		_coneOfSight;
	SkJoint *	_coneOfSight_leftEye;
	SkJoint *	_coneOfSight_rightEye;
	std::string	_coneOfSight_character;

	std::map<std::string, Renderable> mRenderables;
	std::unique_ptr<SBRenderSceneListener> mListener;
};
}


#endif //SMARTBODY_SBRENDERSCENE_H
