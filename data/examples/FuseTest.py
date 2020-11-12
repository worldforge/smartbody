print("Sample Fuse Character")

# location of the Fuse model as a COLLADA file
modelName = "FuseB"
scene.loadAssetsFromPath("c:/users/Ari/Dropbox/fuse/" + modelName)

obj = scene.createPawn("light0")
obj.setPosition(SrVec(0, 180, 0))
obj.setOrientation(SrQuat(-0.127278, -0.316205, -0.647529, 0.681557))
obj.setBoolAttribute("visible", False)

obj = scene.createPawn("light1")
obj.setPosition(SrVec(0, 180, 0))
obj.setOrientation(SrQuat(0.0011471, 0.741289, -0.662899, -0.105139))
obj.setBoolAttribute("visible", False)

obj = scene.createPawn("light2")
obj.setPosition(SrVec(0, 0, 0))
obj.setOrientation(SrQuat(1, 0, 0, 0))
obj.setBoolAttribute("visible", False)

scene.addAssetPath("script", "sbm-common/scripts")
scene.addAssetPath("script", "scripts")
scene.addAssetPath("script", "behaviorsets")

scene.setBoolAttribute("internalAudio", True)

skeleton = scene.getSkeleton(modelName + ".dae")
scene.run("mixamo-map2.py")

mixamoMap = scene.getJointMapManager().getJointMap("mixamorig")
mixamoMap.applySkeleton(skeleton)

c = scene.createCharacter("test", "")
c.setSkeleton(scene.createSkeleton(modelName + ".dae"))
c.createStandardControllers()

fd = scene.createFaceDefinition('fuse')
fd.setFaceNeutral("")
fd.setAU(1, "left", "")
fd.setAU(1, "right", "")
fd.setAU(2, "left", "")
fd.setAU(2, "right", "")
fd.setAU(4, "left", "")
fd.setAU(4, "right", "")
fd.setAU(5, "both", "")
fd.setAU(6, "both", "")
fd.setAU(7, "both", "")
fd.setAU(10, "both", "")
fd.setAU(12, "left", "")
fd.setAU(12, "right", "")
fd.setAU(24, "both", "")
fd.setAU(25, "both", "")
fd.setAU(26, "both", "")
fd.setAU(45, "left", "")
fd.setAU(45, "right", "")
fd.setViseme("blink", "")

fd.setViseme("open", "")
fd.setViseme("W", "")
fd.setViseme("ShCh", "")
fd.setViseme("PBM", "")
fd.setViseme("FV", "")
fd.setViseme("wide", "")
fd.setViseme("tBack", "")
fd.setViseme("tRoof", "")
fd.setViseme("tTeeth", "")

c.setFaceDefinition(fd)

c.setStringAttribute("deformableMesh", modelName + ".dae")

c.setActionAttribute("updateChannel")

scene.run("init-diphoneDefault.py")
c.setStringAttribute('lipSyncSetName', 'default')
c.setBoolAttribute('usePhoneBigram', True)
c.setStringAttribute('voiceCode', 'CereVoice|Isabella|-|English|(East|Coast|America)')
c.setStringAttribute('voice', 'remote')

scene.run("runNVBG.py")
nvbg = c.getNvbg().nvbg

# setup gestures
scene.run('BehaviorSetGestures.py')
setupBehaviorSet()
retargetBehaviorSet('test')


