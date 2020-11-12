print "|--------------------------------------------|"
print "|  data/sbm-common/scripts/hotpotato.py      |"
print "|Object passing between agents - David Huang |"
print "|--------------------------------------------|"

# =====================================	
# FOLLOW THESE STEPS TO USE THIS SCRIPT
'''
# 1. load the script
scene.run("hot-potato.py")

# 2. set up names for agents and object
giver = 'doctor'
taker = 'brad'
obj = 'box'

# 3. make sure object is somewhere can be picked up
dummyPos = offerHandPos(giver, taker)
dummy.setPosition(dummyPos)
target = scene.getPawn(obj)
target.setPosition(dummyPos)

# 4. this triggers the object passing, from giver to taker
# giver will pickup object if not already attached in hand
# taker will walk towards giver when giver "offers" the obj
# make sure object can be grabbed by giver (e.g. not in someone's hand)
giverHandToTaker(giver, taker, obj)
# or simply
t()

# swap the giver and taker, and run it again:
swap()
t()

# taker release the object:
reset()
# or simply
r()

# if agent(s) got stuck in the middle of the state, use this to reset:
masterReset()
# or simply
mr()

# you can also directly specify giver, taker, obj as follows:
#giverHandToTaker('elder', 'utah', 'box')
#giverHandToTaker('brad', 'doctor', 'box')

# optional: enable pathFollowingMode
giverChr = scene.getCharacter(giver)
giverChr.setBoolAttribute("steering.pathFollowingMode", True)
takerChr = scene.getCharacter(taker)
takerChr.setBoolAttribute("steering.pathFollowingMode", True)
'''

# =====================================	
# for state machine
IDLE = 0
PASSING = 1
LOCOMOTION = 2

state = IDLE

# =====================================	
# define constant variables

offerHandPosHeightRatio	= 0.75
offerHandBodyDistHeightRatio = 0.3
#offerHandIsLeft = True
#takerHandIsLeft = True
takerGiverDistHeightRatio = 0.3
# field of view of human eye is 95 degrees away from nose (wiki Human eye)
facingThreshold = 95

# add randomness when offer object to pickup (location and delay)
import random

# ========= UTILITY FUNCTIONS =========
def mDBGINFO(txt):
	print txt
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
	return "\""+str(x)+" "+str(y)+" "+str(z)+"\""

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
	return SrVec(x,y,z)

def vecPlus(vec1, vec2):
	x = vec1.getData(0) + vec2.getData(0)
	y = vec1.getData(1) + vec2.getData(1)
	z = vec1.getData(2) + vec2.getData(2)
	return SrVec(x,y,z)	

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
	if(val >= 0):
		return val
	else:
		return -val

def getPos(chrName):
	chr = scene.getCharacter(chrName)
	return chr.getPosition()
	
def getPos2D(chrName):
	chr = scene.getCharacter(chrName)
	pos = chr.getPosition()
	pos.setData(1, 0) # clear Y
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


# set up target (box) and a dummy object
if scene.getPawn("box") == None:
	target = scene.createPawn("box")
	target.setStringAttribute("collisionShape", "box")
	target.getAttribute("collisionShapeScale").setValue(SrVec(5,5,5))

if scene.getPawn("dummyobj") == None:
	dummy = scene.createPawn("dummyobj")

evtMgr = scene.getEventManager()

# =====================================	
# offer hand angle based on taker's location: -180~180->-90~90 non-linear mapping
def offerAngleBasedOnTakerPos(x):
	negX = False
	if x < 0:
		x = -x
		negX = True
	res = 0.00000010139*x*x*x*x - 0.000031499*x*x*x - 0.0013*x*x + 1.1619*x
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
	#mDBGINFO( 'taker's direction in giver's local coord (deg): ' + str(x) )
	return x

# position to place object for taker (giver's hand position)
def offerHandPos(giverName, takerName):
	giverChr = scene.getCharacter(giverName)
	giverPos = giverChr.getPosition()
	giverPos.setData(1, 0)
	giverHeight = giverChr.getHeight()
	offerHeight = giverHeight * offerHandPosHeightRatio
	# add randomness to offer hand height
	offerHeight = offerHeight * (1 - random.random()/5)
	offerDist = giverHeight * offerHandBodyDistHeightRatio
	# add randomness to offer hand distance to giver body
	offerDist = offerDist * (1 - random.random()/5)
	giverHPR = giverChr.getHPR()
	giverHeading = giverHPR.getData(0)
	#mDBGINFO( 'giverHeading: ' + str(giverHeading) )
	takerChr = scene.getCharacter(takerName)
	takerPos = takerChr.getPosition()
	takerPos.setData(1, 0)
	x = toDegree(vec2Yaw(vecSub(takerPos, giverPos)))
	x = normalizeAngleDeg(x - giverHeading)
	#mDBGINFO( 'taker direction relative to giver: ' + str(x) )
	offset2D = SrVec(0, offerHeight, offerDist)
	offerHandDir = offerAngleBasedOnTakerPos(x)
	#mDBGINFO( 'offer hand direction relative to giver: ' + str(offerHandDir) )
	offerHandDir = normalizeAngleDeg(giverHeading + offerHandDir)
	#mDBGINFO( 'offer hand direction in world coord: ' + str(offerHandDir) )
	offset2D.rotY(toRadian(offerHandDir))
	offerPos = vecPlus(giverPos, offset2D);
	return offerPos
