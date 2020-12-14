#define ENABLE_CMDL_TEST		0
#define ENABLE_808_TEST			0

#define SBM_REPORT_MEMORY_LEAKS  0
#define SBM_EMAIL_CRASH_REPORTS  0

#include "GL/glew.h"

#ifndef SB_NO_VHMSG
#include "vhmsg-tt.h"
#endif
#include <sbm/lin_win.h>
#include <iostream>
#include <string>
#include <vector>
#include "fltk_viewer.h"
#include "RootWindow.h"
#include <channelbufferviewer/channelbufferWindow.hpp>
#include <faceviewer/FaceViewer.h>
#include <sb/SBBoneBusManager.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <pythonbind/SBPythonAutoRig.h>

#include <sbm/sbm_constants.h>
#include <sbm/xercesc_utils.hpp>

#include <sb/SBScene.h>
#include <sbm/mcontrol_callbacks.h>
#include <sbm/sbm_test_cmds.hpp>
#include <bml/bml_processor.hpp>
#include <sbm/remote_speech.h>
#include <sbm/local_speech.h>
#include <sbm/sbm_audio.h>
#include <sbm/sbm_speech_audiofile.hpp>
#include <sbm/text_speech.h> // [BMLR]
#include <sbm/time_regulator.h>
#include "CommandWindow.h"
#include "SBPython.h"
#include <sb/SBSteerManager.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBSpeechManager.h>
#include <sb/SBAssetManager.h>
#include "SBUtilities.h"
#ifdef SB_NO_BONEBUS
#include <sb/SBBoneBusManager.h>
#endif
#include "FLTKListener.h"
#include "sb/SBDebuggerServer.h"
#include <sb/SBDebuggerClient.h>
#include <sbm/PPRAISteeringAgent.h>
#ifndef SB_NO_VHMSG
#include <vhmsg.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "SBPythonClass.h"
#include "SBNavmeshPython.h"
#include "gwiz_cmdl.h"

#ifdef WIN32
#ifndef NOMINMAX
#define NOMINMAX        /* Don't defined min() and max() */
#endif
#include <windows.h>
#include <mmsystem.h>
#if SBM_REPORT_MEMORY_LEAKS
#include <malloc.h>
#include <crtdbg.h>
#endif
#endif

#if SBM_EMAIL_CRASH_REPORTS
#include <vhcl_crash.h>
#endif

#include "pic.h"
#include "sr/sr_model.h"

#include "SBRendererGLPython.h"

///////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32_LEAN_AND_MEAN
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
#else
#endif

using std::vector;
using std::string;

int snapshotCounter = 0;
int G_usesymbols = 0;

// Global FLTK callback for drawing all label text
void MyDraw(const Fl_Label *o, int X, int Y, int W, int H, Fl_Align a) {
	fl_font(o->font, o->size);
	fl_color((Fl_Color)o->color);
	fl_draw(o->value, X, Y, W, H, a, o->image, G_usesymbols);
}

// Global FLTK callback for measuring all labels
void MyMeasure(const Fl_Label *o, int &W, int &H) {
	fl_font(o->font, o->size);
	fl_measure(o->value, W, H, G_usesymbols);
}

///////////////////////////////////////////////////////////////////////////////////
//
//void sbm_vhmsg_callback( const char *op, const char *args, void * user_data )
//{
//	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
//	// Replace singleton with a user_data pointer
//	if (scene->isRemoteMode())
//	{
//		scene->getDebuggerClient()->ProcessVHMsgs(op, args);
//		return;
//	}
//	else
//	{
//		scene->getDebuggerServer()->ProcessVHMsgs(op, args);
//	}
//
//	std::stringstream strstr;
//	strstr << op << "  " << args;
//	switch( scene->command(strstr.str() ))
//	{
//        case false:
//            SmartBody::util::log("SmartBody Error: command FAILED: '%s' + '%s'", op, args );
//            break;
//    }
//}

/*

	viewer open <width> <height> <px> <py> 
	viewer show|hide
	
*/
//
//int mcu_viewer_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr )
//{
//	auto& renderScene = Session::current->renderScene;
//	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
//	char *view_cmd = args.read_token();
////	if( strcmp( view_cmd, "open" ) == 0 )
////	{
////		if( scene->getViewer() == nullptr )
////		{
////			int argc = args.calc_num_tokens();
////			int width = 1024;
////			int height = 768;
////			int px = 100;
////			int py = 100;
////			if( argc >= 4 )	{
////				width = args.read_int();
////				height = args.read_int();
////				px = args.read_int();
////				py = args.read_int();
////			}
////
////			if( scene->getViewer() == nullptr )
////			{
////				if (!scene->getViewerFactory())
////					return CMD_FAILURE;
////				scene->setViewer(scene->getViewerFactory()->create( px, py, width, height));
////				scene->getViewer()->label_viewer( "SB Viewer - Local Mode" );
////				SrCamera* camera = renderScene.createCamera("activeCamera");
////				scene->getViewer()->set_camera( camera );
////				//((FltkViewer*)viewer_p)->set_mcu(this);
////				scene->getViewer()->show_viewer();
////				if( scene->getRootGroup() )	{
////					scene->getViewer()->root( scene->getRootGroup() );
////				}
////#if !defined (__ANDROID__) && !defined(SBM_IPHONE) && !defined(__native_client__)
////				SbmShaderManager::singleton().setViewer(scene->getViewer());
////#endif
////				return( CMD_SUCCESS );
////			}
////			return( CMD_FAILURE );
////		}
////		else
////		{
////			scene->getViewer()->show_viewer();
////		}
////	}
////	else if( strcmp( view_cmd, "close" ) == 0 )
////	{
////		if( scene->getViewer() )
////		{
////			scene->getViewerFactory()->remove(scene->getViewer());
////			scene->setViewer(nullptr);
////#if !defined (__ANDROID__) && !defined(SBM_IPHONE) && !defined(__native_client__)
////			SbmShaderManager::singleton().setViewer(nullptr);
////#endif
////			return( CMD_SUCCESS );
////		}
////	}
////	else
//		if( strcmp( view_cmd, "show" ) == 0 )
//	{
//		if( scene->getViewer() )
//		{
//			scene->getViewer()->show_viewer();
//			return( CMD_SUCCESS );
//		}
//	}
//	else if( strcmp( view_cmd, "hide" ) == 0 )
//	{
//		if( scene->getViewer() )
//		{
//			scene->getViewer()->hide_viewer();
//			return( CMD_SUCCESS );
//		}
//	}
//	else
//	{
//		return( CMD_NOT_FOUND );
//	}
//
//	return( CMD_FAILURE );
//}

