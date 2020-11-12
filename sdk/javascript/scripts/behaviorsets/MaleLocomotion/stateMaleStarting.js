 var stateMaleStarting = {
	startingSetup : function (origSkelName, skeletonName, baseJoint, preFix, statePreFix){
	
		var mirrorMotion = scene.getMotion("ChrUtah_StopToWalkRt01")
		mirrorMotion.mirror("ChrUtah_StopToWalkLf01", skeletonName)
		
		var mirrorMotion1 = scene.getMotion("ChrUtah_Idle01_ToWalk01_Turn90Lf01")
		mirrorMotion1.mirror("ChrUtah_Idle01_ToWalk01_Turn90Rt01", skeletonName)
		var mirrorMotion2 = scene.getMotion("ChrUtah_Idle01_ToWalk01_Turn180Lf01")
		mirrorMotion2.mirror("ChrUtah_Idle01_ToWalk01_Turn180Rt01", skeletonName)

		var originalMotions1 = new Module.StringVec()
		var originalMotions2 = new Module.StringVec()
		originalMotions1.push_back("ChrUtah_StopToWalkLf01")
		originalMotions1.push_back("ChrUtah_Idle01_ToWalk01_Turn90Lf01")
		originalMotions1.push_back("ChrUtah_Idle01_ToWalk01_Turn180Lf01")
		originalMotions2.push_back("ChrUtah_StopToWalkRt01")
		originalMotions2.push_back("ChrUtah_Idle01_ToWalk01_Turn90Rt01")
		originalMotions2.push_back("ChrUtah_Idle01_ToWalk01_Turn180Rt01")
		
		var motions1 = new StringVec()
		var motions2 = new StringVec()
		var assetManager = scene.getAssetManager()
		for(var i = 0; i < originalMotions1.size(); ++i){
			motions1.push_back(preFix + originalMotions1.get(i))
			motions2.push_back(preFix + originalMotions2.get(i))
			var sbMotion1 = assetManager.getMotion(originalMotions1.get(i))
			if(sbMotion1 != null){
				sbMotion1.setMotionSkeletonName(origSkelName)
				sbMotion1.buildJointTrajectory('l_forefoot','base')
				sbMotion1.buildJointTrajectory('r_forefoot','base')
				sbMotion1.buildJointTrajectory('l_ankle','base')
				sbMotion1.buildJointTrajectory('r_ankle','base')
			}
			var sbMotion2 = assetManager.getMotion(originalMotions2.get(i))
			if(sbMotion2 != null){
				sbMotion2.setMotionSkeletonName(origSkelName)
				sbMotion2.buildJointTrajectory('l_forefoot','base')
				sbMotion2.buildJointTrajectory('r_forefoot','base')
				sbMotion2.buildJointTrajectory('l_ankle','base')
				sbMotion2.buildJointTrajectory('r_ankle','base')
			}
		}	
		var stateManager = scene.getStateManager()

		console.log("** State: allStartingLeft")
		
		
		var startLefStateName = statePreFix+"StartingLeft"
		if (stateManager.getBlend(startLefStateName) == null){// don't create duplicate state		
			var state1 = stateManager.createState1D(statePreFix+"StartingLeft")
			state1.setBlendSkeleton(origSkelName)
			
			// motions1.push_back("ChrUtah_StopToWalkLf01")
			// motions1.push_back("ChrUtah_Idle01_ToWalk01_Turn90Lf01")
			// motions1.push_back("ChrUtah_Idle01_ToWalk01_Turn180Lf01")
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
		console.log("** State: allStartingRight")
		
		startRightStateName = statePreFix+"StartingRight"
		if (stateManager.getBlend(startRightStateName) == null){
			var state2 = stateManager.createState1D(statePreFix+"StartingRight")
			state2.setBlendSkeleton(origSkelName)
			
			// motions2.push_back("ChrUtah_StopToWalkRt01")
			// motions2.push_back("ChrUtah_Idle01_ToWalk01_Turn90Rt01")
			// motions2.push_back("ChrUtah_Idle01_ToWalk01_Turn180Rt01")
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
 