# =====================================	



# Just run this function after setting up giver, taker, obj, state
def giverHandToTaker(giverName, takerName, objName):
	global giver
	global taker
	global obj
	giver = giverName
	taker = takerName
	obj = objName
	reachMgr = scene.getReachManager()
	giverReach = reachMgr.getReach(giverName)
	if giverReach.isPawnAttached(objName)==True:
		# giver "offer" give-out
		dummy.setPosition(offerHandPos(giverName, takerName))
		cmd = '<sbm:reach sbm:action="put-down" sbm:target-pos=' + vec2str(dummy.getPosition()) + ' sbm:reach-finish="false"/>'
		cmd = cmd + '<gaze sbm:joint-range="EYES NECK" target=\"' + takerName + '\"/>'
		bml.execBML(giverName, cmd)
	else:
		# giver grabs the object first
		type = '\"right\"'
		if takerDirRelativeToGiver(giverName, takerName) >= 0:
			type = '\"left\"'
		cmd = '<sbm:reach sbm:action=\"pick-up\" sbm:use-locomotion="true" sbm:reach-finish=\"true\" sbm:reach-type=' +  type + ' target=\"' + obj + '\"/>'
		cmd = cmd + '<gaze sbm:joint-range=\"EYES NECK\" target=\"dummyobj\"/>'
		bml.execBML(giverName, cmd)
		# taker gaze at target
		bml.execBML(takerName, '<gaze sbm:joint-range="EYES NECK" target="dummyobj"/>')
	global state
	state = PASSING

def t():
	giverHandToTaker(giver, taker, obj)
	
def reset():
	reachMgr = scene.getReachManager()
	giverReach = reachMgr.getReach(giver)
	if giverReach.isPawnAttached(obj)==True:
		dummy.setPosition(offerHandPos(giver, taker))
		cmd = '<sbm:reach sbm:action="put-down" sbm:target-pos=' + vec2str(dummy.getPosition()) + ' sbm:reach-finish="true"/>'
		bml.execBML(giver, cmd)
	takerReach = reachMgr.getReach(taker)
	if takerReach.isPawnAttached(obj)==True:
		dummy.setPosition(offerHandPos(taker, giver))
		cmd = '<sbm:reach sbm:action="put-down" sbm:target-pos=' + vec2str(dummy.getPosition()) + ' sbm:reach-finish="true"/>'	
		bml.execBML(taker, cmd)
	global state
	state = IDLE

def r():
	reset()

def masterReset():
	cmd = '<sbm:reach sbm:action=\"put-down\" sbm:reach-finish=\"true\"/>'
	bml.execBML(giver, cmd)
	bml.execBML(taker, cmd)
	global state
	state = IDLE
	
def mr():
	masterReset()
	
