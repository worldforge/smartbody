function allReady(){
	console.log("|--------------------------------------------|")
	console.log("|         Starting Ogre Crowd Demo           |")
	console.log("|--------------------------------------------|")
	if(scene){
		var mediaPath = "";
		scene.setMediaPath(mediaPath);
		// Add asset paths
		scene.addAssetPath('mesh', 'Sinbad')
		scene.addAssetPath('motion', 'Sinbad')
		scene.loadAssets()
		// Set scene parameters and camera
		scene.setScale(0.1)
		//scene.getPawn('camera').setPosition(SrVec(0, -5, 0))

		// Set joint map for Sinbad
		console.log('Setting up joint map for Brad')
		orgeSinbadMap.orgeSinbadMap();
		var sinbadSkName = 'Sinbad.skeleton.xml'
		var jointMapManager = scene.getJointMapManager()
		var sinbadMap = jointMapManager.getJointMap('Sinbad.skeleton.xml')
		var ogreSk = scene.getSkeleton(sinbadSkName)
		sinbadMap.applySkeleton(ogreSk)

		var steerManager = scene.getSteerManager()
		console.log('Setting up Sinbads')
		var amount = 10
		var row = 0, column = 0; 
		var sinbadList = [];
		
		for(var i = 0; i < amount; ++i){
			var sinbadName = 'sinbad' + i
			var sinbad = scene.createCharacter(sinbadName,'')
			var sinbadSk = scene.createSkeleton(sinbadSkName)
			sinbad.setSkeleton(sinbadSk)
			// Set position logic
			var posX = Math.cos(360.0 / amount * i) * 100.0
			var posZ = Math.sin(360.0 / amount * i) * 100.0
			//console.log("Sinbad = " + posX + " " + posZ);
			var sinbadPos = new Module.SrVec(posX, 5.16, posZ)
			sinbad.setPosition(sinbadPos)
			sinbad.createStandardControllers()
			sinbad.setStringAttribute('deformableMesh', 'Sinbad.mesh.xml')
			
			// Retarget character
			if(i == 0){
				BehaviorSetMaleLocomotion.setupBehaviorSet()
				BehaviorSetMaleLocomotion.retargetBehaviorSet(sinbadName)
			}
			var steerAgent = steerManager.createSteerAgent(sinbadName)
			steerAgent.setSteerStateNamePrefix("all")
			steerAgent.setSteerType("example")
			
			sinbadList.push(sinbad)
			sinbad.setStringAttribute("displayType", "mesh")
			sinbad.setBoolAttribute("steering.pathFollowingMode",true)
			// Play default animation
			bml.execBML(sinbadName, '<body posture="ChrUtah_Idle001"/>')	
		}
		steerManager.setEnable(false)
		steerManager.setDoubleAttribute("gridDatabaseOptions.gridSizeX", 200)
		steerManager.setDoubleAttribute("gridDatabaseOptions.gridSizeY", 200)
		steerManager.setIntAttribute("gridDatabaseOptions.numGridCellsX", 200)
		steerManager.setIntAttribute("gridDatabaseOptions.numGridCellsY", 200)
		steerManager.setEnable(true)

		// Set up list of characters
		
		console.log('Configuring scene parameters and camera');
		scene.setBoolAttribute('internalAudio', true)
		defaultViewer.defaultViewer()
		var camera = Module.getCamera()
		camera.setEye(0, 80.98, 53.44)
		camera.setCenter(1.0, 1.7, -39.5)
		camera.setUpVector(new Module.SrVec(0, 1, 0))
		camera.setScale(1)
		camera.setFov(1.0472)
		camera.setFarPlane(500)
		camera.setNearPlane(0.1)
		camera.setAspectRatio(1.02)
		defaultInterface.defaultInterface();

		sim.start()
		sim.resume()
		
		// Paths for characters
		sinbadPath = [new Module.SrVec(-70, -70, 0), new Module.SrVec(70, 70, 0), new Module.SrVec(70, -70, 0), new Module.SrVec(-70, 70, 0)]
		sinbadCur = 0
		pathAmt = sinbadPath.length
		sinbadReached = true

		var CrowdDemo = Module.SBScript.extend("SBScript", {
			update : function(time){
				// Once utah completes path, do again
				if(sinbadReached){
					
					for(var i = 0; i < sinbadList.length; ++i){
						var sinbad = sinbadList[i];
						// Move character
						//bml.execBML('sinbad2',"<locomotion speed='3.22' target='0 -70 0' type='basic'/>")
						bml.execBML(sinbad.getName(), '<locomotion speed="' + (Math.random() * (5.0 - 1.2) + 1.2).toFixed(2) + '" target="' + vec2str(sinbadPath[sinbadCur]) + '" type="basic"/>')
						//console.log(sinbad.getName() + '<locomotion speed="' + (Math.random() * (5.0 - 1.2) + 1.2).toFixed(2) + '" target="' + vec2str(sinbadPath[sinbadCur]) + '" type="basic"/>');
					}
					sinbadCur = sinbadCur + 1
					// If reaches max path, reset
					if(sinbadCur >= pathAmt)
						sinbadCur = 0
					sinbadReached = false
				}
			}
		});		
		// Run the update script
		scene.removeScript('crowddemo')
		var crowddemo = new CrowdDemo
		scene.addScript('crowddemo', crowddemo)
		//crowddemo.delete();
		
		reachCount = 0
		// Locomotion handler to check if characters have arrived
		var LocomotionHandler = Module.SBEventHandler.extend("SBEventHandler", {
			executeAction : function (ev){
				var params = ev.getParameters()
				//console.log("Event Parameters:" + params)
				if(params.indexOf('success') != -1){
					if(params.indexOf('sinbad') != -1){
						reachCount = reachCount + 1
						if(reachCount >= 6){
							sinbadReached = true	
							reachCount = 0
						}
					}
				}
			}
		});
					

		var locomotionHdl = new LocomotionHandler
		var evtMgr = scene.getEventManager()
		evtMgr.addEventHandler('locomotion', locomotionHdl)
		//locomotionHdl.delete();
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