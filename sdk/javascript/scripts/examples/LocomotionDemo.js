function allReady(){
	console.log( "|--------------------------------------------|")
	console.log( "|         Starting Locomotion Demo           |")
	console.log( "|--------------------------------------------|")
	if(scene){
		// Add asset paths
		scene.addAssetPath('mesh', 'ChrMaarten')
		scene.addAssetPath('motion', 'ChrMaarten')
		scene.loadAssets()

		// Set scene parameters and camera
		console.log('Configuring scene parameters and camera')
		scene.setScale(1.0)
		scene.setBoolAttribute('internalAudio', true)
		defaultViewer.defaultViewer();
		var camera = Module.getCamera()
		camera.setEye(0, 19, 25)
		camera.setCenter(0, 18, 24)
		camera.setUpVector(new Module.SrVec(0, 1, 0))
		camera.setScale(1)
		camera.setFov(1.0472)
		camera.setFarPlane(100)
		camera.setNearPlane(0.1)
		camera.setAspectRatio(0.966897)
		scene.getPawn('camera').setPosition(new Module.SrVec(0, -5, 0))
		defaultInterface.defaultInterface();
		// Set joint map for Brad
		console.log('Setting up joint map for Brad')
		zebra2map.zebra2Map();
		var zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
		var bradSkeleton = scene.getSkeleton('ChrBrad.sk')
		zebra2Map.applySkeleton(bradSkeleton)
		zebra2Map.applyMotionRecurse('ChrBrad')

		var steerManager = scene.getSteerManager()
		// Setting up Brads
		console.log('Setting up Brads')
		var amount = 8
		var row = 0, column = 0;
		var offsetX = 0, offsetZ = 0;
		for(var i = 0; i < amount; ++i){
			var baseName = 'ChrBrad' + i
			var brad = scene.createCharacter(baseName, '')
			var bradSkeleton = scene.createSkeleton('ChrBrad.sk')
			brad.setSkeleton(bradSkeleton)
			// Set position logic
			var posX = (-100 * (5/2)) + 100 * column
			var posZ = ((-100 / Math.sqrt(amount)) * (amount/2)) + 100 * row
			var column = column + 1
			if(column >= 3){
				column = 0
				row = row + 1
			}
			var bradPos = new Module.SrVec((posX + offsetX)/100, 0, (posZ + offsetZ)/100)
			brad.setPosition(bradPos)
			// Set up standard controllers
			brad.createStandardControllers()
			// Set deformable mesh
			brad.setVec3Attribute('deformableMeshScale', .01, .01, .01);
			brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
			brad.setStringAttribute("displayType", "mesh")
			// Retarget character
			if( i== 0){ 
				BehaviorSetMaleLocomotion.setupBehaviorSet()
			}
			BehaviorSetMaleLocomotion.retargetBehaviorSet(baseName)
			// Set up steering	
			steerManager.setEnable(false)
			brad.setBoolAttribute('steering.pathFollowingMode', false)
			steerManager.setEnable(true)
			// Play default animation
			bml.execBML(baseName, '<body posture="ChrBrad@Idle01"/>')
		}
			
		var steeringGroup = []
		var pathfindingGroup = []
		// Assign groups
		console.log( 'Assigning Brads in groups')
		var names = scene.getCharacterNames();
		for(var i = 0; i < names.size(); ++i){
			var name = names.get(i)
			if(name.indexOf('ChrBrad') != -1){
				if(steeringGroup.length < amount/2)
					steeringGroup.push(scene.getCharacter(name))
				else{
					// Set pathfinding on
					scene.getCharacter(name).setBoolAttribute('steering.pathFollowingMode', true)
					pathfindingGroup.push(scene.getCharacter(name))
				}
			}
		}
		// Adding pawns to scene
		console.log( 'Adding pawns to scene')
		var target0 = scene.createPawn('target0')
		target0.setPosition(new Module.SrVec(-10, 0, -10))
		var target1 = scene.createPawn('target1')
		target1.setPosition(new Module.SrVec(-4, 0, 10))
					
		group1Reached = true
		group2Reached = true

		// Update to repeat paths
		last = 0
		canTime = true
		delay = 30
		var LocomotionDemo = Module.SBScript.extend("SBScript", {
			update : function(time){
				if(canTime){
					last = time
					canTime = false
					group1Reached = group2Reached = true
				}
				diff = time - last
				if(diff >= delay){
					diff = 0
					canTime = true
				}
				// Once group 1 completes path, do again
				if(group1Reached){
					for(var i = 0; i < steeringGroup.length; ++i){
						var brad = steeringGroup[i];
						bml.execBML(brad.getName(), '<locomotion manner="run" target="-10 10 -4 -10 target1 target0"/>')
					}
					group1Reached = false
				}		
					
				// Once group 2 completes path, do again
				if(group2Reached){
					for(var i = 0; i < pathfindingGroup.length; ++i){
						var brad = pathfindingGroup[i];
						bml.execBML(brad.getName(), '<locomotion manner="run" target="10 10 4 -10 4 10 10 -10"/>')
					}
					group2Reached = false
				}
			}
		});
	
		// Run the update script
		scene.removeScript('locomotiondemo')
		var locomotiondemo = new LocomotionDemo
		scene.addScript('locomotiondemo', locomotiondemo)
	}
	else{
		console.error("Scene does not exist!");
	}
}

