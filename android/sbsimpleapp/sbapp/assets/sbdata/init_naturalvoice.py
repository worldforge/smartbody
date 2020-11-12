from VH import *
import math

import sys
sys.path.append("/sdcard/vhdata/pythonlibs")
print sys.modules.keys()

def saySomething(actor, message):
	bmlMsg = androidEngine.getNonverbalBehavior(message)
	print bmlMsg
	bml.execXML(actor, bmlMsg)
		
class MyVHEngine(VHEngine):
	isFlat = False
	question = 0
	repeat = 1
	
	questions = ["books", "sports", "home", "travel", "bother", "food", "excited", "favorite", "feelings", "furious", "guilty", "happy", "hate", "lifepath", "music", "personality", "proud",  "survive",  "unhappy", "appearance", "awful", "depressed", "death", "thanks1"]
	
	def eventInit(self):
		print "Starting VH engine..."		
		self.initTextToSpeechEngine('cerevoice') # initialize tts engine for speech synthesis
	
	def eventInitUI(self):	
		self.setWidgetProperty('button1',1, 'Press and hold To Speak')
		self.setWidgetProperty('button2',-1, 'Speak 2')
		self.setWidgetProperty('button3',-1, 'Speak 3')
		self.setWidgetProperty('button4',-1, 'Speak 4')
		self.setWidgetProperty('speak_button',-1, 'Press To Speak')
		self.setWidgetProperty('exit_button',-1, 'Exit button')
		
		self.setBackgroundImage('/sdcard/vhdata/office2.png')
		self.setWidgetProperty('topic_text',1, 'chat with me')
		self.setWidgetProperty('program_text',1, 'chat with me') 

	
	def eventButtonTouch(self, btnName, btnAction):
		if btnName == 'button1':
			if btnAction == 0: # ACTION_DOWN
				print "BUTTON DOWN"
				if self.isVoiceRecognitionListening():
					print "Voice recognition already active, not starting another one."
				else:
					print 'startVoiceRecognition'
					self.startVoiceRecognition()
					self.setWidgetProperty('button1',1, 'Release to Stop')
					bml.execBML('ChrRachel', self.getBehavior("half_nod", 2, 4))
			elif btnAction == 1: # ACTION_UP
				print "BUTTON Up"
				if not self.isVoiceRecognitionListening():
					print "Voice recognition is not already active, ignoring stop voice recognition request..."
				else:
					print 'stopVoiceRecognition'
					self.stopVoiceRecognition()
					self.setWidgetProperty('button1',1, 'Press and hold To Speak')
				
	def eventVoiceRecognition(self, success, recogText):
		if success:
			print 'pyscript voiceRecognitionSuccess : ' + recogText			
			self.question = self.question + 1
			if self.question >= len(self.questions):
				self.question = 0
			txt = "t_" + self.questions[self.question] + ".xml"
			scene.command("bml char ChrRachel file ./Sounds/" + txt)
		else:
			txt = "repeat" + str(self.repeat) + ".xml"
			self.repeat = self.repeat + 1
			if self.repeat > 3:
				self.repeat = 1
			scene.command("bml char ChrRachel file ./Sounds/" + txt)
			print 'pyscript voiceRecognitionFail.'

	def eventButtonClick(self, btnName):
		print "button clicked"

	def eventDialogButton(self, dialogName, action, message):
		print 'pyscript dialogButtonEvent'
		if dialogName == 'exitBox':
			if action == 1:
				self.exitApp()
				
androidEngine = MyVHEngine()
setVHEngine(androidEngine)		
	
scene.addAssetPath("script", "scripts")
scene.addAssetPath("motion", "characters")
scene.setBoolAttribute("internalAudio", True)
scene.run("camera.py")
scene.run("light.py")
scene.run("setupCharacter.py")
setupCharacter("ChrRachel", "ChrAmity", "", "")
scene.addAssetPath("audio", "Sounds")
character = scene.getCharacter("ChrRachel")
character.setStringAttribute("voice", "audiofile")
character.setStringAttribute("voiceCode", ".")
bml.execBML('ChrRachel', '<body posture="ChrBrad@Idle01"/>')
bml.execBML('ChrRachel', '<saccade mode="listen"/>')
sim.start()

# start the first utterance
scene.command('bml char ChrRachel file ./Sounds/intro_age_1.xml')
