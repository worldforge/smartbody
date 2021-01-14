print("|--------------------------------------------|")
print("|  data/sbm-common/scripts/bradrachel.py  |")
print("|--------------------------------------------|")
scene.run("motion-retarget.py")
### Load data/sbm-common assets
scene.addAssetPath("script", "sbm-common/scripts")
scene.addAssetPath("mesh", "mesh")
scene.addAssetPath("audio", ".")

scene.setScale(1.0)
scene.setBoolAttribute("internalAudio", True)

scene.run("default-viewer.py")

cameraPawn = scene.getPawn("camera")
cameraPos = SrVec(0, 1.6, 10)
cameraPawn.setPosition(cameraPos)

camera = getCamera()
camera.setEye(-0.35, 1.63, 2.3)
camera.setCenter(0.012, 1.18, 0.40)
camera.setUpVector(SrVec(0, 1, 0))
camera.setScale(1)
camera.setFov(1.0472)
camera.setFarPlane(100)
camera.setNearPlane(0.1)
camera.setAspectRatio(0.966897)

# map to the SmartBody standard
scene.run("zebra2-map.py")
zebra2Map = scene.getJointMapManager().getJointMap("zebra2")

scene.addAssetPath("motion", "ChrRachel")
scene.addAssetPath("motion", "ChrBrad")
scene.addAssetPath("motion", "retarget/motion")
scene.addAssetPath("motion", "sbm-common/common-sk")
scene.loadAssets()

# established lip syncing data set
scene.run("init-diphoneDefault.py")

# param animation setup
scene.run("init-param-animation.py")

# established gesture data set

