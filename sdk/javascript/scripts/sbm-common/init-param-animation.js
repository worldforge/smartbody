var initParamAnimation = {
	initParamAnimation : function() {
		// locomotion main state
		stateAllLocomotion.locomotionSetup("test_utah.sk", "base", "", "all")

		// starting state, starting locomotion with different angle
		stateAllStarting.startingSetup("test_utah.sk", "base", "", "all")

		// idle turn state, facing adjusting
		stateAllIdleTurn.idleTurnSetup("test_utah.sk", "base", "", "all")

		// step state, stepping adjusting
		stateAllStep.stepSetup("test_utah.sk", "base", "", "all")

		// transitions
		transitions.transitionSetup("", "all")
		//scene.run("stateSimple.py")
	}
}
