function playLipSync(dir, name){
	console.log("Play");
	var chrname = $("#characterRadioButton input[type='radio']:checked").val();
	
	var filename = name;
	if(chrname == "ChrBrad"){
		brad.setDoubleAttribute("lipSyncScale", parseFloat($("#lipSyncScale").val()));
		brad.setDoubleAttribute("lipSyncSmoothWindow", parseFloat($("#lipSyncSmoothWindow").val()));
		brad.setDoubleAttribute("lipSyncSpeedLimit", parseFloat($("#lipSyncSpeedLimit").val()));
	}else{
		rachel.setDoubleAttribute("lipSyncScale", parseFloat($("#lipSyncScale").val()));
		rachel.setDoubleAttribute("lipSyncSmoothWindow", parseFloat($("#lipSyncSmoothWindow").val()));
		rachel.setDoubleAttribute("lipSyncSpeedLimit", parseFloat($("#lipSyncSpeedLimit").val()));
	}
	var wavfilename = filename.replace(".bml", ".wav");
	if(vfs_filenames.indexOf(wavfilename) == -1){
		console.log("Make request to the server for: " + wavfilename);
		//make a get request to the server
		var xhr = new XMLHttpRequest();
		xhr.open('GET', '../php/phonemeScheduler.php?dir=' + dir + '&name=' + wavfilename, true);
		xhr.responseType = 'arraybuffer';
		xhr.onload = function (e) {
			// response is unsigned 8 bit integer
			var responseArray = new Uint8Array(this.response);
			FS.writeFile('/sounds/' + wavfilename, responseArray, {
				encoding: 'binary'
			});
			vfs_filenames.push(wavfilename);
			console.log('dtest: ' + (FS.stat('/sounds/' + wavfilename)).size);
			bml.execBML(chrname, '<speech ref=\'' + filename.substring(0, filename.lastIndexOf(".")) +'\'/>');
		};
		xhr.send();
	}else{
		bml.execBML(chrname, '<speech ref=\'' + filename.substring(0, filename.lastIndexOf(".")) +'\'/>');
	}
}
function allReady(){
	console.log( "|--------------------------------------------|")
	console.log( "|       Starting Talking Face Demo 	       |")
	console.log( "|--------------------------------------------|")
	
	if(scene){
		// Add asset paths
		scene.addAssetPath('mesh', 'ChrBrad')
		scene.addAssetPath('motion', 'ChrBrad')
		scene.addAssetPath('mesh', 'ChrRachel');
		scene.addAssetPath('motion', 'ChrRachel');
		scene.addAssetPath('audio', 'sounds')
		
		scene.loadAssets()
		
		// Runs the default viewer for camera
		scene.setScale(1.0)
		scene.setBoolAttribute("internalAudio", true)
		scene.command("PlaySound sounds/test.wav ChrBrad")
		defaultViewer.defaultViewer();
		var camera = Module.getCamera()
		camera.setEye(0.013, 1.59, 0.67)
		camera.setCenter(-0.002, 1.59, -0.08)
		camera.setUpVector(new Module.SrVec(0, 1, 0))
		camera.setScale(1)
		camera.setFov(1.0472)
		camera.setFarPlane(100)
		camera.setNearPlane(0.1)
		camera.setAspectRatio(1.78125)
		defaultInterface.defaultInterface();


		console.log( 'Setting up joint map for Brad and Rachel')
		zebra2map.zebra2Map();
		var zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
		var bradSkeleton = scene.getSkeleton('ChrBrad.sk')
		zebra2Map.applySkeleton(bradSkeleton)
		zebra2Map.applyMotionRecurse('ChrBrad')
		
		var rachelSkeleton = scene.getSkeleton('ChrRachel.sk');
		zebra2Map.applySkeleton(rachelSkeleton);
		zebra2Map.applyMotionRecurse('ChrRachel');
		
		// Establish lip syncing data set
		console.log('Establishing lip syncing data set')
		initDiphoneDefault.initDiphoneDefault()

		// Setting up face definition
		console.log( 'Setting up Brad\'s face definition')
		var bradFace = scene.createFaceDefinition('ChrBrad')
		bradFace.setFaceNeutral('ChrBrad@face_neutral')
		bradFace.setAU(1,  "left",  "ChrBrad@001_inner_brow_raiser_lf")
		bradFace.setAU(1,  "right", "ChrBrad@001_inner_brow_raiser_rt")
		bradFace.setAU(2,  "left",  "ChrBrad@002_outer_brow_raiser_lf")
		bradFace.setAU(2,  "right", "ChrBrad@002_outer_brow_raiser_rt")
		bradFace.setAU(4,  "left",  "ChrBrad@004_brow_lowerer_lf")
		bradFace.setAU(4,  "right", "ChrBrad@004_brow_lowerer_rt")
		bradFace.setAU(5,  "both",  "ChrBrad@005_upper_lid_raiser")
		bradFace.setAU(6,  "both",  "ChrBrad@006_cheek_raiser")
		bradFace.setAU(7,  "both",  "ChrBrad@007_lid_tightener")
		bradFace.setAU(10, "both",  "ChrBrad@010_upper_lip_raiser")
		bradFace.setAU(12, "left",  "ChrBrad@012_lip_corner_puller_lf")
		bradFace.setAU(12, "right", "ChrBrad@012_lip_corner_puller_rt")
		bradFace.setAU(25, "both",  "ChrBrad@025_lips_part")
		bradFace.setAU(26, "both",  "ChrBrad@026_jaw_drop")
		bradFace.setAU(45, "left",  "ChrBrad@045_blink_lf")
		bradFace.setAU(45, "right", "ChrBrad@045_blink_rt")

		bradFace.setViseme("open",    "ChrBrad@open")
		bradFace.setViseme("W",       "ChrBrad@W")
		bradFace.setViseme("ShCh",    "ChrBrad@ShCh")
		bradFace.setViseme("PBM",     "ChrBrad@PBM")
		bradFace.setViseme("FV",      "ChrBrad@FV")
		bradFace.setViseme("wide",    "ChrBrad@wide")
		bradFace.setViseme("tBack",   "ChrBrad@tBack")
		bradFace.setViseme("tRoof",   "ChrBrad@tRoof")
		bradFace.setViseme("tTeeth",  "ChrBrad@tTeeth")
		
		// Establish lip syncing data set
		console.log('Establishing lip syncing data set')
		initDiphoneDefault.initDiphoneDefault()

		// Setting up face definition
		console.log( 'Setting up Brad\'s face definition')
		var rachelFace = scene.createFaceDefinition('ChrRachel')
		rachelFace.setFaceNeutral('ChrRachel@face_neutral')
		rachelFace.setAU(1,  "left",  "ChrRachel@001_inner_brow_raiser_lf")
		rachelFace.setAU(1,  "right", "ChrRachel@001_inner_brow_raiser_rt")
		rachelFace.setAU(2,  "left",  "ChrRachel@002_outer_brow_raiser_lf")
		rachelFace.setAU(2,  "right", "ChrRachel@002_outer_brow_raiser_rt")
		rachelFace.setAU(4,  "left",  "ChrRachel@004_brow_lowerer_lf")
		rachelFace.setAU(4,  "right", "ChrRachel@004_brow_lowerer_rt")
		rachelFace.setAU(5,  "both",  "ChrRachel@005_upper_lid_raiser")
		rachelFace.setAU(6,  "both",  "ChrRachel@006_cheek_raiser")
		rachelFace.setAU(7,  "both",  "ChrRachel@007_lid_tightener")
		rachelFace.setAU(10, "both",  "ChrRachel@010_upper_lip_raiser")
		rachelFace.setAU(12, "left",  "ChrRachel@012_lip_corner_puller_lf")
		rachelFace.setAU(12, "right", "ChrRachel@012_lip_corner_puller_rt")
		rachelFace.setAU(25, "both",  "ChrRachel@025_lips_part")
		rachelFace.setAU(26, "both",  "ChrRachel@026_jaw_drop")
		rachelFace.setAU(45, "left",  "ChrRachel@045_blink_lf")
		rachelFace.setAU(45, "right", "ChrRachel@045_blink_rt")

		rachelFace.setViseme("open",    "ChrRachel@open")
		rachelFace.setViseme("W",       "ChrRachel@W")
		rachelFace.setViseme("ShCh",    "ChrRachel@ShCh")
		rachelFace.setViseme("PBM",     "ChrRachel@PBM")
		rachelFace.setViseme("FV",      "ChrRachel@FV")
		rachelFace.setViseme("wide",    "ChrRachel@wide")
		rachelFace.setViseme("tBack",   "ChrRachel@tBack")
		rachelFace.setViseme("tRoof",   "ChrRachel@tRoof")
		rachelFace.setViseme("tTeeth",  "ChrRachel@tTeeth")
		

		// Setting up Brad
		console.log( 'Setting up Brad')
		brad = scene.createCharacter('ChrBrad', '')
		var bradSkeleton = scene.createSkeleton('ChrBrad.sk')
		brad.setSkeleton(bradSkeleton)
		var bradPos = new Module.SrVec(.25, 0, 0);
		brad.setPosition(bradPos);
		brad.setHPR(new Module.SrVec(-17, 0, 0));
		brad.setFaceDefinition(bradFace)
		brad.createStandardControllers()
		// DeformableMesh
		brad.setDoubleAttribute('deformableMeshScale', .01)
		brad.setStringAttribute('deformableMesh', 'ChrBrad.dae')
		
		// Lip syncing diphone setup
		brad.setStringAttribute('lipSyncSetName', 'default')
		brad.setBoolAttribute('usePhoneBigram', true)
		brad.setStringAttribute("voice", "audiofile")
		brad.setStringAttribute("voiceCode", ".")
		brad.setDoubleAttribute("lipSyncScale", 1.0)
		brad.setDoubleAttribute("lipSyncSmoothWindow", 0.18);
		brad.setDoubleAttribute("lipSyncSpeedLimit", 6.0);
  
		brad.setStringAttribute("displayType", "GPUmesh")
		bml.execBML('ChrBrad', '<body posture="ChrBrad@Idle01"/>')
		bml.execBML('ChrBrad', '<saccade mode="listen"/>')
		
		// Setting up Rachel
		console.log('Setting up Rachel')
		rachel = scene.createCharacter('ChrRachel', '')
		var rachelSkeleton = scene.createSkeleton('ChrRachel.sk')
		rachel.setSkeleton(rachelSkeleton)
		rachelPos = new Module.SrVec(-.25, 0, 0)
		rachel.setPosition(rachelPos)
		rachel.setHPR(new Module.SrVec(17, 0, 0))
		rachel.setFaceDefinition(rachelFace)
		rachel.createStandardControllers()
		// Deformable mesh
		rachel.setDoubleAttribute('deformableMeshScale', .01)
		rachel.setStringAttribute('deformableMesh', 'ChrRachel.dae')
		
		// Lip syncing diphone setup
		rachel.setStringAttribute('lipSyncSetName', 'default')
		rachel.setBoolAttribute('usePhoneBigram', true)
		rachel.setStringAttribute("voice", "audiofile")
		rachel.setStringAttribute("voiceCode", ".")
		rachel.setDoubleAttribute("lipSyncScale", 1.0)
		rachel.setDoubleAttribute("lipSyncSmoothWindow", 0.18);
		rachel.setDoubleAttribute("lipSyncSpeedLimit", 6.0);

		bml.execBML('ChrRachel', '<body posture="ChrConnor@IdleStand01" ready=".2" relax=".2"/>')
	}else{
		console.error("Scene does not exist!");
	}
}
