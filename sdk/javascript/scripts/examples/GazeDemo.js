function allReady(){
	if(scene != null){
		var mediaPath = '';
		console.log("|--------------------------------------------|");
		console.log("|             Starting Gaze Demo             |");
		console.log("|--------------------------------------------|");
		//Set Media Path
		scene.setMediaPath(mediaPath);
		//Add asset paths
		scene.addAssetPath('mesh', 'ChrBrad');
		scene.addAssetPath('mesh', 'ChrRachel');
		scene.addAssetPath("motion", "behaviorsets");
		scene.addAssetPath('motion', 'ChrBrad');
		scene.addAssetPath('motion', 'ChrRachel');

		scene.loadAssets()
		// Set scene parameters and camera
		console.log('Configuring scene parameters and camera');
		scene.setScale(1.0);
		scene.setBoolAttribute('internalAudio', true);
		defaultViewer.defaultViewer();
		var camera = Module.getCamera();
		camera.setEye(0, 1.68, 2.58);
		camera.setCenter(0, 0.89, -0.14);
		camera.setUpVector(new Module.SrVec(0, 1, 0));
		camera.setScale(1);
		camera.setFov(1.0472);
		camera.setFarPlane(100);
		camera.setNearPlane(0.1);
		camera.setAspectRatio(0.966897);
		scene.getPawn('camera').setPosition(new Module.SrVec(0, -5, 0));
		defaultInterface.defaultInterface();

		// Set joint map for Brad and Rachel
		console.log('Setting up joint map for Brad and Rachel');
		zebra2map.zebra2Map();
		var zebra2Map = scene.getJointMapManager().getJointMap('zebra2');
		var bradSkeleton = scene.getSkeleton('ChrBrad.sk');
		zebra2Map.applySkeleton(bradSkeleton);
		zebra2Map.applyMotionRecurse('ChrBrad');
		var rachelSkeleton = scene.getSkeleton('ChrRachel.sk');
		zebra2Map.applySkeleton(rachelSkeleton);
		zebra2Map.applyMotionRecurse('ChrRachel');
		
		// Setting up Brad and Rachel
		console.log('Setting up Brad');
		var brad = scene.createCharacter('ChrBrad', '');
		var bradSkeleton = scene.createSkeleton('ChrBrad.sk');
		brad.setSkeleton(bradSkeleton);
		var bradPos = new Module.SrVec(.35, 0, 0);
		brad.setPosition(bradPos);
		brad.setHPR(new Module.SrVec(-17, 0, 0));
		brad.createStandardControllers();
		// Deformable mesh
		brad.setVec3Attribute('deformableMeshScale', .01, .01, .01);
		brad.setStringAttribute('deformableMesh', 'ChrBrad.dae');
		
		BehaviorSetGestures.setupBehaviorSet()
		BehaviorSetGestures.retargetBehaviorSet('ChrBrad')
		bml.execBML('ChrBrad', '<body posture="ChrBrad@Idle01" ready="0" relax="0"/>')
		
		console.log('Setting up Rachel')
		var rachel = scene.createCharacter('ChrRachel', '')
		var rachelSkeleton = scene.createSkeleton('ChrRachel.sk')
		rachel.setSkeleton(rachelSkeleton)
		rachelPos = new Module.SrVec(-.35, 0, 0)
		rachel.setPosition(rachelPos)
		rachel.setHPR(new Module.SrVec(17, 0, 0))
		rachel.createStandardControllers()
		// Deformable mesh
		rachel.setVec3Attribute('deformableMeshScale', .01, .01, .01);
		rachel.setStringAttribute('deformableMesh', 'ChrRachel.dae')
		
		// setup gestures
		BehaviorSetFemaleGestures.setupBehaviorSet()
		BehaviorSetFemaleGestures.retargetBehaviorSet('ChrRachel')

		bml.execBML('ChrRachel', '<body posture="ChrConnor@IdleStand01" ready=".2" relax=".2"/>')

		// Add pawns in scene
		console.log('Adding pawn to scene')
		var gazeTarget = scene.createPawn('gazeTarget')
		gazeTarget.setPosition(new Module.SrVec(0.75, 1.54, 0.33))

		// Turn on GPU deformable geometry for all
		var names = scene.getCharacterNames();
		for(var i = 0; i < names.size(); ++i){
			var name = names.get(i);
			scene.getCharacter(name).setStringAttribute("displayType", "mesh")
		}

		// Make characters gaze at pawn
		bml.execBML('ChrRachel', '<gaze sbm:joint-range="EYES CHEST" target="gazeTarget"/>')
		
		// Variables to move pawn
		gazeX = -2
		gazeZ = 2
		dirX = 1
		dirZ = 1
		speed = 0.005
		lastTime = -8
		
		var GazeDemo = Module.SBScript.extend("SBScript", {
			update: function(time){
				if(gazeX > 2)
					dirX = -1
				else if (gazeX < -2)
					dirX = 1
				if(gazeZ > 2)
					dirZ = -1
				else if(gazeZ < -0)
					dirZ = 1
				gazeX = gazeX + speed * dirX
				gazeZ = gazeZ + speed * dirZ
				gazeTarget.setPosition(new Module.SrVec(gazeX, 2, gazeZ))
				//console.log("time:" + time);
				diff = time - lastTime
				if(diff > 10){
					diff = 0
					lastTime = time
					//Gaze at joints
					bml.execBMLAt(0, 'ChrBrad', '<gaze target="ChrRachel:base" sbm:joint-speed="800" sbm:joint-smooth="0.2"/>')
					bml.execBMLAt(2, 'ChrBrad', '<gaze target="ChrBrad:l_wrist" sbm:joint-speed="800" sbm:joint-smooth="0.2"/>')
					bml.execBMLAt(4, 'ChrBrad', '<gaze target="ChrBrad:r_ankle" sbm:joint-speed="800" sbm:joint-smooth="0.2"/>')
					bml.execBMLAt(6, 'ChrBrad', '<gaze target="ChrRachel:l_wrist" sbm:joint-speed="800" sbm:joint-smooth="0.2"/>')
					bml.execBMLAt(8, 'ChrBrad', '<gaze target="ChrRachel:spine4" sbm:joint-speed="800" sbm:joint-smooth="0.2"/>')
				}
			}
		
		});
		
		scene.removeScript('gazedemo')
		var gazedemo = new GazeDemo;
		scene.addScript('gazedemo', gazedemo);
		
	}else{
		console.log("SBScene does not exist");
	}
}
