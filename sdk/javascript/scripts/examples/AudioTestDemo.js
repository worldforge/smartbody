function allReady(){
	console.log( "|--------------------------------------------|")
	console.log( "|       	Audio Test Demo 	       |")
	console.log( "|--------------------------------------------|")
	if(scene){
		scene.setScale(1.0)
		scene.setBoolAttribute('internalAudio', true)
		
		defaultViewer.defaultViewer();
		var camera = Module.getCamera()
		camera.setEye(0, 1.71, 1.86)
		camera.setCenter(0, 1, 0.01)
		camera.setUpVector(new Module.SrVec(0, 1, 0))
		camera.setScale(0.01)
		camera.setFov(1.0472)
		camera.setFarPlane(100)
		camera.setNearPlane(0.1)
		camera.setAspectRatio(0.966897)
		cameraPos = new Module.SrVec(0, 1.6, 10)
		scene.getPawn('camera').setPosition(cameraPos)
		defaultInterface.defaultInterface();
		
		console.log("Play Audio Now!");
		//don't know why the first call could not be played, seems like it needs some trigger
		scene.command("PlaySound /sounds/test.wav ChrBrad")	
		scene.command("PlaySound /sounds/test.wav ChrBrad")	
	}else{
		console.error("Scene does not exist!");
	}
}