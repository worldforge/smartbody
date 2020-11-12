scene.run("BehaviorSetCommon.py")

def setupBehaviorSet():
	print "Setting up behavior set for gestures..."
	scene.addAssetPath("script", "behaviorsets/PNGestures/scripts")
	
	assetManager = scene.getAssetManager()	
	motionPath = "behaviorsets/PNGestures/motions/"
	skel = scene.getSkeleton("pnskeleton.bvh")
	if skel == None:
		scene.loadAssetsFromPath("behaviorsets/PNGestures/skeletons")
	
	scene.run("perceptionneuron-map.py")
	pnmap = scene.getJointMapManager().getJointMap("perceptionneuron")
	pnSkeleton = scene.getSkeleton("pnskeleton.bvh")
	pnmap.applySkeleton(pnSkeleton)
	
	gestureMotions = StringVec()
	gestureMotions.append("idle2")
	gestureMotions.append("me2")
	gestureMotions.append("me_intense2")
	gestureMotions.append("you")
	gestureMotions.append("you_intense2")
	gestureMotions.append("us2")
	gestureMotions.append("us_intense2")
	gestureMotions.append("we")
	gestureMotions.append("we_intense2")	
	gestureMotions.append("bigger2")
	gestureMotions.append("bigger_intense2")
	gestureMotions.append("inclusivity2")
	gestureMotions.append("inclusivity_intense2")
	gestureMotions.append("negation2")
	gestureMotions.append("negation_intense2")
	gestureMotions.append("pointleft2")
	gestureMotions.append("pointleft_intense2")
	gestureMotions.append("pointright2")
	gestureMotions.append("pointright_intense2")
	gestureMotions.append("pointcenter2")
	gestureMotions.append("pointcenter_intense2")
	gestureMotions.append("pointhere2")
	gestureMotions.append("pointhere_intense2")
	gestureMotions.append("question2")
	gestureMotions.append("question_intense2")	
	gestureMotions.append("stop2")
	gestureMotions.append("stop_intense2")		
	
	for i in range(0, len(gestureMotions)):
		motion = scene.getMotion(gestureMotions[i])	
		if motion == None:
			assetManager.loadAsset(motionPath+gestureMotions[i] + ".skm")
			motion = scene.getMotion(gestureMotions[i])		
		if motion != None:
			motion = scene.getMotion(gestureMotions[i])
			pnmap.applyMotion(motion)
	
	# add gesture blends
	skeleton = "pnskeleton.bvh"
	
	stateTest = scene.getBlendManager().createState1D("MeBlend")
	stateTest.setBlendSkeleton(skeleton)
	motions = StringVec()
	stateTest.addMotion("me_intense2", 0.0)
	stateTest.addMotion("me2", 1.0)

	stateTest = scene.getBlendManager().createState1D("YouBlend")
	stateTest.setBlendSkeleton(skeleton)
	motions = StringVec()
	stateTest.addMotion("you_intense2", 0.0)
	stateTest.addMotion("you2", 1.0)
	
	stateTest = scene.getBlendManager().createState1D("UsBlend")
	stateTest.setBlendSkeleton(skeleton)
	motions = StringVec()
	stateTest.addMotion("us_intense2", 0.0)	
	stateTest.addMotion("us2", 1.0)
	
	stateTest = scene.getBlendManager().createState1D("WeBlend")
	stateTest.setBlendSkeleton(skeleton)
	motions = StringVec()
	stateTest.addMotion("we_intense2", 0.0)	
	stateTest.addMotion("we2", 1.0)
	
	stateTest = scene.getBlendManager().createState1D("StopBlend")
	stateTest.setBlendSkeleton(skeleton)
	motions = StringVec()
	stateTest.addMotion("stop_intense2", 0.0)	
	stateTest.addMotion("stop2", 1.0)
	
	stateTest = scene.getBlendManager().createState1D("QuestionBlend")
	stateTest.setBlendSkeleton(skeleton)
	motions = StringVec()
	stateTest.addMotion("question_intense2", 0.0)	
	stateTest.addMotion("question2", 1.0)
	
	stateTest = scene.getBlendManager().createState1D("PointRightBlend")
	stateTest.setBlendSkeleton(skeleton)
	motions = StringVec()
	stateTest.addMotion("pointright_intense2", 0.0)	
	stateTest.addMotion("pointright2", 1.0)
		
	stateTest = scene.getBlendManager().createState1D("PointLeftBlend")
	stateTest.setBlendSkeleton(skeleton)
	motions = StringVec()
	stateTest.addMotion("pointleft_intense2", 0.0)	
	stateTest.addMotion("pointleft2", 1.0)

	stateTest = scene.getBlendManager().createState1D("PointCenterBlend")
	stateTest.setBlendSkeleton(skeleton)
	motions = StringVec()
	stateTest.addMotion("pointcenter_intense2", 0.0)	
	stateTest.addMotion("pointcenter2", 1.0)
	
	stateTest = scene.getBlendManager().createState1D("PointHereBlend")
	stateTest.setBlendSkeleton(skeleton)
	motions = StringVec()
	stateTest.addMotion("pointhere_intense2", 0.0)		
	stateTest.addMotion("pointhere2", 1.0)
	
	stateTest = scene.getBlendManager().createState1D("NegationBlend")
	stateTest.setBlendSkeleton(skeleton)
	motions = StringVec()
	stateTest.addMotion("negation_intense2", 0.0)	
	stateTest.addMotion("negation2", 1.0)
	
	stateTest = scene.getBlendManager().createState1D("StopBlend")
	stateTest.setBlendSkeleton(skeleton)
	motions = StringVec()
	stateTest.addMotion("stop_intense2", 0.0)	
	stateTest.addMotion("stop2", 1.0)

	stateTest = scene.getBlendManager().createState1D("BiggerBlend")
	stateTest.setBlendSkeleton(skeleton)
	motions = StringVec()
	stateTest.addMotion("bigger_intense2", 0.0)	
	stateTest.addMotion("bigger2", 1.0)
	
	
