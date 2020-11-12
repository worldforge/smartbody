print("|--------------------------------------------|")
print("|  data/sbm-common/scripts/obj-interact.py  |")
print("|--------------------------------------------|")

import random


# ========= UTILITY FUNCTIONS =========
def mDBGINFO(txt):
    print(txt)
    return


def vec2str(vec):
    x = vec.getData(0)
    y = vec.getData(1)
    z = vec.getData(2)
    if x < 0.0001 and x > -0.0001:
        x = 0
    if y < 0.0001 and y > -0.0001:
        y = 0
    if z < 0.0001 and z > -0.0001:
        z = 0
    return "\"" + str(x) + " " + str(y) + " " + str(z) + "\""


def vecXZ2str(vec):
    x = vec.getData(0)
    z = vec.getData(2)
    if x < 0.0001 and x > -0.0001:
        x = 0
    if z < 0.0001 and z > -0.0001:
        z = 0
    return "\"" + str(x) + " " + str(z) + "\""


def vecSub(vec1, vec2):
    x = vec1.getData(0) - vec2.getData(0)
    y = vec1.getData(1) - vec2.getData(1)
    z = vec1.getData(2) - vec2.getData(2)
    return SrVec(x, y, z)


def vecPlus(vec1, vec2):
    x = vec1.getData(0) + vec2.getData(0)
    y = vec1.getData(1) + vec2.getData(1)
    z = vec1.getData(2) + vec2.getData(2)
    return SrVec(x, y, z)


def vecDist(vec1, vec2):
    distVec = vecSub(vec1, vec2)
    return distVec.len()


def angDist(ang1, ang2):
    angDiff = ang1 - ang2
    return normalizeAngleDeg(angDiff)


def normalizeAngleDeg(ang):
    while (ang > 180):
        ang = ang - 360
    while (ang < -180):
        ang = ang + 360
    return ang


def vec2Yaw(vec):
    return vec.vecYaw(vec)


def toDegree(radian):
    return 180 * radian / 3.14159265


def toRadian(degree):
    return 3.14159265 * degree / 180


def abs(val):
    if (val >= 0):
        return val
    else:
        return -val


def getPos(chrName):
    chr = scene.getCharacter(chrName)
    return chr.getPosition()


def getPos2D(chrName):
    chr = scene.getCharacter(chrName)
    pos = chr.getPosition()
    pos.setData(1, 0)  # clear Y
    return pos


def getHeight(chrName):
    chr = scene.getCharacter(chrName)
    return chr.getHeight()


def getFacing(chrName):
    chr = scene.getCharacter(chrName)
    hpr = chr.getHPR()
    return hpr.getData(0)


def setPos2D(chrName, posVec):
    chr = scene.getCharacter(chrName)
    chrPos = chr.getPosition()
    chrPos.setData(0, posVec.getData(0))
    chrPos.setData(2, posVec.getData(2))
    chr.setPosition(chrPos)


def setFacing(chrName, heading):
    chr = scene.getCharacter(chrName)
    chr.setHPR(SrVec(heading, 0, 0))


# =====================================


offerHandPosHeightRatio = 0.75
offerHandBodyDistHeightRatio = 0.4
offerHandIsLeft = True
takerHandIsLeft = True
takerGiverDistHeightRatio = 0.3
# field of view of human eye is 95 degrees away from nose
facingThreshold = 95

# for state machine
IDLE = 0
PASSING = 1
LOCOMOTION = 2

# for set up character array
totalChr = 30
maxItemsPerGridCell = 7
if (totalChr > maxItemsPerGridCell):
    steerManager = scene.getSteerManager()
    steerManager.setIntAttribute("gridDatabaseOptions.maxItemsPerGridCell", totalChr)
chrCircleRadiusHeightRatio = 0.467

scene.run("default-viewer.py")

# scene.setVHMsgEnabled(False)

### Load data/sbm-common assets
scene.addAssetPath("seq", "sbm-common/scripts")
scene.addAssetPath("seq", "sbm-test/scripts")
scene.addAssetPath("mesh", "mesh")
scene.addAssetPath("mesh", "retarget/mesh")

scene.run("init-common-assets.py")
scene.run("init-common-face.py")
scene.run("init-utah-face.py")

