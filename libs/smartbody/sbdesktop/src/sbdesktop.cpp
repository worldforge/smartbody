#define ENABLE_CMDL_TEST		0
#define ENABLE_808_TEST			0

#define SBM_REPORT_MEMORY_LEAKS  0
#define SBM_EMAIL_CRASH_REPORTS  1


#ifndef SB_NO_VHMSG
#include "vhmsg-tt.h"
#endif
#include "GL/glew.h"

#include <signal.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <fstream>

#include <sb/SBScene.h>
#include <sbm/sbm_constants.h>
#include <sbm/xercesc_utils.hpp>

#include <sb/SBSkeleton.h>
#include <sbm/mcontrol_callbacks.h>
#include <sbm/sbm_test_cmds.hpp>
#include <bml/bml_processor.hpp>
#include <sbm/remote_speech.h>
#include <sbm/sbm_audio.h>
#include <sbm/sbm_speech_audiofile.hpp>
#include <sbm/local_speech.h> // [BMLR]
#include <sbm/text_speech.h> // [BMLR]
#include <sbm/locomotion_cmds.hpp>
#include <sbm/time_regulator.h>
//#include "SBMWindow.h"
#include <sb/SBPython.h>
#include <sb/SBSteerManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBAssetManager.h>
#include <sbm/PPRAISteeringAgent.h>
#include "TransparentListener.h"
#include "TransparentViewer.h"
#include "sb/SBDebuggerServer.h"
#include "sb/SBSpeechManager.h"
#include "sb/SBVHMsgManager.h"
#include "sb/SBCommandManager.h"
#include "sb/SBBoneBusManager.h"
#include "sbm/GPU/SbmShader.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <sbm/sr_cmd_line.h>
#include "gwiz_cmdl.h"

#ifdef WIN32
#ifndef NOMINMAX
#define NOMINMAX        /* Don't defined min() and max() */
#endif
#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
#if SBM_REPORT_MEMORY_LEAKS
#include <malloc.h>
#include <crtdbg.h>
#endif
#endif

#if SBM_EMAIL_CRASH_REPORTS
#include <vhcl_crash.h>
#endif

//#include <FL/Fl_glut.h>
#include "sr/sr_model.h"

#define ENABLE_DEFAULT_BOOTSTRAP	(1) 

///////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32_LEAN_AND_MEAN
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
#else
#endif

using std::vector;
using std::string;

///////////////////////////////////////////////////////////////////////////////////


class TransparentViewerFactory : public SrViewerFactory
 {
	public:
		TransparentViewerFactory();

		//void setFltkViewer(FltkViewer* viewer);

		virtual SrViewer* create(int x, int y, int w, int h);
		virtual void remove(SrViewer* viewer);
		virtual void reset(SrViewer* viewer);

	private:
		static SrViewer* s_viewer;

 };

SrViewer* TransparentViewerFactory::s_viewer = NULL;

TransparentViewerFactory::TransparentViewerFactory()
{
	s_viewer = NULL;
}

SrViewer* TransparentViewerFactory::create(int x, int y, int w, int h)
{
	if (!s_viewer)
		s_viewer =new TransparentViewer(x, y, w, h, "SmartBody");
	return s_viewer;
}

void TransparentViewerFactory::remove(SrViewer* viewer)
{
	if (viewer && (viewer == s_viewer))
	{
		viewer->hide_viewer();
	}
}