class MyController(PythonController):

    def init(self, pawn):
        setName("FuseShapes")
        # setup rig nodes
        print("Setting up face rig...")
        # set ja/li parameters to .5
        self.setChannelValue("ja", .5)
        self.setChannelValue("li", .5)

    def evaluate(self):
        # map the Fuse shapes to SmartBody AUs and visemes

        # eyebrows
        val = self.getChannelValue("au_1_left")
        self.setChannelValue("BrowsOuterLower_Left_ncl1_2-lib", val)
        val = self.getChannelValue("au_1_right")
        self.setChannelValue("BrowsOuterLower_Right_ncl1_2-lib", val)

        val = self.getChannelValue("au_2_left")
        self.setChannelValue("BrowsUp_Left_ncl1_2-lib", val)
        val = self.getChannelValue("au_2_right")
        self.setChannelValue("BrowsUp_Right_ncl1_2-lib", val)

        val = self.getChannelValue("au_4_left")
        self.setChannelValue("BrowsDown_Left_ncl1_2-lib", val)
        val = self.getChannelValue("au_4_right")
        self.setChannelValue("BrowsDown_Right_ncl1_2-lib", val)

        val = self.getChannelValue("au_5")
        self.setChannelValue("EyesWide_Left_ncl1_2-lib", val)
        self.setChannelValue("EyesWide_Left-lib", val)
        self.setChannelValue("EyesWide_Right_ncl1_2-lib", val)
        self.setChannelValue("EyesWide_Right-lib", val)

        val = self.getChannelValue("au_6")

        val = self.getChannelValue("au_7")
        self.setChannelValue("Squint_Left_ncl1_2-lib", val)
        self.setChannelValue("Squint_Left-lib", val)
        self.setChannelValue("Squint_Right_ncl1_2-lib", val)
        self.setChannelValue("Squint_Right-lib", val)

        val = self.getChannelValue("au_10")
        self.setChannelValue("UpperLipUp_Left_ncl1_2-lib", val)
        self.setChannelValue("UpperLipUp_Right_ncl1_2-lib", val)

        val = self.getChannelValue("au_12_left")
        self.setChannelValue("Smile_Left_ncl1_2-lib", val)
        val = self.getChannelValue("au_12_right")
        self.setChannelValue("Smile_Right_ncl1_2-lib", val)

        val = self.getChannelValue("au_24")
        self.setChannelValue("UpperLipIn_ncl1_2-lib", val)
        self.setChannelValue("LowerLipIn_ncl1_2-lib", val)

        val = self.getChannelValue("au_25")
        self.setChannelValue("UpperLipUp_Left_ncl1_2-lib", val * .5)
        self.setChannelValue("UpperLipUp_Right_ncl1_2-lib", val * .5)
        self.setChannelValue("LowerLipOut_ncl1_2-lib", val * .5)

        val = self.getChannelValue("au_26")
        self.setChannelValue("Jaw_Down_ncl1_2-lib", val)

        # blinking
        '''
        val = self.getChannelValue("au_45_left")
        self.setChannelValue("Blink_Left_ncl1_2-lib", val)
        self.setChannelValue("Blink_Left-lib", val)
        val = self.getChannelValue("au_45_right")
        self.setChannelValue("Blink_Right_ncl1_2-lib", val)
        self.setChannelValue("Blink_Right-lib", val)
        '''
        val = self.getChannelValue("blink")
        self.setChannelValue("Blink_Left_ncl1_2-lib", val)
        self.setChannelValue("Blink_Left-lib", val)
        self.setChannelValue("Blink_Right_ncl1_2-lib", val)
        self.setChannelValue("Blink_Right-lib", val)

        # expressive speech modifier
        javal = scene.getCharacter("test").getDouble("ja")  # jaw
        lival = scene.getCharacter("test").getDouble("li")  # lips

        # activate the eyelashes morph as needed
        val = self.getChannelValue("EyesWide_Left_ncl1_2-lib")
        if (val > 0.0):
            self.setChannelValue("EyesWide_Left-lib", val)

        val = self.getChannelValue("EyesWide_Right_ncl1_2-lib")
        if (val > 0.0):
            self.setChannelValue("EyesWide_Right-lib", val)

        val = self.getChannelValue("Blink_Left_ncl1_2-lib")
        if (val > 0.0):
            self.setChannelValue("Blink_Left-lib", val)

        val = self.getChannelValue("Blink_Right_ncl1_2-lib")
        if (val > 0.0):
            self.setChannelValue("Blink_Right-lib", val)

        val = self.getChannelValue("Squint_Left_ncl1_2-lib")
        if (val > 0.0):
            self.setChannelValue("Squint_Right-lib", val)

        val = self.getChannelValue("Squint_Right_ncl1_2-lib")
        if (val > 0.0):
            self.setChannelValue("Squint_Right-lib", val)

        # lip shapes
        fvval = self.getChannelValue("FV")
        self.setChannelValue("JawBackward_ncl1_2-lib", fvval)
        self.setChannelValue("LowerLipIn_ncl1_2-lib", fvval)
        self.setChannelValue("UpperLipUp_Left_ncl1_2-lib", fvval * .5)
        self.setChannelValue("UpperLipUp_Right_ncl1_2-lib", fvval * .5)

        val = self.getChannelValue("PBM")
        self.setChannelValue("UpperLipIn_ncl1_2-lib", val)
        self.setChannelValue("LowerLipIn_ncl1_2-lib", val)

        val = self.getChannelValue("ShCh")
        self.setChannelValue("LowerLipOut_ncl1_2-lib", val)
        self.setChannelValue("UpperLipOut_ncl1_2-lib", val)
        self.setChannelValue("MouthNarrow_Left_ncl1_2-lib", val)
        self.setChannelValue("MouthNarrow_Right_ncl1_2-lib", val)

        self.setChannelValue("LowerLipDown_Left_ncl1_2-lib", val * .5)
        self.setChannelValue("LowerLipDown_Right_ncl1_2-lib", val * .5)
        self.setChannelValue("UpperLipDown_Left_ncl1_2-lib", val * .5)
        self.setChannelValue("UpperLipDown_Right_ncl1_2-lib", val * .5)

        val2 = self.getChannelValue("W")
        if (val2 > val):
            self.setChannelValue("LowerLipOut_ncl1_2-lib", val2)
            self.setChannelValue("UpperLipOut_ncl1_2-lib", val2)
            self.setChannelValue("MouthNarrow_Left_ncl1_2-lib", val2)
            self.setChannelValue("MouthNarrow_Right_ncl1_2-lib", val2)

        val = self.getChannelValue("open")
        # jaw opening gets magnified according to ja parameter
        val = val * (javal * 2.0)
        if val > 1.0:
            val = 1.0
        self.setChannelValue("Jaw_Down_ncl1_2-lib", val)
        curval = self.getChannelValue("UpperLipUp_Left_ncl1_2-lib")
        if curval < val * .25:
            self.setChannelValue("UpperLipUp_Left_ncl1_2-lib", val * .25)
            self.setChannelValue("UpperLipUp_Right_ncl1_2-lib", val * .25)

        val = self.getChannelValue("wide")
        self.setChannelValue("FrownLeft_ncl1_2-lib", val)
        self.setChannelValue("FrownRight_ncl1_2-lib", val)
        self.setChannelValue("SmileLeft_ncl1_2-lib", val)
        self.setChannelValue("SmileRight_ncl1_2-lib", val)

        val = self.getChannelValue("tBack")

        val = self.getChannelValue("tRoof")
        self.setChannelValue("TongueUp_ncl1_2-lib", val)

        val = self.getChannelValue("tTeeth")

        # lips are magnified according to li parameter
        val = self.getChannelValue("LowerLipDown_Left_ncl1_2-lib")
        val = val * (lival * 2.0)
        if val > 1.0:
            val = 1.0
        self.setChannelValue("LowerLipDown_Left_ncl1_2-lib", val)
        self.setChannelValue("LowerLipDown_Right_ncl1_2-lib", val)

        val = self.getChannelValue("UpperLipUp_Left_ncl1_2-lib")
        val = val * (lival * 2.0)
        if val > 1.0:
            val = 1.0
        self.setChannelValue("UpperLipUp_Left_ncl1_2-lib", val)
        self.setChannelValue("UpperLipUp_Right_ncl1_2-lib", val)


# instantiate this controller once for each character
myc = MyController()
# get the character
character = scene.getCharacter('test')
character.addController(50, myc)
# character.removeController(myc)


bml.execBML("*", "<body posture=\"ChrBrad@Idle01\"/>")

c.setStringAttribute('voiceCode', 'CereVoice|Isabella|-|English|(East|Coast|America)')
c.setStringAttribute('voice', 'remote')

scene.run("runNVBG.py")
nvbg = c.getNvbg().nvbg

# setup gestures
scene.run('BehaviorSetGestures.py')
setupBehaviorSet()
retargetBehaviorSet('test')
