# locomotion main state
scene.run("stateAllLocomotion.py")
locomotionSetup("test_utah.sk", "base", "", "all")

# starting state, starting locomotion with different angle
scene.run("stateAllStarting.py")
startingSetup("test_utah.sk", "base", "", "all")

# idle turn state, facing adjusting
scene.run("stateAllIdleTurn.py")
idleTurnSetup("test_utah.sk", "base", "", "all")

# step state, stepping adjusting
scene.run("stateAllStep.py")
stepSetup("test_utah.sk", "base", "", "all")

# transitions
scene.run("transitions.py")
transitionSetup("", "all")
# scene.run("stateSimple.py")
