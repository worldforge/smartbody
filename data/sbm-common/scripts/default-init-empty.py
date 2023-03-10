print("|--------------------------------------------|")
print("|  data/sbm-common/scripts/default-init.py  |")
print("|--------------------------------------------|")

import SmartBody
from SmartBody import *

sim = scene.getSimulationManager()
#bml = scene.getBmlProcessor()
scene.setScale(.01)


def setupFace(charName):
    defaultFace = scene.createFaceDefinition("_default_")
    if defaultFace is not None:
        defaultFace.setFaceNeutral("face_neutral")
        defaultFace.setAU(1, "left", "fac_1L_inner_brow_raiser")
        defaultFace.setAU(1, "right", "fac_1R_inner_brow_raiser")
        defaultFace.setAU(2, "left", "fac_2L_outer_brow_raiser")
        defaultFace.setAU(2, "right", "fac_2R_outer_brow_raiser")
        defaultFace.setAU(4, "left", "fac_4L_brow_lowerer")
        defaultFace.setAU(4, "right", "fac_4R_brow_lowerer")
        defaultFace.setAU(5, "both", "fac_5_upper_lid_raiser")
        defaultFace.setAU(6, "both", "fac_6_cheek_raiser")
        defaultFace.setAU(7, "both", "fac_7_lid_tightener")
        defaultFace.setAU(9, "both", "fac_9_nose_wrinkler")
        defaultFace.setAU(10, "both", "fac_10_upper_lip_raiser")
        defaultFace.setAU(12, "both", "fac_12_lip_corner_puller")
        defaultFace.setAU(15, "both", "fac_15_lip_corner_depressor")
        defaultFace.setAU(20, "both", "fac_20_lip_stretcher")
        defaultFace.setAU(23, "both", "fac_23_lip_tightener")
        defaultFace.setAU(25, "both", "fac_25_lips_part")
        defaultFace.setAU(26, "both", "fac_26_jaw_drop")
        defaultFace.setAU(27, "both", "fac_27_mouth_stretch")
        defaultFace.setAU(38, "both", "fac_38_nostril_dilator")
        defaultFace.setAU(39, "both", "fac_39_nostril_compressor")
        defaultFace.setAU(45, "left", "fac_45L_blink")
        defaultFace.setAU(45, "right", "fac_45R_blink")

        defaultFace.setViseme("Ao", "viseme_ao")
        defaultFace.setViseme("D", "viseme_d")
        defaultFace.setViseme("EE", "viseme_ee")
        defaultFace.setViseme("Er", "viseme_er")
        defaultFace.setViseme("f", "viseme_f")
        defaultFace.setViseme("j", "viseme_j")
        defaultFace.setViseme("KG", "viseme_kg")
        defaultFace.setViseme("Ih", "viseme_ih")
        defaultFace.setViseme("NG", "viseme_ng")
        defaultFace.setViseme("oh", "viseme_oh")
        defaultFace.setViseme("OO", "viseme_oo")
        defaultFace.setViseme("R", "viseme_r")
        defaultFace.setViseme("Th", "viseme_th")
        defaultFace.setViseme("Z", "viseme_z")
        defaultFace.setViseme("BMP", "viseme_bmp")
        defaultFace.setViseme("blink", "fac_45_blink")

        # mapping for new lip sync
        defaultFace.setViseme("FV", "viseme_f")
        defaultFace.setViseme("open", "viseme_kg")
        defaultFace.setViseme("PBM", "viseme_bmp")
        defaultFace.setViseme("ShCh", "viseme_er")
        defaultFace.setViseme("W", "viseme_oo")
        defaultFace.setViseme("Wide", "viseme_ee")
    # defaultFace.setViseme("tBack", "")
    # defaultFace.setViseme("tRoof", "")
    # defaultFace.setViseme("tTeeth", "")
    else:
        defaultFace = scene.getFaceDefinition("_default_")

    sbChar = scene.getCharacter(charName)
    if sbChar is not None:
        sbChar.setFaceDefinition(defaultFace)


def characterSetup(charName):
    print("Setting up character " + charName)
    setupFace(charName)

    sbChar = scene.getCharacter(charName)

    # locomotion
    # scene.run('BehaviorSetMaleMocapLocomotion.py')
    # setupBehaviorSet()
    # retargetBehaviorSet(charName)

    # reaching
    # scene.run('BehaviorSetMocapReaching.py')
    # setupBehaviorSet()
    # retargetBehaviorSet(charName)

    # jumping
    # scene.run('BehaviorSetJumping.py')
    # setupBehaviorSet()
    # retargetBehaviorSet(charName)

    # gesture
    scene.run('BehaviorSetGestures.py')
    setupBehaviorSet()
    retargetBehaviorSet(charName)


# punching
# scene.run('BehaviorSetPunching.py')
# setupBehaviorSet()
# retargetBehaviorSet(charName)

# kicking
# scene.run('BehaviorSetKicking.py')
# setupBehaviorSet()
# retargetBehaviorSet(charName)


scene.run("default-viewer.py")

### Load data/sbm-common assets
scene.addAssetPath("seq", "sbm-common/scripts")
scene.addAssetPath("seq", "behaviorsets")

scene.run("init-common-assets.py")
scene.run("init-common-face.py")

scene.run("init-param-animation.py")

scene.run("init-steer-agents.py")
scene.run("init-example-reach.py")

scene.setBoolAttribute("internalAudio", True)

scene.vhmsg("vrAllCall")

print("Finished default-init-empty.py for Panda")
