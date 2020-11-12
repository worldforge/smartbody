var stateAllStarting = {
	startingSetup : function(skeletonName, baseJoint, preFix, statePreFix){
		var mirrorMotion = scene.getMotion(preFix+"ChrUtah_StopToWalkRt01")
		mirrorMotion.mirror(preFix+"ChrUtah_StopToWalkLf01", skeletonName)
		
		var mirrorMotion1 = scene.getMotion(preFix+"ChrUtah_Idle01_ToWalk01_Turn90Lf01")
		mirrorMotion1.mirror(preFix+"ChrUtah_Idle01_ToWalk01_Turn90Rt01", skeletonName)
		var mirrorMotion2 = scene.getMotion(preFix+"ChrUtah_Idle01_ToWalk01_Turn180Lf01")
		mirrorMotion2.mirror(preFix+"ChrUtah_Idle01_ToWalk01_Turn180Rt01", skeletonName)

		var stateManager = scene.getStateManager()

		console.log( "** State: allStartingLeft")
		var startLefStateName = statePreFix+"StartingLeft"
		if (stateManager.getBlend(startLefStateName) == null){ // don't create duplicate state		
			var state1 = stateManager.createState1D(statePreFix+"StartingLeft")
			var motions1 = new Module.StringVec()
			motions1.push_back(preFix+"ChrUtah_StopToWalkLf01")
			motions1.push_back(preFix+"ChrUtah_Idle01_ToWalk01_Turn90Lf01")
			motions1.push_back(preFix+"ChrUtah_Idle01_ToWalk01_Turn180Lf01")
			var params1 = new Module.DoubleVec()
			params1.push_back(0)
			params1.push_back(-90)
			params1.push_back(-180)

			for(var i = 0; i < motions1.size(); ++i) 
				state1.addMotion(motions1.get(i), params1.get(i))

			var points1 = new Module.DoubleVec()
			points1.push_back(0)
			points1.push_back(0)
			points1.push_back(0)
			state1.addCorrespondancePoints(motions1, points1)
			var points2 = new Module.DoubleVec()
			points2.push_back(0.57541)
			points2.push_back(1.2)
			points2.push_back(1.35)
			state1.addCorrespondancePoints(motions1, points2)
			var points3 = new Module.DoubleVec()
			points3.push_back(0.943716)
			points3.push_back(1.41)
			points3.push_back(1.6)
			state1.addCorrespondancePoints(motions1, points3)
		}
		console.log( "** State: allStartingRight")
		
		var startRightStateName = statePreFix+"StartingRight"
		if (stateManager.getBlend(startRightStateName) == null){
			var state2 = stateManager.createState1D(statePreFix+"StartingRight")
			var motions2 = new Module.StringVec()
			motions2.push_back(preFix+"ChrUtah_StopToWalkRt01")
			motions2.push_back(preFix+"ChrUtah_Idle01_ToWalk01_Turn90Rt01")
			motions2.push_back(preFix+"ChrUtah_Idle01_ToWalk01_Turn180Rt01")
			var params2 = new Module.DoubleVec()
			params2.push_back(0)
			params2.push_back(90)
			params2.push_back(180)

			for(var i = 0; i < motions2.size(); ++i)
				state2.addMotion(motions2.get(i), params2.get(i))

			// Since the right is symetric with the left, so the correspondance points are the same
			state2.addCorrespondancePoints(motions2, points1)
			state2.addCorrespondancePoints(motions2, points2)
			state2.addCorrespondancePoints(motions2, points3)
		}
			
	}
}
