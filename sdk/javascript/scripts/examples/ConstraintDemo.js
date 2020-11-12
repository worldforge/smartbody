function allReady(){
	console.log( "|--------------------------------------------|")
	console.log( "|          Starting Constraint Demo          |")
	console.log( "|--------------------------------------------|")
	if(scene != null){
		scene.setScale(1.0)
		scene.setBoolAttribute("enableAlphaBlend", true)
		// Add asset paths
		scene.addAssetPath('mesh', 'ChrMaarten')
		scene.addAssetPath('motion', 'ChrMaarten')
		scene.loadAssets()
		

		// Runs the default viewer for camera
		defaultViewer.defaultViewer();
		var camera = Module.getCamera();
		camera.setEye(-0.193661, 2.12169, 4.92749)
		camera.setCenter(-0.0138556, 1.1562, 1.2083)
		camera.setFarPlane(100)
		camera.setNearPlane(0.1)
		defaultInterface.defaultInterface();


		// Set joint map for Brad
		console.log( 'Setting up joint map for Brad')
		zebra2map.zebra2Map();
		var zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
		var bradSkeleton = scene.getSkeleton('ChrBrad.sk')
		zebra2Map.applySkeleton(bradSkeleton)
		zebra2Map.applyMotionRecurse('ChrMaarten')

		console.log('Adding characters into scene')
		// Set up multiple Brads
		var posX = -1.45;
		for(var i = 0; i < 4; ++i){
			var baseName = 'ChrBrad' + i
			var brad = scene.createCharacter(baseName, '')
			var bradSkeleton = scene.createSkeleton('ChrBrad.sk')
			brad.setSkeleton(bradSkeleton)
			// Set position
			var bradPos = new Module.SrVec(posX + (i * 1.00), 0, 0)
			brad.setPosition(bradPos)
			// Set up standard controllers
			brad.createStandardControllers()
			// Set deformable mesh
			brad.setVec3Attribute('deformableMeshScale', .01, .01, .01);
			brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
			brad.setStringAttribute("displayType", "mesh")
			// Play idle animation
			bml.execBML(baseName, '<body posture="ChrBrad@Idle01"/>')
			// Retarget character
			if(i == 0) 
				BehaviorSetReaching.setupBehaviorSet()
			BehaviorSetReaching.retargetBehaviorSet(baseName)	
		}
		// Set camera position
		scene.getPawn('camera').setPosition(new Module.SrVec(0, -.50, 0))

		// Set up multiple pawns
		console.log( 'Adding pawns into scene')
		
		for(var i = 0; i < 5; ++i){
			var baseName = 'pawn' + i
			console.log(baseName);
			var pawn = scene.createPawn(baseName)
			pawn.setStringAttribute('collisionShape', 'sphere')
			var collisionShapeScale = new Module.SrVec(.05,.05,.05)
			pawn.getAttribute('collisionShapeScale').setValue(collisionShapeScale)
		}
		console.log("list pawns")
		for(var i = 0; i < scene.getNumPawns(); ++i)
			console.log(scene.getPawnNames().get(i))
		
		// Set pawn position and collision scale
		console.log( 'Set pawn position')
		scene.getPawn('pawn0').setPosition(new Module.SrVec(1.30, 1.30, .15))
		scene.getPawn('pawn1').setPosition(new Module.SrVec(1.90, 1.30, .15))
		scene.getPawn('pawn2').setPosition(new Module.SrVec(-1.00, 1.75, 1.50))
		scene.getPawn('pawn3').setPosition(new Module.SrVec(-1.10, 1.55, .43))
		scene.getPawn('pawn4').setPosition(new Module.SrVec(-.10, 1.55, .43))

		// 2 on the left touch pawns
		console.log( 'ChrBrad0 and ChrBrad1 touching pawns')
		bml.execBMLAt(1, 'ChrBrad0', '<sbm:reach sbm:action="touch" target="pawn3"/>')
		bml.execBMLAt(1, 'ChrBrad1', '<sbm:reach sbm:action="touch" target="pawn4"/>')
		// Set constraint for ChrBrad0
		bml.execBMLAt(2, 'ChrBrad0', '<sbm:constraint effector="l_wrist" sbm:effector-root="l_sternoclavicular" sbm:handle="cbrad0" target="pawn3" sbm:fade-in="0.5"/>')

		// 2 on the right use constraint
		console.log( 'ChrBrad3 using contraints')
		bml.execBMLAt(2, 'ChrBrad3', '<sbm:constraint effector="r_wrist" sbm:effector-root="r_sternoclavicular" sbm:handle="cbrad3" target="pawn0" sbm:fade-in="0.5"/>')
		bml.execBMLAt(2, 'ChrBrad3', '<sbm:constraint effector="l_wrist" sbm:effector-root="l_sternoclavicular" sbm:handle="cbrad3" target="pawn1" sbm:fade-in="0.5"/>')

		// Make all characters gaze at pawn2
		bml.execBMLAt(5, '*', '<gaze target="pawn2"/>')

		// Move pawn around
		gazeX = -2.00
		dir = 1
		speed = .01
		pawn2 = scene.getPawn('pawn2')
		
		var ConstraintDemo = Module.SBScript.extend("SBScript", {
			update: function(time){
				if(gazeX > 2.00)
					dir = -1
				else if(gazeX < -2.00)
					dir = 1
				gazeX = gazeX + speed * dir
				// Set pawn position
				pawn2.setPosition(new Module.SrVec(gazeX, 1.75, 1.50 * Math.sin(time)))
			}
		});

		// Run the update script
		scene.removeScript('constraintdemo')
		var constraintdemo = new ConstraintDemo
		scene.addScript('constraintdemo', constraintdemo)
				
	}else{
		console.log("SBScene does not exist");
	}
}

