import sys
sys.path.append("/sdcard/vhdata/pythonlibs")
print sys.modules.keys()

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
