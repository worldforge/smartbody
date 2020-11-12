var defaultViewer = {
	defaultViewer : function (){
		var viewer = Module.getViewer();
		if(viewer)
			viewer.show();
		else
			console.log("getViewer is NULL"); 

		var sscale = 1.0/scene.getScale();

		var camera = scene.getCamera("activeCamera")
		if(camera){
			camera.setEye(0*sscale, 1.66*sscale, 1.85*sscale);
			camera.setCenter(0, .92*sscale, 0);
			//camera.setFarPlane(100*sscale);
			//camera.setNearPlane(0.1*sscale);
		}else{
			console.log("getCamera is NULL");
		}

        //what this camera pawn is used???
		var cameraPawn = scene.createPawn("camera");
		var cameraPos = new Module.SrVec(0, 1.66*sscale, 1.85*sscale);
		cameraPawn.setPosition(cameraPos);

		var light0 = scene.createPawn("light0");
		light0Pos = new Module.SrVec(1.00*sscale, 2.50*sscale, 4.00*sscale);
		light0.setPosition(light0Pos);
		light0.setHPR(new Module.SrVec(180, -50, 180));

		light0.createBoolAttribute("lightCastShadow", true, true, "LightAttributes", 8, false, false, false, "Does the light cast shadow");
		light0.createBoolAttribute("lightIsDirectional", true, true, "LightAttributes", 5, false, false, false, "Is the light a directional light");
		light0.createVec3Attribute("lightDiffuseColor", 1, 1, 1, true, "LightAttributes", 10, false, false, false, "Diffuse color");
		light0.createVec3Attribute("lightAmbientColor", 0, 0, 0, true, "LightAttributes", 20, false, false, false, "Ambient color");
		light0.createVec3Attribute("lightSpecularColor", 0, 0, 0,  true, "LightAttributes", 30, false, false, false, "Specular color");
		light0.createDoubleAttribute("lightSpotExponent", 0, true, "LightAttributes", 40, false, false, false, "Spotlight exponent");
		light0.createVec3Attribute("lightSpotDirection", 0, 0, -1,  true, "LightAttributes", 50, false, false, false, "Spotlight direction");
		light0.createDoubleAttribute("lightSpotCutoff", 180.0, true, "LightAttributes", 60, false, false, false, "Spotlight cutoff angle");
		light0.createDoubleAttribute("lightConstantAttenuation", 1.0, true, "LightAttributes", 70, false, false, false, "Constant attenuation");
		light0.createDoubleAttribute("lightLinearAttenuation", 0.0, true, "LightAttributes", 80, false, false, false, "Linear attenuation");
		light0.createDoubleAttribute("lightQuadraticAttenuation", 0.0, true, "LightAttributes", 90, false, false, false, "Quadratic attenuation");


		var light1 = scene.createPawn("light1");
		var light1Pos = new Module.SrVec(1.00*sscale, 5.00*sscale, -10.00*sscale);
		light1.setPosition(light1Pos);
		light1.setHPR(new Module.SrVec(180, 61, 180));

		light1.createBoolAttribute("lightCastShadow", true, true, "LightAttributes", 8, false, false, false, "Does the light cast shadow");
		light1.createBoolAttribute("lightIsDirectional", true, true, "LightAttributes", 5, false, false, false, "Is the light a directional light");
		light1.createVec3Attribute("lightDiffuseColor", 1, 1, 1, true, "LightAttributes", 10, false, false, false, "Diffuse color");
		light1.createVec3Attribute("lightAmbientColor", 0, 0, 0, true, "LightAttributes", 20, false, false, false, "Ambient color");
		light1.createVec3Attribute("lightSpecularColor", 0, 0, 0,  true, "LightAttributes", 30, false, false, false, "Specular color");
		light1.createDoubleAttribute("lightSpotExponent", 0, true, "LightAttributes", 40, false, false, false, "Spotlight exponent");
		light1.createVec3Attribute("lightSpotDirection", 0, 0, -1,  true, "LightAttributes", 50, false, false, false, "Spotlight direction");
		light1.createDoubleAttribute("lightSpotCutoff", 180.0, true, "LightAttributes", 60, false, false, false, "Spotlight cutoff angle");
		light1.createDoubleAttribute("lightConstantAttenuation", 1.0, true, "LightAttributes", 70, false, false, false, "Constant attenuation");
		light1.createDoubleAttribute("lightLinearAttenuation", 0.0, true, "LightAttributes", 80, false, false, false, "Linear attenuation");
		light1.createDoubleAttribute("lightQuadraticAttenuation", 0.0, true, "LightAttributes", 90, false, false, false, "Quadratic attenuation");
		
		//scene.createBoolAttribute("enableAlphaBlend", false, true, "", 5, false, false, false, "enable alpha blending?");
	}
	
}
