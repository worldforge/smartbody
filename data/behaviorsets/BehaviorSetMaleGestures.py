scene.run("BehaviorSetCommon.py")

def setupBehaviorSet():
	print "Setting up behavior set for gestures..."
	scene.addAssetPath("script", "behaviorsets/MaleGestures/scripts")
	
	assetManager = scene.getAssetManager()	
	motionPath = "behaviorsets/MaleGestures/motions/"
	skel = scene.getSkeleton("ChrGarza.sk")
	if skel == None:
		scene.loadAssetsFromPath("behaviorsets/MaleGestures/skeletons")
	# map the zebra2 skeleton
	scene.run("zebra2-map.py")
	zebra2Map = scene.getJointMapManager().getJointMap("zebra2")
	bradSkeleton = scene.getSkeleton("ChrGarza.sk")
	zebra2Map.applySkeleton(bradSkeleton)
	
	gestureMotions = StringVec()
	gestureMotions.append("ChrGarza@IdleStand01")
	gestureMotions.append("ChrGarza@IdleStand01_BeatFlipBt01")
	gestureMotions.append("ChrGarza@IdleStand01_BeatFlipRt01")
	gestureMotions.append("ChrGarza@IdleStand01_BeatForwardBt01")
	gestureMotions.append("ChrGarza@IdleStand01_BeatForwardRt01")
	gestureMotions.append("ChrGarza@IdleStand01_BeatMidBt01")
	gestureMotions.append("ChrGarza@IdleStand01_BeatMidRt01")
	gestureMotions.append("ChrGarza@IdleStand01_ChopBt01")
	gestureMotions.append("ChrGarza@IdleStand01_ChopRt01")
	gestureMotions.append("ChrGarza@IdleStand01_MeBt01")
	gestureMotions.append("ChrGarza@IdleStand01_MeRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateForwardLowBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateForwardLowRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateForwardMidBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateForwardMidRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateLeftLowBt01")

	gestureMotions.append("ChrGarza@IdleStand01_NegateLeftLowRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateLeftMidBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateLeftMidRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateRightLowBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateRightLowRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateRightMidBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateRightMidRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NoForwardBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NoForwardRt01")	
	gestureMotions.append("ChrGarza@IdleStand01_NoLeftBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NoLeftRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NoRightBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NoRightRt01")
	gestureMotions.append("ChrGarza@IdleStand01_ScrewYouBt01")
	gestureMotions.append("ChrGarza@IdleStand01_ScrewYouRt01")
	gestureMotions.append("ChrGarza@IdleStand01_StopForwardBt01")
	gestureMotions.append("ChrGarza@IdleStand01_StopForwardRt01")
	gestureMotions.append("ChrGarza@IdleStand01_StopLeftBt01")
	gestureMotions.append("ChrGarza@IdleStand01_StopLeftRt01")
	gestureMotions.append("ChrGarza@IdleStand01_StopRightBt01")
	gestureMotions.append("ChrGarza@IdleStand01_StopRightRt01")
	gestureMotions.append("ChrGarza@IdleStand01_ToIdleFight01")
	gestureMotions.append("ChrGarza@IdleStand01_YouForwardBt01")
	gestureMotions.append("ChrGarza@IdleStand01_YouForwardRt01")
	gestureMotions.append("ChrGarza@IdleStand01_YouLeftBt01")
	gestureMotions.append("ChrGarza@IdleStand01_YouLeftRt01")
	gestureMotions.append("ChrGarza@IdleStand01_YouRightBt01")
	gestureMotions.append("ChrGarza@IdleStand01_YouRightRt01")
	
	for i in range(0, len(gestureMotions)):
		motion = scene.getMotion(gestureMotions[i])	
		if motion == None:
			assetManager.loadAsset(motionPath+gestureMotions[i]+'.skm')
			motion = scene.getMotion(gestureMotions[i])		
		if motion != None:
			motion = scene.getMotion(gestureMotions[i])
			zebra2Map.applyMotion(motion)

	mirroredMotions = StringVec()
	
	mirroredMotions.append("ChrGarza@IdleStand01_BeatFlipRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_BeatForwardRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_BeatMidRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_ChopRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_MeRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_NegateForwardLowRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_NegateForwardMidRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_NegateLeftLowRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_NegateLeftMidRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_NegateRightLowRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_NegateRightMidRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_NoForwardRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_NoLeftRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_NoRightRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_ScrewYouRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_StopForwardRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_StopLeftRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_StopLeftRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_YouForwardRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_YouLeftRt01")
	mirroredMotions.append("ChrGarza@IdleStand01_YouRightRt01")
	
	for i in range(0,len(mirroredMotions)):
		mirrorMotion = scene.getMotion(mirroredMotions[i])
		if mirrorMotion != None:
			mirrorMotion.mirror(mirroredMotions[i] + "Lf", "ChrGarza.sk")
			
	# create a set of blends between the 1x and the 2x speed motions
	allMotions = []
	for m in range(0, len(gestureMotions)):
		allMotions.append(gestureMotions[m])
	for m in range(0, len(mirroredMotions)):
		allMotions.append(mirroredMotions[m] + "Lf")
		
	# create sped-up versions of the gestures
	for s in range(1, len(allMotions)):
		m = scene.getMotion(allMotions[s])
		n = m.copy(allMotions[s] + "2x")
		n.speed(2)
		scene.getAssetManager().addMotion(n)
	
	# add gesture blends
	skeleton = "ChrGarza.sk"
	
	motionNames = []
	motionNames.append("ChrGarza@IdleStand01")
	motionNames.append("ChrGarza@IdleStand01_BeatFlipBt01")
	motionNames.append("ChrGarza@IdleStand01_BeatFlipRt01")
	motionNames.append("ChrGarza@IdleStand01_BeatForwardBt01")
	motionNames.append("ChrGarza@IdleStand01_BeatForwardRt01")
	motionNames.append("ChrGarza@IdleStand01_BeatMidBt01")
	motionNames.append("ChrGarza@IdleStand01_BeatMidRt01")
	motionNames.append("ChrGarza@IdleStand01_ChopBt01")
	motionNames.append("ChrGarza@IdleStand01_ChopRt01")
	motionNames.append("ChrGarza@IdleStand01_MeBt01")
	motionNames.append("ChrGarza@IdleStand01_MeRt01")
	motionNames.append("ChrGarza@IdleStand01_NegateForwardLowBt01")
	motionNames.append("ChrGarza@IdleStand01_NegateForwardLowRt01")
	motionNames.append("ChrGarza@IdleStand01_NegateForwardMidBt01")
	motionNames.append("ChrGarza@IdleStand01_NegateForwardMidRt01")
	motionNames.append("ChrGarza@IdleStand01_NegateLeftLowBt01")

	motionNames.append("ChrGarza@IdleStand01_NegateLeftLowRt01")
	motionNames.append("ChrGarza@IdleStand01_NegateLeftMidBt01")
	motionNames.append("ChrGarza@IdleStand01_NegateLeftMidRt01")
	motionNames.append("ChrGarza@IdleStand01_NegateRightLowBt01")
	motionNames.append("ChrGarza@IdleStand01_NegateRightLowRt01")
	motionNames.append("ChrGarza@IdleStand01_NegateRightMidBt01")
	motionNames.append("ChrGarza@IdleStand01_NegateRightMidRt01")
	motionNames.append("ChrGarza@IdleStand01_NoForwardBt01")
	motionNames.append("ChrGarza@IdleStand01_NoForwardRt01")	
	motionNames.append("ChrGarza@IdleStand01_NoLeftBt01")
	motionNames.append("ChrGarza@IdleStand01_NoLeftRt01")
	motionNames.append("ChrGarza@IdleStand01_NoRightBt01")
	motionNames.append("ChrGarza@IdleStand01_NoRightRt01")
	motionNames.append("ChrGarza@IdleStand01_ScrewYouBt01")
	motionNames.append("ChrGarza@IdleStand01_ScrewYouRt01")
	motionNames.append("ChrGarza@IdleStand01_StopForwardBt01")
	motionNames.append("ChrGarza@IdleStand01_StopForwardRt01")
	motionNames.append("ChrGarza@IdleStand01_StopLeftBt01")
	motionNames.append("ChrGarza@IdleStand01_StopLeftRt01")
	motionNames.append("ChrGarza@IdleStand01_StopRightBt01")
	motionNames.append("ChrGarza@IdleStand01_StopRightRt01")
	motionNames.append("ChrGarza@IdleStand01_ToIdleFight01")
	motionNames.append("ChrGarza@IdleStand01_YouForwardBt01")
	motionNames.append("ChrGarza@IdleStand01_YouForwardRt01")
	motionNames.append("ChrGarza@IdleStand01_YouLeftBt01")
	motionNames.append("ChrGarza@IdleStand01_YouLeftRt01")
	motionNames.append("ChrGarza@IdleStand01_YouRightBt01")
	motionNames.append("ChrGarza@IdleStand01_YouRightRt01")
	motionNames.append("ChrGarza@IdleStand01_BeatFlipRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_BeatForwardRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_BeatMidRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_ChopRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_MeRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_NegateForwardLowRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_NegateForwardMidRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_NegateLeftLowRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_NegateLeftMidRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_NegateRightLowRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_NegateRightMidRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_NoForwardRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_NoLeftRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_NoRightRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_ScrewYouRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_StopForwardRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_StopLeftRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_StopLeftRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_YouForwardRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_YouLeftRt01Lf")
	motionNames.append("ChrGarza@IdleStand01_YouRightRt01Lf")
	
	skeleton = "ChrGarza.sk"

	for b in range(1, len(motionNames)):	
		stateTest = scene.getBlendManager().createState1D(motionNames[b] + "Blend")
		stateTest.setBlendSkeleton(skeleton)
		motions = StringVec()
		stateTest.addMotion(motionNames[b], 0.0)
		stateTest.addMotion(motionNames[b] + "2x", 1.0)
		print "Added gesture blend " + motionNames[b] + "Blend"