class ReachingHandler(EventHandler):
	def executeAction(self, ev):
		params = ev.getParameters()
		global giver
		global taker
		global state
		#global offerHandIsLeft
		#global takerHandIsLeft
		mDBGINFO( "[[--------------------------------------------" )
		mDBGINFO( params )
		'''
		if "reach-complete" in params:
			mDBGINFO( "[REACH COMPLETE] " + params )
			if ('bml char '+giver) in params and 'reach-complete: left' in params:
				offerHandIsLeft = True
				mDBGINFO( "[GIVER USING LEFT HAND] " )
			elif ('bml char '+giver) in params and 'reach-complete: right' in params:
				offerHandIsLeft = False
				mDBGINFO( "[GIVER USING RIGHT HAND] " )
			if ('bml char '+taker) in params and 'reach-complete: left' in params:
				takerHandIsLeft = True
				mDBGINFO( "[TAKER USING LEFT HAND] " )
			elif ('bml char '+taker) in params and 'reach-complete: right' in params:
				takerHandIsLeft = False
				mDBGINFO( "[TAKER USING RIGHT HAND] " )
		'''

		if "reach-returned" in params:
			mDBGINFO( "[REACH RETURNED] " + params )
		#if "reach-return" in params:
		#	mDBGINFO( "[REACH RETURN] " + params )
		#if "reach-newTarget" in params:
		#	mDBGINFO( "[REACH NEW TARGET] " + params )


		if "pawn-released" in params:
			mDBGINFO( "[PAWN RELEASED] from " + giver )
			keywd = "bml char " + giver
			if keywd in params:
				#print '[TAKER PICKUP: ' + taker + ']'
				#3 taker pick-up obj
				takerPickup()

		elif "reach-stateNew: Idle" in params:
			mDBGINFO( '[REACHED RETURNED] ' + params )
			keywd = "bml char " + giver
			if keywd in params:
				if "pawn-attached" in params:
					if state == PASSING:
						mDBGINFO( '[GIVE OUT: ' + giver + '] ' + params )
						# giver "offer" give-out
						dummy.setPosition(offerHandPos(giver, taker))
						cmd = '<sbm:reach sbm:action="put-down" sbm:target-pos=' + vec2str(dummy.getPosition()) + ' sbm:reach-finish="false"/>'
						cmd = cmd + '<gaze sbm:joint-range="EYES NECK" target=\"' + taker + '\"/>'
						bml.execBML(giver, cmd)
						# taker gaze at target
						bml.execBML(taker, '<gaze sbm:joint-range="EYES NECK" target="dummyobj"/>')

		elif "pawn-attached" in params:
			keywd = "bml char " + taker
			if keywd in params:
				mDBGINFO( "[PAWN ATTACHED, GIVER PASS TO TAKER] " )
				# giver "offer" give-return
				cmd = '<sbm:reach sbm:action="put-down" sbm:target-pos=' + vec2str(dummy.getPosition()) + ' sbm:reach-finish="true"/>'
				bml.execBML(giver, cmd)
				# taker grab-return
				cmd = '<sbm:reach sbm:action="pick-up" sbm:reach-finish="true" target=\"' + obj + '\"/><gaze sbm:joint-range="EYES NECK" target="dummyobj"/>'
				bml.execBML(taker, cmd)
			keywd = "bml char " + giver
			if keywd in params:
				mDBGINFO( "[PAWN ATTACHED, GIVER GRABBED OBJ] " + params )

		mDBGINFO( "--------------------------------------------]]" )

reachingHdl = ReachingHandler()
evtMgr.addEventHandler("reachNotifier", reachingHdl)

'''
# this was for taker to grab at end of locomotion (before facing adjustment)
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

# this is for taker to grab when facing adjustment ends (after locomotion)
class FacingHandler(EventHandler):
	def executeAction(self, ev):
		params = ev.getParameters()
		global state
		if state == LOCOMOTION:
			if taker in params and 'success' in params:
				state = PASSING
				cmd = '<sbm:reach sbm:action="pick-up" sbm:reach-finish="false" target=\"' + obj + '\"/>'
				# also cancel facing adjustment, otherwise agent keeps on doing the adjustment
				curPos = getPos(taker)
				curPosX = curPos.getData(0)
				curPosZ = curPos.getData(2)
				cmd = cmd + '<locomotion target=\"' + str(curPosX) + ' ' + str(curPosZ) + '\"/>'
				bml.execBML(taker, cmd)
				mDBGINFO( '[FACING ADJUSTMENT FINISHED]' )

facingHdl = FacingHandler()
# this event is sent out here: SteeringAgent::adjustFacingAngle(float angleDiff)
evtMgr.addEventHandler("adjustFacing", facingHdl)		

def takerPickup():
	giverChr = scene.getCharacter(giver)
	giverPos = getPos2D(giver)
	takerPos = getPos2D(taker)
	target = scene.getPawn(obj)
	tgtPos = target.getPosition()
	tgtPos.setData(1, 0)
	chrHeight = giverChr.getHeight()
	walkThreshold = chrHeight * (takerGiverDistHeightRatio-0.15)
	takerFacingCurrent = getFacing(taker)
	vecGiverToTaker = vecSub(giverPos, takerPos)
	giverDirectionToTaker = toDegree(vec2Yaw(vecGiverToTaker))
	facingDist = abs(angDist(takerFacingCurrent, giverDirectionToTaker))
	#mDBGINFO( 'giverDirectionToTaker: ' + str(giverDirectionToTaker) + ' , takerHeading: ' + str(takerFacingCurrent) )
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
	reachDist = vecDist(tgtPos, takerPos)
	print 'walkThreshold: ' + str(walkThreshold) + ' , reachDist: ' + str(reachDist)
	#mDBGINFO( 'chr dist: ' + str(reachDist) + ' ,  facing dist: ' + str(facingDist) )
	if reachDist > walkThreshold or facingDist > facingThreshold:
		global state
		state = LOCOMOTION
		#mDBGINFO( 'locomotion target: ' + vecXZ2str(locomGoalPos) + ' ,  facing: ' + str(takerHeading) )
		cmd = '<locomotion facing=\"' + str(takerHeading) + '\" target=' + vecXZ2str(locomGoalPos) + '/>'
		cmd = cmd + '<gaze sbm:joint-range="EYES NECK" target="dummyobj"/>'
		#mDBGINFO( cmd )
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

# =====================================	
# =====================================
