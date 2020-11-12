var initReach = {
	reachSetup : function(characterName, interpolatorType, originalSkeleton, preFix){
		console.log("Setting up reach database for " + characterName)
		var rightHandMotions = new Module.StringVec();
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachRtHigh")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachRtMidHigh")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachRtMidLow")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachLfLow")
		
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachLfHigh")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachLfMidHigh")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachRtMidLow")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachRtLow")
		
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachMiddleHigh")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachMiddleMidHigh")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachMiddleMidLow")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachMiddleLow")
		
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachClose_Lf")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachClose_Rt")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachClose_MiddleHigh")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachClose_MiddleLow")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachClose_MiddleMidHigh")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachClose_MiddleMidLow")
		
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachBehind_High1")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachBehind_High2")	
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachBehind_Low1")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachBehind_Low2")	
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachBehind_MidHigh1")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachBehind_MidHigh2")	
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachBehind_MidLow1")
		rightHandMotions.push_back(preFix+"ChrHarmony_Relax001_ArmReachBehind_MidLow2")	
		
		var leftHandMotions = new Module.StringVec();
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachRtHigh")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachRtMidHigh")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachRtMidLow")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachLfLow")
		
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachLfHigh")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachLfMidHigh")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachRtMidLow")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachRtLow")
		
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachMiddleHigh")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachMiddleMidHigh")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachMiddleMidLow")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachMiddleLow")
		
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachClose_Lf")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachClose_Rt")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachClose_MiddleHigh")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachClose_MiddleLow")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachClose_MiddleMidHigh")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachClose_MiddleMidLow")
		
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachBehind_High1")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachBehind_High2")	
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachBehind_Low1")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachBehind_Low2")	
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachBehind_MidHigh1")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachBehind_MidHigh2")	
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachBehind_MidLow1")
		leftHandMotions.push_back(preFix+"ChrHarmony_Relax001_LArmReachBehind_MidLow2")	
		
		var character = scene.getCharacter(characterName)
		var skeleton = character.getSkeleton()
		var reachManager = scene.getReachManager()
		var reach = reachManager.createReachWithTag(characterName,'default')	
		console.log('setting up reach for ' + characterName + ' , numReach()=' + reachManager.getNumReaches())
		reach.setInterpolatorType(interpolatorType)
		for(var i = 0; i < rightHandMotions.size(); ++i){
			//mirrorMotion1 = scene.getMotion(rightHandMotions[i])
			//mirrorMotion1.mirror(leftHandMotions[i], originalSkeleton)
			reach.addMotion("right",scene.getMotion(rightHandMotions.get(i)))
			reach.addMotion("left",scene.getMotion(leftHandMotions.get(i)))
		}
			
		
		//grabMirrorMotion = scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Grasp")
		//grabMirrorMotion.mirror(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Grasp", originalSkeleton)
		reach.setGrabHandMotion("right",scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Grasp"));
		reach.setGrabHandMotion("left",scene.getMotion(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Grasp"));
		
		//reachMirrorMotion = scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Reach")
		//reachMirrorMotion.mirror(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Reach", originalSkeleton)
		reach.setReachHandMotion("right",scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Reach"));
		reach.setReachHandMotion("left",scene.getMotion(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Reach"));
		
		//releaseMirrorMotion = scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Release")
		//releaseMirrorMotion.mirror(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Release", originalSkeleton)
		reach.setReleaseHandMotion("right",scene.getMotion(preFix+"ChrHarmony_Relax001_HandGraspSmSphere_Release"));
		reach.setReleaseHandMotion("left",scene.getMotion(preFix+"ChrHarmony_Relax001_LHandGraspSmSphere_Release"));
		
		//pointMirrorMotion = scene.getMotion(preFix+"HandsAtSide_RArm_GestureYou")
		//pointMirrorMotion.mirror(preFix+"HandsAtSide_LArm_GestureYou", originalSkeleton)
		reach.setPointHandMotion("right",scene.getMotion(preFix+"HandsAtSide_RArm_GestureYou"));
		reach.setPointHandMotion("left",scene.getMotion(preFix+"HandsAtSide_LArm_GestureYou"));
		
		reach.build(scene.getCharacter(characterName))		
		// handle grasp event
		// grasp event currently uses old-style command interface
		// by storing the command as the parameter of the event
		var GraspHandler = Module.SBEventHandler.extend("SBEventHandler", {
			executeAction: function(event){
				var params = event.getParameters()
				scene.command(params)
			}
		});
		
		// now create the event handler for the 'reach' event
		var graspHandler = new GraspHandler
		var em = scene.getEventManager()
		em.addEventHandler("reach", graspHandler)
	}
}

















