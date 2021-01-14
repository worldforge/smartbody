gMapManager = scene.getGestureMapManager()
numCharacters = scene.getNumCharacters()
charNames = scene.getCharacterNames()

# to be finished...
for i in range(0, numCharacters):
    gMap = gMapManager.createGestureMap(charNames[i])
    gMap.addGestureMapping("HandsAtSide_Arms_Sweep", "", "SWEEP", "HandsAtSide_Motex", "BOTH_HANDS", "")
    gMap.addGestureMapping("HandsAtSide_RArm_GestureYou", "", "YOU", "HandsAtSide_Motex", "RIGHT_HAND", "")
    gMap.addGestureMapping("LHandOnHip_Arms_GestureWhy", "", "WHY", "LHandOnHip_Motex", "BOTH_HANDS", "")
    gMap.addGestureMapping("LHandOnHip_RArm_GestureOffer", "", "OFFER", "LHandOnHip_Motex", "RIGHT_HAND", "")
    gMap.addGestureMapping("LHandOnHip_RArm_SweepRight", "", "SWEEP", "LHandOnHip_Motex", "RIGHT_HAND", "")
    gMap.addGestureMapping("LHandOnHip_RArm_SweepRight", "", "YOU", "ChrUtah_Idle003", "RIGHT_HAND", "")
