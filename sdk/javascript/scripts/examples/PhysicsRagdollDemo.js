function allReady(){
	console.log( "|--------------------------------------------|")
	console.log( "|      Starting Physics Ragdoll Demo         |")
	console.log( "|--------------------------------------------|")
	if(scene){
		// Add asset paths
		scene.addAssetPath('mesh', 'mesh')
		scene.addAssetPath('motion', 'sbm-common/common-sk')
		scene.loadAssets()

		// Run required scripts
		initParamAnimation.initParamAnimation();

		// Set scene parameters and camera
		console.log( 'Configuring scene parameters and camera')
		scene.setBoolAttribute('internalAudio', true)

		scene.setScale(0.01)
		var camera = Module.getCamera()
		camera.setEye(-20, 187, 443)
		camera.setCenter(-20, 136, 151)
		camera.setUpVector(new Module.SrVec(0, 1, 0))
		camera.setScale(1)
		camera.setFov(1.0472)
		camera.setFarPlane(5000.0)
		camera.setNearPlane(5.0)
		camera.setAspectRatio(1.02)
		defaultInterface.defaultInterface();


		// Setting up characters
		console.log( 'Setting up characters')
		var brad1 = scene.createCharacter('brad1', '')
		var brad1Skeleton = scene.createSkeleton('common.sk')
		brad1.setSkeleton(brad1Skeleton)
		brad1.setPosition(new Module.SrVec(0, 200, 0))
		brad1.createStandardControllers()
		brad1.setStringAttribute('deformableMesh', 'Brad.dae')
		bml.execBML('brad1', '<body posture="HandsAtSide_Motex"/>')

		var brad2 = scene.createCharacter('brad2', '')
		var brad2Skeleton = scene.createSkeleton('common.sk')
		brad2.setSkeleton(brad2Skeleton)
		brad2.setPosition(new Module.SrVec(0, 150, 200))
		brad2.createStandardControllers()
		brad2.setStringAttribute('deformableMesh', 'Brad.dae')
		bml.execBML('brad2', '<body posture="HandsAtSide_Motex"/>')

		var bradList = []
		var names = scene.getCharacterNames();
		for(var i = 0; i < names.size(); ++i){
			var name = names.get(i)
			bradList.push(scene.getCharacter(name))
			scene.getCharacter(name).setStringAttribute("displayType", "mesh")
		}
		// Adding pawns in scene
		console.log( 'Adding pawns in the scene')
		// Constraint
		var constraint = scene.createPawn('constraint')
		constraint.setStringAttribute('collisionShape', 'sphere')
		constraint.getAttribute('collisionShapeScale').setValue(new Module.SrVec(10, 10, 10))
		constraint.setPosition(new Module.SrVec(0, 240, 0))
		// Ball moved by force
		var ball = scene.createPawn('ball')
		ball.setStringAttribute('collisionShape', 'sphere')
		ball.getAttribute('collisionShapeScale').setValue(new Module.SrVec(15, 15, 15))
		ball.setPosition(new Module.SrVec(75, 150, 0))
		// Obstacles for ragdoll to fall on
		var wall1 = scene.createPawn('wall1')
		wall1.setStringAttribute('collisionShape', 'box')
		wall1.getAttribute('collisionShapeScale').setValue(new Module.SrVec(20, 80, 50))
		wall1.setPosition(new Module.SrVec(-50, 80, -40))
		var wall2 = scene.createPawn('wall2')
		wall2.setStringAttribute('collisionShape', 'box')
		wall2.getAttribute('collisionShapeScale').setValue(new Module.SrVec(60, 50, 40))
		wall2.setPosition(new Module.SrVec(-120, 70, 160))
		var wall3 = scene.createPawn('wall3')
		wall3.setStringAttribute('collisionShape', 'box')
		wall3.getAttribute('collisionShapeScale').setValue(new Module.SrVec(50, 50, 50))
		wall3.setPosition(new Module.SrVec(150, 80, -20))
		var wall4 = scene.createPawn('wall4')
		wall4.setStringAttribute('collisionShape', 'box')
		wall4.getAttribute('collisionShapeScale').setValue(new Module.SrVec(50, 50, 50))
		wall4.setPosition(new Module.SrVec(90, 100, 150))

		// Configuring character physics
		console.log( 'Configuring character physics')
		var phyManager = scene.getPhysicsManager()
		phyManager.getPhysicsEngine().setBoolAttribute('enable', true)
		// Brad1 and Brad2
		brad1.getAttribute('createPhysics').setValue()
		brad2.getAttribute('createPhysics').setValue()
		phyManager.getPhysicsCharacter('brad1').setBoolAttribute('usePD', true)
		phyManager.getPhysicsCharacter('brad2').setBoolAttribute('usePD', true)
			
		constrainChr('brad1', 'l_wrist', 'constraint')

		// Set up pawn physics
		console.log( 'Setting up pawn physics')
		ball.getAttribute('createPhysics').setValue()
		wall1.getAttribute('createPhysics').setValue()
		wall2.getAttribute('createPhysics').setValue()
		wall3.getAttribute('createPhysics').setValue()
		wall4.getAttribute('createPhysics').setValue()

		// Forces
		forceX = forceZ = 150
		bradRagdoll = false
		
		// Enable collision
		//collisionManager = scene.getCollisionManager()
		//collisionManager.setStringAttribute('collisionResolutionType', 'default')
		//collisionManager.setEnable(true)
	
		curZ = 0
		curX = 0
		amountZ = amountX = -1
		speed = 0.05
		last = 0
		canTime = true
		delay = 5
		started = false
		
		var PhysicsRagdollDemo = Module.SBScript.extend("SBScript", {
			update : function(time){
				if(canTime){
					last = time
					canTime = false
				}
				diff = time - last
				if(diff >= delay){
					diff = 0
					canTime = true
				}
				// Trigger once
				if(canTime && !started){
					started = true
					ball.setBoolAttribute('enablePhysics', true)
					phyManager.getPhysicsCharacter('brad1').setBoolAttribute('enable', true)
					bml.execBML('brad1', '<body posture="Walk"/>')
				}
				// If time's up, do action
				if(canTime){
					forceX = Math.random() * (200.0 + 200.0 ) - 200.0;
					forceZ = Math.random() * (200.0 + 200.0 ) - 200.0;
					//console.log("ForceX = " + forceX + " ForceZ = " + forceZ);
					phyManager.applyForceToPawn('ball', new Module.SrVec(forceX, 0, forceZ))
					randX = Math.random() * (50000.0 + 50000.0 ) - 50000.0;
					randY = Math.random() * (50000.0 + 50000.0 ) - 50000.0;
					randZ = Math.random() * (50000.0 + 50000.0 ) - 50000.0;
					//console.log("randX = " + randX + " randY = " + randY + " randZ = " + randZ);
					phyManager.applyForceToCharacter('brad1', 'spine1', new Module.SrVec(randX, randY, randZ))
					if(!bradRagdoll){
						bradRagdoll = true
						// Brad 2
						phyManager.getPhysicsCharacter(brad2.getName()).setBoolAttribute('enable', true)
					}
					else if(bradRagdoll){
						bradRagdoll = false
						phyManager.getPhysicsCharacter(brad2.getName()).setBoolAttribute('enable', false)
						// Randomize position and rotation
						randX = Math.random() * (150.0 + 150.0 ) - 150.0;
						randY = Math.random() * (250.0 - 200.0 ) + 200.0;
						randZ = Math.random() * (150.0 - 0.0 ) + 0.0;
						randH = Math.random() * (180.0 + 180.0 ) - 180.0;
						randP = Math.random() * (180.0 + 180.0 ) - 180.0;
						randR = Math.random() * (180.0 + 180.0 ) - 180.0;
						//console.log("randX = " + randX + " randY = " + randY + " randZ = " + randZ);
						//console.log("randH = " + randH + " randP = " + randP + " randR = " + randR);
						scene.getCharacter(brad2.getName()).setPosition(new Module.SrVec(randX, randY, randZ))
						scene.getCharacter(brad2.getName()).setHPR(new Module.SrVec(randH, randP, randR))
					}
				}
			}
		})
					
		// Run the update script
		scene.removeScript('physicsragdolldemo')
		var physicsragdolldemo = new PhysicsRagdollDemo
		scene.addScript('physicsragdolldemo', physicsragdolldemo)
		
	}else{
		console.error("Scene does not exist!");
	}
}
function constrainChr(chr, joint, object){
	//Name of character, joint name of character, object name to act as constraint
	var phyManager = scene.getPhysicsManager()
	var bodyLink = phyManager.getJointObj(chr, joint)
	bodyLink.setBoolAttribute('constraint', true)
	var bodyLink = phyManager.getJointObj(chr, joint)
	bodyLink.setStringAttribute('constraintTarget', object)
}	

