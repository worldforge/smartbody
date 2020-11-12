# Google Cardboard demo
# also uncomment the Cardboard rendering line in setup.py

from VH import *
import math

import sys
sys.path = ["/sdcard/vhdata/pythonlibs", "/sdcard/vhdata/pythonlibs/site-packages", "/sdcard/vhdata"]
print sys.path
print sys.modules.keys()



def saySomething(actor, message):
	bmlMsg = androidEngine.getNonverbalBehavior(message)
	bml.execXML(actor, bmlMsg)
		
class MyVHEngine(VHEngine):
	buttonPressed = False
	
	isFlat = False
	def eventInit(self):
		print "Starting VH engine..."
		self.initTextToSpeechEngine('cerevoice') # initialize tts engine for speech synthesis
	
	def eventButtonClick(self, btnName):
		if btnName == 'cardboard_button':
			if (self.buttonPressed is True):
				self.stopVoiceRecognition()
				self.buttonPressed = False
			else:
				self.startVoiceRecognition()
				self.buttonPressed = True
				
	def eventVoiceRecognition(self, success, recogText):
		if success:
			print 'pyscript voiceRecognitionSuccess : ' + recogText			
			saySomething('ChrAlyssa', recogText)
		else:
			print 'pyscript voiceRecognitionFail.'
				
androidEngine = MyVHEngine()
setVHEngine(androidEngine)		
	
scene.addAssetPath("script", "scripts")
scene.addAssetPath("motion", "characters")
scene.setBoolAttribute("internalAudio", True)
scene.run("camera.py")
scene.run("light.py")
scene.run("setupCharacter.py")

# create a few characters
characters = ["ChrJin", "ChrAlyssa", "ChrHarrison", "ChrTessa"]
for c in range(0, len(characters)):
	setupCharacter(characters[c], characters[c], "", "")
	bml.execBML(characters[c], '<body posture="ChrBrad@Idle01"/>')
	bml.execBML(characters[c], '<saccade mode="listen"/>')
	addBehavior(characters[c], "MaleMocapLocomotion")
	position = SrVec(0 + 1 * c, 0, -1)
	scene.getCharacter(characters[c]).setPosition(position)

bml.execBML('ChrJin', '<blend name="mocapLocomotion" x="2.3" y="50"/>')

sim.start()