# set up characters standing in a circle
chrBaseName = 'brad'
characters = StringVec()
for i in range(0, totalChr):
    chrName = chrBaseName + str(i)
    # chr = scene.createCharacter(chrName, "SasoBase.SasoDoctorPerez")
    chr = scene.createCharacter(chrName, "")
    chrSkeleton = scene.createSkeleton("common.sk")
    chr.setSkeleton(chrSkeleton)
    chr.setFaceDefinition(defaultFace)
    chrCircleRadius = chr.getHeight() * chrCircleRadiusHeightRatio
    chrPos = SrVec(0, 102, chrCircleRadius)
    rot = 360 / totalChr * i
    chrPos.rotY(toRadian(rot))
    chr.setPosition(chrPos)
    chrHPR = SrVec(rot + 180, 0, 0)
    chr.setHPR(chrHPR)
    chr.setVoice("remote")
    chr.setVoiceCode("Festival_voice_rab_diphone")
    chr.createStandardControllers()
    chr.setStringAttribute("deformableMesh", chrBaseName)
    characters.append(chrName)

# set up target (box) and a dummy object
target = scene.createPawn("box")
target.setStringAttribute("collisionShape", "box")
target.getAttribute("collisionShapeScale").setValue(SrVec(5, 5, 5))

dummy = scene.createPawn("dummyobj")

scene.run("init-param-animation.py")

scene.run("init-steer-agents.py")
steerManager = scene.getSteerManager()

# set up steering
numCharacters = scene.getNumCharacters()
charNames = scene.getCharacterNames()
for i in range(0, numCharacters):
    setupSteerAgent(charNames[i], 'all')
steerManager.setEnable(True)

scene.setBoolAttribute("internalAudio", True)

# add the reaching database
scene.run("init-example-reach.py")
names = scene.getCharacterNames()
for n in range(0, len(names)):
    reachSetup(names[n], "KNN", "")

evtMgr = scene.getEventManager()


# =====================================
# offer hand angle based on taker's location: -180~180->-90~90 non-linear mapping
def offerAngleBasedOnTakerPos(x):
    negX = False
    if x < 0:
        x = -x
        negX = True
    res = 0.00000010139 * x * x * x * x - 0.000031499 * x * x * x - 0.0013 * x * x + 1.1619 * x
    if negX:
        return -res
    else:
        return res


# taker's relative direction in giver's local coordinate
# useful in checking which hand should be use to grab obj in the first place
def takerDirRelativeToGiver(giverName, takerName):
    giverPos = getPos2D(giverName)
    giverHeading = getFacing(giverName)
    takerPos = getPos2D(takerName)
    x = toDegree(vec2Yaw(vecSub(takerPos, giverPos)))
    x = normalizeAngleDeg(x - giverHeading)
    # mDBGINFO( 'taker's direction in giver's local coord (deg): ' + str(x) )
    return x


# position to place object for taker (giver's hand position)
def offerHandPos(giverName, takerName):
    giverChr = scene.getCharacter(giver)
    giverPos = giverChr.getPosition()
    giverPos.setData(1, 0)
    giverHeight = giverChr.getHeight()
    offerHeight = giverHeight * offerHandPosHeightRatio
    # add randomness to offer hand height
    offerHeight = offerHeight * (1 - random.random() / 5)
    offerDist = giverHeight * offerHandBodyDistHeightRatio
    # add randomness to offer hand distance to giver body
    offerDist = offerDist * (1 - random.random() / 5)
    giverHPR = giverChr.getHPR()
    giverHeading = giverHPR.getData(0)
    # mDBGINFO( 'giverHeading: ' + str(giverHeading) )
    takerChr = scene.getCharacter(taker)
    takerPos = takerChr.getPosition()
    takerPos.setData(1, 0)
    x = toDegree(vec2Yaw(vecSub(takerPos, giverPos)))
    x = normalizeAngleDeg(x - giverHeading)
    # mDBGINFO( 'taker direction relative to giver: ' + str(x) )
    offset2D = SrVec(0, offerHeight, offerDist)
    offerHandDir = offerAngleBasedOnTakerPos(x)
    # mDBGINFO( 'offer hand direction relative to giver: ' + str(offerHandDir) )
    offerHandDir = normalizeAngleDeg(giverHeading + offerHandDir)
    # mDBGINFO( 'offer hand direction in world coord: ' + str(offerHandDir) )
    offset2D.rotY(toRadian(offerHandDir))
    offerPos = vecPlus(giverPos, offset2D);
    return offerPos


# =====================================


