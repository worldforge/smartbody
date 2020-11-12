var AutoRiggingDemo = {
	setMotionNameSkeleton : function (motionName, skelName){
		var motion = scene.getMotion(motionName)
		if(motion != null){	
			//console.log('motionName = ' + motionName + ' skelName = ' + skelName)
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
        relativeJoints.push_back('JtPelvis')
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
	
	console.log( "|--------------------------------------------|")
	console.log( "|       	Auto Rigging Demo        	       |")
	console.log( "|--------------------------------------------|")
	
	if(scene){
        //create a folder in VFS
        //configure scene
		scene.setScale(0.5);
		scene.addAssetPath('audio', 'sounds');
        scene.addAssetPath('motion', 'AutoRigging/motions');
		scene.loadAssets();
		scene.setBoolAttribute('internalAudio', true);
        scene.setBoolAttribute('lightEnabled', false);
        //configure light and camera
		defaultViewer.defaultViewer();
		var camera = Module.getCamera()
        camera.setEye(-0.01, 3.34, 4.5)
        camera.setCenter(-0.06, 2.4, 0.03)
        camera.setUpVector(new Module.SrVec(0, 1, 0))
        camera.setScale(1.0)
        camera.setFov(1.0472)
        camera.setFarPlane(100)
        camera.setNearPlane(0.1)
        camera.setAspectRatio(1.78125)
		cameraPos = new Module.SrVec(0, 1.6, 10)
		scene.getPawn('camera').setPosition(cameraPos)
        //add default interface control
		defaultInterface.defaultInterface();
        
	}else{
		console.error("Scene does not exist!");
	}

}

