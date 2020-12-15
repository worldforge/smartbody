/*
 Copyright (C) 2020 Erik Ogenvik
 Copyright (C) 2017 University of Southern California

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

#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include "SBRenderSceneListener.h"
#include "SBRenderScene.h"
#include "sb/SBScene.h"
#include "sb/SBAttribute.h"
#include "sb/SBSkeleton.h"
#include "sb/SBAssetManager.h"
#include "sb/SBRenderAssetManager.h"
#include "sr/sr_camera.h"

namespace SmartBody {

SBRenderSceneListener::SBRenderSceneListener(SBRenderScene& renderScene)
		: mRenderScene(renderScene) {

}


void SBRenderSceneListener::OnCharacterCreate(const std::string& name, const std::string& objectClass) {
	SmartBody::SBPawn* pawn = mRenderScene.mScene.getPawn(name);
	if (!pawn)
		return;

	auto result = mRenderScene.mRenderables.emplace(name, SBRenderScene::Renderable{pawn});
	result.first->second.scene_p.reset(new SkScene(), true);
	result.first->second.scene_p->init(pawn->getSkeleton());

	if (mRenderScene.mScene.getRootGroup()) {
		mRenderScene.mScene.getRootGroup()->add(result.first->second.scene_p.get());
	}

	auto observerResult = mObservers.emplace(name, Observer(result.first->second, *this));
	auto* observer = &observerResult.first->second;

	// add attribute observations
	auto* attr = pawn->getAttribute("mesh");
	if (attr)
		attr->registerObserver(observer);

	attr = pawn->getAttribute("meshScale");
	if (attr)
		attr->registerObserver(observer);

	attr = pawn->getAttribute("deformableMesh");
	if (attr)
		attr->registerObserver(observer);

	attr = pawn->getAttribute("deformableMeshScale");
	if (attr)
		attr->registerObserver(observer);

	attr = pawn->getAttribute("displayType");
	if (attr)
		attr->registerObserver(observer);

	attr = pawn->getAttribute("showBones");
	if (attr)
		attr->registerObserver(observer);

	attr = pawn->getAttribute("showJoints");
	if (attr)
		attr->registerObserver(observer);

	attr = pawn->getAttribute("displayBoneScale");
	if (attr)
		attr->registerObserver(observer);

	attr = pawn->getAttribute("displayJointScale");
	if (attr)
		attr->registerObserver(observer);



//	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
//	if (window)
//		window->updateObjectList();
//	if (name.find("light") == 0) {
//		pawn->registerObserver(this);
//		if (window) {
//			window->curViewer->updateLights();
//		}
//
//	}
//
//	// if this is a camera, update the camera list in the main window
//	SrCamera* camera = dynamic_cast<SrCamera*>(pawn);
//	if (camera) {
//		if (window) {
//			window->updateCameraList();
//			window->redraw();
//		}
//	}
}

void SBRenderSceneListener::OnCharacterDelete(const std::string& name) {
	mObservers.erase(name);
	auto I = mRenderScene.mRenderables.find(name);
	if (I != mRenderScene.mRenderables.end()) {
		if (I->second.scene_p) {
			mRenderScene.mScene.getRootGroup()->remove(I->second.scene_p.get());
		}

		mRenderScene.mRenderables.erase(I);
	}
//
//	// remove any existing deformable mesh
//#if 0
//	if (pawn->dMesh_p)
//	{
//		for (size_t i = 0; i < pawn->dMesh_p->dMeshDynamic_p.size(); i++)
//		{
//			scene->getRootGroup()->remove( pawn->dMesh_p->dMeshDynamic_p[i] );
//		}
//		//delete character->dMesh_p; // AS 1/28/13 causing crash related to mesh instances
//		pawn->dMesh_p = nullptr;
//	}
//#endif
//
//#if 1 //!USE_OGRE_VIEWER
//	// make sure the character isn't associated with the viewer
//
//
//
//	BaseWindow* window = dynamic_cast<BaseWindow*>(scene->getViewer());
//	if (window) {
//		window->updateObjectList(name);
//		if (window->curViewer->_objManipulator.get_selected_pawn() == pawn) {
//			window->curViewer->_objManipulator.set_selected_pawn(nullptr);
//			window->curViewer->_objManipulator.get_active_control()->detach_pawn();
//			window->curViewer->_objManipulator.removeActiveControl();
//		}
//	}
//
//	// if this is a camera, update the camera list in the main window
//	SrCamera* camera = dynamic_cast<SrCamera*>(pawn);
//	if (camera) {
//		if (window) {
//			window->updateCameraList();
//		}
//	}
//#endif
//
//	if (window) {
//		if (window->curViewer->_objManipulator.get_selected_pawn() == pawn) {
//			window->curViewer->_objManipulator.set_selected_pawn(nullptr);
//			window->curViewer->_objManipulator.get_active_control()->detach_pawn();
//			window->curViewer->_objManipulator.removeActiveControl();
//		}
//	}
//
//	if (otherListener)
//		otherListener->OnCharacterDelete(name);
}

void SBRenderSceneListener::OnCharacterUpdate(const std::string& name) {

	auto I = mRenderScene.mRenderables.find(name);
	if (I != mRenderScene.mRenderables.end()) {
		auto& renderable = I->second;
		if (renderable.scene_p && mRenderScene.mScene.getRootGroup()) {
			//Remove any existing entries
			mRenderScene.mScene.getRootGroup()->remove(renderable.scene_p.get());
		}

		renderable.scene_p.reset(new SkScene(), true);
		renderable.scene_p->init(renderable.pawn->getSkeleton());
		renderable.scene_p->visible(renderable.pawn->getBoolAttribute("visible"));

		if (mRenderScene.mScene.getRootGroup()) {
			mRenderScene.mScene.getRootGroup()->add(renderable.scene_p.get());
		}
	}

}

void SBRenderSceneListener::OnPawnCreate(const std::string& name) {
	SBRenderSceneListener::OnCharacterCreate(name, "");
}

void SBRenderSceneListener::OnPawnDelete(const std::string& name) {
	auto camera = dynamic_cast<SrCamera*>(mRenderScene.mScene.getPawn(name));
	if (camera) {
		//Calling this when the camera already has been removed is a no-op, so it's safe.
		mRenderScene.removeCamera(camera);
	}

	SBRenderSceneListener::OnCharacterDelete(name);
}

void SBRenderSceneListener::Observer::notify(SmartBody::SBSubject* subject) {

//	auto* subjectPawn = dynamic_cast<SmartBody::SBPawn*>(subject);
//	if (subjectPawn) {
//		const std::string& pawnName = subjectPawn->getName();
//		if (subjectPawn->getName().find("light") == 0) {
//			// adjust the lights based on the new position and color
//			BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
//			if (window) {
//				window->curViewer->updateLights();
//			}
//
//
//		}
//
//	}

	auto* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute) {
		auto* pawn = renderable.pawn;
		auto* character = dynamic_cast<SmartBody::SBCharacter*>(pawn);
		const std::string& name = attribute->getName();
		if (name == "visible") {
			auto* boolAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			if (boolAttribute) {
				if (renderable.scene_p) {
					renderable.scene_p->visible(boolAttribute->getValue());
				}
			}
		} else if (name == "deformableMeshScale" || name == "meshScale") {
			//SmartBody::util::log("name = deformableMeshScale");
			bool useDeformableMesh = (name == "deformableMeshScale");
			auto* vec3Attribute = dynamic_cast<SmartBody::Vec3Attribute*>(attribute);
			if (vec3Attribute) {
				if (!renderable.meshInstance && useDeformableMesh)
					renderable.meshInstance = std::make_unique<SbmDeformableMeshGPUInstance>();
				else if (!renderable.staticMeshInstance && !useDeformableMesh)
					renderable.staticMeshInstance = std::make_unique<SbmDeformableMeshGPUInstance>();

				auto& meshInstance = useDeformableMesh ? renderable.meshInstance : renderable.staticMeshInstance;
				SrVec scale(vec3Attribute->getValue().x, vec3Attribute->getValue().y, vec3Attribute->getValue().z);
				meshInstance->setMeshScale(scale);
				//SmartBody::util::log("Set mesh scale = %f",doubleAttribute->getValue());
			}
		} else if (name == "deformableMesh" || name == "mesh") {
			bool useDeformableMesh = (name == "deformableMesh");
			auto* strAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
			if (strAttribute) {
				const std::string& value = strAttribute->getValue();
				// clean up any old meshes

				if (value.empty())
					return;

				auto& assetManager = parent.mRenderScene._renderAssetManager;
				DeformableMesh* mesh = assetManager.getDeformableMesh(value);
				if (!mesh) {
					int index = value.find('.');
					if (index != std::string::npos) {
						std::string prefix = value.substr(0, index);
						const std::vector<std::string>& meshPaths = parent.mRenderScene.mScene.getAssetStore().getAssetPaths("mesh");
						for (const auto& meshPath : meshPaths) {
							parent.mRenderScene.mScene.getAssetStore().loadAsset(meshPath + "/" + prefix + "/" + value);
						}
					}
					mesh = assetManager.getDeformableMesh(value);
				}


				if (mesh) {
					if (!renderable.meshInstance && useDeformableMesh) {
						renderable.meshInstance = std::make_unique<SbmDeformableMeshGPUInstance>();
						//renderable.meshInstance = new DeformableMeshInstance();
						renderable.meshInstance->setToStaticMesh(false);
					} else if (!renderable.staticMeshInstance && !useDeformableMesh) {
						renderable.staticMeshInstance = std::make_unique<SbmDeformableMeshGPUInstance>();
						//pawn->dStaticMeshInstance_p = new DeformableMeshInstance();
						renderable.staticMeshInstance->setToStaticMesh(true);

					} else if (!renderable.staticMeshInstance && name == "mesh") {
						renderable.staticMeshInstance = std::make_unique<SbmDeformableMeshGPUInstance>();
						//pawn->dStaticMeshInstance_p = new DeformableMeshInstance();
						renderable.staticMeshInstance->setToStaticMesh(true);
					}

					// create attributes for all the blend shapes if there's any
					auto iter = mesh->blendShapeMap.begin();
					for (; iter != mesh->blendShapeMap.end(); ++iter) {
						for (size_t c = 0; c < iter->second.size(); ++c) {
							//if (strcmp(iter->first.c_str(), (const char*)iter->second[c]->shape().name) == 0)	// you don't create it for base shape
							//continue;

							std::stringstream ss;
							ss << "blendShape.channelName." << (const char*) iter->second[c]->shape().name;
							std::stringstream ss1;
							ss1 << "blendShape.channelWeightLimit." << (const char*) iter->second[c]->shape().name;
							std::stringstream ss2;
							ss2 << "blendShape.controller." << (const char*) iter->second[c]->shape().name;
							pawn->createStringAttribute(ss.str(), (const char*) iter->second[c]->shape().name, true, "Blend Shape", c * 3 + 1, false, false, false, "blend shape channel name");
							pawn->createDoubleAttribute(ss1.str(), 1.0f, true, "Blend Shape", c * 3 + 2, false, false, false, "blend shape channel weight limit");
							pawn->createStringAttribute(ss2.str(), iter->first, true, "Blend Shape", c * 3 + 3, false, false, false, "blend shape controller name");
							pawn->createActionAttribute("updateChannel", true, "Blend Shape", 0, false, false, false, "update blend shape channel");
						}
					}

					if (mesh->blendShapeMap.empty()) {
						// if there are no blendshapes, but there are blendShape.channelName attributes, 
						// then add the morph targets
						std::vector<SmartBody::StringAttribute*> shapeAttributes;
						std::map<std::string, SmartBody::SBAttribute*>& attributes = pawn->getAttributeList();
						for (auto& entry : attributes) {
							SmartBody::SBAttribute* attr = entry.second;
							const std::string& attrName = attr->getName();
							size_t pos = attrName.find("blendShape.channelName.");
							if (pos != std::string::npos) {
								auto* strAttr = dynamic_cast<SmartBody::StringAttribute*>(attr);
								shapeAttributes.emplace_back(strAttr);
							}
						}

						int numShapeAttributes = shapeAttributes.size();
						if (numShapeAttributes > 0) {
							// make space for all the attributes
							mesh->morphTargets.insert(std::pair<std::string, std::vector<std::string> >("face", std::vector<std::string>()));
							auto shapeIter = mesh->morphTargets.begin();
							(*shapeIter).second.resize(numShapeAttributes);


							bool hasNeutral = false;
							for (auto& shapeAttribute : shapeAttributes) {
								const std::string& attrName = shapeAttribute->getName();
								// get the shape name and value
								std::string shapeName = attrName.substr(23);

								std::string shapeChannel = shapeAttribute->getValue();
								if (shapeChannel == "Neutral") {
									auto& assetManager = parent.mRenderScene._renderAssetManager;

									DeformableMesh* neutralMesh = assetManager.getDeformableMesh(shapeName);
									SrSnModel* staticModel = neutralMesh->dMeshStatic_p[0];
									SrModel& s = staticModel->shape();
									std::string blendShapeMapName = (const char*) s.name;
									(*shapeIter).second[0] = blendShapeMapName;
									//mesh->blendShapeMap.insert(std::pair<std::string, std::vector<SrSnModel*> >(neutralMesh->getName(), std::vector<SrSnModel*>() ));
									mesh->blendShapeMap.insert(std::pair<std::string, std::vector<SrSnModel*> >(blendShapeMapName, std::vector<SrSnModel*>()));
									auto blendshapeIter = mesh->blendShapeMap.begin();
									(*blendshapeIter).second.resize(numShapeAttributes);
									auto* model = new SrSnModel();
									model->shape(staticModel->shape());
									model->shape().name = staticModel->shape().name;
									model->changed(true);
									model->visible(false);
									(*blendshapeIter).second[0] = model;
									model->ref();
									hasNeutral = true;
									// add the shape to the deformable mesh
									//mesh->dMeshStatic_p.emplace_back(model);
									//mesh->dMeshDynamic_p.emplace_back(model);
								}

							}

							SrVec blendShapeOffset = pawn->getVec3Attribute("blendShape.translationOffset");
							auto blendshapeIter = mesh->blendShapeMap.begin();
							if (blendshapeIter != mesh->blendShapeMap.end()) {
								(*blendshapeIter).second.resize(numShapeAttributes);

								int count = 1;
								if (hasNeutral) {
									for (auto& shapeAttribute : shapeAttributes) {
										const std::string& attrName = shapeAttribute->getName();
										// get the shape name and value
										std::string shapeName = attrName.substr(23);
										std::string shapeChannel = shapeAttribute->getValue();
										if (shapeChannel == "Neutral")
											continue;
										if (blendshapeIter != mesh->blendShapeMap.end())
											(*shapeIter).second[count] = shapeName;
										auto& assetManager = parent.mRenderScene._renderAssetManager;

										DeformableMesh* shapeModel = assetManager.getDeformableMesh(shapeName);

										if (shapeModel) {
											shapeModel->dMeshStatic_p[0]->shape().translate(blendShapeOffset);
											shapeModel->dMeshDynamic_p[0]->shape().translate(blendShapeOffset);
											(*blendshapeIter).second[count] = shapeModel->dMeshStatic_p[0];
											shapeModel->dMeshStatic_p[0]->ref();
										} else {
											(*blendshapeIter).second[count] = nullptr;
										}
										count++;
									}
								}
							}
						}
					}
					//mesh->rebuildVertexBuffer(true);

					auto& meshInstance = useDeformableMesh ? renderable.meshInstance : renderable.staticMeshInstance;
					meshInstance->setDeformableMesh(mesh);

					//meshInstance->setSkeleton(pawn->getSkeleton());	

					meshInstance->setPawn(pawn);
					pawn->setStringAttribute("displayType", pawn->getStringAttribute("displayType"));

#if 0
					for (size_t i = 0; i < pawn->dMesh_p->dMeshDynamic_p.size(); i++)
					{
						scene->getRootGroup()->add( pawn->dMesh_p->dMeshDynamic_p[i] );
					}
#endif
				}
			}
		} else if (name == "displayType") {
			auto* strAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
			if (strAttribute) {
				const std::string& value = strAttribute->getValue();
				if (value == "bones") {
					if (renderable.scene_p)
						renderable.scene_p->set_visibility(1, 0, 0, 0);
					if (renderable.meshInstance)
						renderable.meshInstance->setVisibility(0);
				} else if (value == "visgeo") {
					if (renderable.scene_p)
						renderable.scene_p->set_visibility(0, 1, 0, 0);
					if (renderable.meshInstance)
						renderable.meshInstance->setVisibility(0);
				} else if (value == "colgeo") {
					if (renderable.scene_p)
						renderable.scene_p->set_visibility(0, 0, 1, 0);
					if (renderable.meshInstance)
						renderable.meshInstance->setVisibility(0);
				} else if (value == "axis") {
					if (renderable.scene_p)
						renderable.scene_p->set_visibility(0, 0, 0, 1);
					if (renderable.meshInstance)
						renderable.meshInstance->setVisibility(0);
				} else if (value == "skinWeight") {
					if (renderable.scene_p)
						renderable.scene_p->set_visibility(0, 0, 0, 0);
					if (renderable.meshInstance)
						renderable.meshInstance->setVisibility(2);
				} else if (value == "wireframeMesh") {
					if (renderable.scene_p)
						renderable.scene_p->set_visibility(0, 0, 0, 0);
					if (renderable.meshInstance)
						renderable.meshInstance->setVisibility(3);
				} else if (value == "mesh") {
					if (renderable.scene_p)
						renderable.scene_p->set_visibility(0, 0, 0, 0);
					if (renderable.meshInstance)
						renderable.meshInstance->setVisibility(1);
					SbmDeformableMeshGPU::useGPUDeformableMesh = false;
				} else if (value == "GPUmesh") {
					if (renderable.scene_p)
						renderable.scene_p->set_visibility(0, 0, 0, 0);
					SbmDeformableMeshGPU::useGPUDeformableMesh = true;
					if (renderable.meshInstance)
						renderable.meshInstance->setVisibility(1);

				}
			}
		} else if (name == "showBones") {
			bool val = pawn->getBoolAttribute("showBones");
			if (renderable.scene_p) {
				renderable.scene_p->setShowBones(val);
				renderable.scene_p->init(pawn->getSkeleton());
			}

		} else if (name == "showJoints") {
			bool val = pawn->getBoolAttribute("showJoints");
			if (renderable.scene_p) {
				renderable.scene_p->setShowJoints(val);
				renderable.scene_p->init(pawn->getSkeleton());
			}
		} else if (name == "displayBoneScale") {
			double val = pawn->getDoubleAttribute("displayBoneScale");
			if (renderable.scene_p) {
				renderable.scene_p->set_skeleton_radius((float) val);
				renderable.scene_p->init(pawn->getSkeleton());
			}
		} else if (name == "displayJointScale") {
			double val = pawn->getDoubleAttribute("displayJointScale");
			if (renderable.scene_p) {
				auto skeleton = pawn->getSkeleton();
				if (skeleton) {
					int numJoints = skeleton->getNumJoints();
					for (int j = 0; j < numJoints; j++) {
						SmartBody::SBJoint* joint = skeleton->getJoint(j);
						renderable.scene_p->setJointRadius(joint, (float) val);
						renderable.scene_p->init(pawn->getSkeleton());
					}
				}
			}
//		} else if (name == "useDefaultLights") {
//			BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
//			window->curViewer->updateLights();
		} else if (name == "showStaticMesh") {
			renderable.useStaticMesh = pawn->getBoolAttribute("showStaticMesh");
		}

		// check for scene attributes
		/*
		if (name == "showGrid")
		{
			SmartBody::BoolAttribute* boolAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			bool showGrid = boolAttribute->getValue();
			BaseWindow* window = dynamic_cast<BaseWindow*>(scene->getViewer());
			if (window)
			{
				window->updateObjectList(name);
				if (showGrid)
					window->curViewer->_data->gridMode = ModeShowGrid;
				else
					window->curViewer->_data->gridMode = ModeNoGrid;

		}
		else if (name == "gridSize")
		{
		}
		else if (name == "showFloor")
		{
		}
		else if (name == "floorColor")
		{
		}
		else if (name == "floorTexture")
		{
		}
		else if (name == "floorSize")
		{
		}
		else if (name == "backgroundColor")
		{
		}
		*/


	}
}


void SBRenderSceneListener::OnSimulationUpdate() {
	for (auto& entry : mRenderScene.mRenderables) {
		auto& renderable = entry.second;
		if (renderable.scene_p) {
			renderable.scene_p->update();
		}
	}

	mRenderScene.updateTrackedCameras();

	if (mRenderScene.hasConeOfSight()) {
		mRenderScene.updateConeOfSight();
	}


}

}