function allReady(){
	$.when(
		$.getScript( "../scripts/scripts/zebra2-map.js" ),
		
		//$.getScript( "" ),
		$.Deferred(function( deferred ){
			$( deferred.resolve );
		})
	).done(function(){
		if(scene){
			console.log("|--------------------------------------------|")
			console.log("|            Starting Head Demo              |")
			console.log("|--------------------------------------------|")
			
			var mediaPath = "";
			scene.setMediaPath(mediaPath);
			// Add asset paths
			scene.addAssetPath('mesh', 'ChrMaarten')
			scene.addAssetPath('motion', 'ChrMaarten')
			scene.loadAssets()

			// Set scene parameters and camera
			console.log('Configuring scene parameters and camera')
			scene.setScale(1.0)
			scene.setBoolAttribute("enableAlphaBlend", true)
			defaultViewer.defaultViewer();
			
			var camera = Module.getCamera()
			camera.setEye(0.0, 1.65, 0.38)
			camera.setCenter(0.0, 1.59, -0.02)
			camera.setUpVector(new Module.SrVec(0, 1, 0))
			camera.setScale(1)
			camera.setFov(1.0472)
			camera.setFarPlane(100)
			camera.setNearPlane(0.1)
			camera.setAspectRatio(0.966897)
			defaultInterface.defaultInterface();
			// Setting up joint map for Brad
			console.log('Setting up joint map and configuring Brad\'s skeleton')
			zebra2map.zebra2Map();
			var zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
			var bradSkeleton = scene.getSkeleton('ChrBrad.sk')
			zebra2Map.applySkeleton(bradSkeleton)
			zebra2Map.applyMotionRecurse('ChrMaarten')

			// Setting up Brad
			console.log('Setting up Brad')
			var brad = scene.createCharacter('ChrBrad', '')
			var bradSkeleton = scene.createSkeleton('ChrBrad.sk')
			brad.setSkeleton(bradSkeleton)
			brad.createStandardControllers()
			// Deformable mesh
            brad.setVec3Attribute('deformableMeshScale', .01, .01, .01);
			brad.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
			bml.execBML('ChrBrad', '<body posture="ChrBrad@Idle01"/>')

			last = 0
			canTime = true
			delay = 5
			
			var HeadDemo = Module.SBScript.extend("SBScript", {
				update : function(time){
					var diff = time - last
					if(diff >= delay){
						canTime = true
						diff = 0
					}
					if(canTime){
						last = time
						canTime = false
						nextHead()
					}
				}
			});	
			
			// Run the update script
			scene.removeScript('headdemo')
			var headdemo = new HeadDemo
			scene.addScript('headdemo', headdemo)
		}else{
			console.error("Scene does not exist!");
		}
	});
}

headList = ['NOD', 'SHAKE', 'TOSS', 'WIGGLE', 'WAGGLE']
curHead = 0
headAmt = headList.length

function nextHead(){
	var headChoice = headList[curHead]
	console.log('Playing ' + headChoice)
	// Random repeats, velocity and amount
	repeats = Math.random() * (2.0 - 0.1) + 0.1
	amount = (Math.random() * (1.0 - 0.1) + 0.1) / 2.0
	// Perform BML command
	bml.execBML('*', '<head type="' + headChoice + '" amount="' + amount + '" repeats="' + repeats + '"/>')
	// Increment index, reset when hit max
	curHead = curHead + 1
	if(curHead >= headAmt)
		curHead = 0
}			