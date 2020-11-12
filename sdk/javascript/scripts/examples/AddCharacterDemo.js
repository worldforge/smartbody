function allReady(){
	console.log( "|--------------------------------------------|")
	console.log( "|       	 Starting Character Demo	       |")
	console.log( "|--------------------------------------------|")
	if(scene){

		// Add asset paths
		scene.addAssetPath('mesh', 'ChrBrad')
		scene.addAssetPath('motion', 'ChrBrad')
		scene.addAssetPath('audio', 'sounds')
		scene.loadAssets()

		// Set scene parameters and camera
		console.log('Configuring scene parameters and camera')
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
		cameraPos = new Module.SrVec(0, 1.6, 10)
		scene.getPawn('camera').setPosition(cameraPos)
		defaultInterface.defaultInterface();

		// Set up joint map for Brad
		console.log('Setting up joint map and configuring Brad\'s skeleton')
		zebra2map.zebra2Map();
		var zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
		var bradSkeleton = scene.getSkeleton('ChrBrad.sk')
		zebra2Map.applySkeleton(bradSkeleton)
		zebra2Map.applyMotionRecurse('ChrBrad')
		
		// Establish lip syncing data set
		console.log('Establishing lip syncing data set')
		initDiphoneDefault.initDiphoneDefault()

		// Set up face definition
		console.log('Setting up face definition')
		// Brad's face definition
		var bradFace = scene.createFaceDefinition('ChrBrad')
		bradFace.setFaceNeutral('ChrBrad@face_neutral')
		bradFace.setAU(1,  "left",  "ChrBrad@001_inner_brow_raiser_lf")
		bradFace.setAU(1,  "right", "ChrBrad@001_inner_brow_raiser_rt")
		bradFace.setAU(2,  "left",  "ChrBrad@002_outer_brow_raiser_lf")
		bradFace.setAU(2,  "right", "ChrBrad@002_outer_brow_raiser_rt")
		bradFace.setAU(4,  "left",  "ChrBrad@004_brow_lowerer_lf")
		bradFace.setAU(4,  "right", "ChrBrad@004_brow_lowerer_rt")
		bradFace.setAU(5,  "both",  "ChrBrad@005_upper_lid_raiser")
		bradFace.setAU(6,  "both",  "ChrBrad@006_cheek_raiser")
		bradFace.setAU(7,  "both",  "ChrBrad@007_lid_tightener")
		bradFace.setAU(10, "both",  "ChrBrad@010_upper_lip_raiser")
		bradFace.setAU(12, "left",  "ChrBrad@012_lip_corner_puller_lf")
		bradFace.setAU(12, "right", "ChrBrad@012_lip_corner_puller_rt")
		bradFace.setAU(25, "both",  "ChrBrad@025_lips_part")
		bradFace.setAU(26, "both",  "ChrBrad@026_jaw_drop")
		bradFace.setAU(45, "left",  "ChrBrad@045_blink_lf")
		bradFace.setAU(45, "right", "ChrBrad@045_blink_rt")

		bradFace.setViseme("open",    "ChrBrad@open")
		bradFace.setViseme("W",       "ChrBrad@W")
		bradFace.setViseme("ShCh",    "ChrBrad@ShCh")
		bradFace.setViseme("PBM",     "ChrBrad@PBM")
		bradFace.setViseme("FV",      "ChrBrad@FV")
		bradFace.setViseme("wide",    "ChrBrad@wide")
		bradFace.setViseme("tBack",   "ChrBrad@tBack")
		bradFace.setViseme("tRoof",   "ChrBrad@tRoof")
		bradFace.setViseme("tTeeth",  "ChrBrad@tTeeth")
		
		
		
		console.log('Adding character into scene')
		// Set up Brad
		var brad = scene.createCharacter('ChrBrad', '')
		var bradSkeleton = scene.createSkeleton('ChrBrad.sk')
		brad.setSkeleton(bradSkeleton)
		// Set position
		var bradPos = new Module.SrVec(0, 0, 0)
		brad.setPosition(bradPos)
		// Set facing direction
		var bradFacing = new Module.SrVec(0, 0, 0)
		brad.setHPR(bradFacing)
		// Set face definition
		brad.setFaceDefinition(bradFace)
		// Set standard controller
		brad.createStandardControllers()
		// Deformable mesh
		brad.setVec3Attribute('deformableMeshScale', .01, .01, .01);
		brad.setStringAttribute('deformableMesh', 'ChrBrad.dae')

		// Lip syncing diphone setup
		brad.setStringAttribute('lipSyncSetName', 'default')
		brad.setBoolAttribute('usePhoneBigram', true)
		brad.setStringAttribute("voice", "audiofile")
		brad.setStringAttribute("voiceCode", ".")
		
		/*
		bml.execBML('ChrBrad', '<speech ref=\'test\'/>')
		import platform
		if platform.system() == "Windows":
			windowsVer = platform.platform()
			if windowsVer.find("Windows-7") == 0:
				brad.setVoiceCode('Microsoft|Anna')
			else:
				if windowsVer.find("Windows-8") == 0 or windowsVer.find("Windows-post2008Server") == 0:
					brad.setVoiceCode('Microsoft|David|Desktop')
		else: // non-Windows platform, use Festival voices
			brad.setVoiceCode('voice_kal_diphone')
		// note that Microsoft|Anna is the default Windows 7 voice.

		// Windows 8 includes other voices: 
		//Microsoft|David|Desktop
		*/
		
		// setup locomotion
		BehaviorSetMaleMocapLocomotion.setupBehaviorSet()
		BehaviorSetMaleMocapLocomotion.retargetBehaviorSet('ChrBrad')

		// setup gestures
		BehaviorSetGestures.setupBehaviorSet()
		BehaviorSetGestures.retargetBehaviorSet('ChrBrad')

		// setup reach 
		BehaviorSetReaching.setupBehaviorSet()
		BehaviorSetReaching.retargetBehaviorSet('ChrBrad')
		// Turn on GPU deformable geometry
		brad.setStringAttribute("displayType", "mesh")

		// Set up steering
		console.log('Setting up steering')
		var steerManager = scene.getSteerManager()
		steerManager.setEnable(false)
		brad.setBoolAttribute('steering.pathFollowingMode', false) // disable path following mode so that obstacles will be respected
		steerManager.setEnable(true)
		// Start the simulation
		console.log('Starting the simulation')
		sim.start()

		bml.execBML('ChrBrad', '<body posture="ChrBrad@Idle01"/>')
		bml.execBML('ChrBrad', '<saccade mode="listen"/>')
		//bml.execBML('ChrBrad', '<gaze sbm:handle="brad" target="camera"/>')

		//bml.execBML('*', '<blend name="mocapLocomotion" x="40" y="-70"/>')
		//bml.execBML('*', '<blend name="mocapStartingRight" x="40" y="-70"/>')
		sim.resume()

	}else{
		console.error("Scene does not exist!");
	}
}


