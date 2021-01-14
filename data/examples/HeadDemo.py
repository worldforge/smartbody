import random

print("|--------------------------------------------|")
print("|            Starting Head Demo              |")
print("|--------------------------------------------|")

# Add asset paths
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath('motion', 'ChrBrad')
scene.addAssetPath('script', 'scripts')
scene.loadAssets()

# Set scene parameters and camera
print('Configuring scene parameters and camera')
scene.setScale(1.0)
scene.run("default-viewer.py")
camera = getCamera()
camera.setEye(0.0, 1.65, 0.38)
camera.setCenter(0.0, 1.59, -0.02)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(0.966897)

# Setting up joint map for Brad
print('Setting up joint map and configuring Brad\'s skeleton')
scene.run('zebra2-map.py')
zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
bradSkeleton = scene.getSkeleton('ChrBrad.sk')
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse(scene, 'ChrBrad')

characters = ['ChrBrad']
faces = {}
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
    faces[c] = face


# Setting up Brad
print('Setting up Brad')
brad = scene.createCharacter('ChrBrad', '')
bradSkeleton = scene.createSkeleton('ChrBrad.sk')
brad.setSkeleton(bradSkeleton)
brad.setFaceDefinition(faces['ChrBrad'])
brad.createStandardControllers()
# Deformable mesh
brad.setVec3Attribute('deformableMeshScale', .01, .01, .01)
brad.setStringAttribute('deformableMesh', 'ChrBrad.dae')

# Turn on GPU deformable Geometry
brad.setStringAttribute("displayType", "GPUmesh")




last = 0
canTime = True
delay = 5


class HeadDemo(SBScript):
    def update(self, time):
        global last, canTime
        diff = time - last
        if diff >= delay:
            diff = 0
            canTime = True
        # When time's up do action
        if canTime:
            last = time
            canTime = False
            nextHead()


headList = ['NOD', 'SHAKE', 'TOSS', 'WIGGLE', 'WAGGLE']
curHead = 0
headAmt = len(headList)


def nextHead():
    global curHead
    headChoice = headList[curHead]
    print('Playing %s' % headChoice)
    # Random repeats, velocity and amount
    repeats = random.uniform(0.1, 2)
    amount = random.uniform(0.1, 1) / 2.0
    # Perform BML command
    bml.execBML('*', '<head type="' + headChoice + '" amount="' + str(amount) + '" repeats="' + str(repeats) + '"/>')
    # Increment index, reset when hit max
    curHead = curHead + 1
    if curHead >= headAmt:
        curHead = 0


# Run the update script
scene.removeScript('headdemo')
headdemo = HeadDemo()
scene.addScript('headdemo', headdemo)