def objManip(giver, taker, obj):
    type = '\"right\"'
    if takerDirRelativeToGiver(giver, taker) >= 0:
        type = '\"left\"'
    cmd = '<sbm:reach sbm:action=\"pick-up\" sbm:reach-finish=\"true\" sbm:reach-type=' + type + ' target=\"' + obj + '\"/>'
    cmd = cmd + '<gaze sbm:joint-range=\"EYES NECK\" target=\"dummyobj\"/>'
    bml.execBML(giver, cmd)
    global state
    state = PASSING


def t():
    objManip(giver, taker, obj)


def objManipReset():
    takerPos2D = getPos2D(taker)
    takerHeight = getHeight(taker)
    putdownHeight = offerHandPosHeightRatio * takerHeight
    putdownDist = offerHandBodyDistHeightRatio * takerHeight
    offset2D = SrVec(0, putdownHeight, putdownDist)
    takerHeading = getFacing(taker)
    offset2D.rotY(toRadian(takerHeading))
    dummy.setPosition(vecPlus(takerPos2D, offset2D))
    cmd = '<sbm:reach sbm:action="put-down" sbm:target-pos=' + vec2str(dummy.getPosition()) + ' sbm:reach-finish="true"/>'
    # bml.execBML(taker, cmd)
    scene.commandAt(0.8, 'bml char ' + taker + ' ' + cmd);
    global state


# state = IDLE

def r():
    objManipReset()


class ReachingHandler(EventHandler):
    def executeAction(self, ev):
        params = ev.getParameters()
        global giver
        global taker
        global state
        global offerHandIsLeft
        global takerHandIsLeft
        # print "[[--------------------------------------------"
        # print params

        if "reach-complete" in params:
            mDBGINFO("[REACH COMPLETE] " + params)
            if ('bml char ' + giver) in params and 'reach-complete: left' in params:
                offerHandIsLeft = True
                mDBGINFO("[GIVER USING LEFT HAND] ")
            elif ('bml char ' + giver) in params and 'reach-complete: right' in params:
                offerHandIsLeft = False
                mDBGINFO("[GIVER USING RIGHT HAND] ")
            if ('bml char ' + taker) in params and 'reach-complete: left' in params:
                takerHandIsLeft = True
                mDBGINFO("[TAKER USING LEFT HAND] ")
            elif ('bml char ' + taker) in params and 'reach-complete: right' in params:
                takerHandIsLeft = False
                mDBGINFO("[TAKER USING RIGHT HAND] ")

        if "reach-returned" in params:
            mDBGINFO("[REACH RETURNED] " + params)
        # if "reach-return" in params:
        #	mDBGINFO( "[REACH RETURN] " + params )
        # if "reach-newTarget" in params:
        #	mDBGINFO( "[REACH NEW TARGET] " + params )

        if "pawn-released" in params:
            mDBGINFO("[PAWN RELEASED] from " + giver)
            keywd = "bml char " + giver
            if keywd in params:
                # print '[TAKER PICKUP: ' + taker + ']'
                # 3 taker pick-up obj
                takerPickup()

        elif "reach-stateNew: Idle" in params:
            mDBGINFO('[REACHED RETURNED] ' + params)
            keywd = "bml char " + giver
            if keywd in params:
                if "pawn-attached" in params:
                    if state == PASSING:
                        mDBGINFO('[GIVE OUT: ' + giver + '] ' + params)
                        # giver "offer" give-out
                        dummy.setPosition(offerHandPos(giver, taker))
                        cmd = '<sbm:reach sbm:action="put-down" sbm:target-pos=' + vec2str(dummy.getPosition()) + ' sbm:reach-finish="false"/>'
                        cmd = cmd + '<gaze sbm:joint-range="EYES NECK" target=\"' + taker + '\"/>'
                        bml.execBML(giver, cmd)
                        # taker gaze at target
                        bml.execBML(taker, '<gaze sbm:joint-range="EYES NECK" target="dummyobj"/>')
            keywd = "bml char " + taker
            if keywd in params:
                if "pawn-attached" in params:
                    if state == PASSING:
                        # print "[TAKE RETURNED: " + taker + "]"
                        alternate()
                        # giver "offer" give-out (keep passing out obj to next person)
                        dummy.setPosition(offerHandPos(giver, taker))
                        cmd = '<sbm:reach sbm:action="put-down" sbm:target-pos=' + vec2str(dummy.getPosition()) + ' sbm:reach-finish="false"/>'
                        cmd = cmd + '<gaze sbm:joint-range="EYES NECK" target=\"' + taker + '\"/>'
                        # bml.execBML(giver, cmd)
                        # add delay (with randomized duration 0.5~1.5s) inbetween reach-finish and the following reach-start
                        delay = 0.5 + random.random()
                        scene.commandAt(delay, 'bml char ' + giver + ' ' + cmd);
                        # taker gaze at target
                        takergazecmd = '<gaze sbm:joint-range="EYES NECK" target=\"' + giver + '\"/>'
                        # bml.execBML(taker, takergazecmd)
                        scene.commandAt(delay - 0.4, 'bml char ' + taker + ' ' + takergazecmd);
                    else:
                        objManipReset()

        elif "pawn-attached" in params:
            keywd = "bml char " + taker
            if keywd in params:
                mDBGINFO("[PAWN ATTACHED, GIVER PASS TO TAKER] ")
                # giver "offer" give-return
                cmd = '<sbm:reach sbm:action="put-down" sbm:target-pos=' + vec2str(dummy.getPosition()) + ' sbm:reach-finish="true"/>'
                bml.execBML(giver, cmd)
                # taker grab-return
                cmd = '<sbm:reach sbm:action="pick-up" sbm:reach-finish="true" target=\"' + obj + '\"/><gaze sbm:joint-range="EYES NECK" target="dummyobj"/>'
                bml.execBML(taker, cmd)
            keywd = "bml char " + giver
            if keywd in params:
                mDBGINFO("[PAWN ATTACHED, GIVER GRABBED OBJ] " + params)


