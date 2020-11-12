
scene.addAssetPath('mesh', 'mesh')
scene.addAssetPath("script", "behaviorsets")
scene.addAssetPath('script', 'scripts')
print 'Loading assets...'
scene.loadAssets()
print 'Done loading assets.'

motionsLoaded = []
modelsLoaded = []

# ussage: character name, character type, 
def setupCharacter(name, charType, customPrefix, voiceType):
	global motionsLoaded
	global modelsLoaded
	
	found = False
	voice = ""
	scale = 1.0
	if charType == "ChrBrad":
		prefix = "ChrBrad"
		voice = "male"
		found = True
		scale = .01
	elif charType == "ChrRachel":
		prefix = "ChrRachel"
		voice = "female"
		found = True	
		scale = .01		
	elif charType == "ChrAli":
		prefix = "ChrGenericHeadMleAdult03"
		voice = "male"
		found = True
	elif charType == "ChrAlyssa":
		prefix = "ChrGenericHeadFmlAdult02"
		voice = "female"
		found = True
	elif charType == "ChrAmity":
		prefix = "ChrGenericHeadFmlAdult01"
		voice = "female"
		found = True	
	elif charType == "ChrCathy":
		prefix = "ChrGenericHeadFmlAdult01"
		voice = "female"
		found = True	
	elif charType == "ChrChuck":
		prefix = "ChrGenericHeadMleAdult03"
		voice = "male"
		found = True	
	elif charType == "ChrDan":
		prefix = "ChrGenericHeadMleAdult03"
		voice = "male"
		found = True	
	elif charType == "ChrHarrison":
		prefix = "ChrGenericHeadMleAdult01"
		voice = "male"
		found = True
	elif charType == "ChrJavier":
		prefix = "ChrJavier"
		voice = "male"
		found = True	
		scale = .01
	elif charType == "ChrJin":
		prefix = "ChrGenericHeadMleAdult02"
		voice = "male"
		found = True	
	elif charType == "ChrJohn":
		prefix = "ChrGenericHeadMleAdult03"
		voice = "male"
		found = True	
	elif charType == "ChrLindsay":
		prefix = "ChrGenericHeadFmlAdult02"
		voice = "female"
		found = True
	elif charType == "ChrMaria":
		prefix = "ChrGenericHeadFmlAdult03"
		voice = "female"
		found = True		
	elif charType == "ChrMaxine":
		prefix = "ChrMaxine"
		voice = "female"
		found = True
	elif charType == "ChrPete":
		prefix = "ChrGenericHeadMleAdult01"
		voice = "male"
		found = True	
	elif charType == "ChrRosie":
		prefix = "ChrGenericHeadFmlTeen01"
		voice = "female"
		found = True	
	elif charType == "ChrSara":
		prefix = "ChrGenericHeadFmlAdult01"
		voice = "female"
		found = True
	elif charType == "ChrShirley":
		prefix = "ChrGenericHeadFmlAdult01"
		voice = "female"
		found = True
	elif charType == "ChrSofia":
		prefix = "ChrGenericHeadFmlAdult01"
		voice = "female"
		found = True
	elif charType == "ChrSteve":
		prefix = "ChrGenericHeadMleAdult03"
		voice = "male"
		found = True	
	elif charType == "ChrTessa":
		prefix = "ChrGenericHeadFmlAdult03"
		voice = "female"
		found = True
	elif charType == "ChrWes":
		prefix = "ChrGenericHeadMleAdult01"
		voice = "male"
		found = True	



	if not found:
		if False: #customPrefix == "":
			print "Character " + charType + " is not known. No character will be set up."
			return
		else:
			prefix = customPrefix
			voice = voiceType
	
	print "Now loading motion assets"
	motionsFound = False
	for m in range(0, len(motionsLoaded)):
		if (motionsLoaded[m] == prefix):
			motionsFound = True
	if (not motionsFound):
		scene.loadAssetsFromPath("motions/" + prefix)
		motionsLoaded.append(prefix)
	print "Now loading model assets"
	modelsFound = False
	for m in range(0, len(modelsLoaded)):
		if (modelsLoaded[m] == prefix):
			modelsFound = True
	if (not modelsFound):
		scene.loadAssetsFromPath("mesh/" + charType)
		modelsLoaded.append(charType)

	# Set up joint map 
	print 'Setting up the joint map.'
	zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
	if zebra2Map is None:
		scene.run('zebra2-map.py')
		zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
	skeleton = scene.getSkeleton(prefix + ".sk")
	zebra2Map.applySkeleton(skeleton)
	zebra2Map.applyMotionRecurse("motions/" + prefix)
	
	# Establish lip syncing data set
	print 'Establishing lip syncing data set'
	lipsyncSets = scene.getDiphoneManager().getDiphoneMapNames()
	lipSyncSetFound = False
	for i in range(0, len(lipsyncSets)):
		if (lipsyncSets[i] == "default"):
			lipSyncSetFound = True
	if (not lipSyncSetFound):
		scene.run('init-diphoneDefault.py')
	
	# Set up face definition
	print 'Setting up face definition'
	facedef = scene.createFaceDefinition(name)
	if prefix == "ChrBrad" or prefix == "ChrRachel":
		facedef.setFaceNeutral(prefix + '@face_neutral')
	else:
		facedef.setFaceNeutral(prefix + '@neutral')
	facedef.setAU(1,  "left",  prefix + "@001_inner_brow_raiser_lf")
	facedef.setAU(1,  "right", prefix + "@001_inner_brow_raiser_rt")
	facedef.setAU(2,  "left",  prefix + "@002_outer_brow_raiser_lf")
	facedef.setAU(2,  "right", prefix + "@002_outer_brow_raiser_rt")
	facedef.setAU(4,  "left",  prefix + "@004_brow_lowerer_lf")
	facedef.setAU(4,  "right", prefix + "@004_brow_lowerer_rt")
	facedef.setAU(5,  "both",  prefix + "@005_upper_lid_raiser")
	facedef.setAU(6,  "both",  prefix + "@006_cheek_raiser")
	facedef.setAU(7,  "both",  prefix + "@007_lid_tightener")
	facedef.setAU(10, "both",  prefix + "@010_upper_lip_raiser")
	facedef.setAU(12, "left",  prefix + "@012_lip_corner_puller_lf")
	facedef.setAU(12, "right", prefix + "@012_lip_corner_puller_rt")
	if (charType != "ChrBrad" and charType != "ChrRachel"):
		facedef.setAU(14, "both",  prefix + "@014_dimpler")
		facedef.setAU(15, "both",  prefix + "@015_lip_corner_depressor")
		facedef.setAU(17, "both",  prefix + "@017_chin_raiser")
		facedef.setAU(18, "both",  prefix + "@018_lip_pucker")
		facedef.setAU(20, "both",  prefix + "@020_lip_stretcher")
		facedef.setAU(23, "both",  prefix + "@023_lip_tightener")
		facedef.setAU(24, "both",  prefix + "@024_lip_pressor")

	facedef.setAU(25, "both",  prefix + "@025_lips_part")
	facedef.setAU(26, "both",  prefix + "@026_jaw_drop")
	facedef.setAU(45, "left",  prefix + "@045_blink_lf")
	facedef.setAU(45, "right", prefix + "@045_blink_rt")

	facedef.setViseme("open",    prefix + "@open")
	facedef.setViseme("W",       prefix + "@W")
	facedef.setViseme("ShCh",    prefix + "@ShCh")
	facedef.setViseme("PBM",     prefix + "@PBM")
	facedef.setViseme("FV",      prefix + "@FV")
	facedef.setViseme("wide",    prefix + "@wide")
	facedef.setViseme("tBack",   prefix + "@tBack")
	facedef.setViseme("tRoof",   prefix + "@tRoof")
	facedef.setViseme("tTeeth",  prefix + "@tTeeth")

	
	print 'Adding character into scene'
	# Set up Brad
	character = scene.createCharacter(name, '')
	#skeleton = scene.createSkeleton(charType + ".dae")
	skeleton = scene.createSkeleton(prefix + ".sk")
	character.setSkeleton(skeleton)
	# Set position
	position = SrVec(0, 0, 0)
	character.setPosition(position)
	# Set facing direction
	facing = SrVec(0, 0, 0)
	character.setHPR(facing)
	# Set face definition
	character.setFaceDefinition(facedef)
	# Set standard controller
	character.createStandardControllers()
	# Deformable mesh
	character.setVec3Attribute('deformableMeshScale', scale, scale, scale)
	character.setStringAttribute('deformableMesh', charType + ".dmb")

	# Lip syncing diphone setup
	character.setStringAttribute('lipSyncSetName', 'default')
	character.setBoolAttribute('usePhoneBigram', True)
	
	character.setVoice("local")
	if voice == 'female':
		character.setVoiceCode("Katherine")
		addBehavior(name, "Gestures")

	elif voice == 'male':
		character.setVoiceCode("Star")
		addBehavior(name, "Gestures")
	
	character.setStringAttribute("displayType", "mesh")

scene.setBoolAttribute("internalAudio", True)		

def addBehavior(name, behavior):
	scene.run('BehaviorSet' + behavior + '.py')
	setupBehaviorSet()
	retargetBehaviorSet(name)
