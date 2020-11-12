var HandSynthesisDemo = {
	setMotionNameSkeleton : function (motionName, skelName){
		var motion = scene.getMotion(motionName)
		if(motion != null){	
			//console.log('motionName = ' + motionName + ' skelName = ' + skelName);
			//motion.scale(6)
			motion.setMotionSkeletonName(skelName)
		}
	},
	
	createRetargetInstance : function (srcSkelName, tgtSkelName){
		// replace retarget each animation with just a simple retarget instance
		
		// these joints and their children are not retargeted
		var endJoints = new Module.StringVec();
		endJoints.push_back('l_forefoot')
		endJoints.push_back('l_toe')
		endJoints.push_back('l_wrist')
		endJoints.push_back('r_forefoot')	
		endJoints.push_back('r_toe')	
		endJoints.push_back('r_wrist')

		// these joints are skipped during skeleton alignment
		var relativeJoints = new Module.StringVec();
		relativeJoints.push_back('spine1')
		relativeJoints.push_back('spine2')
		relativeJoints.push_back('spine3')
		relativeJoints.push_back('spine4')
		relativeJoints.push_back('spine5')
		relativeJoints.push_back('r_sternoclavicular')
		relativeJoints.push_back('l_sternoclavicular')
		relativeJoints.push_back('r_acromioclavicular')
		relativeJoints.push_back('l_acromioclavicular')	
		
		var retargetManager = scene.getRetargetManager()
		var retarget = retargetManager.getRetarget(srcSkelName,tgtSkelName)
		if(retarget == null){
			var retarget = 	retargetManager.createRetarget(srcSkelName,tgtSkelName)	
			retarget.initRetarget(endJoints,relativeJoints)
		}
	}

}	

