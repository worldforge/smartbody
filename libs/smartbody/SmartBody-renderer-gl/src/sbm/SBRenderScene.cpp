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

#include "SBRenderScene.h"
#include "sb/SBScene.h"
#include "sb/SBService.h"
#include "sb/SBCharacter.h"
#include "sb/SBSkeleton.h"
#include "sr/sr_gl.h"
#include "sr/sr_camera.h"
#include "sk/sk_joint.h"

#include "sbm/GPU/SbmShader.h"
#include "sbm/GPU/SbmTexture.h"
#include "SBRenderSceneListener.h"
#include "sb/SBRenderAssetManager.h"
#include "sb/SBAssetManager.h"

namespace {
bool blendScalePos(SrVec &v, SrVec &rootPos, int rootIdx, int headIdx, SrVec4i& boneIdx, SrVec4& boneWeight, float blendThreshold, float scaleRatio)
{
	if (v.y > rootPos.y + blendThreshold)
		return false;

	float rootHeadSkinWeightMax = 0.9f;
	float rootHeadSkinWeightMin = 0.7f;
	float rootSkinWeightMin = 0.1f;
	float headSkinWeight = -1.f;
	float rootSkinWeight = -1.f;
	for (int k = 0; k < 4; k++)
	{
		if (boneIdx[k] == rootIdx)
		{
			rootSkinWeight = boneWeight[k];
			//SmartBody::util::log("neck skin weight = %f", rootSkinWeight);
		}
		else if (boneIdx[k] == headIdx)
		{
			headSkinWeight = boneWeight[k];
			//SmartBody::util::log("head skin weight = %f", headSkinWeight);
		}
	}
	if (headSkinWeight > rootHeadSkinWeightMax) return false; // don't scale the part when head weight is larger than threshold
	//if (v.y > rootPos.y && rootSkinWeight < rootSkinWeightMin) return false;
	//float blendRatio = (v.y - rootPos.y + blendThreshold) / (blendThreshold*2.0);
	float blendRatio = (v.y - rootPos.y) / (blendThreshold);
	//float blendRatio = 0.f;
	if (blendRatio < 0.f) blendRatio = 0.0;
// 	if (headSkinWeight > rootHeadSkinWeightMin)
// 	{
// 		blendRatio = (headSkinWeight - rootHeadSkinWeightMin) / (rootHeadSkinWeightMax - rootHeadSkinWeightMin);
// 		SmartBody::util::log("head skin weight = %f, root skin weight = %f, blendRatio = %f", headSkinWeight, rootSkinWeight, blendRatio);
// 	}

	float blendScaleRatio = (blendRatio)+(1.0f - blendRatio)*scaleRatio;
	v = rootPos + (v - rootPos)*blendScaleRatio;
	return true;
}

bool blendScalePos2(SrVec &v, SrVec &rootPos, SrVec &neckPos, int rootIdx, int neckIdx, SrVec4i& boneIdx, SrVec4& boneWeight, float blendThreshold, float scaleRatio)
{
	if (v.y > rootPos.y) // don't change anything
		return false;

	float rootHeadSkinWeightMax = 0.9f;
	float rootHeadSkinWeightMin = 0.7f;
	float rootSkinWeightMin = 0.1f;
	float neckSkinWeight = -1.f;
	float rootSkinWeight = -1.f;
	for (int k = 0; k < 4; k++)
	{
		if (boneIdx[k] == rootIdx)
		{
			rootSkinWeight = boneWeight[k];
			//SmartBody::util::log("neck skin weight = %f", rootSkinWeight);
		}
		else if (boneIdx[k] == neckIdx)
		{
			neckSkinWeight = boneWeight[k];
			//SmartBody::util::log("head skin weight = %f", headSkinWeight);
		}
	}
	if (rootSkinWeight > rootHeadSkinWeightMax) return false; // don't scale the part when head weight is larger than threshold
	if (v.y > neckPos.y && neckSkinWeight < 0.1f) return false;

	//float blendRatio = (v.y - rootPos.y + blendThreshold) / (blendThreshold*2.0);
	float blendRatio = (rootPos.y - v.y) / (blendThreshold);
	//float blendRatio = 0.f;
	if (blendRatio > 1.f) blendRatio = 1.f;
	//if (v.y < neckPos.y) blendRatio = 1.f;
	//if (rootPos.y < neckPos.y) blendRatio = 0.f
	// 	if (headSkinWeight > rootHeadSkinWeightMin)
	// 	{
	// 		blendRatio = (headSkinWeight - rootHeadSkinWeightMin) / (rootHeadSkinWeightMax - rootHeadSkinWeightMin);
	// 		SmartBody::util::log("head skin weight = %f, root skin weight = %f, blendRatio = %f", headSkinWeight, rootSkinWeight, blendRatio);
	// 	}

	float blendScaleRatio = (1.0f - blendRatio)+(blendRatio)*scaleRatio;
	v = rootPos + (v - rootPos)*blendScaleRatio;
	return true;
}

}