# print "--------------------------------------------]]"

reachingHdl = ReachingHandler()
evtMgr.addEventHandler("reachNotifier", reachingHdl)

'''
class LocomotionHandler(EventHandler):
	def executeAction(self, ev):
		params = ev.getParameters()
		global state
		if state == LOCOMOTION:
			if "success" in params and taker in params:
				state = PASSING
				cmd = '<sbm:reach sbm:action="pick-up" sbm:reach-finish="false" target=\"' + obj + '\"/>'
				bml.execBML(taker, cmd)
				print '[LOCOMOTION ENDED]'

locomotionHdl = LocomotionHandler()
# this event is added inside here: SteeringAgent::sendLocomotionEvent
evtMgr.addEventHandler("locomotion", locomotionHdl)
'''


class FacingHandler(EventHandler):
    def executeAction(self, ev):
        params = ev.getParameters()
        global state
        if state == LOCOMOTION:
            if taker in params and 'success' in params:
                state = PASSING
                cmd = '<sbm:reach sbm:action="pick-up" sbm:reach-finish="false" target=\"' + obj + '\"/>'
                # also cancel facing adjustment
                curPos = getPos(taker)
                curPosX = curPos.getData(0)
                curPosZ = curPos.getData(2)
                cmd = cmd + '<locomotion target=\"' + str(curPosX) + ' ' + str(curPosZ) + '\"/>'
                bml.execBML(taker, cmd)
                mDBGINFO('[FACING ADJUSTED]')


facingHdl = FacingHandler()
# this event is added inside here: SteeringAgent::adjustFacingAngle(float angleDiff)
evtMgr.addEventHandler("adjustFacing", facingHdl)


def takerPickup():
    giverChr = scene.getCharacter(giver)
    giverPos = getPos2D(giver)
    takerPos = getPos2D(taker)
    target = scene.getPawn(obj)
    tgtPos = target.getPosition()
    tgtPos.setData(1, 0)
    chrHeight = giverChr.getHeight()
    walkThreshold = chrHeight * (takerGiverDistHeightRatio + -0.15)
    takerFacingCurrent = getFacing(taker)
    vecGiverToTaker = vecSub(giverPos, takerPos)
    giverDirectionToTaker = toDegree(vec2Yaw(vecGiverToTaker))
    facingDist = abs(angDist(takerFacingCurrent, giverDirectionToTaker))
    # mDBGINFO( 'giverDirectionToTaker: ' + str(giverDirectionToTaker) + ' , takerHeading: ' + str(takerFacingCurrent) )
    giverHPR = giverChr.getHPR()
    giverHeading = giverHPR.getData(0)
    offset2D = SrVec(0, 0, chrHeight * takerGiverDistHeightRatio)

    x = toDegree(vec2Yaw(vecSub(takerPos, giverPos)))
    x = normalizeAngleDeg(x - giverHeading)
    offerHandDir = offerAngleBasedOnTakerPos(x)
    offerHandDir = normalizeAngleDeg(giverHeading + offerHandDir)
    takerHeading = normalizeAngleDeg(offerHandDir + 180)

    offset2D.rotY(toRadian(offerHandDir))
    locomGoalPos = vecPlus(tgtPos, offset2D)
    chrDist = vecDist(locomGoalPos, takerPos)
    # mDBGINFO( 'chr dist: ' + str(chrDist) + ' ,  facing dist: ' + str(facingDist) )
    if chrDist > walkThreshold or facingDist > facingThreshold:
        global state
        state = LOCOMOTION
        # mDBGINFO( 'locomotion target: ' + vecXZ2str(locomGoalPos) + ' ,  facing: ' + str(takerHeading) )
        cmd = '<locomotion facing=\"' + str(takerHeading) + '\" target=' + vecXZ2str(locomGoalPos) + '/>'
        cmd = cmd + '<gaze sbm:joint-range="EYES NECK" target="dummyobj"/>'
        # mDBGINFO( cmd )
        bml.execBML(taker, cmd)
    else:
        cmd = '<sbm:reach sbm:action="pick-up" sbm:reach-finish="false" target=\"' + obj + '\"/>'
        cmd = cmd + '<gaze sbm:joint-range="EYES NECK" target="dummyobj"/>'
        bml.execBML(taker, cmd)


