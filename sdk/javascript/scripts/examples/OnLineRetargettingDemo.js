var OnlineRetargettingDemo = {
	setMotionNameSkeleton : function (motionName, skelName){
		var motion = scene.getMotion(motionName)
		if(motion != null){	
			//console.log('motionName = ' + motionName + ' skelName = ' + skelName);
			motion.scale(6)
			motion.setMotionSkeletonName(skelName)
		}
	},
	
	createRetargetInstance : function (srcSkelName, tgtSkelName){
		// replace retarget each animation with just a simple retarget instance
		
		// these joints and their children are not retargeted
		var endJoints = new Module.StringVec();
		endJoints.push_back('l_forefoot')
		endJoints.push_back('l_toe')
		endJoints.push_back('l_wrist')
		endJoints.push_back('r_forefoot')	
		endJoints.push_back('r_toe')	
		endJoints.push_back('r_wrist')

		// these joints are skipped during skeleton alignment
		var relativeJoints = new Module.StringVec();
		relativeJoints.push_back('spine1')
		relativeJoints.push_back('spine2')
		relativeJoints.push_back('spine3')
		relativeJoints.push_back('spine4')
		relativeJoints.push_back('spine5')
		relativeJoints.push_back('r_sternoclavicular')
		relativeJoints.push_back('l_sternoclavicular')
		relativeJoints.push_back('r_acromioclavicular')
		relativeJoints.push_back('l_acromioclavicular')	
		
		var retargetManager = scene.getRetargetManager()
		var retarget = retargetManager.getRetarget(srcSkelName,tgtSkelName)
		if(retarget == null){
			var retarget = 	retargetManager.createRetarget(srcSkelName,tgtSkelName)	
			retarget.initRetarget(endJoints,relativeJoints)
		}
	}

}		
function allReady(){
	
	console.log("|--------------------------------------------|")
	console.log("|        Starting Retargetting Demo          |")
	console.log("|--------------------------------------------|")
	if(scene){
		var mediaPath = "";
		scene.setMediaPath(mediaPath);
		// Add asset paths
		scene.addAssetPath('mesh', 'ChrMaarten')
		scene.addAssetPath('mesh', 'Sinbad')
		scene.addAssetPath('motion', 'ChrMaarten')
		scene.addAssetPath('motion', 'Sinbad')
		
		scene.loadAssets()

		// Runs the default viewer for camera
		scene.setScale(1.0)
		scene.setBoolAttribute("enableAlphaBlend", true)
		// Apply zebra joint map for ChrBrad
		zebra2map.zebra2Map();
		var zebra2Map = scene.getJointMapManager().getJointMap("zebra2")
		var bradSkeleton = scene.getSkeleton("ChrBrad.sk")
		bradSkeleton.rescale(6)
		zebra2Map.applySkeleton(bradSkeleton)
		zebra2Map.applyMotionRecurse("ChrMaarten")

		orgeSinbadMap.orgeSinbadMap();
		var sinbadSkName = 'Sinbad.skeleton.xml'
		var jointMapManager = scene.getJointMapManager()
		var sinbadMap = jointMapManager.getJointMap('Sinbad.skeleton.xml')
		console.log("Sinbad Mappings: " + sinbadMap.getNumMappings());
		var ogreSk = scene.getSkeleton(sinbadSkName)
		sinbadMap.applySkeleton(ogreSk)
		
		// set skeleton names for source motions, so the on-line retargeting will know how to apply retargeting
		OnlineRetargettingDemo.setMotionNameSkeleton("ChrBrad@Idle01", "ChrBrad.sk")
		OnlineRetargettingDemo.setMotionNameSkeleton("ChrBrad@Guitar01", "ChrBrad.sk")
		OnlineRetargettingDemo.setMotionNameSkeleton("ChrBrad@PushUps01", "ChrBrad.sk")
		
		// Create on-line retarget instance from ChrBrad.sk to common.sk
		OnlineRetargettingDemo.createRetargetInstance("ChrBrad.sk", sinbadSkName)

		var sinbadName = 'target'
		var sinbad = scene.createCharacter(sinbadName,'')
		var sinbadSk = scene.createSkeleton(sinbadSkName)
		sinbad.setSkeleton(sinbadSk)
		var sinbadPos = new Module.SrVec(-6,5.16, 0)
		sinbad.setPosition(sinbadPos)
		sinbad.createStandardControllers()
        sinbad.setVec3Attribute('deformableMeshScale', 1, 1, 1);
		sinbad.setStringAttribute('deformableMesh', 'Sinbad.mesh.xml')
		
		BehaviorSetMaleLocomotion.setupBehaviorSet()
		BehaviorSetMaleLocomotion.retargetBehaviorSet(sinbadName)
		bml.execBML('target', '<body posture="ChrBrad@Idle01"/>')
		//bml.execBML(sinbadName, '<body posture="ChrUtah_Walk001"/>')
		sinbad.setStringAttribute("displayType", "mesh")
		
		// Add ChrBrad (source character )
		var source = scene.createCharacter('source', '')
		var sourceSkeleton = scene.createSkeleton('ChrBrad.sk')
		source.setSkeleton(sourceSkeleton)
		var sourcePos = new Module.SrVec(6, 0, 0)
		source.setPosition(sourcePos)
		source.createStandardControllers()
		// Deformable mesh
        source.setVec3Attribute('deformableMeshScale', .06, .06, .06);
		source.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
		bml.execBML('source', '<body posture="ChrBrad@Idle01"/>')
		source.setStringAttribute("displayType", "mesh")

		console.log('Configuring scene parameters and camera')
		defaultViewer.defaultViewer();
		var camera = Module.getCamera()
		camera.setEye(0, 7.98, 17.44)
		camera.setCenter(1.0, 1.7, -39.5)
		camera.setUpVector(new Module.SrVec(0, 1, 0))
		camera.setScale(1)
		camera.setFov(1.0472)
		camera.setFarPlane(100)
		camera.setNearPlane(0.1)
		camera.setAspectRatio(1.02)
		defaultInterface.defaultInterface();

		// Retarget motion
			
		last = 0
		canTime = true
		delay = 10
		
		var RetargettingDemo = Module.SBScript.extend("SBScript", {
			update : function(time){
				var diff = time - last
				if(diff >= delay){
					canTime = true
					diff = 0
				}
				if(canTime){
					last = time
					canTime = false
					// Play non retargetted and retargetted add delay
					bml.execBML('target', '<animation name="ChrBrad@Guitar01"/>')
					bml.execBML('source', '<animation name="ChrBrad@Guitar01"/>')
				}
				
			}
		});	
					
		scene.removeScript('retargettingdemo')
		var retargettingdemo = new RetargettingDemo
		scene.addScript('retargettingdemo', retargettingdemo)
	}else{
		console.error("Scene does not exist!");
	}
}