/*

	camera	eye <x y z>
	camera	center <x y z>
#	camera	up <x y z>
#	camera	fovy <degrees>
	camera	scale <factor>
	camera	default [<preset>]

*/

int mcu_camera_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr )	{

		auto& renderScene = Session::current->renderScene;
		SrCamera* camera = renderScene.getActiveCamera();
		if (!camera)
		{
			SmartBody::util::log("No active camera. Camera command not executed.");
			return CMD_FAILURE;
		}
		char *cam_cmd = args.read_token();
		if( strcmp( cam_cmd, "eye" ) == 0 )	{
			float x = args.read_float();
			float y = args.read_float();
			float z = args.read_float();
			camera->setEye( x, y, z );
		}
		else
		if( strcmp( cam_cmd, "center" ) == 0 )	{
			float x = args.read_float();
			float y = args.read_float();
			float z = args.read_float();
			camera->setCenter( x, y, z );
		}
		else
		if( strcmp( cam_cmd, "scale" ) == 0 )	{
			camera->setScale(args.read_float());
		}
		else
		if( strcmp( cam_cmd, "track" ) == 0 )	{
			char* name = args.read_token();
			if (!name || strcmp(name, "") == 0)
			{
				if (renderScene.hasCameraTrack())
				{
					renderScene.removeCameraTrack();
					SmartBody::util::log("Removing current tracked object.");
						return( CMD_SUCCESS );
				}
				SmartBody::util::log("Need to specify an object and a joint to track.");
				return( CMD_FAILURE );
			}
			SmartBody::SBPawn* pawn = Session::current->scene.getPawn(name);
			if (!pawn)
			{
				pawn = Session::current->scene.getCharacter(name);
				if (!pawn)
				{
					SmartBody::util::log("Object %s was not found, cannot track.", name);
					return( CMD_FAILURE );
				}
			}
			char* jointName = args.read_token();
			if (!jointName || strcmp(jointName, "") == 0)
			{
				SmartBody::util::log("Need to specify a joint to track.");
				return( CMD_FAILURE );
			}
			auto skeleton = pawn->getSkeleton();

			SkJoint* joint = pawn->getSkeleton()->search_joint(jointName);
			if (!joint)
			{
				SmartBody::util::log("Could not find joint %s on object %s.", jointName, name);
				return( CMD_FAILURE );
			}
	
			bool hasTracks = renderScene.hasCameraTrack();
			if (hasTracks)
			{
				renderScene.removeCameraTrack();
				SmartBody::util::log("Removing current tracked object.");
			}
			renderScene.setCameraTrack(name, joint->getName());
			return CMD_SUCCESS;			
		}
		else
		if( strcmp( cam_cmd, "default" ) == 0 )	{
			int preset = args.read_int();
				
			if( preset == 1 )	{
				BaseWindow::getInstance().view_all();
			}
		}
		else if (strcmp( cam_cmd, "reset" ) == 0 ) {
			float scale = 1.f/Session::current->scene.getScale();
			SrVec camEye = SrVec(0,1.66f,1.85f)*scale;
			SrVec camCenter = SrVec(0,0.92f,0)*scale;	
			float znear = 0.01f*scale;
			float zfar = 100.0f*scale;
			char camCommand[256];
			sprintf(camCommand,"camera eye %f %f %f",camEye[0],camEye[1],camEye[2]);				
			Session::current->scene.getCommandManager()->execute((char*)camCommand);
			sprintf(camCommand,"camera center %f %f %f",camCenter[0],camCenter[1],camCenter[2]);
			Session::current->scene.getCommandManager()->execute((char*)camCommand);
			camera->setNearPlane(znear);
			camera->setFarPlane(zfar);

		}
		else if (strcmp( cam_cmd, "frame" ) == 0 ) {
			SrBox sceneBox;
			const std::vector<std::string>& pawnNames = Session::current->scene.getPawnNames();

			for (const auto & pawnName : pawnNames)
			{
				SmartBody::SBPawn* pawn = Session::current->scene.getPawn(pawnName);
				bool visible = pawn->getBoolAttribute("visible");
					if (!visible)
						continue;
				SrBox box = pawn->getSkeleton()->getBoundingBox();
				sceneBox.extend(box);
			}
			camera->view_all(sceneBox, camera->getFov());	
			float scale = 1.f/Session::current->scene.getScale();
			float znear = 0.01f*scale;
			float zfar = 100.0f*scale;
			camera->setNearPlane(znear);
			camera->setFarPlane(zfar);
		}
		return( CMD_SUCCESS );


	return( CMD_FAILURE );
}