gMapManager = scene.getGestureMapManager()
# Brad's gestures
gMap = gMapManager.createGestureMap("ChrBrad")
gMap.addGestureMapping("ChrBrad@Idle01_YouLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle02_YouLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle03_YouLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle01_MeLf01", "DEICTIC", "ME", "LEFT_HAND", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle02_MeRt01", "DEICTIC", "ME", "RIGHT_HAND", "", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle03_MeLf01", "DEICTIC", "ME", "LEFT_HAND", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle03_Me", "DEICTIC", "ME", "BOTH_HANDS", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle01_IndicateLeftLf01", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle01_IndicateLeftBt01", "DEICTIC", "LEFT", "BOTH_HANDS", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle01_IndicateRightRt01", "DEICTIC", "RIGHT", "RIGHT_HAND", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle01_IndicateRightBt01", "DEICTIC", "RIGHT", "BOTH_HANDS", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle01_NegativeBt01", "METAPHORIC", "NEGATION", "BOTH_HANDS", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle01_NegativeRt01", "METAPHORIC", "NEGATION", "RIGHT_HAND", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle02_NegativeBt01", "METAPHORIC", "NEGATION", "BOTH_HANDS", "", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle02_NegativeLf01", "METAPHORIC", "NEGATION", "LEFT_HAND", "", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle03_NegativeBt01", "METAPHORIC", "NEGATION", "BOTH_HANDS", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle03_NegativeLf01", "METAPHORIC", "NEGATION", "LEFT_HAND", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle01_HoweverLf01", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle01_Shrug01", "METAPHORIC", "CONTRAST", "BOTH_HANDS", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle02_Shrug01", "METAPHORIC", "CONTRAST", "BOTH_HANDS", "", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle03_Shrug01", "METAPHORIC", "CONTRAST", "BOTH_HANDS", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle01_PleaBt02", "METAPHORIC", "ASSUMPTION", "BOTH_HANDS", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle02_PleaBt01", "METAPHORIC", "ASSUMPTION", "BOTH_HANDS", "open", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle02_PleaBt02", "METAPHORIC", "ASSUMPTION", "BOTH_HANDS", "tight", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle03_PleaBt01", "METAPHORIC", "ASSUMPTION", "BOTH_HANDS", "open", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle03_PleaBt02", "METAPHORIC", "ASSUMPTION", "BOTH_HANDS", "tight", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "METAPHORIC", "RHETORICAL", "LEFT_HAND", "noheadtilt", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "RHETORICAL", "LEFT_HAND", "headtilt", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "METAPHORIC", "RHETORICAL", "RIGHT_HAND", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle02_BeatLowLf01", "METAPHORIC", "RHETORICAL", "LEFT_HAND", "", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle02_BeatLowRt01", "METAPHORIC", "RHETORICAL", "RIGHT_HAND", "", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle03_BeatLowLf01", "METAPHORIC", "RHETORICAL", "LEFT_HAND", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle03_BeatLowRt01", "METAPHORIC", "RHETORICAL", "RIGHT_HAND", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle01_BeatMidBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "mid", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle02_BeatMidBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "mid", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle02_BeatLowBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle03_BeatMidBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "mid", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle03_BeatLowBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "METAPHORIC", "QUESTION", "LEFT_HAND", "noheadtilt", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "QUESTION", "LEFT_HAND", "headtilt", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "METAPHORIC", "QUESTION", "RIGHT_HAND", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle02_BeatLowLf01", "METAPHORIC", "QUESTION", "LEFT_HAND", "", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle02_BeatLowRt01", "METAPHORIC", "QUESTION", "RIGHT_HAND", "", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle03_BeatLowLf01", "METAPHORIC", "QUESTION", "LEFT_HAND", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle03_BeatLowRt01", "METAPHORIC", "QUESTION", "RIGHT_HAND", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle01_ChopLf01", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle01_ChopBt01", "METAPHORIC", "OBLIGATION", "BOTH_HANDS", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle02_ChopLf01", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle02_ChopBt01", "METAPHORIC", "OBLIGATION", "BOTH_HANDS", "", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle03_ChopLf01", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle03_ChopBt01", "METAPHORIC", "OBLIGATION", "BOTH_HANDS", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "EMBLEM", "GREETING", "LEFT_HAND", "", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle02_BeatLowLf01", "EMBLEM", "GREETING", "LEFT_HAND", "", "ChrBrad@Idle02")
gMap.addGestureMapping("ChrBrad@Idle03_BeatLowLf01", "EMBLEM", "GREETING", "LEFT_HAND", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle01_Contemplate01", "ADAPTOR", "CONTEMPLATE", "BOTH_HANDS", "mid", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle01_Think01", "ADAPTOR", "CONTEMPLATE", "BOTH_HANDS", "low", "ChrBrad@Idle01")
gMap.addGestureMapping("ChrBrad@Idle03_DeicticLeft", "DEICTIC", "LEFT", "BOTH_HANDS", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle03_DeicticRight", "DEICTIC", "RIGHT", "RIGHT_HAND", "", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle03_EmptyLarge", "METAPHORIC", "EMPTY", "BOTH_HANDS", "large", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle03_EmptyMedium", "METAPHORIC", "EMPTY", "BOTH_HANDS", "medium", "ChrBrad@Idle03")
gMap.addGestureMapping("ChrBrad@Idle03_Frame", "METAPHORIC", "FRAME", "BOTH_HANDS", "", "ChrBrad@Idle03")

# Rachel's gestures
gMap1 = gMapManager.createGestureMap("ChrRachel")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_YouLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_YouLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_YouLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_MeLf01", "DEICTIC", "ME", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_MeRt01", "DEICTIC", "ME", "RIGHT_HAND", "", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_MeLf01", "DEICTIC", "ME", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_IndicateLeftLf01", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_IndicateLeftBt01", "DEICTIC", "LEFT", "BOTH_HANDS", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_IndicateRightRt01", "DEICTIC", "RIGHT", "RIGHT_HAND", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_IndicateRightBt01", "DEICTIC", "RIGHT", "BOTH_HANDS", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_NegativeBt01", "METAPHORIC", "NEGATION", "BOTH_HANDS", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_NegativeRt01", "METAPHORIC", "NEGATION", "RIGHT_HAND", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_NegativeBt01", "METAPHORIC", "NEGATION", "BOTH_HANDS", "", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_NegativeLf01", "METAPHORIC", "NEGATION", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_NegativeBt01", "METAPHORIC", "NEGATION", "BOTH_HANDS", "", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_NegativeLf01", "METAPHORIC", "NEGATION", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_HoweverLf01", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_Shrug01", "METAPHORIC", "CONTRAST", "BOTH_HANDS", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_Shrug01", "METAPHORIC", "CONTRAST", "BOTH_HANDS", "", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_Shrug01", "METAPHORIC", "CONTRAST", "BOTH_HANDS", "", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_PleaBt02", "METAPHORIC", "ASSUMPTION", "BOTH_HANDS", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_PleaBt01", "METAPHORIC", "ASSUMPTION", "BOTH_HANDS", "open", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_PleaBt02", "METAPHORIC", "ASSUMPTION", "BOTH_HANDS", "tight", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_PleaBt01", "METAPHORIC", "ASSUMPTION", "BOTH_HANDS", "open", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_PleaBt02", "METAPHORIC", "ASSUMPTION", "BOTH_HANDS", "tight", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_BeatLowLf01", "METAPHORIC", "RHETORICAL", "LEFT_HAND", "noheadtilt", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "RHETORICAL", "LEFT_HAND", "headtilt", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_BeatLowRt01", "METAPHORIC", "RHETORICAL", "RIGHT_HAND", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_BeatLowLf01", "METAPHORIC", "RHETORICAL", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_BeatLowRt01", "METAPHORIC", "RHETORICAL", "RIGHT_HAND", "", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_BeatLowLf01", "METAPHORIC", "RHETORICAL", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_BeatLowRt01", "METAPHORIC", "RHETORICAL", "RIGHT_HAND", "", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_BeatMidBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "mid", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_BeatLowBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_BeatMidBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "mid", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_BeatLowBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_BeatMidBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "mid", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_BeatLowBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_BeatLowLf01", "METAPHORIC", "QUESTION", "LEFT_HAND", "noheadtilt", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "QUESTION", "LEFT_HAND", "headtilt", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_BeatLowRt01", "METAPHORIC", "QUESTION", "RIGHT_HAND", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_BeatLowLf01", "METAPHORIC", "QUESTION", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_BeatLowRt01", "METAPHORIC", "QUESTION", "RIGHT_HAND", "", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_BeatLowLf01", "METAPHORIC", "QUESTION", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_BeatLowRt01", "METAPHORIC", "QUESTION", "RIGHT_HAND", "", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_ChopLf01", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_ChopBt01", "METAPHORIC", "OBLIGATION", "BOTH_HANDS", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_ChopLf01", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle02_ChopBt01", "METAPHORIC", "OBLIGATION", "BOTH_HANDS", "", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_ChopLf01", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle03_ChopBt01", "METAPHORIC", "OBLIGATION", "BOTH_HANDS", "", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_BeatLowLf01", "EMBLEM", "GREETING", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_BeatLowLf02", "EMBLEM", "GREETING", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle02")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_BeatLowLf03", "EMBLEM", "GREETING", "LEFT_HAND", "", "ChrRachel_ChrBrad@Idle03")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_Contemplate01", "ADAPTOR", "CONTEMPLATE", "BOTH_HANDS", "mid", "ChrRachel_ChrBrad@Idle01")
gMap1.addGestureMapping("ChrRachel_ChrBrad@Idle01_Think01", "ADAPTOR", "CONTEMPLATE", "BOTH_HANDS", "low", "ChrRachel_ChrBrad@Idle01")

############################# Brad
bradSkeleton = scene.getSkeleton("ChrBrad.sk")
zebra2Map.applySkeleton(bradSkeleton)
zebra2Map.applyMotionRecurse("ChrBrad")

# Brad's face definition
bradFace = scene.createFaceDefinition("ChrBrad")
bradFace.setFaceNeutral("ChrBrad@face_neutral")

bradFace.setAU(1, "left", "ChrBrad@001_inner_brow_raiser_lf")
bradFace.setAU(1, "right", "ChrBrad@001_inner_brow_raiser_rt")
bradFace.setAU(2, "left", "ChrBrad@002_outer_brow_raiser_lf")
bradFace.setAU(2, "right", "ChrBrad@002_outer_brow_raiser_rt")
bradFace.setAU(4, "left", "ChrBrad@004_brow_lowerer_lf")
bradFace.setAU(4, "right", "ChrBrad@004_brow_lowerer_rt")
bradFace.setAU(5, "both", "ChrBrad@005_upper_lid_raiser")
bradFace.setAU(6, "both", "ChrBrad@006_cheek_raiser")
bradFace.setAU(7, "both", "ChrBrad@007_lid_tightener")
bradFace.setAU(10, "both", "ChrBrad@010_upper_lip_raiser")
bradFace.setAU(12, "left", "ChrBrad@012_lip_corner_puller_lf")
bradFace.setAU(12, "right", "ChrBrad@012_lip_corner_puller_rt")
bradFace.setAU(25, "both", "ChrBrad@025_lips_part")
bradFace.setAU(26, "both", "ChrBrad@026_jaw_drop")
bradFace.setAU(45, "left", "ChrBrad@045_blink_lf")
bradFace.setAU(45, "right", "ChrBrad@045_blink_rt")

bradFace.setViseme("open", "ChrBrad@open")
bradFace.setViseme("W", "ChrBrad@W")
bradFace.setViseme("ShCh", "ChrBrad@ShCh")
bradFace.setViseme("PBM", "ChrBrad@PBM")
bradFace.setViseme("FV", "ChrBrad@FV")
bradFace.setViseme("wide", "ChrBrad@wide")
bradFace.setViseme("tBack", "ChrBrad@tBack")
bradFace.setViseme("tRoof", "ChrBrad@tRoof")
bradFace.setViseme("tTeeth", "ChrBrad@tTeeth")

brad = scene.createCharacter("ChrBrad", "")
bradSkeleton = scene.createSkeleton("ChrBrad.sk")
brad.setSkeleton(bradSkeleton)
brad.setFaceDefinition(bradFace)
bradPos = SrVec(.35, 0, 0)
brad.setPosition(bradPos)
bradHPR = SrVec(-17, 0, 0)
brad.setHPR(bradHPR)
brad.createStandardControllers()
# deformable mesh
brad.setDoubleAttribute("deformableMeshScale", .01)
brad.setStringAttribute("deformableMesh", "ChrBrad.dae")
# lip syncing diphone setup
brad.setStringAttribute("lipSyncSetName", "default")
brad.setBoolAttribute("usePhoneBigram", True)
brad.setVoice("remote")
brad.setVoiceCode("Festival_voice_kal_diphone")
# gesture map setup
brad.setStringAttribute("gestureMap", "ChrBrad")
brad.setBoolAttribute("gestureRequest.autoGestureTransition", True)
# retarget
retargetCharacter("ChrBrad", "ChrBrad.sk")

############################# Rachel
rachelSkeleton = scene.getSkeleton("ChrRachel.sk")
zebra2Map.applySkeleton(rachelSkeleton)
zebra2Map.applyMotionRecurse("ChrRachel")

# Rachel's face definition
rachelFace = scene.createFaceDefinition("ChrRachel")
rachelFace.setFaceNeutral("ChrRachel@face_neutral")

rachelFace.setAU(1, "left", "ChrRachel@001_inner_brow_raiser_lf")
rachelFace.setAU(1, "right", "ChrRachel@001_inner_brow_raiser_rt")
rachelFace.setAU(2, "left", "ChrRachel@002_outer_brow_raiser_lf")
rachelFace.setAU(2, "right", "ChrRachel@002_outer_brow_raiser_rt")
rachelFace.setAU(4, "left", "ChrRachel@004_brow_lowerer_lf")
rachelFace.setAU(4, "right", "ChrRachel@004_brow_lowerer_rt")
rachelFace.setAU(5, "both", "ChrRachel@005_upper_lid_raiser")
rachelFace.setAU(6, "both", "ChrRachel@006_cheek_raiser")
rachelFace.setAU(7, "both", "ChrRachel@007_lid_tightener")
rachelFace.setAU(10, "both", "ChrRachel@010_upper_lip_raiser")
rachelFace.setAU(12, "left", "ChrRachel@012_lip_corner_puller_lf")
rachelFace.setAU(12, "right", "ChrRachel@012_lip_corner_puller_rt")
rachelFace.setAU(25, "both", "ChrRachel@025_lips_part")
rachelFace.setAU(26, "both", "ChrRachel@026_jaw_drop")
rachelFace.setAU(45, "left", "ChrRachel@045_blink_lf")
rachelFace.setAU(45, "right", "ChrRachel@045_blink_rt")

rachelFace.setViseme("open", "ChrRachel@open")
rachelFace.setViseme("W", "ChrRachel@W")
rachelFace.setViseme("ShCh", "ChrRachel@ShCh")
rachelFace.setViseme("PBM", "ChrRachel@PBM")
rachelFace.setViseme("FV", "ChrRachel@FV")
rachelFace.setViseme("wide", "ChrRachel@wide")
rachelFace.setViseme("tBack", "ChrRachel@tBack")
rachelFace.setViseme("tRoof", "ChrRachel@tRoof")
rachelFace.setViseme("tTeeth", "ChrRachel@tTeeth")

rachel = scene.createCharacter("ChrRachel", "")
rachelSkeleton = scene.createSkeleton("ChrRachel.sk")
rachel.setSkeleton(rachelSkeleton)
rachel.setFaceDefinition(rachelFace)
rachelPos = SrVec(-.35, 0, 0)
rachel.setPosition(rachelPos)
rachelHPR = SrVec(17, 0, 0)
rachel.setHPR(rachelHPR)
rachel.createStandardControllers()
# deformable mesh
rachel.setDoubleAttribute("deformableMeshScale", .01)
rachel.setStringAttribute("deformableMesh", "ChrRachel.dae")
# lip syncing diphone setup
rachel.setStringAttribute("lipSyncSetName", "default")
rachel.setBoolAttribute("usePhoneBigram", True)
rachel.setVoice("remote")
rachel.setVoiceCode("MicrosoftAnna")
# rachel.setVoice("audiofile")
# rachel.setVoiceCode("Sounds2")
# gesture map setup
rachel.setStringAttribute("gestureMap", "ChrRachel")
rachel.setBoolAttribute("gestureRequest.autoGestureTransition", True)
# retarget
retargetCharacter("ChrRachel", "ChrRachel.sk")

############################# Rachel
rachel2Skeleton = scene.getSkeleton("ChrRachel.sk")
zebra2Map.applySkeleton(rachel2Skeleton)
zebra2Map.applyMotionRecurse("ChrRachel")

# Rachel2's face definition
rachel2Face = scene.createFaceDefinition("ChrRachel2")
rachel2Face.setFaceNeutral("ChrEllieDcaps@face_neutral")

rachel2Face.setAU(1, "left", "ChrRachel@001_inner_brow_raiser_lf")
rachel2Face.setAU(1, "right", "ChrRachel@001_inner_brow_raiser_rt")
rachel2Face.setAU(2, "left", "ChrRachel@002_outer_brow_raiser_lf")
rachel2Face.setAU(2, "right", "ChrRachel@002_outer_brow_raiser_rt")
rachel2Face.setAU(4, "left", "ChrRachel@004_brow_lowerer_lf")
rachel2Face.setAU(4, "right", "ChrRachel@004_brow_lowerer_rt")
rachel2Face.setAU(5, "both", "ChrRachel@005_upper_lid_raiser")
rachel2Face.setAU(6, "both", "ChrRachel@006_cheek_raiser")
rachel2Face.setAU(7, "both", "ChrRachel@007_lid_tightener")
rachel2Face.setAU(10, "both", "ChrRachel@010_upper_lip_raiser")
rachel2Face.setAU(12, "left", "ChrRachel@012_lip_corner_puller_lf")
rachel2Face.setAU(12, "right", "ChrRachel@012_lip_corner_puller_rt")
rachel2Face.setAU(25, "both", "ChrRachel@025_lips_part")
rachel2Face.setAU(26, "both", "ChrRachel@026_jaw_drop")
# rachel2Face.setAU(45, "left",  "ChrRachel@045_blink_lf")
# rachel2Face.setAU(45, "right", "ChrRachel@045_blink_rt")

rachel2Face.setViseme("open", "ChrEllieDcaps@open")
rachel2Face.setViseme("W", "ChrEllieDcaps@W")
rachel2Face.setViseme("ShCh", "ChrEllieDcaps@ShCh")
rachel2Face.setViseme("PBM", "ChrEllieDcaps@PBM")
rachel2Face.setViseme("FV", "ChrEllieDcaps@FV")
rachel2Face.setViseme("wide", "ChrEllieDcaps@wide")
rachel2Face.setViseme("tBack", "ChrEllieDcaps@tBack")
rachel2Face.setViseme("tRoof", "ChrEllieDcaps@tRoof")
rachel2Face.setViseme("tTeeth", "ChrEllieDcaps@tTeeth")

rachel2 = scene.createCharacter("ChrRachel2", "")
rachel2Skeleton = scene.createSkeleton("ChrRachel.sk")
rachel2.setSkeleton(rachel2Skeleton)
rachel2.setFaceDefinition(rachel2Face)
rachel2Pos = SrVec(-1, 0, 0)
rachel2.setPosition(rachel2Pos)
rachel2HPR = SrVec(17, 0, 0)
rachel2.setHPR(rachel2HPR)
rachel2.createStandardControllers()
# deformable mesh
rachel2.setDoubleAttribute("deformableMeshScale", .01)
rachel2.setStringAttribute("deformableMesh", "ChrRachel.dae")
# lip syncing diphone setup
rachel2.setStringAttribute("diphoneSetName", "default")
rachel2.setBoolAttribute("useDiphone", True)
rachel2.setVoice("remote")
rachel2.setVoiceCode("MicrosoftAnna")
# rachel2.setVoice("audiofile")
# rachel2.setVoiceCode("Sounds2")
# gesture map setup
rachel2.setStringAttribute("gestureMap", "ChrRachel")
rachel2.setBoolAttribute("gestureRequest.autoGestureTransition", True)
# retarget
retargetCharacter("ChrRachel", "ChrRachel.sk")

scene.setDefaultCharacter("ChrBrad")
scene.setDefaultRecipient("ChrRachel")

# steering
scene.run("init-steer-agents.py")
setupSteerAgent("ChrRachel", "ChrRachel.sk")
setupSteerAgent("ChrBrad", "ChrBrad.sk")
steerManager.setEnable(False)
brad.setBoolAttribute("steering.pathFollowingMode", True)
rachel.setBoolAttribute("steering.pathFollowingMode", True)

brad.setStringAttribute("displayType", "GPUmesh")
rachel.setStringAttribute("displayType", "GPUmesh")
rachel2.setStringAttribute("displayType", "GPUmesh")

# scene.run("runNVBG.py")
# start the simulation
sim.start()

bml.execBML('ChrBrad', '<body posture="ChrBrad@Idle01"/>')
bml.execBML('ChrRachel', '<body posture="ChrRachel_ChrBrad@Idle01" start="1"/>')
bml.execBML('ChrRachel2', '<body posture="ChrRachel_ChrBrad@Idle01" start="1"/>')
bml.execBML('ChrBrad', '<saccade mode="listen"/>')
bml.execBML('ChrRachel', '<saccade mode="listen"/>')
bml.execBML('ChrRachel2', '<saccade mode="listen"/>')

bml.execBML('ChrBrad', '<gaze sbm:handle="brad" target="camera"/>')
bml.execBML('ChrRachel', '<gaze sbm:handle="rachel" target="camera"/>')
bml.execBML('ChrRachel2', '<gaze sbm:handle="rachel" target="camera"/>')

sim.resume()

steerManager.setEnable(True)
