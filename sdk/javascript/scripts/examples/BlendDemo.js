function allReady(){
	console.log("|--------------------------------------------|")
	console.log("|           Starting Blend Demo              |")
	console.log("|--------------------------------------------|")
	if(scene != null){
		// Add asset paths
		scene.addAssetPath('mesh', 'ChrMaarten')
		scene.addAssetPath('mesh', 'ChrRachel')
		scene.addAssetPath('mesh', 'Sinbad')
		scene.addAssetPath('motion', 'ChrMaarten')
		scene.addAssetPath('motion', 'ChrRachel')
		scene.addAssetPath('motion', 'Sinbad')
		scene.loadAssets()

		// Set scene parameters and camera
		console.log('Configuring scene parameters and camera')
		scene.setBoolAttribute('internalAudio', true)

		console.log('Setting up joint map for Brad and Rachel')
		zebra2map.zebra2Map();
		var zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
		var bradSkeleton = scene.getSkeleton('ChrBrad.sk')
		bradSkeleton.rescale(6)
		zebra2Map.applySkeleton(bradSkeleton)
		zebra2Map.applyMotionRecurse('ChrMaarten')
		var rachelSkeleton = scene.getSkeleton('ChrRachel.sk')
		rachelSkeleton.rescale(6)
		zebra2Map.applySkeleton(rachelSkeleton)
		zebra2Map.applyMotionRecurse('ChrRachel')

		orgeSinbadMap.orgeSinbadMap();
		var sinbadSkName = 'Sinbad.skeleton.xml'
		var jointMapManager = scene.getJointMapManager()
		var sinbadMap = jointMapManager.getJointMap('Sinbad.skeleton.xml')
		var ogreSk = scene.getSkeleton(sinbadSkName)
		sinbadMap.applySkeleton(ogreSk)
		
		BehaviorSetMaleLocomotion.setupBehaviorSet()

		// Setting up characters
		console.log('Setting up characters')
		// chr0D
		var chr0D = scene.createCharacter('chr0D', '')
		var chr0DSkeleton = scene.createSkeleton('ChrBrad.sk')
		chr0D.setSkeleton(chr0DSkeleton)
		chr0D.setPosition(new Module.SrVec(-14.5, 0, 0))
		chr0D.createStandardControllers()
		chr0D.setVec3Attribute('deformableMeshScale', .06, .06, .06);
		chr0D.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
		chr0D.setStringAttribute("displayType", "mesh")
		BehaviorSetCommon.createRetargetInstance('test_utah.sk','ChrBrad.sk')
		bml.execBML('chr0D', '<body posture="ChrUtah_Idle001"/>')
		// chr1D
		var chr1D = scene.createCharacter('chr1D', '')
		var chr1DSkeleton = scene.createSkeleton(sinbadSkName)
		chr1D.setSkeleton(chr1DSkeleton)
		chr1D.setPosition(new Module.SrVec(-5.5, 5.16, 0))
		chr1D.createStandardControllers()
		chr1D.setStringAttribute('deformableMesh', 'Sinbad.mesh.xml')
		chr1D.setStringAttribute("displayType", "mesh")
		BehaviorSetCommon.createRetargetInstance('test_utah.sk',sinbadSkName)
		bml.execBML('chr1D', '<body posture="ChrUtah_Idle001"/>')
		// chr2D
		var chr2D = scene.createCharacter('chr2D', '')
		var chr2DSkeleton = scene.createSkeleton('ChrRachel.sk')
		chr2D.setSkeleton(chr2DSkeleton)
		chr2D.setPosition(new Module.SrVec(5.5, 0, 0))
		chr2D.createStandardControllers()
		chr2D.setVec3Attribute('deformableMeshScale', .06, .06, .06);
		chr2D.setStringAttribute("displayType", "mesh")
		chr2D.setStringAttribute('deformableMesh', 'ChrRachel.dae')
		BehaviorSetCommon.createRetargetInstance('test_utah.sk','ChrRachel.sk')
		bml.execBML('chr2D', '<body posture="ChrUtah_Idle001"/>')

		// Set up blends
		var blendManager = scene.getBlendManager()

		// 0D Blend
		console.log('Setting up 0D blend')
		var blend0D = blendManager.createBlend0D('blend0D')
		blend0D.setBlendSkeleton('test_utah.sk')
		var motions = new Module.StringVec()
		motions.push_back('ChrUtah_WalkInCircleRight001')
		blend0D.addMotion(motions.get(0))

		// 1D Blend
		console.log('Setting up 1D blend')
		var blend1D = blendManager.createBlend1D('blend1D')
		blend1D.setBlendSkeleton('test_utah.sk')
		var motions = new Module.StringVec()
		motions.push_back('ChrUtah_Idle001')
		motions.push_back('ChrUtah_Turn90Lf01')
		motions.push_back('ChrUtah_Turn180Lf01')
		motions.push_back('ChrUtah_Turn90Rt01')
		motions.push_back('ChrUtah_Turn180Rt01')

		var paramsX = new Module.DoubleVec()
		paramsX.push_back(0) // ChrUtah_Idle001 X
		paramsX.push_back(-90) // ChrUtah_Turn90Lf01 X
		paramsX.push_back(-180) // ChrUtah_Turn180Lf01 X
		paramsX.push_back(90) // ChrUtah_Turn90Rt01 X
		paramsX.push_back(180) // ChrUtah_Turn180Rt01 X
		for(var i = 0; i < motions.size(); ++i)
			blend1D.addMotion(motions.get(i), paramsX.get(i))

		var points0 = new Module.DoubleVec()
		points0.push_back(0) // ChrUtah_Idle001 0
		points0.push_back(0) // ChrUtah_Turn90Lf01 0
		points0.push_back(0) // ChrUtah_Turn180Lf01 0
		points0.push_back(0) // ChrUtah_Turn90Rt01 0
		points0.push_back(0) // ChrUtah_Turn180Rt01 0
		blend1D.addCorrespondencePoints(motions, points0)
		var points1 = new Module.DoubleVec()
		points1.push_back(0.255738) // ChrUtah_Idle001 1
		points1.push_back(0.762295) // ChrUtah_Turn90Lf01 1
		points1.push_back(0.87541) // ChrUtah_Turn180Lf01 1
		points1.push_back(0.757377) // ChrUtah_Turn90Rt01 1
		points1.push_back(0.821311) // ChrUtah_Turn180Rt01 1
		blend1D.addCorrespondencePoints(motions, points1)
		var points2 = new Module.DoubleVec()
		points2.push_back(0.633333) // ChrUtah_Idle001 2
		points2.push_back(1.96667) // ChrUtah_Turn90Lf01 2
		points2.push_back(2.46667) // ChrUtah_Turn180Lf01 2
		points2.push_back(1.96667) // ChrUtah_Turn90Rt01 2
		points2.push_back(2.46667) // ChrUtah_Turn180Rt01 2
		blend1D.addCorrespondencePoints(motions, points2)

		// 2D Blend
		console.log('Setting up 2D blend')
		var blend2D = blendManager.createBlend2D("blend2D")
		blend2D.setBlendSkeleton('test_utah.sk')

		var motions = new Module.StringVec()
		motions.push_back("ChrUtah_Idle001")
		motions.push_back("ChrUtah_Idle01_StepBackwardRt01")
		motions.push_back("ChrUtah_Idle01_StepForwardRt01")
		motions.push_back("ChrUtah_Idle01_StepSidewaysRt01")
		motions.push_back("ChrUtah_Idle01_StepBackwardLf01")
		motions.push_back("ChrUtah_Idle01_StepForwardLf01")
		motions.push_back("ChrUtah_Idle01_StepSidewaysLf01")

		var paramsX = new Module.DoubleVec()
		var paramsY = new Module.DoubleVec()
		paramsX.push_back(0) // ChrUtah_Idle001 X
		paramsY.push_back(0) // ChrUtah_Idle001 Y
		paramsX.push_back(-0.0275919) // ChrUtah_Idle01_StepBackwardRt01 X
		paramsY.push_back(-19.5057) // ChrUtah_Idle01_StepBackwardRt01 Y
		paramsX.push_back(0.0240943) // ChrUtah_Idle01_StepForwardRt01 X
		paramsY.push_back(45.4044) // ChrUtah_Idle01_StepForwardRt01 Y
		paramsX.push_back(28.8772) // ChrUtah_Idle01_StepSidewaysRt01 X
		paramsY.push_back(0.00321) // ChrUtah_Idle01_StepSidewaysRt01 Y
		paramsX.push_back(-0.0212764) // ChrUtah_Idle01_StepBackwardLf01 X
		paramsY.push_back(-39.203) // ChrUtah_Idle01_StepBackwardLf01 Y
		paramsX.push_back(0.0480087) // ChrUtah_Idle01_StepForwardLf01 X
		paramsY.push_back(47.8086) // ChrUtah_Idle01_StepForwardLf01 Y
		paramsX.push_back(-31.7367) // ChrUtah_Idle01_StepSidewaysLf01 X
		paramsY.push_back(0) // ChrUtah_Idle01_StepSidewaysLf01 Y
		for(var i = 0; i < motions.size(); ++i)
			blend2D.addMotion(motions.get(i), paramsX.get(i), paramsY.get(i))

		var points0 = new Module.DoubleVec()
		points0.push_back(0) // ChrUtah_Idle001 0
		points0.push_back(0) // ChrUtah_Idle01_StepBackwardRt01 0
		points0.push_back(0) // ChrUtah_Idle01_StepForwardRt01 0
		points0.push_back(0) // ChrUtah_Idle01_StepSidewaysRt01 0
		points0.push_back(0) // ChrUtah_Idle01_StepBackwardLf01 0
		points0.push_back(0) // ChrUtah_Idle01_StepForwardLf01 0
		points0.push_back(0) // ChrUtah_Idle01_StepSidewaysLf01 0
		blend2D.addCorrespondencePoints(motions, points0)
		var points1 = new Module.DoubleVec()
		points1.push_back(0.556322) // ChrUtah_Idle001 1
		points1.push_back(0.556322) // ChrUtah_Idle01_StepBackwardRt01 1
		points1.push_back(0.543678) // ChrUtah_Idle01_StepForwardRt01 1
		points1.push_back(0.482989) // ChrUtah_Idle01_StepSidewaysRt01 1
		points1.push_back(0.395402) // ChrUtah_Idle01_StepBackwardLf01 1
		points1.push_back(0.531034) // ChrUtah_Idle01_StepForwardLf01 1
		points1.push_back(0.473563) // ChrUtah_Idle01_StepSidewaysLf01 1
		blend2D.addCorrespondencePoints(motions, points1)
		var points2 = new Module.DoubleVec()
		points2.push_back(1.46414) // ChrUtah_Idle001 2
		points2.push_back(1.46414) // ChrUtah_Idle01_StepBackwardRt01 2
		points2.push_back(1.46414) // ChrUtah_Idle01_StepForwardRt01 2
		points2.push_back(1.46414) // ChrUtah_Idle01_StepSidewaysRt01 2
		points2.push_back(1.33333) // ChrUtah_Idle01_StepBackwardLf01 2
		points2.push_back(1.33333) // ChrUtah_Idle01_StepForwardLf01 2
		points2.push_back(1.33103) // ChrUtah_Idle01_StepSidewaysLf01 2
		blend2D.addCorrespondencePoints(motions, points2)

		blend2D.addTriangle("ChrUtah_Idle001", "ChrUtah_Idle01_StepBackwardLf01", "ChrUtah_Idle01_StepSidewaysLf01")
		blend2D.addTriangle("ChrUtah_Idle001", "ChrUtah_Idle01_StepForwardLf01", "ChrUtah_Idle01_StepSidewaysLf01")
		blend2D.addTriangle("ChrUtah_Idle001", "ChrUtah_Idle01_StepBackwardRt01", "ChrUtah_Idle01_StepSidewaysRt01")
		blend2D.addTriangle("ChrUtah_Idle001", "ChrUtah_Idle01_StepForwardRt01", "ChrUtah_Idle01_StepSidewaysRt01")

		//''' MISSING 3D BLEND '''
		bml.execBML('chr0D', '<blend name="blend0D"/>')

		console.log('Configuring scene parameters and camera')
		defaultViewer.defaultViewer();
		var camera = Module.getCamera();
		camera.setEye(-6, 7.98, 28.44)
		camera.setCenter(-6, 1.7, -39.5)
		camera.setUpVector(new Module.SrVec(0, 1, 0))
		camera.setScale(1)
		camera.setFov(1.0472)
		camera.setFarPlane(100)
		camera.setNearPlane(0.01)
		camera.setAspectRatio(1.02)
		defaultInterface.defaultInterface();

		last = 0
		canTime = true
		delay = 5

		last1D = 0
		delay1D = 1

		blend1DX = -180
		amount = 10
		blend2DX = 28.88
		blend2DY = 47.81
		var BlendDemo = Module.SBScript.extend("SBScript", {
			update: function(time){
				// Update every few seconds
				var diff1D = time - last1D
				if(diff1D > delay1D)
					// 1D Blend
					bml.execBML('chr1D', '<blend name="blend1D" x="' + blend1DX + '"/>')
					// Slowly increment and decrement
					blend1DX = blend1DX + amount
					if(blend1DX <= -180)
						amount = amount * -1
					else if (blend1DX >= 180)
						amount = amount * -1
					last1D = time
				
				var diff = time - last
				// Start blend after delay
				if(diff > delay){
					// 2D Blend
					bml.execBML('chr2D', '<blend name="blend2D" x="' + blend2DX + '" y="' + blend2DY + '"/>')
					// Alternate between 2D blend
					if (blend2DY == 47.81){
						blend2DX = -31.74
						blend2DY = -39.20
					}
					else if (blend2DY == -39.20){ 
						blend2DX = 28.88
						blend2DY = 47.81
					}
					last = time
				}
			}
		});
				
		// Run the update script
		scene.removeScript('blenddemo')
		var blenddemo = new BlendDemo
		scene.addScript('blenddemo', blenddemo)
		
	}else{
		console.log("SBScene does not exist");
	}
}

