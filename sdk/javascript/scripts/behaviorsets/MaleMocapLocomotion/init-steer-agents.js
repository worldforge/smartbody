var initSteerAgents = {

	setupSteerAgent : function (charName, prefix){
		console.log("Init Steer Agent")
		var steerManager = scene.getSteerManager()
		steerManager.removeSteerAgent(charName)
		var steerAgent = steerManager.createSteerAgent(charName)
		steerAgent.setSteerStateNamePrefix(prefix)
		steerAgent.setSteerType("example")
		//var sbCharacter = scene.getCharacter(charName)
	}
}


	