void TransparentViewerFactory::reset(SrViewer* viewer)
{
}
int mcu_viewer_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr )
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	char *view_cmd = args.read_token();
	if( strcmp( view_cmd, "open" ) == 0 )
	{
		if( scene->getViewer() == NULL )
		{
			int argc = args.calc_num_tokens();
			int width = 1024;
			int height = 768;
			int px = 100;
			int py = 100;
			if( argc >= 4 )	{
				width = args.read_int();
				height = args.read_int();
				px = args.read_int();
				py = args.read_int();
			}

			if( scene->getViewer() == NULL )
			{
				if (!scene->getViewerFactory())
					return CMD_FAILURE;
				scene->setViewer(scene->getViewerFactory()->create( px, py, width, height));
				scene->getViewer()->label_viewer( "SmartBody Viewer" );
				SrCamera* camera = scene->createCamera("activeCamera");
				scene->getViewer()->set_camera( camera );
				//((FltkViewer*)viewer_p)->set_mcu(this);
				scene->getViewer()->show_viewer();
				if( scene->getRootGroup() )	{
					scene->getViewer()->root( scene->getRootGroup() );
				}
#if !defined (__ANDROID__) && !defined(SBM_IPHONE) && !defined(__native_client__)
				SbmShaderManager::singleton().setViewer(scene->getViewer());
#endif
				return( CMD_SUCCESS );
			}
			return( CMD_FAILURE );
		}
		else
		{
			scene->getViewer()->show_viewer();
		}
	}		
	else if( strcmp( view_cmd, "close" ) == 0 )
	{
		if( scene->getViewer() )
		{
			scene->getViewerFactory()->remove(scene->getViewer());
			scene->setViewer(NULL);
#if !defined (__ANDROID__) && !defined(SBM_IPHONE) && !defined(__native_client__)
			SbmShaderManager::singleton().setViewer(NULL);
#endif		
			return( CMD_SUCCESS );
		}
	}
	else if( strcmp( view_cmd, "show" ) == 0 )
	{
		if( scene->getViewer() )
		{
			scene->getViewer()->show_viewer();
			return( CMD_SUCCESS );
		}
	}
	else if( strcmp( view_cmd, "hide" ) == 0 )
	{
		if( scene->getViewer() )
		{
			scene->getViewer()->hide_viewer();
			return( CMD_SUCCESS );
		}
	}
	else
	{
		return( CMD_NOT_FOUND );
	}

	return( CMD_FAILURE );
}

/*

	camera	eye <x y z>
	camera	center <x y z>
#	camera	up <x y z>
#	camera	fovy <degrees>
	camera	scale <factor>
	camera	default [<preset>]

*/

