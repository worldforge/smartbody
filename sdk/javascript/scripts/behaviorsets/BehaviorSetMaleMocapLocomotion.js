var BehaviorSetMaleMocapLocomotion = {
	setupBehaviorSet : function(){
		console.log("Setting up behavior set for MaleMocapLocomotion...")
		//scene.loadAssetsFromPath("behaviorsets/MaleMocapLocomotion/skeletons")
		//scene.loadAssetsFromPath("behaviorsets/MaleMocapLocomotion/motions")
		var assetManager = scene.getAssetManager()
		//scene.addAssetPath("script", "behaviorsets/MaleMocapLocomotion/scripts")
		var motionPath = "behaviorsets/MaleMocapLocomotion/motions/"
		var skel = scene.getSkeleton("ChrBackovic.sk")
		if (skel == null)
			scene.loadAssetsFromPath("behaviorsets/MaleMocapLocomotion/skeletons")
			
		
		// map the zebra2 skeleton
		zebra2map.zebra2Map();
		var zebra2Map = scene.getJointMapManager().getJointMap("zebra2")
		var bradSkeleton = scene.getSkeleton("ChrBackovic.sk")
		zebra2Map.applySkeleton(bradSkeleton)	
		var locoMotions = new Module.StringVec()
		locoMotions.push_back("ChrMarine@Idle01")
		locoMotions.push_back("ChrMarine@Jog01")
		locoMotions.push_back("ChrMarine@Meander01")
		locoMotions.push_back("ChrMarine@Run01")
		//locoMotions.push_back("ChrMarine@RunCircleLf01")
		locoMotions.push_back("ChrMarine@RunCircleRt01")
		//locoMotions.push_back("ChrMarine@RunTightCircleLf01")
		locoMotions.push_back("ChrMarine@RunTightCircleRt01")
		//locoMotions.push_back("ChrMarine@StrafeSlowLf01")
		locoMotions.push_back("ChrMarine@StrafeSlowRt01")
		locoMotions.push_back("ChrMarine@Walk01")
		//locoMotions.push_back("ChrMarine@WalkCircleLf01")
		locoMotions.push_back("ChrMarine@WalkCircleRt01")
		//locoMotions.push_back("ChrMarine@WalkTightCircleLf01")
		locoMotions.push_back("ChrMarine@WalkTightCircleRt01")
		//locoMotions.push_back("ChrMarine@Turn360Lf01")
		locoMotions.push_back("ChrMarine@Turn360Rt01")
		//locoMotions.push_back("ChrMarine@StrafeFastLf01")
		locoMotions.push_back("ChrMarine@StrafeFastRt01")
		//locoMotions.push_back("ChrMarine@Idle01_StepBackwardsLf01")
		locoMotions.push_back("ChrMarine@Idle01_StepBackwardsRt01")
		//locoMotions.push_back("ChrMarine@Idle01_StepForwardLf01")
		locoMotions.push_back("ChrMarine@Idle01_StepForwardRt01")
		//locoMotions.push_back("ChrMarine@Idle01_StepSidewaysLf01")
		locoMotions.push_back("ChrMarine@Idle01_StepSidewaysRt01")
		//locoMotions.push_back("ChrMarine@Turn90Lf01")	
		//locoMotions.push_back("ChrMarine@Turn180Lf01")
		locoMotions.push_back("ChrMarine@Turn90Rt01")
		locoMotions.push_back("ChrMarine@Turn180Rt01")
		//locoMotions.push_back("ChrMarine@Idle01_ToWalkLf01")
		//locoMotions.push_back("ChrMarine@Idle01_ToWalk01_Turn90Lf01")
		//locoMotions.push_back("ChrMarine@Idle01_ToWalk01_Turn180Lf01")
		locoMotions.push_back("ChrMarine@Idle01_ToWalk01")
		locoMotions.push_back("ChrMarine@Idle01_ToWalk01_Turn90Rt01")
		locoMotions.push_back("ChrMarine@Idle01_ToWalk01_Turn180Rt01")
		
		
		for(var i = 0; i < locoMotions.size(); ++i){
			var motion = scene.getMotion(locoMotions.get(i))
			if (motion == null){
				assetManager.loadAsset(motionPath+locoMotions.get(i)+'.skm')
				motion = scene.getMotion(locoMotions.get(i))
			}
			//print 'motionName = ' + locoMotions[i]
			if (motion != null){
				motion.setMotionSkeletonName('ChrBackovic.sk')
				zebra2Map.applyMotion(motion)
				motion.buildJointTrajectory('l_forefoot','base')
				motion.buildJointTrajectory('r_forefoot','base')
				motion.buildJointTrajectory('l_ankle','base')
				motion.buildJointTrajectory('r_ankle','base')
			}
		}
			
	},
	
	retargetBehaviorSet : function(charName){
		locomotionChrMarineInit.locomotionChrMarineInit()
	
		var locoMotions = new StringVec()
		locoMotions.push_back("ChrMarine@Idle01")
		locoMotions.push_back("ChrMarine@Jog01")
		locoMotions.push_back("ChrMarine@Meander01")
		locoMotions.push_back("ChrMarine@Run01")
		locoMotions.push_back("ChrMarine@RunCircleLf01_smooth")
		locoMotions.push_back("ChrMarine@RunCircleRt01_smooth")
		locoMotions.push_back("ChrMarine@RunTightCircleLf01")
		locoMotions.push_back("ChrMarine@RunTightCircleRt01")
		locoMotions.push_back("ChrMarine@StrafeSlowLf01")
		locoMotions.push_back("ChrMarine@StrafeSlowRt01")
		locoMotions.push_back("ChrMarine@Walk01")
		locoMotions.push_back("ChrMarine@WalkCircleLf01_smooth")
		locoMotions.push_back("ChrMarine@WalkCircleRt01_smooth")
		locoMotions.push_back("ChrMarine@WalkTightCircleLf01_smooth")
		locoMotions.push_back("ChrMarine@WalkTightCircleRt01_smooth")
		locoMotions.push_back("ChrMarine@Turn360Lf01")
		locoMotions.push_back("ChrMarine@Turn360Rt01")
		locoMotions.push_back("ChrMarine@StrafeFastLf01_smooth")
		locoMotions.push_back("ChrMarine@StrafeFastRt01_smooth")
		locoMotions.push_back("ChrMarine@Idle01_StepBackwardsLf01")
		locoMotions.push_back("ChrMarine@Idle01_StepBackwardsRt01")
		locoMotions.push_back("ChrMarine@Idle01_StepForwardLf01")
		locoMotions.push_back("ChrMarine@Idle01_StepForwardRt01")
		locoMotions.push_back("ChrMarine@Idle01_StepSidewaysLf01")
		locoMotions.push_back("ChrMarine@Idle01_StepSidewaysRt01")
		locoMotions.push_back("ChrMarine@Turn90Lf01")
		locoMotions.push_back("ChrMarine@Turn180Lf01")
		locoMotions.push_back("ChrMarine@Turn90Rt01")
		locoMotions.push_back("ChrMarine@Turn180Rt01")
		locoMotions.push_back("ChrMarine@Idle01_ToWalkLf01")
		locoMotions.push_back("ChrMarine@Idle01_ToWalk01_Turn90Lf01")
		locoMotions.push_back("ChrMarine@Idle01_ToWalk01_Turn180Lf01")
		locoMotions.push_back("ChrMarine@Idle01_ToWalk01")
		locoMotions.push_back("ChrMarine@Idle01_ToWalk01_Turn90Rt01")
		locoMotions.push_back("ChrMarine@Idle01_ToWalk01_Turn180Rt01")
		
		var sbChar = scene.getCharacter(charName)
		if(sbChar == null)
			return
		var skelName = sbChar.getSkeleton().getName()
		
		//var outDir = scene.getMediaPath() + '/retarget/motion/' + skelName + '/';
		//if(!os.path.exists(outDir))
		//	os.makedirs(outDir)
			
		// retarget standard locomotions
		/*
		for(var n = 0; n < locoMotions.size(); ++n){
			var curMotion = scene.getMotion(locoMotions.get(n))
			if(curMotion != null)
				retargetMotion(locoMotions.get(n), 'ChrBackovic.sk', skelName, outDir + 'MaleMocapLocomotion/');	
		}*/
		// setup standard locomotion
		locomotionChrMarineStateLocomotion.marineLocomotionSetup('ChrBackovic.sk', skelName, "base", skelName, skelName)
		
		// starting state, starting locomotion with different angle
		locomotionChrMarineStateStartingLeft.mocapStartLeftSetup(skelName, "base", skelName, skelName)
		
		// starting state, starting locomotion with different angle
		locomotionChrMarineStateStartingRight.mocapStartRightSetup(skelName, "base", skelName, skelName)

		// idle turn state, facing adjusting
		locomotionChrMarineStateIdleTurn.mocapIdleTurnSetup(skelName, "base", skelName, skelName)

		// step state, stepping adjusting
		locomotionChrMarineStateStep.mocapStepSetup(skelName, "base", skelName, skelName)
		
		BehaviorSetCommon.createRetargetInstance('ChrBackovic.sk', skelName)
		var stateManager = scene.getStateManager()
		var mocapLocomotionState = stateManager.getBlend('mocapLocomotion')
		if( mocapLocomotionState == null){
			// compute motion trajectory
			for(var i = 0; i < locoMotions.size(); ++i){
				var motion = scene.getMotion(locoMotions.get(i))
				//print 'motionName = ' + locoMotions[i]
				if( motion != null )
					motion.buildJointTrajectory('l_forefoot','base')
					motion.buildJointTrajectory('r_forefoot','base')
					motion.buildJointTrajectory('l_ankle','base')
					motion.buildJointTrajectory('r_ankle','base')
			}
			// setup standard locomotion
			locomotionChrMarineStateLocomotion.marineLocomotionSetup('ChrBackovic.sk', skelName, "base", skelName, skelName)
			
			// starting state, starting locomotion with different angle
			locomotionChrMarineStateStartingLeft.mocapStartLeftSetup(skelName, "base", skelName, skelName)
			
			// starting state, starting locomotion with different angle
			locomotionChrMarineStateStartingRight.mocapStartRightSetup(skelName, "base", skelName, skelName)

			// idle turn state, facing adjusting
			locomotionChrMarineStateIdleTurn.mocapIdleTurnSetup(skelName, "base", skelName, skelName)

			// step state, stepping adjusting
			locomotionChrMarineStateStep.mocapStepSetup(skelName, "base", skelName, skelName)
		  
			// setup transitions
			locomotionChrMarineTransition.transitionSetup();
		}
		// add IK constraint for foot automatically
		sbChar.addJointTrajectoryConstraint('l_forefoot','base')
		sbChar.addJointTrajectoryConstraint('r_forefoot','base')
		//sbChar.addJointTrajectoryConstraint('l_ankle','base')
		//sbChar.addJointTrajectoryConstraint('r_ankle','base')	
		// setup steering
		var steerManager = scene.getSteerManager()
		steerManager.setEnable(false)
		initSteerAgents.setupSteerAgent(charName, 'mocap')	
		steerManager.setEnable(true)
	}
}

