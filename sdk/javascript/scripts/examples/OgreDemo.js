function allReady(){
	console.log("|--------------------------------------------|")
	console.log("|         Starting Ogre Demo           |")
	console.log("|--------------------------------------------|")
	if(scene){
		scene.setScale(0.1)
		// Add asset paths
		scene.addAssetPath('mesh', 'Sinbad')
		scene.addAssetPath('motion', 'Sinbad')
		scene.addAssetPath('motion', 'behaviorsets')
		scene.loadAssets()

		// Set scene parameters and camera
		//scene.getPawn('camera').setPosition(SrVec(0, -5, 0))

		// Set joint map for Sinbad
		console.log('Setting up joint map for Brad')
		orgeSinbadMap.orgeSinbadMap();
		var sinbadSkName = 'Sinbad.skeleton.xml'
		var jointMapManager = scene.getJointMapManager()
		var sinbadMap = jointMapManager.getJointMap('Sinbad.skeleton.xml')
		var ogreSk = scene.getSkeleton(sinbadSkName)
		sinbadMap.applySkeleton(ogreSk)

		// Behavior set setup
		//scene.run('behaviorsetup.py')

		// Animation setup
		//scene.run('init-param-animation.py')
		var steerManager = scene.getSteerManager()

		// Setting up Sinbad
		console.log('Setting up Sinbad')
		var sinbadName = 'sinbad'
		var sinbad = scene.createCharacter(sinbadName,'')
		var sinbadSk = scene.createSkeleton(sinbadSkName)
		sinbad.setSkeleton(sinbadSk)
		var sinbadPos = new Module.SrVec(0,5.16, 0)
		sinbad.setPosition(sinbadPos)
		sinbad.createStandardControllers()
		sinbad.setStringAttribute('deformableMesh', 'Sinbad.mesh.xml')
		// setup locomotion
		BehaviorSetMaleLocomotion.setupBehaviorSet()
		BehaviorSetMaleLocomotion.retargetBehaviorSet(sinbadName)
		// setup reaching
		BehaviorSetReaching.setupBehaviorSet()
		BehaviorSetReaching.retargetBehaviorSet(sinbadName)
		sinbad.setStringAttribute("displayType", "mesh")

		console.log('Configuring scene parameters and camera')
		scene.setBoolAttribute('internalAudio', true)
		//defaultViewer.defaultViewer()
		var camera = Module.getCamera()
		camera.setEye(0, 5.98, 13.44)
		camera.setCenter(1.0, 1.7, -39.5)
		camera.setUpVector(new Module.SrVec(0, 1, 0))
		camera.setScale(1)
		camera.setFov(1.0472)
		camera.setFarPlane(100)
		camera.setNearPlane(0.1)
		camera.setAspectRatio(1.02)
		defaultInterface.defaultInterface();

		var stateManager = scene.getStateManager()	
		var blendNames = stateManager.getBlendNames();
		for(var i = 0; i < blendNames.size(); ++i)
			console.log("Blend Name:" + blendNames.get(i));
		sim.start()
		//bml.execBML('*', '<blend name="allLocomotion" x="40" y="-70"/>')
		//bml.execBML('*', '<blend name="allStartingRight" x="40" y="-70"/>')
		//bml.execBML('*', '<blend name="allStartingLeft" x="40" y="-70"/>')
		//bml.execBML('*', '<blend name="allStep" x="40" y="-70"/>')
		//bml.execBML('*', '<blend name="allIdleTurn" x="40" y="-70"/>')
		bml.execBML(sinbadName, '<body posture="ChrUtah_Idle001"/>')
		sim.resume()
		
	}else{
		console.error("Scene does not exist!");
	}
}