int mcu_camera_func( srArgBuffer& args, SmartBody::SBCommandManager* cmdMgr )	{
	
	if( SmartBody::SBScene::getScene()->getViewer() )	{
		SrCamera* camera = SmartBody::SBScene::getScene()->getActiveCamera();
		if (!camera)
		{
			SmartBody::util::SmartBody::util::log("No active camera. Camera command not executed.");
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
				if (SmartBody::SBScene::getScene()->hasCameraTrack())
				{
					SmartBody::SBScene::getScene()->removeCameraTrack();
					SmartBody::util::SmartBody::util::log("Removing current tracked object.");
						return( CMD_SUCCESS );
				}
				SmartBody::util::SmartBody::util::log("Need to specify an object and a joint to track.");
				return( CMD_FAILURE );
			}
			SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(name);
			if (!pawn)
			{
				pawn = SmartBody::SBScene::getScene()->getCharacter(name);
				if (!pawn)
				{
					SmartBody::util::SmartBody::util::log("Object %s was not found, cannot track.", name);
					return( CMD_FAILURE );
				}
			}
			char* jointName = args.read_token();
			if (!jointName || strcmp(jointName, "") == 0)
			{
				SmartBody::util::SmartBody::util::log("Need to specify a joint to track.");
				return( CMD_FAILURE );
			}
			auto skeleton = NULL;
			skeleton = pawn->getSkeleton();

			SkJoint* joint = pawn->getSkeleton()->search_joint(jointName);
			if (!joint)
			{
				SmartBody::util::SmartBody::util::log("Could not find joint %s on object %s.", jointName, name);
				return( CMD_FAILURE );
			}
	
			bool hasTracks = SmartBody::SBScene::getScene()->hasCameraTrack();
			if (hasTracks)
			{
				SmartBody::SBScene::getScene()->removeCameraTrack();
				SmartBody::util::SmartBody::util::log("Removing current tracked object.");
			}
			SmartBody::SBScene::getScene()->setCameraTrack(name, joint->getName());
			return CMD_SUCCESS;			
		}
		else
		if( strcmp( cam_cmd, "default" ) == 0 )	{
			int preset = args.read_int();
				
			if( preset == 1 )	{
				SmartBody::SBScene::getScene()->getViewer()->view_all();
			}
		}
		else if (strcmp( cam_cmd, "reset" ) == 0 ) {
			float scale = 1.f/SmartBody::SBScene::getScene()->getScale();
			SrVec camEye = SrVec(0,1.66f,1.85f)*scale;
			SrVec camCenter = SrVec(0,0.92f,0)*scale;	
			float znear = 0.01f*scale;
			float zfar = 100.0f*scale;
			char camCommand[256];
			sprintf(camCommand,"camera eye %f %f %f",camEye[0],camEye[1],camEye[2]);				
			SmartBody::SBScene::getScene()->getCommandManager()->execute((char*)camCommand);
			sprintf(camCommand,"camera center %f %f %f",camCenter[0],camCenter[1],camCenter[2]);
			SmartBody::SBScene::getScene()->getCommandManager()->execute((char*)camCommand);			
			camera->setNearPlane(znear);
			camera->setFarPlane(zfar);

		}
		else if (strcmp( cam_cmd, "frame" ) == 0 ) {
			SrBox sceneBox;
			const std::vector<std::string>& pawnNames = SmartBody::SBScene::getScene()->getPawnNames();

			for (std::vector<std::string>::const_iterator iter = pawnNames.begin();
				iter != pawnNames.end();
				iter++)
			{
				SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(*iter);
				bool visible = pawn->getBoolAttribute("visible");
					if (!visible)
						continue;
				SrBox box = pawn->getSkeleton()->getBoundingBox();
				sceneBox.extend(box);
			}
			camera->view_all(sceneBox, camera->getFov());	
			float scale = 1.f/SmartBody::SBScene::getScene()->getScale();
			float znear = 0.01f*scale;
			float zfar = 100.0f*scale;
			camera->setNearPlane(znear);
			camera->setFarPlane(zfar);
		}
		return( CMD_SUCCESS );
	}

	return( CMD_FAILURE );
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
		for (std::vector<std::string>::const_iterator iter = characterNames.begin();
			iter != characterNames.end();
			iter++)
		{
			SmartBody::SBCharacter* character = scene->getCharacter((*iter));
			SmartBody::SBSteerAgent* steerAgent = SmartBody::SBScene::getScene()->getSteerManager()->getSteerAgent(character->getName());
			if (steerAgent)
			{
				PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
				ppraiAgent->setAgent(NULL);
			}
		}
	}
	return( CMD_SUCCESS );
}

void sbm_vhmsg_callback( const char *op, const char *args, void * user_data ) {
	// Replace singleton with a user_data pointer
	switch( SmartBody::SBScene::getScene()->getCommandManager()->execute( op, (char *)args ) ) {
        case CMD_NOT_FOUND:
            SmartBody::util::SmartBody::util::log("SmartBody error: command NOT FOUND: '%s' + '%s'", op, args );
            break;
        case CMD_FAILURE:
            SmartBody::util::SmartBody::util::log("SmartBody error: command FAILED: '%s' + '%s'", op, args );
            break;
    }

	SmartBody::SBScene::getScene()->getDebuggerServer()->ProcessVHMsgs(op, args);
}

