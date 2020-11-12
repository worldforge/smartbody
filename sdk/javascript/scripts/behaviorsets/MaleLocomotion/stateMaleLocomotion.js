var stateMaleLocomotion = {
	locomotionSetup: function (origSkelName, skeletonName, baseJoint, preFix, statePreFix){
		console.log("** State: allLocomotion")
		var stateManager = scene.getStateManager()
		var stateName = statePreFix+"Locomotion"
		if (stateManager.getBlend(stateName) != null) // don't create duplicate state
			return		
		var state = stateManager.createBlend3D(stateName)
		
		state.setBlendSkeleton(origSkelName)
		// add motions
		var originalMotions = new Module.StringVec()
		originalMotions.push_back("ChrUtah_Walk001")
		originalMotions.push_back("ChrUtah_Idle001")
		originalMotions.push_back("ChrUtah_Idle01_ToWalk01_Turn360Lf01")
		originalMotions.push_back("ChrUtah_Idle01_ToWalk01_Turn360Rt01")
		originalMotions.push_back("ChrUtah_Meander01")
		originalMotions.push_back("ChrUtah_Shuffle01")
		originalMotions.push_back("ChrUtah_Jog001")
		originalMotions.push_back("ChrUtah_Run001")
		originalMotions.push_back("ChrUtah_WalkInCircleLeft001")
		originalMotions.push_back("ChrUtah_WalkInCircleRight001")
		originalMotions.push_back("ChrUtah_WalkInTightCircleLeft001")
		originalMotions.push_back("ChrUtah_WalkInTightCircleRight001")
		originalMotions.push_back("ChrUtah_RunInCircleLeft001")
		originalMotions.push_back("ChrUtah_RunInCircleRight001")
		originalMotions.push_back("ChrUtah_RunInTightCircleLeft01")
		originalMotions.push_back("ChrUtah_RunInTightCircleRight01")
		originalMotions.push_back("ChrUtah_StrafeSlowRt01")
		originalMotions.push_back("ChrUtah_StrafeSlowLf01")
		originalMotions.push_back("ChrUtah_StrafeFastRt01")
		originalMotions.push_back("ChrUtah_StrafeFastLf01")
		
		var motions = new Module.StringVec()
		var assetManager = scene.getAssetManager()
		for(var i = 0; i < originalMotions.size(); ++i){
			motions.push_back(preFix + originalMotions.get(i))
			var sbMotion = assetManager.getMotion(originalMotions.get(i))
			if(sbMotion != null)
				sbMotion.setMotionSkeletonName(origSkelName)
		}
		for(var i = 0; i < motions.size(); ++i){
			state.addMotion(motions.get(i), 0, 0, 0)
		}


		// correspondance points
		var floatarray = [0, 0.811475, 1.34262, 2.13333, 0, 0.698851, 1.37931, 2.13333, 0, 0.82023, 1.30207, 2.12966, 0, 0.812874, 1.35356, 2.12966, 0, 0.988525, 1.87377, 3.06667, 0, 0.713115, 1.29836, 2.13333, 0, 0.501639, 0.92459, 1.6, 0, 0.422951, 0.772131, 1.33333, 0, 0.840984, 1.39672, 2.13333, 0, 0.840984, 1.29836, 2.13333, 0, 0.845902, 1.30328, 2.13333, 0, 0.880328, 1.33279, 2.13333, 0, 0.437705, 0.811475, 1.33333, 0, 0.452459, 0.791803, 1.33333, 0, 0.462295, 0.757377, 1.33333, 0, 0.452459, 0.796721, 1.33333, 0, 0.90000, 1.41000, 2.13000, 0.72, 1.38, 1.92, 0.72, 0, 0.473684, 0.920079, 1.6, 0.4, 0.893233, 1.28421, 0.4]
		var numCorrespondancePoints = 4
		if (floatarray.length != 4 * motions.size())
			console.log("**Correspondance points input wrong")
			
		for(var i = 0; i < numCorrespondancePoints; ++i){
			var points = new Module.DoubleVec()
			for(var j = 0; j < motions.size(); ++j){
				points.push_back(floatarray[j * numCorrespondancePoints + i])
			}
			state.addCorrespondancePoints(motions, points)
		}
		// reset parameters (because it needs context of correspondance points, extract parameters from motion)
		var skeleton = scene.getSkeleton(skeletonName)
		var joint = skeleton.getJointByName(baseJoint)

		var travelDirection = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 90, -90, 90, -90]
		for(var i = 0; i < motions.size(); ++i){
			var motion = scene.getMotion(motions.get(i))
			motion.connect(skeleton)
			var correspondancePoints = state.getCorrespondancePoints(i)
			var lenCorrespondancePoints = correspondancePoints.size();
			var speed = motion.getJointSpeed(joint, correspondancePoints.get(0), correspondancePoints.get(lenCorrespondancePoints - 1))
			var omega = motion.getJointAngularSpeed(joint, correspondancePoints.get(0), correspondancePoints.get(lenCorrespondancePoints - 1))
			var direction = travelDirection[i]
			//console.log("Motion: " + motions.get(i) + " " + "Speed: " + speed + " " + "Omega: " + omega + " " + "direction: " + direction);
			state.setParameter(motions.get(i), speed, omega, direction)
			motion.disconnect()
		}
											 
		// add tetrahedrons (need automatic way to generate)
		state.addTetrahedron(preFix+"ChrUtah_Shuffle01", preFix+"ChrUtah_Meander01", preFix+"ChrUtah_WalkInTightCircleLeft001", preFix+"ChrUtah_StrafeSlowRt01")
		state.addTetrahedron(preFix+"ChrUtah_Shuffle01", preFix+"ChrUtah_Meander01", preFix+"ChrUtah_WalkInTightCircleLeft001", preFix+"ChrUtah_StrafeSlowLf01")
		state.addTetrahedron(preFix+"ChrUtah_Shuffle01", preFix+"ChrUtah_Meander01", preFix+"ChrUtah_WalkInTightCircleRight001", preFix+"ChrUtah_StrafeSlowRt01")
		state.addTetrahedron(preFix+"ChrUtah_Shuffle01", preFix+"ChrUtah_Meander01", preFix+"ChrUtah_WalkInTightCircleRight001", preFix+"ChrUtah_StrafeSlowLf01")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_Walk001", preFix+"ChrUtah_Jog001", preFix+"ChrUtah_WalkInCircleRight001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_Walk001", preFix+"ChrUtah_Jog001", preFix+"ChrUtah_WalkInCircleLeft001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_Jog001", preFix+"ChrUtah_Run001", preFix+"ChrUtah_RunInCircleLeft001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_Jog001", preFix+"ChrUtah_Run001", preFix+"ChrUtah_RunInCircleRight001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_WalkInCircleLeft001", preFix+"ChrUtah_RunInCircleLeft001", preFix+"ChrUtah_Jog001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_WalkInCircleRight001", preFix+"ChrUtah_RunInCircleRight001", preFix+"ChrUtah_Jog001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_WalkInCircleLeft001", preFix+"ChrUtah_WalkInTightCircleLeft001", preFix+"ChrUtah_RunInCircleLeft001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_WalkInCircleRight001", preFix+"ChrUtah_WalkInTightCircleRight001", preFix+"ChrUtah_RunInCircleRight001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_WalkInTightCircleLeft001", preFix+"ChrUtah_RunInCircleLeft001", preFix+"ChrUtah_RunInTightCircleLeft01")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_WalkInTightCircleRight001", preFix+"ChrUtah_RunInCircleRight001", preFix+"ChrUtah_RunInTightCircleRight01")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_Meander01", preFix+"ChrUtah_Walk001", preFix+"ChrUtah_WalkInCircleLeft001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_Meander01", preFix+"ChrUtah_WalkInCircleLeft001", preFix+"ChrUtah_WalkInTightCircleLeft001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_Meander01", preFix+"ChrUtah_Walk001", preFix+"ChrUtah_WalkInCircleRight001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_Meander01", preFix+"ChrUtah_WalkInCircleRight001", preFix+"ChrUtah_WalkInTightCircleRight001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_Walk001", preFix+"ChrUtah_Jog001", preFix+"ChrUtah_WalkInCircleRight001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_Walk001", preFix+"ChrUtah_Jog001", preFix+"ChrUtah_WalkInCircleLeft001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_Jog001", preFix+"ChrUtah_Run001", preFix+"ChrUtah_RunInCircleLeft001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_Jog001", preFix+"ChrUtah_Run001", preFix+"ChrUtah_RunInCircleRight001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_WalkInCircleLeft001", preFix+"ChrUtah_RunInCircleLeft001", preFix+"ChrUtah_Jog001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_WalkInCircleRight001", preFix+"ChrUtah_RunInCircleRight001", preFix+"ChrUtah_Jog001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_WalkInCircleLeft001", preFix+"ChrUtah_WalkInTightCircleLeft001", preFix+"ChrUtah_RunInCircleLeft001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_WalkInCircleRight001", preFix+"ChrUtah_WalkInTightCircleRight001", preFix+"ChrUtah_RunInCircleRight001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_WalkInTightCircleLeft001", preFix+"ChrUtah_RunInCircleLeft001", preFix+"ChrUtah_RunInTightCircleLeft01")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_WalkInTightCircleRight001", preFix+"ChrUtah_RunInCircleRight001", preFix+"ChrUtah_RunInTightCircleRight01")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_Meander01", preFix+"ChrUtah_Walk001", preFix+"ChrUtah_WalkInCircleLeft001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_Meander01", preFix+"ChrUtah_WalkInCircleLeft001", preFix+"ChrUtah_WalkInTightCircleLeft001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_Meander01", preFix+"ChrUtah_Walk001", preFix+"ChrUtah_WalkInCircleRight001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_Meander01", preFix+"ChrUtah_WalkInCircleRight001", preFix+"ChrUtah_WalkInTightCircleRight001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_StrafeFastRt01", preFix+"ChrUtah_RunInCircleRight001", preFix+"ChrUtah_RunInTightCircleRight01")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_StrafeFastRt01", preFix+"ChrUtah_RunInCircleRight001", preFix+"ChrUtah_Run001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_StrafeFastRt01", preFix+"ChrUtah_RunInCircleLeft001", preFix+"ChrUtah_RunInTightCircleLeft01")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_StrafeFastRt01", preFix+"ChrUtah_RunInCircleLeft001", preFix+"ChrUtah_Run001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_StrafeFastLf01", preFix+"ChrUtah_RunInCircleRight001", preFix+"ChrUtah_RunInTightCircleRight01")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_StrafeFastLf01", preFix+"ChrUtah_RunInCircleRight001", preFix+"ChrUtah_Run001")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_StrafeFastLf01", preFix+"ChrUtah_RunInCircleLeft001", preFix+"ChrUtah_RunInTightCircleLeft01")
		state.addTetrahedron(preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_StrafeFastLf01", preFix+"ChrUtah_RunInCircleLeft001", preFix+"ChrUtah_Run001")
		state.addTetrahedron(preFix+"ChrUtah_Shuffle01", preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_WalkInTightCircleLeft001", preFix+"ChrUtah_Idle01_ToWalk01_Turn360Lf01")
		state.addTetrahedron(preFix+"ChrUtah_Shuffle01", preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_WalkInTightCircleRight001", preFix+"ChrUtah_Idle01_ToWalk01_Turn360Rt01")
		state.addTetrahedron(preFix+"ChrUtah_Shuffle01", preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_Idle001", preFix+"ChrUtah_Idle01_ToWalk01_Turn360Lf01")
		state.addTetrahedron(preFix+"ChrUtah_Shuffle01", preFix+"ChrUtah_StrafeSlowLf01", preFix+"ChrUtah_Idle001", preFix+"ChrUtah_Idle01_ToWalk01_Turn360Rt01")
		state.addTetrahedron(preFix+"ChrUtah_Shuffle01", preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_WalkInTightCircleLeft001", preFix+"ChrUtah_Idle01_ToWalk01_Turn360Lf01")
		state.addTetrahedron(preFix+"ChrUtah_Shuffle01", preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_WalkInTightCircleRight001", preFix+"ChrUtah_Idle01_ToWalk01_Turn360Rt01")
		state.addTetrahedron(preFix+"ChrUtah_Shuffle01", preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_Idle001", preFix+"ChrUtah_Idle01_ToWalk01_Turn360Lf01")
		state.addTetrahedron(preFix+"ChrUtah_Shuffle01", preFix+"ChrUtah_StrafeSlowRt01", preFix+"ChrUtah_Idle001", preFix+"ChrUtah_Idle01_ToWalk01_Turn360Rt01")
	}
}