# shift character along its facing direction
def shiftCharacter(chrName, dist):
    chr = scene.getCharacter(chrName)
    chrPos = getPos(chrName)
    chrHeading = getFacing(chrName)
    offset2D = SrVec(0, 0, dist)
    offset2D.rotY(toRadian(chrHeading))
    chr.setPosition(vecPlus(chrPos, offset2D))


# =====================================
# =====================================

def sceneSetup():
    global giver
    global taker
    global obj
    global state
    global chrLocator
    giver = characters[0]
    taker = characters[1]
    obj = 'box'
    # state is global var
    state = IDLE
    chrLocator = 0

    scene.setDefaultCharacter(giver)
    scene.setDefaultRecipient(taker)

    target.setPosition(SrVec(5, 125, 26))
    dummy.setPosition(SrVec(5, 125, 26))
    cam = scene.getPawn("camera")
    cam.setPosition(SrVec(0, -15, 0))

    for i in range(1, len(characters)):
        shiftCharacter(characters[i], -500)
        chr = scene.getCharacter(characters[i])
        chr.setBoolAttribute("steering.pathFollowingMode", True)


def tenChrSetup():
    if len(characters) != 10:
        return
    setPos2D(characters[0], SrVec(-10, 0, 96))
    setFacing(characters[0], -147.6)
    setPos2D(characters[1], SrVec(43, 0, 18))
    setFacing(characters[1], -21.6)
    setPos2D(characters[2], SrVec(40, 0, -41))
    setFacing(characters[2], -29)
    setPos2D(characters[3], SrVec(39, 0, -101.6))
    setFacing(characters[3], -21.6)
    setPos2D(characters[4], SrVec(40.4, 0, -176.5))
    setFacing(characters[4], -7)
    setPos2D(characters[5], SrVec(-30, 0, -247))
    setFacing(characters[5], -29)
    setPos2D(characters[6], SrVec(-84, 0, -160))
    setFacing(characters[6], 162)
    setPos2D(characters[7], SrVec(-79, 0, -95))
    setFacing(characters[7], 162)
    setPos2D(characters[8], SrVec(-80, 0, -27))
    setFacing(characters[8], 154)
    setPos2D(characters[9], SrVec(-79, 0, 37))
    setFacing(characters[9], 154)


# start the simulation
sim.start()
for i in range(0, len(characters)):
    chrName = characters[i]
    if 'doctor' in chrName:
        bml.execBML(chrName, '<body posture="LHandOnHip_Motex"/>')
    elif 'brad' in chrName:
        bml.execBML(chrName, '<body posture="ChrUtah_Idle003"/>')
bml.execBML(chrName, '<saccade mode="listen"/>')

sceneSetup()

tenChrSetup()

# this starts the chain reaction of "hot potato"
t()

sim.resume()


# =====================================

def masterReset():
    cmd = '<sbm:reach sbm:action="put-down" sbm:target-pos=' + vec2str(dummy.getPosition()) + ' sbm:reach-finish="true"/>'
    bml.execBML(giver, cmd)
    bml.execBML(taker, cmd)
    global state
    state = IDLE


def mr():
    masterReset()


def swap():
    global giver
    global taker
    temp = giver
    giver = taker
    taker = temp


def alternate():
    global giver
    global taker
    global chrLocator
    numChr = len(characters)
    chrLocator = (chrLocator + 1) % numChr
    giver = characters[chrLocator]
    nextLocator = (chrLocator + 1) % numChr
    taker = characters[nextLocator]