namespace SmartBody {
SBRenderScene::SBRenderScene(SBScene& scene, SBRenderAssetManager& renderAssetManager) :
		mScene(scene),
		_renderAssetManager(renderAssetManager),
		_isCameraLocked(false),
		_coneOfSight(false),
		_coneOfSight_leftEye(nullptr),
		_coneOfSight_rightEye(nullptr),
		mListener(std::make_unique<SBRenderSceneListener>(*this)) {
	//TODO is this needed?
	SbmShaderManager::singleton().setViewer(nullptr);

	mScene.addSceneListener(mListener.get());

	mScene.registerObjectProvider("envmap", SBScene::Provider{
			[&](const std::string& suffix)->SmartBody::SBObject* {
				//TODO: allow getting envmap texture
//				SbmTextureManager& texManager = SbmTextureManager::singleton();
//				auto texture = texManager.findTexture(suffix.c_str());
//				if (texture) {
//					return texture.get();
//				}
				return nullptr;
			},
			[](SmartBody::SBObject&) {
				return "";
			}

	});
	//HACK until we can refactor this setup
	mScene._removeAllPawnsCallback = [&]() {
		// clear the cameras
		_cameras.clear();
	};
}

SBRenderScene::~SBRenderScene() {
	mScene.removeSceneListener(mListener.get());

	mScene._removeAllPawnsCallback = {};
	mScene.removeObjectProvider("envmap");

	SbmTextureManager::destroy_singleton();
	SbmShaderManager::destroy_singleton();
}

//
//		std::vector<std::string> SBScene::checkVisibility_current_view()
//
//		Returns a list of visible pawns from current view port
//
std::vector<std::string> SBRenderScene::checkVisibility_current_view() {
	//SmartBody::util::log("Checking visibility...\n");
	std::vector<std::string> pawnNames = mScene.getPawnNames();
	auto removeItem = std::remove(pawnNames.begin(), pawnNames.end(), "cameraDefault");
	pawnNames.erase(removeItem, pawnNames.end());
	std::vector<std::string> visible_pawns = frustumTest(pawnNames);

#if 0
	SrFrustum frustum;
	frustum.extractFrustum();

	// Checks visibility
	const std::vector<std::string>& pawns = mScene.getPawnNames();

	for (	std::vector<std::string>::const_iterator pawnIter = pawns.begin();
			pawnIter != pawns.end();
			pawnIter++)
	{
		SmartBody::SBPawn* pawn		= mScene.getPawn((*pawnIter));

		SrBox pawn_bb				= pawn->getBoundingBox();

		if(frustum.pointInFrustum(pawn_bb.a) || frustum.pointInFrustum(pawn_bb.b))
			visible_pawns.emplace_back(pawn->getName());

		/*
		// Usign OgreCamera
		SrCamera* camera = dynamic_cast<SrCamera*>(pawn);
		if (!camera) {
			SrBox pawn_bb				= pawn->getBoundingBox();
			Ogre::Vector3 point_min		= Ogre::Vector3(pawn_bb.a.x, pawn_bb.a.y, pawn_bb.a.z);
			Ogre::Vector3 point_max		= Ogre::Vector3(pawn_bb.b.x, pawn_bb.b.y, pawn_bb.b.z);

			if(ogreCam->isVisible(point_min) || ogreCam->isVisible(point_max))
				SmartBody::util::log("Pawn %s is visible", pawn->getName().c_str());
		}
		*/
	}
#endif
	std::vector<std::string> nonOccludePawns = occlusionTest(visible_pawns);

	return nonOccludePawns;
}


//
//		std::vector<std::string> SBScene::checkVisibility(const std::string& characterName)
//
//		Returns a list of visible pawns from a given chracter
//
std::vector<std::string> SBRenderScene::checkVisibility(const std::string& characterName) {
	std::vector<std::string> visible_pawns, nonOccludePawns;
#if !defined(EMSCRIPTEN)
	//	Gets the character from which we want to look from
	SmartBody::SBCharacter* character = mScene.getCharacter(characterName);

	if (!character) {
		SmartBody::util::log("Character %s not found.", characterName.c_str());
		return visible_pawns;
	}

	character->getSkeleton()->invalidate_global_matrices();
	character->getSkeleton()->update_global_matrices();

	//	Left eye world location
	SkJoint* leftEye = character->getSkeleton()->search_joint("eyeball_left");
	if (!leftEye) {
		SmartBody::util::log("Can't find 'eyeball_left' joint.\n");
		return visible_pawns;
	}
	const SrMat& gmat_leftEye = leftEye->gmat();
	SrVec leftEye_location = SrVec(gmat_leftEye.get(3, 0), gmat_leftEye.get(3, 1), gmat_leftEye.get(3, 2));

	//	Right eye world location
	SkJoint* rightEye = character->getSkeleton()->search_joint("eyeball_right");
	if (!rightEye) {
		SmartBody::util::log("Can't find 'eyeball_right' joint.\n");
		return visible_pawns;
	}
	const SrMat& gmat_rightEye = rightEye->gmat();
	SrVec rightEye_location = SrVec(gmat_rightEye.get(3, 0), gmat_rightEye.get(3, 1), gmat_rightEye.get(3, 2));

	//	Center of the eyse world coordinates
	SrVec center_eyes_location = (leftEye_location + rightEye_location) / 2;

	float scale = 1.f / mScene.getScale();
	float znear = 0.01f * scale;
	float zfar = 100.0f * scale;
	float eyebeamLength = 100 * character->getHeight() / 175.0f;

	SrVec localAxis_right = rightEye->localGlobalAxis(2) * eyebeamLength;
	SrVec eyes_look_at_right = localAxis_right * rightEye->gmat();
	SrVec localAxis_left = leftEye->localGlobalAxis(2) * eyebeamLength;
	SrVec eyes_look_at_left = localAxis_left * leftEye->gmat();
	SrVec eyes_look_at = (eyes_look_at_right + eyes_look_at_left) / 2;

	//	Stores current camera setup, to restore it at the end
	SrCamera* camera = getActiveCamera();
	SrVec tmp_eye = camera->getEye();
	SrVec tmp_center = camera->getCenter();
	float tmp_near = camera->getNearPlane();
	float tmp_far = camera->getFarPlane();

	//	Sets the camera setup to simulate characters viewport
	//	Sets FROM where the chararacter is looking
	camera->setEye(center_eyes_location.x,
				   center_eyes_location.y,
				   center_eyes_location.z);

	//	Sets where the character looking is AT
	camera->setCenter(eyes_look_at.x,
					  eyes_look_at.y,
					  eyes_look_at.z);

	//	Sets near clip plane
	camera->setNearPlane(znear);

	//	Sets far clip plane
	camera->setFarPlane(zfar);

#if USE_GL_FIXED_PIPELINE
	SrMat mat(SrMat::NotInitialized);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrix(camera->get_perspective_mat(mat));
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrix(camera->get_view_mat(mat));
	//	Creates characters frustrm
	std::vector<std::string> pawnNames = mScene.getPawnNames();
	auto removeItem = std::remove(pawnNames.begin(), pawnNames.end(), characterName);
	pawnNames.erase(removeItem, pawnNames.end());
	visible_pawns = frustumTest(pawnNames);
	nonOccludePawns = occlusionTest(visible_pawns);
	// Restores matrices and camera set up
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
#endif
	//	Sets FROM where the chararacter is looking
	camera->setEye(tmp_eye.x,
				   tmp_eye.y,
				   tmp_eye.z);

	//	Sets where the character looking is AT
	camera->setCenter(tmp_center.x,
					  tmp_center.y,
					  tmp_center.z);

	//	Sets near clip plane
	camera->setNearPlane(tmp_near);

	//	Sets near clip plane
	camera->setFarPlane(tmp_far);
#endif
	//	Returns visible pawns
	return nonOccludePawns;
}


std::vector<std::string> SBRenderScene::occlusionTest(const std::vector<std::string>& testPawns) {
	std::vector<std::string> visiblePawns;
#if !defined(__ANDROID__) && !defined(EMSCRIPTEN) && !defined(SB_IPHONE)
	float m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	SrMat modelViewMat = SrMat(m);
	// sort the pawn from front to back
	std::vector<std::pair<float, int> > pawnZSortedList;
	for (unsigned int i = 0; i < testPawns.size(); i++) {
		SmartBody::SBPawn* pawn = mScene.getPawn(testPawns[i]);

		SBGeomObject* geomObject = pawn->getGeomObject();
		auto* box = dynamic_cast<SBGeomBox*>(geomObject);
		SrBox pawn_bb;
		pawn_bb = pawn->getBoundingBox();
		std::vector<SrVec> bbPts = pawn_bb.getCorners();
		float zMin = -1e30f;
		for (auto& bbPt : bbPts) {
			SrVec camPt = bbPt * modelViewMat;
			if (camPt.z > zMin)
				zMin = camPt.z;
		}
		pawnZSortedList.emplace_back(zMin, i);
	}
	std::sort(pawnZSortedList.begin(), pawnZSortedList.end()); // sort pawns from back to front
	reverse(pawnZSortedList.begin(), pawnZSortedList.end()); // reverse the order so the order is front to back

	//for (unsigned int i=0;i<pawnZSortedList.size();i++)
	//	SmartBody::util::log("Pawn %d, zMin = %f", pawnZSortedList[i].second, pawnZSortedList[i].first);

	// occlusion window width/height
	int width = 256, height = 256;
	// create render buffer
	GLuint fb, color_rb, depth_rb;
	//RGBA8 RenderBuffer, 24 bit depth RenderBuffer, 256x256
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	//Create and attach a color buffer
	glGenRenderbuffers(1, &color_rb);
	//We must bind color_rb before we call glRenderbufferStorageEXT
	glBindRenderbuffer(GL_RENDERBUFFER, color_rb);
	//The storage format is RGBA8
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
	//Attach color buffer to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_rb);
	//-------------------------
	glGenRenderbuffers(1, &depth_rb);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	//-------------------------
	//Attach depth buffer to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);
	//-------------------------
	//Does the GPU support current FBO configuration?
	/*
    GLenum status;
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status)
    {
		case GL_FRAMEBUFFER_COMPLETE:
            SmartBody::util::log("FrameBufferObject success");
			break;

	   default:
			SmartBody::util::log("FrameBufferObject error");
	}
	*/
	//-------------------------
	// bind FBO for rendering
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//-------------------------
	glViewport(0, 0, width, height);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	// perform occlusion test
	GLuint uiOcclusionQuery;
	glGenQueries(1, &uiOcclusionQuery);
	for (auto& entry : pawnZSortedList) {
		int pawnIdx = entry.second;
		SmartBody::SBPawn* pawn = mScene.getPawn(testPawns[pawnIdx]);
		SrBox pawn_bb;
		pawn_bb = pawn->getBoundingBox();
		//SmartBody::util::log("pawn %s, min = %.3f %.3f %.3f, max = %.3f %.3f %.3f", pawn->getName().c_str(), pawn_bb.a[0], pawn_bb.a[1], pawn_bb.a[2], pawn_bb.b[0], pawn_bb.b[1], pawn_bb.b[2]);
		glBeginQuery(GL_SAMPLES_PASSED, uiOcclusionQuery);
		// Every pixel that passes the depth test now gets added to the result
		glDrawBox(pawn_bb.a, pawn_bb.b);
		glEndQuery(GL_SAMPLES_PASSED);
		// Now get tthe number of pixels passed
		int iSamplesPassed = 0;
		glGetQueryObjectiv(uiOcclusionQuery, GL_QUERY_RESULT, &iSamplesPassed);
		if (iSamplesPassed > 0) // not occluded
		{
			//SmartBody::util::log("pawn %s, visible samples = %d", pawn->getName().c_str(), iSamplesPassed);
			visiblePawns.emplace_back(pawn->getName());
			//glDrawBox(pawn_bb.a, pawn_bb.b);
		}
	}
	glDeleteQueries(1, &uiOcclusionQuery);
	//Bind 0, which means render to back buffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	//Delete resources
	glDeleteRenderbuffersEXT(1, &color_rb);
	glDeleteRenderbuffersEXT(1, &depth_rb);
	//Bind 0, which means render to back buffer, as a result, fb is unbound
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &fb);

