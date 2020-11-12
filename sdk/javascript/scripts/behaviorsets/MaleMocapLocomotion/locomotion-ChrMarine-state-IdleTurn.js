var locomotionChrMarineStateIdleTurn = {
	mocapIdleTurnSetup : function(skeletonName, baseJoint, preFix, statePreFix){
		var mirrorMotion = scene.getMotion("ChrMarine@Turn90Rt01")
		mirrorMotion.mirror("ChrMarine@Turn90Lf01", "ChrBackovic.sk")
		var mirrorMotion1 = scene.getMotion("ChrMarine@Turn180Rt01")
		mirrorMotion1.mirror("ChrMarine@Turn180Lf01", "ChrBackovic.sk")


		var stateManager = scene.getStateManager()

		//print "** State: ChrMarineIdleTurn"
		var state = stateManager.createState1D("mocapIdleTurn")
		state.setBlendSkeleton('ChrBackovic.sk')
		var motions = new Module.StringVec()
		motions.push_back("ChrMarine@Idle01")
		motions.push_back("ChrMarine@Turn90Lf01")
		motions.push_back("ChrMarine@Turn180Lf01")
		motions.push_back("ChrMarine@Turn90Rt01")
		motions.push_back("ChrMarine@Turn180Rt01")
		var params = new Module.DoubleVec()
		params.push_back(0)
		params.push_back(-90)
		params.push_back(-180)
		params.push_back(90)
		params.push_back(180)

		for(var i = 0; i < motions.size(); ++i)
			state.addMotion(motions.get(i), params.get(i))

		var points1 = new Module.DoubleVec()
		points1.push_back(0)
		points1.push_back(0)
		points1.push_back(0)
		points1.push_back(0)
		points1.push_back(0)
		state.addCorrespondancePoints(motions, points1)
		var points2 = new Module.DoubleVec()
		points2.push_back(0.255738)
		points2.push_back(0.762295)
		points2.push_back(0.87541)
		points2.push_back(0.757377)
		points2.push_back(0.821311)
		state.addCorrespondancePoints(motions, points2)
		var points3 = new Module.DoubleVec()
		points3.push_back(2.17)
		points3.push_back(2.13)
		points3.push_back(1.73)
		points3.push_back(2.13)
		points3.push_back(1.73)
		state.addCorrespondancePoints(motions, points3)
	}
}