int mcu_snapshot_func2(srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr)
{// get the image data
	auto& rootWindow = BaseWindow::getInstance();

	string output_file = args.read_token();

	if (output_file.empty())
	{
		std::stringstream output_file_os;
		output_file_os << "snapshot_" << snapshotCounter << ".tga";	// default output name
		snapshotCounter++;
		output_file = output_file_os.str();
	}

	int y = rootWindow.curViewer->h();
	int x = rootWindow.curViewer->w();

	long imageSize = x * y * 3;

	auto *data = new unsigned char[imageSize];
	glReadPixels(0, 0, x, y, GL_BGR, GL_UNSIGNED_BYTE, data);// split x and y sizes into bytes
	int xa = x % 256;
	int xb = (x - xa) / 256; int ya = y % 256;
	int yb = (y - ya) / 256;//assemble the header
	unsigned char header[18] = { 0,0,2,0,0,0,0,0,0,0,0,0,(unsigned char)xa,(unsigned char)xb,(unsigned char)ya,(unsigned char)yb,24,0 };
	// write header and data to file
	SmartBody::util::log("Snapshot to %s", output_file.c_str());
	fstream File(output_file, ios::out | ios::binary);
	File.write(reinterpret_cast<char *>(header), sizeof(char) * 18);
	File.write(reinterpret_cast<char *>(data), sizeof(char)*imageSize);
	File.close();

	delete[] data;
	data = nullptr;
	return 0;
}

// snapshot <output file>
int mcu_snapshot_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr )
{
	auto& rootWindow = BaseWindow::getInstance();
	string output_file = args.read_token();

	int windowHeight = rootWindow.curViewer->h();
	int windowWidth = rootWindow.curViewer->w();
	int offsetHeight = 0;
	int offsetWidth = 0;

	if (output_file.empty())
	{
		std::stringstream output_file_os;
		output_file_os << "snapshot_" << snapshotCounter << ".ppm";	// default output name
		snapshotCounter++;
		output_file = output_file_os.str();
	}
	// Allocate a picture buffer snap
	Pic * in = pic_alloc(windowWidth, windowHeight, 3, nullptr);
	SmartBody::util::log("  File to save to: %s", output_file.c_str());

	for (int i = windowHeight - 1; i >= 0; i--)
	{
		glReadPixels(0 + offsetWidth, windowHeight + offsetHeight - i - 1, windowWidth, 1, GL_RGB, GL_UNSIGNED_BYTE, &in->pix[i*in->nx*in->bpp]);
	}

	if (ppm_write(output_file.c_str(), in))
	{
		pic_free(in);
		SmartBody::util::log("  File saved Successfully\n");
		return(CMD_SUCCESS);
	}
	else
	{
		pic_free(in);
		SmartBody::util::log("  Error in Saving\n");
		return(CMD_FAILURE);
	}

	return(CMD_SUCCESS);
}


int mcu_quit_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr  )	{

	

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	scene->getSimulationManager()->stop();
	if (scene->getSteerManager()->getEngineDriver()->isInitialized())
	{
		scene->getSteerManager()->getEngineDriver()->stopSimulation();
		scene->getSteerManager()->getEngineDriver()->unloadSimulation();
		scene->getSteerManager()->getEngineDriver()->finish();
	
		const std::vector<std::string>& characterNames = scene->getCharacterNames();
		for (const auto & characterName : characterNames)
		{
			SmartBody::SBCharacter* character = scene->getCharacter(characterName);
			SmartBody::SBSteerAgent* steerAgent = Session::current->scene.getSteerManager()->getSteerAgent(character->getName());
			if (steerAgent)
			{
				auto* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
				ppraiAgent->setAgent(nullptr);
			}
		}
	}
	return( CMD_SUCCESS );
}

///////////////////////////////////////////////////////////////////////////////////

void mcu_register_callbacks( ) {
	
	SmartBody::SBCommandManager* cmdMgr = Session::current->scene.getCommandManager();
	
	// additional commands associated with this viewer
	cmdMgr->insert( "q",			mcu_quit_func );
	cmdMgr->insert( "quit",			mcu_quit_func );
	cmdMgr->insert( "snapshot",		mcu_snapshot_func );
	cmdMgr->insert( "snapshot2",	mcu_snapshot_func2);
//	cmdMgr->insert( "viewer",		mcu_viewer_func );
	cmdMgr->insert( "camera",		mcu_camera_func );
}

void cleanup( )	{
	{


		if (Session::current) {
			if (Session::current->scene.getSimulationManager()->isStopped())
			{
				SmartBody::util::log( "SmartBody NOTE: unexpected exit " );
			}

			if (Session::current->scene.getBoolAttribute("internalAudio"))
			{
				AUDIO_Close();
			}
			Session::current->vhmMsgManager.send("vrProcEnd sbm");
		}


		/*
#if LINK_VHMSG_CLIENT
		if (Session::current->vhmMsgManager.isEnable())
		{
			Session::current->vhmMsgManager.disconnect();
		}
#endif
		*/

		/*BaseWindow* rootWindow = dynamic_cast<BaseWindow*>(Session::current->scene.getViewer());
		if (rootWindow)
		{
		   printf("Delete SBGUI Window\n");
		   delete rootWindow;
		}*/
	}

	// explicitly delete SBScene singleton
	//delete SmartBody::SBScene::getScene();	
	XMLPlatformUtils::Terminate();

	SmartBody::util::log( "sbgui terminated gracefully." );


#if SBM_REPORT_MEMORY_LEAKS
	_CrtMemState  state;
	_CrtMemCheckpoint( &state );
	_CrtMemDumpStatistics( &state );
#endif
}