	//for (unsigned int i=0;i<visiblePawns.size();i++)
	//	SmartBody::util::log("Visible pawn %d = %s", i, visiblePawns[i].c_str());
#endif
	return visiblePawns;
}


std::vector<std::string> SBRenderScene::frustumTest(const std::vector<std::string>& testPawnNames) {
	std::vector<std::string> visiblePawns;

	SrFrustum frustum;
	frustum.extractFrustum();

	const std::vector<std::string>& pawns = testPawnNames;

	//	Iterates over all pawns, to check which ones are visible from the characters viewport
	for (const auto& pawnIter : pawns) {
		SmartBody::SBPawn* pawn = mScene.getPawn(pawnIter);

		SBGeomObject* geomObject = pawn->getGeomObject();
		auto* box = dynamic_cast<SBGeomBox*>(geomObject);
		SrBox pawn_bb;
		if (box) {
			SBTransform& transform = pawn->getGlobalTransform();
			SrVec min(-box->extent[0] / 2.0f, -box->extent[1] / 2.0f, -box->extent[2] / 2.0f);
			SrVec max(box->extent[0] / 2.0f, box->extent[1] / 2.0f, box->extent[2] / 2.0f);

			SrVec finalMin = transform.localToGlobal(min);
			SrVec finalMax = transform.localToGlobal(max);
			pawn_bb.set(finalMin, finalMax);
		} else {
			pawn_bb = pawn->getBoundingBox();
		}

//		float ax = pawn_bb.a.x;
//		float ay = pawn_bb.a.y;
//		float az = pawn_bb.a.z;
//		float bx = pawn_bb.b.x;
//		float by = pawn_bb.b.y;
//		float bz = pawn_bb.b.z;

		SrVec pointAAB(pawn_bb.a.x, pawn_bb.a.y, pawn_bb.b.z);
		SrVec pointABB(pawn_bb.a.x, pawn_bb.b.y, pawn_bb.b.z);
		SrVec pointBBA(pawn_bb.b.x, pawn_bb.b.y, pawn_bb.a.z);
		SrVec pointBAA(pawn_bb.b.x, pawn_bb.a.y, pawn_bb.a.z);
		SrVec pointABA(pawn_bb.a.x, pawn_bb.b.y, pawn_bb.a.z);
		SrVec pointBAB(pawn_bb.b.x, pawn_bb.a.y, pawn_bb.b.z);

		//	If bounding box visible, adds pawn to list of visible pawns
		SrPnt center = pawn_bb.getCenter();
		if (frustum.pointInFrustum(pawn_bb.a) ||
			frustum.pointInFrustum(pawn_bb.b) ||
			frustum.pointInFrustum(center) ||
			frustum.pointInFrustum(pointAAB) ||
			frustum.pointInFrustum(pointABB) ||
			frustum.pointInFrustum(pointBBA) ||
			frustum.pointInFrustum(pointBAA) ||
			frustum.pointInFrustum(pointABA) ||
			frustum.pointInFrustum(pointBAB))
			visiblePawns.emplace_back(pawn->getName());
	}
	return visiblePawns;
}