int mcu_quit_func( srArgBuffer& args )
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	scene->getSimulationManager()->stop();

	if (scene->getSteerManager()->getEngineDriver()->isInitialized())
	{
		scene->getSteerManager()->getEngineDriver()->stopSimulation();
		scene->getSteerManager()->getEngineDriver()->unloadSimulation();
		scene->getSteerManager()->getEngineDriver()->finish();

		/*
		for (std::map<std::string, SbmCharacter*>::iterator iter = mcu_p->getCharacterMap().begin();
			iter != mcu_p->getCharacterMap().end();
			iter++)
		{
			SbmCharacter* character = (*iter).second;
			SmartBody::SBSteerAgent* steerAgent = SmartBody::SBScene::getScene()->getSteerManager()->getSteerAgent(character->getName());
			if (steerAgent)
			{
				PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
				ppraiAgent->setAgent(NULL);
			}
		}
		*/
	}
	return( CMD_SUCCESS );
}

void mcu_register_callbacks() {
	
	SmartBody::SBCommandManager* cmdMgr = SmartBody::SBScene::getScene()->getCommandManager();
	
	// additional commands associated with this viewer
	cmdMgr->insert( "q",			mcu_quit_func );
	cmdMgr->insert( "quit",			mcu_quit_func );
//	mcu.insert( "snapshot",		mcu_snapshot_func );
	cmdMgr->insert( "viewer",		mcu_viewer_func );
	cmdMgr->insert( "camera",		mcu_camera_func );
}

void cleanup()	{
	{

		
		if (SmartBody::SBScene::getScene()->getSimulationManager()->isStopped())
		{
			SmartBody::util::SmartBody::util::log( "SmartBody NOTE: unexpected exit " );
		}

		if (SmartBody::SBScene::getScene()->getBoolAttribute("internalAudio"))
		{
			AUDIO_Close();
		}

		SmartBody::SBScene::getScene()->getVHMsgManager()->send("vrProcEnd sbm");

#if LINK_VHMSG_CLIENT
		if (SmartBody::SBScene::getScene()->getVHMsgManager()->isEnable())
		{
			SmartBody::SBScene::getScene()->getVHMsgManager()->disconnect();
		}
#endif
	}
	
	XMLPlatformUtils::Terminate();

	SmartBody::util::SmartBody::util::log( "SBM: terminated gracefully." );


#if SBM_REPORT_MEMORY_LEAKS
	_CrtMemState  state;
	_CrtMemCheckpoint( &state );
	_CrtMemDumpStatistics( &state );
#endif
}

void signal_handler(int sig) {
//	std::cout << "SmartBody shutting down after catching signal " << sig << std::endl;

	
	SmartBody::SBScene::getScene()->getVHMsgManager()->send( "vrProcEnd sbm" );
	// get the current directory
#ifdef WIN32
	char buffer[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, buffer);
#else
	char buffer[PATH_MAX];
	getcwd(buffer, PATH_MAX);
#endif
}


class SBMCrashCallback : public vhcl::Crash::CrashCallback
{
   public:
      virtual void OnCrash() { signal_handler( -1 ); }
};