void signal_handler(int sig) {
//	std::cout << "SmartBody shutting down after catching signal " << sig << std::endl;



	if (Session::current) {
		Session::current->vhmMsgManager.send("vrProcEnd sbm" );
	}
	// get the current directory
#ifdef WIN32
	char buffer[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, buffer);
#else
	char buffer[PATH_MAX];
	getcwd(buffer, PATH_MAX);
#endif
	/*
	// dump to an available file in the current directory
	int counter = 1;
	bool fileOk = false;
	char file[1024];
	while (!fileOk || counter > 999)
	{
		sprintf(file, "%s\\smartbodycommands%.5d.dump", buffer, counter);

		std::fstream fs(file, std::ios_base::in);// attempt open for read
		if (!fs)
		{
			fileOk = true;
			fs.close();
			std::ofstream dumpFile;
			dumpFile.open(file);

			dumpFile << "Last commands run:" << std::endl;

			// dump the command resources
			int numResources = mcuCBHandle::singleton().resource_manager->getNumResources();
			for (int r = 0; r < numResources; r++)
			{
				CmdResource * res = dynamic_cast<CmdResource  *>(mcuCBHandle::singleton().resource_manager->getResource(r));
				if(res)
					dumpFile << res->dump() << std::endl;
			}
			//std::cout << "Wrote commands to " << file << std::endl;

			dumpFile.close();

		}
		else //ok, file exists. close and reopen in write mode
		{
		  fs.close();
		  counter++;
		}
	}
	*/
	
	//cleanup(); // 
	//exit(sig);
}


class SBMCrashCallback : public vhcl::Crash::CrashCallback
{
   public:
      void OnCrash() override { signal_handler( -1 ); }
};


///////////////////////////////////////////////////////////////////////////////////
std::string test_cmdl_tab_callback( std::string io_str )	{
	std::string prompt( "X> " );

	gwiz::cmdl commandline;
	commandline.set_callback( gwiz::cmdl::editor_callback );
	commandline.render_prompt( prompt );
	commandline.write( io_str );

	bool quit = false;
	while( !quit ) {
		if( commandline.pending( prompt ) )	{

			std::string new_str = commandline.read();
			if( new_str == "t" )	{
				commandline.write( std::string( "test" ) );
			}
			else
			if( new_str == "q" )	{
				quit = true;
				io_str = "done";
			}
		}
	}
	fprintf( stdout, "<exit>" ); 
	fprintf( stdout, "\n" ); 
	return( io_str );
}

///////////////////////////////////////////////////////////////////////////////////

int main( int argc, char **argv )	{

 // 808: undefined reference to `bonebus::BoneBusCharacter::StartSendBoneRotations()'
//	return( 0 );
#if SBM_REPORT_MEMORY_LEAKS
	// CRT Debugging flags - Search help:
	//   _CrtSetDbgFlag
	// Memory Leaks - To find out where a memory leak happens:
	//   Rerun the programs and set this variable in the debugger
	//   to the alloc # that leaks: {,,msvcr90d.dll}_crtBreakAlloc
	//   The program will stop right before the alloc happens

	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_DELAY_FREE_MEM_DF );

	_CrtMemState  state;
	_CrtMemCheckpoint( &state );
	_CrtMemDumpStatistics( &state );
#endif

#if SBM_EMAIL_CRASH_REPORTS
	// Log crashes, with a dialog option to report via email
	vhcl::Crash::EnableExceptionHandling( true );
	vhcl::Crash::AddCrashCallback( new SBMCrashCallback() );
#endif

	
	
	Fl::set_labeltype(FL_NORMAL_LABEL, MyDraw, MyMeasure);

	// init glew to use OpenGL extension
	//bool hasShaderSupport = SbmShader::initShader();

	//CommandWindow* commandWindow = new CommandWindow(100, 100, 640, 480, "Commands");
	//commandWindow->show();
	//vhcl::Log::g_log.AddListener(commandWindow);
	//FltkViewer* viewer = new FltkViewer(100, 150, 640, 480, "SmartBody");

	// register the log listener
	auto* listener = new SmartBody::util::StdoutListener();
	SmartBody::util::g_log.AddListener(listener);

	int err;
	string net_host;
	vector<string> seq_paths;
	vector<string> py_paths;
	vector<string> me_paths;
	vector<string> audio_paths;
	vector<string> init_seqs;
	vector<string> init_pys;
	string proc_id;
	
	XMLPlatformUtils::Initialize();  // Initialize Xerces before creating MCU

	

	//FLTKListener fltkListener;

#ifdef __APPLE__
	Fl::scheme("plastic");
#else
	Fl::scheme("gtk+");
