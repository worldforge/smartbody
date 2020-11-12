import Cerebella
from Cerebella.NVBG import *

"""
This class define the NVBG python class that override smartbody c++ one
SmartBodyNVBG serves as an connection to the real NVBG to smartbody
- Steps to setup pNVBG for a character: create NVBG, create SmartBodyNVBG, set NVBG to SmartBodyNVBG, set SmartBodyNVBG to character
- Tool setup is in charge of setting up the attributes that can used inside notifyAction function
"""


class SmartBodyNVBG(Nvbg):
    def setNvbg(self, n):
        """
        Necessary
        Set the NVBG to SmartBodyNVBG
        """
        self.nvbg = n

    def reset(self):
        """
        Optional
        Reset NVBG and Pyke
        """
        nvbgCharName = ""
        if hasattr(self, 'nvbg') is False:
            return
        scene.run("resetNVBG.py")

    def notifyAction(self, name):
        """
        Optional
        Override the C++ notifiyAction function.
        """
        if hasattr(self, 'nvbg') is False:
            return

        # print "In notifyAction, attribute is " + name
        if (name == "play dialog"):
            dialogStr = self.getAttribute("dialog").getValue()
            audioStr = self.getAttribute("audio").getValue()
            tagStr = self.getAttribute("tag").getValue()
            if (dialogStr == ""):
                # print "dialog not selected, play default one"
                self.nvbg.speak()
            else:
                if (audioStr == ""):
                    self.nvbg.speak(dialogStr)
                elif (tagStr == ""):
                    self.nvbg.speak(dialogStr, audioStr)
                else:
                    self.nvbg.speak(dialogStr, audioStr, tagStr)
        if (name == "save"):
            dialogStr = self.getAttribute("dialog").getValue()
            audioStr = self.getAttribute("audio").getValue()
            if (dialogStr == ""):
                return
            else:
                if (audioStr == ""):
                    self.nvbg.speakToFile(dialogStr)
                else:
                    self.nvbg.speakToFile(dialogStr, audioStr)
        if (name == "play behavior"):
            selectedBml = self.getAttribute("bml").getValue()
            bml.execBML(self.nvbg.characterName, selectedBml)

        if (name == "reset"):
            self.reset()
        return

    def notifyBool(self, name, val):
        """
        Optional
        Override the C++ notifyBool function.
        """
        if hasattr(self, 'nvbg') is False:
            return

        if (name == "enable"):
            if (val is True):
                print("enabled!")
            else:
                print("disabled!")
        return

    def notifyString(self, name, val):
        """
        Optional
        Override the C++ notifyString function.
        """
        if hasattr(self, 'nvbg') is False:
            return

        if (name == "default behavior"):
            behVec = self.getAttribute("default behavior").getValidValues()
            bmlVec = self.getAttribute("bml hidden").getValidValues()
            for i in range(0, len(behVec)):
                if (behVec[i] == val):
                    bmlStr = bmlVec[i]
                    self.getAttribute("bml").setValue(bmlStr)
                    return

        if (name == "emotion"):
            selectedEmotion = self.getAttribute("emotion").getValue()
            self.nvbg.curEmotion = selectedEmotion
        return

    def executeSpeech(self, character, speechStatus, speechId, speaker):
        """
        Necessary
        Override the C++ executeSpeech function
        Process vrSpeech message used to indicate push-to-talk status
        """
        if hasattr(self, 'nvbg') is False:
            return

        if (speechStatus == "start"):
            self.nvbg.set_push_to_talk(True)

        if (speechStatus == "finished-speaking"):
            self.nvbg.set_push_to_talk(False)
        return True

    def executeSpeechRequest(self, behaviors, types, times, targets, info):
        """
        Necessary
        Override the C++ executeSpeechRequest function
        Process listening feedback request from virtual world
        """
        if hasattr(self, 'nvbg') is False:
            return

        bmlstr, listeners = self.nvbg.process_feedback_virtual(behaviors, types, times, targets, info)
        for i in range(0, len(listeners)):
            bml.execBML(listeners[i], str(bmlstr))
        return True

    def executeEvent(self, character, messageId, state):
        """
        Necessary
        Override the C++ executeEvent function
        Event Handler for bml execution
        Process vrAgentBML message which indicate the status of bml execution
        """
        if hasattr(self, 'nvbg') is False:
            return

        if (state == "start"):
            # print "NVBG executeEvent: start executing msg " + messageId
            return True
        if (state == "end"):
            # print "NVBG executeEvent: end executing msg " + messageId
            self.nvbg.reset_feedback_msgId(character, messageId)
        return True

    def execute(self, character, recipient, messageId, xml):
        """
        Necessary
        Override the C++ execute function
        Execute all xml vrX messages
        """
        if hasattr(self, 'nvbg') is False:
            return

        xmlRoot = ET.XML(xml)
        rootTag = xmlRoot.tag
        ''' processing PML '''
        if (rootTag == "pml"):
            bmlstr = self.nvbg.process_percepts(character, xml)
            if (len(bmlstr) != 0):
                bml.execBML(character, str(bmlstr))
        else:
            actElem = ET.XML(xml)
            bmlElem = actElem.find('bml')
            fmlElem = actElem.find('fml')
            ''' processing BML Speech '''
            if (bmlElem != None):
                bmlstr = self.nvbg.process_speech(character, recipient, messageId, xml)
                if (len(bmlstr) != 0):
                    bml.execXML(character, str(bmlstr))

            ''' processing FML '''
            if (fmlElem != None):
                bmlstr = self.nvbg.process_feedback(character, xml)
                if (len(bmlstr) != 0):
                    msgId = bml.execBML(character, str(bmlstr))
                    print("Keep track of msgId for vrBCFeedback " + msgId)
                    if (msgId != ""):
                        self.nvbg.set_feedback_msgId(character, msgId)
        return True

    def objectEvent(self, character, name, isAnimate, cP, cV, position, velocity, relativePosition, relativeVelocity):
        """
        Necessary
        Override the C++ objectEvent function
        Process events from virtual world (SmartBody)
        """
        if hasattr(self, 'nvbg') is False:
            return

        bmlstr = self.nvbg.process_events(character, name, isAnimate, cP, cV, position, velocity, relativePosition, relativeVelocity)
        if (len(bmlstr) != 0):
            bml.execXML(character, str(bmlstr))
        return

    def setupTool(self):
        """
        Optional
        Debugging options and tools
        """
        if hasattr(self, 'nvbg') is False:
            return

        ''' reset '''
        self.createActionAttribute("reset", True, "nvbgs", 20, False, False, False, "Reload pyke")

        ''' dialogs '''
        dialog = self.createStringAttribute("dialog", "", True, "nvbgs", 50, False, False, False, "Dialog")
        audio = self.createStringAttribute("audio", "", True, "nvbgs", 55, False, False, False, "Audio file")
        tag = self.createStringAttribute("tag", "", True, "nvbgs", 57, False, False, False, "Emotion Tag")
        self.createActionAttribute("play dialog", True, "nvbgs", 60, False, False, False, "Play the chosen dialog")
        self.createActionAttribute("save", True, "nvbgs", 70, False, False, False, "Save the behavior to autoXML.xml")

        ''' chosing the right emotion set '''
        emotionVec = StringVec()
        emotionVec.append("neutral")
        emotionVec.append("angry")
        emotionVec.append("sad")
        emotion = self.createStringAttribute("emotion", "", True, "nvbgs", 80, False, False, False, "Emotion set available")
        emotion.setValidValues(emotionVec)

        ''' nvbg behaviors '''
        behavior = self.createStringAttribute("default behavior", "", True, "nvbgs", 100, False, False, False, "Universal behaviors availabe")
        bmlHidden = self.createStringAttribute("bml hidden", "", True, "nvbgs", 110, False, False, True, "Hidden bml drop list")
        factList = nvbg_engine.get_kb('nvbg').entity_lists['dict']
        universalFact = factList.universal_facts[0]
        numBehaviors = len(universalFact[1])
        behaviorVec = StringVec()
        bmlHiddenVec = StringVec()
        for i in range(0, numBehaviors):
            behaviorUnit = universalFact[1][i]
            behaviorVec.append(behaviorUnit[0])
            bmlTuple = behaviorUnit[1]
            bmlStr = ""
            for j in range(0, len(bmlTuple)):
                bmlStr = bmlStr + bmlTuple[j]
            bmlHiddenVec.append(bmlStr)
        behavior.setValidValues(behaviorVec)
        bmlHidden.setValidValues(bmlHiddenVec)
        self.createStringAttribute("bml", "", True, "nvbgs", 120, False, False, False, "BML corresponding to the behavior")
        self.createActionAttribute("play behavior", True, "nvbgs", 130, False, False, False, "Play the chosen behavior")

    def resetTool(self):
        ''' reload kfb rules '''
        behavior = self.getAttribute("default behavior")
        bmlHidden = self.getAttribute("bml hidden")
        factList = nvbg_engine.get_kb('nvbg').entity_lists['dict']
        universalFact = factList.universal_facts[0]
        numBehaviors = len(universalFact[1])
        behaviorVec = StringVec()
        bmlHiddenVec = StringVec()
        for i in range(0, numBehaviors):
            behaviorUnit = universalFact[1][i]
            behaviorVec.append(behaviorUnit[0])
            bmlTuple = behaviorUnit[1]
            bmlStr = ""
            for j in range(0, len(bmlTuple)):
                bmlStr = bmlStr + bmlTuple[j]
            bmlHiddenVec.append(bmlStr)
        behavior.setValidValues(behaviorVec)
        bmlHidden.setValidValues(bmlHiddenVec)
