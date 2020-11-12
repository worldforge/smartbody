# PLEASE NOTE:
#  This .py file is not longer in use.  All smartbody initialization is now done within Unity itself.  See the SmartbodyManager gameobject
#
#

def addHandMap(jointMapName):
    jointMapManager = scene.getJointMapManager()
    zebra2Map = jointMapManager.getJointMap(jointMapName)
    if (zebra2Map == None):
        return
    # Core
    zebra2Map.setMapping("JtWristLf", "l_wrist1")
    zebra2Map.setMapping("JtThumbALf", "l_thumb1")
    zebra2Map.setMapping("JtThumbBLf", "l_thumb2")
    zebra2Map.setMapping("JtThumbCLf", "l_thumb3")
    zebra2Map.setMapping("JtThumbDLf", "l_thumb4")
    zebra2Map.setMapping("JtIndexALf", "l_index1")
    zebra2Map.setMapping("JtIndexBLf", "l_index2")
    zebra2Map.setMapping("JtIndexCLf", "l_index3")
    zebra2Map.setMapping("JtIndexDLf", "l_index4")
    zebra2Map.setMapping("JtMiddleALf", "l_middle1")
    zebra2Map.setMapping("JtMiddleBLf", "l_middle2")
    zebra2Map.setMapping("JtMiddleCLf", "l_middle3")
    zebra2Map.setMapping("JtMiddleDLf", "l_middle4")
    zebra2Map.setMapping("JtRingALf", "l_ring1")
    zebra2Map.setMapping("JtRingBLf", "l_ring2")
    zebra2Map.setMapping("JtRingCLf", "l_ring3")
    zebra2Map.setMapping("JtRingDLf", "l_ring4")
    zebra2Map.setMapping("JtLittleALf", "l_pinky1")
    zebra2Map.setMapping("JtLittleBLf", "l_pinky2")
    zebra2Map.setMapping("JtLittleCLf", "l_pinky3")
    zebra2Map.setMapping("JtLittleDLf", "l_pinky4")

    # Arm, right   
    zebra2Map.setMapping("JtWristRt", "r_wrist1")
    zebra2Map.setMapping("JtThumbARt", "r_thumb1")
    zebra2Map.setMapping("JtThumbBRt", "r_thumb2")
    zebra2Map.setMapping("JtThumbCRt", "r_thumb3")
    zebra2Map.setMapping("JtThumbDRt", "r_thumb4")
    zebra2Map.setMapping("JtIndexARt", "r_index1")
    zebra2Map.setMapping("JtIndexBRt", "r_index2")
    zebra2Map.setMapping("JtIndexCRt", "r_index3")
    zebra2Map.setMapping("JtIndexDRt", "r_index4")
    zebra2Map.setMapping("JtMiddleARt", "r_middle1")
    zebra2Map.setMapping("JtMiddleBRt", "r_middle2")
    zebra2Map.setMapping("JtMiddleCRt", "r_middle3")
    zebra2Map.setMapping("JtMiddleDRt", "r_middle4")
    zebra2Map.setMapping("JtRingARt", "r_ring1")
    zebra2Map.setMapping("JtRingBRt", "r_ring2")
    zebra2Map.setMapping("JtRingCRt", "r_ring3")
    zebra2Map.setMapping("JtRingDRt", "r_ring4")
    zebra2Map.setMapping("JtLittleARt", "r_pinky1")
    zebra2Map.setMapping("JtLittleBRt", "r_pinky2")
    zebra2Map.setMapping("JtLittleCRt", "r_pinky3")
    zebra2Map.setMapping("JtLittleDRt", "r_pinky4")
    print('Finish Add Hand Map')
