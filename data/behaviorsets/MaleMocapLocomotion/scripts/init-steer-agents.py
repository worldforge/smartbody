print("Init Steer Agent")


def setupSteerAgent(charName, prefix):
    steerManager.removeSteerAgent(charName)
    steerAgent = steerManager.createSteerAgent(charName)
    steerAgent.setSteerStateNamePrefix(prefix)
    steerAgent.setSteerType("example")
    sbCharacter = scene.getCharacter(charName)
