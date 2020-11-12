
// ==== OpenVR Headers ===
#include <SDL.h>
#include <external/glew/glew.h>
#include <SDL_opengl.h>
#include <gl/glu.h>
#include <stdio.h>
#include <string>
#include <cstdlib>

#include <openvr.h>

#include "shared/lodepng.h"
#include "shared/Matrices.h"
#include "shared/pathtools.h"

#include "CMainApplication.h"

// === SmartBody Headers ===

#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBPython.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBVHMsgManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBSceneListener.h>

#include <sbm/sbm_constants.h>


#include "SBOpenVRListener.h"
#include <sbm/GPU/SbmShader.h>
#include <sb/SBCharacter.h>
#include <sbm/GPU/SbmTexture.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int main_OpenVR(int argc, char *argv[])
{
	
	//pMainApplication->RunMainLoop();

	//pMainApplication->Shutdown();

	return 0;
}





int main( int argc, char ** argv )
{
	vhcl::Log::StdoutListener* stdoutLog = new vhcl::Log::StdoutListener();
	vhcl::Log::g_log.AddListener(stdoutLog);
	
	
	//OpenVR App initialization
	CMainApplication *pMainApplication = new CMainApplication(argc, argv);

	if (!pMainApplication->BInit())
	{
		pMainApplication->Shutdown();
		return 1;
	}

	int counter = 0;
	bool shaderInitOk = SbmShaderManager::singleton().checkShaderInit(counter);

	// set the relative path from the location of the simplesmartbody binary to the data directory
	// if you are downloading the source code from SVN, it will be ../../../../data
	//std::string mediaPath = "../../../../data/vhdata";
	std::vector<std::string> py_paths;
	std::vector<std::string> init_pys;
	std::string mediaPath = "E:/SmartBody/trunk/data/vhdata";
	std::string python_lib_path = "../../../../core/smartbody/Python27/Lib";
	std::string scriptName = "TestDrSaxonScriptPi.py";
	for (int i = 1; i < argc; i++)
	{
		SmartBody::util::log("SmartBody ARG[%d]: '%s'", i, argv[i]);
		std::string s = argv[i];
		std::string mediapathstr = "";
		std::string fpsStr = "";
		if (s.size() > 11)
			mediapathstr = s.substr(0, 10);
		if (s.size() > 5)
			fpsStr = s.substr(0, 5);

		if (s == "-scriptpath")  // -mepath <dirpath> to specify where sequence files (.seq) should be loaded from
		{
			if (++i < argc) {
				SmartBody::util::log("    Adding sequence path '%s'\n", argv[i]);

				//seq_paths.push_back( argv[i] );
				py_paths.push_back(argv[i]);
			}
			else {
				SmartBody::util::log("ERROR: Expected directory path to follow -seqpath\n");
				// return -1
			}
		}
		else if (s == "-script")  // -seq <filename> to load seq file (replaces old -initseq notation)
		{
			if (++i < argc) {
				std::string filename = argv[i];
				std::string::size_type idx;

				idx = filename.rfind('.');

				if (idx != std::string::npos)
				{
					std::string extension = filename.substr(idx + 1);
					if (extension == "py")
					{
						SmartBody::util::log("    Loading Python scrypt '%s'\n", argv[i]);
						init_pys.push_back(argv[i]);
					}					
				}				
			}
			else {
				SmartBody::util::log("ERROR: Expected filename to follow -script\n");
				// return -1
			}
		}
		else if (mediapathstr == "-mediapath")
		{
			mediaPath = s.substr(11);
			SmartBody::SBScene::getScene()->setMediaPath(mediaPath);
		}
		else
		{
			SmartBody::util::log("ERROR: Unrecognized command line argument: \"%s\"\n", s.c_str());
		}
	}

	// if you're using the SDK, this path will be ../data
	//std::string mediaPath = "../data";

	// add a message logger to stdout
	

	SmartBody::util::log("Loading Python...");
	SmartBody::SBScene::setSystemParameter("pythonlibpath", python_lib_path);
	initPython(python_lib_path);
	// initialize the Python libraries
	//initPython("../Python27/Libs");

	// set up VHMSG
	char * vhmsg_server = getenv("VHMSG_SERVER");
	char * vhmsg_port = getenv("VHMSG_PORT");

	std::string vhmsgServerStr = "";
	if (vhmsg_server)
		vhmsgServerStr = vhmsg_server;
	std::string vhmsgPortStr = "";
	if (vhmsg_port)
		vhmsgPortStr = vhmsg_port;


	SmartBody::SBVHMsgManager* vhmsgManager = SmartBody::SBScene::getScene()->getVHMsgManager();
	
	if (vhmsgServerStr != "")
		vhmsgManager->setServer(vhmsgServerStr);
	if (vhmsgPortStr != "")
		vhmsgManager->setPort(vhmsgPortStr);

	vhmsgManager->setEnable(true);
	if (!vhmsgManager->isEnable())
	{
		SmartBody::util::log("Could not connect to server %s, VHMSG service not enabled.", vhmsg_server);
	}





	// get the scene object
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SBOpenVRListener listener;
	scene->addSceneListener(&listener);
	// set the mediapath which dictates the top-level asset directory
	//scene->setMediaPath(mediaPath);
	//SmartBody::SBScene::getScene()->addAssetPath("script", "examples");	
	//scene->runScript("OculusDemo.py");
	//scene->runScript("OgreCrowdDemo.py");
	//SmartBody::SBScene::getScene()->addAssetPath("script", ".");
	//scene->runScript("TestDrSaxonScriptPi.py");

	std::vector<std::string>::iterator it;
	for (it = py_paths.begin();
		it != py_paths.end();
		++it)
	{
		std::stringstream strstr;
		strstr << "scene.addAssetPath('script', '" << it->c_str() << "')";
		SmartBody::SBScene::getScene()->run((char *)strstr.str().c_str());
	}
	for (it = init_pys.begin();
		it != init_pys.end();
		++it)
	{
		std::string cmd = it->c_str();
		std::stringstream strstr;
		strstr << "scene.run(\"" << cmd.c_str() << "\")";
		SmartBody::SBScene::getScene()->run(strstr.str().c_str());
		SmartBody::util::log("Run Script = %s", strstr.str().c_str());
	}

	SmartBody::util::log("After InitVRBuffers");
	
	SmartBody::util::log("ShaderManageR::LoadShaders");
	
	SmartBody::util::log("g_application.OnStart()");
	// get the simulation object 
	SmartBody::SBSimulationManager* sim = scene->getSimulationManager();

	// if you want to use a real-time clock do the following:
	bool useRealTimeClock = true;
	if (useRealTimeClock)
	{
		sim->setupTimer();
	}
	else
	{
		// otherwise, the time will run according
		sim->setTime(0.0);
	}


	// init text input
	bool bQuit = false;

	SDL_StartTextInput();
	SDL_ShowCursor(SDL_DISABLE);
	
	// make the character do something	
	SmartBody::util::log("Starting the simulation...");
	double lastPrint = 0;
	bool bFirst = true;
	sim->start();
	while (sim->isRunning() && !bQuit)
	{
		if (SmartBody::SBScene::getScene()->getVHMsgManager()->isEnable())
		{
			int err = SmartBody::SBScene::getScene()->getVHMsgManager()->poll();
			if (err == CMD_FAILURE) {
				fprintf(stderr, "ttu_poll ERROR\n");
			}
		}


		scene->update();
		bool update_sim = false; 
		if (!useRealTimeClock)
			sim->setTime(sim->getTime() + 0.16); // update at 1/60 of a second when running in simulated time
		else
			update_sim = sim->updateTimer();
		
		if (update_sim || bFirst)
		{
			scene->update();
			//SmartBody::util::log("Scene update, simulation is at time: %5.2f\n", sim->getTime());

			// Update blendshape and body animation
#if 1
			const std::vector<std::string>& pawns = SmartBody::SBScene::getScene()->getPawnNames();
			for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
				pawnIter != pawns.end();
				pawnIter++)
			{
				SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn((*pawnIter));
				DeformableMeshInstance* meshInstance = pawn->getActiveMesh();
				if (meshInstance)
				{
					meshInstance->blendShapeStaticMesh();
					//pawn->dMesh_p->update();
					if (!meshInstance->isStaticMesh()) // is skinned mesh
					{
						//SmartBody::util::log("drawDeformableModels(): Rendering %s", pawn->getName().c_str());
						//meshInstance->update();
					}
				}
			}
#endif
			bFirst = false;
		}

		bQuit = pMainApplication->HandleInput();

		pMainApplication->RenderFrame();

		//glClearColor(0.2f, 0.2f, 0.6f, 0.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	}
	sim->stop();	

	SDL_StopTextInput();
	pMainApplication->Shutdown();

	return 0;
}
