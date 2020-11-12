print "** State: allCrouchProtectHead"

mirrorMotion = scene.getMotion("ChrUtah_Relax001_CrouchProtectHead_right")
mirrorMotion.mirror("ChrUtah_Relax001_CrouchProtectHead_left", "test_utah.sk")

stateManager = scene.getStateManager()
state1 = stateManager.createState1D("allCrouchProtectHead")
motions = StringVec()
motions.append("ChrUtah_Relax001_CrouchProtectHead_center")
motions.append("ChrUtah_Relax001_CrouchProtectHead_left")
motions.append("ChrUtah_Relax001_CrouchProtectHead_right")

params = DoubleVec()
params.append(0)
params.append(-90)
params.append(90)

for i in range(0, len(motions)):
	state1.addMotion(motions[i], params[i])

points1 = DoubleVec()
points1.append(0)
points1.append(0)
points1.append(0)
state1.addCorrespondancePoints(motions, points1)

points2 = DoubleVec()
points2.append(1.0331)
points2.append(1.33724)
points2.append(1.3131)
state1.addCorrespondancePoints(motions, points2)

points3 = DoubleVec()
points3.append(2.46667)
points3.append(2.8)
points3.append(2.79517)
state1.addCorrespondancePoints(motions, points3)


