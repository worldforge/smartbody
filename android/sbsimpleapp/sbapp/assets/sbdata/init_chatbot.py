from VH import *
import math

import sys
sys.path = ["/sdcard/vhdata/pythonlibs", "/sdcard/vhdata/pythonlibs/site-packages", "/sdcard/vhdata"]
print sys.path
print sys.modules.keys()

import aiml

from random import randint


	
class MyVHEngine(VHEngine):
	isFlat = False
	
	# ready the AIML-based chatbot
	chatbot = aiml.Kernel()
	aimlLocation = "/sdcard/vhdata/alice/"
	
	def eventInit(self):
		print "Starting VH engine..."		
		self.initTextToSpeechEngine('cerevoice') # initialize tts engine for speech synthesis
		
		self.chatbot.setBotPredicate("name", "Lindsay")
		print "Loading AIML files..."
		# there are two ways to load AIML files: directly, or from a .brn file
		# since it takes a couple of minutes to load the .aiml files, use the .brn file instead
		self.chatbot.bootstrap(self.aimlLocation + "alice_brain.brn")
		# if you want to load or modify the AIML files, comment out the bootstrap command above and uncomment the section below
		# files from: https://code.google.com/p/aiml-en-us-foundation-alice/wiki/AIMLFileLoadingOrder
		'''
		self.chatbot.learn(self.aimlLocation + "reduction0.safe.aiml")
		self.chatbot.learn(self.aimlLocation + "reduction1.safe.aiml")
		self.chatbot.learn(self.aimlLocation + "reduction2.safe.aiml")
		self.chatbot.learn(self.aimlLocation + "reduction3.safe.aiml")
		self.chatbot.learn(self.aimlLocation + "reduction4.safe.aiml")
		self.chatbot.learn(self.aimlLocation + "reductions-update.aiml")
		self.chatbot.learn(self.aimlLocation + "mp0.aiml")
		self.chatbot.learn(self.aimlLocation + "mp1.aiml")
		self.chatbot.learn(self.aimlLocation + "mp2.aiml")
		self.chatbot.learn(self.aimlLocation + "mp3.aiml")
		self.chatbot.learn(self.aimlLocation + "mp4.aiml")
		self.chatbot.learn(self.aimlLocation + "mp5.aiml")
		self.chatbot.learn(self.aimlLocation + "mp6.aiml")
		self.chatbot.learn(self.aimlLocation + "ai.aiml")
		self.chatbot.learn(self.aimlLocation + "alice.aiml")
		self.chatbot.learn(self.aimlLocation + "astrology.aiml")
		self.chatbot.learn(self.aimlLocation + "atomic.aiml")
		self.chatbot.learn(self.aimlLocation + "badanswer.aiml")
		self.chatbot.learn(self.aimlLocation + "biography.aiml")
		self.chatbot.learn(self.aimlLocation + "bot.aiml")
		self.chatbot.learn(self.aimlLocation + "bot_profile.aiml")
		self.chatbot.learn(self.aimlLocation + "client.aiml")
		self.chatbot.learn(self.aimlLocation + "client_profile.aiml")
		self.chatbot.learn(self.aimlLocation + "computers.aiml")
		self.chatbot.learn(self.aimlLocation + "continuation.aiml")
		self.chatbot.learn(self.aimlLocation + "date.aiml")
		self.chatbot.learn(self.aimlLocation + "default.aiml")
		self.chatbot.learn(self.aimlLocation + "drugs.aiml")
		self.chatbot.learn(self.aimlLocation + "emotion.aiml")
		self.chatbot.learn(self.aimlLocation + "food.aiml")
		self.chatbot.learn(self.aimlLocation + "geography.aiml")
		self.chatbot.learn(self.aimlLocation + "gossip.aiml")
		self.chatbot.learn(self.aimlLocation + "history.aiml")
		self.chatbot.learn(self.aimlLocation + "humor.aiml")
		self.chatbot.learn(self.aimlLocation + "imponderables.aiml")
		self.chatbot.learn(self.aimlLocation + "inquiry.aiml")
		self.chatbot.learn(self.aimlLocation + "interjection.aiml")
		self.chatbot.learn(self.aimlLocation + "iu.aiml")
		self.chatbot.learn(self.aimlLocation + "knowledge.aiml")
		self.chatbot.learn(self.aimlLocation + "literature.aiml")
		self.chatbot.learn(self.aimlLocation + "loebner10.aiml")
		self.chatbot.learn(self.aimlLocation + "money.aiml")
		self.chatbot.learn(self.aimlLocation + "movies.aiml")
		self.chatbot.learn(self.aimlLocation + "music.aiml")
		self.chatbot.learn(self.aimlLocation + "numbers.aiml")
		self.chatbot.learn(self.aimlLocation + "personality.aiml")
		self.chatbot.learn(self.aimlLocation + "phone.aiml")
		self.chatbot.learn(self.aimlLocation + "pickup.aiml")
		self.chatbot.learn(self.aimlLocation + "politics.aiml")
		self.chatbot.learn(self.aimlLocation + "primeminister.aiml")
		self.chatbot.learn(self.aimlLocation + "primitive-math.aiml")
		self.chatbot.learn(self.aimlLocation + "psychology.aiml")
		self.chatbot.learn(self.aimlLocation + "religion.aiml")
		self.chatbot.learn(self.aimlLocation + "salutations.aiml")
		self.chatbot.learn(self.aimlLocation + "science.aiml")
		self.chatbot.learn(self.aimlLocation + "sex.aiml")
		self.chatbot.learn(self.aimlLocation + "sports.aiml")
		self.chatbot.learn(self.aimlLocation + "stack.aiml")
		self.chatbot.learn(self.aimlLocation + "stories.aiml")
		self.chatbot.learn(self.aimlLocation + "that.aiml")
		self.chatbot.learn(self.aimlLocation + "update-mccormick.aiml")
		self.chatbot.learn(self.aimlLocation + "update1.aiml")
		self.chatbot.learn(self.aimlLocation + "wallace.aiml")
		self.chatbot.learn(self.aimlLocation + "xfind.aiml")
		'''
		
		print "Finished loading AIML files..."
		
	def eventInitUI(self):	
		self.setWidgetProperty('button1',1, 'Press and Hold to Speak')
		self.setWidgetProperty('button2',-1, 'Speak 2')
		self.setWidgetProperty('button3',-1, 'Speak 3')
		self.setWidgetProperty('button4',-1, 'Speak 4')
		self.setWidgetProperty('speak_button',-1, 'Press To Speak')
		self.setWidgetProperty('exit_button',-1, 'Exit button')
		
		self.setBackgroundImage('/sdcard/vhdata/office2.png')
		self.setWidgetProperty('topic_text',1, 'Alice: tell me your problems')
		self.setWidgetProperty('program_text',1, 'Alice') 

	
	def eventButtonTouch(self, btnName, btnAction):
		if btnName == 'button1':
			if btnAction == 0: # ACTION_DOWN
				print 'startVoiceRecognition'
				self.startVoiceRecognition()
				self.setWidgetProperty('button1',1, 'Release to Stop')
			elif btnAction == 1: # ACTION_UP
				print 'stopVoiceRecognition'
				self.stopVoiceRecognition()
				self.setWidgetProperty('button1',1, 'Press and hold To Speak')
				
	def eventVoiceRecognition(self, success, recogText):
		if success:
			print 'pyscript voiceRecognitionSuccess : ' + recogText			
			self.saySomething('ChrLindsay', recogText)
		else:
			print 'pyscript voiceRecognitionFail.'

	def eventButtonClick(self, btnName):
		charName = 'ChrLindsay'
		if btnName == 'exit_button':
			print 'click exit_button'
			self.createDialogBox('exitBox','Exit Program?', 'Are you sure you want to exit the program?', False)

	def eventDialogButton(self, dialogName, action, message):
		print 'pyscript dialogButtonEvent'
		if dialogName == 'exitBox':
			if action == 1:
				self.exitApp()

	def saySomething(self, actor, message):
		aliceResponse = self.chatbot.respond(message)
		bmlMsg = androidEngine.getNonverbalBehavior(aliceResponse)
		print bmlMsg
		bml.execXML(actor, bmlMsg)				
		# change affect
		startTime = 4
		endTime = 10
		r = randint(0,9)
		if r == 0:
			behavior = self.getBehavior("social_smile", startTime, endTime)
		elif r == 1:
			behavior = self.getBehavior("brow_raise", startTime, endTime)
		elif r == 2:
			behavior = self.getBehavior("brow_frown", startTime, endTime)
		elif r == 3:
			behavior = self.getBehavior("concern", startTime, endTime)
		elif r == 4:
			behavior = self.getBehavior("gaze_aversion_left", startTime, endTime)
		elif r == 5:
			behavior = self.getBehavior("gaze_aversion_right", startTime, endTime)
		elif r == 6:
			behavior = self.getBehavior("half_nod_up", startTime, endTime)
		elif r == 7:
			behavior = self.getBehavior("big_smile", startTime, endTime)
		elif r == 8:
			behavior = self.getBehavior("angry", startTime, endTime)
		elif r == 9:
			behavior = self.getBehavior("mouth_open", 4, 6)
		
		print "Now running: " + behavior
		bml.execBML(actor, behavior)
androidEngine = MyVHEngine()
setVHEngine(androidEngine)		
	
scene.addAssetPath("script", "scripts")
scene.addAssetPath("motion", "characters")
scene.setBoolAttribute("internalAudio", True)
scene.run("camera.py")
scene.run("light.py")
scene.run("setupCharacter.py")
setupCharacter("ChrLindsay", "ChrLindsay", "", "")
bml.execBML('ChrLindsay', '<body posture="ChrBrad@Idle01"/>')
bml.execBML('ChrLindsay', '<saccade mode="listen"/>')

behaviors = androidEngine.getBehaviorNames()
for b in range(0, len(behaviors)):
	print behaviors[b]
sim.start()