def retargetBehaviorSet(charName):
	gestureMotions = StringVec()
	gestureMotions.append("idle2")
	gestureMotions.append("me2")
	gestureMotions.append("me_intense2")
	gestureMotions.append("you")
	gestureMotions.append("you_intense2")
	gestureMotions.append("us2")
	gestureMotions.append("us_intense2")
	gestureMotions.append("we")
	gestureMotions.append("we_intense2")	
	gestureMotions.append("bigger2")
	gestureMotions.append("bigger_intense2")
	gestureMotions.append("inclusivity2")
	gestureMotions.append("inclusivity_intense2")
	gestureMotions.append("negation2")
	gestureMotions.append("negation_intense2")
	gestureMotions.append("pointleft2")
	gestureMotions.append("pointleft_intense2")
	gestureMotions.append("pointright2")
	gestureMotions.append("pointright_intense2")
	gestureMotions.append("pointcenter2")
	gestureMotions.append("pointcenter_intense2")
	gestureMotions.append("pointthere2")
	gestureMotions.append("pointthere_intense2")
	gestureMotions.append("question2")
	gestureMotions.append("question_intense2")	
	gestureMotions.append("stop2")
	gestureMotions.append("stop_intense2")		
	
	sbChar = scene.getCharacter(charName)
	if sbChar == None:
		return
	skelName = sbChar.getSkeleton().getName()
	
	assetManager = scene.getAssetManager()	
	for i in range(0, len(gestureMotions)):
		sbMotion = assetManager.getMotion(gestureMotions[i])
		if sbMotion != None:
			sbMotion.setMotionSkeletonName("pnskeleton.bvh")
	
	createRetargetInstance('pnskeleton.bvh', skelName)			
	
	# set up the gesture map
	gestureMap = scene.getGestureMapManager().createGestureMap(charName)
	gestureMap.addGestureMapping( "YouBlend", "DEICTIC", "YOU", "", "", "idle2")
	gestureMap.addGestureMapping( "MeBlend", "DEICTIC", "ME", "", "",  "idle2")
	gestureMap.addGestureMapping( "WeBlend", "DEICTIC", "WE", "", "",  "idle2")
	gestureMap.addGestureMapping( "UsBlend", "DEICTIC", "US", "", "",  "idle2")
	gestureMap.addGestureMapping( "StopBlend", "DEICTIC", "US", "", "",  "idle2")
	gestureMap.addGestureMapping( "QuestionBlend", "METAPHORIC", "QUESTION", "", "",  "idle2")
	gestureMap.addGestureMapping( "PointRightBlend", "DEICTIC", "RIGHT", "", "",  "idle2")
	gestureMap.addGestureMapping( "PointLeftBlend", "DEICTIC", "LEFT", "", "",  "idle2")
	gestureMap.addGestureMapping( "PointCenterBlend", "DEICTIC", "CENTER", "", "",  "idle2")
	gestureMap.addGestureMapping( "PointHereBlend", "DEICTIC", "HERE", "", "",  "idle2")
	gestureMap.addGestureMapping( "NegationBlend", "METAPHORIC", "NEGATION", "", "",  "idle2")
	gestureMap.addGestureMapping( "StopBlend", "METAPHORIC", "STOP", "", "",  "idle2")
	gestureMap.addGestureMapping( "BiggerBlend", "METAPHORIC", "COMPARATIVE_BIGGER", "", "",  "idle2")	
	gestureMap.addGestureMapping( "YouBlend", "BEAT", "RHYTHM", "", "",  "idle2")	

	print "Setting gesture map attribute..."
	sbChar.setStringAttribute("gestureMap", charName)
		
	
	
	
	

		
