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
	questionText = ""
	answerText = ""
	def eventInit(self):
		print "Starting VH engine..."
		
		self.initTextToSpeechEngine('cerevoice') # initialize tts engine for speech synthesis

	def eventInitUI(self):
		self.setWidgetProperty('button1',1, 'Play Video')
		self.setWidgetProperty('button2',1, 'Stop Video')
		self.setWidgetProperty('button3',-1, '')
		self.setWidgetProperty('button4',-1, '')
		self.setWidgetProperty('speak_button',-1, '')
		self.setWidgetProperty('exit_button',-1, '')
		
		self.setWidgetProperty('topic_text',-1, '')
		self.setWidgetProperty('program_text',-1, '') 
	
	def eventButtonClick(self, btnName):
		if btnName == 'button1':
			print 'click button1'
			self.playVideo('videoView','/sdcard/vhdata/Richard_firstpass.wmv', True)
		elif btnName == 'button2':
			print 'click button2'
			self.stopVideo('videoView')
			
	def eventVideoCompletion(self, videoViewName):
		if videoViewName == 'videoView':
			self.createDialogBox('videoDoneBox','Video Finish', 'Video playback is done. Do you want to replay it ?', False)
		return result
		
	def eventDialogButton(self, dialogName, action, textMsg):
		#print 'pyscript dialogButtonEvent'
		if dialogName == 'videoDoneBox':
			if action == 1:
				self.playVideo('videoView','/sdcard/vhdata/Richard_firstpass.wmv', True)
				
androidEngine = MyVHEngine()
setVHEngine(androidEngine)		
	
sim.start()