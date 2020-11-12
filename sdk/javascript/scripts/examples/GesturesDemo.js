function allReady(){
	if(scene){
		console.log("|--------------------------------------------|")
		console.log("|          Starting Gestures Demo            |")
		console.log("|--------------------------------------------|")

		// Add asset paths
		scene.addAssetPath('mesh', 'ChrBrad')
		scene.addAssetPath('motion', 'ChrBrad')
		scene.loadAssets()

		// Set scene parameters
		console.log('Configuring scene parameters and camera')
		scene.setScale(1.0)
		scene.setBoolAttribute('internalAudio', true)
		defaultViewer.defaultViewer();
		var camera = Module.getCamera()
		camera.setEye(-1.55829, 1.51057, 2.48738)
		camera.setCenter(-1.48597, 1.23478, 1.36451)
		camera.setUpVector(new Module.SrVec(0, 1, 0))
		camera.setScale(1)
		camera.setFov(1.0472)
		camera.setFarPlane(100)
		camera.setNearPlane(0.1)
		camera.setAspectRatio(0.966897)
		scene.getPawn('camera').setPosition(new Module.SrVec(0, -2, 0))
		defaultInterface.defaultInterface();

		// Set joint map for Brad and Rachel
		console.log('Setting up joint map for Brad and Rachel')
		zebra2map.zebra2Map();
		var zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
		var bradSkeleton = scene.getSkeleton('ChrBrad.sk')
		zebra2Map.applySkeleton(bradSkeleton)
		zebra2Map.applyMotionRecurse('ChrBrad')
		var rachelSkeleton = scene.getSkeleton('ChrRachel.sk')
		zebra2Map.applySkeleton(rachelSkeleton)
		zebra2Map.applyMotionRecurse('ChrRachel')

		// Setting up Brads and Rachels
		console.log('Setting up Brads')
		var bradPosX = -145

		// prepare male gestures
		BehaviorSetGestures.setupBehaviorSet()

		for(var i = 0; i < 2; ++i){
			var baseName = 'ChrBrad' + i
			var brad = scene.createCharacter(baseName, '')
			var bradSkeleton = scene.createSkeleton('ChrBrad.sk')
			brad.setSkeleton(bradSkeleton)
			// Set position
			bradPos = new Module.SrVec((bradPosX + (i * 100))/100, 0, 0)
			brad.setPosition(bradPos)
			brad.createStandardControllers()
			// Set deformable mesh
			brad.setVec3Attribute('deformableMeshScale', .01, .01, .01);
			brad.setStringAttribute('deformableMesh', 'ChrBrad.dae')
			brad.setStringAttribute("displayType", "mesh")
			// Play idle animation
			bml.execBML(baseName, '<body posture="ChrBrad@Idle01" ready="0" relax="0"/>')
			BehaviorSetGestures.retargetBehaviorSet(baseName)
		}
		// Turn on GPU deformable geometry for all+
		//var names = scene.getCharacterNames()
		//for(var i = 0; i < scene.getNumCharacters(); ++i){
			//var name = names.get(i);
			//scene.getCharacter(name).setStringAttribute("displayType", "mesh")
		//} 
		
		last = 0
		delay = 5
		var GesturesDemo = Module.SBScript.extend("SBScript", {
			update : function(time){
				var diff = time - last
				if (diff >= delay){
					// Brad gestures
					// With pauses (manual/hardcoded)
					bml.execBML('ChrBrad0', '<gesture id="cbrad0" lexeme="DEICTIC" type="YOU" stroke="0" relax="2" sbm:joint-range="l_shoulder" sbm:frequency="0.09" sbm:scale="0.04"/>')
					bml.execBML('ChrBrad0', '<gesture id="cbrad0" lexeme="METAPHORIC" type="OBLIGATION" mode="LEFT_HAND" stroke="3" relax="5" sbm:joint-range="l_shoulder" sbm:frequency="0.09" sbm:scale="0.04"/>')
					bml.execBML('ChrBrad0', '<gesture id="cbrad0" lexeme="DEICTIC" type="ME" stroke="6" relax="8" sbm:joint-range="l_shoulder" sbm:frequency="0.09" sbm:scale="0.04"/>')
					// No pauses
					bml.execBML('ChrBrad1', '<gesture id="cbrad1" lexeme="DEICTIC" type="YOU" stroke="0" relax="2"/>')
					bml.execBML('ChrBrad1', '<gesture id="cbrad1" lexeme="METAPHORIC" type="OBLIGATION" mode="LEFT_HAND" stroke="2" relax="4"/>')
					bml.execBML('ChrBrad1', '<gesture id="cbrad1" lexeme="DEICTIC" type="ME" stroke="4"/>')	
					// Reset timer
					last = time
					
				}
			}
		});
		
		scene.removeScript('gesturesdemo')
		var gesturesdemo = new GesturesDemo
		scene.addScript('gesturesdemo', gesturesdemo)
	}else{
		console.error("Scene does not exist!");
	}
}
