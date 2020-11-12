var locomotionChrMarineStateLocomotion = {
	marineLocomotionSetup: function (origSkelName, skeletonName, baseJoint, preFix, statePreFix){
		var stateManager = scene.getStateManager()

		var statemarineLocomotion = stateManager.createState3D("mocapLocomotion")
		statemarineLocomotion.setBlendSkeleton('ChrBackovic.sk')
		var motions = new Module.StringVec()
		motions.push_back("ChrMarine@Idle01")
		motions.push_back("ChrMarine@Jog01")
		motions.push_back("ChrMarine@Meander01")
		motions.push_back("ChrMarine@Run01")
		// motions.push_back("ChrMarine@RunCircleLf01_smooth")
		// motions.push_back("ChrMarine@RunCircleRt01_smooth")
		motions.push_back("ChrMarine@RunCircleLf01_smooth")
		motions.push_back("ChrMarine@RunCircleRt01_smooth")
		motions.push_back("ChrMarine@RunTightCircleLf01")
		motions.push_back("ChrMarine@RunTightCircleRt01")
		motions.push_back("ChrMarine@StrafeSlowLf01")
		motions.push_back("ChrMarine@StrafeSlowRt01")
		motions.push_back("ChrMarine@Walk01")
		// motions.push_back("ChrMarine@WalkCircleLf01_smooth")
		// motions.push_back("ChrMarine@WalkCircleRt01_smooth")
		// motions.push_back("ChrMarine@WalkTightCircleLf01_smooth")
		// motions.push_back("ChrMarine@WalkTightCircleRt01_smooth")
		motions.push_back("ChrMarine@WalkCircleLf01_smooth")
		motions.push_back("ChrMarine@WalkCircleRt01_smooth")
		motions.push_back("ChrMarine@WalkTightCircleLf01_smooth")
		motions.push_back("ChrMarine@WalkTightCircleRt01_smooth")
		motions.push_back("ChrMarine@Turn360Lf01")
		motions.push_back("ChrMarine@Turn360Rt01")
		motions.push_back("ChrMarine@StrafeFastLf01_smooth")
		motions.push_back("ChrMarine@StrafeFastRt01_smooth")

		var paramsX = new Module.DoubleVec()
		var paramsY = new Module.DoubleVec()
		var paramsZ = new Module.DoubleVec()
		paramsX.push_back(0) // ChrMarine@Idle01 X
		paramsY.push_back(0) // ChrMarine@Idle01 Y
		paramsZ.push_back(0) // ChrMarine@Idle01 Z
		paramsX.push_back(0) // ChrMarine@Jog01 X
		paramsY.push_back(0) // ChrMarine@Jog01 Y
		paramsZ.push_back(0) // ChrMarine@Jog01 Z
		paramsX.push_back(0) // ChrMarine@Meander01 X
		paramsY.push_back(0) // ChrMarine@Meander01 Y
		paramsZ.push_back(0) // ChrMarine@Meander01 Z
		paramsX.push_back(0) // ChrMarine@Run01 X
		paramsY.push_back(0) // ChrMarine@Run01 Y
		paramsZ.push_back(0) // ChrMarine@Run01 Z
		paramsX.push_back(0) // ChrMarine@RunCircleLf01_smooth X
		paramsY.push_back(0) // ChrMarine@RunCircleLf01_smooth Y
		paramsZ.push_back(0) // ChrMarine@RunCircleLf01_smooth Z
		paramsX.push_back(0) // ChrMarine@RunCircleRt01_smooth X
		paramsY.push_back(0) // ChrMarine@RunCircleRt01_smooth Y
		paramsZ.push_back(0) // ChrMarine@RunCircleRt01_smooth Z
		paramsX.push_back(0) // ChrMarine@RunTightCircleLf01 X
		paramsY.push_back(0) // ChrMarine@RunTightCircleLf01 Y
		paramsZ.push_back(0) // ChrMarine@RunTightCircleLf01 Z
		paramsX.push_back(0) // ChrMarine@RunTightCircleRt01 X
		paramsY.push_back(0) // ChrMarine@RunTightCircleRt01 Y
		paramsZ.push_back(0) // ChrMarine@RunTightCircleRt01 Z
		paramsX.push_back(0) // ChrMarine@StrafeFastLf01_smooth X
		paramsY.push_back(0) // ChrMarine@StrafeFastLf01_smooth Y
		paramsZ.push_back(0) // ChrMarine@StrafeFastLf01_smooth Z
		paramsX.push_back(0) // ChrMarine@StrafeFastRt01_smooth X
		paramsY.push_back(0) // ChrMarine@StrafeFastRt01_smooth Y
		paramsZ.push_back(0) // ChrMarine@StrafeFastRt01_smooth Z
		paramsX.push_back(0) // ChrMarine@Walk01 X
		paramsY.push_back(0) // ChrMarine@Walk01 Y
		paramsZ.push_back(0) // ChrMarine@Walk01 Z
		paramsX.push_back(0) // ChrMarine@WalkCircleLf01_smooth X
		paramsY.push_back(0) // ChrMarine@WalkCircleLf01_smooth Y
		paramsZ.push_back(0) // ChrMarine@WalkCircleLf01_smooth Z
		paramsX.push_back(0) // ChrMarine@WalkCircleRt01_smooth X
		paramsY.push_back(0) // ChrMarine@WalkCircleRt01_smooth Y
		paramsZ.push_back(0) // ChrMarine@WalkCircleRt01_smooth Z
		paramsX.push_back(0) // ChrMarine@WalkTightCircleLf01_smooth X
		paramsY.push_back(0) // ChrMarine@WalkTightCircleLf01_smooth Y
		paramsZ.push_back(0) // ChrMarine@WalkTightCircleLf01_smooth Z
		paramsX.push_back(0) // ChrMarine@WalkTightCircleRt01_smooth X
		paramsY.push_back(0) // ChrMarine@WalkTightCircleRt01_smooth Y
		paramsZ.push_back(0) // ChrMarine@WalkTightCircleRt01_smooth Z
		paramsX.push_back(0) // ChrMarine@WalkTightCircleLf01_smooth X
		paramsY.push_back(0) // ChrMarine@WalkTightCircleLf01_smooth Y
		paramsZ.push_back(0) // ChrMarine@WalkTightCircleLf01_smooth Z
		paramsX.push_back(0) // ChrMarine@WalkTightCircleRt01_smooth X
		paramsY.push_back(0) // ChrMarine@WalkTightCircleRt01_smooth Y
		paramsZ.push_back(0) // ChrMarine@WalkTightCircleRt01_smooth Z

		paramsX.push_back(0) // ChrMarine@WalkTightCircleLf01_smooth X
		paramsY.push_back(0) // ChrMarine@WalkTightCircleLf01_smooth Y
		paramsZ.push_back(0) // ChrMarine@WalkTightCircleLf01_smooth Z
		paramsX.push_back(0) // ChrMarine@WalkTightCircleRt01_smooth X
		paramsY.push_back(0) // ChrMarine@WalkTightCircleRt01_smooth Y
		paramsZ.push_back(0) // ChrMarine@WalkTightCircleRt01_smooth Z

		for(var i = 0; i < motions.size(); ++i)
			statemarineLocomotion.addMotion(motions.get(i), paramsX.get(i), paramsY.get(i), paramsZ.get(i))

		var points0 = new Module.DoubleVec()
		points0.push_back(0.2) // ChrMarine@Idle01 0
		points0.push_back(0.4) // ChrMarine@Jog01 0
		points0.push_back(0.75) // ChrMarine@Meander01 0
		points0.push_back(0.3) // ChrMarine@Run01 0
		points0.push_back(0.4) // ChrMarine@RunCircleLf01_smooth 0
		points0.push_back(0.1) // ChrMarine@RunCircleRt01_smooth 0
		points0.push_back(0.1) // ChrMarine@RunTightCircleLf01 0
		points0.push_back(0.4) // ChrMarine@RunTightCircleRt01 0
		points0.push_back(0.1) // ChrMarine@StrafeSlowLf01 0
		points0.push_back(0.75) // ChrMarine@StrafeSlowRt01 0
		points0.push_back(0.65) // ChrMarine@Walk01 0
		points0.push_back(0.15) // ChrMarine@WalkCircleLf01_smooth 0
		points0.push_back(0.85) // ChrMarine@WalkCircleRt01_smooth 0
		points0.push_back(0.1) // ChrMarine@WalkTightCircleLf01_smooth 0
		points0.push_back(1.0) // ChrMarine@WalkTightCircleRt01_smooth 0
		points0.push_back(0.7) // ChrMarine@Turn360Lf01 0
		points0.push_back(0.1) // ChrMarine@Turn360Rt01 0
		points0.push_back(0.45) // ChrMarine@StrafeFastLf01_smooth 0
		points0.push_back(0.1) // ChrMarine@StrafeFastRt01_smooth 0

		statemarineLocomotion.addCorrespondencePoints(motions, points0)
		var points1 = new Module.DoubleVec()
		points1.push_back(0.7) // ChrMarine@Idle01 1
		points1.push_back(0.75) // ChrMarine@Jog01 1
		points1.push_back(1.45) // ChrMarine@Meander01 1
		points1.push_back(0.55) // ChrMarine@Run01 1
		points1.push_back(0.85) // ChrMarine@RunCircleLf01_smooth 1
		points1.push_back(0.4) // ChrMarine@RunCircleRt01_smooth 1
		points1.push_back(0.4) // ChrMarine@RunTightCircleLf01 1
		points1.push_back(0.74) // ChrMarine@RunTightCircleRt01 1
		points1.push_back(0.75) // ChrMarine@StrafeSlowLf01 1
		points1.push_back(1.3) // ChrMarine@StrafeSlowRt01 1
		points1.push_back(1.25) // ChrMarine@Walk01 1
		points1.push_back(0.85) // ChrMarine@WalkCircleLf01_smooth 1
		points1.push_back(1.55) // ChrMarine@WalkCircleRt01_smooth 1
		points1.push_back(1.0) // ChrMarine@WalkTightCircleLf01_smooth 1
		points1.push_back(1.6) // ChrMarine@WalkTightCircleRt01_smooth 1
		points1.push_back(1.2) // ChrMarine@Idle01 1
		points1.push_back(0.7) // ChrMarine@Jog01 1
		points1.push_back(0.85) // ChrMarine@StrafeFastLf01_smooth 1
		points1.push_back(0.45) // ChrMarine@StrafeFastRt01_smooth 1

		statemarineLocomotion.addCorrespondencePoints(motions, points1)
		var points2 = new Module.DoubleVec()
		points2.push_back(1.25) // ChrMarine@Idle01 2
		points2.push_back(1.1) // ChrMarine@Jog01 2
		points2.push_back(1.95) // ChrMarine@Meander01 2
		points2.push_back(0.85) // ChrMarine@Run01 2
		points2.push_back(1.25) // ChrMarine@RunCircleLf01_smooth 2
		points2.push_back(0.85) // ChrMarine@RunCircleRt01_smooth 2
		points2.push_back(0.74) // ChrMarine@RunTightCircleLf01 2
		points2.push_back(1.1) // ChrMarine@RunTightCircleRt01 2
		points2.push_back(1.3) // ChrMarine@StrafeSlowLf01 2
		points2.push_back(1.85) // ChrMarine@StrafeSlowRt01 2
		points2.push_back(1.80) // ChrMarine@Walk01 2
		points2.push_back(1.55) // ChrMarine@WalkCircleLf01_smooth 2
		points2.push_back(2.2) // ChrMarine@WalkCircleRt01_smooth 2
		points2.push_back(1.6) // ChrMarine@WalkTightCircleLf01_smooth 2
		points2.push_back(2.45) // ChrMarine@WalkTightCircleRt01_smooth 2
		points2.push_back(1.75) // ChrMarine@Idle01 2
		points2.push_back(1.2) // ChrMarine@Jog01 2
		points2.push_back(1.2) // ChrMarine@StrafeFastLf01_smooth 2
		points2.push_back(0.85) // ChrMarine@StrafeFastRt01_smooth 2



		statemarineLocomotion.addCorrespondencePoints(motions, points2)
		var points3 = new Module.DoubleVec()
		points3.push_back(1.80) // ChrMarine@Idle01 3
		points3.push_back(1.45) // ChrMarine@Jog01 3
		points3.push_back(2.5333) // ChrMarine@Meander01 3
		points3.push_back(1.166) // ChrMarine@Run01 3
		points3.push_back(1.666) // ChrMarine@RunCircleLf01_smooth 3
		points3.push_back(1.25) // ChrMarine@RunCircleRt01_smooth 3
		points3.push_back(1.1) // ChrMarine@RunTightCircleLf01 3
		points3.push_back(1.5) // ChrMarine@RunTightCircleRt01 3
		points3.push_back(1.85) // ChrMarine@StrafeSlowLf01 3
		points3.push_back(2.4) // ChrMarine@StrafeSlowRt01 3
		points3.push_back(2.216) // ChrMarine@Walk01 3
		points3.push_back(2.2) // ChrMarine@WalkCircleLf01_smooth 3
		points3.push_back(2.75) // ChrMarine@WalkCircleRt01_smooth 3
		points3.push_back(2.45) // ChrMarine@WalkTightCircleLf01_smooth 3
		points3.push_back(3.03) // ChrMarine@WalkTightCircleRt01_smooth 3
		points3.push_back(2.23) // ChrMarine@Idle01 3
		points3.push_back(1.75) // ChrMarine@Jog01 3
		points3.push_back(1.70) // ChrMarine@StrafeFastLf01_smooth 3
		points3.push_back(1.20) // ChrMarine@StrafeFastRt01_smooth 3

		statemarineLocomotion.addCorrespondencePoints(motions, points3)
		var points4 = new Module.DoubleVec()
		points4.push_back(2.27) // ChrMarine@Idle01 4
		points4.push_back(1.80) // ChrMarine@Jog01 4
		points4.push_back(3.0833) // ChrMarine@Meander01 4
		points4.push_back(1.366) // ChrMarine@Run01 4
		points4.push_back(1.966) // ChrMarine@RunCircleLf01_smooth 4
		points4.push_back(1.666) // ChrMarine@RunCircleRt01_smooth 4
		points4.push_back(1.5) // ChrMarine@RunTightCircleLf01 4
		points4.push_back(1.8) // ChrMarine@RunTightCircleRt01 4
		points4.push_back(2.4) // ChrMarine@StrafeFastLf01_smooth 4
		points4.push_back(3.05) // ChrMarine@StrafeFastRt01_smooth 4
		points4.push_back(2.71666) // ChrMarine@Walk01 4
		points4.push_back(2.75) // ChrMarine@WalkCircleLf01_smooth 4
		points4.push_back(3.45) // ChrMarine@WalkCircleRt01_smooth 4
		points4.push_back(3.03) // ChrMarine@WalkTightCircleLf01_smooth 4
		points4.push_back(3.93) // ChrMarine@WalkTightCircleRt01_smooth 4
		points4.push_back(2.83) // ChrMarine@Idle01 4
		points4.push_back(2.23) // ChrMarine@Jog01 4
		points4.push_back(2.05) // ChrMarine@StrafeSlowLf01 4
		points4.push_back(1.70) // ChrMarine@StrafeSlowRt01 4

		statemarineLocomotion.addCorrespondencePoints(motions, points4)


		var stanceArray = []
		stanceArray.push([0.2, 0.7, 1.25, 1.80]) // idle
		stanceArray.push([0.4, 0.05,1.1,0.75]) // jog
		stanceArray.push([0.75,0.2, 1.95, 1.45]) // meander
		stanceArray.push([0.3, 0.1, 0.85, 0.55]) // run
		stanceArray.push([0.4, 0.1, 1.25, 0.85]) // run circle lf
		stanceArray.push([0.1, 0.4, 0.85, 1.25]) // run circle rt
		stanceArray.push([0.1, 0.4, 0.74, 1.10]) // run tight circle lf
		stanceArray.push([0.4, 0.1, 1.10, 0.74]) // run tight circle rt
		stanceArray.push([0.1, 0.75, 1.3, 1.85]) // strafe slow lf
		stanceArray.push([0.75, 0.1, 1.85, 1.3]) // strafe slow rt
		stanceArray.push([0.65, 0.15, 1.80, 1.25]) // walk
		stanceArray.push([0.15, 0.85, 1.55, 2.20]) // walk circle lf
		stanceArray.push([0.85, 0.15, 2.20, 1.55]) // walk circle rt
		stanceArray.push([0.1, 1.0, 1.6, 2.45]) // walk tight circle lf
		stanceArray.push([1.0, 0.1, 2.45, 1.6]) // walk tight circle rt
		stanceArray.push([0.7, 0.1, 1.75, 1.20]) // turn 360 lf
		stanceArray.push([0.1, 0.7, 1.20, 1.75]) // turn 360 rt	
		stanceArray.push([0.45, 0.1, 1.20, 0.85]) // strafe fast lf
		stanceArray.push([0.1, 0.45, 0.85, 1.20]) // strafe fast rt

		var liftArray = []
		liftArray.push([0.7, 0.2, 1.80, 1.25]) // idle
		liftArray.push([0.55, 0.22,1.22,0.92]) // jog
		liftArray.push([0.2, 0.75, 1.45, 1.95]) // meander
		liftArray.push([0.4, 0.2, 0.95, 0.65]) // run
		liftArray.push([0.5, 0.2, 1.35, 0.95]) // run circle lf
		liftArray.push([0.2, 0.5, 0.95, 1.35]) // run circle rt
		liftArray.push([0.2, 0.5, 0.84, 1.20]) // run tight circle lf
		liftArray.push([0.5, 0.2, 1.20, 0.84]) // run tight circle rt
		liftArray.push([0.75, 0.1, 1.85, 1.3]) // strafe slow lf
		liftArray.push([0.1, 0.75, 1.3, 1.85]) // strafe slow rt
		liftArray.push([0.15, 0.65, 1.25,  1.80]) // walk
		liftArray.push([0.85, 0.15, 2.20, 1.55]) // walk circle lf
		liftArray.push([0.15, 0.85, 1.55, 2.20]) // walk circle rt
		liftArray.push([1.0, 0.1, 2.45, 1.6]) // walk tight circle lf
		liftArray.push([0.1, 1.0, 1.6, 2.45]) // walk tight circle rt
		liftArray.push([0.1, 0.7, 1.20, 1.75]) // turn 360 lf
		liftArray.push([0.7, 0.1, 1.75, 1.20]) // turn 360 rt	
		liftArray.push([0.1, 0.45, 0.85, 1.20]) // strafe fast lf
		liftArray.push([0.45, 0.1, 1.20, 0.85]) // strafe fast rt
			

		for(var i = 0; i < motions.size(); ++i){
			var motionName = motions.get(i)
			var motion = scene.getMotion(motionName)
			var motionDuration = motion.getDuration()		
			for(var j = 0; j < 2; ++j){
				var s1 = stanceArray[i][0+j]
				var s2 = stanceArray[i][2+j]
				var leg = j
				statemarineLocomotion.addKeyTagValue(motionName,leg,"stance",s1);
				statemarineLocomotion.addKeyTagValue(motionName,leg,"stance",s2);	
				var lift1 = liftArray[i][0+j]%motionDuration
				var lift2 = liftArray[i][2+j]%motionDuration
				statemarineLocomotion.addKeyTagValue(motionName,leg,"lift",lift1);
				statemarineLocomotion.addKeyTagValue(motionName,leg,"lift",lift2);	
				var land1 = (s2-0.05)%motionDuration
				var land2 = (s1-0.05)%motionDuration
				statemarineLocomotion.addKeyTagValue(motionName,leg,"land",land1);
				statemarineLocomotion.addKeyTagValue(motionName,leg,"land",land2);	
				var off1 = (lift1+0.1)%motionDuration
				var off2 = (lift2+0.1)%motionDuration
				statemarineLocomotion.addKeyTagValue(motionName,leg,"lift-off",off1);
				statemarineLocomotion.addKeyTagValue(motionName,leg,"lift-off",off2);
				var str1 = (land1-0.15)%motionDuration
				var str2 = (land2-0.15)%motionDuration
				statemarineLocomotion.addKeyTagValue(motionName,leg,"strike",str1);
				statemarineLocomotion.addKeyTagValue(motionName,leg,"strike",str2);		
				// add motion event, only for walk motion
				if(i == 10 && leg == 0){ 
					statemarineLocomotion.addEvent(motionName, s1, 'footstep', leg + '-1', false)
					statemarineLocomotion.addEvent(motionName, s2, 'footstep', leg + '-2', false)
				}
			}
		}
		statemarineLocomotion.buildMotionAnalysis("ChrBackovic.sk","base",motions, '');		

		var skeleton = scene.getSkeleton("ChrBackovic.sk")
		var joint = skeleton.getJointByName("base")
		var travelDirection = [0, 0, 0, 0, 0, 0, 0, 0, -90, 90, 0, 0, 0, 0 ,0, 0, 0, -90, 90 ]

		for(var i = 0; i < motions.size(); ++i){
			var motion = scene.getMotion(motions.get(i))
			motion.connect(skeleton)
			var correspondancePoints = statemarineLocomotion.getCorrespondancePoints(i)
			var lenCorrespondancePoints = correspondancePoints.size()
			//speed = motion.getJointSpeed(joint, correspondancePoints[0], correspondancePoints[lenCorrespondancePoints - 1])
			//omega = motion.getJointAngularSpeed(joint, correspondancePoints[0], correspondancePoints[lenCorrespondancePoints - 1])
			var speed = motion.getJointSpeed(joint, 0, motion.getDuration())
			//speedZ = motion.getJointSpeedAxis(joint, "Z", 0, motion.getDuration())
			//if (travelDirection[i] != 0):
			//	direction = motion.getJointSpeedAxis(joint, "X", 0, motion.getDuration())
			//else:
			var direction = travelDirection[i]
			
			var omega = motion.getJointAngularSpeed(joint, 0, motion.getDuration())
			
			statemarineLocomotion.setParameter(motions.get(i), speed, omega, direction)
			//print "Motion " + motion.getName() + " (" + str(speed) + ", " + str(omega) + ", " + str(direction) + ")"
			motion.disconnect()
		}
			
		statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@Meander01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkCircleLf01_smooth", "ChrMarine@WalkTightCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkTightCircleLf01_smooth", "ChrMarine@Turn360Lf01")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkTightCircleLf01_smooth", "ChrMarine@Turn360Lf01")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkCircleLf01_smooth", "ChrMarine@WalkTightCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeSlowRt01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeSlowRt01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@Run01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@StrafeFastRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@Run01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@StrafeFastRt01_smooth")

		statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@Meander01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkCircleLf01_smooth", "ChrMarine@WalkTightCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkTightCircleLf01_smooth", "ChrMarine@Turn360Lf01")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkTightCircleLf01_smooth", "ChrMarine@Turn360Lf01")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkCircleLf01_smooth", "ChrMarine@WalkTightCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeSlowLf01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeSlowLf01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@Run01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@StrafeFastLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@Run01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@StrafeFastLf01_smooth")

		statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@Meander01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkCircleRt01_smooth", "ChrMarine@WalkTightCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkTightCircleRt01_smooth", "ChrMarine@Turn360Rt01")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkTightCircleRt01_smooth", "ChrMarine@Turn360Rt01")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeSlowRt01", "ChrMarine@WalkCircleRt01_smooth", "ChrMarine@WalkTightCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeSlowRt01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeSlowRt01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@Run01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@StrafeFastRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@Run01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@StrafeFastRt01_smooth")

		statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@Meander01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkCircleRt01_smooth", "ChrMarine@WalkTightCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkTightCircleRt01_smooth", "ChrMarine@Turn360Rt01")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkTightCircleRt01_smooth", "ChrMarine@Turn360Rt01")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeSlowLf01", "ChrMarine@WalkCircleRt01_smooth", "ChrMarine@WalkTightCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeSlowLf01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeSlowLf01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleRt01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@Run01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@StrafeFastLf01_smooth")
		statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@Run01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@StrafeFastLf01_smooth")

			
		// statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@Meander01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleLf01_smooth", "ChrMarine@WalkTightCircleLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleLf01_smooth", "ChrMarine@WalkTightCircleLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunCircleLf01_smooth", "ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@Run01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@StrafeFastLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Run01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkTightCircleLf01_smooth", "ChrMarine@Turn360Lf01")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkTightCircleLf01_smooth", "ChrMarine@Turn360Lf01")
		//statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Run01", "ChrMarine@Turn360Lf01")

		// statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@Meander01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleLf01_smooth", "ChrMarine@WalkTightCircleLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleLf01_smooth", "ChrMarine@WalkTightCircleLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunCircleLf01_smooth", "ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@Run01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@StrafeFastRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Run01", "ChrMarine@RunCircleLf01_smooth", "ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkTightCircleLf01_smooth", "ChrMarine@Turn360Lf01")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkTightCircleLf01_smooth", "ChrMarine@Turn360Lf01")
		//statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleLf01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Run01", "ChrMarine@Turn360Lf01")

		// statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@Meander01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleRt01_smooth", "ChrMarine@WalkTightCircleRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkCircleRt01_smooth", "ChrMarine@WalkTightCircleRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunCircleRt01_smooth", "ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@Walk01")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@Run01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@StrafeFastLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Run01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastLf01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkTightCircleRt01_smooth", "ChrMarine@Turn360Rt01")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastLf01_smooth", "ChrMarine@WalkTightCircleRt01_smooth", "ChrMarine@Turn360Rt01")

		// statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@Meander01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleRt01_smooth", "ChrMarine@WalkTightCircleRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Meander01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01", "ChrMarine@WalkCircleRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkCircleRt01_smooth", "ChrMarine@WalkTightCircleRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunCircleRt01_smooth", "ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@Walk01")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Jog01", "ChrMarine@Run01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@StrafeFastRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Run01", "ChrMarine@RunCircleRt01_smooth", "ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastRt01_smooth")
		// statemarineLocomotion.addTetrahedron("ChrMarine@Idle01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkTightCircleRt01_smooth", "ChrMarine@Turn360Rt01")
		// statemarineLocomotion.addTetrahedron("ChrMarine@RunTightCircleRt01", "ChrMarine@StrafeFastRt01_smooth", "ChrMarine@WalkTightCircleRt01_smooth", "ChrMarine@Turn360Rt01")
	}
}
