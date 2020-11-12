function allReady(){
	console.log( "|--------------------------------------------|")
	console.log( "|        Starting Speech/Face Demo           |")
	console.log( "|--------------------------------------------|")

	if(scene != null){
		// Add asset paths
		scene.addAssetPath('mesh', 'mesh')
		scene.addAssetPath('motion', 'ChrMaarten')
		scene.loadAssets()

		// Set scene parameters and camera
		console.log( 'Configuring scene parameters and camera')
		scene.setScale(1.0)
		scene.setBoolAttribute('internalAudio', true)
		scene.setBoolAttribute("enableAlphaBlend", true)
		defaultViewer.defaultViewer();
		var camera = Module.getCamera()
		camera.setEye(0.08, 1.55, 0.79)
		camera.setCenter(0.08, 1.45, 0.0)
		camera.setUpVector(new Module.SrVec(0, 1, 0))
		camera.setScale(1)
		camera.setFov(1.0472)
		camera.setFarPlane(100)
		camera.setNearPlane(0.1)
		camera.setAspectRatio(0.966897)
		scene.getPawn('camera').setPosition(new Module.SrVec(0, -5, 0))
		defaultInterface.defaultInterface();
		// Set joint map for Brad
		console.log( 'Setting up joint map for Brad')
		zebra2map.zebra2Map();
		var zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
		var bradSkeleton = scene.getSkeleton('ChrBrad.sk')
		zebra2Map.applySkeleton(bradSkeleton)
		zebra2Map.applyMotionRecurse('ChrMaarten')

		// Set up 3 Brads
		console.log( 'Adding characters into scene')
		var posX = 0.0
		for(var i = 0; i < 1; ++i){
			var baseName = 'ChrBrad' + i
			var brad = scene.createCharacter(baseName, '')
			var bradSkeleton = scene.createSkeleton('ChrBrad.sk')
			brad.setSkeleton(bradSkeleton)
			// Set position
			var bradPos = new Module.SrVec((posX + (i * 55))/100, 0, 0)
			brad.setPosition(bradPos)
			// Set up standard controllers
			brad.createStandardControllers()
			// Set deformable mesh
			brad.setVec3Attribute('deformableMeshScale', .01, .01, .01);
			brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
			// Play idle animation
			bml.execBML(baseName, '<body posture="ChrBrad@Idle01"/>')
		}	
		// Set proper facing angle
		//scene.getCharacter('ChrBrad0').setHPR(new Module.SrVec(25, 0, 0))
		//scene.getCharacter('ChrBrad2').setHPR(new Module.SrVec(-25, 0, 0))

		// Turn on GPU deformable geometry for all
		var names = scene.getCharacterNames()
		for(var i = 0; i < names.size(); ++i){
			var name = names.get(i)
			scene.getCharacter(name).setStringAttribute("displayType", "mesh")
		}
		// Talk
		bml.execBML('ChrBrad0', '<saccade mode="talk"/>')
		// Listen
		//bml.execBML('ChrBrad1', '<saccade mode="listen"/>')
		// Think
		//bml.execBML('ChrBrad2', '<saccade mode="think"/>')
	}else{
		console.log("SBScene does not exist");
	}
}


