function allReady(){
	//Provides a way to execute callback functions based on one or more objects, 
	//usually Deferred objects that represent asynchronous events.
	//place your code here, the scripts are all loaded
	//You'll get an error in the console since the script files does'nt exists, but that shows that the browser tried loading them
	console.log( "|--------------------------------------------|")
	console.log( "|       	Interface Test Demo 	       |")
	console.log( "|--------------------------------------------|")
	
	if(scene){
		scene.setScale(1.0)
		scene.addAssetPath('audio', 'sounds')
		scene.loadAssets()
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
	}else{
		console.error("Scene does not exist!");
	}

}

