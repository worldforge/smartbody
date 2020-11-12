var BehaviorSetReaching = {
	setupBehaviorSet : function (){
		console.log("Setting up behavior set for Reaching ...")
		//scene.loadAssetsFromPath("behaviorsets/reaching/skeletons")
		//scene.loadAssetsFromPath("behaviorsets/reaching/motions")
		var assetManager = scene.getAssetManager()	
		var motionPath = "behaviorsets/reaching/motions/"
		var skel = scene.getSkeleton("common.sk")
		if(skel == null)
			scene.loadAssetsFromPath("behaviorsets/reaching/skeletons")
			
		var commonSk = scene.getSkeleton("common.sk")
		// mirror all arm and hand motions
		var preFix = ""
		var rightHandMotions = new Module.StringVec()
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
		
		rightHandMotions.push_back("ChrHarmony_Relax001_HandGraspSmSphere_Grasp")
		rightHandMotions.push_back("ChrHarmony_Relax001_HandGraspSmSphere_Reach")
		rightHandMotions.push_back("ChrHarmony_Relax001_HandGraspSmSphere_Release")
		rightHandMotions.push_back("HandsAtSide_RArm_GestureYou")
		
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
		
		leftHandMotions.push_back("ChrHarmony_Relax001_LHandGraspSmSphere_Grasp")	
		leftHandMotions.push_back("ChrHarmony_Relax001_LHandGraspSmSphere_Reach")	
		leftHandMotions.push_back("ChrHarmony_Relax001_LHandGraspSmSphere_Release")	
		leftHandMotions.push_back("HandsAtSide_LArm_GestureYou")	
		
		for(var i = 0; i < rightHandMotions.size(); ++i){
			var motion = scene.getMotion(rightHandMotions.get(i))
			if(motion == null){
				assetManager.loadAsset(motionPath+rightHandMotions.get(i)+'.skm')
				var motion = scene.getMotion(rightHandMotions.get(i))
			}
			//print 'motionName = ' + locoMotions[i]
			if(motion != null)
				motion.setMotionSkeletonName("common.sk")			
			var mirrorMotion1 = scene.getMotion(rightHandMotions.get(i))
			mirrorMotion1.mirror(leftHandMotions.get(i), 'common.sk')	

		}			
	
	},
	
	retargetBehaviorSet : function (charName){
		var reachMotions = new Module.StringVec()
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachRtHigh")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachRtMidHigh")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachRtMidLow")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachLfLow")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachLfHigh")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachLfMidHigh")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachRtMidLow")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachRtLow")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachMiddleHigh")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachMiddleMidHigh")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachMiddleMidLow")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachMiddleLow")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachClose_Lf")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachClose_Rt")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachClose_MiddleHigh")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachClose_MiddleLow")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachClose_MiddleMidHigh")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachClose_MiddleMidLow")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachBehind_High1")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachBehind_High2")	
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachBehind_Low1")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachBehind_Low2")	
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachBehind_MidHigh1")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachBehind_MidHigh2")	
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachBehind_MidLow1")
		reachMotions.push_back("ChrHarmony_Relax001_ArmReachBehind_MidLow2")	
			
		reachMotions.push_back("ChrHarmony_Relax001_HandGraspSmSphere_Grasp")
		reachMotions.push_back("ChrHarmony_Relax001_HandGraspSmSphere_Reach")
		reachMotions.push_back("ChrHarmony_Relax001_HandGraspSmSphere_Release")
		reachMotions.push_back("HandsAtSide_RArm_GestureYou")
		
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachRtHigh")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachRtMidHigh")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachRtMidLow")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachLfLow")	
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachLfHigh")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachLfMidHigh")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachRtMidLow")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachRtLow")	
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachMiddleHigh")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachMiddleMidHigh")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachMiddleMidLow")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachMiddleLow")	
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachClose_Lf")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachClose_Rt")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachClose_MiddleHigh")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachClose_MiddleLow")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachClose_MiddleMidHigh")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachClose_MiddleMidLow")	
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachBehind_High1")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachBehind_High2")	
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachBehind_Low1")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachBehind_Low2")	
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachBehind_MidHigh1")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachBehind_MidHigh2")	
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachBehind_MidLow1")
		reachMotions.push_back("ChrHarmony_Relax001_LArmReachBehind_MidLow2")	
		
		reachMotions.push_back("ChrHarmony_Relax001_LHandGraspSmSphere_Grasp")	
		reachMotions.push_back("ChrHarmony_Relax001_LHandGraspSmSphere_Reach")	
		reachMotions.push_back("ChrHarmony_Relax001_LHandGraspSmSphere_Release")	
		reachMotions.push_back("HandsAtSide_LArm_GestureYou")
		
		
		
		//outDir = scene.getMediaPath() + 'retarget/motion/' + skelName + '/';
		//print 'outDir = ' + outDir ;
		//if not os.path.exists(outDir):
		//	os.makedirs(outDir)	
		// retarget reaching
		//for n in range(0, len(reachMotions)):
		//	retargetMotion(reachMotions[n], 'common.sk', skelName, outDir + 'Reaching/');	
		var assetManager = scene.getAssetManager()
		for(var i = 0; i < reachMotions.size(); ++i){
			var sbMotion = assetManager.getMotion(reachMotions.get(i))
			if(sbMotion != null)
				sbMotion.setMotionSkeletonName('common.sk')
		}
		var sbChar = scene.getCharacter(charName)
		if (sbChar == null)
			return
		var skelName = sbChar.getSkeleton().getName()
		BehaviorSetCommon.createRetargetInstance('common.sk', skelName)
		
		initReach.reachSetup(charName, "KNN", 'common.sk', '')	
	}
}

	