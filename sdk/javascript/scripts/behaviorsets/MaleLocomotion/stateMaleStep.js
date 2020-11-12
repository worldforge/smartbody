var stateMaleStep = {
	stepSetup : function (origSkelName, skeletonName, baseJoint, preFix, statePreFix){
		var stateManager = scene.getStateManager()
		console.log("** State: allStep")
		var stateName = statePreFix+"Step"
		if (stateManager.getBlend(stateName) != null)// don't create duplicate state
			return				
		var state = stateManager.createState2D(stateName)
		state.setBlendSkeleton(origSkelName)
		// add motions
		var originalMotions = new Module.StringVec()
		originalMotions.push_back("ChrUtah_Idle001")
		originalMotions.push_back("ChrUtah_Idle01_StepBackwardRt01")
		originalMotions.push_back("ChrUtah_Idle01_StepForwardRt01")
		originalMotions.push_back("ChrUtah_Idle01_StepSidewaysRt01")
		originalMotions.push_back("ChrUtah_Idle01_StepBackwardLf01")
		originalMotions.push_back("ChrUtah_Idle01_StepForwardLf01")
		originalMotions.push_back("ChrUtah_Idle01_StepSidewaysLf01")
		
		var motions = new Module.StringVec()
		var assetManager = scene.getAssetManager()
		for(var i = 0; i < originalMotions.size(); ++i){
			motions.push_back(preFix + originalMotions.get(i))
			var sbMotion = assetManager.getMotion(originalMotions.get(i))
			if(sbMotion != null)
				sbMotion.setMotionSkeletonName(origSkelName)
		}
		for(var i = 0; i < motions.size(); ++i)
			state.addMotion(motions.get(i), 0, 0)

		// add correspondance points
		var points1 = new Module.DoubleVec()
		points1.push_back(0)
		points1.push_back(0)
		points1.push_back(0)
		points1.push_back(0)
		points1.push_back(0)
		points1.push_back(0)
		points1.push_back(0)
		state.addCorrespondancePoints(motions, points1)
		
		var points2 = new Module.DoubleVec()
		points2.push_back(0.556322)
		points2.push_back(0.556322)
		points2.push_back(0.543678)
		points2.push_back(0.482989)
		points2.push_back(0.395402)
		points2.push_back(0.531034)
		points2.push_back(0.473563)
		state.addCorrespondancePoints(motions, points2)
		
		var points3 = new Module.DoubleVec()
		points3.push_back(1.46414)
		points3.push_back(1.46414)
		points3.push_back(1.46414)
		points3.push_back(1.46414)
		points3.push_back(1.33333)
		points3.push_back(1.33333)
		points3.push_back(1.33103)
		state.addCorrespondancePoints(motions, points3)

		// reset parameters (because it needs context of correspondance points)
		var skeleton = scene.getSkeleton(skeletonName)
		var joint = skeleton.getJointByName(baseJoint)
		for(var i = 0; i < motions.size(); ++i){
			//print 'motion = ' + motions[i]
			var motion = scene.getMotion(motions.get(i))
			motion.connect(skeleton)
			var correspondancePoints = state.getCorrespondancePoints(i)
			var lenCorrespondancePoints = correspondancePoints.size()
			var jointTransition = motion.getJointTransition(joint, correspondancePoints.get(0), correspondancePoints.get(lenCorrespondancePoints - 1))
			state.setParameter(motions.get(i), jointTransition.get(0), jointTransition.get(2))
			motion.disconnect()
		}
		// add triangles
		state.addTriangle(preFix+"ChrUtah_Idle001", preFix+"ChrUtah_Idle01_StepBackwardLf01", preFix+"ChrUtah_Idle01_StepSidewaysLf01")
		state.addTriangle(preFix+"ChrUtah_Idle001", preFix+"ChrUtah_Idle01_StepForwardLf01", preFix+"ChrUtah_Idle01_StepSidewaysLf01")
		state.addTriangle(preFix+"ChrUtah_Idle001", preFix+"ChrUtah_Idle01_StepBackwardRt01", preFix+"ChrUtah_Idle01_StepSidewaysRt01")
		state.addTriangle(preFix+"ChrUtah_Idle001", preFix+"ChrUtah_Idle01_StepForwardRt01", preFix+"ChrUtah_Idle01_StepSidewaysRt01")
		
	}
	
	
}