//
//	void SBScene::updateConeOfSight()
//
//	Updates the camera location parameters when coneOfSight is enabled. This
//	renders the scene from the characters viewpoint
//
void SBRenderScene::updateConeOfSight() {
	//	Gets the character from which we want to look from
	SmartBody::SBCharacter* character = mScene.getCharacter(_coneOfSight_character);
	character->getSkeleton()->invalidate_global_matrices();
	character->getSkeleton()->update_global_matrices();

	//	Left eye world location
	SkJoint* leftEye = character->getSkeleton()->search_joint("eyeball_left");
	if (!leftEye) {
		SmartBody::util::log("Can't find 'eyeball_left' joint.\n");
		return;
	}
	const SrMat& gmat_leftEye = leftEye->gmat();
	SrVec leftEye_location = SrVec(gmat_leftEye.get(3, 0), gmat_leftEye.get(3, 1), gmat_leftEye.get(3, 2));

	//	Right eye world location
	SkJoint* rightEye = character->getSkeleton()->search_joint("eyeball_right");
	if (!rightEye) {
		SmartBody::util::log("Can't find 'eyeball_right' joint.\n");
		return;
	}
	const SrMat& gmat_rightEye = rightEye->gmat();
	SrVec rightEye_location = SrVec(gmat_rightEye.get(3, 0), gmat_rightEye.get(3, 1), gmat_rightEye.get(3, 2));

	//	Center of the eyse world coordinates
	SrVec center_eyes_location = (leftEye_location + rightEye_location) / 2;

	float scale = 1.f / mScene.getScale();
	float znear = 0.01f * scale;
	float zfar = 100.0f * scale;
	SrCamera* camera = getActiveCamera();
	float eyebeamLength = 100 * character->getHeight() / 175.0f;

	SrVec localAxis_right = rightEye->localGlobalAxis(2) * eyebeamLength;
	SrVec eyes_look_at_right = localAxis_right * rightEye->gmat();

	SrVec localAxis_left = leftEye->localGlobalAxis(2) * eyebeamLength;
	SrVec eyes_look_at_left = localAxis_left * leftEye->gmat();

	SrVec eyes_look_at = (eyes_look_at_right + eyes_look_at_left) / 2;

	//	Sets FROM where the chararacter is looking
	camera->setEye(center_eyes_location.x,
				   center_eyes_location.y,
				   center_eyes_location.z);

	//	Sets where the character looking is AT
	camera->setCenter(eyes_look_at.x,
					  eyes_look_at.y,
					  eyes_look_at.z);

	//	Sets near clip plane
	camera->setNearPlane(znear);

	//	Sets far clip plane
	camera->setFarPlane(zfar);
}