#endif
	
	std::string python_lib_path;
	std::string festivalLibDir = "../../../../lib/festival/festival/lib/";
	std::string festivalCacheDir = "../../../../data/cache/festival/";
	std::string mediaPath = SMARTBODY_DATADIR "/smartbody/data";
	std::string renderer = "custom";

	std::string cereprocLibDir = "../../../../lib/cerevoice/voices/";	



	// look for a file called .smartbodysettings in the current directory
	// to determine the initial settings
	std::ifstream settingsFile(".smartbodysettings");
	// look for a .smartbodysettings file in the current folder
	std::string binaryLocation = argv[0];
	boost::filesystem::path path(binaryLocation);
	boost::filesystem::path parentPath = path.parent_path();
	bool useBinaryPath = false;
	if (!settingsFile.good())
	{
		SmartBody::util::log("Did not find .smartbodysettings in current directory, checking binary location.");
		
		boost::filesystem::path settingsPath = parentPath;
		settingsPath /= ".smartbodysettings";
		settingsFile.close();
		settingsFile.open(boost::filesystem::complete(settingsPath).string());
		if (!settingsFile.good())
		{
			SmartBody::util::log("Did not find .smartbodysettings in binary directory, using defaults.");
		}
		else
		{
			SmartBody::util::log("Found .smartbodysettings file in binary folder.");
			useBinaryPath = true;
		}
	}
	else
	{
		SmartBody::util::log("Found .smartbodysettings file in current folder.");
	}

	if (settingsFile.good())
	{
		boost::filesystem::path settingsPath(".smartbodysettings");
		// save the .smartbodysettings file location
#if (BOOST_VERSION > 104400)
			SmartBody::SBScene::setSystemParameter(".smartbodysettings",  boost::filesystem::absolute(settingsPath).string());
#else
		SmartBody::SBScene::setSystemParameter(".smartbodysettings",  boost::filesystem::complete(settingsPath).string());
#endif
		SmartBody::util::log("Found .smartbodysettings file.");
		std::string line;
		while (!settingsFile.eof())
		{
			getline(settingsFile, line);
			
			std::vector<std::string> tokens;
			SmartBody::util::tokenize(line, tokens, "=");
			for (size_t t = 0; t < tokens.size(); t++)
			{
				if (tokens[t] == "pythonlibpath")
				{
					if (tokens.size() > t + 1)
					{
						python_lib_path = tokens[t + 1];
						SmartBody::util::log("Setting Python Library path to %s", tokens[t + 1].c_str());
						if (useBinaryPath)
						{
							boost::filesystem::path pythonFolderPath = parentPath;
							pythonFolderPath /= python_lib_path;
							std::string absPath = boost::filesystem::canonical(pythonFolderPath).string();
#ifdef WIN32
							absPath = SmartBody::util::replace(absPath, "\\", "/");
#endif
							python_lib_path = absPath;
							SmartBody::util::log("pythonlibpath = %s", absPath.c_str());
							SmartBody::SBScene::setSystemParameter("pythonlibpath", absPath);
						}
						else
						{
							SmartBody::util::log("pythonlibpath = %s", python_lib_path.c_str());
							SmartBody::SBScene::setSystemParameter("pythonlibpath", python_lib_path);
						}
						t++;
					}
					
				}
				else if (tokens[t] == "ogrepath")
				{
					if (tokens.size() > t + 1)
					{
						SmartBody::util::log("Setting Ogre path to %s", tokens[t + 1].c_str());
						if (useBinaryPath)
						{
							boost::filesystem::path ogreFolderPath = parentPath;
							ogreFolderPath /= tokens[t + 1];
							try {
								std::string absPath = boost::filesystem::canonical(ogreFolderPath).string();
#ifdef WIN32
								absPath = SmartBody::util::replace(absPath, "\\", "/");
#endif
								SmartBody::util::log("ogrepath = %s", absPath.c_str());
								SmartBody::SBScene::setSystemParameter("ogrepath", absPath);
							} catch (exception& e) {
								SmartBody::util::log("Could not set ogre path %s", tokens[t + 1].c_str());
								
							}
						}
						else
						{
							SmartBody::util::log("ogrepath = %s", tokens[t + 1].c_str());
							SmartBody::SBScene::setSystemParameter("ogrepath", tokens[t + 1]);
						}
						t++;
					}
					
				}
				else if (tokens[t] == "mediapath")
				{
					if (tokens.size() > t + 1)
					{
						mediaPath = tokens[t + 1];
						SmartBody::util::log("Setting mediapath to %s", tokens[t + 1].c_str());
						if (useBinaryPath)
						{
							boost::filesystem::path mediaFolderPath = parentPath;
							mediaFolderPath /= mediaPath;
							std::string absPath = boost::filesystem::canonical(mediaFolderPath).string();
#ifdef WIN32
							absPath = SmartBody::util::replace(absPath, "\\", "/");
#endif
							SmartBody::util::log("mediapath = %s", absPath.c_str());
							mediaPath = absPath;
						}
						t++;
					}
				}
				else if (tokens[t] == "speechrelaycommand")
				{
					if (tokens.size() > t + 1)
					{
						SmartBody::util::log("Setting speech relay command to %s", tokens[t + 1].c_str());
						SmartBody::SBScene::setSystemParameter("speechrelaycommand", tokens[t + 1]);
						t++;
					}
				}
				else if (tokens[t] == "renderer")
				{
					if (tokens.size() > t + 1)
					{
						renderer = tokens[t + 1];
						SmartBody::util::log("Setting renderer to %s", tokens[t + 1].c_str());
						SmartBody::SBScene::setSystemParameter("renderer", renderer);
						t++;
					}
				}
				else
				{
					if (tokens.size() > t + 1)
					{
						SmartBody::util::log("Setting system parameter %s to %s", tokens[t].c_str(), tokens[t + 1].c_str());
						SmartBody::SBScene::setSystemParameter(tokens[t], tokens[t + 1]);
						t++;
					}
					else
					{
						SmartBody::util::log("Setting system parameter %s to \"\"", tokens[t].c_str());
						SmartBody::SBScene::setSystemParameter(tokens[t], "");
					}
				}
			}
		}
	}
	SmartBody::SBScene::setSystemParameter("mediapath", mediaPath);
	settingsFile.close();

	//Create the global session here.
	Session::current = new Session();

	auto& scene = Session::current->scene;

	// EDF - taken from tre_main.cpp, a fancier command line parser can be put here if desired.
	//	check	command line parameters:
	bool lock_dt_mode = false;
	float sleepFPS = -1;
	float intervalAmount = -1;
	bool isInteractive = true;

	bool useEditor = true;
	bool maximize = false;
	std::string windowName = "SmartBody";

	int locX = 0;
	int locY =0;
	int locW = 1024;
	int locH = 768;

	std::vector<std::string> envNames;
	std::vector<std::string> envValues;

	std::string logFile = "./smartbody.log";
	for (int i=1; i<argc; i++ )
	{
		SmartBody::util::log( "SmartBody ARG[%d]: '%s'", i, argv[i] );
		std::string s = argv[i];
		std::string mediapathstr;
		std::string fpsStr;
		if (s.size() > 11)
			mediapathstr = s.substr(0, 10);
		if (s.size() > 5)
			fpsStr = s.substr(0, 5);
			

		if( s == "-pythonpath" )  // argument -pythonpath
		{
			if( ++i < argc ) {
				SmartBody::util::log( "    Adding path path '%s'\n", argv[i] );
				python_lib_path = argv[i];
			} else {
				SmartBody::util::log( "ERROR: Expected directory path to follow -pythonpath\n" );
				// return -1
			}
		}
		else if( s.compare( 0, 6, "-host=" ) == 0 )  // argument starts with -host=
		{
			net_host = s;
			net_host.erase(0, 6);
		}
		else if( s == "-mepath" )  // -mepath <dirpath> to specify where Motion Engine files (.sk, .skm) should be loaded from
		{
			if( ++i < argc ) {
				SmartBody::util::log( "    Adding ME path '%s'\n", argv[i] );

				me_paths.emplace_back( argv[i] );
			} else {
				SmartBody::util::log( "ERROR: Expected directory path to follow -mepath\n" );
				// return -1
			}
		}
		else if( s == "-seqpath" || s == "-scriptpath" )  // -mepath <dirpath> to specify where sequence files (.seq) should be loaded from
		{
			if( ++i < argc ) {
				SmartBody::util::log( "    Adding sequence path '%s'\n", argv[i] );

				//seq_paths.emplace_back( argv[i] );
				py_paths.emplace_back( argv[i] );
			} else {
				SmartBody::util::log( "ERROR: Expected directory path to follow -seqpath\n" );
				// return -1
			}
		}
		else if( s == "-pypath" )  // -mepath <dirpath> to specify where sequence files (.seq) should be loaded from
		{
			if( ++i < argc ) {
				SmartBody::util::log( "    Adding python script path '%s'\n", argv[i] );

				py_paths.emplace_back( argv[i] );
			} else {
				SmartBody::util::log( "ERROR: Expected directory path to follow -pypath\n" );
				// return -1
			}
		}
		else if( s == "-audiopath" )  // -audiopath <dirpath> to specify where audio files (.wav and .bml) should be loaded from
		{
			if( ++i < argc ) {
				SmartBody::util::log( "    Addcore/smartbody/sbm/src/sbm_main.cpping audio path '%s'\n", argv[i] );

				audio_paths.emplace_back( argv[i] );
			} else {
				SmartBody::util::log( "ERROR: Expected directory path to follow -audiopath\n" );
				// return -1
			}
		}
		else if( s == "-seq" || s == "-script")  // -seq <filename> to load seq file (replaces old -initseq notation)
		{
			if( ++i < argc ) {
				std::string filename = argv[i];
				std::string::size_type idx;

				idx = filename.rfind('.');

				if(idx != std::string::npos)
				{
					std::string extension = filename.substr(idx+1);
					if (extension == "py")
					{
						SmartBody::util::log( "    Loading Python scrypt '%s'\n", argv[i] );
						init_pys.emplace_back( argv[i] );
					}
					else
					{
						SmartBody::util::log( "    Loading sequence '%s'\n", argv[i] );
						init_seqs.emplace_back( argv[i] );
					}
				}
				else
				{
					// No extension found
					SmartBody::util::log( "    Loading sequence '%s'\n", argv[i] );
					init_seqs.emplace_back( argv[i] );
				}
			} else {
				SmartBody::util::log( "ERROR: Expected filename to follow -seq\n" );
				// return -1
			}
		}
		else if( s == "-procid=" )  // argument starts with -procid=
		{
			proc_id = s;
			proc_id.erase( 0, 8 );
		}
		else if( s == "-audio" )  // argument equals -audio
		{
			 scene.setBoolAttribute("internalAudio", true);
		}
		else if( s == "-lockdt" )  // argument equals -lockdt
		{
			lock_dt_mode = true;
		}
		else if( fpsStr == "-fps="  )  // argument starts with -fps=
		{
			string fps = s;
			fps.erase( 0, 5 );
			sleepFPS = (float) atof( fps.c_str() );
		}
		else if( s == "-perf=" )  // argument starts with -perf=
		{
			string interval = s;
			interval.erase( 0, 6 );
			intervalAmount = (float) atof( interval.c_str() );
			
		}
		else if ( s == "-facebone" )
		{
			SmartBody::util::log("-facebone option has been deprecated.");
		}
		else if ( s == "-skscale=" )
		{
			string skScale = s;
			skScale.erase( 0, 9 );
			scene.getAssetManager()->setGlobalSkeletonScale(atof(skScale.c_str()));
		}
		else if ( s == "-skmscale=" )
		{
			string skmScale = s;
			skmScale.erase( 0, 10 );
			scene.getAssetManager()->setGlobalSkeletonScale(atof(skmScale.c_str()));
		}
		else if (mediapathstr == "-mediapath")
		{
			mediaPath = s.substr(11);
			scene.setMediaPath(mediaPath);
		}
        else if ( s == "-noninteractive")
        {
                isInteractive = false;
        }
		else if ( s == "-noeditor")
        {
                useEditor = false;
        }
		else if ( s == "-maximize")
        {
                maximize = true;
        }
		else if ( s == "-name")
        {
            if( ++i < argc )
			{
				windowName = argv[i];
			}
        }
		else if ( s == "-x")
		{
            if( ++i < argc )
			{
				locX = atoi(argv[i]);
			}
		}
		else if ( s == "-y")
		{
            if( ++i < argc )
			{
				locY = atoi(argv[i]);
			}
		}
		else if ( s == "-w")
		{
            if( ++i < argc )
			{
				locW = atoi(argv[i]);
			}
		}
		else if ( s == "-h")
		{
            if( ++i < argc )
			{
				locH = atoi(argv[i]);
			}
		}
		else if ( s == "-renderer=")
        {
				renderer = s;
				renderer.erase( 0, 10 );
        }
		else if (s == "-log")
		{
			if (++i < argc)
			{
				logFile = argv[i];
#ifdef WIN32
				logFile = SmartBody::util::replace(logFile, "\\", "/");
#endif
			}
		}
		else if ( s == "-env")
        {
			if( ++i < argc )
			{
				// environmental variables to be used during Python/scripting startup
				 std::string nameValuePair = argv[i];
				 std::vector<string> strs;
				 boost::split(strs, nameValuePair, boost::is_any_of("="));
				 if (!strs.empty())
					 envNames.emplace_back(strs[0]);
				 if (strs.size() > 1)
					 envValues.emplace_back(strs[1]);
				 else
					 envValues.emplace_back("");
			}
        }
		else
		{
			SmartBody::util::log( "ERROR: Unrecognized command line argument: \"%s\"\n", s.c_str() );
		}
	}

	// initialize python
	SmartBody::util::log("Initializing Python.");

	//SmartBody::SBScene::setSystemParameter("pythonlibpath", python_lib_path);
	initAutoRigPythonModule();
	initMiscPythonModule();
	initGUIInterfacePythonModule();
	pythonExtraModuleDeclarations.emplace_back([](){
		initPythonRenderer();
		initPythonNavmesh();
	});

	initPython();
	setupPython(scene);
	
	mcu_register_callbacks();

	//scene.addSceneListener(&fltkListener);

	SmartBody::util::log("Logging to file %s", logFile.c_str());
	scene.startFileLogging(logFile);


	scene.getSimulationManager()->setupTimer();

	if( lock_dt_mode )	{ 
		scene.getSimulationManager()->setSleepLock();
	}

	if (sleepFPS != -1.f)
		scene.getSimulationManager()->setSleepFps( sleepFPS) ;
	if (intervalAmount != -1.f)
		scene.getSimulationManager()->printPerf(intervalAmount);

	scene.setMediaPath(mediaPath);

