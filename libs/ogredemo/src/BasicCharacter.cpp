/*************************************************************
Copyright (C) 2021 Erik Ogenvik <erik@ogenvik.org>

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

#include "BasicCharacter.h"
#include <sb/SBGestureMapManager.h>
#include <sb/SBGestureMap.h>

#include <sb/SBRetargetManager.h>
#include <sb/SBRetarget.h>

#include <sb/SBAnimationStateManager.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBSkeleton.h>
#include <sb/SBJointMapManager.h>
#include <sb/SBJointMap.h>
#include <sb/SBAssetStore.h>
#include <sb/SBAssetManager.h>

#include <array>
namespace {
void createRetargetInstance(SmartBody::SBScene& scene, const std::string& srcSkelName, const std::string& tgtSkelName) {
	std::vector<std::string> endJoints;
// endJoints.emplace_back("l_ankle");
// endJoints.emplace_back("l_forefoot");
	endJoints.emplace_back("l_toe");
	endJoints.emplace_back("l_wrist");
// endJoints.emplace_back("r_ankle");
	// endJoints.emplace_back("r_forefoot");
	endJoints.emplace_back("r_toe");
	endJoints.emplace_back("r_wrist");

	std::vector<std::string> relativeJoints;
	relativeJoints.emplace_back("JtPelvis");
	relativeJoints.emplace_back("spine1");
	relativeJoints.emplace_back("spine2");
	relativeJoints.emplace_back("spine3");
	relativeJoints.emplace_back("spine4");
	relativeJoints.emplace_back("spine5");
	relativeJoints.emplace_back("r_sternoclavicular");
	relativeJoints.emplace_back("l_sternoclavicular");
	relativeJoints.emplace_back("r_acromioclavicular");
	relativeJoints.emplace_back("l_acromioclavicular");
// replace retarget each animation with just a simple retarget instance
	auto retargetManager = scene.getRetargetManager();
	auto retarget = retargetManager->getRetarget(srcSkelName, tgtSkelName);
	if (!retarget) {
		retarget = retargetManager->createRetarget(srcSkelName, tgtSkelName);
		retarget->initRetarget(*scene.getAssetManager(), endJoints, relativeJoints);
	}
}

void locomotionChrMarineinit(SmartBody::SBScene& scene) {

// locomotion mirror
	if (!scene.getMotion("ChrMarine@WalkCircleLf01")) {
//locomotion smooth cycle
//print("locomotion smooth cycle");
		auto smoothMotion = scene.getMotion("ChrMarine@RunCircleRt01");
		smoothMotion->smoothCycle("ChrMarine@RunCircleRt01_smooth", 0.1);
		smoothMotion = scene.getMotion("ChrMarine@WalkCircleRt01");
		smoothMotion->smoothCycle("ChrMarine@WalkCircleRt01_smooth", 0.1);
		smoothMotion = scene.getMotion("ChrMarine@WalkTightCircleRt01");
		smoothMotion->smoothCycle("ChrMarine@WalkTightCircleRt01_smooth", 0.1);
		smoothMotion = scene.getMotion("ChrMarine@StrafeFastRt01");
		smoothMotion->smoothCycle("ChrMarine@StrafeFastRt01_smooth", 0.1);
		smoothMotion = scene.getMotion("ChrMarine@Turn180Rt01");
		smoothMotion->smoothCycle("ChrMarine@Turn180Rt01_smooth", 0.1);

// locomotion mirror
// print "locomotion mirror"
		auto mirrorMotion = scene.getMotion("ChrMarine@WalkCircleRt01");
		mirrorMotion->mirror("ChrMarine@WalkCircleLf01", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@WalkTightCircleRt01");
		mirrorMotion->mirror("ChrMarine@WalkTightCircleLf01", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@StrafeFastRt01");
		mirrorMotion->mirror("ChrMarine@StrafeFastLf01", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@StrafeSlowRt01");
		mirrorMotion->mirror("ChrMarine@StrafeSlowLf01", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@RunCircleRt01");
		mirrorMotion->mirror("ChrMarine@RunCircleLf01", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@RunTightCircleRt01");
		mirrorMotion->mirror("ChrMarine@RunTightCircleLf01", "ChrBackovic.sk");

// idle turn
// print "idle turn mirror"
		mirrorMotion = scene.getMotion("ChrMarine@Turn90Rt01");
		mirrorMotion->mirror("ChrMarine@Turn90Lf01", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@Turn180Rt01_smooth");
		mirrorMotion->mirror("ChrMarine@Turn180Lf01_smooth", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@Turn360Rt01");
		mirrorMotion->mirror("ChrMarine@Turn360Lf01", "ChrBackovic.sk");

// mirroring for smooth cycle motion
		mirrorMotion = scene.getMotion("ChrMarine@WalkCircleRt01_smooth");
		mirrorMotion->mirror("ChrMarine@WalkCircleLf01_smooth", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@WalkTightCircleRt01_smooth");
		mirrorMotion->mirror("ChrMarine@WalkTightCircleLf01_smooth", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@StrafeFastRt01_smooth");
		mirrorMotion->mirror("ChrMarine@StrafeFastLf01_smooth", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@RunCircleRt01_smooth");
		mirrorMotion->mirror("ChrMarine@RunCircleLf01_smooth", "ChrBackovic.sk");

// starting mirror
		mirrorMotion = scene.getMotion("ChrMarine@Idle01_ToWalk01");
		mirrorMotion->mirror("ChrMarine@Idle01_ToWalkLf01", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@Idle01_ToWalk01_Turn90Rt01");
		mirrorMotion->mirror("ChrMarine@Idle01_ToWalk01_Turn90Lf01", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@Idle01_ToWalk01_Turn180Rt01");
		mirrorMotion->mirror("ChrMarine@Idle01_ToWalk01_Turn180Lf01", "ChrBackovic.sk");

// step mirror
// print "step mirror"
		mirrorMotion = scene.getMotion("ChrMarine@Idle01_StepBackwardsRt01");
		mirrorMotion->mirror("ChrMarine@Idle01_StepBackwardsLf01", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@Idle01_StepForwardRt01");
		mirrorMotion->mirror("ChrMarine@Idle01_StepForwardLf01", "ChrBackovic.sk");
		mirrorMotion = scene.getMotion("ChrMarine@Idle01_StepSidewaysRt01");
		mirrorMotion->mirror("ChrMarine@Idle01_StepSidewaysLf01", "ChrBackovic.sk");
	}
}

void locomotionChrMarinestateLocomotion(SmartBody::SBScene& scene) {
	auto stateManager = scene.getBlendManager();

	auto statemarineLocomotion = stateManager->createBlend3D("mocapLocomotion");
	statemarineLocomotion->setBlendSkeleton("ChrBackovic.sk");
	std::vector<std::string> motions;
	motions.emplace_back("ChrMarine@Idle01");
	motions.emplace_back("ChrMarine@Jog01");
	motions.emplace_back("ChrMarine@Meander01");
	motions.emplace_back("ChrMarine@Run01");
// motions.emplace_back("ChrMarine@RunCircleLf01_smooth");
// motions.emplace_back("ChrMarine@RunCircleRt01_smooth");
	motions.emplace_back("ChrMarine@RunCircleLf01_smooth");
	motions.emplace_back("ChrMarine@RunCircleRt01_smooth");
	motions.emplace_back("ChrMarine@RunTightCircleLf01");
	motions.emplace_back("ChrMarine@RunTightCircleRt01");
	motions.emplace_back("ChrMarine@StrafeSlowLf01");
	motions.emplace_back("ChrMarine@StrafeSlowRt01");
	motions.emplace_back("ChrMarine@Walk01");
// motions.emplace_back("ChrMarine@WalkCircleLf01_smooth");
// motions.emplace_back("ChrMarine@WalkCircleRt01_smooth");
// motions.emplace_back("ChrMarine@WalkTightCircleLf01_smooth");
// motions.emplace_back("ChrMarine@WalkTightCircleRt01_smooth");
	motions.emplace_back("ChrMarine@WalkCircleLf01_smooth");
	motions.emplace_back("ChrMarine@WalkCircleRt01_smooth");
	motions.emplace_back("ChrMarine@WalkTightCircleLf01_smooth");
	motions.emplace_back("ChrMarine@WalkTightCircleRt01_smooth");
	motions.emplace_back("ChrMarine@Turn360Lf01");
	motions.emplace_back("ChrMarine@Turn360Rt01");
	motions.emplace_back("ChrMarine@StrafeFastLf01_smooth");
	motions.emplace_back("ChrMarine@StrafeFastRt01_smooth");

	std::vector<double> paramsX;
	std::vector<double> paramsY;
	std::vector<double> paramsZ;
	paramsX.emplace_back(0);  // ChrMarine@Idle01 X
	paramsY.emplace_back(0);  // ChrMarine@Idle01 Y
	paramsZ.emplace_back(0);  // ChrMarine@Idle01 Z
	paramsX.emplace_back(0);  // ChrMarine@Jog01 X
	paramsY.emplace_back(0);  // ChrMarine@Jog01 Y
	paramsZ.emplace_back(0);  // ChrMarine@Jog01 Z
	paramsX.emplace_back(0);  // ChrMarine@Meander01 X
	paramsY.emplace_back(0);  // ChrMarine@Meander01 Y
	paramsZ.emplace_back(0);  // ChrMarine@Meander01 Z
	paramsX.emplace_back(0);  // ChrMarine@Run01 X
	paramsY.emplace_back(0);  // ChrMarine@Run01 Y
	paramsZ.emplace_back(0);  // ChrMarine@Run01 Z
	paramsX.emplace_back(0);  // ChrMarine@RunCircleLf01_smooth X
	paramsY.emplace_back(0);  // ChrMarine@RunCircleLf01_smooth Y
	paramsZ.emplace_back(0);  // ChrMarine@RunCircleLf01_smooth Z
	paramsX.emplace_back(0);  // ChrMarine@RunCircleRt01_smooth X
	paramsY.emplace_back(0);  // ChrMarine@RunCircleRt01_smooth Y
	paramsZ.emplace_back(0);  // ChrMarine@RunCircleRt01_smooth Z
	paramsX.emplace_back(0);  // ChrMarine@RunTightCircleLf01 X
	paramsY.emplace_back(0);  // ChrMarine@RunTightCircleLf01 Y
	paramsZ.emplace_back(0);  // ChrMarine@RunTightCircleLf01 Z
	paramsX.emplace_back(0);  // ChrMarine@RunTightCircleRt01 X
	paramsY.emplace_back(0);  // ChrMarine@RunTightCircleRt01 Y
	paramsZ.emplace_back(0);  // ChrMarine@RunTightCircleRt01 Z
	paramsX.emplace_back(0);  // ChrMarine@StrafeFastLf01_smooth X
	paramsY.emplace_back(0);  // ChrMarine@StrafeFastLf01_smooth Y
	paramsZ.emplace_back(0);  // ChrMarine@StrafeFastLf01_smooth Z
	paramsX.emplace_back(0);  // ChrMarine@StrafeFastRt01_smooth X
	paramsY.emplace_back(0);  // ChrMarine@StrafeFastRt01_smooth Y
	paramsZ.emplace_back(0);  // ChrMarine@StrafeFastRt01_smooth Z
	paramsX.emplace_back(0);  // ChrMarine@Walk01 X
	paramsY.emplace_back(0);  // ChrMarine@Walk01 Y
	paramsZ.emplace_back(0);  // ChrMarine@Walk01 Z
	paramsX.emplace_back(0);  // ChrMarine@WalkCircleLf01_smooth X
	paramsY.emplace_back(0);  // ChrMarine@WalkCircleLf01_smooth Y
	paramsZ.emplace_back(0);  // ChrMarine@WalkCircleLf01_smooth Z
	paramsX.emplace_back(0);  // ChrMarine@WalkCircleRt01_smooth X
	paramsY.emplace_back(0);  // ChrMarine@WalkCircleRt01_smooth Y
	paramsZ.emplace_back(0);  // ChrMarine@WalkCircleRt01_smooth Z
	paramsX.emplace_back(0);  // ChrMarine@WalkTightCircleLf01_smooth X
	paramsY.emplace_back(0);  // ChrMarine@WalkTightCircleLf01_smooth Y
	paramsZ.emplace_back(0);  // ChrMarine@WalkTightCircleLf01_smooth Z
	paramsX.emplace_back(0);  // ChrMarine@WalkTightCircleRt01_smooth X
	paramsY.emplace_back(0);  // ChrMarine@WalkTightCircleRt01_smooth Y
	paramsZ.emplace_back(0);  // ChrMarine@WalkTightCircleRt01_smooth Z
	paramsX.emplace_back(0);  // ChrMarine@WalkTightCircleLf01_smooth X
	paramsY.emplace_back(0);  // ChrMarine@WalkTightCircleLf01_smooth Y
	paramsZ.emplace_back(0);  // ChrMarine@WalkTightCircleLf01_smooth Z
	paramsX.emplace_back(0);  // ChrMarine@WalkTightCircleRt01_smooth X
	paramsY.emplace_back(0);  // ChrMarine@WalkTightCircleRt01_smooth Y
	paramsZ.emplace_back(0);  // ChrMarine@WalkTightCircleRt01_smooth Z

	paramsX.emplace_back(0);  // ChrMarine@WalkTightCircleLf01_smooth X
	paramsY.emplace_back(0);  // ChrMarine@WalkTightCircleLf01_smooth Y
	paramsZ.emplace_back(0);  // ChrMarine@WalkTightCircleLf01_smooth Z
	paramsX.emplace_back(0);  // ChrMarine@WalkTightCircleRt01_smooth X
	paramsY.emplace_back(0);  // ChrMarine@WalkTightCircleRt01_smooth Y
	paramsZ.emplace_back(0);  // ChrMarine@WalkTightCircleRt01_smooth Z

	for (auto i = 0; i < motions.size(); ++i) {
		statemarineLocomotion->addMotion(motions[i], paramsX[i], paramsY[i], paramsZ[i]);
	}

	std::vector<double> points0;
	points0.emplace_back(0.2);  // ChrMarine@Idle01 0
	points0.emplace_back(0.4);  // ChrMarine@Jog01 0
	points0.emplace_back(0.75);  // ChrMarine@Meander01 0
	points0.emplace_back(0.3);  // ChrMarine@Run01 0
	points0.emplace_back(0.4);  // ChrMarine@RunCircleLf01_smooth 0
	points0.emplace_back(0.1);  // ChrMarine@RunCircleRt01_smooth 0
	points0.emplace_back(0.1);  // ChrMarine@RunTightCircleLf01 0
	points0.emplace_back(0.4);  // ChrMarine@RunTightCircleRt01 0
	points0.emplace_back(0.1);  // ChrMarine@StrafeSlowLf01 0
	points0.emplace_back(0.75);  // ChrMarine@StrafeSlowRt01 0
	points0.emplace_back(0.65);  // ChrMarine@Walk01 0
	points0.emplace_back(0.15);  // ChrMarine@WalkCircleLf01_smooth 0
	points0.emplace_back(0.85);  // ChrMarine@WalkCircleRt01_smooth 0
	points0.emplace_back(0.1);  // ChrMarine@WalkTightCircleLf01_smooth 0
	points0.emplace_back(1.0);  // ChrMarine@WalkTightCircleRt01_smooth 0
	points0.emplace_back(0.7);  // ChrMarine@Turn360Lf01 0
	points0.emplace_back(0.1);  // ChrMarine@Turn360Rt01 0
	points0.emplace_back(0.45);  // ChrMarine@StrafeFastLf01_smooth 0
	points0.emplace_back(0.1);  // ChrMarine@StrafeFastRt01_smooth 0

	statemarineLocomotion->addCorrespondencePoints(motions, points0);
	std::vector<double> points1;
	points1.emplace_back(0.7);  // ChrMarine@Idle01 1
	points1.emplace_back(0.75);  // ChrMarine@Jog01 1
	points1.emplace_back(1.45);  // ChrMarine@Meander01 1
	points1.emplace_back(0.55);  // ChrMarine@Run01 1
	points1.emplace_back(0.85);  // ChrMarine@RunCircleLf01_smooth 1
	points1.emplace_back(0.4);  // ChrMarine@RunCircleRt01_smooth 1
	points1.emplace_back(0.4);  // ChrMarine@RunTightCircleLf01 1
	points1.emplace_back(0.74);  // ChrMarine@RunTightCircleRt01 1
	points1.emplace_back(0.75);  // ChrMarine@StrafeSlowLf01 1
	points1.emplace_back(1.3);  // ChrMarine@StrafeSlowRt01 1
	points1.emplace_back(1.25);  // ChrMarine@Walk01 1
	points1.emplace_back(0.85);  // ChrMarine@WalkCircleLf01_smooth 1
	points1.emplace_back(1.55);  // ChrMarine@WalkCircleRt01_smooth 1
	points1.emplace_back(1.0);  // ChrMarine@WalkTightCircleLf01_smooth 1
	points1.emplace_back(1.6);  // ChrMarine@WalkTightCircleRt01_smooth 1
	points1.emplace_back(1.2);  // ChrMarine@Idle01 1
	points1.emplace_back(0.7);  // ChrMarine@Jog01 1
	points1.emplace_back(0.85);  // ChrMarine@StrafeFastLf01_smooth 1
	points1.emplace_back(0.45);  // ChrMarine@StrafeFastRt01_smooth 1

	statemarineLocomotion->addCorrespondencePoints(motions, points1);
	std::vector<double> points2;
	points2.emplace_back(1.25);  // ChrMarine@Idle01 2
	points2.emplace_back(1.1);  // ChrMarine@Jog01 2
	points2.emplace_back(1.95);  // ChrMarine@Meander01 2
	points2.emplace_back(0.85);  // ChrMarine@Run01 2
	points2.emplace_back(1.25);  // ChrMarine@RunCircleLf01_smooth 2
	points2.emplace_back(0.85);  // ChrMarine@RunCircleRt01_smooth 2
	points2.emplace_back(0.74);  // ChrMarine@RunTightCircleLf01 2
	points2.emplace_back(1.1);  // ChrMarine@RunTightCircleRt01 2
	points2.emplace_back(1.3);  // ChrMarine@StrafeSlowLf01 2
	points2.emplace_back(1.85);  // ChrMarine@StrafeSlowRt01 2
	points2.emplace_back(1.80);  // ChrMarine@Walk01 2
	points2.emplace_back(1.55);  // ChrMarine@WalkCircleLf01_smooth 2
	points2.emplace_back(2.2);  // ChrMarine@WalkCircleRt01_smooth 2
	points2.emplace_back(1.6);  // ChrMarine@WalkTightCircleLf01_smooth 2
	points2.emplace_back(2.45);  // ChrMarine@WalkTightCircleRt01_smooth 2
	points2.emplace_back(1.75);  // ChrMarine@Idle01 2
	points2.emplace_back(1.2);  // ChrMarine@Jog01 2
	points2.emplace_back(1.2);  // ChrMarine@StrafeFastLf01_smooth 2
	points2.emplace_back(0.85);  // ChrMarine@StrafeFastRt01_smooth 2

	statemarineLocomotion->addCorrespondencePoints(motions, points2);
	std::vector<double> points3;
	points3.emplace_back(1.80);  // ChrMarine@Idle01 3
	points3.emplace_back(1.45);  // ChrMarine@Jog01 3
	points3.emplace_back(2.5333);  // ChrMarine@Meander01 3
	points3.emplace_back(1.166);  // ChrMarine@Run01 3
	points3.emplace_back(1.666);  // ChrMarine@RunCircleLf01_smooth 3
	points3.emplace_back(1.25);  // ChrMarine@RunCircleRt01_smooth 3
	points3.emplace_back(1.1);  // ChrMarine@RunTightCircleLf01 3
	points3.emplace_back(1.5);  // ChrMarine@RunTightCircleRt01 3
	points3.emplace_back(1.85);  // ChrMarine@StrafeSlowLf01 3
	points3.emplace_back(2.4);  // ChrMarine@StrafeSlowRt01 3
	points3.emplace_back(2.216);  // ChrMarine@Walk01 3
	points3.emplace_back(2.2);  // ChrMarine@WalkCircleLf01_smooth 3
	points3.emplace_back(2.75);  // ChrMarine@WalkCircleRt01_smooth 3
	points3.emplace_back(2.45);  // ChrMarine@WalkTightCircleLf01_smooth 3
	points3.emplace_back(3.03);  // ChrMarine@WalkTightCircleRt01_smooth 3
	points3.emplace_back(2.23);  // ChrMarine@Idle01 3
	points3.emplace_back(1.75);  // ChrMarine@Jog01 3
	points3.emplace_back(1.70);  // ChrMarine@StrafeFastLf01_smooth 3
	points3.emplace_back(1.20);  // ChrMarine@StrafeFastRt01_smooth 3

	statemarineLocomotion->addCorrespondencePoints(motions, points3);
	std::vector<double> points4;
	points4.emplace_back(2.27);  // ChrMarine@Idle01 4
	points4.emplace_back(1.80);  // ChrMarine@Jog01 4
	points4.emplace_back(3.0833);  // ChrMarine@Meander01 4
	points4.emplace_back(1.366);  // ChrMarine@Run01 4
	points4.emplace_back(1.966);  // ChrMarine@RunCircleLf01_smooth 4
	points4.emplace_back(1.666);  // ChrMarine@RunCircleRt01_smooth 4
	points4.emplace_back(1.5);  // ChrMarine@RunTightCircleLf01 4
	points4.emplace_back(1.8);  // ChrMarine@RunTightCircleRt01 4
	points4.emplace_back(2.4);  // ChrMarine@StrafeFastLf01_smooth 4
	points4.emplace_back(3.05);  // ChrMarine@StrafeFastRt01_smooth 4
	points4.emplace_back(2.71666);  // ChrMarine@Walk01 4
	points4.emplace_back(2.75);  // ChrMarine@WalkCircleLf01_smooth 4
	points4.emplace_back(3.45);  // ChrMarine@WalkCircleRt01_smooth 4
	points4.emplace_back(3.03);  // ChrMarine@WalkTightCircleLf01_smooth 4
	points4.emplace_back(3.93);  // ChrMarine@WalkTightCircleRt01_smooth 4
	points4.emplace_back(2.83);  // ChrMarine@Idle01 4
	points4.emplace_back(2.23);  // ChrMarine@Jog01 4
	points4.emplace_back(2.05);  // ChrMarine@StrafeSlowLf01 4
	points4.emplace_back(1.70);  // ChrMarine@StrafeSlowRt01 4

	statemarineLocomotion->addCorrespondencePoints(motions, points4);

	typedef std::array<double, 4> A;
	std::vector<A> stanceArray;
	stanceArray.emplace_back(A{0.2, 0.7, 1.25, 1.80});  // idle
	stanceArray.emplace_back(A{0.4, 0.05, 1.1, 0.75});  // jog
	stanceArray.emplace_back(A{0.75, 0.2, 1.95, 1.45});  // meander
	stanceArray.emplace_back(A{0.3, 0.1, 0.85, 0.55});  // run
	stanceArray.emplace_back(A{0.4, 0.1, 1.25, 0.85});  // run circle lf
	stanceArray.emplace_back(A{0.1, 0.4, 0.85, 1.25});  // run circle rt
	stanceArray.emplace_back(A{0.1, 0.4, 0.74, 1.10});  // run tight circle lf
	stanceArray.emplace_back(A{0.4, 0.1, 1.10, 0.74});  // run tight circle rt
	stanceArray.emplace_back(A{0.1, 0.75, 1.3, 1.85});  // strafe slow lf
	stanceArray.emplace_back(A{0.75, 0.1, 1.85, 1.3});  // strafe slow rt
	stanceArray.emplace_back(A{0.65, 0.15, 1.80, 1.25});  // walk
	stanceArray.emplace_back(A{0.15, 0.85, 1.55, 2.20});  // walk circle lf
	stanceArray.emplace_back(A{0.85, 0.15, 2.20, 1.55});  // walk circle rt
	stanceArray.emplace_back(A{0.1, 1.0, 1.6, 2.45});  // walk tight circle lf
	stanceArray.emplace_back(A{1.0, 0.1, 2.45, 1.6});  // walk tight circle rt
	stanceArray.emplace_back(A{0.7, 0.1, 1.75, 1.20});  // turn 360 lf
	stanceArray.emplace_back(A{0.1, 0.7, 1.20, 1.75});  // turn 360 rt
	stanceArray.emplace_back(A{0.45, 0.1, 1.20, 0.85});  // strafe fast lf
	stanceArray.emplace_back(A{0.1, 0.45, 0.85, 1.20});  // strafe fast rt

	std::vector<A> liftArray;
	liftArray.emplace_back(A{0.7, 0.2, 1.80, 1.25});  // idle
	liftArray.emplace_back(A{0.55, 0.22, 1.22, 0.92});  // jog
	liftArray.emplace_back(A{0.2, 0.75, 1.45, 1.95});  // meander
	liftArray.emplace_back(A{0.4, 0.2, 0.95, 0.65});  // run
	liftArray.emplace_back(A{0.5, 0.2, 1.35, 0.95});  // run circle lf
	liftArray.emplace_back(A{0.2, 0.5, 0.95, 1.35});  // run circle rt
	liftArray.emplace_back(A{0.2, 0.5, 0.84, 1.20});  // run tight circle lf
	liftArray.emplace_back(A{0.5, 0.2, 1.20, 0.84});  // run tight circle rt
	liftArray.emplace_back(A{0.75, 0.1, 1.85, 1.3});  // strafe slow lf
	liftArray.emplace_back(A{0.1, 0.75, 1.3, 1.85});  // strafe slow rt
	liftArray.emplace_back(A{0.15, 0.65, 1.25, 1.80});  // walk
	liftArray.emplace_back(A{0.85, 0.15, 2.20, 1.55});  // walk circle lf
	liftArray.emplace_back(A{0.15, 0.85, 1.55, 2.20});  // walk circle rt
	liftArray.emplace_back(A{1.0, 0.1, 2.45, 1.6});  // walk tight circle lf
	liftArray.emplace_back(A{0.1, 1.0, 1.6, 2.45});  // walk tight circle rt
	liftArray.emplace_back(A{0.1, 0.7, 1.20, 1.75});  // turn 360 lf
	liftArray.emplace_back(A{0.7, 0.1, 1.75, 1.20});  // turn 360 rt
	liftArray.emplace_back(A{0.1, 0.45, 0.85, 1.20});  // strafe fast lf
	liftArray.emplace_back(A{0.45, 0.1, 1.20, 0.85});  // strafe fast rt

	for (auto i = 0; i < motions.size(); ++i) {
		auto motionName = motions[i];
		auto motion = scene.getMotion(motionName);
		auto motionDuration = motion->getDuration();
		for (auto j : {0, 1}) {

			auto s1 = stanceArray[i][0 + j];
			auto s2 = stanceArray[i][2 + j];
			auto leg = j;
			statemarineLocomotion->addKeyTagValue(motionName, leg, "stance", s1);
			statemarineLocomotion->addKeyTagValue(motionName, leg, "stance", s2);
			auto lift1 = std::fmod(liftArray[i][0 + j], motionDuration);
			auto lift2 = std::fmod(liftArray[i][2 + j], motionDuration);
			statemarineLocomotion->addKeyTagValue(motionName, leg, "lift", lift1);
			statemarineLocomotion->addKeyTagValue(motionName, leg, "lift", lift2);
			auto land1 = std::fmod((s2 - 0.05), motionDuration);
			auto land2 = std::fmod((s1 - 0.05), motionDuration);
			statemarineLocomotion->addKeyTagValue(motionName, leg, "land", land1);
			statemarineLocomotion->addKeyTagValue(motionName, leg, "land", land2);
			auto off1 = std::fmod((lift1 + 0.1), motionDuration);
			auto off2 = std::fmod((lift2 + 0.1), motionDuration);
			statemarineLocomotion->addKeyTagValue(motionName, leg, "lift-off", off1);
			statemarineLocomotion->addKeyTagValue(motionName, leg, "lift-off", off2);
			auto str1 = std::fmod((land1 - 0.15), motionDuration);
			auto str2 = std::fmod((land2 - 0.15), motionDuration);
			statemarineLocomotion->addKeyTagValue(motionName, leg, "strike", str1);
			statemarineLocomotion->addKeyTagValue(motionName, leg, "strike", str2);
			// add motion event, only for walk motion
			if (i == 10 && leg == 0) {
				statemarineLocomotion->addEvent(motionName, s1, "footstep", std::to_string(leg) + "-1", false);
				statemarineLocomotion->addEvent(motionName, s2, "footstep", std::to_string(leg) + "-2", false);
			}
		}
	}
	statemarineLocomotion->buildMotionAnalysis(scene, "ChrBackovic.sk", "base", motions, "");

	auto skeleton = scene.getSkeleton("ChrBackovic.sk");
	auto joint = skeleton->getJointByName("base");
	std::array<double, 19> travelDirection = {0, 0, 0, 0, 0, 0, 0, 0, -90, 90, 0, 0, 0, 0, 0, 0, 0, -90, 90};

	for (auto i = 0; i < motions.size(); ++i) {
		auto motion = scene.getMotion(motions[i]);
		motion->connect(skeleton.get());
//correspondancePoints = statemarineLocomotion->getCorrespondancePoints(i);
//lenCorrespondancePoints = len(correspondancePoints);
// speed = motion.getJointSpeed(joint, correspondancePoints[0], correspondancePoints[lenCorrespondancePoints - 1]);
// omega = motion.getJointAngularSpeed(joint, correspondancePoints[0], correspondancePoints[lenCorrespondancePoints - 1]);
		auto speed = motion->getJointSpeed(joint, 0, motion->getDuration());
// speedZ = motion.getJointSpeedAxis(joint, "Z", 0, motion.getDuration());
// if (travelDirection[i] != 0):
//	direction = motion.getJointSpeedAxis(joint, "X", 0, motion.getDuration());
// else:
		auto direction = travelDirection[i];

		auto omega = motion->getJointAngularSpeed(joint, 0, motion->getDuration());

		statemarineLocomotion->setParameter(motions[i], speed, omega, direction);
		//print("Motion " + motion->getName() + " (" + std::to_string(speed) + ", " + std::to_string(omega) + ", " + str(direction) + ")");
		motion->disconnect();
	}


	statemarineLocomotion->addTetrahedron("ChrMarine@Idle01", "ChrMarine@Meander01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkCircleLf01_smooth", "ChrMarine@WalkTightCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkTightCircleLf01_smooth", "ChrMarine@Turn360Lf01");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkTightCircleLf01_smooth",
										  "ChrMarine@Turn360Lf01");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkCircleLf01_smooth",
										  "ChrMarine@WalkTightCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeSlowRt01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeSlowRt01", "ChrMarine@StrafeFastRt01_smooth",
										  "ChrMarine@WalkCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@Run01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@StrafeFastRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@Run01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@StrafeFastRt01_smooth");

	statemarineLocomotion->addTetrahedron("ChrMarine@Idle01", "ChrMarine@Meander01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkCircleLf01_smooth", "ChrMarine@WalkTightCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkTightCircleLf01_smooth", "ChrMarine@Turn360Lf01");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkTightCircleLf01_smooth",
										  "ChrMarine@Turn360Lf01");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkCircleLf01_smooth",
										  "ChrMarine@WalkTightCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeSlowLf01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeSlowLf01", "ChrMarine@StrafeFastLf01_smooth",
										  "ChrMarine@WalkCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@Run01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@StrafeFastLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@Run01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@StrafeFastLf01_smooth");

	statemarineLocomotion->addTetrahedron("ChrMarine@Idle01", "ChrMarine@Meander01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkCircleRt01_smooth", "ChrMarine@WalkTightCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkTightCircleRt01_smooth", "ChrMarine@Turn360Rt01");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkTightCircleRt01_smooth",
										  "ChrMarine@Turn360Rt01");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkCircleRt01_smooth",
										  "ChrMarine@WalkTightCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeSlowRt01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeSlowRt01", "ChrMarine@StrafeFastRt01_smooth",
										  "ChrMarine@WalkCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@Run01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@StrafeFastRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@Run01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@StrafeFastRt01_smooth");

	statemarineLocomotion->addTetrahedron("ChrMarine@Idle01", "ChrMarine@Meander01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkCircleRt01_smooth", "ChrMarine@WalkTightCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkTightCircleRt01_smooth", "ChrMarine@Turn360Rt01");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkTightCircleRt01_smooth",
										  "ChrMarine@Turn360Rt01");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkCircleRt01_smooth",
										  "ChrMarine@WalkTightCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeSlowLf01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeSlowLf01", "ChrMarine@StrafeFastLf01_smooth",
										  "ChrMarine@WalkCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleRt01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@Jog01", "ChrMarine@Run01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@StrafeFastLf01_smooth");
	statemarineLocomotion->addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@Run01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@StrafeFastLf01_smooth");
}

void locomotionChrMarinestateStartingLeft(SmartBody::SBScene& scene) {
	auto stateManager = scene.getBlendManager();

	auto stateChrMarineStartingLeft = stateManager->createBlend1D("mocapStartingLeft");
	stateChrMarineStartingLeft->setBlendSkeleton("ChrBackovic.sk");
	std::vector<std::string> motions;
	motions.emplace_back("ChrMarine@Idle01_ToWalk01");
	motions.emplace_back("ChrMarine@Idle01_ToWalk01_Turn90Lf01");
	motions.emplace_back("ChrMarine@Idle01_ToWalk01_Turn180Lf01");

	std::vector<double> paramsX;
	paramsX.emplace_back(0);  // ChrMarine@Idle01 X
	paramsX.emplace_back(-90);  // ChrMarine@Idle01_ToWalk01_Turn90Lf01 X
	paramsX.emplace_back(-180);  // ChrMarine@Idle01_ToWalk01_Turn180Lf01 X

	for (auto i = 0; i < motions.size(); ++i) {
		stateChrMarineStartingLeft->addMotion(motions[i], paramsX[i]);
	}

	std::vector<double> points0;
	points0.emplace_back(0);  // ChrBrad_ChrMarine@Idle01 0
	points0.emplace_back(0);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 0
	points0.emplace_back(0);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 0
	stateChrMarineStartingLeft->addCorrespondencePoints(motions, points0);
	std::vector<double> points1;
	points1.emplace_back(0.73);  // ChrBrad_ChrMarine@Idle01 1
	points1.emplace_back(1.42);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 1
	points1.emplace_back(1.37);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 1
	stateChrMarineStartingLeft->addCorrespondencePoints(motions, points1);
	std::vector<double> points2;
	points2.emplace_back(1.32);  // ChrBrad_ChrMarine@Idle01 2
	points2.emplace_back(2.08);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 2
	points2.emplace_back(2.06);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 2
	stateChrMarineStartingLeft->addCorrespondencePoints(motions, points2);
	std::vector<double> points3;
	points3.emplace_back(1.56);  // ChrBrad_ChrMarine@Idle01 2
	points3.emplace_back(2.43);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 2
	points3.emplace_back(2.46);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 2
	stateChrMarineStartingLeft->addCorrespondencePoints(motions, points3);
}

void locomotionChrMarinestateStartingRight(SmartBody::SBScene& scene) {
	auto stateManager = scene.getBlendManager();

	auto stateChrMarineStartingRight = stateManager->createBlend1D("mocapStartingRight");
	stateChrMarineStartingRight->setBlendSkeleton("ChrBackovic.sk");
	std::vector<std::string> motions;
	motions.emplace_back("ChrMarine@Idle01_ToWalkLf01");
	motions.emplace_back("ChrMarine@Idle01_ToWalk01_Turn90Rt01");
	motions.emplace_back("ChrMarine@Idle01_ToWalk01_Turn180Rt01");

	std::vector<double> paramsX;
	paramsX.emplace_back(0);  // ChrMarine@Idle01 X
	paramsX.emplace_back(90);  // ChrMarine@Idle01_ToWalk01_Turn90Rt01 X
	paramsX.emplace_back(165);  // ChrMarine@Idle01_ToWalk01_Turn180Rt01 X
	for (auto i = 0; i < motions.size(); ++i) {
		stateChrMarineStartingRight->addMotion(motions[i], paramsX[i]);
	}

	std::vector<double> points0;
	points0.emplace_back(0);  // ChrBrad_ChrMarine@Idle01 0
	points0.emplace_back(0);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 0
	points0.emplace_back(0);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 0
	stateChrMarineStartingRight->addCorrespondencePoints(motions, points0);
	std::vector<double> points1;
	points1.emplace_back(0.73);  // ChrBrad_ChrMarine@Idle01 1
	points1.emplace_back(1.42);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 1
	points1.emplace_back(1.37);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 1
	stateChrMarineStartingRight->addCorrespondencePoints(motions, points1);
	std::vector<double> points2;
	points2.emplace_back(1.32);  // ChrBrad_ChrMarine@Idle01 2
	points2.emplace_back(2.08);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 2
	points2.emplace_back(2.06);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 2
	stateChrMarineStartingRight->addCorrespondencePoints(motions, points2);
	std::vector<double> points3;
	points3.emplace_back(1.56);  // ChrBrad_ChrMarine@Idle01 2
	points3.emplace_back(2.43);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 2
	points3.emplace_back(2.46);  // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 2
	stateChrMarineStartingRight->addCorrespondencePoints(motions, points3);
}

void locomotionChrMarinestateIdleTurn(SmartBody::SBScene& scene) {
	auto mirrorMotion = scene.getMotion("ChrMarine@Turn90Rt01");
	mirrorMotion->mirror("ChrMarine@Turn90Lf01", "ChrBackovic.sk");
	auto mirrorMotion1 = scene.getMotion("ChrMarine@Turn180Rt01");
	mirrorMotion1->mirror("ChrMarine@Turn180Lf01", "ChrBackovic.sk");

	auto stateManager = scene.getBlendManager();

// print "** State: ChrMarineIdleTurn"
	auto state = stateManager->createBlend1D("mocapIdleTurn");
	state->setBlendSkeleton("ChrBackovic.sk");
	std::vector<std::string> motions;
	motions.emplace_back("ChrMarine@Idle01");
	motions.emplace_back("ChrMarine@Turn90Lf01");
	motions.emplace_back("ChrMarine@Turn180Lf01");
	motions.emplace_back("ChrMarine@Turn90Rt01");
	motions.emplace_back("ChrMarine@Turn180Rt01");
	std::vector<double> params;
	params.emplace_back(0);
	params.emplace_back(-90);
	params.emplace_back(-180);
	params.emplace_back(90);
	params.emplace_back(180);

	for (auto i = 0; i < motions.size(); ++i) {
		state->addMotion(motions[i], params[i]);
	}

	std::vector<double> points1;
	points1.emplace_back(0);
	points1.emplace_back(0);
	points1.emplace_back(0);
	points1.emplace_back(0);
	points1.emplace_back(0);
	state->addCorrespondencePoints(motions, points1);
	std::vector<double> points2;
	points2.emplace_back(0.255738);
	points2.emplace_back(0.762295);
	points2.emplace_back(0.87541);
	points2.emplace_back(0.757377);
	points2.emplace_back(0.821311);
	state->addCorrespondencePoints(motions, points2);
	std::vector<double> points3;
	points3.emplace_back(2.17);
	points3.emplace_back(2.13);
	points3.emplace_back(1.73);
	points3.emplace_back(2.13);
	points3.emplace_back(1.73);
	state->addCorrespondencePoints(motions, points3);
}


void locomotionChrMarinestateStep(SmartBody::SBScene& scene) {
	auto stateManager = scene.getBlendManager();

	auto stateChrMarineStep = stateManager->createBlend2D("mocapStep");
	stateChrMarineStep->setBlendSkeleton("ChrBackovic.sk");
	std::vector<std::string> motions;
	motions.emplace_back("ChrMarine@Idle01");
	motions.emplace_back("ChrMarine@Idle01_StepBackwardsLf01");
	motions.emplace_back("ChrMarine@Idle01_StepBackwardsRt01");
	motions.emplace_back("ChrMarine@Idle01_StepForwardLf01");
	motions.emplace_back("ChrMarine@Idle01_StepForwardRt01");
	motions.emplace_back("ChrMarine@Idle01_StepSidewaysLf01");
	motions.emplace_back("ChrMarine@Idle01_StepSidewaysRt01");

	std::vector<double> paramsX;
	std::vector<double> paramsY;
	paramsX.emplace_back(8.31458e-005);  // ChrMarine@Idle01 X
	paramsY.emplace_back(-6.8546e-005);  // ChrMarine@Idle01 Y
	paramsX.emplace_back(0.0);  // ChrMarine@Idle01_StepBackwardsLf01 X
	paramsY.emplace_back(-0.349858);  // ChrMarine@Idle01_StepBackwardsLf01 Y
	paramsX.emplace_back(0.0);  // ChrMarine@Idle01_StepBackwardsRt01 X
	paramsY.emplace_back(-0.349858);  // ChrMarine@Idle01_StepBackwardsRt01 Y
	paramsX.emplace_back(-0.0225843);  // ChrMarine@Idle01_StepForwardLf01 X
	paramsY.emplace_back(0.535643);  // ChrMarine@Idle01_StepForwardLf01 Y
	paramsX.emplace_back(0.0234374);  // ChrMarine@Idle01_StepForwardRt01 X
	paramsY.emplace_back(0.535643);  // ChrMarine@Idle01_StepForwardRt01 Y
	paramsX.emplace_back(-0.234709);  // ChrMarine@Idle01_StepSidewaysLf01 X
	paramsY.emplace_back(-0.010002);  // ChrMarine@Idle01_StepSidewaysLf01 Y
	paramsX.emplace_back(0.234693);  // ChrMarine@Idle01_StepSidewaysRt01 X
	paramsY.emplace_back(-0.010002);  // ChrMarine@Idle01_StepSidewaysRt01 Y

	for (auto i = 0; i < motions.size(); ++i) {
		stateChrMarineStep->addMotion(motions[i], paramsX[i], paramsY[i]);
	}

	std::vector<double> points0;
	points0.emplace_back(0);  // ChrMarine@Idle01 0
	points0.emplace_back(0);  // ChrMarine@Idle01_StepBackwardsLf01 0
	points0.emplace_back(0);  // ChrMarine@Idle01_StepBackwardsRt01 0
	points0.emplace_back(0);  // ChrMarine@Idle01_StepForwardLf01 0
	points0.emplace_back(0);  // ChrMarine@Idle01_StepForwardRt01 0
	points0.emplace_back(0);  // ChrMarine@Idle01_StepSidewaysLf01 0
	points0.emplace_back(0);  // ChrMarine@Idle01_StepSidewaysRt01 0
	stateChrMarineStep->addCorrespondencePoints(motions, points0);
	std::vector<double> points1;
	points1.emplace_back(1.1153);  // ChrMarine@Idle01 1
	points1.emplace_back(0.68);  // ChrMarine@Idle01_StepBackwardsLf01 1
	points1.emplace_back(0.68);  // ChrMarine@Idle01_StepBackwardsRt01 1
	points1.emplace_back(1.02258);  // ChrMarine@Idle01_StepForwardLf01 1
	points1.emplace_back(1.0303);  // ChrMarine@Idle01_StepForwardRt01 1
	points1.emplace_back(0.821667);  // ChrMarine@Idle01_StepSidewaysLf01 1
	points1.emplace_back(0.824242);  // ChrMarine@Idle01_StepSidewaysRt01 1
	stateChrMarineStep->addCorrespondencePoints(motions, points1);
	std::vector<double> points2;
	points2.emplace_back(2.165);  // ChrMarine@Idle01 2
	points2.emplace_back(1.32);  // ChrMarine@Idle01_StepBackwardsLf01 2
	points2.emplace_back(1.32);  // ChrMarine@Idle01_StepBackwardsRt01 2
	points2.emplace_back(1.985);  // ChrMarine@Idle01_StepForwardLf01 2
	points2.emplace_back(2);  // ChrMarine@Idle01_StepForwardRt01 2
	points2.emplace_back(1.595);  // ChrMarine@Idle01_StepSidewaysLf01 2
	points2.emplace_back(1.6);  // ChrMarine@Idle01_StepSidewaysRt01 2
	stateChrMarineStep->addCorrespondencePoints(motions, points2);

	stateChrMarineStep->addTriangle("ChrMarine@Idle01", "ChrMarine@Idle01_StepBackwardsLf01", "ChrMarine@Idle01_StepSidewaysLf01");
	stateChrMarineStep->addTriangle("ChrMarine@Idle01", "ChrMarine@Idle01_StepForwardLf01", "ChrMarine@Idle01_StepSidewaysLf01");
	stateChrMarineStep->addTriangle("ChrMarine@Idle01", "ChrMarine@Idle01_StepBackwardsRt01", "ChrMarine@Idle01_StepSidewaysRt01");
	stateChrMarineStep->addTriangle("ChrMarine@Idle01", "ChrMarine@Idle01_StepForwardRt01", "ChrMarine@Idle01_StepSidewaysRt01");

}

void locomotionChrMarinetransitions(SmartBody::SBScene& scene) {
// mirrorMotion = scene.getMotion("ChrUsaMleAdult@Idle01_ToWalkRt01");
// mirrorMotion->mirror("ChrUsaMleAdult@Idle01_ToWalkLf01", "ChrGarza.sk");

	auto stateManager = scene.getBlendManager();

// transition1 = stateManager.createTransition("ChrGarzaStartingLeft", "ChrGarzaLocomotion");
// transition1.addCorrespondancePoint("ChrUsaMleAdult@Idle01_ToWalkLf01", "ChrUsaMleAdult@Walk01", 0.54, 0.85, 0.00, 0.2);
// transition2 = stateManager.createTransition("ChrGarzaStartingRight", "ChrGarzaLocomotion");
// transition2.addCorrespondancePoint("ChrUsaMleAdult@Idle01_ToWalkRt01", "ChrUsaMleAdult@Walk01", 0.54, 0.9, 0.74, 0.93);

	auto walkRightTransitionIn = stateManager->createTransition("mocapStartingRight", "mocapLocomotion");
	walkRightTransitionIn->setEaseInInterval("ChrMarine@Walk01", 1.30, 1.56);
// walkRightTransitionIn.addEaseOutInterval("ChrMarine@Idle01_ToWalkLf01",1.34,1.56);
	walkRightTransitionIn->addEaseOutInterval("ChrMarine@Idle01_ToWalk01_Turn180Rt01", 2.06, 2.45);

	auto walkLeftTransitionIn = stateManager->createTransition("mocapStartingLeft", "mocapLocomotion");
	walkLeftTransitionIn->setEaseInInterval("ChrMarine@Walk01", 0.75, 1.08);
	walkLeftTransitionIn->addEaseOutInterval("ChrMarine@Idle01_ToWalk01_Turn180Lf01", 2.06, 2.45);
// walkLeftTransitionIn.addEaseOutInterval("ChrMarine@Idle01_ToWalk01",1.33,1.56);


// jumpTransitionIn = stateManager.createTransition("ChrMarineLocomotion", "ChrMarineRunJumpState");
// jumpTransitionIn.setEaseInInterval("ChrBrad_ChrMarine@Run01_JumpHigh01", 0.00,0.2);
// jumpTransitionIn.addEaseOutInterval("ChrBrad_ChrMarine@Run01",0.47,0.67);

// jumpTransitionOut = stateManager.createTransition( "ChrMarineRunJumpState","ChrMarineLocomotion");
// jumpTransitionOut.setEaseInInterval("ChrBrad_ChrMarine@Run01",0.33,0.5);
// jumpTransitionOut.addEaseOutInterval("ChrBrad_ChrMarine@Run01_JumpHigh01",1.10,1.35);

}

SmartBody::SBJointMap& createZebra2Mapping(SmartBody::SBScene& scene) {
	auto jointMapManager = scene.getJointMapManager();
	auto zebra2Map = jointMapManager->getJointMap("zebra2");
	if (!zebra2Map) {
		zebra2Map = jointMapManager->createJointMap("zebra2");
	}

	zebra2Map->setMapping("JtRoot", "base");
	zebra2Map->setMapping("JtSpineA", "spine1");
	zebra2Map->setMapping("JtSpineB", "spine2");
	zebra2Map->setMapping("JtSpineC", "spine3");
	zebra2Map->setMapping("JtNeckA", "spine4");
	zebra2Map->setMapping("JtNeckB", "spine5");
	zebra2Map->setMapping("JtSkullA", "skullbase");

	// Arm, left
	zebra2Map->setMapping("JtClavicleLf", "l_sternoclavicular");
	zebra2Map->setMapping("JtShoulderLf", "l_shoulder");
	zebra2Map->setMapping("JtUpperArmTwistALf", "l_upperarm1");
	zebra2Map->setMapping("JtUpperArmTwistBLf", "l_upperarm2");
	zebra2Map->setMapping("JtElbowLf", "l_elbow");
	zebra2Map->setMapping("JtForearmTwistALf", "l_forearm1");
	zebra2Map->setMapping("JtForearmTwistBLf", "l_forearm2");
	zebra2Map->setMapping("JtWristLf", "l_wrist");
	zebra2Map->setMapping("JtThumbALf", "l_thumb1");
	zebra2Map->setMapping("JtThumbBLf", "l_thumb2");
	zebra2Map->setMapping("JtThumbCLf", "l_thumb3");
	zebra2Map->setMapping("JtThumbDLf", "l_thumb4");
	zebra2Map->setMapping("JtIndexALf", "l_index1");
	zebra2Map->setMapping("JtIndexBLf", "l_index2");
	zebra2Map->setMapping("JtIndexCLf", "l_index3");
	zebra2Map->setMapping("JtIndexDLf", "l_index4");
	zebra2Map->setMapping("JtMiddleALf", "l_middle1");
	zebra2Map->setMapping("JtMiddleBLf", "l_middle2");
	zebra2Map->setMapping("JtMiddleCLf", "l_middle3");
	zebra2Map->setMapping("JtMiddleDLf", "l_middle4");
	zebra2Map->setMapping("JtRingALf", "l_ring1");
	zebra2Map->setMapping("JtRingBLf", "l_ring2");
	zebra2Map->setMapping("JtRingCLf", "l_ring3");
	zebra2Map->setMapping("JtRingDLf", "l_ring4");
	zebra2Map->setMapping("JtLittleALf", "l_pinky1");
	zebra2Map->setMapping("JtLittleBLf", "l_pinky2");
	zebra2Map->setMapping("JtLittleCLf", "l_pinky3");
	zebra2Map->setMapping("JtLittleDLf", "l_pinky4");

	// Arm, right
	zebra2Map->setMapping("JtClavicleRt", "r_sternoclavicular");
	zebra2Map->setMapping("JtShoulderRt", "r_shoulder");
	zebra2Map->setMapping("JtUpperArmTwistARt", "r_upperarm1");
	zebra2Map->setMapping("JtUpperArmTwistBRt", "r_upperarm2");
	zebra2Map->setMapping("JtElbowRt", "r_elbow");
	zebra2Map->setMapping("JtForearmTwistARt", "r_forearm1");
	zebra2Map->setMapping("JtForearmTwistBRt", "r_forearm2");
	zebra2Map->setMapping("JtWristRt", "r_wrist");
	zebra2Map->setMapping("JtThumbARt", "r_thumb1");
	zebra2Map->setMapping("JtThumbBRt", "r_thumb2");
	zebra2Map->setMapping("JtThumbCRt", "r_thumb3");
	zebra2Map->setMapping("JtThumbDRt", "r_thumb4");
	zebra2Map->setMapping("JtIndexARt", "r_index1");
	zebra2Map->setMapping("JtIndexBRt", "r_index2");
	zebra2Map->setMapping("JtIndexCRt", "r_index3");
	zebra2Map->setMapping("JtIndexDRt", "r_index4");
	zebra2Map->setMapping("JtMiddleARt", "r_middle1");
	zebra2Map->setMapping("JtMiddleBRt", "r_middle2");
	zebra2Map->setMapping("JtMiddleCRt", "r_middle3");
	zebra2Map->setMapping("JtMiddleDRt", "r_middle4");
	zebra2Map->setMapping("JtRingARt", "r_ring1");
	zebra2Map->setMapping("JtRingBRt", "r_ring2");
	zebra2Map->setMapping("JtRingCRt", "r_ring3");
	zebra2Map->setMapping("JtRingDRt", "r_ring4");
	zebra2Map->setMapping("JtLittleARt", "r_pinky1");
	zebra2Map->setMapping("JtLittleBRt", "r_pinky2");
	zebra2Map->setMapping("JtLittleCRt", "r_pinky3");
	zebra2Map->setMapping("JtLittleDRt", "r_pinky4");

	// Leg, left
	zebra2Map->setMapping("JtHipLf", "l_hip");
	zebra2Map->setMapping("JtKneeLf", "l_knee");
	zebra2Map->setMapping("JtAnkleLf", "l_ankle");
	zebra2Map->setMapping("JtBallLf", "l_forefoot");
	zebra2Map->setMapping("JtToeLf", "l_toe");

	// Leg, right
	zebra2Map->setMapping("JtHipRt", "r_hip");
	zebra2Map->setMapping("JtKneeRt", "r_knee");
	zebra2Map->setMapping("JtAnkleRt", "r_ankle");
	zebra2Map->setMapping("JtBallRt", "r_forefoot");
	zebra2Map->setMapping("JtToeRt", "r_toe");

	// Head, left
	zebra2Map->setMapping("JtEyeLf", "eyeball_left");
	zebra2Map->setMapping("JtEyelidUpperLf", "upper_eyelid_left");
	zebra2Map->setMapping("JtEyelidLowerLf", "lower_eyelid_left");

	// Head, right
	zebra2Map->setMapping("JtEyeRt", "eyeball_right");
	zebra2Map->setMapping("JtEyelidUpperRt", "upper_eyelid_right");
	zebra2Map->setMapping("JtEyelidLowerRt", "lower_eyelid_right");
	return *zebra2Map;
}

}

BasicCharacter::BasicCharacter(SmartBody::SBScene& scene) : mScene(scene), bradFace(*scene.createFaceDefinition("ChrBrad")){
	auto& zebra2Map = createZebra2Mapping(scene);

	auto bradSkeleton = scene.getSkeleton("ChrBrad.dae");
	bradSkeleton->rescale(.01);
	zebra2Map.applySkeleton(bradSkeleton.get());
	zebra2Map.applyMotionRecurse(scene, "ChrBrad");

	// Establish lip syncing data set
	//print("Establishing lip syncing data set");
	//mScene.run("init-diphoneDefault.py");


	bradFace.setFaceNeutral("ChrBrad@face_neutral");
	bradFace.setAU(1, "left", "ChrBrad@001_inner_brow_raiser_lf");
	bradFace.setAU(1, "right", "ChrBrad@001_inner_brow_raiser_rt");
	bradFace.setAU(2, "left", "ChrBrad@002_outer_brow_raiser_lf");
	bradFace.setAU(2, "right", "ChrBrad@002_outer_brow_raiser_rt");
	bradFace.setAU(4, "left", "ChrBrad@004_brow_lowerer_lf");
	bradFace.setAU(4, "right", "ChrBrad@004_brow_lowerer_rt");
	bradFace.setAU(5, "both", "ChrBrad@005_upper_lid_raiser");
	bradFace.setAU(6, "both", "ChrBrad@006_cheek_raiser");
	bradFace.setAU(7, "both", "ChrBrad@007_lid_tightener");
	bradFace.setAU(10, "both", "ChrBrad@010_upper_lip_raiser");
	bradFace.setAU(12, "left", "ChrBrad@012_lip_corner_puller_lf");
	bradFace.setAU(12, "right", "ChrBrad@012_lip_corner_puller_rt");
	bradFace.setAU(25, "both", "ChrBrad@025_lips_part");
	bradFace.setAU(26, "both", "ChrBrad@026_jaw_drop");
	bradFace.setAU(45, "left", "ChrBrad@045_blink_lf");
	bradFace.setAU(45, "right", "ChrBrad@045_blink_rt");

	bradFace.setViseme("open", "ChrBrad@open");
	bradFace.setViseme("W", "ChrBrad@W");
	bradFace.setViseme("ShCh", "ChrBrad@ShCh");
	bradFace.setViseme("PBM", "ChrBrad@PBM");
	bradFace.setViseme("FV", "ChrBrad@FV");
	bradFace.setViseme("wide", "ChrBrad@wide");
	bradFace.setViseme("tBack", "ChrBrad@tBack");
	bradFace.setViseme("tRoof", "ChrBrad@tRoof");
	bradFace.setViseme("tTeeth", "ChrBrad@tTeeth");

	setupBehaviorSetMaleMocapLocomotion();
	setupBehaviorSetGestures();
	setupBehaviorSetReaching();


}

void BasicCharacter::setupBehaviorSetMaleMocapLocomotion() {
	//print("Setting up behavior set for MaleMocapLocomotion...");

// mScene.loadAssetsFromPath("behaviorsets/MaleMocapLocomotion/skeletons");
// mScene.loadAssetsFromPath("behaviorsets/MaleMocapLocomotion/motions");
	auto& assetStore = mScene.getAssetStore();
	auto assetManager = mScene.getAssetManager();
	std::string motionPath = "behaviorsets/MaleMocapLocomotion/motions/";
	auto skel = mScene.getSkeleton("ChrBackovic.sk");
	if (!skel) {
		mScene.loadAssetsFromPath("behaviorsets/MaleMocapLocomotion/skeletons");
	}

// map the zebra2 skeleton
	auto zebra2Map = mScene.getJointMapManager()->getJointMap("zebra2");
	auto bradSkeleton = mScene.getSkeleton("ChrBackovic.sk");
	zebra2Map->applySkeleton(bradSkeleton.get());
	std::vector<std::string> locoMotions;
	locoMotions.emplace_back("ChrMarine@Idle01");
	locoMotions.emplace_back("ChrMarine@Jog01");
	locoMotions.emplace_back("ChrMarine@Meander01");
	locoMotions.emplace_back("ChrMarine@Run01");
// locoMotions.emplace_back("ChrMarine@RunCircleLf01");
	locoMotions.emplace_back("ChrMarine@RunCircleRt01");
// locoMotions.emplace_back("ChrMarine@RunTightCircleLf01");
	locoMotions.emplace_back("ChrMarine@RunTightCircleRt01");
// locoMotions.emplace_back("ChrMarine@StrafeSlowLf01");
	locoMotions.emplace_back("ChrMarine@StrafeSlowRt01");
	locoMotions.emplace_back("ChrMarine@Walk01");
// locoMotions.emplace_back("ChrMarine@WalkCircleLf01");
	locoMotions.emplace_back("ChrMarine@WalkCircleRt01");
// locoMotions.emplace_back("ChrMarine@WalkTightCircleLf01");
	locoMotions.emplace_back("ChrMarine@WalkTightCircleRt01");
// locoMotions.emplace_back("ChrMarine@Turn360Lf01");
	locoMotions.emplace_back("ChrMarine@Turn360Rt01");
// locoMotions.emplace_back("ChrMarine@StrafeFastLf01");
	locoMotions.emplace_back("ChrMarine@StrafeFastRt01");
// locoMotions.emplace_back("ChrMarine@Idle01_StepBackwardsLf01");
	locoMotions.emplace_back("ChrMarine@Idle01_StepBackwardsRt01");
// locoMotions.emplace_back("ChrMarine@Idle01_StepForwardLf01");
	locoMotions.emplace_back("ChrMarine@Idle01_StepForwardRt01");
// locoMotions.emplace_back("ChrMarine@Idle01_StepSidewaysLf01");
	locoMotions.emplace_back("ChrMarine@Idle01_StepSidewaysRt01");
// locoMotions.emplace_back("ChrMarine@Turn90Lf01");
// locoMotions.emplace_back("ChrMarine@Turn180Lf01");
	locoMotions.emplace_back("ChrMarine@Turn90Rt01");
	locoMotions.emplace_back("ChrMarine@Turn180Rt01");
// locoMotions.emplace_back("ChrMarine@Idle01_ToWalkLf01");
// locoMotions.emplace_back("ChrMarine@Idle01_ToWalk01_Turn90Lf01");
// locoMotions.emplace_back("ChrMarine@Idle01_ToWalk01_Turn180Lf01");
	locoMotions.emplace_back("ChrMarine@Idle01_ToWalk01");
	locoMotions.emplace_back("ChrMarine@Idle01_ToWalk01_Turn90Rt01");
	locoMotions.emplace_back("ChrMarine@Idle01_ToWalk01_Turn180Rt01");

	for (auto& locomotion : locoMotions) {
		auto motion = mScene.getMotion(locomotion);
		if (!motion) {
			assetStore.loadAsset(motionPath + locomotion + ".skm");

			motion = mScene.getMotion(locomotion);
		}
// print "motionName = " + locoMotions[i]
		if (motion) {
			motion->setMotionSkeletonName("ChrBackovic.sk");
			zebra2Map->applyMotion(motion);
			motion->buildJointTrajectory("l_forefoot", "base");
			motion->buildJointTrajectory("r_forefoot", "base");
			motion->buildJointTrajectory("l_ankle", "base");
			motion->buildJointTrajectory("r_ankle", "base");
		}

	}

}

void BasicCharacter::retargetBehaviorSetMaleMocapLocomotion(const std::string& charName) {
	locomotionChrMarineinit(mScene);

	std::vector<std::string> locoMotions;
	locoMotions.emplace_back("ChrMarine@Idle01");
	locoMotions.emplace_back("ChrMarine@Jog01");
	locoMotions.emplace_back("ChrMarine@Meander01");
	locoMotions.emplace_back("ChrMarine@Run01");
	locoMotions.emplace_back("ChrMarine@RunCircleLf01_smooth");
	locoMotions.emplace_back("ChrMarine@RunCircleRt01_smooth");
	locoMotions.emplace_back("ChrMarine@RunTightCircleLf01");
	locoMotions.emplace_back("ChrMarine@RunTightCircleRt01");
	locoMotions.emplace_back("ChrMarine@StrafeSlowLf01");
	locoMotions.emplace_back("ChrMarine@StrafeSlowRt01");
	locoMotions.emplace_back("ChrMarine@Walk01");
	locoMotions.emplace_back("ChrMarine@WalkCircleLf01_smooth");
	locoMotions.emplace_back("ChrMarine@WalkCircleRt01_smooth");
	locoMotions.emplace_back("ChrMarine@WalkTightCircleLf01_smooth");
	locoMotions.emplace_back("ChrMarine@WalkTightCircleRt01_smooth");
	locoMotions.emplace_back("ChrMarine@Turn360Lf01");
	locoMotions.emplace_back("ChrMarine@Turn360Rt01");
	locoMotions.emplace_back("ChrMarine@StrafeFastLf01_smooth");
	locoMotions.emplace_back("ChrMarine@StrafeFastRt01_smooth");
	locoMotions.emplace_back("ChrMarine@Idle01_StepBackwardsLf01");
	locoMotions.emplace_back("ChrMarine@Idle01_StepBackwardsRt01");
	locoMotions.emplace_back("ChrMarine@Idle01_StepForwardLf01");
	locoMotions.emplace_back("ChrMarine@Idle01_StepForwardRt01");
	locoMotions.emplace_back("ChrMarine@Idle01_StepSidewaysLf01");
	locoMotions.emplace_back("ChrMarine@Idle01_StepSidewaysRt01");
	locoMotions.emplace_back("ChrMarine@Turn90Lf01");
	locoMotions.emplace_back("ChrMarine@Turn180Lf01");
	locoMotions.emplace_back("ChrMarine@Turn90Rt01");
	locoMotions.emplace_back("ChrMarine@Turn180Rt01");
	locoMotions.emplace_back("ChrMarine@Idle01_ToWalkLf01");
	locoMotions.emplace_back("ChrMarine@Idle01_ToWalk01_Turn90Lf01");
	locoMotions.emplace_back("ChrMarine@Idle01_ToWalk01_Turn180Lf01");
	locoMotions.emplace_back("ChrMarine@Idle01_ToWalk01");
	locoMotions.emplace_back("ChrMarine@Idle01_ToWalk01_Turn90Rt01");
	locoMotions.emplace_back("ChrMarine@Idle01_ToWalk01_Turn180Rt01");


	auto sbChar = mScene.getCharacter(charName);
	if (!sbChar) {
		return;
	}
	auto skelName = sbChar->getSkeleton()->getName();
	createRetargetInstance(mScene, "ChrBackovic.sk", skelName);
	auto stateManager = mScene.getBlendManager();
	auto mocapLocomotionState = stateManager->getBlend("mocapLocomotion");
	if (!mocapLocomotionState) {
// compute motion trajectory
		for (auto& locomotion : locoMotions) {
			auto motion = mScene.getMotion(locomotion);
// print "motionName = " + locoMotions[i]
			if (motion) {
				motion->buildJointTrajectory("l_forefoot", "base");
				motion->buildJointTrajectory("r_forefoot", "base");
				motion->buildJointTrajectory("l_ankle", "base");
				motion->buildJointTrajectory("r_ankle", "base");
			}
		}

// setup standard locomotion
		//	mScene.run("locomotion-ChrMarine-state-Locomotion.py");
		locomotionChrMarinestateLocomotion(mScene);
// marineLocomotionSetup("ChrBackovic.sk", "ChrBackovic.sk", "base", "", "mocap");

// starting state, starting locomotion with different angle
		//mScene.run("locomotion-ChrMarine-state-StartingLeft.py");
		locomotionChrMarinestateStartingLeft(mScene);
		// mocapStartLeftSetup("ChrBackovic.sk", "ChrBackovic.sk", "base", "", "mocap");

// starting state, starting locomotion with different angle
//		mScene.run("locomotion-ChrMarine-state-StartingRight.py");
		locomotionChrMarinestateStartingRight(mScene);
// mocapStartRightSetup("ChrBackovic.sk", "ChrBackovic.sk", "base", "", "mocap");

// idle turn state, facing adjusting
//		mScene.run("locomotion-ChrMarine-state-IdleTurn.py");
		locomotionChrMarinestateIdleTurn(mScene);
// mocapIdleTurnSetup("ChrBackovic.sk", "ChrBackovic.sk", "base", "", "mocap");

// step state, stepping adjusting
//		mScene.run("locomotion-ChrMarine-state-Step.py");
		locomotionChrMarinestateStep(mScene);
// mocapStepSetup("ChrBackovic.sk", "ChrBackovic.sk", "base", "", "mocap");

// setup transitions
//		mScene.run("locomotion-ChrMarine-transitions.py");
		locomotionChrMarinetransitions(mScene);

	}
// add IK constraint for foot automatically
	sbChar->addJointTrajectoryConstraint("l_forefoot", "base");
	sbChar->addJointTrajectoryConstraint("r_forefoot", "base");
// sbChar->addJointTrajectoryConstraint("l_ankle", "base");
// sbChar->addJointTrajectoryConstraint("r_ankle", "base");
// setup steering
//	mScene.run("init-steer-agents.py");


}

void BasicCharacter::setupBehaviorSetGestures() {
	//print("Setting up behavior set for gestures...");
	mScene.addAssetPath("script", "behaviorsets/Gestures2/scripts");

	auto assetManager = mScene.getAssetManager();
	auto& assetStore = mScene.getAssetStore();
//	motionPath = "behaviorsets/Gestures2/motions/"
	auto motionPath = "behaviorsets/Gestures2/motionsBinary/";
	auto skel = mScene.getSkeleton("ChrBrad2.sk");
	if (!skel) {
		mScene.loadAssetsFromPath("behaviorsets/Gestures2/skeletons");
	}
// map the zebra2 skeleton
//	mScene.run("zebra2-map.py");
	auto zebra2Map = mScene.getJointMapManager()->getJointMap("zebra2");
	auto bradSkeleton = mScene.getSkeleton("ChrBrad2.sk");
	zebra2Map->applySkeleton(bradSkeleton.get());

	std::vector<std::string> gestureMotions;

	gestureMotions.emplace_back("ChrBrad@Idle01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ArmStretch01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatFistMidLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatFistMidLf02");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatHighBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatHighBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatHighBt03");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatHighLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatLowBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatLowBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatLowLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatLowLf02");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatLowRt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatMidBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatMidLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatPointMidLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BesidesLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ChopBoth01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ChopLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ChopLf02");
	gestureMotions.emplace_back("ChrBrad@Idle01_ChopRt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_Contemplate01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ExampleLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_Guitar01");
	gestureMotions.emplace_back("ChrBrad@Idle01_HereBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_HereBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_HoweverLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_InclusivityNegBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_InclusivityNegBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_InclusivityPosBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_InclusivityPosBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_IndicateLeftBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_IndicateLeftLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_IndicateRightBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_IndicateRightRt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_MeLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_MeLf02");
	gestureMotions.emplace_back("ChrBrad@Idle01_NegativeBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_NegativeBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_NegativeBt03");
	gestureMotions.emplace_back("ChrBrad@Idle01_NegativeRt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_OfferBoth01");
	gestureMotions.emplace_back("ChrBrad@Idle01_OfferLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_PleaBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_PointLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_PointLf02");
	gestureMotions.emplace_back("ChrBrad@Idle01_PointRt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ReceiveRt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_SafeLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ScratchChest01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ScratchHeadLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ScratchTempleLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ShoulderStretch01");
	gestureMotions.emplace_back("ChrBrad@Idle01_Shrug01");
	gestureMotions.emplace_back("ChrBrad@Idle01_Shrug02");
	gestureMotions.emplace_back("ChrBrad@Idle01_SqueezeHand01");
	gestureMotions.emplace_back("ChrBrad@Idle01_StopBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_StopBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_SurroundBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_SurroundBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_Think01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ToIdle02");
	gestureMotions.emplace_back("ChrBrad@Idle01_ToIdle03");
	gestureMotions.emplace_back("ChrBrad@Idle01_ToLocIdle01");
	gestureMotions.emplace_back("ChrBrad@Idle01_TouchHands01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ToWalk01_Turn180Rt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ToWalk01_Turn360Rt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ToWalk01_Turn90Rt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_WeightShift01");
	gestureMotions.emplace_back("ChrBrad@Idle01_WeightShift02");
	gestureMotions.emplace_back("ChrBrad@Idle01_WeRt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_WeRt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_WhenBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_WhenBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_YouLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_YouLf02");
	gestureMotions.emplace_back("ChrBrad@Idle01_YouLf03");
	gestureMotions.emplace_back("ChrBrad@Idle01_YouPointLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_YouRt01");

	for (auto& motionName : gestureMotions) {
		auto motion = mScene.getMotion(motionName);
		if (!motion) {
// assetManager.loadAsset(motionPath+gestureMotions[i]+".skm");
			assetStore.loadAsset(motionPath + motionName + ".skmb");
			motion = mScene.getMotion(motionName);
		}
// print "motionName = " + locoMotions[i]
		if (motion) {
			motion->setMotionSkeletonName("ChrBrad2.sk");
			zebra2Map->applyMotion(motion);
		}
	}


// create a set of blends between the 1x and the 2x speed motions
	auto skeleton = "ChrBrad2.sk";
	for (auto i = 1; i < gestureMotions.size(); ++i) {
		auto stateTest = mScene.getBlendManager()->createBlend1D(gestureMotions[i] + "Blend");
		stateTest->setBlendSkeleton(skeleton);
		stateTest->addMotion(gestureMotions[i], 0.0);
		stateTest->addMotion(gestureMotions[i] + "2x", 1.0);
	}

// establish gesture data set
	auto gMapManager = mScene.getGestureMapManager();

// Brad"s gestures
// neutral

// add gesture blends
	skeleton = "ChrBrad2.sk";
	std::vector<std::string> stateNames;
	stateNames.emplace_back("BeathHighBt");
	stateNames.emplace_back("BeatFistMidLf");
	stateNames.emplace_back("BeatLowBt");
	stateNames.emplace_back("ChopLf");
	stateNames.emplace_back("HereBt");
	stateNames.emplace_back("InclusivityNegBt");
	stateNames.emplace_back("InclusivityPosBt");
	stateNames.emplace_back("MeLf");
	stateNames.emplace_back("YouLf");
	stateNames.emplace_back("NegativeBt");
	stateNames.emplace_back("PointLf");
	stateNames.emplace_back("Shrug");
	stateNames.emplace_back("Stop");
	stateNames.emplace_back("Surround");

	std::vector<std::string> motionNames;
	motionNames.emplace_back("ChrBrad@Idle01_BeatHighBt01");
	motionNames.emplace_back("ChrBrad@Idle01_BeatHighBt03");
	motionNames.emplace_back("ChrBrad@Idle01_BeatFistMidLf01");
	motionNames.emplace_back("ChrBrad@Idle01_BeatFistMidLf02");
	motionNames.emplace_back("ChrBrad@Idle01_BeatLowBt01");
	motionNames.emplace_back("ChrBrad@Idle01_BeatLowBt02");
	motionNames.emplace_back("ChrBrad@Idle01_ChopLf01");
	motionNames.emplace_back("ChrBrad@Idle01_ChopLf02");
	motionNames.emplace_back("ChrBrad@Idle01_HereBt01");
	motionNames.emplace_back("ChrBrad@Idle01_HereBt02");
	motionNames.emplace_back("ChrBrad@Idle01_InclusivityNegBt01");
	motionNames.emplace_back("ChrBrad@Idle01_InclusivityNegBt02");
	motionNames.emplace_back("ChrBrad@Idle01_InclusivityPosBt01");
	motionNames.emplace_back("ChrBrad@Idle01_InclusivityPosBt02");
	motionNames.emplace_back("ChrBrad@Idle01_MeLf01");
	motionNames.emplace_back("ChrBrad@Idle01_MeLf02");
	motionNames.emplace_back("ChrBrad@Idle01_YouLf01");
	motionNames.emplace_back("ChrBrad@Idle01_YouLf02");
	motionNames.emplace_back("ChrBrad@Idle01_NegativeBt01");
	motionNames.emplace_back("ChrBrad@Idle01_NegativeBt02");
	motionNames.emplace_back("ChrBrad@Idle01_PointLf01");
	motionNames.emplace_back("ChrBrad@Idle01_PointLf02");
	motionNames.emplace_back("ChrBrad@Idle01_Shrug01");
	motionNames.emplace_back("ChrBrad@Idle01_Shrug02");
	motionNames.emplace_back("ChrBrad@Idle01_StopBt01");
	motionNames.emplace_back("ChrBrad@Idle01_StopBt02");
	motionNames.emplace_back("ChrBrad@Idle01_SurroundBt01");
	motionNames.emplace_back("ChrBrad@Idle01_SurroundBt02");

	for (auto i = 0; i < stateNames.size(); ++i) {
		auto stateTest = mScene.getBlendManager()->createBlend1D(stateNames[i]);
		stateTest->setBlendSkeleton(skeleton);
		std::vector<std::string> motions;
		motions.emplace_back(motionNames[i * 2]);
		motions.emplace_back(motionNames[i * 2 + 1]);

		stateTest->addMotion(motionNames[i * 2], 0.0);
		stateTest->addMotion(motionNames[i * 2 + 1], 1.0);
		//print(("Added gesture blend " + stateNames[i]));
	}


	auto gMapB1 = gMapManager->createGestureMap("BradGesture");

// gMapB1->addGestureMapping("ChrBrad@Idle01_ChopLf01", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChopLf", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_ChopRt02Blend", "METAPHORIC", "OBLIGATION", "RIGHT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_ChopBoth01Blend", "METAPHORIC", "OBLIGATION", "BOTH_HANDS", "", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "BEAT", "RHYTHM", "RIGHT_HAND", "", "ChrBrad@Idle01");
// gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01");

// Begin STACY ADDING HERE
// hands terrible
// "ChrBrad@Idle01_BeatHighLf01"
// "ChrBrad@Idle01_BeatLowBt01"
// BeatLowLf01 and 02

// Both hands

	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatHighBt01Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatHighBt02Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatHighBt03Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatMidBt01Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_ChopBoth01Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_Shrug02Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01");

// Left hand
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatFistMidLf01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatFistMidLf02Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatMidLf01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatPointMidLf01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BesidesLf01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_ChopLf01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_ChopLf02Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_PointLf02Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_SafeLf01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_YouLf02Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_YouPoint01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");

// Right Hand
	gMapB1->addGestureMapping("ChrBrad@Idle01_ChopRt02Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_PointRt01Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_WeRt02Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01");

// End STACY ADDING HERE

	gMapB1->addGestureMapping("BeatLowBt", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatHighLf01Blend", "DEICTIC", "LEFT", "LEFT_HAND", "high", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatMidLf01Blend", "DEICTIC", "LEFT", "LEFT_HAND", "high", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_HoweverLf01Blend", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_IndicateLeftLf01Blend", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowRt01Blend", "DEICTIC", "RIGHT", "RIGHT_HAND", "high", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_IndicateRightRt01Blend", "DEICTIC", "RIGHT", "RIGHT_HAND", "", "ChrBrad@Idle01");

// gMapB1->addGestureMapping("ChrBrad@Idle01_HereBt02", "DEICTIC", "HERE", "BOTH_HANDS", "high", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("HereBt", "DEICTIC", "HERE", "BOTH_HANDS", "high", "ChrBrad@Idle01");
// gMapB1->addGestureMapping("ChrBrad@Idle01_ChopBoth01", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowBt01Blend", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01");
// gMapB1->addGestureMapping("ChrBrad@Idle01_Frame", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_NegativeBt02Blend", "METAPHORIC", "NEGATION", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_NegativeRt01Blend", "METAPHORIC", "NEGATION", "RIGHT_HAND", "", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_SafeLf01Blend", "METAPHORIC", "ASIDE", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BesidesLf01Blend", "METAPHORIC", "ASIDE", "LEFT_HAND", "", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatMidBt01Blend", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatMidLf01Blend", "METAPHORIC", "QUESTION", "LEFT_HAND", "noheadtilt", "ChrBrad@Idle01");

// gMapB1->addGestureMapping("ChrBrad@Idle01_YouLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("YouLf", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_OfferBoth01Blend", "DEICTIC", "YOU", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_OfferLf01Blend", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_ReceiveRt01Blend", "DEICTIC", "YOU", "RIGHT_HAND", "", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowBt01Blend", "METAPHORIC", "INTENSIFIER_POSITIVE", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatMidBt01Blend", "METAPHORIC", "INTENSIFIER_POSITIVE", "BOTH_HANDS", "", "ChrBrad@Idle01");

// gMapB1->addGestureMapping("ChrBrad@Idle01_MeLf01", "DEICTIC", "ME", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("MeLf", "DEICTIC", "ME", "LEFT_HAND", "", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_WeRt01Blend", "DEICTIC", "WE", "RIGHT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_WeRt02Blend", "DEICTIC", "WE", "RIGHT_HAND", "", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowLf01Blend", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowRt01Blend", "METAPHORIC", "CONTRAST", "RIGHT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowLf02Blend", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_IndicateRightR01Blend", "METAPHORIC", "CONTRAST", "RIGHT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_HoweverLf01Blend", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrBrad@Idle01");

// gMapB1->addGestureMapping("ChrBrad@Idle01_BeatHighBt01", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01");
// gMapB1->addGestureMapping("ChrBrad@Idle01_BeatHighBt02", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("BeathHighBt", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowBt01Blend", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatMidBt01Blend", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatMidLf01Blend", "METAPHORIC", "QUESTION", "LEFT_HAND", "noheadtilt", "ChrBrad@Idle01");

// gMapB1->addGestureMapping("ChrBrad@Idle01_Shrug01", "METAPHORIC", "UNCERTAINTY", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("Shrug", "METAPHORIC", "UNCERTAINTY", "BOTH_HANDS", "", "ChrBrad@Idle01");

// gMapB1->addGestureMapping("ChrBrad@Idle01_Shrug01", "METAPHORIC", "INDIFFERENCE", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("Shrug", "METAPHORIC", "INDIFFERENCE", "BOTH_HANDS", "", "ChrBrad@Idle01");

// metaphoric inclusivity
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatMidBt01Blend", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "mid", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowBt01Blend", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrBrad@Idle01");
// gMapB1->addGestureMapping("ChrBrad@Idle01_InclusivityPosBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("InclusivityPosBt", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrBrad@Idle01");

// metaphoric quantity_nothing
// gMapB1->addGestureMapping("ChrBrad@Idle01_NegativeBt01", "METAPHORIC", "EMPTY", "BOTH_HANDS", "high", "ChrBrad@Idle01");
// gMapB1->addGestureMapping("ChrBrad@Idle01_NegativeBt02", "METAPHORIC", "EMPTY", "BOTH_HANDS", "high", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("NegativeBt", "METAPHORIC", "EMPTY", "BOTH_HANDS", "high", "ChrBrad@Idle01");

	gMapB1->addGestureMapping("ChrBrad@Idle01_NegativeRt01Blend", "METAPHORIC", "EMPTY", "RIGHT_HAND", "high", "ChrBrad@Idle01");

// gMapB1->addGestureMapping("ChrBrad@Idle01_SurroundBt01", "METAPHORIC", "SURROUND", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("SurroundBt", "METAPHORIC", "SURROUND", "BOTH_HANDS", "", "ChrBrad@Idle01");

// gMapB1->addGestureMapping("ChrBrad@Idle01_StopBt01", "METAPHORIC", "STOP", "RIGHT_HAND", "", "ChrBrad@Idle02");
	gMapB1->addGestureMapping("StopBt", "METAPHORIC", "STOP", "RIGHT_HAND", "", "ChrBrad@Idle02");

	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowLf01Blend", "EMBLEM", "GREETING", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB1->addGestureMapping("ChrBrad@Idle01_BeatLowRt01Blend", "EMBLEM", "GREETING", "RIGHT_HAND", "", "ChrBrad@Idle01");

	auto gMapB2 = gMapManager->createGestureMap("BradGestureAngry");

	gMapB2->addGestureMapping("ChrBrad@Idle01_YouLf02", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_YouPointLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_MeLf02", "DEICTIC", "ME", "LEFT_HAND", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_WeRt02", "DEICTIC", "WE", "RIGHT_HAND", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_BeatMidBt01", "DEICTIC", "WE", "BOTH_HANDS", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_PointLf02", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_BeatHighLf01", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_PointRt01", "DEICTIC", "RIGHT", "RIGHT_HAND", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "DEICTIC", "CENTER", "BOTH_HANDS", "", "ChrBrad@Idle01");
// gMapB2->addGestureMapping("ChrBrad@Idle01_HereBt02", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01");
// gMapB2->addGestureMapping("ChrBrad@Idle01_HereBt02", "DEICTIC", "CENTER", "BOTH_HANDS", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_InclusivityNegBt02", "METAPHORIC", "NEGATION", "BOTH_HANDS", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_BeatHighBt03", "METAPHORIC", "INTENSIFIER_POSITIVE", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_BeatHighBt02", "METAPHORIC", "INTENSIFIER_POSITIVE", "BOTH_HANDS", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_PointRt01", "METAPHORIC", "CONTRAST", "RIGHT_HAND", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_PointLf02", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_BeatHighBt03", "METAPHORIC", "QUESTION", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_PointRt01", "METAPHORIC", "QUESTION", "RIGHT_HAND", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_PointLf02", "METAPHORIC", "QUESTION", "LEFT_HAND", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_Shrug02", "METAPHORIC", "UNCERTAINTY", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_BeatHighBt03", "METAPHORIC", "UNCERTAINTY", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_Shrug02", "METAPHORIC", "INDIFFERENCE", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_BeatHighBt03", "METAPHORIC", "INDIFFERENCE", "BOTH_HANDS", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_ChopLf02", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_InclusivityNegBt02", "METAPHORIC", "EMPTY", "BOTH_HANDS", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_BesidesLf01", "METAPHORIC", "ASIDE", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_BesidesLf01", "METAPHORIC", "SURROUND", "BOTH_HANDS", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_InclusivityNegBt02", "METAPHORIC", "STOP", "BOTH_HANDS", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_ChopLf02", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_ChopRt02", "METAPHORIC", "OBLIGATION", "RIGHT_HAND", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_BeatLowBt02", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_BeatPointMidLf01", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_BeatFistMidLf01", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_BeatFistMidLf02", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");

	gMapB2->addGestureMapping("ChrBrad@Idle01_ChopLf02", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB2->addGestureMapping("ChrBrad@Idle01_YouPointLf01", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01");

// sad
	auto gMapB3 = gMapManager->createGestureMap("BradGestureSad");
	gMapB3->addGestureMapping("ChrBrad@Idle01_YouLf03", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB3->addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB3->addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "DEICTIC", "YOU", "RIGHT_HAND", "", "ChrBrad@Idle01");

	gMapB3->addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB3->addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "DEICTIC", "RIGHT", "RIGHT_HAND", "", "ChrBrad@Idle01");

	gMapB3->addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "METAPHORIC", "NEGATION", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB3->addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "METAPHORIC", "NEGATION", "RIGHT_HAND", "", "ChrBrad@Idle01");

	gMapB3->addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "QUESTION", "LEFT_HAND", "", "ChrBrad@Idle01");

	gMapB3->addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "INDIFFERENCE", "LEFT_HAND", "", "ChrBrad@Idle01");
	gMapB3->addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "UNCERTAINTY", "LEFT_HAND", "", "ChrBrad@Idle01");
}

void BasicCharacter::retargetBehaviorSetGestures(const std::string& charName) {
	std::vector<std::string> gestureMotions;
	gestureMotions.emplace_back("ChrBrad@Idle01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ArmStretch01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatFistMidLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatFistMidLf02");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatHighBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatHighBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatHighBt03");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatHighLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatLowBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatLowBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatLowLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatLowLf02");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatLowRt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatMidBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatMidLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BeatPointMidLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_BesidesLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ChopBoth01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ChopLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ChopLf02");
	gestureMotions.emplace_back("ChrBrad@Idle01_ChopRt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_Contemplate01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ExampleLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_Guitar01");
	gestureMotions.emplace_back("ChrBrad@Idle01_HereBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_HereBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_HoweverLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_InclusivityNegBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_InclusivityNegBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_InclusivityPosBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_InclusivityPosBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_IndicateLeftBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_IndicateLeftLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_IndicateRightBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_IndicateRightRt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_MeLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_MeLf02");
	gestureMotions.emplace_back("ChrBrad@Idle01_NegativeBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_NegativeBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_NegativeBt03");
	gestureMotions.emplace_back("ChrBrad@Idle01_NegativeRt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_OfferBoth01");
	gestureMotions.emplace_back("ChrBrad@Idle01_OfferLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_PleaBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_PointLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_PointLf02");
	gestureMotions.emplace_back("ChrBrad@Idle01_PointRt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ReceiveRt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_SafeLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ScratchChest01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ScratchHeadLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ScratchTempleLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ShoulderStretch01");
	gestureMotions.emplace_back("ChrBrad@Idle01_Shrug01");
	gestureMotions.emplace_back("ChrBrad@Idle01_Shrug02");
	gestureMotions.emplace_back("ChrBrad@Idle01_SqueezeHand01");
	gestureMotions.emplace_back("ChrBrad@Idle01_StopBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_StopBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_SurroundBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_SurroundBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_Think01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ToIdle02");
	gestureMotions.emplace_back("ChrBrad@Idle01_ToIdle03");
	gestureMotions.emplace_back("ChrBrad@Idle01_ToLocIdle01");
	gestureMotions.emplace_back("ChrBrad@Idle01_TouchHands01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ToWalk01_Turn180Rt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ToWalk01_Turn360Rt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_ToWalk01_Turn90Rt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_WeightShift01");
	gestureMotions.emplace_back("ChrBrad@Idle01_WeightShift02");
	gestureMotions.emplace_back("ChrBrad@Idle01_WeRt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_WeRt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_WhenBt01");
	gestureMotions.emplace_back("ChrBrad@Idle01_WhenBt02");
	gestureMotions.emplace_back("ChrBrad@Idle01_YouLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_YouLf02");
	gestureMotions.emplace_back("ChrBrad@Idle01_YouLf03");
	gestureMotions.emplace_back("ChrBrad@Idle01_YouPointLf01");
	gestureMotions.emplace_back("ChrBrad@Idle01_YouRt01");

	auto sbChar = mScene.getCharacter(charName);
	if (!sbChar) {
		return;
	}
	auto skelName = sbChar->getSkeleton()->getName();

	std::vector<std::string> motions;
	auto assetManager = mScene.getAssetManager();
	for (auto& gestureName : gestureMotions) {
		auto sbMotion = assetManager->getMotion(gestureName);
		if (sbMotion) {
			sbMotion->setMotionSkeletonName("ChrBrad2.sk");
		}
	}

	createRetargetInstance(mScene, "ChrBrad2.sk", skelName);


	sbChar->setStringAttribute("gestureMap", "BradGesture");
	if (!sbChar->getAttribute("gestureMapAngry")) {
		sbChar->createStringAttribute("gestureMapAngry", "", true, "Gestures", 51, false, false, false, "angry gesture map");
	}
	sbChar->setStringAttribute("gestureMapAngry", "BradGestureAngry");
	if (!sbChar->getAttribute("gestureMapSad")) {
		sbChar->createStringAttribute("gestureMapSad", "", true, "Gestures", 52, false, false, false, "sad gesture map");
	}
	sbChar->setStringAttribute("gestureMapSad", "BradGestureSad");
}

void BasicCharacter::setupBehaviorSetReaching() {
	//print("Setting up behavior set for Reaching ...");
	mScene.addAssetPath("script", "behaviorsets/reaching/scripts");

	auto& assetStore = mScene.getAssetStore();
	auto assetManager = mScene.getAssetManager();
// motionPath = "behaviorsets/reaching/motions/"
	auto motionPath = "behaviorsets/reaching/motionsBinary/";
	auto skel = mScene.getSkeleton("common.sk");
	if (!skel) {
		mScene.loadAssetsFromPath("behaviorsets/reaching/skeletons");
	}

	auto commonSk = mScene.getSkeleton("common.sk");
// mirror all arm and hand motions
	std::string preFix;
	std::vector<std::string> rightHandMotions;
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachRtHigh");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachRtMidHigh");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachRtMidLow");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachLfLow");

	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachLfHigh");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachLfMidHigh");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachRtMidLow");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachRtLow");

	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachMiddleHigh");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachMiddleMidHigh");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachMiddleMidLow");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachMiddleLow");

	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachClose_Lf");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachClose_Rt");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachClose_MiddleHigh");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachClose_MiddleLow");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachClose_MiddleMidHigh");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachClose_MiddleMidLow");

	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachBehind_High1");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachBehind_High2");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachBehind_Low1");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachBehind_Low2");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachBehind_MidHigh1");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachBehind_MidHigh2");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachBehind_MidLow1");
	rightHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_ArmReachBehind_MidLow2");

	rightHandMotions.emplace_back("ChrHarmony_Relax001_HandGraspSmSphere_Grasp");
	rightHandMotions.emplace_back("ChrHarmony_Relax001_HandGraspSmSphere_Reach");
	rightHandMotions.emplace_back("ChrHarmony_Relax001_HandGraspSmSphere_Release");

	std::vector<std::string> leftHandMotions;
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachRtHigh");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachRtMidHigh");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachRtMidLow");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachLfLow");

	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachLfHigh");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachLfMidHigh");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachRtMidLow");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachRtLow");

	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachMiddleHigh");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachMiddleMidHigh");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachMiddleMidLow");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachMiddleLow");

	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachClose_Lf");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachClose_Rt");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachClose_MiddleHigh");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachClose_MiddleLow");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachClose_MiddleMidHigh");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachClose_MiddleMidLow");

	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachBehind_High1");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachBehind_High2");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachBehind_Low1");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachBehind_Low2");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachBehind_MidHigh1");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachBehind_MidHigh2");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachBehind_MidLow1");
	leftHandMotions.emplace_back(preFix + "ChrHarmony_Relax001_LArmReachBehind_MidLow2");

	leftHandMotions.emplace_back("ChrHarmony_Relax001_LHandGraspSmSphere_Grasp");
	leftHandMotions.emplace_back("ChrHarmony_Relax001_LHandGraspSmSphere_Reach");
	leftHandMotions.emplace_back("ChrHarmony_Relax001_LHandGraspSmSphere_Release");

	for (auto& motionName : rightHandMotions) {
		auto motion = mScene.getMotion(motionName);
		if (!motion) {
// assetManager.loadAsset(motionPath+rightHandMotions[i]+".skm");
			assetStore.loadAsset(motionPath + motionName + ".skmb");
		}
		motion = mScene.getMotion(motionName);
// print "motionName = " + locoMotions[i]
		if (motion) {
			motion->setMotionSkeletonName("common.sk");
		}
		auto mirrorMotion1 = mScene.getMotion(motionName);
		mirrorMotion1->mirror(motionName, "common.sk");

	}
}

void BasicCharacter::retargetBehaviorSetReaching(const std::string& charName) {
	std::vector<std::string> reachMotions;
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachRtHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachRtMidHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachRtMidLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachLfLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachLfHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachLfMidHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachRtMidLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachRtLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachMiddleHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachMiddleMidHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachMiddleMidLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachMiddleLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachClose_Lf");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachClose_Rt");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachClose_MiddleHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachClose_MiddleLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachClose_MiddleMidHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachClose_MiddleMidLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachBehind_High1");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachBehind_High2");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachBehind_Low1");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachBehind_Low2");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachBehind_MidHigh1");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachBehind_MidHigh2");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachBehind_MidLow1");
	reachMotions.emplace_back("ChrHarmony_Relax001_ArmReachBehind_MidLow2");

	reachMotions.emplace_back("ChrHarmony_Relax001_HandGraspSmSphere_Grasp");
	reachMotions.emplace_back("ChrHarmony_Relax001_HandGraspSmSphere_Reach");
	reachMotions.emplace_back("ChrHarmony_Relax001_HandGraspSmSphere_Release");
	reachMotions.emplace_back("HandsAtSide_RArm_GestureYou");

	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachRtHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachRtMidHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachRtMidLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachLfLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachLfHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachLfMidHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachRtMidLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachRtLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachMiddleHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachMiddleMidHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachMiddleMidLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachMiddleLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachClose_Lf");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachClose_Rt");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachClose_MiddleHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachClose_MiddleLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachClose_MiddleMidHigh");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachClose_MiddleMidLow");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachBehind_High1");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachBehind_High2");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachBehind_Low1");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachBehind_Low2");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachBehind_MidHigh1");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachBehind_MidHigh2");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachBehind_MidLow1");
	reachMotions.emplace_back("ChrHarmony_Relax001_LArmReachBehind_MidLow2");

	reachMotions.emplace_back("ChrHarmony_Relax001_LHandGraspSmSphere_Grasp");
	reachMotions.emplace_back("ChrHarmony_Relax001_LHandGraspSmSphere_Reach");
	reachMotions.emplace_back("ChrHarmony_Relax001_LHandGraspSmSphere_Release");
	reachMotions.emplace_back("HandsAtSide_LArm_GestureYou");

	auto assetManager = mScene.getAssetManager();
	for (auto& motionName : reachMotions) {
		auto sbMotion = assetManager->getMotion(motionName);
		if (sbMotion) {
			sbMotion->setMotionSkeletonName("common.sk");
		}
	}

	auto sbChar = mScene.getCharacter(charName);
	if (!sbChar) {
		return;
	}
	auto skelName = sbChar->getSkeleton()->getName();
	createRetargetInstance(mScene, "common.sk", skelName);

	//TODO: convert reach code below to C++
	//mScene.run("init-reach.py");
	//reachSetup(charName, "KNN", "common.sk", "");

}
