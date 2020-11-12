print "|--------------------------------------------|"
print "|  data/sbm-common/scripts/default-init.py   |"
print "|--------------------------------------------|"


### Load data/sbm-common assets
scene.addAssetPath("script", "sbm-common/scripts")
scene.addAssetPath("motion", "sbm-common/common-sk")
scene.addAssetPath("mesh", "./mesh")
scene.loadAssets()

scene.setBoolAttribute("internalAudio", True)

scene.run("init-common-face.py")

# camera
camera = scene.getActiveCamera()
camera.setCenter(0, 92, 0);
camera.setUpVector(SrVec(0, 1, 0))
camera.setEye( 0, 166, 300 )

brad = scene.createCharacter("doctor", "")
bradSkeleton = scene.createSkeleton("common.sk")
brad.setSkeleton(bradSkeleton)
brad.setFaceDefinition(defaultFace)
bradPos = SrVec(0, 107, 0)
brad.setPosition(bradPos)
bradHPR = SrVec(0, 0, 0)
brad.setHPR(bradHPR)
brad.createStandardControllers()
brad.setStringAttribute("deformableMesh", "doctor")

# start the simulation
sim.start()
bml.execBML('doctor', '<body posture="LHandOnHip_Motex"/>')
bml.execBML('doctor', '<saccade mode="listen"/>')
sim.resume()

