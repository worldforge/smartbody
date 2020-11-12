function allReady(){
		console.log("|--------------------------------------------|")
		console.log("|          Starting Physics Demo             |")
		console.log("|--------------------------------------------|")
		
	if(scene){
		// Add asset paths
		scene.addAssetPath('mesh', 'mesh')
		scene.addAssetPath('motion', 'sbm-common/common-sk/motion')
		scene.loadAssets()
		scene.loadAssetsFromPath('sbm-common/common-sk/common.sk')

		scene.setScale(0.01)
		// Set simulation fps
		scene.getSimulationManager().setSimFps(60)
		
		//scene.run('default-viewer.py')
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

		//scene.getPawn('camera').setPosition(SrVec(0, -20, 0))


		// Set scene parameters and camera
		console.log( 'Configuring scene parameters and camera')
		scene.setBoolAttribute('internalAudio', true)

		// Setting up characters
		console.log( 'Setting up characters')
		// Brad
		var brad = scene.createCharacter('brad', '')
		var bradSkeleton = scene.createSkeleton('common.sk')
		brad.setSkeleton(bradSkeleton)
		brad.setPosition(new Module.SrVec(-150, 200, 20))
		brad.createStandardControllers()
		brad.setStringAttribute('deformableMesh', 'Brad.dae')
		bml.execBML('brad', '<body posture="HandsAtSide_Motex"/>')
		// Elder
		var elder = scene.createCharacter('elder', '')
		var elderSkeleton = scene.createSkeleton('common.sk')
		elder.setSkeleton(elderSkeleton)
		elder.setPosition(new Module.SrVec(-75, 102, 10))
		elder.createStandardControllers()
		elder.setStringAttribute('deformableMesh', 'Elder.dae')
		bml.execBML('elder', '<body posture="LHandOnHip_Motex"/>')
		// Doctor
		var doctor = scene.createCharacter('doctor', '')
		var doctorSkeleton = scene.createSkeleton('common.sk')
		doctor.setSkeleton(doctorSkeleton)
		doctor.setPosition(new Module.SrVec(75, 102, 0))
		doctor.setHPR(new Module.SrVec(90, 0, 0))
		doctor.createStandardControllers()
		doctor.setStringAttribute('deformableMesh', 'Doctor.dae')

		bml.execBML('doctor', '<body posture="LHandOnHip_Motex"/>')
		// Brad2
		var brad2 = scene.createCharacter('brad2', '')
		var brad2Skeleton = scene.createSkeleton('common.sk')
		brad2.setSkeleton(brad2Skeleton)
		brad2.setPosition(new Module.SrVec(135, 102, 0))
		brad2.setHPR(new Module.SrVec(-90, 0, 0))
		brad2.createStandardControllers()
		brad2.setStringAttribute('deformableMesh', 'Brad.dae')

		// setup reach 
		BehaviorSetReaching.setupBehaviorSet()
		BehaviorSetReaching.retargetBehaviorSet('doctor')
		BehaviorSetReaching.retargetBehaviorSet('brad2')

		bml.execBML('brad2', '<body posture="HandsAtSide_Motex"/>')

		// Turn on GPU deformable geometry for all
		var names = scene.getCharacterNames();
		for(var i = 0; i < names.size(); ++i){
			var name = names.get(i);
			scene.getCharacter(name).setStringAttribute("displayType", "mesh")
		}
		// Setting up pawns
		console.log( 'Adding pawns to scene')
		for(var i = 0; i < 6; ++i){
			baseName = 'pawn' + i
			pawn = scene.createPawn(baseName)
			pawn.setStringAttribute('collisionShape', 'sphere')
			collisionShapeScale = new Module.SrVec(0, 0, 0)
			pawn.getAttribute('collisionShapeScale').setValue(collisionShapeScale)
		}
			
		// Set pawn positions
		console.log( 'Setting pawn positions')
		var pawn0 = scene.getPawn('pawn0')
		pawn0.setPosition(new Module.SrVec(-150, 240, 20))
		pawn0.getAttribute('collisionShapeScale').setValue(new Module.SrVec(5, 5, 5))
		var pawn1 = scene.getPawn('pawn1')
		pawn1.setPosition(new Module.SrVec(-75, 150, 20))
		pawn1.getAttribute('collisionShapeScale').setValue(new Module.SrVec(5, 5, 5))
		var pawn2 = scene.getPawn('pawn2')
		pawn2.setPosition(new Module.SrVec(75, 150, 10))
		var pawn3 = scene.getPawn('pawn3')
		pawn3.setPosition(new Module.SrVec(75, 150, -10))
		var pawn4 = scene.getPawn('pawn4')
		pawn4.setPosition(new Module.SrVec(135, 150, 10))
		var pawn5 = scene.getPawn('pawn5')
		pawn5.setPosition(new Module.SrVec(135, 150, -10))

		// Configure character physics
		console.log( 'Configuring character physics')
		var phyManager = scene.getPhysicsManager()
		phyManager.getPhysicsEngine().setBoolAttribute('enable', true)
		
		for(var i = 0; i < names.size(); ++i){
			var name = names.get(i)
			var character = scene.getCharacter(name)
			character.getAttribute('createPhysics').setValue()
			phyManager.getPhysicsCharacter(name).setBoolAttribute('usePD', true)
		}
		// Set up constraints
		constrainChr('brad', 'l_wrist', 'pawn0')
		constrainChr('doctor', 'spine1', '')
		constrainChr('doctor', 'r_wrist', '')
		constrainChr('doctor', 'l_wrist', '')
		constrainChr('doctor', 'r_ankle', '')
		constrainChr('doctor', 'l_ankle', '')
		constrainChr('brad2', 'spine1', '')
		constrainChr('brad2', 'r_wrist', '')
		constrainChr('brad2', 'l_wrist', '')
		constrainChr('brad2', 'r_ankle', '')
		constrainChr('brad2', 'l_ankle', '')
		constrainChr('elder', 'spine1', '')
		constrainChr('elder', 'l_wrist', '')
		constrainChr('elder', 'r_wrist', '')
		constrainChr('elder', 'l_ankle', '')
		constrainChr('elder', 'r_ankle', '')

		// Set up pawn physics
		pawn1.getAttribute('createPhysics').setValue()

		bradX = -150
		bradCur = -1
		curZ = 20
		curX = -75
		amountZ = amountX = -1
		speed = 0.2
		last = 0
		canTime = true
		delay = 6
		started = false
		
		var PhysicsDemo = Module.SBScript.extend("SBScript", {
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
				if (canTime && !started){
					started = true
					phyManager.getPhysicsCharacter('brad').setBoolAttribute('enable', true)
					phyManager.getPhysicsCharacter('elder').setBoolAttribute('enable', true)
					phyManager.getPhysicsCharacter('doctor').setBoolAttribute('enable', true)
					pawn1.setBoolAttribute('enablePhysics', true)
					pawn1.setBoolAttribute('enablePhysics', false)
					
					pawn1.setPosition(new Module.SrVec(-75, 150, 20))
					bml.execBML('brad', '<body posture="Walk"/>')
				}
				// If time's up, do action
				if(canTime){
					bml.execBML('brad', '<head repeats="5" velocity="0.75" type="SHAKE"/>')
					boxingLogic()
				}
				// Elder pawn
				pawn1.setPosition(new Module.SrVec(curX, 150, curZ))
				curX = curX + speed * amountX
				curZ = curZ + speed * amountZ
				if(curX < -90) amountX = 1
				if(curX > -60) amountX = -1
				if(curZ < 9) amountZ = 1
				if(curZ > 20) amountZ = -1
				// Brad pawn
				pawn0.setPosition(new Module.SrVec(bradX, 240, 20))
				bradX = bradX + speed * bradCur
				if(bradX < -170) bradCur = 1
				if(bradX > -130) bradCur = -1
			}
		});
		
		var CollisionHandler = Module.SBEventHandler.extend("SBEventHandler",{
			executeAction : function(ev){
				var params = ev.getParameters()
				var list = params.split(" ")
				//console.log(list);
				if (list.length > 2){
					// Elder collision
					if(list[1] == 'elder'){
						if(list[2] == 'pawn1'){
							//console.log( '%s collided with %s' % (list[1], list[2])
							bml.execBML('elder', '<gaze target="pawn1"/>')
							bml.execBML('elder', '<gaze target="elder" start="2"/>')
						}
					}
					// Doctor doesn't get collision events for some reason
					if(list[1] == 'brad2'){
						// Brad hits doctor
						if(list[2].indexOf('wrist') != -1 && currentTurn == 'doctor'){
							var target = list[3] + ' ' + list[4] + ' ' + list[5]
							bml.execBML('doctor', '<gaze sbm:target-pos="' + target + '"/>')
							bml.execBML('doctor', '<gaze target="doctor" start="2"/>')
						}
						// Doctor hits brad
						else if(list[2].indexOf('_') != -1 && currentTurn == 'brad2'){
							var target = list[3] + ' ' + list[4] + ' ' + list[5]
							bml.execBML('brad2', '<gaze sbm:target-pos="' + target + '"/>')
							bml.execBML('brad2', '<gaze target="brad2" start="2"/>')
						}
					}
				}
				
			}
		});
			
		var collisionHdl = new CollisionHandler
		var evtMgr = scene.getEventManager()
		evtMgr.addEventHandler('collision', collisionHdl)
					
		// Run the update script
		scene.removeScript('physicsdemo')
		var physicsdemo = new PhysicsDemo
		scene.addScript('physicsdemo', physicsdemo)
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

// Current turn
currentTurn = 'brad2'		
function boxingLogic(){
	var phyManager = scene.getPhysicsManager()
	// Brad's turn, toggle physics and play reach
	//Math.floor(Math.random() * (max - min + 1)) + min;
	if(currentTurn == 'brad2'){
		phyManager.getPhysicsCharacter('brad2').setBoolAttribute('enable', false)
		phyManager.getPhysicsCharacter('doctor').setBoolAttribute('enable', true)

		var randNum = Math.floor(Math.random() * (1.0 - 0.0 + 1.0)) + 0.0;
		var randDodge = Math.floor(Math.random() * (2.0 - 0.0 + 1.0)) + 0.0;
		//console.log("RandomNum = " + randNum + " randomDodge = " + randDodge);
		if(randNum == 0)
			bml.execBML('brad2', '<sbm:reach sbm:action="touch" sbm:reach-finish="true" sbm:reach-type="left" target="pawn2"/>')
		else if (randNum == 1)
			bml.execBML('brad2', '<sbm:reach sbm:action="touch" sbm:reach-finish="true" sbm:reach-type="right" target="pawn3"/>')
		if(randDodge == 2)
			phyManager.getPhysicsCharacter('doctor').setBoolAttribute('enable', false)
		currentTurn = 'doctor'
	}
	// Doctor's turn, toggle physics and play reach
	else if(currentTurn == 'doctor'){
		phyManager.getPhysicsCharacter('doctor').setBoolAttribute('enable', false)
		phyManager.getPhysicsCharacter('brad2').setBoolAttribute('enable', true)
		
		var randNum = Math.floor(Math.random() * (1.0 - 0.0 + 1.0)) + 0.0;
		var randDodge = Math.floor(Math.random() * (2.0 - 0.0 + 1.0)) + 0.0;
		if(randNum == 0)
			bml.execBML('doctor', '<sbm:reach sbm:action="touch" sbm:reach-finish="true" sbm:reach-type="right" target="pawn4"/>')
		else if(randNum == 1)
			bml.execBML('doctor', '<sbm:reach sbm:action="touch" sbm:reach-finish="true" sbm:reach-type="left" target="pawn5"/>')
		if(randDodge == 2)
			phyManager.getPhysicsCharacter('brad2').setBoolAttribute('enable', false)
		currentTurn = 'brad2'
	}

}