bool SBRenderScene::setCameraConeOfSight(const std::string& characterName) {
	SrCamera* camera = getActiveCamera();
	if (!camera) {
		SmartBody::util::log("No active camera found. Cannot create camera track.");
		return false;
	}
	SbmPawn* pawn = mScene.getPawn(characterName);
	if (!pawn) {
		SmartBody::util::log("Object %s was not found, cannot track.", characterName.c_str());
		return false;
	}

	auto skeleton = pawn->getSkeleton();

	if (!skeleton->search_joint("eyeball_left")) {
		SmartBody::util::log("Can't enable coneOfsight: 'eyeball_left' joint not found.");
		return false;
	}

	if (!skeleton->search_joint("eyeball_right")) {
		SmartBody::util::log("Can't enable coneOfsight: 'eyeball_right' joint not found.");
		return false;
	}

	_coneOfSight = true;
	_coneOfSight_character = characterName;

	return true;
}

bool SBRenderScene::hasConeOfSight() const {
	return _coneOfSight;
}

void SBRenderScene::removeConeOfSight() {
	_coneOfSight = false;
	_coneOfSight_character = "";
}

SrCamera* SBRenderScene::createCamera(const std::string& name) {
// 	if (camera)
// 	{
// 		SmartBody::util::log("A camera with name '%s' already exists.", name.c_str());
// 		return camera;
// 	}
// 	else
	if (mScene.getPawn(name)) {
		SmartBody::util::log("A pawn with name '%s' already exists. Camera will not be created.", name.c_str());
		return nullptr;
	}
	auto camera = new SrCamera(*this);
	camera->setName(name);
	//SBSkeleton* skeleton = new SBSkeleton();
	//camera->setSkeleton(skeleton);
	//SkJoint* joint = skeleton->add_joint(SkJoint::TypeQuat);
	//joint->setName("world_offset");

	_cameras.insert(std::pair<std::string, SrCamera*>(name, camera));

	mScene.insertPawn(camera);

	// if this is the first camera that is created, make it the active camera
	if (_cameras.size() == 1) {
		this->setActiveCamera(camera);
	}

	return camera;
}

