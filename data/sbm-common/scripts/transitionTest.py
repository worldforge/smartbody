stateManager = scene.getStateManager()

transition = stateManager.createTransition("allCrouchSwayBlock", "allCrouchProtectHead")
transition.addCorrespondancePoint("ChrUtah_CrouchBlock02High_swayLf", "ChrUtah_Relax001_CrouchProtectHead_center", 0, 1, .5, 1.6)
