print "|--------------------------------------------|"
print "|  data/sbm-common/scripts/rachelrachel.py  |"
print "|--------------------------------------------|"


### Load data/sbm-common assets
scene.addAssetPath("script", "sbm-common/scripts")
scene.addAssetPath("mesh", "mesh")
scene.addAssetPath("audio", ".")

scene.setBoolAttribute("internalAudio", True)
scene.setBoolAttribute("useGPUBlendshapes",False)
scene.run("default-viewer.py")

camera = scene.getActiveCamera()
camera.setEye(0.0, 1.7, 2)
camera.setCenter(0.08, 1.4, 0)
camera.setUpVector(SrVec(0, 1, 0))
camera.setFov(0.4)
camera.setFarPlane(100)
camera.setNearPlane(0.1)


# map to the SmartBody standard
scene.run("zebra2-map.py")
zebra2Map = scene.getJointMapManager().getJointMap("zebra2")

scene.addAssetPath("motion", "ChrRachel")
scene.loadAssets()

# establish lip syncing data set
scene.run("init-diphoneDefault.py")

# load up gesture map
scene.run("gestureMap.py")


############################# rachel
rachelSkeleton = scene.getSkeleton("ChrRachel.sk")
zebra2Map.applySkeleton(rachelSkeleton)
zebra2Map.applyMotionRecurse("ChrRachel")

# rachel's face definition
rachelFace = scene.createFaceDefinition("ChrRachel")
rachelFace.setFaceNeutral("ChrRachel@face_neutral")

rachelFace.setAU(1,  "left",  "ChrRachel@001_inner_brow_raiser_lf")
rachelFace.setAU(1,  "right", "ChrRachel@001_inner_brow_raiser_rt")
rachelFace.setAU(2,  "left",  "ChrRachel@002_outer_brow_raiser_lf")
rachelFace.setAU(2,  "right", "ChrRachel@002_outer_brow_raiser_rt")
rachelFace.setAU(4,  "left",  "ChrRachel@004_brow_lowerer_lf")
rachelFace.setAU(4,  "right", "ChrRachel@004_brow_lowerer_rt")
rachelFace.setAU(5,  "both",  "ChrRachel@005_upper_lid_raiser")
rachelFace.setAU(6,  "both",  "ChrRachel@006_cheek_raiser")
rachelFace.setAU(7,  "both",  "ChrRachel@007_lid_tightener")
rachelFace.setAU(10, "both",  "ChrRachel@010_upper_lip_raiser")
rachelFace.setAU(12, "left",  "ChrRachel@012_lip_corner_puller_lf")
rachelFace.setAU(12, "right", "ChrRachel@012_lip_corner_puller_rt")
rachelFace.setAU(25, "both",  "ChrRachel@025_lips_part")
rachelFace.setAU(26, "both",  "ChrRachel@026_jaw_drop")
rachelFace.setAU(45, "left",  "ChrRachel@045_blink_lf")
rachelFace.setAU(45, "right", "ChrRachel@045_blink_rt")

rachelFace.setViseme("open",    "ChrRachel@open")
rachelFace.setViseme("W",       "ChrRachel@W")
rachelFace.setViseme("ShCh",    "ChrRachel@ShCh")
rachelFace.setViseme("PBM",     "ChrRachel@PBM")
rachelFace.setViseme("FV",      "ChrRachel@FV")
rachelFace.setViseme("wide",    "ChrRachel@wide")
rachelFace.setViseme("tBack",   "ChrRachel@tBack")
rachelFace.setViseme("tRoof",   "ChrRachel@tRoof")
rachelFace.setViseme("tTeeth",  "ChrRachel@tTeeth")

rachel = scene.createCharacter("ChrRachel", "")
rachelSkeleton = scene.createSkeleton("ChrRachel.sk")
rachel.setSkeleton(rachelSkeleton)
rachel.setFaceDefinition(rachelFace)
rachelPos = SrVec(0, 0, 0)
rachel.setPosition(rachelPos)
rachelHPR = SrVec(0, 0, 0)
rachel.setHPR(rachelHPR)
rachel.createStandardControllers()
# deformable mesh
rachel.setDoubleAttribute("deformableMeshScale", 0.01)
rachel.setStringAttribute("deformableMesh", "ChrRachel.dmb")
rachel.setStringAttribute("displayType", "mesh")
# lip syncing diphone setup
rachel.setStringAttribute("lipSyncSetName", "default")
rachel.setBoolAttribute("usePhoneBigram", True)
rachel.setVoice("audiofile")
rachel.setVoiceCode("Sounds")
# setup gesture map
rachel.setStringAttribute("gestureMap", "ChrRachel")
rachel.setBoolAttribute("gestureRequest.autoGestureTransition", True)

# start the simulation
sim.start()
bml.execBML('ChrRachel', '<body posture="ChrRachel_ChrBrad@Idle01"/>')
bml.execBML('ChrRachel', '<saccade mode="listen"/>')
sim.resume()


