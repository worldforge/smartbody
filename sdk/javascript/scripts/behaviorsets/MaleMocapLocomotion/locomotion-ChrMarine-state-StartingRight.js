var locomotionChrMarineStateStartingRight = {
	mocapStartRightSetup : function(skeletonName, baseJoint, preFix, statePreFix){
		var stateManager = scene.getStateManager()

		var stateChrMarineStartingRight = stateManager.createState1D("mocapStartingRight")
		stateChrMarineStartingRight.setBlendSkeleton('ChrBackovic.sk')
		var motions = new Module.StringVec()
		motions.push_back("ChrMarine@Idle01_ToWalkLf01")
		motions.push_back("ChrMarine@Idle01_ToWalk01_Turn90Rt01")
		motions.push_back("ChrMarine@Idle01_ToWalk01_Turn180Rt01")

		var paramsX = new Module.DoubleVec()
		paramsX.push_back(0) // ChrMarine@Idle01 X
		paramsX.push_back(90) // ChrMarine@Idle01_ToWalk01_Turn90Rt01 X
		paramsX.push_back(165) // ChrMarine@Idle01_ToWalk01_Turn180Rt01 X
		for(var i = 0; i < motions.size(); ++i)
			stateChrMarineStartingRight.addMotion(motions.get(i), paramsX.get(i))

		var points0 = new Module.DoubleVec()
		points0.push_back(0) // ChrBrad_ChrMarine@Idle01 0
		points0.push_back(0) // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 0
		points0.push_back(0) // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 0
		stateChrMarineStartingRight.addCorrespondencePoints(motions, points0)
		var points1 = new Module.DoubleVec()
		points1.push_back(0.73) // ChrBrad_ChrMarine@Idle01 1
		points1.push_back(1.42) // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 1
		points1.push_back(1.37) // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 1
		stateChrMarineStartingRight.addCorrespondencePoints(motions, points1)
		var points2 = new Module.DoubleVec()
		points2.push_back(1.32) // ChrBrad_ChrMarine@Idle01 2
		points2.push_back(2.08) // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 2
		points2.push_back(2.06) // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 2
		stateChrMarineStartingRight.addCorrespondencePoints(motions, points2)
		var points3 = new Module.DoubleVec()
		points3.push_back(1.56) // ChrBrad_ChrMarine@Idle01 2
		points3.push_back(2.43) // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 2
		points3.push_back(2.46) // ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 2
		stateChrMarineStartingRight.addCorrespondencePoints(motions, points3)
	}
}

