print "** State: allCrouchSwayBlock"

stateManager = scene.getStateManager()
state = stateManager.createState2D("allCrouchSwayBlock")

motions = StringVec()
motions.append("ChrUtah_CrouchBlock02High_swayLf")
motions.append("ChrUtah_CrouchBlock02High_swayRt")
motions.append("ChrUtah_CrouchBlock02Low_swayLf")
motions.append("ChrUtah_CrouchBlock02Low_swayRt")

params1 = DoubleVec()
params1.append(-40)
params1.append(40)
params1.append(-40)
params1.append(40)

params2 = DoubleVec()
params2.append(100)
params2.append(100)
params2.append(60)
params2.append(60)

for i in range(0, len(motions)):
	state2.addMotion(motions[i], params1[i], params2[i])

points1 = DoubleVec()
points1.append(0)
points1.append(0)
points1.append(0)
points1.append(0)
state2.addCorrespondancePoints(motions, points1)

points2 = DoubleVec()
points2.append(0.671264)
points2.append(0.528736)
points2.append(0.590805)
points2.append(0.68046)
state2.addCorrespondancePoints(motions, points2)

points3 = DoubleVec()
points3.append(1.3)
points3.append(1.26667)
points3.append(1.3)
points3.append(1.33333)
state2.addCorrespondancePoints(motions, points3)

state2.addTriangle("ChrUtah_CrouchBlock02High_swayLf", "ChrUtah_CrouchBlock02High_swayRt", "ChrUtah_CrouchBlock02Low_swayLf")
state2.addTriangle("ChrUtah_CrouchBlock02High_swayRt", "ChrUtah_CrouchBlock02Low_swayLf", "ChrUtah_CrouchBlock02Low_swayRt")