// change the default font size
	FL_NORMAL_SIZE = 11;
//	FltkViewerFactory viewerFactory;
//	viewerFactory.setDefaultSize(locX, locY, locW, locH);
//	viewerFactory.setUseEditor(useEditor);
//	viewerFactory.setMaximize(maximize);
//	viewerFactory.setWindowName(windowName);


	if (maximize)
	{
		Fl::screen_xywh(locX, locY, locW, locH);
		//Why is this needed?
		locY += 10;
		locH -= 10;
	}

	BaseWindow baseWindow(useEditor, locX, locY, locW, locH, windowName.c_str());

	SmartBody::PythonInterface::getViewerFn = [&]() {
		return &baseWindow;
	};
	Session::current->renderScene.createCamera("cameraDefault");


	scene.getSpeechManager()->festivalRelay()->initSpeechRelay(festivalLibDir,festivalCacheDir);
	scene.getSpeechManager()->cereprocRelay()->initSpeechRelay(cereprocLibDir,festivalCacheDir);

#if LINK_VHMSG_CLIENT
	char * vhmsg_server = getenv( "VHMSG_SERVER" );
	char * vhmsg_port = getenv("VHMSG_PORT");
	bool vhmsg_disabled = ( vhmsg_server != nullptr && strcasecmp( vhmsg_server, "none" ) == 0 );  // hope there is no valid server named "none"
	std::string vhmsgServerStr;
	if (vhmsg_server)
		vhmsgServerStr = vhmsg_server;
	std::string vhmsgPortStr;
	if (vhmsg_port)
		vhmsgPortStr = vhmsg_port;


	auto& vhmsgManager = Session::current->vhmMsgManager;
	if( !vhmsg_disabled)
	{
		if (!vhmsgServerStr.empty())
			vhmsgManager.setServer(vhmsgServerStr);
		if (!vhmsgPortStr.empty())
			vhmsgManager.setPort(vhmsgPortStr);
		
		vhmsgManager.setEnable(true);
		if (!vhmsgManager.isEnable())
		{
			SmartBody::util::log("Could not connect to server %s, VHMSG service not enabled.", vhmsg_server);
		}
	}
	else
	{
		if( vhmsg_disabled )
		{
			SmartBody::util::log( "SmartBody: VHMSG_SERVER='%s': Messaging disabled.\n", vhmsg_server?"nullptr":vhmsg_server );
		} else {
#if 0 // disable server name query until vhmsg is fixed
			std::string vhserver = (vhmsg_server? vhmsg_server : "localhost");
			std::string vhport = (vhmsg_port ? vhmsg_port : "61616");
			SmartBody::util::log( "SmartBody Error: ttu_open FAILED\n" );
			SmartBody::util::log("Could not connect to %s:%s", vhserver.c_str(), vhport.c_str());
#endif
		}
		vhmsgManager.setEnable(false);
	}