///////////////////////////////////////////////////////////////////////////////////
int WINAPI _tWinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str,int nWinMode)
{
//int main( int argc, char **argv )	{

	TCHAR cmdline[4096] ;
    TCHAR* argv[4096] ;
    int argc = 0 ;
    _tcscpy( cmdline, GetCommandLine() ) ;
    argv[argc] = _tcstok( cmdline, TEXT(" \t") ) ;
    while( argv[argc] != 0 )
    {
        argc++ ;
        argv[argc] = _tcstok( 0, TEXT(" \t") ) ;
    }

	int w = 320;
	int h = 240;
	int x = 200;
	int y = 150;


#if SBM_EMAIL_CRASH_REPORTS
	// Log crashes, with a dialog option to report via email
	vhcl::Crash::EnableExceptionHandling( true );
	vhcl::Crash::AddCrashCallback( new SBMCrashCallback() );
#endif

	// register the log listener
	vhcl::Log::StdoutListener* listener = new vhcl::Log::StdoutListener();
	vhcl::Log::g_log.AddListener(listener);

	vhcl::Log::FileListener* fileListener = new vhcl::Log::FileListener("smartbody.log");
	vhcl::Log::g_log.AddListener(fileListener);

	int err;
	SrString net_host;
	vector<string> seq_paths;
	vector<string> py_paths;
	vector<string> me_paths;
	vector<string> audio_paths;
	vector<string> init_seqs;
	vector<string> init_pys;
	SrString proc_id;
	
	XMLPlatformUtils::Initialize();  // Initialize Xerces before creating MCU

	

	TransparentListener transparentListener;
	SmartBody::SBScene::getScene()->addSceneListener(&transparentListener);
	
	mcu_register_callbacks();

	SmartBody::SBScene::getScene()->getSimulationManager()->setupTimer();

	std::string python_lib_path = "../../../../libs/smartbody/Python27/Lib";
	std::string festivalLibDir = "../../../../lib/festival/festival/lib/";
	std::string festivalCacheDir = "../../../../data/cache/festival/";

	// EDF - taken from tre_main.cpp, a fancier command line parser can be put here if desired.
	//	check	command line parameters:
	bool lock_dt_mode = false;
	bool isInteractive = true;
	int i;
	SrString	s;
	for (	i=1; i<argc; i++ )
	{
		SmartBody::util::SmartBody::util::log( "SmartBody argument [%d]: '%s'", i, argv[i] );
		s = argv[i];

		if( s == "-pythonpath" )  // argument -pythonpath
		{
			if( ++i < argc ) {
				SmartBody::util::SmartBody::util::log( "    Adding path path '%s'\n", argv[i] );
				python_lib_path = argv[i];
			} else {
				SmartBody::util::SmartBody::util::log( "ERROR: Expected directory path to follow -pythonpath\n" );
				// return -1
			}
		}
		else if( s.search(	"-host=" ) == 0 )  // argument starts with -host=
		{
			net_host = s;
			net_host.remove( 0, 6 );
		}
		else if( s == "-mepath" )  // -mepath <dirpath> to specify where Motion Engine files (.sk, .skm) should be loaded from
		{
			if( ++i < argc ) {
				SmartBody::util::SmartBody::util::log( "    Adding ME path '%s'\n", argv[i] );

				me_paths.emplace_back( argv[i] );
			} else {
				SmartBody::util::SmartBody::util::log( "ERROR: Expected directory path to follow -mepath\n" );
				// return -1
			}
		}
		else if( s == "-seqpath" || s == "-scriptpath" )  // -mepath <dirpath> to specify where sequence files (.seq) should be loaded from
		{
			if( ++i < argc ) {
				SmartBody::util::SmartBody::util::log( "    Adding sequence path '%s'\n", argv[i] );

				seq_paths.emplace_back( argv[i] );
				py_paths.emplace_back( argv[i] );
			} else {
				SmartBody::util::SmartBody::util::log( "ERROR: Expected directory path to follow -seqpath\n" );
				// return -1
			}
		}
		else if( s == "-pypath" )  // -mepath <dirpath> to specify where sequence files (.seq) should be loaded from
		{
			if( ++i < argc ) {
				SmartBody::util::SmartBody::util::log( "    Adding python script path '%s'\n", argv[i] );

				py_paths.emplace_back( argv[i] );
			} else {
				SmartBody::util::SmartBody::util::log( "ERROR: Expected directory path to follow -pypath\n" );
				// return -1
			}
		}
		else if( s == "-audiopath" )  // -audiopath <dirpath> to specify where audio files (.wav and .bml) should be loaded from
		{
			if( ++i < argc ) {
				SmartBody::util::SmartBody::util::log( "    Addcore/smartbody/sbm/src/sbm_main.cpping audio path '%s'\n", argv[i] );

				audio_paths.emplace_back( argv[i] );
			} else {
				SmartBody::util::SmartBody::util::log( "ERROR: Expected directory path to follow -audiopath\n" );
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
						SmartBody::util::SmartBody::util::log( "    Loading Python scrypt '%s'\n", argv[i] );
						init_pys.emplace_back( argv[i] );
					}
					else
					{
						SmartBody::util::SmartBody::util::log( "    Loading sequence '%s'\n", argv[i] );
						init_seqs.emplace_back( argv[i] );
					}
				}
				else
				{
					// No extension found
					SmartBody::util::SmartBody::util::log( "    Loading sequence '%s'\n", argv[i] );
					init_seqs.emplace_back( argv[i] );
				}
			} else {
				SmartBody::util::SmartBody::util::log( "ERROR: Expected filename to follow -seq\n" );
				// return -1
			}
		}
		else if( s.search( "-procid=" ) == 0 )  // argument starts with -procid=
		{
			proc_id = s;
			proc_id.remove( 0, 8 );
		}
		else if( s.search( "-audio" ) == 0 )  // argument equals -audio
		{
			 SmartBody::SBScene::getScene()->setBoolAttribute("internalAudio", true);
		}
		else if( s.search( "-lockdt" ) == 0 )  // argument equals -lockdt
		{
			lock_dt_mode = true;
		}
		else if( s.search( "-fps=" ) == 0 )  // argument starts with -fps=
		{
			SrString fps = s;
			fps.remove( 0, 5 );
			SmartBody::SBScene::getScene()->getSimulationManager()->setSleepFps( (float)atof( fps ) );
		}
		else if ( s.search( "-facebone" ) == 0 )
		{
			SmartBody::util::SmartBody::util::log("-facebone option has been deprecated.");
		}
		else if ( s.search( "-skscale=" ) == 0 )
		{
			SrString skScale = s;
			skScale.remove( 0, 9 );
			SmartBody::SBScene::getScene()->getAssetManager()->setDoubleAttribute("globalSkeletonScale()", atof(skScale));
		}
		else if ( s.search( "-skmscale=" ) == 0 )
		{
			SrString skmScale = s;
			skmScale.remove( 0, 10 );
			SmartBody::SBScene::getScene()->getAssetManager()->setDoubleAttribute("globalMotionScale()", atof(skmScale));
		}
		else if ( s.search( "-mediapath=" ) == 0 )
		{
			std::string mediaPath = (const char*) s;
			mediaPath = mediaPath.substr(11);
			SmartBody::SBScene::getScene()->setMediaPath(mediaPath);
		}
        else if ( s.search ("-noninteractive") == 0)
        {
              isInteractive = false;
        }
		else if ( s.search ("-x") == 0)
        {
			i++;
			x = atoi(argv[i]);
               
        }
		else if ( s.search ("-y") == 0)
        {
			i++;
			y = atoi(argv[i]);
               
        }
		else if ( s.search ("-w") == 0)
        {
            i++;
			w = atoi(argv[i]);
        }
		else if ( s.search ("-h") == 0)
        {
			i++;
			h = atoi(argv[i]);    
        }

		else
		{
			SmartBody::util::SmartBody::util::log( "ERROR: Unrecognized command line argument: \"%s\"\n", (const char*)s );
		}
	}
	if( lock_dt_mode )	{ 
		SmartBody::SBScene::getScene()->getSimulationManager()->setSleepLock();
	}

	TransparentViewerFactory* viewerFactory = new TransparentViewerFactory();
	SmartBody::SBScene::getScene()->setViewerFactory(viewerFactory);
	TransparentViewer* viewer = dynamic_cast<TransparentViewer*>(viewerFactory->create(x, y, w, h));
	SbmShaderManager::singleton().setViewer(viewer);
	viewer->init(hThisInst, hPrevInst, str, nWinMode);
	viewer->resizeViewer();
	viewer->root( SmartBody::SBScene::getScene()->getRootGroup() );
	SmartBody::SBScene::getScene()->setViewer(viewer);

	// initialize python
	initPython(python_lib_path);

	SmartBody::SBScene::getScene()->getSpeechManager()->festivalRelay()->initSpeechRelay(festivalLibDir,festivalCacheDir);



