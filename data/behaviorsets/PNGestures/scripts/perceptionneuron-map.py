



def perceptionNeuronMap():

    # Mapping from Zebra2 skeleton to SmartBody skeleton

    jointMapManager = scene.getJointMapManager()
    perceptionNeuronMap = jointMapManager.getJointMap("perceptionneuron")
    if (perceptionNeuronMap == None):
		perceptionNeuronMap = jointMapManager.createJointMap('perceptionneuron')

    # Core
    perceptionNeuronMap.setMapping("Hips", "base")
    perceptionNeuronMap.setMapping("Spine1", "spine1")
    perceptionNeuronMap.setMapping("Spine2", "spine2")
    perceptionNeuronMap.setMapping("Spine3", "spine3")
    perceptionNeuronMap.setMapping("Neck", "spine5")
    #perceptionNeuronMap.setMapping("", "spine5")
    perceptionNeuronMap.setMapping("Head", "skullbase")

    # Arm, left
    #perceptionNeuronMap.setMapping("LeftShoulder", "l_sternoclavicular")
    perceptionNeuronMap.setMapping("LeftArm", "l_shoulder")
    perceptionNeuronMap.setMapping("", "l_upperarm1")
    perceptionNeuronMap.setMapping("", "l_upperarm2")
    perceptionNeuronMap.setMapping("LeftForeArm", "l_elbow")
    perceptionNeuronMap.setMapping("", "l_forearm1")
    perceptionNeuronMap.setMapping("", "l_forearm2")
    perceptionNeuronMap.setMapping("LeftHand", "l_wrist")
    perceptionNeuronMap.setMapping("LeftHandThumb1", "l_thumb1")
    perceptionNeuronMap.setMapping("LeftHandThumb2", "l_thumb2")
    perceptionNeuronMap.setMapping("LeftHandThumb3", "l_thumb3")
    perceptionNeuronMap.setMapping("", "l_thumb4")
    perceptionNeuronMap.setMapping("LeftInHandIndex", "l_index1")
    perceptionNeuronMap.setMapping("LeftHandIndex1", "l_index2")
    perceptionNeuronMap.setMapping("LeftHandIndex2", "l_index3")
    perceptionNeuronMap.setMapping("LeftHandIndex3", "l_index4")
    perceptionNeuronMap.setMapping("LeftInHandMiddle", "l_middle1")
    perceptionNeuronMap.setMapping("LeftHandMiddle1", "l_middle2")
    perceptionNeuronMap.setMapping("LeftHandMiddle2", "l_middle3")
    perceptionNeuronMap.setMapping("LeftHandMiddle3", "l_middle4")
    perceptionNeuronMap.setMapping("LeftInHandRing", "l_ring1")
    perceptionNeuronMap.setMapping("LeftHandRing1", "l_ring2")
    perceptionNeuronMap.setMapping("LeftHandRing2", "l_ring3")
    perceptionNeuronMap.setMapping("LeftHandRing3", "l_ring4")
    perceptionNeuronMap.setMapping("LeftInHandPinky", "l_pinky1")
    perceptionNeuronMap.setMapping("LeftHandPinky1", "l_pinky2")
    perceptionNeuronMap.setMapping("LeftHandPinky2", "l_pinky3")
    perceptionNeuronMap.setMapping("LeftHandPinky3", "l_pinky4")

    # Arm, right
    #perceptionNeuronMap.setMapping("RightShoulder", "r_sternoclavicular")
    perceptionNeuronMap.setMapping("RightArm", "r_shoulder")
    perceptionNeuronMap.setMapping("", "r_upperarm1")
    perceptionNeuronMap.setMapping("", "r_upperarm2")
    perceptionNeuronMap.setMapping("RightForeArm", "r_elbow")
    perceptionNeuronMap.setMapping("", "r_forearm1")
    perceptionNeuronMap.setMapping("", "r_forearm2")
    perceptionNeuronMap.setMapping("RightHand", "r_wrist")
    perceptionNeuronMap.setMapping("RightHandThumb1", "r_thumb1")
    perceptionNeuronMap.setMapping("RightHandThumb2", "r_thumb2")
    perceptionNeuronMap.setMapping("RightHandThumb3", "r_thumb3")
    perceptionNeuronMap.setMapping("", "r_thumb4")
    perceptionNeuronMap.setMapping("RightInHandIndex", "r_index1")
    perceptionNeuronMap.setMapping("RightHandIndex1", "r_index2")
    perceptionNeuronMap.setMapping("RightHandIndex2", "r_index3")
    perceptionNeuronMap.setMapping("RightHandIndex3", "r_index4")
    perceptionNeuronMap.setMapping("RightInHandMiddle", "r_middle1")
    perceptionNeuronMap.setMapping("RightHandMiddle1", "r_middle2")
    perceptionNeuronMap.setMapping("RightHandMiddle2", "r_middle3")
    perceptionNeuronMap.setMapping("RightHandMiddle3", "r_middle4")
    perceptionNeuronMap.setMapping("RightInHandRing", "r_ring1")
    perceptionNeuronMap.setMapping("RightHandRing1", "r_ring2")
    perceptionNeuronMap.setMapping("RightHandRing2", "r_ring3")
    perceptionNeuronMap.setMapping("RightHandRing3", "r_ring4")
    perceptionNeuronMap.setMapping("RightInHandPinky", "r_pinky1")
    perceptionNeuronMap.setMapping("RightHandPinky1", "r_pinky2")
    perceptionNeuronMap.setMapping("RightHandPinky2", "r_pinky3")
    perceptionNeuronMap.setMapping("RightHandPinky3", "r_pinky4")

    # Leg, left
    perceptionNeuronMap.setMapping("LeftUpLeg", "l_hip")
    perceptionNeuronMap.setMapping("LeftLeg", "l_knee")
    perceptionNeuronMap.setMapping("LeftFoot", "l_ankle")
    perceptionNeuronMap.setMapping("LeftFootEndSite", "l_forefoot")
    perceptionNeuronMap.setMapping("", "l_toe")

    # Leg, right
    perceptionNeuronMap.setMapping("RightUpLeg", "r_hip")
    perceptionNeuronMap.setMapping("RightLeg", "r_knee")
    perceptionNeuronMap.setMapping("RightFoot", "r_ankle")
    perceptionNeuronMap.setMapping("RightFootEndSite", "r_forefoot")
    perceptionNeuronMap.setMapping("", "r_toe")

    # Head, left
    perceptionNeuronMap.setMapping("", "eyeball_left")
    perceptionNeuronMap.setMapping("", "upper_eyelid_left")
    perceptionNeuronMap.setMapping("", "lower_eyelid_left")

    # Head, right
    perceptionNeuronMap.setMapping("", "eyeball_right")
    perceptionNeuronMap.setMapping("", "upper_eyelid_right")
    perceptionNeuronMap.setMapping("", "lower_eyelid_right")

    #perceptionNeuronMap.setMapping("eyeJoint_R", "eyeball_right")
    #perceptionNeuronMap.setMapping("eyeJoint_L", "eyeball_left")



perceptionNeuronMap()
