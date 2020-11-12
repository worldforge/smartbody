def startingSetup(skeletonName, baseJoint, preFix, statePreFix):
    mirrorMotion = scene.getMotion(preFix + "ChrUtah_StopToWalkRt01")
    mirrorMotion.mirror(preFix + "ChrUtah_StopToWalkLf01", skeletonName)

    mirrorMotion1 = scene.getMotion(preFix + "ChrUtah_Idle01_ToWalk01_Turn90Lf01")
    mirrorMotion1.mirror(preFix + "ChrUtah_Idle01_ToWalk01_Turn90Rt01", skeletonName)
    mirrorMotion2 = scene.getMotion(preFix + "ChrUtah_Idle01_ToWalk01_Turn180Lf01")
    mirrorMotion2.mirror(preFix + "ChrUtah_Idle01_ToWalk01_Turn180Rt01", skeletonName)

    stateManager = scene.getStateManager()

    print("** State: allStartingLeft")
    startLefStateName = statePreFix + "StartingLeft"
    if (stateManager.getBlend(startLefStateName) == None):  # don't create duplicate state
        state1 = stateManager.createState1D(statePreFix + "StartingLeft")
        motions1 = StringVec()
        motions1.append(preFix + "ChrUtah_StopToWalkLf01")
        motions1.append(preFix + "ChrUtah_Idle01_ToWalk01_Turn90Lf01")
        motions1.append(preFix + "ChrUtah_Idle01_ToWalk01_Turn180Lf01")
        params1 = DoubleVec()
        params1.append(0)
        params1.append(-90)
        params1.append(-180)

        for i in range(0, len(motions1)):
            state1.addMotion(motions1[i], params1[i])

        points1 = DoubleVec()
        points1.append(0)
        points1.append(0)
        points1.append(0)
        state1.addCorrespondancePoints(motions1, points1)
        points2 = DoubleVec()
        points2.append(0.57541)
        points2.append(1.2)
        points2.append(1.35)
        state1.addCorrespondancePoints(motions1, points2)
        points3 = DoubleVec()
        points3.append(0.943716)
        points3.append(1.41)
        points3.append(1.6)
        state1.addCorrespondancePoints(motions1, points3)

    print("** State: allStartingRight")

    startRightStateName = statePreFix + "StartingRight"
    if (stateManager.getBlend(startRightStateName) == None):
        state2 = stateManager.createState1D(statePreFix + "StartingRight")
        motions2 = StringVec()
        motions2.append(preFix + "ChrUtah_StopToWalkRt01")
        motions2.append(preFix + "ChrUtah_Idle01_ToWalk01_Turn90Rt01")
        motions2.append(preFix + "ChrUtah_Idle01_ToWalk01_Turn180Rt01")
        params2 = DoubleVec()
        params2.append(0)
        params2.append(90)
        params2.append(180)

        for i in range(0, len(motions2)):
            state2.addMotion(motions2[i], params2[i])

        # Since the right is symetric with the left, so the correspondance points are the same
        state2.addCorrespondancePoints(motions2, points1)
        state2.addCorrespondancePoints(motions2, points2)
        state2.addCorrespondancePoints(motions2, points3)