#if LINK_VHMSG_CLIENT
	char * vhmsg_server = getenv( "VHMSG_SERVER" );
	char * vhmsg_port = getenv("VHMSG_PORT");
	bool vhmsg_disabled = ( vhmsg_server != NULL && strcasecmp( vhmsg_server, "none" ) == 0 );  // hope there is no valid server named "none"

	if( !vhmsg_disabled &&
		vhmsg::ttu_open()==vhmsg::TTU_SUCCESS )
	{
		vhmsg::ttu_set_client_callback( sbm_vhmsg_callback );
		err = vhmsg::ttu_register( "sb" );
		err = vhmsg::ttu_register( "sbm" );
		err = vhmsg::ttu_register( "vrAgentBML" );
		err = vhmsg::ttu_register( "vrExpress" );
		err = vhmsg::ttu_register( "vrSpeak" );
		err = vhmsg::ttu_register( "RemoteSpeechReply" );
		err = vhmsg::ttu_register( "PlaySound" );
		err = vhmsg::ttu_register( "StopSound" );
		err = vhmsg::ttu_register( "CommAPI" );
		err = vhmsg::ttu_register( "object-data" );
		err = vhmsg::ttu_register( "vrAllCall" );
		err = vhmsg::ttu_register( "vrKillComponent" );
		err = vhmsg::ttu_register( "wsp" );
		err = vhmsg::ttu_register( "receiver" );
		err = vhmsg::ttu_register( "sbmdebugger" );
		err = vhmsg::ttu_register( "vrPerception" );
		err = vhmsg::ttu_register( "vrBCFeedback" );
		err = vhmsg::ttu_register( "vrSpeech" );

		SmartBody::SBScene::getScene()->getVHMsgManager()->setEnable(true);
	} else {
		if( vhmsg_disabled ) {
			SmartBody::util::SmartBody::util::log( "SBM: VHMSG_SERVER='%s': Messaging disabled.\n", vhmsg_server?"NULL":vhmsg_server );
		} else {
#if 0 // disable server name query until vhmsg is fixed
			const char* vhmsg_server_actual = vhmsg::ttu_get_server();
			SmartBody::util::SmartBody::util::log( "SmartBody error: ttu_open VHMSG_SERVER='%s' FAILED\n", vhmsg_server_actual?"NULL":vhmsg_server_actual );
#else
			std::string vhserver = (vhmsg_server? vhmsg_server : "localhost");
			std::string vhport = (vhmsg_port ? vhmsg_port : "61616");
			SmartBody::util::SmartBody::util::log( "SmartBody error: ttu_open FAILED\n" );
			SmartBody::util::SmartBody::util::log("Could not connect to %s:%s", vhserver.c_str(), vhport.c_str());
#endif
		}
		SmartBody::SBScene::getScene()->getVHMsgManager()->setEnable(false);
	}