#endif

	// Sets up the network connection for sending bone rotations over to the renderer
	
	if( !net_host.empty() )
	{
		Session::current->bonebusManager.setHost(net_host);
		Session::current->bonebusManager.setEnable(true);
	}

	if( !proc_id.empty() )
	{
		scene.setProcessId( proc_id );

		// Using a process id is a sign that we're running in a multiple SBM environment.
		// So.. ignore BML requests with unknown agents by default
		scene.getBmlProcessor()->getBMLProcessor()->set_warn_unknown_agents( false );
	}

	if (scene.getBoolAttribute("internalAudio"))
	{
		if ( !AUDIO_Init() )
		{
			SmartBody::util::log( "ERROR: Audio initialization failed\n" );
		}
	}

	Session::current->debuggerServer.setStringAttribute("id", "sbgui");

	// initialize any Python environment variables from the command line
	for (unsigned int x = 0; x < envNames.size(); x++)
	{
		std::stringstream strstr;
		strstr << envNames[x] << " = \"" << envValues[x] << "\"";
		SmartBody::util::log(strstr.str().c_str());
		scene.run(strstr.str());
	}

//	(void)signal( SIGABRT, signal_handler );
//	(void)signal( SIGFPE, signal_handler );
//	(void)signal( SIGILL, signal_handler );
//	(void)signal( SIGINT, signal_handler );
//	(void)signal( SIGSEGV, signal_handler );
//	(void)signal( SIGTERM, signal_handler );
#ifdef WIN32
	(void)signal( SIGBREAK, signal_handler );
