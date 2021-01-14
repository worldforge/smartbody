scene.run("BehaviorSetCommon.py")


def setupBehaviorSet():
    print("Setting up behavior set for gestures...")
    scene.addAssetPath("script", "behaviorsets/Gestures2/scripts")

    assetManager = scene.getAssetManager()
    assetStore = scene.getAssetStore()
    #	motionPath = "behaviorsets/Gestures2/motions/"
    motionPath = "behaviorsets/Gestures2/motionsBinary/"
    skel = scene.getSkeleton("ChrBrad2.sk")
    if skel == None:
        scene.loadAssetsFromPath("behaviorsets/Gestures2/skeletons")
    # map the zebra2 skeleton
    scene.run("zebra2-map.py")
    zebra2Map = scene.getJointMapManager().getJointMap("zebra2")
    bradSkeleton = scene.getSkeleton("ChrBrad2.sk")
    zebra2Map.applySkeleton(bradSkeleton)

    gestureMotions = StringVec()

    gestureMotions.append("ChrBrad@Idle01")
    gestureMotions.append("ChrBrad@Idle01_ArmStretch01")
    gestureMotions.append("ChrBrad@Idle01_BeatFistMidLf01")
    gestureMotions.append("ChrBrad@Idle01_BeatFistMidLf02")
    gestureMotions.append("ChrBrad@Idle01_BeatHighBt01")
    gestureMotions.append("ChrBrad@Idle01_BeatHighBt02")
    gestureMotions.append("ChrBrad@Idle01_BeatHighBt03")
    gestureMotions.append("ChrBrad@Idle01_BeatHighLf01")
    gestureMotions.append("ChrBrad@Idle01_BeatLowBt01")
    gestureMotions.append("ChrBrad@Idle01_BeatLowBt02")
    gestureMotions.append("ChrBrad@Idle01_BeatLowLf01")
    gestureMotions.append("ChrBrad@Idle01_BeatLowLf02")
    gestureMotions.append("ChrBrad@Idle01_BeatLowRt01")
    gestureMotions.append("ChrBrad@Idle01_BeatMidBt01")
    gestureMotions.append("ChrBrad@Idle01_BeatMidLf01")
    gestureMotions.append("ChrBrad@Idle01_BeatPointMidLf01")
    gestureMotions.append("ChrBrad@Idle01_BesidesLf01")
    gestureMotions.append("ChrBrad@Idle01_ChopBoth01")
    gestureMotions.append("ChrBrad@Idle01_ChopLf01")
    gestureMotions.append("ChrBrad@Idle01_ChopLf02")
    gestureMotions.append("ChrBrad@Idle01_ChopRt02")
    gestureMotions.append("ChrBrad@Idle01_Contemplate01")
    gestureMotions.append("ChrBrad@Idle01_ExampleLf01")
    gestureMotions.append("ChrBrad@Idle01_Guitar01")
    gestureMotions.append("ChrBrad@Idle01_HereBt01")
    gestureMotions.append("ChrBrad@Idle01_HereBt02")
    gestureMotions.append("ChrBrad@Idle01_HoweverLf01")
    gestureMotions.append("ChrBrad@Idle01_InclusivityNegBt01")
    gestureMotions.append("ChrBrad@Idle01_InclusivityNegBt02")
    gestureMotions.append("ChrBrad@Idle01_InclusivityPosBt01")
    gestureMotions.append("ChrBrad@Idle01_InclusivityPosBt02")
    gestureMotions.append("ChrBrad@Idle01_IndicateLeftBt01")
    gestureMotions.append("ChrBrad@Idle01_IndicateLeftLf01")
    gestureMotions.append("ChrBrad@Idle01_IndicateRightBt01")
    gestureMotions.append("ChrBrad@Idle01_IndicateRightRt01")
    gestureMotions.append("ChrBrad@Idle01_MeLf01")
    gestureMotions.append("ChrBrad@Idle01_MeLf02")
    gestureMotions.append("ChrBrad@Idle01_NegativeBt01")
    gestureMotions.append("ChrBrad@Idle01_NegativeBt02")
    gestureMotions.append("ChrBrad@Idle01_NegativeBt03")
    gestureMotions.append("ChrBrad@Idle01_NegativeRt01")
    gestureMotions.append("ChrBrad@Idle01_OfferBoth01")
    gestureMotions.append("ChrBrad@Idle01_OfferLf01")
    gestureMotions.append("ChrBrad@Idle01_PleaBt02")
    gestureMotions.append("ChrBrad@Idle01_PointLf01")
    gestureMotions.append("ChrBrad@Idle01_PointLf02")
    gestureMotions.append("ChrBrad@Idle01_PointRt01")
    gestureMotions.append("ChrBrad@Idle01_ReceiveRt01")
    gestureMotions.append("ChrBrad@Idle01_SafeLf01")
    gestureMotions.append("ChrBrad@Idle01_ScratchChest01")
    gestureMotions.append("ChrBrad@Idle01_ScratchHeadLf01")
    gestureMotions.append("ChrBrad@Idle01_ScratchTempleLf01")
    gestureMotions.append("ChrBrad@Idle01_ShoulderStretch01")
    gestureMotions.append("ChrBrad@Idle01_Shrug01")
    gestureMotions.append("ChrBrad@Idle01_Shrug02")
    gestureMotions.append("ChrBrad@Idle01_SqueezeHand01")
    gestureMotions.append("ChrBrad@Idle01_StopBt01")
    gestureMotions.append("ChrBrad@Idle01_StopBt02")
    gestureMotions.append("ChrBrad@Idle01_SurroundBt01")
    gestureMotions.append("ChrBrad@Idle01_SurroundBt02")
    gestureMotions.append("ChrBrad@Idle01_Think01")
    gestureMotions.append("ChrBrad@Idle01_ToIdle02")
    gestureMotions.append("ChrBrad@Idle01_ToIdle03")
    gestureMotions.append("ChrBrad@Idle01_ToLocIdle01")
    gestureMotions.append("ChrBrad@Idle01_TouchHands01")
    gestureMotions.append("ChrBrad@Idle01_ToWalk01_Turn180Rt01")
    gestureMotions.append("ChrBrad@Idle01_ToWalk01_Turn360Rt01")
    gestureMotions.append("ChrBrad@Idle01_ToWalk01_Turn90Rt01")
    gestureMotions.append("ChrBrad@Idle01_WeightShift01")
    gestureMotions.append("ChrBrad@Idle01_WeightShift02")
    gestureMotions.append("ChrBrad@Idle01_WeRt01")
    gestureMotions.append("ChrBrad@Idle01_WeRt02")
    gestureMotions.append("ChrBrad@Idle01_WhenBt01")
    gestureMotions.append("ChrBrad@Idle01_WhenBt02")
    gestureMotions.append("ChrBrad@Idle01_YouLf01")
    gestureMotions.append("ChrBrad@Idle01_YouLf02")
    gestureMotions.append("ChrBrad@Idle01_YouLf03")
    gestureMotions.append("ChrBrad@Idle01_YouPointLf01")
    gestureMotions.append("ChrBrad@Idle01_YouRt01")

    for i in range(0, len(gestureMotions)):
        motion = scene.getMotion(gestureMotions[i])
        if motion == None:
            # assetManager.loadAsset(motionPath+gestureMotions[i]+'.skm')
            assetStore.loadAsset(motionPath + gestureMotions[i] + '.skmb')
            motion = scene.getMotion(gestureMotions[i])
        # print 'motionName = ' + locoMotions[i]
        if motion != None:
            motion.setMotionSkeletonName("ChrBrad2.sk")
            zebra2Map.applyMotion(motion)

    print("| BradGestureMap.py |")

    '''
    # create sped-up versions of the gestures
    for s in range(1, len(gestureMotions)):
        m = scene.getMotion(gestureMotions[s])
        n = m.copy(gestureMotions[s] + "2x")
        n.speed(2)
        scene.getAssetManager().addMotion(n)
    '''

    # create a set of blends between the 1x and the 2x speed motions
    skeleton = "ChrBrad2.sk"
    for s in range(1, len(gestureMotions)):
        stateTest = scene.getBlendManager().createState1D(gestureMotions[s] + "Blend")
        stateTest.setBlendSkeleton(skeleton)
        stateTest.addMotion(gestureMotions[s], 0.0)
        stateTest.addMotion(gestureMotions[s] + "2x", 1.0)

    # establish gesture data set
    gMapManager = scene.getGestureMapManager()

    # Brad's gestures
    # neutral

    # add gesture blends
    skeleton = "ChrBrad2.sk"
    stateNames = []
    stateNames.append("BeathHighBt")
    stateNames.append("BeatFistMidLf")
    stateNames.append("BeatLowBt")
    stateNames.append("ChopLf")
    stateNames.append("HereBt")
    stateNames.append("InclusivityNegBt")
    stateNames.append("InclusivityPosBt")
    stateNames.append("MeLf")
    stateNames.append("YouLf")
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
    motionNames.append("ChrBrad@Idle01_YouLf01")
    motionNames.append("ChrBrad@Idle01_YouLf02")
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
        print(("Added gesture blend " + stateNames[b]))

    gMapB1 = gMapManager.createGestureMap("BradGesture")

    # gMapB1.addGestureMapping("ChrBrad@Idle01_ChopLf01", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChopLf", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_ChopRt02Blend", "METAPHORIC", "OBLIGATION", "RIGHT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_ChopBoth01Blend", "METAPHORIC", "OBLIGATION", "BOTH_HANDS", "", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "BEAT", "RHYTHM", "RIGHT_HAND", "", "ChrBrad@Idle01")
    # gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")

    # Begin STACY ADDING HERE
    # hands terrible
    # "ChrBrad@Idle01_BeatHighLf01"
    # "ChrBrad@Idle01_BeatLowBt01"
    # BeatLowLf01 and 02

    # Both hands

    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatHighBt01Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatHighBt02Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatHighBt03Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidBt01Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_ChopBoth01Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_Shrug02Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")

    # Left hand
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatFistMidLf01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatFistMidLf02Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidLf01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatPointMidLf01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BesidesLf01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_ChopLf01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_ChopLf02Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_PointLf02Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_SafeLf01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_YouLf02Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_YouPoint01Blend", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")

    # Right Hand
    gMapB1.addGestureMapping("ChrBrad@Idle01_ChopRt02Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_PointRt01Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_WeRt02Blend", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")

    # End STACY ADDING HERE

    gMapB1.addGestureMapping("BeatLowBt", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatHighLf01Blend", "DEICTIC", "LEFT", "LEFT_HAND", "high", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidLf01Blend", "DEICTIC", "LEFT", "LEFT_HAND", "high", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_HoweverLf01Blend", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_IndicateLeftLf01Blend", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowRt01Blend", "DEICTIC", "RIGHT", "RIGHT_HAND", "high", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_IndicateRightRt01Blend", "DEICTIC", "RIGHT", "RIGHT_HAND", "", "ChrBrad@Idle01")

    # gMapB1.addGestureMapping("ChrBrad@Idle01_HereBt02", "DEICTIC", "HERE", "BOTH_HANDS", "high", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("HereBt", "DEICTIC", "HERE", "BOTH_HANDS", "high", "ChrBrad@Idle01")
    # gMapB1.addGestureMapping("ChrBrad@Idle01_ChopBoth01", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowBt01Blend", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01")
    # gMapB1.addGestureMapping("ChrBrad@Idle01_Frame", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_NegativeBt02Blend", "METAPHORIC", "NEGATION", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_NegativeRt01Blend", "METAPHORIC", "NEGATION", "RIGHT_HAND", "", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_SafeLf01Blend", "METAPHORIC", "ASIDE", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BesidesLf01Blend", "METAPHORIC", "ASIDE", "LEFT_HAND", "", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidBt01Blend", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidLf01Blend", "METAPHORIC", "QUESTION", "LEFT_HAND", "noheadtilt", "ChrBrad@Idle01")

    # gMapB1.addGestureMapping("ChrBrad@Idle01_YouLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("YouLf", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_OfferBoth01Blend", "DEICTIC", "YOU", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_OfferLf01Blend", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_ReceiveRt01Blend", "DEICTIC", "YOU", "RIGHT_HAND", "", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowBt01Blend", "METAPHORIC", "INTENSIFIER_POSITIVE", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidBt01Blend", "METAPHORIC", "INTENSIFIER_POSITIVE", "BOTH_HANDS", "", "ChrBrad@Idle01")

    # gMapB1.addGestureMapping("ChrBrad@Idle01_MeLf01", "DEICTIC", "ME", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("MeLf", "DEICTIC", "ME", "LEFT_HAND", "", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_WeRt01Blend", "DEICTIC", "WE", "RIGHT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_WeRt02Blend", "DEICTIC", "WE", "RIGHT_HAND", "", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowLf01Blend", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowRt01Blend", "METAPHORIC", "CONTRAST", "RIGHT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowLf02Blend", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_IndicateRightR01Blend", "METAPHORIC", "CONTRAST", "RIGHT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_HoweverLf01Blend", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrBrad@Idle01")

    # gMapB1.addGestureMapping("ChrBrad@Idle01_BeatHighBt01", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01")
    # gMapB1.addGestureMapping("ChrBrad@Idle01_BeatHighBt02", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("BeathHighBt", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowBt01Blend", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidBt01Blend", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidLf01Blend", "METAPHORIC", "QUESTION", "LEFT_HAND", "noheadtilt", "ChrBrad@Idle01")

    # gMapB1.addGestureMapping("ChrBrad@Idle01_Shrug01", "METAPHORIC", "UNCERTAINTY", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("Shrug", "METAPHORIC", "UNCERTAINTY", "BOTH_HANDS", "", "ChrBrad@Idle01")

    # gMapB1.addGestureMapping("ChrBrad@Idle01_Shrug01", "METAPHORIC", "INDIFFERENCE", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("Shrug", "METAPHORIC", "INDIFFERENCE", "BOTH_HANDS", "", "ChrBrad@Idle01")

    # metaphoric inclusivity
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidBt01Blend", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "mid", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowBt01Blend", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrBrad@Idle01")
    # gMapB1.addGestureMapping("ChrBrad@Idle01_InclusivityPosBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("InclusivityPosBt", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrBrad@Idle01")

    # metaphoric quantity_nothing
    # gMapB1.addGestureMapping("ChrBrad@Idle01_NegativeBt01", "METAPHORIC", "EMPTY", "BOTH_HANDS", "high", "ChrBrad@Idle01")
    # gMapB1.addGestureMapping("ChrBrad@Idle01_NegativeBt02", "METAPHORIC", "EMPTY", "BOTH_HANDS", "high", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("NegativeBt", "METAPHORIC", "EMPTY", "BOTH_HANDS", "high", "ChrBrad@Idle01")

    gMapB1.addGestureMapping("ChrBrad@Idle01_NegativeRt01Blend", "METAPHORIC", "EMPTY", "RIGHT_HAND", "high", "ChrBrad@Idle01")

    # gMapB1.addGestureMapping("ChrBrad@Idle01_SurroundBt01", "METAPHORIC", "SURROUND", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("SurroundBt", "METAPHORIC", "SURROUND", "BOTH_HANDS", "", "ChrBrad@Idle01")

    # gMapB1.addGestureMapping("ChrBrad@Idle01_StopBt01", "METAPHORIC", "STOP", "RIGHT_HAND", "", "ChrBrad@Idle02")
    gMapB1.addGestureMapping("StopBt", "METAPHORIC", "STOP", "RIGHT_HAND", "", "ChrBrad@Idle02")

    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowLf01Blend", "EMBLEM", "GREETING", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowRt01Blend", "EMBLEM", "GREETING", "RIGHT_HAND", "", "ChrBrad@Idle01")

    # #metaphoric "UNCERTAINTY" to add (low energy shrug)
    # #metaphoric "INDIFFERENCE" to add (low energy shrug)
    # #add gesture "EMPTY"
    # #emphatic
    gMapB2 = gMapManager.createGestureMap("BradGestureAngry")

    gMapB2.addGestureMapping("ChrBrad@Idle01_YouLf02", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_YouPointLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_MeLf02", "DEICTIC", "ME", "LEFT_HAND", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_WeRt02", "DEICTIC", "WE", "RIGHT_HAND", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_BeatMidBt01", "DEICTIC", "WE", "BOTH_HANDS", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_PointLf02", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_BeatHighLf01", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_PointRt01", "DEICTIC", "RIGHT", "RIGHT_HAND", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "DEICTIC", "CENTER", "BOTH_HANDS", "", "ChrBrad@Idle01")
    # gMapB2.addGestureMapping("ChrBrad@Idle01_HereBt02", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01")
    # gMapB2.addGestureMapping("ChrBrad@Idle01_HereBt02", "DEICTIC", "CENTER", "BOTH_HANDS", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_InclusivityNegBt02", "METAPHORIC", "NEGATION", "BOTH_HANDS", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_BeatHighBt03", "METAPHORIC", "INTENSIFIER_POSITIVE", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_BeatHighBt02", "METAPHORIC", "INTENSIFIER_POSITIVE", "BOTH_HANDS", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_PointRt01", "METAPHORIC", "CONTRAST", "RIGHT_HAND", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_PointLf02", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_BeatHighBt03", "METAPHORIC", "QUESTION", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_PointRt01", "METAPHORIC", "QUESTION", "RIGHT_HAND", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_PointLf02", "METAPHORIC", "QUESTION", "LEFT_HAND", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_Shrug02", "METAPHORIC", "UNCERTAINTY", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_BeatHighBt03", "METAPHORIC", "UNCERTAINTY", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_Shrug02", "METAPHORIC", "INDIFFERENCE", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_BeatHighBt03", "METAPHORIC", "INDIFFERENCE", "BOTH_HANDS", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_ChopLf02", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_InclusivityNegBt02", "METAPHORIC", "EMPTY", "BOTH_HANDS", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_BesidesLf01", "METAPHORIC", "ASIDE", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_BesidesLf01", "METAPHORIC", "SURROUND", "BOTH_HANDS", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_InclusivityNegBt02", "METAPHORIC", "STOP", "BOTH_HANDS", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_ChopLf02", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_ChopRt02", "METAPHORIC", "OBLIGATION", "RIGHT_HAND", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_BeatLowBt02", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_BeatPointMidLf01", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_BeatFistMidLf01", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_BeatFistMidLf02", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")

    gMapB2.addGestureMapping("ChrBrad@Idle01_ChopLf02", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB2.addGestureMapping("ChrBrad@Idle01_YouPointLf01", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")

    # sad
    gMapB3 = gMapManager.createGestureMap("BradGestureSad")
    gMapB3.addGestureMapping("ChrBrad@Idle01_YouLf03", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "DEICTIC", "YOU", "RIGHT_HAND", "", "ChrBrad@Idle01")

    gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "DEICTIC", "RIGHT", "RIGHT_HAND", "", "ChrBrad@Idle01")

    gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "METAPHORIC", "NEGATION", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "METAPHORIC", "NEGATION", "RIGHT_HAND", "", "ChrBrad@Idle01")

    gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "QUESTION", "LEFT_HAND", "", "ChrBrad@Idle01")

    gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "INDIFFERENCE", "LEFT_HAND", "", "ChrBrad@Idle01")
    gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "UNCERTAINTY", "LEFT_HAND", "", "ChrBrad@Idle01")