#endif

	// Sets up the network connection for sending bone rotations over to Unreal
	if( net_host != "" )
	{
		SmartBody::SBScene::getScene()->getBoneBusManager()->setHost((const char*)net_host);
		SmartBody::SBScene::getScene()->getBoneBusManager()->setEnable(true);
	}

	if( proc_id != "" ) {
		SmartBody::SBScene::getScene()->setProcessId( (const char*)proc_id );

		// Using a process id is a sign that we're running in a multiple SBM environment.
		// So.. ignore BML requests with unknown agents by default
		SmartBody::SBScene::getScene()->getBmlProcessor()->getBMLProcessor()->set_warn_unknown_agents( false );
	}

	if ( SmartBody::SBScene::getScene()->getBoolAttribute("internalAudio"))
	{
		if ( !AUDIO_Init() )
		{
			SmartBody::util::SmartBody::util::log( "ERROR: Audio initialization failed\n" );
		}
	}

	SmartBody::SBScene::getScene()->getDebuggerServer()->setStringAttribute("id", "sbdesktop");


#ifdef WIN32
	(void)signal( SIGBREAK, signal_handler );
#endif


#if ENABLE_DEFAULT_BOOTSTRAP
	vector<string>::iterator it;

	if( seq_paths.empty() && py_paths.empty() ) {
		SmartBody::util::SmartBody::util::log( "No script paths specified. Adding current working directory to script path.\n" );
		py_paths.emplace_back( "." );
	}

	for( it = me_paths.begin();
	     it != me_paths.end();
		 ++it )
	{
		std::stringstream strstr;
		strstr << "path me " << it->c_str();
		SmartBody::SBScene::getScene()->command( (char *) strstr.str().c_str() );
	}

	for( it = seq_paths.begin();
	     it != seq_paths.end();
		 ++it )
	{
		std::stringstream strstr;
		strstr << "path seq " << (it->c_str());
		SmartBody::SBScene::getScene()->command( (char *) strstr.str().c_str() );
	}

	for( it = audio_paths.begin();
	     it != audio_paths.end();
		 ++it )
	{
		std::stringstream strstr;
		strstr <<  "path audio " << it->c_str();
		SmartBody::SBScene::getScene()->command( (char *) strstr.str().c_str() );
	}


	for( it = py_paths.begin();
	     it != py_paths.end();
		 ++it )
	{
		std::stringstream strstr;
		strstr << "scene.addAssetPath('script', '" << it->c_str() << "')";
		SmartBody::SBScene::getScene()->run( (char *) strstr.str().c_str() );
	}

	// run the specified scripts
	if( init_seqs.empty() && init_pys.empty())
	{
		SmartBody::util::SmartBody::util::log( "No Python scripts specified. Loading script 'default.py'\n");
		init_pys.emplace_back( "default.py" );
	}

	for( it = init_seqs.begin();
		 it != init_seqs.end();
		 ++it )
	{
		SrString seq_command = SrString( "seq " ) << (it->c_str()) << " begin";
		SmartBody::SBScene::getScene()->getCommandManager()->execute( (char *)(const char *)seq_command );
	}
	
	SrCamera* camera = SmartBody::SBScene::getScene()->createCamera("cameraDefault");
	camera->reset();

	for( it = init_pys.begin();
		 it != init_pys.end();
		 ++it )
	{
		std::string cmd = it->c_str();
		std::stringstream strstr;
		strstr << "scene.run(\"" << cmd.c_str() << "\")";
		SmartBody::SBScene::getScene()->run(strstr.str().c_str());
	}

	me_paths.clear();
	seq_paths.clear();
	init_seqs.clear();
