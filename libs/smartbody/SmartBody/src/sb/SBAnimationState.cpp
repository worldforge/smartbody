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

#include "SBAnimationState.h"
#include <controllers/MotionAnalysis.h>
#include <sb/SBMotion.h>
#include <sb/SBCharacter.h>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include <sb/SBEvent.h>
#include <sb/SBAssetManager.h>
#include "SBUtilities.h"
#include <set>
#include <boost/algorithm/string.hpp>
#include <controllers/me_ct_param_animation.h>
#include <controllers/me_controller_tree_root.hpp>

#include <sr/sr.h>
#include <sr/sr_lines.h>
#include <sr/sr_quat.h>

namespace SmartBody {

//SBAnimationBlend::SBAnimationBlend() : PABlend(), _isFinalized(false)
//{
//}

SBAnimationBlend::SBAnimationBlend(SBScene& scene, const std::string& name)
: PABlend(name), SBSceneOwned(scene), _isFinalized(false)
{
}

SBAnimationBlend::~SBAnimationBlend() = default;

void SBAnimationBlend::setIncrementWorldOffsetY(bool flag)
{
	incrementWorldOffsetY = flag;
}

//SrSnColorSurf* SBAnimationBlend::createFlatSurface( float depth, unsigned int dimension, SrVec2 topLeft, SrVec2 lowerRight )
//{
//	SrSnColorSurf* surf = new SrSnColorSurf(); surf->ref();
//	SrModel* surf_model = surf->model();
//
//	float xPos, yPos, zPos = depth;
//	const int dim = dimension;
//	const int size = dim * dim;
//	SrVec pnt;
//	float xSize = lowerRight[0] - topLeft[0];
//	float ySize = lowerRight[1] - topLeft[1];
//	SrColor temp_c;
//	SrMaterial mtl;
//
//	SrArray<SrVec> grid_array;
//	// generate vertices for grid edges/surf
//	for (int i=0; i<dim; i++)
//	{
//		yPos = topLeft[1] + float(ySize/dim*i);
//		for (int j=0; j<dim; j++)
//		{
//			xPos = topLeft[0] + float(xSize/dim*j);
//			pnt = SrVec(xPos, yPos, zPos);
//			grid_array.push(pnt);
//		}
//	}
//
//	// build surf
//	surf_model->init();
//	surf_model->culling = false; // !!! back-face culling can be enabled/disabled here !!!
//	for(int i=0; i<dim; i++)
//	{
//		//theta = -(float)(SR_PI/9) + (float)(SR_PI*4/3/ dim * i);
//		yPos = topLeft[1] + float(ySize/dim*i);
//		for(int j=0; j<dim; j++)
//		{
//			//phi = (float)(SR_PI/3.5) + (float)(SR_PI/2 / dim * j);
//			xPos = topLeft[0] + float(xSize/dim*j);
//			//pnt = SrVec( float(ctr.x+r*sin(phi)*cos(theta)),float(ctr.y+r*cos(phi)-j/2.2), ctr.z-r*sin(phi)*sin(theta) );
//			pnt = SrVec( xPos, yPos, zPos );
//
//			//surf_model->V.push(pnt); // set sphere as surf
//			SrVec tempV; tempV.set(grid_array[i*dim + j]);
//			surf_model->V.emplace_back(tempV);
//			//surf_model->V.push().set(grid_array[i*dim + j]);
//			VecOfInt adjs;
//			for (int x=-1;x<=1;x++) // get adjacent vertices
//			{
//				for (int y=-1;y<=1;y++)
//				{
//					if (x==0 && y==0) continue;
//					int nx = (i+x); int ny = (j+y);
//					if (nx < 0 || nx >= dim || ny < 0 || ny >= dim) continue;
//					int adjIdx = nx*dim + (ny);
//					adjs.emplace_back(adjIdx);
//				}
//			}
//			surf->vtxAdjList.emplace_back(adjs);
//			temp_c = SrColor::interphue((float)i / dim);
//			mtl.diffuse.set(temp_c.r, temp_c.g, temp_c.b, (srbyte)255);
//			surf_model->M.emplace_back(mtl);
//		}
//	}
//
//	// make faces out of vertex
//	for (int i=0; i<dim-1; i++)
//	{
//		for (int j=0; j<dim-1; j++)
//		{
//			//surf_model->F.push().set( i*dim+j, i*dim+j+1, (i+1)*dim+j+1 );
//			//surf_model->F.push().set( i*dim+j, (i+1)*dim+j+1, (i+1)*dim+j );
//			surf_model->F.emplace_back(SrVec3i(i*dim+j, i*dim+j+1, (i+1)*dim+j+1 ));
//			surf_model->F.emplace_back(SrVec3i(i*dim+j, (i+1)*dim+j+1, (i+1)*dim+j ));
//
//		}
//	}
//	surf->changed(true); // force update surf
//	//surf->render_mode(srRenderMode::srRenderModeDefault);
//	//surf->render_mode(srRenderMode::srRenderModeFlat);
//	return surf;
//}

void SBAnimationBlend::backupMotionKey()
{
	for (auto & backupKey : backupKeys)
	{
		backupKey.clear();
	}
	backupKeys.clear();

	for (size_t i = 0; i < keys.size(); ++i)
	{
		backupKeys.emplace_back(std::vector<double>());
		for (size_t j = 0; j < keys[i].size(); ++j)
		{
			backupKeys[i].emplace_back(keys[i][j]);
		}
	}
}
//
//SrSnColorSurf* SBAnimationBlend::createCurveSurface( float radius, unsigned int dimension, SrVec center, SrVec2 phiRange, SrVec2 thetaRange )
//{
//	SrSnColorSurf* surf = new SrSnColorSurf(); surf->ref(); // color surf
//	SrModel* surf_model = surf->model();
//
//	float phi, theta, r = radius;
//	const int dim = dimension;
//	const int size = dim * dim;
//	const SrVec ctr = center;
//	SrVec pnt;
//
//	SrColor temp_c;
//	SrMaterial mtl;
//
//	SrArray<SrVec> grid_array;
//
//	// generate vertices for grid edges/surf
//	for (int i=0; i<dim; i++)
//	{
//		//theta = -(float)(SR_PI/9) + (float)(SR_PI*4/3 / dim * i);
//		theta = thetaRange[0] + float(thetaRange[1]/dim*i);
//		for (int j=0; j<dim; j++)
//		{
//			//phi = (float)(SR_PI/3.5) + (float)(SR_PI/2 / dim * j);
//			phi = phiRange[0] + float(phiRange[1]/dim*j);
//
//			//pnt = SrVec( float(ctr.x+r*sin(phi)*cos(theta)), float(ctr.y+r*cos(phi)-j/2.2), float(ctr.z-r*sin(phi)*sin(theta)) );
//			pnt = SrVec( float(ctr.x+r*sin(phi)*sin(theta)), float(ctr.y+r*cos(phi)), float(ctr.z+r*sin(phi)*cos(theta)) );
//			grid_array.push(pnt);
//		}
//	}
//
//	// build surf
//	surf_model->init();
//	surf_model->culling = false; // !!! back-face culling can be enabled/disabled here !!!
//	for(int i=0; i<dim; i++)
//	{
//		//theta = -(float)(SR_PI/9) + (float)(SR_PI*4/3/ dim * i);
//		theta = thetaRange[0] + float(thetaRange[1]/dim*i) ;
//		for(int j=0; j<dim; j++)
//		{
//			//phi = (float)(SR_PI/3.5) + (float)(SR_PI/2 / dim * j);
//			phi = phiRange[0] + float(phiRange[1]/dim*j);
//			//pnt = SrVec( float(ctr.x+r*sin(phi)*cos(theta)),float(ctr.y+r*cos(phi)-j/2.2), ctr.z-r*sin(phi)*sin(theta) );
//			pnt = SrVec( float(ctr.x+r*sin(phi)*sin(theta)), float(ctr.y+r*cos(phi)), float(ctr.z+r*sin(phi)*cos(theta)) );
//
//			//surf_model->V.push(pnt); // set sphere as surf
//			//surf_model->V.push().set(grid_array[i*dim + j]);
//			SrVec tempV; tempV.set(grid_array[i*dim + j]);
//			surf_model->V.emplace_back(tempV);
//			VecOfInt adjs;
//			for (int x=-1;x<=1;x++) // get adjacent vertices
//			{
//				for (int y=-1;y<=1;y++)
//				{
//					if (x==0 && y==0) continue;
//					int nx = (i+x); int ny = (j+y);
//					if (nx < 0 || nx >= dim || ny < 0 || ny >= dim) continue;
//					int adjIdx = nx*dim + (ny);
//					adjs.emplace_back(adjIdx);
//				}
//			}
//			surf->vtxAdjList.emplace_back(adjs);
//
//			temp_c = SrColor::interphue((float)i / dim);
//			mtl.diffuse.set(temp_c.r, temp_c.g, temp_c.b, (srbyte)255);
//			surf_model->M.emplace_back(mtl);
//		}
//	}
//
//	// make faces out of vertex
//	for (int i=0; i<dim-1; i++)
//	{
//		for (int j=0; j<dim-1; j++)
//		{
//			//surf_model->F.push().set( i*dim+j, i*dim+j+1, (i+1)*dim+j+1 );
//			//surf_model->F.push().set( i*dim+j, (i+1)*dim+j+1, (i+1)*dim+j );
//
//			surf_model->F.emplace_back(SrVec3i( i*dim+j, i*dim+j+1, (i+1)*dim+j+1 ));
//			surf_model->F.emplace_back(SrVec3i( i*dim+j, (i+1)*dim+j+1, (i+1)*dim+j  ));
//		}
//	}
//	surf->changed(true); // force update surf
//	//surf->render_mode(srRenderMode::srRenderModeDefault);
//	//surf->render_mode(srRenderMode::srRenderModeFlat);
//	return surf;
//}
//
//void SBAnimationBlend::createErrorSurfaces( const std::string& type, SrVec center, int segments, int dimensions, std::vector<SrSnColorSurf*>& surfList )
//{
//	if (type == "curve")
//	{
//		float minDist = 1e30f, maxDist = -1e30f;
//		float minPhi = 1e30f, maxPhi = -1e30f;
//		float minTheta = 1e30f, maxTheta = -1e30f;
//		float toDegree = 180.f/(float)SR_PI;
//		for (unsigned int i=0;i<parameters.size();i++)
//		{
//			SrVec offset = parameters[i] - center;
//			//SmartBody::util::log("parameter = %f %f %f",parameters[i][0],parameters[i][1],parameters[i][2]);
//			//offset.y = 0.f; // ignore y distance
//			float curDist = offset.norm();
//			if (curDist < minDist)
//				minDist = curDist;
//			if (curDist > maxDist)
//				maxDist = curDist;
//			SrVec offsetDir = parameters[i] - center;
//			offsetDir.normalize();
//			float cosValue = offsetDir.y;
//			float tanValue = offsetDir.x/offsetDir.z;
//			//float phi = acosf(offsetDir.y);//asinf(sqrtf(1.f-offsetDir.y*offsetDir.y));
//			float phi = atan2f(sqrtf(offsetDir.x*offsetDir.x+offsetDir.z*offsetDir.z),offsetDir.y);
//			//if (offsetDir.y < 0) phi = -phi;
//			float theta = atan2f(offsetDir.x, offsetDir.z);//SR_PI - atan2f(offsetDir.x,offsetDir.z);
//			if (theta > (float)SR_PI*3.f/4.f) theta = theta - (float)SR_PI*2;
//			//float theta = atan2f(offsetDir.z,offsetDir.x);
//
//			//SmartBody::util::log("tan = %f, theta = %f, cos = %f, phi = %f",tanValue, theta*toDegree, cosValue, phi*toDegree);
//			if (theta < minTheta) minTheta = theta;
//			if (theta > maxTheta) maxTheta = theta;
//			if (phi < minPhi) minPhi = phi;
//			if (phi > maxPhi) maxPhi = phi;
//		}
//		minDist = (minDist+maxDist)*0.5f; // make sure "near surf" is not too close
//		//SmartBody::util::log("maxPhi = %f, minPhi = %f, maxTheta = %f, minTheta = %f",maxPhi*toDegree, minPhi*toDegree, maxTheta*toDegree, minTheta*toDegree);
//		SrVec2 thetaRange = SrVec2(minTheta, maxTheta-minTheta);
//		SrVec2 phiRange = SrVec2(minPhi, maxPhi-minPhi);
//		if(segments==0 || maxDist-minDist<srtiny) // creat only one surface in the middle
//		{
//			float radius = (minDist+maxDist)*0.5f;
//			SrSnColorSurf* Surf = createCurveSurface(radius, dimensions, center, phiRange, thetaRange);
//			surfList.emplace_back(Surf);
//		}
//		else
//		{
//			float distOffset = (maxDist - minDist)/segments;
//			for (float radius = minDist; radius <= maxDist + distOffset*0.05f; radius+= distOffset)
//			{
//				SrSnColorSurf* Surf = createCurveSurface(radius, dimensions, center, phiRange, thetaRange);
//				surfList.emplace_back(Surf);
//				//updateErrorSurace(Surf, center);
//			}
//		}
//	}
//	else if ( type == "flat")
//	{
//		SrVec bboxMin = SrVec(1e30f, 1e30f, 1e30f);
//		SrVec bboxMax = SrVec(-1e30f, -1e30f, -1e30f);
//		for (unsigned int i=0;i<parameters.size();i++)
//		{
//			SrVec& pos = parameters[i];
//			for (int k=0;k<3;k++)
//			{
//				if (pos[k] < bboxMin[k])
//					bboxMin[k] = pos[k];
//				if (pos[k] > bboxMax[k])
//					bboxMax[k] = pos[k];
//			}
//		}
//		SrVec2 topLeft = SrVec2(bboxMin[0],bboxMin[1]);
//		SrVec2 lowerRight = SrVec2(bboxMax[0],bboxMax[1]);
//		SrVec2 surfSize = lowerRight - topLeft;
//		float surfaceScale = max(surfSize[0],surfSize[1]);
//
//		float minDist = bboxMin[2];
//		float maxDist = bboxMax[2];
//		if (segments == 0)
//		{
//			minDist = (minDist+maxDist)*0.5f;
//		}
//		if(segments==0 || maxDist-minDist<srtiny) // creat only one surface in the middle
//		{
//			float depth = (minDist+maxDist)*0.5f;
//			SrSnColorSurf* Surf = createFlatSurface(depth, dimensions, topLeft, lowerRight);
//			surfList.emplace_back(Surf);
//		}
//		else
//		{
//			float distOffset = (maxDist - minDist)/segments;
//			for (float depth = minDist; depth <= maxDist; depth+= distOffset)
//			{
//				SrSnColorSurf* Surf = createFlatSurface(depth, dimensions, topLeft, lowerRight);
//				surfList.emplace_back(Surf);
//				Surf->surfaceScale = SrVec(surfaceScale/surfSize[0], surfaceScale/surfSize[1], 1.f);
//				//updateErrorSurace(Surf, center);
//			}
//		}
//	}
//}
//
//void SBAnimationBlend::updateSmoothSurface( SrSnColorSurf* surf )
//{
//	SrModel* surfModel = surf->model();
//	std::vector<VecOfDouble> weightList;
//	float maxError = 1e-30f;
//	float totalError = 0.f;
//	for (size_t i=0;i<surfModel->V.size();i++)
//	{
//		SrVec para = surfModel->V[i];
//		std::vector<double> weights;
//		PABlend::getWeightsFromParameters(para[0],para[1],para[2],weights);
//		weightList.emplace_back(weights);
//	}
//
//
//	// compute the smoothness factor based on parameter weights
//	std::vector<float> smoothList;
//	float maxSmooth = 1e-30f;
//	float totalSmooth = 0.f;
//	for (size_t i=0;i<surfModel->V.size();i++)
//	{
//		const VecOfInt& adjIdx = surf->vtxAdjList[i];
//		VecOfDouble laplacian = weightList[i];
//		float ratio = 1.f/adjIdx.size();
//		for (unsigned int k=0;k<adjIdx.size();k++)
//		{
//			const VecOfDouble& adjWeight = weightList[adjIdx[k]];
//			for (unsigned int idx=0;idx<laplacian.size();idx++)
//			{
//				laplacian[idx] -= adjWeight[idx]*ratio;
//			}
//		}
//		float smooth = 0.f;
//		for (unsigned int idx=0;idx<laplacian.size();idx++)
//		{
//			smooth += (float)(laplacian[idx]*laplacian[idx]);
//		}
//		smooth = sqrtf(smooth);
//		totalSmooth += smooth;
//		if (smooth > maxSmooth) maxSmooth = smooth;
//		smoothList.emplace_back(smooth);
//	}
//
//	totalSmooth /= smoothList.size();
//	SmartBody::util::log("total avg smooth = %f",totalSmooth);
//	SrColor temp_c;
//	SrMaterial mtl;
//	//surfModel->M.remove(0,smoothList.size());
//	surfModel->M.clear();
//	for (unsigned int i=0;i<smoothList.size();i++)
//	{
//		//surfModel->M.push()
//		float curError = (float)(smoothList[i]/maxSmooth);
//		if (curError > 1.f) curError = 1.f;
//
//		temp_c = SrColor::interphue(curError);
//		mtl.diffuse.set(temp_c.r, temp_c.g, temp_c.b, (srbyte)160);
//		surfModel->M.emplace_back(mtl);
//	}
//	surf->changed(true);
//}
//
//
//void SBAnimationBlend::updateErrorSurace( SrSnColorSurf* errorSurf, SrVec center )
//{
//	SrModel* surfModel = errorSurf->model();
//	std::vector<float> errorList;
//	std::vector<VecOfDouble> weightList;
//	float maxError = 1e-30f;
//	float totalError = 0.f;
//	for (size_t i=0;i<surfModel->V.size();i++)
//	{
//		SrVec para = surfModel->V[i];
//		std::vector<double> weights;
//		PABlend::getWeightsFromParameters(para[0],para[1],para[2],weights);
//		float x,y,z;
//		getParametersFromWeights(x,y,z,weights);
//
//		float paraError = (para - SrVec(x,y,z)).norm();
//		if (paraError > maxError)
//			maxError = paraError;
//		errorList.emplace_back(paraError);
//		weightList.emplace_back(weights);
//		totalError += paraError;
//	}
//
//	totalError /= errorList.size();
//	SmartBody::util::log("total avg error = %f",totalError);
//	SrColor temp_c;
//	SrMaterial mtl;
//	//maxError = 0.4f;
//	//surfModel->M.remove(0,errorList.size());
//	surfModel->M.clear();
//	for (unsigned int i=0;i<errorList.size();i++)
//	{
//		//surfModel->M.push()
//		float curError = (float)(errorList[i]/maxError);
//		if (curError > 1.f) curError = 1.f;
//
//		temp_c = SrColor::interphue(curError);
//		mtl.diffuse.set(temp_c.r, temp_c.g, temp_c.b, (srbyte)160);
//		surfModel->M.emplace_back(mtl);
//	}
//	errorSurf->changed(true);
//}
//
//
//
//
//void SBAnimationBlend::buildVisSurfaces( const std::string& errorType, const std::string& surfaceType, int segments, int dimensions )
//{
//	if (errorType != "error" && errorType != "smooth")
//	{
//		SmartBody::util::log("Warning : errorType must be 'error' or 'smooth'");
//		return;
//	}
//	if (_dimension != "3D")
//	{
//		SmartBody::util::log("Warning : build Vis Surface only works for 3D parameterization state");
//	}
//
//	std::vector<SrSnColorSurf*>& surfaces = (errorType == "error") ? errorSurfaces : smoothSurfaces;
//	//SBSkeleton* sbSkel = SBScene::getScene()->getSkeleton(skeletonName);
//	SrVec center = SrVec(0,0,0);
//	//if (sbSkel && surfaceType == "curve")
//	//	center = sbSkel->getJointByMappedName("base")->gmat().get_translation();
//	createErrorSurfaces(surfaceType, center, segments, dimensions, surfaces);
//	for (unsigned int i=0;i<surfaces.size();i++)
//	{
//		SrSnColorSurf* surf = surfaces[i];
//		if (errorType == "error")
//			updateErrorSurace(surf, center);
//		else if (errorType == "smooth")
//			updateSmoothSurface(surf);
//	}
//}

// Motion Vector Flow visualization, added by David Huang  June 2012
void SBAnimationBlend::createMotionVectorFlow(const std::string& motionName, const std::string& chrName,
											   float plotThreshold, unsigned int slidWinHalfSize, bool clearAll)
{
	SkMotion* mo = SBAnimationBlend::getSkMotion(motionName);
	if(mo==nullptr) return;
	if(mo->frames()<3)
	{
		SmartBody::util::log("createMotionVectorFlow(): motion does not have enough frames, aborting...");
		return;
	}
	boost::intrusive_ptr<SkSkeleton> sk = mo->connected_skeleton();
	if(sk==nullptr)
	{
		SBCharacter* sbSk = _scene.getCharacter(chrName);
		if(sbSk)
		{
			std::string sbName = sbSk->getSkeleton()->getName();
			sk = _scene.getSkeleton(sbName);
		}
		if(sk)
		{
			SmartBody::util::log("%s connected to %s for plotting.", motionName.c_str(), chrName.c_str());
			mo->connect(sk.get());
		}
		else
		{
			SmartBody::util::log("createMotionVectorFlow(): motion not connected to any skeleton, aborting...");
			return;
		}
	}

	SR_CLIP(plotThreshold, 0.0f, 1.0f);

	if(clearAll)
		clearMotionVectorFlow(); // clear vector flow SrSnLines

	auto& jnts = sk->joints();
	createJointExclusionArray(jnts); // making a list of excluded joints
	const int jsize = jnts.size();
	const int frames = mo->frames();
	const float dur = mo->duration();
	const float intv = dur / (frames-1);
	std::vector<std::vector<SrVec>> pnts_arr; // jsize X frames
	for(int i=0; i<frames; i++)
	{
		pnts_arr.emplace_back(std::vector<SrVec>());
		// mo->apply_frame(i); // use original frames
		mo->apply(intv * i); // resample motion time uniformly
		sk->invalidate_global_matrices();
		sk->update_global_matrices();
		getJointsGPosFromSkel(sk.get(), pnts_arr.back(), jnts);
	}

	// SmartBody::util::log("createMotionVectorFlow(): max vector norm = %f \n", getVectorMaxNorm(pnts_arr));

	// compute all vector norms
	std::vector<std::vector<float>> norm_arr; // jsize X (frames-1)
	for(int i=0; i<frames-1; i++)
	{
		norm_arr.emplace_back(std::vector<float>());
		auto& n_arr = norm_arr.back();
		n_arr.resize(jsize);
		for(int k=0; k<jsize; k++)
		{
			SkJoint* j = jnts[k].get(); if(isExcluded(j)) continue;
			SrVec curr_p = pnts_arr[i][k];
			SrVec next_p = pnts_arr[i+1][k];
			n_arr[k] = dist(curr_p, next_p);
		}
	}

	// compute norm threshold with sliding window
	std::vector<std::vector<float>> norm_th_arr; // jsize X (frames-1)
	for(int i=0; i<frames; i++)
	{
		norm_arr.emplace_back(std::vector<float>());
		auto& n_arr = norm_arr.back();
		n_arr.resize(jsize);
		float th;
		for(int k=0; k<jsize; k++)
		{
			SkJoint* joint = jnts[k].get(); if(isExcluded(joint)) continue;
			th = 0.0f;
			for(unsigned int j=i-slidWinHalfSize; j<=i+slidWinHalfSize; j++)
			{
				int m = j;
				SR_CLIP(m, 0, frames-2);
				th = th + norm_arr[m][k];
			}
			th = th / (2*slidWinHalfSize+1);
			n_arr[k] = th;
		}
		norm_th_arr.emplace_back(n_arr);
	}

	// plot vector flow
	for(int i=1; i<mo->frames()-1; i++)
	{
		SrSnLines* l = new SrSnLines;
		l->resolution(VFLOW_LINE_WIDTH); // change vector flow lines thickness
		vecflowLinesArray.emplace_back(l);
		SrLines& line = l->shape();
		line.init();
		for(int k=0; k<jsize; k++)
		{
			SkJoint* j = jnts[k].get(); if(isExcluded(j)) continue;
			const SrVec& curr_p = pnts_arr[i][k];
			const SrVec& next_p = pnts_arr[i+1][k];
			const float& norm = norm_arr[i][k];
			const float& th = norm_th_arr[i][k];
			float c = norm / th;
			if(c > 1.0f+plotThreshold)
			{
				c = c - (1.0f+plotThreshold);
				c = c + 0.5f; // use 0.5~1 hue space
				SR_CLIP(c, 0.0f, 1.0f);
				line.push_color(SrColor::interphue(c));
				line.push_line(curr_p, next_p);
			}
			else if(c < 1.0f-plotThreshold)
			{
				c = c - (1.0f-plotThreshold);
				c = c - 0.5f; // use 0~0.5 hue space
				SR_CLIP(c, 0.0f, 1.0f);
				line.push_color(SrColor::interphue(c));
				line.push_line(curr_p, next_p);
			}
			else
			{
				line.push_color(SrColor::lightgray);
				line.push_line(curr_p, next_p);
			}
		}
	}
}

void SBAnimationBlend::clearMotionVectorFlow()
{
	vecflowLinesArray.clear();
}

void SBAnimationBlend::plotMotion(const std::string& motionName, const std::string& chrName, unsigned int interval,
								   bool clearAll, bool useRandomColor)
{
	SkMotion* mo = SBAnimationBlend::getSkMotion(motionName);
	if(mo==nullptr) return;
	if(mo->frames()<3)
	{
		SmartBody::util::log("plotMotion(): motion does not have enough frames, aborting...");
		return;
	}
	boost::intrusive_ptr<SkSkeleton> sk = mo->connected_skeleton();
	if(sk==nullptr)
	{
		SBCharacter* sbSk = _scene.getCharacter(chrName);
		if(sbSk)
		{
			std::string sbName = sbSk->getSkeleton()->getName();
			sk = _scene.getSkeleton(sbName);
		}
		if(sk)
		{
			SmartBody::util::log("%s connected to %s for plotting.", motionName.c_str(), chrName.c_str());
			mo->connect(sk.get());
		}
		else
		{
			SmartBody::util::log("plotMotion(): motion not connected to any skeleton, aborting...");
			return;
		}
	}

	if(interval < 2) interval = 2;

	// clear plot motion SrSnLines
	if(clearAll)
		clearPlotMotion();


	const SrColor color_begin = SrColor::blue;
	const SrColor color_end = SrColor::red;
	SrColor color_random;
	if(useRandomColor)
	{
		static float hue = 0.0f;
		color_random = SrColor::interphue(hue);
		hue += 0.1f;
		if(hue>1.0f) hue = 0.0f;
	}
	auto& jnts = sk->joints();
	createJointExclusionArray(jnts); // making a list of excluded joints
	float mo_dur = mo->duration();
	for(unsigned int i=0; i<=interval; i++)
	{
		mo->apply(mo_dur/interval * i);
		sk->invalidate_global_matrices();
		sk->update_global_matrices();

		auto* l = new SrSnLines;
		plotMotionLinesArray.emplace_back(l);
		SrLines& line = l->shape();

		line.init();
		if(useRandomColor) line.push_color(color_random);
		else line.push_color(lerp(color_begin, color_end, (float)i/interval));
		for(const auto & jnt : jnts) // set k=1 to skip ref root
		{
			SkJoint* j = jnt.get();
			if(isExcluded(j)) continue;
			for(int m=0; m<j->num_children();m++)
			{
				if(isExcluded(j->child(m))) continue;
				line.push_line(j->gcenter(), j->child(m)->gcenter());
			}
		}
	}
}

void SBAnimationBlend::plotMotionFrameTime(const std::string& motionName, const std::string& chrName,
										   float time, bool useRandomColor)
{
	SkMotion* mo = SBAnimationBlend::getSkMotion(motionName);
	if(mo==nullptr) return;
	if(mo->frames()<3)
	{
		SmartBody::util::log("plotMotionFrame(): motion does not have enough frames, aborting...");
		return;
	}
	boost::intrusive_ptr<SkSkeleton> sk = mo->connected_skeleton();
	if(sk==nullptr)
	{
		SBCharacter* sbSk = _scene.getCharacter(chrName);
		if(sbSk)
		{
			std::string sbName = sbSk->getSkeleton()->getName();
			sk = _scene.getSkeleton(sbName);
		}
		if(sk)
		{
			SmartBody::util::log("%s connected to %s for plotting.", motionName.c_str(), chrName.c_str());
			mo->connect(sk.get());
		}
		else
		{
			SmartBody::util::log("plotMotionFrame(): motion not connected to any skeleton, aborting...");
			return;
		}
	}

	auto& jnts = sk->joints();
	createJointExclusionArray(jnts); // making a list of excluded joints

	mo->apply(time);
	//sk->invalidate_global_matrices();
	sk->update_global_matrices();

	SrSnLines* l = new SrSnLines;
	plotMotionLinesArray.emplace_back(l);
	SrLines& line = l->shape();
	line.init();
	if(useRandomColor)
	{
		static float hue = 0.0f;
		line.push_color(SrColor::interphue(hue));
		hue += 0.1f;
		if(hue>1.0f) hue = 0.0f;
	}
	else
		line.push_color(SrColor::lightgray);
	for(auto& joint : jnts) // set k=1 to skip ref root
	{
		auto j = joint.get();
			if(isExcluded(j)) continue;
		for(int m=0; m<j->num_children();m++)
		{
			if(isExcluded(j->child(m))) continue;
			line.push_line(j->gcenter(), j->child(m)->gcenter());
		}
	}
}

void SBAnimationBlend::plotMotionJointTrajectory(const std::string& motionName, const std::string& chrName,
												 const std::string& jointName, float start_t, float end_t, bool useRandomColor)
{

	SkMotion* mo = SBAnimationBlend::getSkMotion(motionName);
	if(mo==0) return;
	if(mo->frames()<3)
	{
		SmartBody::util::log("plotMotionJointTrajectory(): motion does not have enough frames, aborting...");
		return;
	}
	boost::intrusive_ptr<SkSkeleton> sk = mo->connected_skeleton();
	if(sk==0)
	{
		
		SBCharacter* sbSk = _scene.getCharacter(chrName);
		if(sbSk)
		{
			std::string sbName = sbSk->getSkeleton()->getName();
			sk = _scene.getSkeleton(sbName);
		}
		if(sk)
		{
			SmartBody::util::log("%s connected to %s for plotting.", motionName.c_str(), chrName.c_str());
			mo->connect(sk.get());
		}
		else
		{
			SmartBody::util::log("plotMotionJointTrajectory(): motion not connected to any skeleton, aborting...");
			return;
		}
	}
	SkJoint* jnt = sk->search_joint(jointName.c_str());
	if(!jnt)
	{
		SmartBody::util::log("plotMotionJointTrajectory(): specified joint is not found in skeleton, aborting...");
		return;
	}
	
	unsigned int frames = mo->frames();
	const float mo_dur = mo->duration();
	if(end_t <= 0.0f) end_t = mo_dur;
	SR_CLIP(start_t, 0.0f, mo_dur);
	SR_CLIP(end_t, 0.0f, mo_dur);
	const SrColor color_begin = SrColor::blue;
	const SrColor color_end = SrColor::red;
	SrColor color_random;
	if(useRandomColor)
	{
		static float hue = 0.0f;
		color_random = SrColor::interphue(hue);
		hue += 0.1f;
		if(hue>1.0f) hue = 0.0f;
	}
	SrSnLines* l = new SrSnLines;
	plotMotionLinesArray.emplace_back(l);
	SrLines& line = l->shape();
	line.init();
	mo->apply(start_t);
	sk->update_global_matrices();
	SrVec last_jnt_pos = jnt->gcenter(); // first frame
	for(float t=start_t; t<end_t; t+=0.016667f)
	{
		if(useRandomColor) line.push_color(color_random);
		else line.push_color(lerp(color_begin, color_end, (t-start_t)/(end_t-start_t))); // begin~end: blue~red
		mo->apply(t);
		sk->update_global_matrices();
		SrVec cur_jnt_pos = jnt->gcenter();
		line.push_line(last_jnt_pos, cur_jnt_pos);
		last_jnt_pos = cur_jnt_pos;
	}
}


void SBAnimationBlend::clearPlotMotion()
{
	plotMotionLinesArray.clear();
}

void SBAnimationBlend::setChrPlotMotionTransform(const std::string& chrName)
{
		SBCharacter* sbSk = _scene.getCharacter(chrName);
		if(sbSk)
		{
			SrVec hpr = sbSk->getHPR();
			setPlotMotionTransform(sbSk->getPosition(), SR_TORAD(hpr.x));
		}
}

void SBAnimationBlend::setPlotMotionTransform(SrVec offset, float yRot)
{
	SrQuat q(SrVec::j, yRot);
	q.get_mat(plotMotionTransform);
	plotMotionTransform.setl4(offset);
}


void SBAnimationBlend::setChrPlotVectorFlowTransform(const std::string& chrName)
{
		SBCharacter* sbSk = _scene.getCharacter(chrName);
		if(sbSk)
		{
			SrVec hpr = sbSk->getHPR();
			setPlotVectorFlowTransform(sbSk->getPosition(), SR_TORAD(hpr.x));
		}
}

void SBAnimationBlend::setPlotVectorFlowTransform(SrVec offset, float yRot)
{
	SrQuat q(SrVec::j, yRot);
	q.get_mat(plotVectorFlowTransform);
	plotVectorFlowTransform.setl4(offset);
}


void SBAnimationBlend::getJointsGPosFromSkel(SkSkeleton* sk, std::vector<SrVec>& pnts_array,
											  const std::vector<std::unique_ptr<SkJoint>>& jnt_list)
{
	auto size = jnt_list.size();
	pnts_array.resize(size);

	for(size_t k=0; k<size; k++) // set k=1 to skip ref root
	{
		auto& j = jnt_list[k];
		pnts_array[k] = j->gcenter();
	}
}

float SBAnimationBlend::getVectorMaxNorm(SrArray<SrArray<SrVec>*>& pnts_arr)
{
	const int frames = pnts_arr.size();
	const int size = pnts_arr[0]->size();
	float max_err = 0.0f;
	for(int i=1; i<frames; i++)
	{
		for(int k=0; k<size; k++)
		{
			float norm = dist(pnts_arr[i]->get(k), pnts_arr[i-1]->get(k));
			if(norm > max_err)
				max_err = norm;
		}
	}
	return max_err;
}


void SBAnimationBlend::createJointExclusionArray(const std::vector<std::unique_ptr<SkJoint>>& orig_list)
{
	plot_excld_list.resize(0);
	for(auto& joint : orig_list)
	{
		auto j = joint.get();
			SrString jname(j->jointName().c_str());
		if(jname.search("face")>=0) { plot_excld_list.emplace_back(j); continue; }
		if(jname.search("brow")>=0) { plot_excld_list.emplace_back(j); continue; }
		if(jname.search("eye")>=0)  { plot_excld_list.emplace_back(j); continue; }
		if(jname.search("nose")>=0) { plot_excld_list.emplace_back(j); continue; }
		if(jname.search("lid")>=0)  { plot_excld_list.emplace_back(j); continue; }
		if(jname.search("jaw")>=0)  { plot_excld_list.emplace_back(j); continue; }
		if(jname.search("tongue")>=0) { plot_excld_list.emplace_back(j); continue; }
		if(jname.search("lip")>=0)    { plot_excld_list.emplace_back(j); continue; }
		if(jname.search("cheek")>=0)  { plot_excld_list.emplace_back(j); continue; }

		if(jname.search("finger")>=0) { plot_excld_list.emplace_back(j); continue; }
		if(jname.search("thumb")>=0)  { plot_excld_list.emplace_back(j); continue; }
		if(jname.search("index")>=0)  { plot_excld_list.emplace_back(j); continue; }
		if(jname.search("middle")>=0) { plot_excld_list.emplace_back(j); continue; }
		if(jname.search("pinky")>=0)  { plot_excld_list.emplace_back(j); continue; }
		if(jname.search("ring")>=0)   { plot_excld_list.emplace_back(j); continue; }
	}
}

bool SBAnimationBlend::isExcluded(SkJoint* j)
{
	for(auto & i : plot_excld_list)
		if(i == j)
			return true;
	return false;
}


void SBAnimationBlend::addCorrespondencePoints(const std::vector<std::string>& motionNames, const std::vector<double>& points)
{
	if (motions.empty())
	{
		SmartBody::util::log("Add motions before add correspondence points for state");
		return;
	}
	if (motionNames.size() != motions.size())
	{
		SmartBody::util::log("Add correspondence points error, input motion number is not the same with that when adding motions. %d motion names, %d motions.", motionNames.size(), motions.size());
		return;		
	}
	for (size_t i = 0; i < motionNames.size(); i++)
	{
		if (motionNames[i] != motions[i]->getName())
		{
			SmartBody::util::log("Add correspondence points error, input motion names are not in the same order with that when adding motions");
			return;
		}
	}
	if (motionNames.size() != points.size())
	{
		SmartBody::util::log("Add correspondence points error, input motion number is not the same with points number. %d motion names, %d points.", motionNames.size(), points.size());
		return;
	}	
	int num = motionNames.size();

	// find the right place to insert the keys
	int insertPosition = -1;
	if (!keys.empty())
	{
		for (size_t i = 0; i < keys[0].size(); i++)
		{
			if (points[0] <= keys[0][i])
			{
				insertPosition = i;
				break;
			}
		}
		if (insertPosition == -1)
		{
			insertPosition = keys[0].size();
		}
	}

	for (int i = 0; i < num; i++)
	{
		keys[i].insert(keys[i].begin() + insertPosition, points[i]);
	}

	validateCorrespondencePoints();
}

void SBAnimationBlend::setCorrespondencePoints(int motionIndex, int pointIndex, double value)
{
	if (motionIndex < 0 || pointIndex < 0 || (keys.size() == 0) || (pointIndex >= (int) keys[0].size()))
		return;

	keys[motionIndex][pointIndex] = value;
	validateCorrespondencePoints();
}

void SBAnimationBlend::removeCorrespondencePoints(int index)
{
	if (index < 0 || (keys.size() == 0) || (index >= (int) keys[0].size()))
		return;

	for (auto & keyArray : keys)
	{
			keyArray.erase(keyArray.begin() + index);
	}	
}

int SBAnimationBlend::getNumMotions()
{
	return motions.size();
}

std::string SBAnimationBlend::getMotion(int num)
{
	if (motions.size() > (size_t) num && num >= 0)
	{
		return motions[num]->getName();
	}
	else
	{
		return "";
	}
}

SBMotion* SBAnimationBlend::getSBMotion(int num)
{
	if (motions.size() > (size_t) num && num >= 0)
	{
		return motions[num];
	}
	else
	{
		return nullptr;
	}
}

int SBAnimationBlend::getNumCorrespondencePoints()
{
	return getNumKeys();
}

std::vector<double> SBAnimationBlend::getCorrespondencePoints(int num)
{
	if (keys.size() > (size_t) num && num >= 0)
	{
		return keys[num];
	}
	else
	{
		return std::vector<double>();
	}
}

std::string SBAnimationBlend::getDimension()
{
	return _dimension;
}

void SBAnimationBlend::removeMotion(const std::string& motionName)
{
	auto I = std::find_if(motions.begin(), motions.end(), [&](const SmartBody::SBMotion* motion){return motion->getName() == motionName;});

	if (I != motions.end()) {
		motions.erase(I);
	} else {
		SmartBody::util::log("No motion named %s found, cannot remove from state %s.", motionName.c_str(), this->stateName.c_str());
	}
}

bool SBAnimationBlend::addSkMotion(const std::string& motion)
{	
	SBMotion* sbmotion = _scene.getAssetManager()->getMotion(motion);

	if (!sbmotion)
	{
		SmartBody::util::log("SBAnimationBlend add sk motion failure, %s doesn't exist", motion.c_str());
		return false;
	}
	else
		return addMotionRef(sbmotion);	
}

bool SBAnimationBlend::removeSkMotion(const std::string& motionName)
{
	// find the index of the motion
	int index = -1;
	for (int i = 0; i < getNumMotions(); i++)
	{
		SkMotion* m = motions[i];
		if (m->getName() == motionName)
		{
			index = i;
			break;
		}
	}
	if (index < 0)
	{
		SmartBody::util::log("SBAnimationBlend delete motion failure, %s doesn't exist", motionName.c_str());
		return false;
	}

	// first delete corresponding time markers
	removeParameter(motionName);

	// delete the motion and correspondence point
	motions.erase(motions.begin() + index);
	keys.erase(keys.begin() + index);
	return true;
}

/*
	P.S. This is organized way, but is not a efficient way to do it
*/
void SBAnimationBlend::validateCorrespondencePoints()
{
	for (int i = 0; i < getNumMotions(); i++)
	{
		
		SBMotion* motion = motions[i];
		for (int j = 1; j < getNumCorrespondencePoints(); j++)
		{
			if (keys[i][j] < keys[i][j - 1])
			{
				for (int k = j; k < getNumCorrespondencePoints(); k++)
					keys[i][k] += motion->duration();
			}
		}
	}
}

bool SBAnimationBlend::validateState()
{
	if (_isFinalized)
		return true;

	for (int i=0; i < getNumMotions(); i++)
	{
		
		SBMotion* motion = motions[i];
		if ((int)keys.size() < i) // no keys for this state
		{			
			keys.emplace_back(std::vector<double>());
		}		
		std::vector<double>& keyVec = keys[i];
		if (keyVec.empty()) // if no keys for the motion, automatically set up this based on motion duration
		{
			keyVec.emplace_back(0.0);
			keyVec.emplace_back(motion->duration());
		}
	}
	_isFinalized = true;
	return true;
}

void SBAnimationBlend::addEvent(const std::string& motion, double time, const std::string& type, const std::string& parameters, bool onceOnly)
{
	SBMotionEvent motionEvent;
	motionEvent.setIsOnceOnly(onceOnly);
	motionEvent.setTime(time);
	motionEvent.setType(type);
	motionEvent.setParameters(parameters);
	addEventToMotion(motion, motionEvent);
}

void SBAnimationBlend::removeEvent(int index)
{
	if (index < 0 || (int) _events.size() > index)
		return;
}

SBMotionEvent* SBAnimationBlend::getEvent(int index)
{
	if (index < 0 || (int) _events.size() > index)
		return nullptr;

	return &_events[index].first;
}

void SBAnimationBlend::removeAllEvents()
{
	_events.clear();
}

int SBAnimationBlend::getNumEvents()
{
	return _events.size();
}

void SBAnimationBlend::offsetMotion(int index, double time)
{
	if (index < 0 || index >= getNumMotions())
		return;

	SmartBody::SBMotion* sbMotion = motions[index];
	int frameId = int(time * sbMotion->getNumFrames() / sbMotion->getDuration());
	if (!sbMotion->getOffsetParent())
		motions[index] = sbMotion->getOffset("", frameId);
}

void SBAnimationBlend::unoffsetMotions()
{
	for (int i = 0; i < getNumMotions(); ++i)
	{
		SmartBody::SBMotion* sbMotion = motions[i];
		if (sbMotion->getOffsetParent())
			motions[i] = sbMotion->getOffsetParent();
	}
}

std::string SBAnimationBlend::saveToString()
{
	SmartBody::SBAnimationBlend* state = this;
	SmartBody::SBAnimationBlend0D* state0D = dynamic_cast<SmartBody::SBAnimationBlend0D*>(state);
	SmartBody::SBAnimationBlend1D* state1D = dynamic_cast<SmartBody::SBAnimationBlend1D*>(state);
	SmartBody::SBAnimationBlend2D* state2D = dynamic_cast<SmartBody::SBAnimationBlend2D*>(state);
	SmartBody::SBAnimationBlend3D* state3D = dynamic_cast<SmartBody::SBAnimationBlend3D*>(state);

	std::string stateNameVariable = state->stateName + "State";
	boost::replace_all(stateNameVariable,"@","");
	boost::replace_all(stateNameVariable,".","dot");
	std::stringstream strstr;
	strstr << "# blend " << stateName << "\n";
	strstr << "# autogenerated by SmartBody\n";
	strstr << "\n";
	strstr << "blendManager = scene.getBlendManager()\n";
	strstr << "\n";
	strstr << "# align motions first if needed\n";
#if 1
	for (int i = 0; i < state->getNumMotions(); i++)
	{
		SBMotion* motion = dynamic_cast<SBMotion*>(state->motions[i]);
		if (!motion)
			continue;
		int alignIndex = motion->getAlignIndex();
		if (alignIndex > 0)
		{
			strstr << "alignmotion = scene.getMotion(\"" << state->getMotionName(i) << "\")\n";
			strstr << "alignmotion.alignToBegin(" << alignIndex << ")\n";
		}
		else if (alignIndex < 0)
		{
			strstr << "alignmotion = scene.getMotion(\"" << state->getMotionName(i) << "\")\n";
			strstr << "alignmotion.alignToEnd(" << -alignIndex << ")\n";
		}
	}
#endif

	if (state0D || state1D || state2D || state3D)
	{
		strstr << "\n";
		// add the motions
		if (state0D)
		{
			strstr << stateNameVariable << " = blendManager.createBlend0D(\"" << stateName << "\")\n";		
		}
		if (state1D)
		{
			strstr << stateNameVariable << " = blendManager.createBlend1D(\"" << stateName << "\")\n";
		}
		else if (state2D)
		{
			strstr << stateNameVariable << " = blendManager.createBlend2D(\"" << stateName << "\")\n";
		}
		else if (state3D)
		{
			strstr << stateNameVariable << " = blendManager.createBlend3D(\"" << stateName << "\")\n";
		}

		strstr << "\n";
		strstr << stateNameVariable << ".setBlendSkeleton(\"" << state->getBlendSkeleton() << "\")\n";
		strstr << "motions = StringVec()\n";
		for (int x = 0; x < state->getNumMotions(); x++)
		{
			strstr << "motions.append(\"" << state->getMotion(x) << "\")\n";
		}
		// add the parameters
		strstr << "\n";
		if (state1D || state2D || state3D)
		{
			strstr << "paramsX = DoubleVec()\n";
		}
		if (state2D || state3D)
		{
			strstr << "paramsY = DoubleVec()\n";
		}
		if (state3D)
		{
			strstr << "paramsZ = DoubleVec()\n";
		}

		for (int x = 0; x < state->getNumMotions(); x++)
		{
			double p1, p2, p3;
			if (state1D)
			{
				state->getParameter(state->getMotion(x), p1);
				strstr << "paramsX.append(" << p1 << ") # " << state->getMotion(x) << " X\n";
			}
			else if (state2D)
			{
				state->getParameter(state->getMotion(x), p1, p2);
				strstr << "paramsX.append(" << p1 << ") # " << state->getMotion(x) << " X\n";
				strstr << "paramsY.append(" << p2 << ") # " << state->getMotion(x) << " Y\n";
			}
			else if (state3D)
			{
				state->getParameter(state->getMotion(x), p1, p2, p3);
				strstr << "paramsX.append(" << p1 << ") # " << state->getMotion(x) << " X\n";
				strstr << "paramsY.append(" << p2 << ") # " << state->getMotion(x) << " Y\n";
				strstr << "paramsZ.append(" << p3 << ") # " << state->getMotion(x) << " Z\n";
			}

		}
		strstr << "for i in range(0, len(motions)):\n";
		if (state0D)
		{
			strstr << "\t" << stateNameVariable << ".addMotion(motions[i])\n";
		}
		else if (state1D)
		{
			strstr << "\t" << stateNameVariable << ".addMotion(motions[i], paramsX[i])\n";
		}
		else if (state2D)
		{
			strstr << "\t" << stateNameVariable << ".addMotion(motions[i], paramsX[i], paramsY[i])\n";
		}
		else if (state3D)
		{
			strstr << "\t" << stateNameVariable << ".addMotion(motions[i], paramsX[i], paramsY[i], paramsZ[i])\n";
		}
		// add the correspondence points
		strstr << "\n";
		for (int c = 0; c < state->getNumKeys(); c++)
		{
			strstr << "points" << c << " = DoubleVec()\n";
			for (int m = 0; m < state->getNumMotions(); m++)
			{
				strstr << "points" << c << ".append(" << state->keys[m][c] << ") # " << state->getMotion(m) << " " << c << "\n";
			}
			strstr << stateNameVariable << ".addCorrespondencePoints(motions, points" << c << ")\n";
		}

	}
	if (state2D)
	{
		// create the triangles
		strstr << "\n";
		std::vector<TriangleInfo>& triangleInfo = state->getTriangles();
		for (auto & t : triangleInfo)
		{
			strstr << stateNameVariable << ".addTriangle(\"" << t.motion1 << "\", \"" <<  t.motion2 << "\", \"" <<  t.motion3 << "\")\n";
		}
	}
	if (state3D)
	{
		// create the tetrahedrons
		strstr << "\n";
		std::vector<TetrahedronInfo>& tetrahedronInfo = state->getTetrahedrons();
		for (auto & t : tetrahedronInfo)
		{
			strstr << stateNameVariable << ".addTetrahedron(\"" << t.motion1 << "\", \"" <<  t.motion2 << "\", \"" <<  t.motion3 << "\", \"" <<  t.motion4 << "\")\n";
		}
	}
	return strstr.str();
}

SkMotion* SBAnimationBlend::getSkMotion( const std::string& motionName )
{
	for(int i=0; i<getNumMotions(); i++)
	{
		SkMotion* m = motions[i];
		if (m->getName() == motionName)
			return m;
	}
	// not found!
	SmartBody::util::log("Error: SBAnimationBlend::getSkMotion(): %s doesn't exist", motionName.c_str());
	return nullptr;
}

void SBAnimationBlend::addKeyTagValue( const std::string& motionName, int iType, const std::string& tagName, double value )
{
	int motionIdx = getMotionId(motionName);
	if (motionIdx < 0 || motionIdx >= (int)keyTagList.size()) return;
	KeyTagMap& tagMap = keyTagList[motionIdx];
	
	if (tagMap.find(iType) == tagMap.end())
	{
		tagMap[iType] = KeyTag();		
	}
	KeyTag& tags = tagMap[iType];
	if (tags.find(tagName) == tags.end())		
		tags[tagName] = std::vector<double>();		
	tags[tagName].emplace_back(value);
}

KeyTagMap* SBAnimationBlend::getKeyTagMap( const std::string& motionName )
{
	int motionIdx = getMotionId(motionName);
	if (motionIdx < 0 || motionIdx >= (int)keyTagList.size()) return nullptr;
	return &keyTagList[motionIdx];
}

MotionAnalysis* SBAnimationBlend::getMotionAnalysis()
{
	return motionAnalysis.get();
}

void SBAnimationBlend::buildMotionAnalysis(SmartBody::SBScene& scene, const std::string& skeletonName, const std::string& baseName, const std::vector<std::string>& motions, std::string motionPrefix )
{
	motionAnalysis = std::make_unique<MotionAnalysis>(scene);
	motionAnalysis->init(skeletonName,baseName, this, motions, motionPrefix);	
}

void SBAnimationBlend::setBlendSkeleton( std::string skelName )
{
	SmartBody::SBAssetManager* assetManager = _scene.getAssetManager();
	auto skel = assetManager->getSkeleton(skelName);
	if (skel)
	{
		blendSkelName = skelName;
	}
}

std::string SBAnimationBlend::getBlendSkeleton()
{
	return blendSkelName;
}

SBAPI SrBox SBAnimationBlend::getParameterBoundBox()
{
	return SrBox(paraMin, paraMax);
}

SBAPI bool SBAnimationBlend::getVectorWeightsFromParameters( SrVec& para, std::vector<float>& weights )
{
	std::vector<double> doubleWeights;
	if (getParameterDimension() == 1)
	{
		getWeightsFromParameters(para[0],doubleWeights);
	}
	else if (getParameterDimension() == 2)
	{
		getWeightsFromParameters(para[0],para[1],doubleWeights);
	}
	else if (getParameterDimension() == 3)
	{
		getWeightsFromParameters(para[0],para[1],para[2],doubleWeights);
	}
	weights.resize(doubleWeights.size());
	for (unsigned int i=0;i<weights.size();i++)
		weights[i] = (float)doubleWeights[i];
	return true;
}

int SBAnimationBlend::getParameterDimension()
{
	return parameterDim;
}

bool SBAnimationBlend::addMotionRef( SBMotion* sbmotion)
{
	if (sbmotion)
	{
		std::string motionName = sbmotion->getName();
		motions.emplace_back(sbmotion);
		/*		
		// adding two correspondence points when the first motion got inserted
		if (motions.size() == 1)
		{
		std::vector<double> keyVec;
		keyVec.resize(2);
		keyVec[0] = 0.0f;
		keyVec[1] = skMotion->duration();
		keys.emplace_back(keyVec);
		}
		else
		*/
		{
			// add a zero-correspondence point for this new motion
			int numPoints = 0;
			if (keys.size() > 0)
				numPoints  = keys[0].size();
			std::vector<double> keyVec;
			if (numPoints > 0)
			{
				keyVec.resize(numPoints);
				double duration = sbmotion->duration();
				if (numPoints >= 2)
				{
					keyVec[0] = 0.0f;
					keyVec[numPoints - 1] = duration;
				}
				// uniformly space the correspondence points
				double step = duration / double(numPoints);
				for (int i = 1; i < numPoints - 1; i++)
				{
					keyVec[i] = double(i) * step;
				}
			}
			keys.emplace_back(keyVec);
			keyTagList.emplace_back(KeyTagMap());
		}

		getParameters().emplace_back(SrVec());
		return true;
	}
	else
	{			
		return false;
	}
}


SBMotion* SBAnimationBlend::createMotionFromBlend(SrVec parameters, SBCharacter* character, std::string motionName, float fps)
{
	return nullptr;
}

void SBAnimationBlend::getAllChannels(SkChannelArray& channels)
{
	int numMotions = this->getNumMotions();

	for (int m = 0; m < numMotions; m++)
	{
		SBMotion* motion = this->getSBMotion(m);
		SkChannelArray motionChannels = motion->channels();
		channels.merge(motionChannels);
	}
}

SBAnimationBlend0D::SBAnimationBlend0D(SBScene& scene, const std::string& name) : SBAnimationBlend(scene, name)
{
	_dimension = "0D";
	setType(0);
	parameterDim = 1;
}

SBAnimationBlend0D::~SBAnimationBlend0D()
{
}

void SBAnimationBlend0D::addMotion(const std::string& motion)
{
	addSkMotion(motion);	
	setParameter(motion, 1.f); // set a dummy parameter since there is only one motion
}


void SBAnimationBlend0D::removeMotion(const std::string& motion)
{
	SBAnimationBlend::removeMotion(motion);

	// remove correspondence points
	removeSkMotion(motion);
}

SBMotion* SBAnimationBlend0D::createMotionFromBlend(SrVec parameters, SBCharacter* character, std::string motionName, float fps)
{
	return getSBMotion(0);
}

SBAnimationBlend1D::SBAnimationBlend1D(SBScene& scene, const std::string& name) : SBAnimationBlend(scene, name)
{
	_dimension = "1D";
	setType(0);
	parameterDim = 1;
}

SBAnimationBlend1D::~SBAnimationBlend1D()
{
}

void SBAnimationBlend1D::addMotion(const std::string& motion, float parameter)
{
	addSkMotion(motion);

	setParameter(motion, parameter);
}

void SBAnimationBlend1D::addMotionFromRef( SmartBody::SBMotion* motion, float parameter )
{
	addMotionRef(motion);	
	setParameter(motion->getName(), parameter); // set a dummy parameter since there is only one motion
}


void SBAnimationBlend1D::removeMotion(const std::string& motionName)
{
	SBAnimationBlend::removeMotion(motionName);

	// remove correspondnce points
	removeSkMotion(motionName);
}

void SBAnimationBlend1D::setParameter(const std::string& motion, float parameter)
{
	PABlend::setParameter(motion, parameter);
}

SBMotion* SBAnimationBlend1D::createMotionFromBlend(SrVec parameters, SBCharacter* character, std::string motionName, float fps)
{
	// let's start by assuming no keys and a linear retiming from one animation to another
	double amount = parameters[0];

	SBMotion* motion1 = _scene.getMotion(this->getMotion(0));
	SBMotion* motion2 = _scene.getMotion(this->getMotion(1));
	if (!motion1 || !motion2)
		return nullptr;

	double blendedMotionLength = (1.0 - amount) * motion1->getDuration() +  amount * motion2->getDuration();

	auto* motion = new SBMotion(_scene);
	motion->setName(motionName);
	
	SkChannelArray channelArray;
	this->getAllChannels(channelArray);
		
	for (int c = 0; c < channelArray.size(); c++)
	{
		SkChannel& channel = channelArray.get(c);
		std::string channelName = channelArray.name(c);
		std::string channelType = SkChannel::type_name(channel.type);
		motion->addChannel(channelName, channelType);
	}

	// perform a joint map if one exists on motion1 or motion2
	if (motion1->channels()._jointLookupFn) {
		motion->channels()._jointLookupFn = motion1->channels()._jointLookupFn;
	}

	float step = 1.0f / fps;

	SrBuffer<float> buffer(channelArray.count_floats());

	SkChannelArray& channelArray1 = motion1->channels();
	SkChannelArray& channelArray2 = motion2->channels();

	double timeratio1 = motion1->getDuration() / blendedMotionLength;
	double timeratio2 = motion2->getDuration() / blendedMotionLength;

	std::vector<int> motionMap1;
	motionMap1.resize(channelArray1.size());
	int counter = 0;
	for (int c = 0; c < channelArray1.size(); c++)
	{
		motionMap1[c] = counter;
		counter += channelArray1.get(c).size();
	}

	std::vector<int> motionMap2;
	motionMap2.resize(channelArray2.size());
	counter = 0;
	for (int c = 0; c < channelArray2.size(); c++)
	{
		motionMap2[c] = counter;
		counter += channelArray2.get(c).size();
	}

	std::vector<float> frame;
	frame.resize(channelArray.count_floats());
	for (float t = 0.0; t < blendedMotionLength; t += step)
	{
		double localTime1 = t * timeratio1;
		double localTime2 = t * timeratio2;

		int whichFrame1 = (int)(localTime1 / motion1->getFrameRate());
		int whichFrame2 = (int)(localTime2 / motion2->getFrameRate());

		std::vector<float> data1 = motion1->getFrameData(whichFrame1);
		std::vector<float> data2 = motion2->getFrameData(whichFrame2);

		int channelPos = 0;
		// for each channel, blend data into that channel
		for (int c = 0; c < channelArray.size(); c++)
		{
			SkChannel& channel = channelArray.get(c);
			const std::string& name = channelArray.name(c);
			std::string mappedName = name;
			if (motion1->channels()._jointLookupFn) {
				mappedName = motion1->channels()._jointLookupFn(name);
			}

			int pos1 = channelArray1.search(mappedName, channel.type);
			int pos2 = channelArray2.search(mappedName, channel.type);

			int index1 = -1;
			int index2 = -1;
			if (pos1 >= 0)
				index1 = motionMap1[pos1];
			if (pos2 >= 0)
				index2 = motionMap2[pos2];

			std::vector<float> frame1 = motion1->getFrameData(whichFrame1);
			std::vector<float> frame2 = motion2->getFrameData(whichFrame2);

			if (index1 == -1 && index2 == -1)
			{
				if (channel.type == SkChannel::XPos ||
					channel.type == SkChannel::YPos ||
					channel.type == SkChannel::ZPos)
				{
					frame[channelPos] = 0.0;
				}
				else if (channel.type == SkChannel::Quat)
				{
					frame[channelPos] = 1.0;
					frame[channelPos + 1] = 0.0;
					frame[channelPos + 2] = 0.0;
					frame[channelPos + 3] = 0.0;
				}
			}
			else if (index1 == -1 && index2 >= 0)
			{
				if (channel.type == SkChannel::XPos ||
					channel.type == SkChannel::YPos ||
					channel.type == SkChannel::ZPos)
				{
					frame[channelPos] = frame2[index2];
				}
				else if (channel.type == SkChannel::Quat)
				{
					frame[channelPos] = frame2[index2];
					frame[channelPos + 1] = frame2[index2 + 1];
					frame[channelPos + 2] = frame2[index2 + 2];
					frame[channelPos + 3] = frame2[index2 + 3];
				}
			}
			else if (index1 >= 0 && index2 == -1)
			{
				if (channel.type == SkChannel::XPos ||
					channel.type == SkChannel::YPos ||
					channel.type == SkChannel::ZPos)
				{
					frame[channelPos] = frame1[index1];
				}
				else if (channel.type == SkChannel::Quat)
				{
					frame[channelPos] = frame1[index1];
					frame[channelPos + 1] = frame1[index1 + 1];
					frame[channelPos + 2] = frame1[index1 + 2];
					frame[channelPos + 3] = frame1[index1 + 3];
				}
			}
			else
			{
				// common channel, blend the two
				if (channel.type == SkChannel::XPos ||
					channel.type == SkChannel::YPos ||
					channel.type == SkChannel::ZPos)
				{
					frame[channelPos] = frame1[index1] * (1.0 - amount) + amount * frame2[index2];
				}
				else if (channel.type == SkChannel::Quat)
				{
					SrQuat quat1(frame1[index1], frame1[index1 + 1], frame1[index1 + 2], frame1[index1 + 3]);
					SrQuat quat2(frame2[index2], frame2[index2 + 1], frame2[index2 + 2], frame2[index2 + 3]);
					SrQuat finalQuat = slerp(quat1, quat2, amount);
					frame[channelPos] = finalQuat.getData(0);
					frame[channelPos + 1] = finalQuat.getData(1);
					frame[channelPos + 2] = finalQuat.getData(2);
					frame[channelPos + 3] = finalQuat.getData(3);
				}
			}

			channelPos += channel.size();
		}

		motion->addFrame(t, frame);
	}

	// set the sync point times
	double timeStart1 = motion1->getTimeStart();
	double timeStart2 = motion2->getTimeStart();
	motion->setSyncPoint("start", (1.0 - amount) * timeStart1 +  amount * timeStart2);

	double timeReady1 = motion1->getTimeReady();
	double timeReady2 = motion2->getTimeReady();
	motion->setSyncPoint("ready", (1.0 - amount) * timeReady1 + amount * timeReady2);

	double timeStrokeStart1 = motion1->getTimeStrokeStart();
	double timeStrokeStart2 = motion2->getTimeStrokeStart();
	motion->setSyncPoint("stroke_start", (1.0 - amount) * timeStrokeStart1 +  amount * timeStrokeStart2);

	double timeStroke1 = motion1->getTimeStroke();
	double timeStroke2 = motion2->getTimeStroke();
	motion->setSyncPoint("stroke", (1.0 - amount) * timeStroke1 + amount * timeStroke2);

	double timeStrokeEnd1 = motion1->getTimeStrokeEnd();
	double timeStrokeEnd2 = motion2->getTimeStrokeEnd();
	motion->setSyncPoint("stroke_stop", (1.0 - amount) * timeStrokeEnd1 + amount * timeStrokeEnd2);

	double timeRelax1 = motion1->getTimeRelax();
	double timeRelax2 = motion2->getTimeRelax();
	motion->setSyncPoint("relax", (1.0 - amount) * timeRelax1 + amount * timeRelax2);

	double timeStop1 = motion1->getTimeStop();
	double timeStop2 = motion2->getTimeStop();
	motion->setSyncPoint("stop", (1.0 - amount) * timeStop1 + amount * timeStop2);

	// approximate the gesture speeds and locations
	SrVec startGestureLoc1 = motion1->getGestureStartLocation();
	SrVec startGestureLoc2 = motion2->getGestureStartLocation();
	SrVec holdGestureLoc1 = motion1->getGestureHoldLocation();
	SrVec holdGestureLoc2 = motion2->getGestureHoldLocation();
	double speed1 = motion1->getGestureSpeed();
	double speed2 = motion2->getGestureSpeed();
	double finalSpeed = (1.0 - amount) * speed1 + amount * speed2;
	motion->setGestureSpeed(finalSpeed);
	SrVec diffStart = startGestureLoc2 - startGestureLoc1;
	SrVec finalStart = startGestureLoc1 + amount * diffStart;
	motion->setGestureStartLocation(finalStart);

	SrVec diffHold = holdGestureLoc2 - holdGestureLoc1;
	SrVec finalHold = holdGestureLoc1 + amount * diffHold;
	motion->setGestureHoldLocation(finalHold);


	return motion;
}



SBAnimationBlend2D::SBAnimationBlend2D(SBScene& scene, const std::string& name) : SBAnimationBlend(scene, name)
{
	_dimension = "2D";
	setType(1);
	parameterDim = 2;
}

SBAnimationBlend2D::~SBAnimationBlend2D()
{
}

void SBAnimationBlend2D::addMotion(const std::string& motion, float parameter1, float parameter2)
{
	addSkMotion(motion);
	PABlend::setParameter(motion, parameter1, parameter2);
}

void SBAnimationBlend2D::removeMotion(const std::string& motionName)
{
	SBAnimationBlend::removeMotion(motionName);

	// remove correspondence points
	removeSkMotion(motionName);

	// do something about triangle
	removeTriangles(motionName);
}

void SBAnimationBlend2D::setParameter(const std::string& motion, float parameter1, float parameter2)
{
	PABlend::setParameter(motion, parameter1, parameter2);
}

void SBAnimationBlend2D::addTriangle(const std::string& motion1, const std::string& motion2, const std::string& motion3)
{
	PABlend::addTriangle(motion1, motion2, motion3);
}

SBMotion* SBAnimationBlend2D::createMotionFromBlend(SrVec parameters, SBCharacter* character, std::string motionName, float fps)
{
	// fix this, this is incorrect
	return getSBMotion(0);
}

//SBAnimationBlend3D::SBAnimationBlend3D() : SBAnimationBlend("unknown")
//{
//}


SBAnimationBlend3D::SBAnimationBlend3D(SBScene& scene, const std::string& name) : SBAnimationBlend(scene, name)
{
	_dimension = "3D";
	setType(2);
	parameterDim = 3;
}

SBAnimationBlend3D::~SBAnimationBlend3D() = default;


void SBAnimationBlend3D::addMotion(const std::string& motion, float parameter1, float parameter2, float parameter3)
{
	addSkMotion(motion);
	
	setParameter(motion, parameter1, parameter2, parameter3);
}

void SBAnimationBlend3D::removeMotion(const std::string& motionName)
{
	SBAnimationBlend::removeMotion(motionName);

	// remove correspondence points
	removeSkMotion(motionName);

	// do something about tetrahedrons
	removeTetrahedrons(motionName);
}

void SBAnimationBlend3D::setParameter(const std::string& motion, float parameter1, float parameter2, float parameter3)
{
	PABlend::setParameter(motion, parameter1, parameter2, parameter3);
}

void SBAnimationBlend3D::addTetrahedron(const std::string& motion1, const std::string& motion2, const std::string& motion3, const std::string& motion4)
{
	PABlend::addTetrahedron(motion1, motion2, motion3, motion4);
}

SBMotion* SBAnimationBlend3D::createMotionFromBlend(SrVec parameters, SBCharacter* character, std::string motionName, float fps)
{
	// fix this, this is incorrect
	return getSBMotion(0);
}

}