void SBRenderScene::removeCamera(SrCamera* camera) {
	auto iter = _cameras.find(camera->getName());
	if (iter == _cameras.end()) {
		SmartBody::util::log("No camera with name '%s' already exists. Camera will not be removed.", camera->getName().c_str());
		return;
	}

	// is this the active camera?
	if (this->getActiveCamera() == camera) {
		setActiveCamera(nullptr);
	}
	_cameras.erase(iter);
	mScene.removePawn(camera->getName());
}

void SBRenderScene::setActiveCamera(SrCamera* camera) {
	if (!camera) {
		_activeCamera = "";
		return;
	}

	_activeCamera = camera->getName();
}

SrCamera* SBRenderScene::getActiveCamera() {
	if (_activeCamera.empty())
		return nullptr;
	auto iter = _cameras.find(_activeCamera);
	if (iter == _cameras.end())
		return nullptr;

	return (*iter).second;
}

SrCamera* SBRenderScene::getCamera(const std::string& name) {

	auto iter = _cameras.find(name);
	if (iter == _cameras.end()) {
		//SmartBody::util::log("No camera with name '%s' found.", name.c_str());
		return nullptr;
	}
	return (*iter).second;
}

void SBRenderScene::SetCameraLocked(bool locked) {
	_isCameraLocked = locked;
}

bool SBRenderScene::IsCameraLocked() {
	return _isCameraLocked;
}

int SBRenderScene::getNumCameras() {
	return _cameras.size();
}

std::vector<std::string> SBRenderScene::getCameraNames() {
	std::vector<std::string> cameraNames;
	for (auto& _camera : _cameras) {
		cameraNames.emplace_back(_camera.first);
	}

	return cameraNames;
}


void SBRenderScene::setCameraTrack(const std::string& characterName, const std::string& jointName) {
	SrCamera* camera = getActiveCamera();
	if (!camera) {
		SmartBody::util::log("No active camera found. Cannot create camera track.");
		return;
	}
	SbmPawn* pawn = mScene.getPawn(characterName);
	if (!pawn) {
		SmartBody::util::log("Object %s was not found, cannot track.", characterName.c_str());
		return;
	}
	if (jointName.empty()) {
		SmartBody::util::log("Need to specify a joint to track.");
		return;
	}

	auto skeleton = pawn->getSkeleton();

	SkJoint* joint = pawn->getSkeleton()->search_joint(jointName.c_str());
	if (!joint) {
		SmartBody::util::log("Could not find joint %s on object %s.", jointName.c_str(), characterName.c_str());
		return;
	}

	joint->skeleton()->update_global_matrices();
	joint->update_gmat();
	const SrMat& jointMat = joint->gmat();
	SrVec jointPos(jointMat[12], jointMat[13], jointMat[14]);
	auto* cameraTrack = new CameraTrack{};
	cameraTrack->joint = joint;
	cameraTrack->jointToCamera = camera->getEye() - jointPos;
	SmartBody::util::log("Vector from joint to target is %f %f %f", cameraTrack->jointToCamera.x, cameraTrack->jointToCamera.y, cameraTrack->jointToCamera.z);
	cameraTrack->targetToCamera = camera->getEye() - camera->getCenter();
	SmartBody::util::log("Vector from target to eye is %f %f %f", cameraTrack->targetToCamera.x, cameraTrack->targetToCamera.y, cameraTrack->targetToCamera.z);
	_cameraTracking.emplace_back(cameraTrack);
	SmartBody::util::log("Object %s will now be tracked at joint %s.", characterName.c_str(), jointName.c_str());
}