def retargetBehaviorSet(charName):
    gestureMotions = StringVec()
    gestureMotions.append("ChrBrad@Idle01")
    gestureMotions.append("ChrBrad@Idle01_ArmStretch01")
    gestureMotions.append("ChrBrad@Idle01_BeatFistMidLf01")
    gestureMotions.append("ChrBrad@Idle01_BeatFistMidLf02")
    gestureMotions.append("ChrBrad@Idle01_BeatHighBt01")
    gestureMotions.append("ChrBrad@Idle01_BeatHighBt02")
    gestureMotions.append("ChrBrad@Idle01_BeatHighBt03")
    gestureMotions.append("ChrBrad@Idle01_BeatHighLf01")
    gestureMotions.append("ChrBrad@Idle01_BeatLowBt01")
    gestureMotions.append("ChrBrad@Idle01_BeatLowBt02")
    gestureMotions.append("ChrBrad@Idle01_BeatLowLf01")
    gestureMotions.append("ChrBrad@Idle01_BeatLowLf02")
    gestureMotions.append("ChrBrad@Idle01_BeatLowRt01")
    gestureMotions.append("ChrBrad@Idle01_BeatMidBt01")
    gestureMotions.append("ChrBrad@Idle01_BeatMidLf01")
    gestureMotions.append("ChrBrad@Idle01_BeatPointMidLf01")
    gestureMotions.append("ChrBrad@Idle01_BesidesLf01")
    gestureMotions.append("ChrBrad@Idle01_ChopBoth01")
    gestureMotions.append("ChrBrad@Idle01_ChopLf01")
    gestureMotions.append("ChrBrad@Idle01_ChopLf02")
    gestureMotions.append("ChrBrad@Idle01_ChopRt02")
    gestureMotions.append("ChrBrad@Idle01_Contemplate01")
    gestureMotions.append("ChrBrad@Idle01_ExampleLf01")
    gestureMotions.append("ChrBrad@Idle01_Guitar01")
    gestureMotions.append("ChrBrad@Idle01_HereBt01")
    gestureMotions.append("ChrBrad@Idle01_HereBt02")
    gestureMotions.append("ChrBrad@Idle01_HoweverLf01")
    gestureMotions.append("ChrBrad@Idle01_InclusivityNegBt01")
    gestureMotions.append("ChrBrad@Idle01_InclusivityNegBt02")
    gestureMotions.append("ChrBrad@Idle01_InclusivityPosBt01")
    gestureMotions.append("ChrBrad@Idle01_InclusivityPosBt02")
    gestureMotions.append("ChrBrad@Idle01_IndicateLeftBt01")
    gestureMotions.append("ChrBrad@Idle01_IndicateLeftLf01")
    gestureMotions.append("ChrBrad@Idle01_IndicateRightBt01")
    gestureMotions.append("ChrBrad@Idle01_IndicateRightRt01")
    gestureMotions.append("ChrBrad@Idle01_MeLf01")
    gestureMotions.append("ChrBrad@Idle01_MeLf02")
    gestureMotions.append("ChrBrad@Idle01_NegativeBt01")
    gestureMotions.append("ChrBrad@Idle01_NegativeBt02")
    gestureMotions.append("ChrBrad@Idle01_NegativeBt03")
    gestureMotions.append("ChrBrad@Idle01_NegativeRt01")
    gestureMotions.append("ChrBrad@Idle01_OfferBoth01")
    gestureMotions.append("ChrBrad@Idle01_OfferLf01")
    gestureMotions.append("ChrBrad@Idle01_PleaBt02")
    gestureMotions.append("ChrBrad@Idle01_PointLf01")
    gestureMotions.append("ChrBrad@Idle01_PointLf02")
    gestureMotions.append("ChrBrad@Idle01_PointRt01")
    gestureMotions.append("ChrBrad@Idle01_ReceiveRt01")
    gestureMotions.append("ChrBrad@Idle01_SafeLf01")
    gestureMotions.append("ChrBrad@Idle01_ScratchChest01")
    gestureMotions.append("ChrBrad@Idle01_ScratchHeadLf01")
    gestureMotions.append("ChrBrad@Idle01_ScratchTempleLf01")
    gestureMotions.append("ChrBrad@Idle01_ShoulderStretch01")
    gestureMotions.append("ChrBrad@Idle01_Shrug01")
    gestureMotions.append("ChrBrad@Idle01_Shrug02")
    gestureMotions.append("ChrBrad@Idle01_SqueezeHand01")
    gestureMotions.append("ChrBrad@Idle01_StopBt01")
    gestureMotions.append("ChrBrad@Idle01_StopBt02")
    gestureMotions.append("ChrBrad@Idle01_SurroundBt01")
    gestureMotions.append("ChrBrad@Idle01_SurroundBt02")
    gestureMotions.append("ChrBrad@Idle01_Think01")
    gestureMotions.append("ChrBrad@Idle01_ToIdle02")
    gestureMotions.append("ChrBrad@Idle01_ToIdle03")
    gestureMotions.append("ChrBrad@Idle01_ToLocIdle01")
    gestureMotions.append("ChrBrad@Idle01_TouchHands01")
    gestureMotions.append("ChrBrad@Idle01_ToWalk01_Turn180Rt01")
    gestureMotions.append("ChrBrad@Idle01_ToWalk01_Turn360Rt01")
    gestureMotions.append("ChrBrad@Idle01_ToWalk01_Turn90Rt01")
    gestureMotions.append("ChrBrad@Idle01_WeightShift01")
    gestureMotions.append("ChrBrad@Idle01_WeightShift02")
    gestureMotions.append("ChrBrad@Idle01_WeRt01")
    gestureMotions.append("ChrBrad@Idle01_WeRt02")
    gestureMotions.append("ChrBrad@Idle01_WhenBt01")
    gestureMotions.append("ChrBrad@Idle01_WhenBt02")
    gestureMotions.append("ChrBrad@Idle01_YouLf01")
    gestureMotions.append("ChrBrad@Idle01_YouLf02")
    gestureMotions.append("ChrBrad@Idle01_YouLf03")
    gestureMotions.append("ChrBrad@Idle01_YouPointLf01")
    gestureMotions.append("ChrBrad@Idle01_YouRt01")

    sbChar = scene.getCharacter(charName)
    if sbChar == None:
        return
    skelName = sbChar.getSkeleton().getName()

    motions = StringVec()
    assetManager = scene.getAssetManager()
    for i in range(0, len(gestureMotions)):
        sbMotion = assetManager.getMotion(gestureMotions[i])
        if sbMotion != None:
            sbMotion.setMotionSkeletonName("ChrBrad2.sk")

    createRetargetInstance('ChrBrad2.sk', skelName)

    # outDir = scene.getMediaPath() + '/retarget/motion/' + skelName + '/';
    # print 'outDir = ' + outDir ;
    # if not os.path.exists(outDir):
    #	os.makedirs(outDir)

    # retarget gestures
    # for n in range(0, len(gestureMotions)):
    #	curMotion = scene.getMotion(gestureMotions[n])
    #	if curMotion is not None:
    #		retargetMotion(gestureMotions[n], 'ChrBrad.sk', skelName, outDir + 'Gestures/');
    #	else:
    #		print "Cannot find motion " + gestureMotions[n] + ", it will be excluded from the gesture setup..."

    sbChar.setStringAttribute("gestureMap", "BradGesture")
    if sbChar.getAttribute("gestureMapAngry") is None:
        sbChar.createStringAttribute("gestureMapAngry", "", True, "Gestures", 51, False, False, False, "angry gesture map")
    sbChar.setStringAttribute("gestureMapAngry", "BradGestureAngry")
    if sbChar.getAttribute("gestureMapSad") is None:
        sbChar.createStringAttribute("gestureMapSad", "", True, "Gestures", 52, False, False, False, "sad gesture map")
    sbChar.setStringAttribute("gestureMapSad", "BradGestureSad")