function allReady(){
	console.log( "|--------------------------------------------|")
	console.log( "|        Setup Hand Demo 			        |")
	console.log( "|--------------------------------------------|")
	console.log( "Demonstrates using a set of gestures to synthesize hand and finger motion on top of a source motion that does not contain hand or finger animation data.")
	if(scene){
		// Add asset paths
		scene.addAssetPath('mesh', 'ChrMaarten')
		scene.addAssetPath('motion', 'ChrMaarten')
		scene.addAssetPath('motion', 'Hand')
		scene.loadAssets()
		
		// configure camera
		// Set scene parameters and camera
		console.log( 'Configuring scene parameters and camera' )
		scene.setScale(1.0)
		scene.setBoolAttribute('internalAudio', true)
		defaultViewer.defaultViewer();
		var camera = Module.getCamera()
		camera.setEye(0, 1.71, 1.86)
		camera.setCenter(0, 1, 0.01)
		camera.setUpVector(new Module.SrVec(0, 1, 0))
		camera.setScale(1)
		camera.setFov(1.0472)
		camera.setFarPlane(100)
		camera.setNearPlane(0.1)
		camera.setAspectRatio(0.966897)
		var cameraPos = new Module.SrVec(0, 1.6, 10)
		scene.getPawn('camera').setPosition(cameraPos)
		defaultInterface.defaultInterface();

		// Runs the default viewer for camera
		scene.setScale(1)

		// Set up joint map for character  
		console.log( 'Setting up joint map and configuring Brad\'s skeleton')
		zebra2map.zebra2Map();
		var zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
		var bradSkeleton = scene.getSkeleton('ChrBrad.sk')
		zebra2Map.applySkeleton(bradSkeleton)
		zebra2Map.applyMotionRecurse('ChrMaarten')

		// setting up retargetting between the source motion and the current character
		console.log("Setting up joint map and configuring Brad\'s skeleton")
		ipisoftmap.ipisoftmap();
		var ipisoftMap = scene.getJointMapManager().getJointMap('ipisoft')


		// retargetting the motion (iPi motion to SmartBody skeleton)
		var m = scene.getMotion("Adil_Gesture1.bvh")
		ipisoftMap.applyMotion(m)
		var ipiSk = scene.getSkeleton("Adil_Gesture1.bvh")
		ipisoftMap.applySkeleton(ipiSk)
		HandSynthesisDemo.setMotionNameSkeleton('Adil_Gesture1.bvh', 'Adil_Gesture1.bvh')
		HandSynthesisDemo.createRetargetInstance('Adil_Gesture1.bvh','ChrBrad.sk')
		// at this point, the source motion has been retargeted to the character,
		// so it can be used to generate the hand animation

		// run behavior set to load motions
		BehaviorSetGestures.setupBehaviorSet()
		BehaviorSetGestures.retargetBehaviorSet('ChrBrad')

		// create a new hand configuration
		var configManager = scene.getHandConfigurationManager()
		var handConfig = configManager.createHandConfiguration("singlegesture")
		handConfig.addMotion("ChrBrad@Idle01_NegativeBt01")
		var configMotions = handConfig.getMotionNames()
		//console.log( "Motions in singlegesture configuration:"
		//for m in range(0, len(configMotions)):
		//	console.log( configMotions[m]
			
		//create another hand configuration
		var handConfig2 = configManager.createHandConfiguration("multiplegestures")
		handConfig2.addMotion("ChrBrad@Idle01_YouPointLf01")
		handConfig2.addMotion("ChrBrad@Idle01_BeatHighBt01")
		handConfig2.addMotion("ChrBrad@Idle01_IndicateRightBt01")
		console.log( "Motions in multiplegestures configuration:")
		//configMotions = handConfig2.getMotionNames()
		//for m in range(0, len(configMotions)):
		//	console.log( configMotions[m]
			
		// create a third detailed hand configuration
		console.log( 'Setting up all_config')
		var handConfig3 = configManager.createHandConfiguration("allgestures")
		handConfig3.addMotion("ChrBrad@Idle01_YouPointLf01")
		handConfig3.addMotion("ChrBrad@Idle01_BeatHighBt01")
		handConfig3.addMotion("ChrBrad@Idle01_ArmStretch01")
		handConfig3.addMotion("ChrBrad@Idle01_BeatHighBt02")
		handConfig3.addMotion("ChrBrad@Idle01_BeatHighLf01")
		handConfig3.addMotion("ChrBrad@Idle01_BeatLowBt01")
		handConfig3.addMotion("ChrBrad@Idle01_BeatLowLf01")
		handConfig3.addMotion("ChrBrad@Idle01_BeatLowLf02")
		handConfig3.addMotion("ChrBrad@Idle01_BeatMidBt01")
		handConfig3.addMotion("ChrBrad@Idle01_BeatMidLf01")
		handConfig3.addMotion("ChrBrad@Idle01_ChopBoth01")
		handConfig3.addMotion("ChrBrad@Idle01_ChopBt01")
		handConfig3.addMotion("ChrBrad@Idle01_ChopLf01")
		handConfig3.addMotion("ChrBrad@Idle01_Contemplate01")
		handConfig3.addMotion("ChrBrad@Idle01_ExampleLf01")
		handConfig3.addMotion("ChrBrad@Idle01_HoweverLf01")
		handConfig3.addMotion("ChrBrad@Idle01_IndicateLeftBt01")
		handConfig3.addMotion("ChrBrad@Idle01_IndicateLeftLf01")
		handConfig3.addMotion("ChrBrad@Idle01_IndicateRightBt01")
		handConfig3.addMotion("ChrBrad@Idle01_IndicateRightRt01")
		handConfig3.addMotion("ChrBrad@Idle01_MeLf01")
		handConfig3.addMotion("ChrBrad@Idle01_NegativeBt01")
		handConfig3.addMotion("ChrBrad@Idle01_NegativeBt02")
		handConfig3.addMotion("ChrBrad@Idle01_NegativeLf01")
		handConfig3.addMotion("ChrBrad@Idle01_NegativeRt01")
		handConfig3.addMotion("ChrBrad@Idle01_OfferBoth01")
		handConfig3.addMotion("ChrBrad@Idle01_OfferLf01")
		handConfig3.addMotion("ChrBrad@Idle01_PleaBt02")
		handConfig3.addMotion("ChrBrad@Idle01_PointLf01")
		handConfig3.addMotion("ChrBrad@Idle01_ReceiveLf01")
		handConfig3.addMotion("ChrBrad@Idle01_ReceiveRt01")
		handConfig3.addMotion("ChrBrad@Idle01_SafeLf01")
		handConfig3.addMotion("ChrBrad@Idle01_SafeRt01")
		handConfig3.addMotion("ChrBrad@Idle01_ScratchChest01")
		handConfig3.addMotion("ChrBrad@Idle01_ScratchHeadLf01")
		handConfig3.addMotion("ChrBrad@Idle01_YouLf01")
		console.log( "Motions in multiplegestures configuration:")
		//configMotions = handConfig3.getMotionNames()
		//for m in range(0, len(configMotions)):
		//	console.log( configMotions[m]

		// Add ChrBrad (source character )
		var source = scene.createCharacter('source', '')
		var sourceSkeleton = scene.createSkeleton('ChrBrad.sk')
		source.setSkeleton(sourceSkeleton)
		var sourcePos = new Module.SrVec(0, 0, 0)
		source.setPosition(sourcePos)
		source.createStandardControllers()

		// Deformable mesh
		source.setDoubleAttribute('deformableMeshScale', 0.01)

		// enable these to show the deformable mesh, otherwise only the skeleton will be shown
		//source.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
		//source.setStringAttribute("displayType", "mesh")


		// set the hand configuration debug to display what is happening
		//source.setBoolAttribute("hand.debug", True)
		// decide which hand configuration to use
		source.setStringAttribute("hand.config", "allgestures")
		source.setStringAttribute("hand.config", "multiplegestures")
		source.setStringAttribute("hand.config", "singlegestures")
		// set the name of the motion to use
		source.setStringAttribute("hand.motionName", "Adil_Gesture1.bvh")
	}else{
		console.error("Scene does not exist!");
	}
}
		