def retargetBehaviorSet(charName):
	gestureMotions = StringVec()
	gestureMotions.append("ChrGarza@IdleStand01")
	gestureMotions.append("ChrGarza@IdleStand01_BeatFlipBt01")
	gestureMotions.append("ChrGarza@IdleStand01_BeatFlipRt01")
	gestureMotions.append("ChrGarza@IdleStand01_BeatForwardBt01")
	gestureMotions.append("ChrGarza@IdleStand01_BeatForwardRt01")
	gestureMotions.append("ChrGarza@IdleStand01_BeatMidBt01")
	gestureMotions.append("ChrGarza@IdleStand01_BeatMidRt01")
	gestureMotions.append("ChrGarza@IdleStand01_ChopBt01")
	gestureMotions.append("ChrGarza@IdleStand01_ChopRt01")
	gestureMotions.append("ChrGarza@IdleStand01_MeBt01")
	gestureMotions.append("ChrGarza@IdleStand01_MeRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateForwardLowBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateForwardLowRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateForwardMidBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateForwardMidRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateLeftLowBt01")

	gestureMotions.append("ChrGarza@IdleStand01_NegateLeftLowRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateLeftMidBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateLeftMidRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateRightLowBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateRightLowRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateRightMidBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NegateRightMidRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NoForwardBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NoForwardRt01")	
	gestureMotions.append("ChrGarza@IdleStand01_NoLeftBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NoLeftRt01")
	gestureMotions.append("ChrGarza@IdleStand01_NoRightBt01")
	gestureMotions.append("ChrGarza@IdleStand01_NoRightRt01")
	gestureMotions.append("ChrGarza@IdleStand01_ScrewYouBt01")
	gestureMotions.append("ChrGarza@IdleStand01_ScrewYouRt01")
	gestureMotions.append("ChrGarza@IdleStand01_StopForwardBt01")
	gestureMotions.append("ChrGarza@IdleStand01_StopForwardRt01")
	gestureMotions.append("ChrGarza@IdleStand01_StopLeftBt01")
	gestureMotions.append("ChrGarza@IdleStand01_StopLeftRt01")
	gestureMotions.append("ChrGarza@IdleStand01_StopRightBt01")
	gestureMotions.append("ChrGarza@IdleStand01_StopRightRt01")
	gestureMotions.append("ChrGarza@IdleStand01_ToIdleFight01")
	gestureMotions.append("ChrGarza@IdleStand01_YouForwardBt01")
	gestureMotions.append("ChrGarza@IdleStand01_YouForwardRt01")
	gestureMotions.append("ChrGarza@IdleStand01_YouLeftBt01")
	gestureMotions.append("ChrGarza@IdleStand01_YouLeftRt01")
	gestureMotions.append("ChrGarza@IdleStand01_YouRightBt01")
	gestureMotions.append("ChrGarza@IdleStand01_YouRightRt01")
	
	
	# mirrored motions
	gestureMotions.append("ChrGarza@IdleStand01_BeatFlipRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_BeatForwardRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_BeatMidRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_ChopRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_MeRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_NegateForwardLowRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_NegateForwardMidRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_NegateLeftLowRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_NegateLeftMidRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_NegateRightLowRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_NegateRightMidRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_NoForwardRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_NoLeftRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_NoRightRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_ScrewYouRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_StopForwardRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_StopLeftRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_StopLeftRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_YouForwardRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_YouLeftRt01Lf")
	gestureMotions.append("ChrGarza@IdleStand01_YouRightRt01Lf")
	
	#outDir = scene.getMediaPath() + '/retarget/motion/' +  '/';
	#print 'outDir = ' + outDir ;
	#if not os.path.exists(outDir):
	#	os.makedirs(outDir)
	
	sbChar = scene.getCharacter(charName)
	if sbChar == None:
		return
	skelName = sbChar.getSkeleton().getName()
	
	assetManager = scene.getAssetManager()	
	for i in range(0, len(gestureMotions)):
		sbMotion = assetManager.getMotion(gestureMotions[i])
		if sbMotion != None:
			sbMotion.setMotionSkeletonName("ChrGarza.sk")
	
	createRetargetInstance('ChrGarza.sk', skelName)			
	
	# set up the gesture map
	gestureMap = scene.getGestureMapManager().createGestureMap(charName)
	# you
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_YouForwardRt01", "YOU", "", "RIGHT_HAND", "", "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_YouForwardRt01Lf", "YOU", "", "LEFT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_YouForwardBt01", "YOU", "", "BOTH_HANDS", "",  "ChrGarza@IdleStand01")

	# me
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_MeRt01", "ME", "", "RIGHT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_MeRt01Lf", "ME", "", "LEFT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_MeBt01", "ME", "", "BOTH_HANDS", "",  "ChrGarza@IdleStand01")

	# beat 
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_BeatForwardRt01", "BEAT", "", "RIGHT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_BeatForwardRt01Lf", "BEAT", "", "LEFT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_BeatForwardBt01", "BEAT", "", "BOTH_HANDS", "",  "ChrGarza@IdleStand01")
	
	# chop 
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_ChopRt01", "CHOP", "", "RIGHT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_ChopRt01Lf", "CHOP", "", "LEFT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_ChopBt01", "CHOP", "", "BOTH_HANDS", "",  "ChrGarza@IdleStand01")

	# negate 
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_NegateRightMidRt01", "NEGATE", "", "RIGHT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_NegateRightMidRt01Lf", "NEGATE", "", "LEFT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_NegateRightMidBt01", "NEGATE", "", "BOTH_HANDS", "",  "ChrGarza@IdleStand01")
	
	# no 
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_NoForwardRt01", "NO", "", "RIGHT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_NoForwardRt01Lf", "NO", "", "LEFT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_NoForwardBt01", "NO", "", "BOTH_HANDS", "",  "ChrGarza@IdleStand01")
		
	# stop 
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_StopRightRt01", "STOP", "", "RIGHT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_StopRightRt01Lf", "STOP", "", "LEFT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_StopRightBt01", "STOP", "", "BOTH_HANDS", "",  "ChrGarza@IdleStand01")
	
	# throw away 
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_ThrowAwayRt01", "THROWAWAY", "", "RIGHT_HAND", "",  "ChrGarza@IdleStand01")
	gestureMap.addGestureMapping( "ChrGarza@IdleStand01_ThrowAwayRt01Lf", "THROWAWAY", "", "LEFT_HAND", "",  "ChrGarza@IdleStand01")
	
	print "Setting gesture map attribute..."
	sbChar.setStringAttribute("gestureMap", charName)
		
	
	
	
	

		