void SBRenderScene::removeCameraTrack() {
	if (!_cameraTracking.empty()) {
		for (auto cameraTrack : _cameraTracking) {
			delete cameraTrack;
		}
		_cameraTracking.clear();
		SmartBody::util::log("Removing current tracked object.");
	}
}

bool SBRenderScene::hasCameraTrack() {
	return !_cameraTracking.empty();
}

const SBRenderScene::Renderable* SBRenderScene::getRenderable(const std::string& name) const {
	auto I = mRenderables.find(name);
	if (I != mRenderables.end()) {
		return &I->second;
	}
	return nullptr;
}

SBRenderScene::Renderable* SBRenderScene::getRenderable(const std::string& name) {
	auto I = mRenderables.find(name);
	if (I != mRenderables.end()) {
		return &I->second;
	}
	return nullptr;
}


void SBRenderScene::updateTrackedCameras() {
	for (auto& x : _cameraTracking) {
		// move the camera relative to the joint
		SkJoint* joint = x->joint;
		joint->skeleton()->update_global_matrices();
		joint->update_gmat();
		const SrMat& jointGmat = joint->gmat();
		SrVec jointLoc(jointGmat[12], jointGmat[13], jointGmat[14]);
		SrVec newJointLoc = jointLoc;
		if (fabs(jointGmat[13] - x->yPos) < x->threshold)
			newJointLoc.y = (float) x->yPos;
		SrVec cameraLoc = newJointLoc + x->jointToCamera;
		SrCamera* activeCamera = getActiveCamera();
		activeCamera->setEye(cameraLoc.x, cameraLoc.y, cameraLoc.z);
		SrVec targetLoc = cameraLoc - x->targetToCamera;
		activeCamera->setCenter(targetLoc.x, targetLoc.y, targetLoc.z);
	}
}

