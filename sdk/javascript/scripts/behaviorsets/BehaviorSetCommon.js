var BehaviorSetCommon = {
	createRetargetInstance: function (srcSkelName, tgtSkelName){
		var endJoints = new Module.StringVec();
		endJoints.push_back('l_ankle')
		endJoints.push_back('l_forefoot')
		endJoints.push_back('l_toe')
		endJoints.push_back('l_wrist')
		endJoints.push_back('r_ankle')		
		endJoints.push_back('r_forefoot')	
		endJoints.push_back('r_toe')	
		endJoints.push_back('r_wrist')

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
		// replace retarget each animation with just a simple retarget instance
		var retargetManager = scene.getRetargetManager()
		var retarget = retargetManager.getRetarget(srcSkelName,tgtSkelName)
		if(retarget == null){
			var retarget = 	retargetManager.createRetarget(srcSkelName,tgtSkelName)
			retarget.initRetarget(endJoints,relativeJoints)
		}
	},
	retargetMotion: function (motionName, srcSkelName, tgtSkelName, outDir){	
		var testMotion = scene.getMotion(motionName)
		if(testMotion == null)
			return
		var outMotionName = tgtSkelName + motionName		
		var existMotion = scene.getMotion(outMotionName)
		if(existMotion != null) // do nothing if the retargeted motion is already there
			return	
			
		var tgtSkel = scene.getSkeleton(tgtSkelName)
		if (tgtSkel == null)
			return
			
		if(!os.path.exists(outDir))
			os.makedirs(outDir)			
			
		var offsetJoints = new Module.VecMap();	
		var endJoints = new Module.StringVec();
		//endJoints.push_back('l_ankle')
		endJoints.push_back('l_forefoot')
		endJoints.push_back('l_toe')
		endJoints.push_back('l_acromioclavicular')	
		//endJoints.push_back('r_ankle')		
		endJoints.push_back('r_forefoot')	
		endJoints.push_back('r_toe')
		endJoints.push_back('r_acromioclavicular')	
		
		var relativeJoints = new Module.StringVec();
		relativeJoints.push_back('spine1')
		relativeJoints.push_back('spine2')
		relativeJoints.push_back('spine3')
		relativeJoints.push_back('spine4')
		relativeJoints.push_back('spine5')
		endJoints.push_back('l_sternoclavicular')
		endJoints.push_back('r_sternoclavicular')
		
		
		var effectorJoints = new Module.StringVec();	
		if(tgtSkel.getJointByName('r_toe') != null){
			effectorJoints.push_back('r_toe')
			effectorJoints.push_back('l_toe')
		}
		else{
			effectorJoints.push_back('r_ankle')
			effectorJoints.push_back('l_ankle')	
		}		
			
		if(tgtSkel.getJointByName('r_forefoot') != null){
			effectorJoints.push_back('r_forefoot')
			effectorJoints.push_back('l_forefoot')
		}		
		//effectorJoints.push_back('l_toe')		
		var effectorRoots = new Module.StringVec();
		effectorRoots.push_back('r_hip')
		effectorRoots.push_back('l_hip')
		effectorRoots.push_back('r_hip')
		effectorRoots.push_back('l_hip')
		
		console.log('Retarget motion = ' + motionName);
		var outMotion = testMotion.retarget(outMotionName,srcSkelName,tgtSkelName, endJoints, relativeJoints, offsetJoints);	
		var cleanMotion = testMotion.constrain(outMotionName, srcSkelName, tgtSkelName, outMotionName, effectorJoints, effectorRoots);
		var saveCommand = 'animation ' + outMotionName + ' save ' + outDir + outMotionName + '.skm';
		console.log('Save command = ' + saveCommand);
		scene.command(saveCommand)
	}

	
}
