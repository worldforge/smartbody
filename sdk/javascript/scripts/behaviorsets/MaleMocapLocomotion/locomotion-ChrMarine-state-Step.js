var locomotionChrMarineStateStep = {
	mocapStepSetup : function(skeletonName, baseJoint, preFix, statePreFix){
		var stateManager = scene.getStateManager()
		var stateChrMarineStep = stateManager.createState2D("mocapStep")
		stateChrMarineStep.setBlendSkeleton('ChrBackovic.sk')
		
		var motions = new Module.StringVec()
		motions.push_back("ChrMarine@Idle01")
		motions.push_back("ChrMarine@Idle01_StepBackwardsLf01")
		motions.push_back("ChrMarine@Idle01_StepBackwardsRt01")
		motions.push_back("ChrMarine@Idle01_StepForwardLf01")
		motions.push_back("ChrMarine@Idle01_StepForwardRt01")
		motions.push_back("ChrMarine@Idle01_StepSidewaysLf01")
		motions.push_back("ChrMarine@Idle01_StepSidewaysRt01")

		var paramsX = new Module.DoubleVec()
		var paramsY = new Module.DoubleVec()
		paramsX.push_back(8.31458e-005) // ChrMarine@Idle01 X
		paramsY.push_back(-6.8546e-005) // ChrMarine@Idle01 Y
		paramsX.push_back(0.0) // ChrMarine@Idle01_StepBackwardsLf01 X
		paramsY.push_back(-0.349858) // ChrMarine@Idle01_StepBackwardsLf01 Y
		paramsX.push_back(0.0) // ChrMarine@Idle01_StepBackwardsRt01 X
		paramsY.push_back(-0.349858) // ChrMarine@Idle01_StepBackwardsRt01 Y
		paramsX.push_back(-0.0225843) // ChrMarine@Idle01_StepForwardLf01 X
		paramsY.push_back(0.535643) // ChrMarine@Idle01_StepForwardLf01 Y
		paramsX.push_back(0.0234374) // ChrMarine@Idle01_StepForwardRt01 X
		paramsY.push_back(0.535643) // ChrMarine@Idle01_StepForwardRt01 Y
		paramsX.push_back(-0.234709) // ChrMarine@Idle01_StepSidewaysLf01 X
		paramsY.push_back(-0.010002) // ChrMarine@Idle01_StepSidewaysLf01 Y
		paramsX.push_back(0.234693) // ChrMarine@Idle01_StepSidewaysRt01 X
		paramsY.push_back(-0.010002) // ChrMarine@Idle01_StepSidewaysRt01 Y
		for(var i = 0; i < motions.size(); ++i)
			stateChrMarineStep.addMotion(motions.get(i), paramsX.get(i), paramsY.get(i))

		var points0 = new Module.DoubleVec()
		points0.push_back(0) // ChrMarine@Idle01 0
		points0.push_back(0) // ChrMarine@Idle01_StepBackwardsLf01 0
		points0.push_back(0) // ChrMarine@Idle01_StepBackwardsRt01 0
		points0.push_back(0) // ChrMarine@Idle01_StepForwardLf01 0
		points0.push_back(0) // ChrMarine@Idle01_StepForwardRt01 0
		points0.push_back(0) // ChrMarine@Idle01_StepSidewaysLf01 0
		points0.push_back(0) // ChrMarine@Idle01_StepSidewaysRt01 0
		stateChrMarineStep.addCorrespondencePoints(motions, points0)
		var points1 = new Module.DoubleVec()
		points1.push_back(1.1153) // ChrMarine@Idle01 1
		points1.push_back(0.68) // ChrMarine@Idle01_StepBackwardsLf01 1
		points1.push_back(0.68) // ChrMarine@Idle01_StepBackwardsRt01 1
		points1.push_back(1.02258) // ChrMarine@Idle01_StepForwardLf01 1
		points1.push_back(1.0303) // ChrMarine@Idle01_StepForwardRt01 1
		points1.push_back(0.821667) // ChrMarine@Idle01_StepSidewaysLf01 1
		points1.push_back(0.824242) // ChrMarine@Idle01_StepSidewaysRt01 1
		stateChrMarineStep.addCorrespondencePoints(motions, points1)
		var points2 = new Module.DoubleVec()
		points2.push_back(2.165) // ChrMarine@Idle01 2
		points2.push_back(1.32) // ChrMarine@Idle01_StepBackwardsLf01 2
		points2.push_back(1.32) // ChrMarine@Idle01_StepBackwardsRt01 2
		points2.push_back(1.985) // ChrMarine@Idle01_StepForwardLf01 2
		points2.push_back(2) // ChrMarine@Idle01_StepForwardRt01 2
		points2.push_back(1.595) // ChrMarine@Idle01_StepSidewaysLf01 2
		points2.push_back(1.6) // ChrMarine@Idle01_StepSidewaysRt01 2
		stateChrMarineStep.addCorrespondencePoints(motions, points2)

		stateChrMarineStep.addTriangle("ChrMarine@Idle01", "ChrMarine@Idle01_StepBackwardsLf01", "ChrMarine@Idle01_StepSidewaysLf01")
		stateChrMarineStep.addTriangle("ChrMarine@Idle01", "ChrMarine@Idle01_StepForwardLf01", "ChrMarine@Idle01_StepSidewaysLf01")
		stateChrMarineStep.addTriangle("ChrMarine@Idle01", "ChrMarine@Idle01_StepBackwardsRt01", "ChrMarine@Idle01_StepSidewaysRt01")
		stateChrMarineStep.addTriangle("ChrMarine@Idle01", "ChrMarine@Idle01_StepForwardRt01", "ChrMarine@Idle01_StepSidewaysRt01")

		
	}
}
