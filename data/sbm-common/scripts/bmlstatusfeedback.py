class FeedbackHandler(EventHandler):
	def executeAction(self, ev):
		params = ev.getParameters()
		# uncomment the following line when debugging
		#print params
		tok = params.split()
		numTokens = len(tok)
		if (len(tok) >= 1):
			if (tok[0] == "syncpointprogress"):
				if (numTokens < 5):
					return
				# replace the $time with the current time
				t = sim.getTime()
				tok[4] = str(t)
				s = "<syncpointprogress characterId=\"" + tok[1] + "\" id=\"" + tok[2] + ":" + tok[3] + "\" time=\"" + tok[4] + "\"/>"
				print s
				vhmsg2(s, "")
			elif (tok[0] == "blockstart"):
				if (numTokens < 4):
					return
				t = sim.getTime()
				s = "<blockstart characterId=\"" + tok[1] + "\" id=\"" + tok[2] + "\" time=\"" + tok[3] + "\"/>"
				print s
				vhmsg2(s, "")
			elif (tok[0] == "blockend"):
				if (numTokens < 4):
					return
				t = sim.getTime()
				s = "<blockend characterId=\"" + tok[1] + "\" id=\"" + tok[2] + "\" time=\"" + tok[3] + "\"/>"
				print s
				vhmsg2(s, "")
				
				
		

f = FeedbackHandler()
em = scene.getEventManager()
em.addEventHandler("bmlstatus", f)
