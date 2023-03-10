# state ChrMarineStartingLeft
# autogenerated by SmartBody

stateManager = scene.getStateManager()

stateChrMarineStartingRight = stateManager.createState1D("mocapStartingRight")
stateChrMarineStartingRight.setBlendSkeleton('ChrBackovic.sk')
motions = StringVec()
motions.append("ChrMarine@Idle01_ToWalkLf01")
motions.append("ChrMarine@Idle01_ToWalk01_Turn90Rt01")
motions.append("ChrMarine@Idle01_ToWalk01_Turn180Rt01")

paramsX = DoubleVec()
paramsX.append(0)  # ChrMarine@Idle01 X
paramsX.append(90)  # ChrMarine@Idle01_ToWalk01_Turn90Rt01 X
paramsX.append(165)  # ChrMarine@Idle01_ToWalk01_Turn180Rt01 X
for i in range(0, len(motions)):
    stateChrMarineStartingRight.addMotion(motions[i], paramsX[i])

points0 = DoubleVec()
points0.append(0)  # ChrBrad_ChrMarine@Idle01 0
points0.append(0)  # ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 0
points0.append(0)  # ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 0
stateChrMarineStartingRight.addCorrespondencePoints(motions, points0)
points1 = DoubleVec()
points1.append(0.73)  # ChrBrad_ChrMarine@Idle01 1
points1.append(1.42)  # ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 1
points1.append(1.37)  # ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 1
stateChrMarineStartingRight.addCorrespondencePoints(motions, points1)
points2 = DoubleVec()
points2.append(1.32)  # ChrBrad_ChrMarine@Idle01 2
points2.append(2.08)  # ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 2
points2.append(2.06)  # ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 2
stateChrMarineStartingRight.addCorrespondencePoints(motions, points2)
points3 = DoubleVec()
points3.append(1.56)  # ChrBrad_ChrMarine@Idle01 2
points3.append(2.43)  # ChrBrad_ChrMarine@Idle01_ToWalk01_Turn90Rt01 2
points3.append(2.46)  # ChrBrad_ChrMarine@Idle01_ToWalk01_Turn180Rt01 2
stateChrMarineStartingRight.addCorrespondencePoints(motions, points3)
