var stateMaleIdleTurn = {
	idleTurnSetup : function (origSkelName, skeletonName, baseJoint, preFix, statePreFix){
		console.log("** State: allIdleTurn")
		var stateManager = scene.getStateManager()	
		var stateName = statePreFix+"IdleTurn"
		if (stateManager.getBlend(stateName) != null)// don't create duplicate state
			return	
		var state = stateManager.createState1D(stateName)
		state.setBlendSkeleton(origSkelName)
		var originalMotions = new Module.StringVec()
		originalMotions.push_back("ChrUtah_Idle001")
		originalMotions.push_back("ChrUtah_Turn90Lf01")
		originalMotions.push_back("ChrUtah_Turn180Lf01")
		originalMotions.push_back("ChrUtah_Turn90Rt01")
		originalMotions.push_back("ChrUtah_Turn180Rt01")
		var motions = new Module.StringVec()
		var assetManager = scene.getAssetManager()
		for(var i = 0; i < originalMotions.size(); ++i){
			motions.push_back(preFix + originalMotions.get(i))
			var sbMotion = assetManager.getMotion(originalMotions.get(i))
			if(sbMotion != null)
				sbMotion.setMotionSkeletonName(origSkelName)
		}			
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
		points3.push_back(0.633333)
		points3.push_back(1.96667)
		points3.push_back(2.46667)
		points3.push_back(1.96667)
		points3.push_back(2.46667)
		state.addCorrespondancePoints(motions, points3)
	}
}