#endif
//	atexit( exit_callback );

	gwiz::cmdl commandline;
	//commandline.set_callback( cmdl_tab_callback );

	vector<string>::iterator it;

	if( seq_paths.empty() && py_paths.empty() ) {
		SmartBody::util::log( "No script paths specified. Adding current working directory to script path.\n" );
		seq_paths.emplace_back( mediaPath + "/sbm-common/scripts" );
	}

	for( it = me_paths.begin();
	     it != me_paths.end();
		 ++it )
	{
		std::stringstream strstr;
		strstr << "path me " << it->c_str();
		scene.command( (char *) strstr.str().c_str() );
	}

	for( it = seq_paths.begin();
	     it != seq_paths.end();
		 ++it )
	{
		std::stringstream strstr;
		strstr << "path seq " << (it->c_str());
		scene.command( (char *) strstr.str().c_str() );
	}

	for( it = audio_paths.begin();
	     it != audio_paths.end();
		 ++it )
	{
		std::stringstream strstr;
		strstr <<  "path audio " << it->c_str();
		scene.command( (char *) strstr.str().c_str() );
	}


	for( it = py_paths.begin();
	     it != py_paths.end();
		 ++it )
	{
		std::stringstream strstr;
		strstr << "scene.addAssetPath('script', '" << it->c_str() << "')";
		scene.run( (char *) strstr.str().c_str() );
	}

	// run the specified scripts
	if( init_seqs.empty() && init_pys.empty())
	{
		SmartBody::util::log( "No Python scripts specified. Loading default configuration.\n" );
		scene.run("getViewer().show()\ngetCamera().reset()");
	}

	for( it = init_seqs.begin();
		 it != init_seqs.end();
		 ++it )
	{
		string seq_command = "seq " + (*it) + " begin";
		scene.command((char *)seq_command.c_str());
	}


	for( it = init_pys.begin();
		 it != init_pys.end();
		 ++it )
	{
		std::string cmd = *it;
		std::stringstream strstr;
		strstr << "scene.run(\"" << cmd.c_str() << "\")";
		scene.run(strstr.str());
		SmartBody::util::log("Run Script = %s", strstr.str().c_str());
	}
	SmartBody::util::log("After running init python script");
	me_paths.clear();
	seq_paths.clear();
	init_seqs.clear();

	// Notify world SBM is ready to receive messages
	srArgBuffer argBuff("");
	mcu_vrAllCall_func( argBuff, Session::current->vhmMsgManager );

	scene.getSimulationManager()->start();

#if ENABLE_808_TEST
	return( 0 );
#endif

#if EARLY_EXIT
	mcu.loop = 0;
#endif
//	commandline.render_prompt( "> " );

	std::string pythonPrompt = "# ";
	std::string commandPrompt = "> ";

	//scene.getCommandManager()->execute("viewer open");

	SrTimer timer;
	timer.start();
	double lastUICheckTime = -1.0;
	while((Session::current->scene.getSimulationManager()->isRunning()))	{


//		mcu.update_profiler( SBM_get_real_time() );
		bool update_sim = Session::current->scene.getSimulationManager()->updateTimer();
//		bool update_sim = mcu.update_timer( SBM_get_real_time() );

	//	mcu.mark( "main", 0, "fltk-check" );
		Fl::check();
		/*double curTime = timer.time();
		if (curTime - lastUICheckTime > .016)
		{
			lastUICheckTime = curTime;
			Fl::check();
		}*/

		auto& theScene = Session::current->scene;

#if LINK_VHMSG_CLIENT
		if (Session::current->vhmMsgManager.isEnable())
		{
			err = Session::current->vhmMsgManager.poll();
			if( err == CMD_FAILURE )	{
				fprintf( stderr, "ttu_poll ERROR\n" );
			}
		}
#endif

		vector<string> commands;// = mcu.bonebus.GetCommand();
		for (auto & command : commands) {
			theScene.command( (char *)command.c_str() );
		}

		if (isInteractive)
		{
			bool hasCommands = false;
			hasCommands =  commandline.pending( pythonPrompt );

			if ( hasCommands )
			{
				std::string cmd_str = commandline.read();
				char *cmd = (char*)cmd_str.c_str();

				if( strlen( cmd ) )	{

					bool result = Session::current->scene.run(cmd);

					if (!result)
					{
						printf("SmartBody Error: when running command: %s", cmd);
					}
				}
			}
		}

		if( update_sim )	{
			theScene.update();
		}
	}	
	
	cleanup();

	//vhcl::Log::g_log.RemoveAllListeners();
	//delete listener;
//	delete sbmWindow;
	// finally, delete SBScene
	Session::current = {};
//	return( 0 ); // NOT NEEDED ??
}

