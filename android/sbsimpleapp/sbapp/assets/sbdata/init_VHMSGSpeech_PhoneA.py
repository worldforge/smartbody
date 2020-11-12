from VH import *
import math

import sys
sys.path = ["/sdcard/vhdata/pythonlibs", "/sdcard/vhdata/pythonlibs/site-packages", "/sdcard/vhdata"]
print sys.path
print sys.modules.keys()

def saySomething(actor, message):
	if scene.getCharacter(actor) != None:
		bmlMsg = androidEngine.getNonverbalBehavior(message)
		bml.execXML(actor, bmlMsg)
		
class MyVHEngine(VHEngine):
	isFlat = False
	def eventInit(self):
		print "Starting VH engine..."		
		self.initTextToSpeechEngine('cerevoice') # initialize tts engine for speech synthesis
	
	def eventInitUI(self):
		self.setWidgetProperty('button1',1, 'Press To Speak')
		self.setWidgetProperty('button2',-1, 'Speak 2')
		self.setWidgetProperty('button3',-1, 'Speak 3')
		self.setWidgetProperty('button4',-1, 'Speak 4')
		self.setWidgetProperty('speak_button',-1, 'Press To Speak')
		self.setWidgetProperty('exit_button',-1, 'Exit App')
		
		self.setBackgroundImage('/sdcard/vhdata/office2.png')
		self.setWidgetProperty('topic_text',1, 'VHMsg Demo')
		self.setWidgetProperty('program_text',1, 'This demo shows the communication between two devices via VHMsg.') 
	
	def eventButtonTouch(self, btnName, btnAction):
		if btnName == 'button1':
			if btnAction == 0: # ACTION_DOWN
				print 'startVoiceRecognition'
				self.startVoiceRecognition()
				self.setWidgetProperty('button1',1, 'Release to Stop')
			elif btnAction == 1: # ACTION_UP
				print 'stopVoiceRecognition'
				self.stopVoiceRecognition()
				self.setWidgetProperty('button1',1, 'Press to Speak')
	def eventVoiceRecognition(self, success, recogText):
		if success:
			print 'pyscript voiceRecognitionSuccess : ' + recogText			
			# text will be spoken by the other mobile device character
			self.send('sb saySomething("ChrJin","' + recogText + '")')
		else:
			print 'pyscript voiceRecognitionFail.'
	def eventButtonClick(self, btnName):
		if btnName == 'exit_button':
			self.createDialogBox('exitBox','Exit Program?', 'Are you sure you want to exit the program?', False)
			
	def eventDialogButton(self, dialogName, action, message):
		#print 'pyscript dialogButtonEvent'
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
setupCharacter("ChrTessa", "ChrTessa", "", "")
bml.execBML('ChrTessa', '<body posture="ChrBrad@Idle01"/>')
bml.execBML('ChrTessa', '<saccade mode="listen"/>')

# connect to an existing ActiveMQ server
androidEngine.connect("172.16.33.27")
if (androidEngine.isConnected()):
	print "Connected!"
else:
	print "Cannot connect to VHMSG server"


sim.start()