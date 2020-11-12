print("|-------------------------------------------------|")
print("|  Mixamo character setup                         |")
print("|-------------------------------------------------|")

skelName = 'mixamo.dae'
characterName = 'mycharacter'
meshName = 'mixamo.dae'

# first, load in the Mixamo asset explicitly

scene.loadAssetsFromPath("mesh/mixamo/mixamo.dae")

# set up location of other assets
scene.addAssetPath("script", "behaviorsets")
scene.addAssetPath("script", "sbm-common/scripts")
scene.addAssetPath("mesh", "mesh")

# load in the mapping from Mixamo to SmartBody
# mappings are inside the file mixamo-map.py
jointMapManager = scene.getJointMapManager()

# map the skeleton to the SmartBody skeleton
skeletonInstance = scene.getSkeleton(skelName)

# two options here:
# 1) have SmartBody guess at the mapping 
mixamoMap = jointMapManager.createJointMap("mixamoMap")
mixamoMap.guessMapping(skeletonInstance, True)

# 2) load an explicit mapping for mixamo
# scene.run("mixamo-map.py")
# mixamoMap = jointMapManager.getJointMap('mixamo')

# now apply that mapping to the skeleton, so that all 
# future uses of that skeleton will be properly mapped
mixamoMap.applySkeleton(skeletonInstance)

# create the character
sbChar = scene.createCharacter(characterName, "")
sbSkeleton = scene.createSkeleton(skelName)
sbChar.setSkeleton(sbSkeleton)
sbPos = SrVec(0, 0, 0)
sbChar.setPosition(sbPos)
sbChar.createStandardControllers()
sbChar.setStringAttribute("deformableMesh", meshName)
sbChar.setStringAttribute("displayType", "GPUMesh")

# setup locomotion
scene.run('BehaviorSetMaleMocapLocomotion.py')
setupBehaviorSet()
retargetBehaviorSet(characterName)

# setup reaching
scene.run('BehaviorSetMocapReaching.py')
setupBehaviorSet()
retargetBehaviorSet(characterName)

# setup gesturing
scene.run('BehaviorSetGestures.py')
setupBehaviorSet()
retargetBehaviorSet(characterName)

# start the simulation
sim.start()
bml.execBML(characterName, '<body posture="ChrMarine@Idle01"/>')

# some commands to try:
# bml.execBML(characterName, '<locomotion target="100 200"/>')
# bml.execBML(characterName, '<gesture name="ChrBrad@Idle01_Contemplate01"/>')

sim.resume()
