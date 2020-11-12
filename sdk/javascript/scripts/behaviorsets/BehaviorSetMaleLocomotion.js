var BehaviorSetMaleLocomotion = {
	setupBehaviorSet: function (){
		console.log("Setting up behavior set for MaleLocomotion...")
		//scene.loadAssetsFromPath("behaviorsets/MaleLocomotion/skeletons")
		//scene.loadAssetsFromPath("behaviorsets/MaleLocomotion/motions")
		//scene.addAssetPath("script", "behaviorsets/MaleLocomotion/scripts")
		
		var assetManager = scene.getAssetManager()	
		motionPath = "behaviorsets/MaleLocomotion/motions/"
		var skel = scene.getSkeleton("test_utah.sk")
		if(skel == null)
			scene.loadAssetsFromPath("behaviorsets/MaleLocomotion/skeletons")
		
		var locoMotions = new Module.StringVec()
		locoMotions.push_back("ChrUtah_Walk001")
		locoMotions.push_back("ChrUtah_Idle001")
		locoMotions.push_back("ChrUtah_Idle01_ToWalk01_Turn360Lf01")
		locoMotions.push_back("ChrUtah_Idle01_ToWalk01_Turn360Rt01")
		locoMotions.push_back("ChrUtah_Meander01")
		locoMotions.push_back("ChrUtah_Shuffle01")
		locoMotions.push_back("ChrUtah_Jog001")
		locoMotions.push_back("ChrUtah_Run001")
		locoMotions.push_back("ChrUtah_WalkInCircleLeft001")
		locoMotions.push_back("ChrUtah_WalkInCircleRight001")
		locoMotions.push_back("ChrUtah_WalkInTightCircleLeft001")
		locoMotions.push_back("ChrUtah_WalkInTightCircleRight001")
		locoMotions.push_back("ChrUtah_RunInCircleLeft001")
		locoMotions.push_back("ChrUtah_RunInCircleRight001")
		locoMotions.push_back("ChrUtah_RunInTightCircleLeft01")
		locoMotions.push_back("ChrUtah_RunInTightCircleRight01")
		locoMotions.push_back("ChrUtah_StrafeSlowRt01")
		locoMotions.push_back("ChrUtah_StrafeSlowLf01")
		locoMotions.push_back("ChrUtah_StrafeFastRt01")
		locoMotions.push_back("ChrUtah_StrafeFastLf01")
		locoMotions.push_back("ChrUtah_Idle001")
		locoMotions.push_back("ChrUtah_Turn90Lf01")
		locoMotions.push_back("ChrUtah_Turn180Lf01")
		locoMotions.push_back("ChrUtah_Turn90Rt01")
		locoMotions.push_back("ChrUtah_Turn180Rt01")
		locoMotions.push_back("ChrUtah_StopToWalkRt01")
		locoMotions.push_back("ChrUtah_Idle01_ToWalk01_Turn90Lf01")
		locoMotions.push_back("ChrUtah_Idle01_ToWalk01_Turn180Lf01")
		locoMotions.push_back("ChrUtah_Idle01_StepBackwardRt01")
		locoMotions.push_back("ChrUtah_Idle01_StepForwardRt01")
		locoMotions.push_back("ChrUtah_Idle01_StepSidewaysRt01")
		locoMotions.push_back("ChrUtah_Idle01_StepBackwardLf01")
		locoMotions.push_back("ChrUtah_Idle01_StepForwardLf01")
		locoMotions.push_back("ChrUtah_Idle01_StepSidewaysLf01")
		
		for(var i = 0; i < locoMotions.size(); ++i){
			var motion = scene.getMotion(locoMotions.get(i))
			if(motion == null){
				assetManager.loadAsset(motionPath+locoMotions.get(i)+'.skm')
				motion = scene.getMotion(locoMotions.get(i))
			}
			//console.log('motionName = ' + locoMotions.get(i));
			if(motion != null){
				motion.setMotionSkeletonName('test_utah.sk')
				motion.buildJointTrajectory('l_forefoot','base')
				motion.buildJointTrajectory('r_forefoot','base')
				motion.buildJointTrajectory('l_ankle','base')
				motion.buildJointTrajectory('r_ankle','base')
			}
		}
	},
	
	retargetBehaviorSet: function (charName){	
		//outDir = scene.getMediaPath() + '/retarget/motion/' + skelName + '/';
		//if not os.path.exists(outDir):
		//	os.makedirs(outDir)
			
		// retarget standard locomotions
		//for n in range(0, len(locoMotions)):
		//	curMotion = scene.getMotion(locoMotions[n])
		//	if curMotion is not None:
		//		retargetMotion(locoMotions[n], 'test_utah.sk', skelName, outDir + 'MaleLocomotion/');
		var sbChar = scene.getCharacter(charName)
		if(sbChar == null)
			return
		var skelName = sbChar.getSkeleton().getName()

		BehaviorSetCommon.createRetargetInstance('test_utah.sk', skelName)
		
		// setup standard locomotion
		stateMaleLocomotion.locomotionSetup('test_utah.sk', 'test_utah.sk', "base", '', 'all')
		
		// starting state, starting locomotion with different angle
		stateMaleStarting.startingSetup('test_utah.sk', 'test_utah.sk', "base", '', 'all')
		
		// idle turn state, facing adjusting
		stateMaleIdleTurn.idleTurnSetup('test_utah.sk', 'test_utah.sk', "base", '', 'all')

		// step state, stepping adjusting
		stateMaleStep.stepSetup('test_utah.sk', 'test_utah.sk', "base", '', 'all')
		
		// transitions
		transitions.transitionSetup('', 'all')
			
		// add IK constraint for foot automatically
		sbChar.addJointTrajectoryConstraint('l_forefoot','base')
		sbChar.addJointTrajectoryConstraint('r_forefoot','base')
		sbChar.addJointTrajectoryConstraint('l_ankle','base')
		sbChar.addJointTrajectoryConstraint('r_ankle','base')	
		
		// setup steering
		var steerManager = scene.getSteerManager()
		steerManager.setEnable(false)
		initSteerAgents.setupSteerAgent(charName, 'all')	
		steerManager.setEnable(true)
	}
}