#endif

	// Notify world SBM is ready to receive messages
	srArgBuffer argBuff("");
	mcu_vrAllCall_func( argBuff, SmartBody::SBScene::getScene()->getCommandManager() );

	SmartBody::SBScene::getScene()->getSimulationManager()->start();

#if ENABLE_808_TEST
	return( 0 );
#endif

#if EARLY_EXIT
	mcu.loop = 0;
#endif
	std::string pythonPrompt = "# ";
	std::string commandPrompt = "> ";
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

	while( scene->getSimulationManager()->isRunning())
	{

		scene->getProfiler()->updateProfiler(scene->getSimulationManager()->getTime());
		bool update_sim = scene->getSimulationManager()->updateTimer();
		

		MSG msg;

		 while (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		 {
			if (msg.message == WM_QUIT)
			{
				scene->getSimulationManager()->stop();
				break;
			}
            if (GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
		 }

#if LINK_VHMSG_CLIENT
		 if( scene->getVHMsgManager()->isEnable() )	{
			err = vhmsg::ttu_poll();
			if( err == vhmsg::TTU_ERROR )	{
				fprintf( stderr, "ttu_poll ERROR\n" );
			}
		}
#endif

		vector<string> commands;// = mcu.bonebus.GetCommand();
		for ( size_t i = 0; i < commands.size(); i++ ) {
			scene->getCommandManager()->execute((char *)commands[i].c_str() );
		}

		if( update_sim )	{
			scene->update();
		}
		
	}	
	cleanup();

}

