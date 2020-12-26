print("|--------------------------------------------|")
print("|         Starting Character Demo            |")
print("|--------------------------------------------|")

scene.setScale(.01)

# Add asset paths
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrBrad')
scene.addAssetPath('motion', 'ChrRachel')
scene.addAssetPath("script", "behaviorsets")
scene.addAssetPath('script', 'scripts')
scene.loadAssets()

scene.setBoolAttribute("internalAudio", True)

# Set up joint map for Brad and Rachel
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')

bradSkeleton = scene.getSkeleton('ChrBrad.sk')
# skeleton is in meters, change to centimeter scale
bradSkeleton.rescale(100)
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse('ChrBrad')

rachelSkeleton = scene.getSkeleton('ChrRachel.sk')
# skeleton is in meters, change to centimeter scale
rachelSkeleton.rescale(100)
zebra2Map.applySkeleton(rachelSkeleton)
zebra2Map.applyMotionRecurse('ChrRachel')

# scale all the motions by 100
motionNames = scene.getMotionNames()
for m in motionNames:
    scene.getMotion(m).scale(100)

# Establish lip syncing data set
print('Establishing lip syncing data set')
scene.run('init-diphoneDefault.py')

# Set up face definition
print('Setting up face definition')

characters = ['ChrBrad', 'ChrRachel']
# face definition
for c in characters:
    face = scene.createFaceDefinition(c)
    face.setFaceNeutral('ChrBrad@face_neutral')
    face.setAU(1, "left", c + "@001_inner_brow_raiser_lf")
    face.setAU(1, "right", c + "@001_inner_brow_raiser_rt")
    face.setAU(2, "left", c + "@002_outer_brow_raiser_lf")
    face.setAU(2, "right", c + "@002_outer_brow_raiser_rt")
    face.setAU(4, "left", c + "@004_brow_lowerer_lf")
    face.setAU(4, "right", c + "@004_brow_lowerer_rt")
    face.setAU(5, "both", c + "@005_upper_lid_raiser")
    face.setAU(6, "both", c + "@006_cheek_raiser")
    face.setAU(7, "both", c + "@007_lid_tightener")
    face.setAU(10, "both", c + "@010_upper_lip_raiser")
    face.setAU(12, "left", c + "@012_lip_corner_puller_lf")
    face.setAU(12, "right", c + "@012_lip_corner_puller_rt")
    face.setAU(25, "both", c + "@025_lips_part")
    face.setAU(26, "both", c + "@026_jaw_drop")
    face.setAU(45, "left", c + "@045_blink_lf")
    face.setAU(45, "right", c + "@045_blink_rt")

    face.setViseme("open", c + "@open")
    face.setViseme("W", c + "@W")
    face.setViseme("ShCh", c + "@ShCh")
    face.setViseme("PBM", c + "@PBM")
    face.setViseme("FV", c + "@FV")
    face.setViseme("wide", c + "@wide")
    face.setViseme("tBack", c + "@tBack")
    face.setViseme("tRoof", c + "@tRoof")
    face.setViseme("tTeeth", c + "@tTeeth")

print('Adding charactesr into scene')

# Set up Brad
brad = scene.createCharacter('ChrBrad', 'ChrMaarten')
bradSkeleton = scene.createSkeleton('ChrBrad.sk')
brad.setSkeleton(bradSkeleton)
# Set position
bradPos = SrVec(50, 0, 0)
brad.setPosition(bradPos)
# Set facing direction
bradFacing = SrVec(0, 0, 0)
brad.setHPR(bradFacing)
# Set face definition
brad.setFaceDefinition(scene.getFaceDefinition('ChrBrad'))
# Set standard controller
brad.createStandardControllers()

# Lip syncing for Brad
brad.setStringAttribute('lipSyncSetName', 'default')
brad.setBoolAttribute('usePhoneBigram', True)
brad.setVoice('remote')
brad.setVoiceCode('Microsoft|Anna')

# Set up Rachel
rachel = scene.createCharacter('ChrRachel', 'ChrRachel')
rachelSkeleton = scene.createSkeleton('ChrRachel.sk')
rachel.setSkeleton(rachelSkeleton)
# Set position
rachelPos = SrVec(-50, 0, 0)
rachel.setPosition(rachelPos)
# Set facing direction
rachelFacing = SrVec(0, 0, 0)
rachel.setHPR(rachelFacing)
# Set face definition
rachel.setFaceDefinition(scene.getFaceDefinition('ChrRachel'))
# Set standard controller
rachel.createStandardControllers()

# Lip syncing for Rachel
rachel.setStringAttribute('lipSyncSetName', 'default')
rachel.setBoolAttribute('usePhoneBigram', True)
rachel.setVoice('remote')
rachel.setVoiceCode('Microsoft|Anna')

# setup locomotion
scene.run('BehaviorSetMaleMocapLocomotion.py')
setupBehaviorSet()
retargetBehaviorSet('ChrBrad')
retargetBehaviorSet('ChrRachel')
'''
# setup gestures
scene.run('BehaviorSetGestures.py')
setupBehaviorSet()
retargetBehaviorSet('ChrBrad')
retargetBehaviorSet('ChrRachel')

# setup reach 
scene.run('BehaviorSetReaching.py')
setupBehaviorSet()
retargetBehaviorSet('ChrBrad')
retargetBehaviorSet('ChrRachel')
'''

# Set up steering
print('Setting up steering')
steerManager.setEnable(False)
steerManager.setEnable(True)
# Start the simulation
print('Starting the simulation')
sim.start()

bml.execBML('ChrBrad', '<body posture="ChrMarine@Idle01"/>')
bml.execBML('ChrBrad', '<saccade mode="listen"/>')

bml.execBML('ChrRachel', '<body posture="ChrMarine@Idle01" start="1"/>')
bml.execBML('ChrRachel', '<saccade mode="listen"/>')

sim.resume()