SBAPI void SBRenderScene::rescalePartialMeshSkeleton(const std::string& meshName, const std::string& skelName, const std::string& rootJointName, const std::vector<std::string>& skipMeshNames, float scaleRatio, float blendRatio) {
	SmartBody::util::log("Rescale mesh and skeleton");
	auto mesh = _renderAssetManager.getDeformableMesh(meshName);


	auto skel = mScene.getAssetManager()->getSkeleton(skelName);
	if (!mesh || !skel) {
		SmartBody::util::log("Warning, can't find mesh '%s' or skeleton '%s'.", meshName.c_str(), skelName.c_str());
		return;
	}

	SmartBody::SBJoint* rootJoint = skel->getJointByName(rootJointName);
	if (!rootJoint) {
		SmartBody::util::log("Warning, can't find joint '%s'.", rootJointName.c_str());
		return;
	}
	SmartBody::util::log("before build skin vertex buffer");
	mesh->buildSkinnedVertexBuffer();
	SmartBody::util::log("after build skin vertex buffer");

	SmartBody::util::log("before skeleton update global matrices");
	skel->update_global_matrices();
	SrVec rootPos = rootJoint->gmat().get_translation();
	std::vector<std::string> belowJointNames;
	std::string headJointName = "none";
	belowJointNames.emplace_back(rootJointName);
	int rootJointIdx = mesh->boneJointIdxMap[rootJointName];
	int headJointIdx = -1;
	int neckJointIdx = -1;
	SmartBody::util::log("before find head joints");
	for (int i = 0; i < skel->getNumJoints(); i++) {
		SmartBody::SBJoint* joint = skel->getJoint(i);
#if 1
		//if (joint->getParent() == rootJoint) // children of root joint
		{
			std::string jointName = joint->getName();
			std::transform(jointName.begin(), jointName.end(), jointName.begin(), ::tolower);
			if (jointName.find("head") != std::string::npos) {
				SmartBody::util::log("Head Joint = %s", joint->getMappedJointName().c_str());
				headJointName = joint->getMappedJointName();
				headJointIdx = mesh->boneJointIdxMap[headJointName];
			}
		}
#endif

		SrVec jpos = joint->gmat().get_translation();
		if (jpos.y < rootPos.y) // lower than root position
		{
			belowJointNames.emplace_back(joint->getMappedJointName());
		}
	}
	SrVec neckPos = rootJoint->getParent()->gmat().get_translation();
	neckJointIdx = mesh->boneJointIdxMap[rootJoint->getParent()->getMappedJointName()];
	SmartBody::util::log("Head Joint name = %s, idx = %d, Root Joint name = %s, idx = %d", headJointName.c_str(), headJointIdx, rootJointName.c_str(), rootJointIdx);

	float blendThreshold = skel->getBoundingBox().size().y * blendRatio;

	//rootPos = rootPos + SrVec(0, blendThreshold, 0);
	std::map<std::string, bool> skipMeshMap;
	for (const auto & skipMeshName : skipMeshNames)
		skipMeshMap[skipMeshName] = true;

	for (unsigned int i = 0; i < mesh->dMeshStatic_p.size(); i++) {
		SrModel& model = mesh->dMeshStatic_p[i]->shape();
		SrModel& dynModel = mesh->dMeshDynamic_p[i]->shape();
		auto& skinWeight = mesh->skinWeights[i];
		skinWeight->buildSkinWeightBuf();
		std::string modelName = (const char*) model.name;
		if (skipMeshMap.find(modelName) != skipMeshMap.end()) // skip the mesh
			continue;
		//std::map<std::string, std::vector<SrSnModel*> > blendShapeMap;
		for (unsigned int k = 0; k < model.V.size(); k++) {
			SrVec& v = model.V[k];
			SrVec4i gboneIdx;
			SrVec4i& lboneIdx = skinWeight->boneIDs[k];
			for (int b = 0; b < 4; b++) {
				gboneIdx[b] = mesh->boneJointIdxMap[skinWeight->infJointName[lboneIdx[b]]];
			}
			//LOG("old bone idx = %d %d %d %d", lboneIdx[0], lboneIdx[1], lboneIdx[2], lboneIdx[3]);
			//LOG("global bone idx = %d %d %d %d", gboneIdx[0], gboneIdx[1], gboneIdx[2], gboneIdx[3]);
			bool willScale = blendScalePos(v, rootPos, rootJointIdx, headJointIdx, gboneIdx, skinWeight->boneWeights[k], blendThreshold, scaleRatio);
			//bool willScale = blendScalePos2(v, rootPos, neckPos, rootJointIdx, neckJointIdx, gboneIdx, skinWeight->boneWeights[k], blendThreshold, scaleRatio);
			if (!willScale) continue;

			model.V[k] = v; // update the new position
			dynModel.V[k] = v;
			//model.VOrig[k] = v;
		}
		model.computeNormals();
		dynModel.computeNormals();
		//std::string modelName = (const char*)model.name;
		if (mesh->blendShapeMap.find(modelName) != mesh->blendShapeMap.end()) // the shape is associated with blendshape
		{
			auto& blendShapes = mesh->blendShapeMap[modelName];
			for (auto & blendShape : blendShapes) {
				SrModel& faceModel = blendShape->shape();
				for (unsigned int k = 0; k < faceModel.V.size(); k++) {
					SrVec v = faceModel.V[k];
#if 0
					if (v.y > rootPos.y + blendThreshold)
						continue;

					float blendRatio = (v.y - rootPos.y) / (blendThreshold);
					if (blendRatio < 0.f) blendRatio = 0.0;
					float blendScaleRatio = (blendRatio)+(1.0 - blendRatio)*scaleRatio;
					v = rootPos + (v - rootPos)*blendScaleRatio;
#else
					SrVec4i gboneIdx;
					SrVec4i& lboneIdx = skinWeight->boneIDs[k];
					for (int b = 0; b < 4; b++) {
						gboneIdx[b] = mesh->boneJointIdxMap[skinWeight->infJointName[lboneIdx[b]]];
					}
					bool willScale = blendScalePos(v, rootPos, rootJointIdx, headJointIdx, gboneIdx, skinWeight->boneWeights[k], blendThreshold, scaleRatio);
					//bool willScale = blendScalePos2(v, rootPos, neckPos, rootJointIdx, neckJointIdx, gboneIdx, skinWeight->boneWeights[k], blendThreshold, scaleRatio);
					if (!willScale) continue;
#endif

					//v = rootPos + (v - rootPos)*scaleRatio;
					//faceModel.V[k] = v; // update the new position
					faceModel.V[k] = model.V[k];
				}
			}

		}
	}

	// rescale the offset for each joint below root joint
	for (auto& belowJointName : belowJointNames) {
		SmartBody::SBJoint* joint = skel->getJointByMappedName(belowJointName);
		joint->setOffset(joint->getOffset() * scaleRatio);
	}

	skel->invalidate_global_matrices();
	skel->update_global_matrices();
	SrVec newRootPos = rootJoint->gmat().get_translation();
	SrVec rootOffset = rootPos - newRootPos;
	auto* skelRoot = dynamic_cast<SmartBody::SBJoint*>(skel->root());
	skelRoot->setOffset(skelRoot->getOffset() + rootOffset);

	skel->invalidate_global_matrices();
	skel->update_global_matrices();
	skel->updateGlobalMatricesZero();

	// update bind pose matrices
	for (auto& sw : mesh->skinWeights) {
			for (unsigned int k = 0; k < sw->infJointName.size(); k++) {
			// manually add all joint names
			SmartBody::SBJoint* joint = skel->getJointByName(sw->infJointName[k]);

			SrMat gmatZeroInv = joint->gmatZero().rigidInverse();
			sw->bindPoseMat[k] = gmatZeroInv;
		}
	}

	mesh->rebuildVertexBuffer(true);
}

}