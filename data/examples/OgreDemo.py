import math
import random

print("|--------------------------------------------|")
print("|         Starting Ogre Demo           |")
print("|--------------------------------------------|")

scene.setScale(0.1)
# Add asset paths

scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'Sinbad')
scene.addAssetPath('script', 'scripts')
scene.addAssetPath('script', 'behaviorsets')
scene.loadAssets()

# Set scene parameters and camera
# scene.getPawn('camera').setPosition(SrVec(0, -5, 0))

# Set joint map for Sinbad
print('Setting up joint map for Brad')
scene.run('ogre-sinbad-map.py')
sinbadSkName = 'Sinbad.skeleton.xml'
jointMapManager = scene.getJointMapManager()
sinbadMap = jointMapManager.getJointMap('Sinbad.skeleton.xml')
ogreSk = scene.getSkeleton(sinbadSkName)
sinbadMap.applySkeleton(ogreSk)

# Behavior set setup
scene.run('behaviorsetup.py')

# Animation setup
# scene.run('init-param-animation.py')
steerManager = scene.getSteerManager()

# Setting up Sinbad
print('Setting up Sinbad')
sinbadName = 'sinbad'
sinbad = scene.createCharacter(sinbadName, '')
sinbadSk = scene.createSkeleton(sinbadSkName)
sinbad.setSkeleton(sinbadSk)
sinbadPos = SrVec(0, 5.16, 0)
sinbad.setPosition(sinbadPos)
sinbad.createStandardControllers()
sinbad.setStringAttribute('deformableMesh', 'Sinbad.mesh.xml')
# setup gestures
scene.run('BehaviorSetGestures.py')
setupBehaviorSet()
retargetBehaviorSet(sinbadName)

# setup locomotion
scene.run('BehaviorSetMaleLocomotion.py')
setupBehaviorSet()
retargetBehaviorSet(sinbadName)
# setup reaching
scene.run('BehaviorSetReaching.py')
setupBehaviorSet()
retargetBehaviorSet(sinbadName)
# setup gestures
scene.run('BehaviorSetGestures.py')
setupBehaviorSet()
retargetBehaviorSet(sinbadName)

sinbad.setStringAttribute("displayType", "GPUmesh")

print('Configuring scene parameters and camera')
scene.setBoolAttribute('internalAudio', True)
camera = getCamera()
camera.setEye(0, 5.98, 13.44)
camera.setCenter(1.0, 1.7, -39.5)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(1.02)

sim.start()
bml.execBML(sinbadName, '<body posture="ChrUtah_Idle001"/>')
sim.resume()

g = scene.getGestureMapManager().getGestureMap("BradGesture")
g.addGestureMapping("test", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")

skeleton = "ChrBrad2.sk"
stateNames = []
stateNames.append("BeathHighBt")
stateNames.append("BeathFistMidLf")
stateNames.append("BeathLowBt")
stateNames.append("ChopLf")
stateNames.append("HereBt")
stateNames.append("InclusivityNegBt")
stateNames.append("InclusivityPosBt")
stateNames.append("MeLf")
stateNames.append("NegativeBt")
stateNames.append("PointLf")
stateNames.append("Shrug")
stateNames.append("Stop")
stateNames.append("Surround")

motionNames = []
motionNames.append("ChrBrad@Idle01_BeatHighBt01")
motionNames.append("ChrBrad@Idle01_BeatHighBt03")
motionNames.append("ChrBrad@Idle01_BeatFistMidLf01")
motionNames.append("ChrBrad@Idle01_BeatFistMidLf02")
motionNames.append("ChrBrad@Idle01_BeatLowBt01")
motionNames.append("ChrBrad@Idle01_BeatLowBt02")
motionNames.append("ChrBrad@Idle01_ChopLf01")
motionNames.append("ChrBrad@Idle01_ChopLf02")
motionNames.append("ChrBrad@Idle01_HereBt01")
motionNames.append("ChrBrad@Idle01_HereBt02")
motionNames.append("ChrBrad@Idle01_InclusivityNegBt01")
motionNames.append("ChrBrad@Idle01_InclusivityNegBt02")
motionNames.append("ChrBrad@Idle01_InclusivityPosBt01")
motionNames.append("ChrBrad@Idle01_InclusivityPosBt02")
motionNames.append("ChrBrad@Idle01_MeLf01")
motionNames.append("ChrBrad@Idle01_MeLf02")
motionNames.append("ChrBrad@Idle01_NegativeBt01")
motionNames.append("ChrBrad@Idle01_NegativeBt02")
motionNames.append("ChrBrad@Idle01_PointLf01")
motionNames.append("ChrBrad@Idle01_PointLf02")
motionNames.append("ChrBrad@Idle01_Shrug01")
motionNames.append("ChrBrad@Idle01_Shrug02")
motionNames.append("ChrBrad@Idle01_StopBt01")
motionNames.append("ChrBrad@Idle01_StopBt02")
motionNames.append("ChrBrad@Idle01_SurroundBt01")
motionNames.append("ChrBrad@Idle01_SurroundBt02")

for b in range(0, len(stateNames)):
    stateTest = scene.getBlendManager().createState1D(stateNames[b])
    stateTest.setBlendSkeleton(skeleton)
    motions = StringVec()
    motions.append(motionNames[b * 2])
    motions.append(motionNames[b * 2 + 1])

    stateTest.addMotion(motionNames[b * 2], 0.0)
    stateTest.addMotion(motionNames[b * 2 + 1], 1.0)
    print("Added gesture blend " + stateNames[b])

bml.execBML(sinbadName, '<body posture="ChrBrad@Idle01"/>')
