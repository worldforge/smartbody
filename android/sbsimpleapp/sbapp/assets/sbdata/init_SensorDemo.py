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
	isFlat = False
	def eventInit(self):
		print "Starting VH engine..."
		self.enableSensor('accelerometer',True)
		self.initTextToSpeechEngine('cerevoice') # initialize tts engine for speech synthesis
	
	def eventInitUI(self):
		self.setWidgetProperty('button1',1, 'Phone is not FLAT or UPRIGHT')
		self.setWidgetProperty('button2',-1, 'Speak 2')
		self.setWidgetProperty('button3',-1, 'Speak 3')
		self.setWidgetProperty('button4',-1, 'Speak 4')
		self.setWidgetProperty('speak_button',-1, 'Press To Speak')
		self.setWidgetProperty('exit_button',-1, 'Exit App')
		self.setBackgroundImage('/sdcard/vhdata/office2.png')
		self.setWidgetProperty('topic_text',1, 'Sensor Demo')
		self.setWidgetProperty('program_text',1, 'This demo shows how use sensors to interact with the virtual human.') 	
	
	def eventStep(self):
		self.updateSensor()
		
	def updateSensor(self):
		sensorVal = self.getAccelerometerValues()
		sx = sensorVal.getData(0)
		sy = sensorVal.getData(1)
		sz = sensorVal.getData(2)
		norm_g = math.sqrt(sx*sx+sy*sy+sz*sz)
		g0 = sx/norm_g
		g1 = sy/norm_g
		g2 = sz/norm_g
		inclination = math.degrees(math.acos(g2))
		str = 'neither'
		charName = 'ChrAmity'
		if inclination < 25 or inclination > 155:
			#print 'device is flat'
			str = 'Phone is FLAT'
			if self.isFlat == False:
				saySomething(charName,'Hey, please pick me up')
				self.isFlat = True
		elif inclination > 50 and inclination < 130:
			#print 'device is upright'
			str = 'Phone is UPRIGHT'
			if self.isFlat == True:
				saySomething(charName,'Ok, thank you. I want to chat with you.')
				self.isFlat = False
		else:
			#print 'device is neither'
			str = 'Phone is not FLAT or UPRIGHT'
		self.setWidgetProperty('button1',1, str)
		
	def eventButtonClick(self, btnName):
		charName = 'ChrAmity'
		if btnName == 'exit_button':
			print 'click exit_button'
			self.createDialogBox('exitBox','Exit Program?', 'Are you sure you want to exit the program?')
	def eventDialogButton(self, dialogName, action):
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
setupCharacter("ChrAmity", "ChrAmity", "", "")
bml.execBML('ChrAmity', '<body posture="ChrBrad@Idle01"/>')
bml.execBML('ChrAmity', '<saccade mode="listen"/>')
sim.start()