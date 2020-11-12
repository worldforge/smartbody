function allReady(){
	console.log("|--------------------------------------------|")
	console.log("|           Starting Crowd Demo              |")
	console.log("|--------------------------------------------|")
	if(scene){
		// Add asset paths
		scene.addAssetPath('mesh', 'ChrBrad')
		scene.addAssetPath('motion', 'ChrBrad')
		scene.loadAssets()
		scene.setScale(1.0)
		
		// Set joint map for Brad
		console.log( 'Setting up joint map for Brad')
		zebra2map.zebra2Map();
		var zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
		var bradSkeleton = scene.getSkeleton('ChrBrad.sk')
		zebra2Map.applySkeleton(bradSkeleton)
		zebra2Map.applyMotionRecurse('ChrBrad')

		// Runs the default viewer for camera
		defaultViewer.defaultViewer();
		var camera = Module.getCamera();
		camera.setEye(1, 13.43, 18.63)
		camera.setCenter(1, 12, 16.91)
		scene.getPawn('camera').setPosition(new Module.SrVec(0, -50, 0))
		defaultInterface.defaultInterface();

		var steerManager = scene.getSteerManager()

		// Setting up Brads
		console.log('Setting up Brads')
		var amount = 4
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
			column = column + 1
			if(column >= 5){
				column = 0
				row = row + 1
			}
			var bradPos = new Module.SrVec((posX + offsetX)/100, 0, (posZ + offsetZ)/100)
			brad.setPosition(bradPos)
			// Set up standard controllers
			brad.createStandardControllers()
			// Set deformable mesh
			brad.setVec3Attribute('deformableMeshScale', .01, .01, .01)
			brad.setStringAttribute('deformableMesh', 'ChrBrad.dae')
			brad.setStringAttribute("displayType", "mesh")
			// Retarget character
			//retargetCharacter(baseName, 'ChrBrad.sk', False)
			// setup mocap locomotion
			if(i== 0){
				BehaviorSetMaleLocomotion.setupBehaviorSet()
				BehaviorSetMaleLocomotion.retargetBehaviorSet(baseName)
				BehaviorSetGestures.setupBehaviorSet()
				BehaviorSetGestures.retargetBehaviorSet(baseName)	
			}				
			var steerAgent = steerManager.createSteerAgent(baseName)
			steerAgent.setSteerStateNamePrefix("all")
			steerAgent.setSteerType("example")
			// Set up steering
			//setupSteerAgent(baseName, 'ChrBrad.sk')
			brad.setBoolAttribute('steering.pathFollowingMode', false)
			// Play default animation
			bml.execBML(baseName, '<body posture="ChrBrad@Idle01"/>')
		}

		steerManager.setEnable(false)
		steerManager.setEnable(true)

		// Set up list of Brads
		var bradList = [];
		for(var i = 0; i < scene.getCharacterNames().size(); ++i){
			var name = scene.getCharacterNames().get(i);
			if(name.indexOf('ChrBrad') != -1)
				bradList.push(scene.getCharacter(name));
		}

		// Paths for characters
		bradPath = [new Module.SrVec(-8, -8, 0), new Module.SrVec(8, 8, 0), new Module.SrVec(8, -8, 0), new Module.SrVec(-8, 8, 0)]
		bradCur = 0
		pathAmt = bradPath.length
		bradReached = true

		var CrowdDemo = Module.SBScript.extend('SBScript', {
			update : function(time){
				if(bradReached){
					for(var i = 0; i < bradList.length; ++i){
						var brad = bradList[i];
						bml.execBML(brad.getName(), '<locomotion speed="' +(Math.random() * (5.0 - 1.2) + 1.2).toFixed(2) + '" target="' + vec2str(bradPath[bradCur]) + '"/>')
					}
					bradCur = bradCur + 1
					// If reaches max path, reset
					if(bradCur >= pathAmt)
						bradCur = 0
					bradReached = false
				}
			}
			
		});
		// Run the update script
		scene.removeScript('crowddemo')
		var crowddemo = new CrowdDemo
		scene.addScript('crowddemo', crowddemo)
		
		reachCount = 0
		var LocomotionHandler = Module.SBEventHandler.extend('SBEventHandler', {
			executeAction : function (ev){
				var params = ev.getParameters()
				//console.log("Event Parameters:" + params)
				if(params.indexOf('success') != -1){
					if(params.indexOf('ChrBrad') != -1){
						reachCount = reachCount + 1
						if(reachCount >= 2){
							bradReached = true	
							reachCount = 0
						}
					}
				}
			}
		});

		var locomotionHdl = new LocomotionHandler
		var evtMgr = scene.getEventManager()
		evtMgr.addEventHandler('locomotion', locomotionHdl)
		
	}else{
		console.error("Scene does not exist!");
	}
}

function vec2str(vec){
	var x = vec.getData(0)
	var y = vec.getData(1)
	var z = vec.getData(2)
	if ( x > -0.0001 && x < 0.0001) x = 0
	if ( y > -0.0001 && y < 0.0001) y = 0
	if ( z > -0.0001 && z < 0.0001) z = 0
	return "" + x + " " + y + " " + z + ""
}