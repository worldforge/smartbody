var BehaviorSetFemaleGestures = {
	setupBehaviorSet: function (){
		console.log("Setting up behavior set for gestures...")
		//scene.loadAssetsFromPath("behaviorsets/FemaleGestures/skeletons")
		//scene.loadAssetsFromPath("behaviorsets/FemaleGestures/motions")
		
		var assetManager = scene.getAssetManager()	
		var motionPath = "behaviorsets/FemaleGestures/motions/"
		var skel = scene.getSkeleton("ChrConnor.sk")
		if (skel == null)
			scene.loadAssetsFromPath("behaviorsets/FemaleGestures/skeletons")
		// map the zebra2 skeleton
		zebra2map.zebra2Map();
		var zebra2Map = scene.getJointMapManager().getJointMap("zebra2")
		var bradSkeleton = scene.getSkeleton("ChrConnor.sk")
		zebra2Map.applySkeleton(bradSkeleton)
		
		gestureMotions = new Module.StringVec()
		gestureMotions.push_back("ChrConnor@IdleStand01")
		gestureMotions.push_back("ChrConnor@IdleCross01_ThrowAwayRt01")
		gestureMotions.push_back("ChrConnor@BeatFlipBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_BeatFlipRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_BeatForwardBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_BeatForwardRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_BeatMidBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_BeatMidRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_ChopRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_MeBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_MeRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateForwardLowBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateForwardLowRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateForwardMidBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateForwardMidRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouForwardBt01")

		gestureMotions.push_back("ChrConnor@IdleStand01_NegateLeftLowBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateLeftLowRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateLeftMidBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateLeftMidRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateRightLowBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateRightLowRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateRightMidBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateRightMidRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NoForwardBt01")	
		gestureMotions.push_back("ChrConnor@IdleStand01_NoForwardRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NoLeftBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NoLeftRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NoRightBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NoRightRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_ScrewYouBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_ScrewYouRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopForwardBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopForwardRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopLeftBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopLeftRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopRightBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopRightRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouForwardRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouLeftBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouLeftRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouRightBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouRightRt01")
		gestureMotions.push_back("ChrConnor@IdleUpright01_KickChair01")
		
		for(var i = 0; i < gestureMotions.size(); ++i){
			var motion = scene.getMotion(gestureMotions.get(i))	
			if (motion == null){
				assetManager.loadAsset(motionPath+gestureMotions.get(i)+'.skm')
				var motion = scene.getMotion(gestureMotions.get(i))	
			}			
			if (motion != null){
				motion = scene.getMotion(gestureMotions.get(i))
				zebra2Map.applyMotion(motion)
			}
		}	
		mirroredMotions = new Module.StringVec()
		
		mirroredMotions.push_back("ChrConnor@IdleCross01_ThrowAwayRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_BeatFlipRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_BeatForwardRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_BeatMidRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_ChopRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_MeRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_NegateForwardLowRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_NegateForwardMidRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_NegateLeftLowRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_NegateLeftMidRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_NegateRightLowRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_NegateRightMidRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_NoForwardRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_NoLeftRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_NoRightRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_ScrewYouRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_StopForwardRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_StopLeftRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_StopRightRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_YouForwardRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_YouLeftRt01")
		mirroredMotions.push_back("ChrConnor@IdleStand01_YouRightRt01")
		
		for(var i = 0; i < mirroredMotions.size(); ++i){
			mirrorMotion = scene.getMotion(mirroredMotions.get(i))
			if(mirrorMotion != null)
				mirrorMotion.mirror(mirroredMotions.get(i)+"Lf", "ChrConnor.sk")
		}
	},

	retargetBehaviorSet: function (charName){
		gestureMotions = new Module.StringVec()
		gestureMotions.push_back("ChrConnor@IdleStand01")
		gestureMotions.push_back("ChrConnor@IdleCross01_ThrowAwayRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_BeatFlipRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_BeatForwardRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_BeatMidRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_ChopRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_MeRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateLeftLowBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateLeftLowRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateLeftMidBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateLeftMidRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateRightLowBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateRightLowRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateRightMidBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateRightMidRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NoForwardBt01")	
		gestureMotions.push_back("ChrConnor@IdleStand01_NoForwardRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NoLeftBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NoLeftRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NoRightBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_NoRightRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_ScrewYouBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_ScrewYouRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopForwardBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopForwardRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopLeftBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopLeftRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopRightBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopRightRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouForwardRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouLeftBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouLeftRt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouRightBt01")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouRightRt01")
		gestureMotions.push_back("ChrConnor@IdleUpright01_KickChair01")
		
		// mirrored motions
		gestureMotions.push_back("ChrConnor@IdleCross01_ThrowAwayRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_BeatFlipRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_BeatForwardRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_BeatMidRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_ChopRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_MeRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateForwardLowRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateForwardMidRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateLeftLowRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateLeftMidRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateRightLowRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_NegateRightMidRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_NoForwardRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_NoLeftRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_NoRightRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_ScrewYouRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopForwardRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopLeftRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_StopRightRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouForwardRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouLeftRt01Lf")
		gestureMotions.push_back("ChrConnor@IdleStand01_YouRightRt01Lf")
		
		//outDir = scene.getMediaPath() + '/retarget/motion/' +  '/';
		//print 'outDir = ' + outDir ;
		//if not os.path.exists(outDir):
		//	os.makedirs(outDir)
		
		sbChar = scene.getCharacter(charName)
		if (sbChar == null)
			return
		skelName = sbChar.getSkeleton().getName()
		
		assetManager = scene.getAssetManager()	
		for(var i = 0; i < gestureMotions.size(); ++i){
			var sbMotion = assetManager.getMotion(gestureMotions.get(i))
			if(sbMotion != null)
				sbMotion.setMotionSkeletonName("ChrConnor.sk")
		}
		BehaviorSetCommon.createRetargetInstance('ChrConnor.sk', skelName)			
		
		// set up the gesture map
		gestureMap = scene.getGestureMapManager().createGestureMap(charName)
		// you
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_YouForwardRt01", "YOU", "", "RIGHT_HAND", "", "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_YouForwardRt01Lf", "YOU", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_YouForwardBt01", "YOU", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")

		// me
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_MeRt01", "ME", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_MeRt01Lf", "ME", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_MeBt01", "ME", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")

		// beat 
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_BeatForwardRt01", "BEAT", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_BeatForwardRt01Lf", "BEAT", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_BeatForwardBt01", "BEAT", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")
		
		// chop 
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_ChopRt01", "CHOP", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_ChopRt01Lf", "CHOP", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_ChopBt01", "CHOP", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")

		// negate 
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_NegateRightMidRt01", "NEGATE", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_NegateRightMidRt01Lf", "NEGATE", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_NegateRightMidBt01", "NEGATE", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")
		
		// no 
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_NoForwardRt01", "NO", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_NoForwardRt01Lf", "NO", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_NoForwardBt01", "NO", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")
			
		// stop 
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_StopRightRt01", "STOP", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_StopRightRt01Lf", "STOP", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_StopRightBt01", "STOP", "", "BOTH_HANDS", "",  "ChrConnor@IdleStand01")
		
		// throw away 
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_ThrowAwayRt01", "THROWAWAY", "", "RIGHT_HAND", "",  "ChrConnor@IdleStand01")
		gestureMap.addGestureMapping( "ChrConnor@IdleStand01_ThrowAwayRt01Lf", "THROWAWAY", "", "LEFT_HAND", "",  "ChrConnor@IdleStand01")
		
		console.log("Setting gesture map attribute...")
		sbChar.setStringAttribute("gestureMap", charName)
	}	
}


	
	
	

		
