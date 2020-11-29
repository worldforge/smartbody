//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file SimulationOptions.cpp
/// @brief Implements SimulationOptions functionality, and defines defaults for options that are not specified by the user.
///
/// @todo
///  - update documentation in this file.
///  - remove dependency on the glfw.h file
///


//====================================
//
// * STOP! *
//
// * READ THIS! *
//
/// @file SimulationOptions.cpp
///
/// <b>If you want to change defaults for yourself, generate and edit an xml config file, don't change this file.</b>
///
/// If you really must change the hard-coded defaults, this is the file to edit.
///
/// To add an option, do the following:
///  -# Add a default-value macro in this .cpp file, which will be the hard-coded default value.
///  -# Add a variable inside the SimulationOptions class, probably inside one of the existing structs.
///  -# Initialize the hard-coded value in the constructor
///  -# Add code in _setupXMLStructure() that defines the option for XML parsing/generating.
///
//====================================

#include <fstream>
#include <algorithm>
#include "simulation/SimulationOptions.h"
#include "util/Misc.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#if defined (TARGET_OS_IPHONE)  || defined (TARGET_IPHONE_SIMULATOR)
#ifndef BUILD_IPHONE
#define BUILD_IPHONE
#endif
#endif
#endif

#ifndef BUILD_IPHONE
#if !defined(__native_client__) && !defined(__FLASHPLAYER__)
//#include "glfw/include/GL/glfw.h"


//========================================================================
// Input handling definitions
//========================================================================

// Key and button state/action definitions
#define GLFW_RELEASE            0
#define GLFW_PRESS              1

// Keyboard key definitions: 8-bit ISO-8859-1 (Latin 1) encoding is used
// for printable keys (such as A-Z, 0-9 etc), and values above 256
// represent special (non-printable) keys (e.g. F1, Page Up etc).
#define GLFW_KEY_UNKNOWN      -1
#define GLFW_KEY_SPACE        32
#define GLFW_KEY_SPECIAL      256
#define GLFW_KEY_ESC          (GLFW_KEY_SPECIAL+1)
#define GLFW_KEY_F1           (GLFW_KEY_SPECIAL+2)
#define GLFW_KEY_F2           (GLFW_KEY_SPECIAL+3)
#define GLFW_KEY_F3           (GLFW_KEY_SPECIAL+4)
#define GLFW_KEY_F4           (GLFW_KEY_SPECIAL+5)
#define GLFW_KEY_F5           (GLFW_KEY_SPECIAL+6)
#define GLFW_KEY_F6           (GLFW_KEY_SPECIAL+7)
#define GLFW_KEY_F7           (GLFW_KEY_SPECIAL+8)
#define GLFW_KEY_F8           (GLFW_KEY_SPECIAL+9)
#define GLFW_KEY_F9           (GLFW_KEY_SPECIAL+10)
#define GLFW_KEY_F10          (GLFW_KEY_SPECIAL+11)
#define GLFW_KEY_F11          (GLFW_KEY_SPECIAL+12)
#define GLFW_KEY_F12          (GLFW_KEY_SPECIAL+13)
#define GLFW_KEY_F13          (GLFW_KEY_SPECIAL+14)
#define GLFW_KEY_F14          (GLFW_KEY_SPECIAL+15)
#define GLFW_KEY_F15          (GLFW_KEY_SPECIAL+16)
#define GLFW_KEY_F16          (GLFW_KEY_SPECIAL+17)
#define GLFW_KEY_F17          (GLFW_KEY_SPECIAL+18)
#define GLFW_KEY_F18          (GLFW_KEY_SPECIAL+19)
#define GLFW_KEY_F19          (GLFW_KEY_SPECIAL+20)
#define GLFW_KEY_F20          (GLFW_KEY_SPECIAL+21)
#define GLFW_KEY_F21          (GLFW_KEY_SPECIAL+22)
#define GLFW_KEY_F22          (GLFW_KEY_SPECIAL+23)
#define GLFW_KEY_F23          (GLFW_KEY_SPECIAL+24)
#define GLFW_KEY_F24          (GLFW_KEY_SPECIAL+25)
#define GLFW_KEY_F25          (GLFW_KEY_SPECIAL+26)
#define GLFW_KEY_UP           (GLFW_KEY_SPECIAL+27)
#define GLFW_KEY_DOWN         (GLFW_KEY_SPECIAL+28)
#define GLFW_KEY_LEFT         (GLFW_KEY_SPECIAL+29)
#define GLFW_KEY_RIGHT        (GLFW_KEY_SPECIAL+30)
#define GLFW_KEY_LSHIFT       (GLFW_KEY_SPECIAL+31)
#define GLFW_KEY_RSHIFT       (GLFW_KEY_SPECIAL+32)
#define GLFW_KEY_LCTRL        (GLFW_KEY_SPECIAL+33)
#define GLFW_KEY_RCTRL        (GLFW_KEY_SPECIAL+34)
#define GLFW_KEY_LALT         (GLFW_KEY_SPECIAL+35)
#define GLFW_KEY_RALT         (GLFW_KEY_SPECIAL+36)
#define GLFW_KEY_TAB          (GLFW_KEY_SPECIAL+37)
#define GLFW_KEY_ENTER        (GLFW_KEY_SPECIAL+38)
#define GLFW_KEY_BACKSPACE    (GLFW_KEY_SPECIAL+39)
#define GLFW_KEY_INSERT       (GLFW_KEY_SPECIAL+40)
#define GLFW_KEY_DEL          (GLFW_KEY_SPECIAL+41)
#define GLFW_KEY_PAGEUP       (GLFW_KEY_SPECIAL+42)
#define GLFW_KEY_PAGEDOWN     (GLFW_KEY_SPECIAL+43)
#define GLFW_KEY_HOME         (GLFW_KEY_SPECIAL+44)
#define GLFW_KEY_END          (GLFW_KEY_SPECIAL+45)
#define GLFW_KEY_KP_0         (GLFW_KEY_SPECIAL+46)
#define GLFW_KEY_KP_1         (GLFW_KEY_SPECIAL+47)
#define GLFW_KEY_KP_2         (GLFW_KEY_SPECIAL+48)
#define GLFW_KEY_KP_3         (GLFW_KEY_SPECIAL+49)
#define GLFW_KEY_KP_4         (GLFW_KEY_SPECIAL+50)
#define GLFW_KEY_KP_5         (GLFW_KEY_SPECIAL+51)
#define GLFW_KEY_KP_6         (GLFW_KEY_SPECIAL+52)
#define GLFW_KEY_KP_7         (GLFW_KEY_SPECIAL+53)
#define GLFW_KEY_KP_8         (GLFW_KEY_SPECIAL+54)
#define GLFW_KEY_KP_9         (GLFW_KEY_SPECIAL+55)
#define GLFW_KEY_KP_DIVIDE    (GLFW_KEY_SPECIAL+56)
#define GLFW_KEY_KP_MULTIPLY  (GLFW_KEY_SPECIAL+57)
#define GLFW_KEY_KP_SUBTRACT  (GLFW_KEY_SPECIAL+58)
#define GLFW_KEY_KP_ADD       (GLFW_KEY_SPECIAL+59)
#define GLFW_KEY_KP_DECIMAL   (GLFW_KEY_SPECIAL+60)
#define GLFW_KEY_KP_EQUAL     (GLFW_KEY_SPECIAL+61)
#define GLFW_KEY_KP_ENTER     (GLFW_KEY_SPECIAL+62)
#define GLFW_KEY_LAST         GLFW_KEY_KP_ENTER

// Mouse button definitions
#define GLFW_MOUSE_BUTTON_1      0
#define GLFW_MOUSE_BUTTON_2      1
#define GLFW_MOUSE_BUTTON_3      2
#define GLFW_MOUSE_BUTTON_4      3
#define GLFW_MOUSE_BUTTON_5      4
#define GLFW_MOUSE_BUTTON_6      5
#define GLFW_MOUSE_BUTTON_7      6
#define GLFW_MOUSE_BUTTON_8      7
#define GLFW_MOUSE_BUTTON_LAST   GLFW_MOUSE_BUTTON_8

// Mouse button aliases
#define GLFW_MOUSE_BUTTON_LEFT   GLFW_MOUSE_BUTTON_1
#define GLFW_MOUSE_BUTTON_RIGHT  GLFW_MOUSE_BUTTON_2
#define GLFW_MOUSE_BUTTON_MIDDLE GLFW_MOUSE_BUTTON_3


// Joystick identifiers
#define GLFW_JOYSTICK_1          0
#define GLFW_JOYSTICK_2          1
#define GLFW_JOYSTICK_3          2
#define GLFW_JOYSTICK_4          3
#define GLFW_JOYSTICK_5          4
#define GLFW_JOYSTICK_6          5
#define GLFW_JOYSTICK_7          6
#define GLFW_JOYSTICK_8          7
#define GLFW_JOYSTICK_9          8
#define GLFW_JOYSTICK_10         9
#define GLFW_JOYSTICK_11         10
#define GLFW_JOYSTICK_12         11
#define GLFW_JOYSTICK_13         12
#define GLFW_JOYSTICK_14         13
#define GLFW_JOYSTICK_15         14
#define GLFW_JOYSTICK_16         15
#define GLFW_JOYSTICK_LAST       GLFW_JOYSTICK_16


//========================================================================
// Other definitions
//========================================================================

// glfwOpenWindow modes
#define GLFW_WINDOW               0x00010001
#define GLFW_FULLSCREEN           0x00010002

// glfwGetWindowParam tokens
#define GLFW_OPENED               0x00020001
#define GLFW_ACTIVE               0x00020002
#define GLFW_ICONIFIED            0x00020003
#define GLFW_ACCELERATED          0x00020004
#define GLFW_RED_BITS             0x00020005
#define GLFW_GREEN_BITS           0x00020006
#define GLFW_BLUE_BITS            0x00020007
#define GLFW_ALPHA_BITS           0x00020008
#define GLFW_DEPTH_BITS           0x00020009
#define GLFW_STENCIL_BITS         0x0002000A

// The following constants are used for both glfwGetWindowParam
// and glfwOpenWindowHint
#define GLFW_REFRESH_RATE         0x0002000B
#define GLFW_ACCUM_RED_BITS       0x0002000C
#define GLFW_ACCUM_GREEN_BITS     0x0002000D
#define GLFW_ACCUM_BLUE_BITS      0x0002000E
#define GLFW_ACCUM_ALPHA_BITS     0x0002000F
#define GLFW_AUX_BUFFERS          0x00020010
#define GLFW_STEREO               0x00020011
#define GLFW_WINDOW_NO_RESIZE     0x00020012
#define GLFW_FSAA_SAMPLES         0x00020013

// glfwEnable/glfwDisable tokens
#define GLFW_MOUSE_CURSOR         0x00030001
#define GLFW_STICKY_KEYS          0x00030002
#define GLFW_STICKY_MOUSE_BUTTONS 0x00030003
#define GLFW_SYSTEM_KEYS          0x00030004
#define GLFW_KEY_REPEAT           0x00030005
#define GLFW_AUTO_POLL_EVENTS     0x00030006

// glfwWaitThread wait modes
#define GLFW_WAIT                 0x00040001
#define GLFW_NOWAIT               0x00040002

// glfwGetJoystickParam tokens
#define GLFW_PRESENT              0x00050001
#define GLFW_AXES                 0x00050002
#define GLFW_BUTTONS              0x00050003

// glfwReadImage/glfwLoadTexture2D flags
#define GLFW_NO_RESCALE_BIT       0x00000001 // Only for glfwReadImage
#define GLFW_ORIGIN_UL_BIT        0x00000002
#define GLFW_BUILD_MIPMAPS_BIT    0x00000004 // Only for glfwLoadTexture2D
#define GLFW_ALPHA_MAP_BIT        0x00000008

// Time spans longer than this (seconds) are considered to be infinity
#define GLFW_INFINITY 100000.0

#endif
#endif

using namespace SteerLib;
using namespace Util;


//====================================
// KEYBOARD-ACTION MAPPINGS
//====================================
#ifndef BUILD_IPHONE



// GUI ACTIONS
#define DEFAULT_KEY_QUIT                  GLFW_KEY_ESC
#define DEFAULT_KEY_PRINT_CAMERA_INFO     'P'
#define DEFAULT_KEY_TOGGLE_ANTIALIASING   'A'
#define DEFAULT_KEY_TAKE_SCREENSHOT       GLFW_KEY_F12
#define DEFAULT_START_DUMPING_FRAMES      GLFW_KEY_HOME
#define DEFAULT_STOP_DUMPING_FRAMES       GLFW_KEY_END
// ENGINE ACTIONS
#define DEFAULT_KEY_PAUSE                 GLFW_KEY_SPACE
#define DEFAULT_KEY_STEP_ONE_FRAME        GLFW_KEY_RIGHT
// REC FILE PLAYER MODULE ACTIONS
#define DEFAULT_KEY_SPEEDUP_PLAYBACK      '='
#define DEFAULT_KEY_SLOWDOWN_PLAYBACK     '-'
#define DEFAULT_KEY_RESET_PLAYBACK_SPEED  GLFW_KEY_BACKSPACE
#define DEFAULT_KEY_STEP_BACKWARD         GLFW_KEY_LEFT
#define DEFAULT_KEY_RESTART_PLAYBACK      'R'

//====================================
// MOUSE-ACTION MAPPINGS
//====================================
#define DEFAULT_MOUSE_BUTTON_TO_SELECT_AGENT   GLFW_MOUSE_BUTTON_LEFT
#define DEFAULT_MOUSE_BUTTON_TO_MOVE_CAMERA    GLFW_MOUSE_BUTTON_MIDDLE
#define DEFAULT_MOUSE_BUTTON_TO_ROTATE_CAMERA  GLFW_MOUSE_BUTTON_LEFT
#define DEFAULT_MOUSE_BUTTON_TO_ZOOM_CAMERA    GLFW_MOUSE_BUTTON_RIGHT
#endif

//====================================
// GUI DEFAULTS
//====================================
#define DEFAULT_ANTIALIASING true
#define DEFAULT_VSYNC false
#define DEFAULT_CAN_USE_MOUSE_SELECTION true
#define DEFAULT_USE_MOUSE_WHEEL_ZOOM true
#define DEFAULT_MOUSE_ROTATE_FACTOR .007f
#define DEFAULT_MOUSE_ZOOM_FACTOR .0075f
#define DEFAULT_MOUSE_MOVE_FACTOR .001f
#define DEFAULT_BACKGROUND_COLOR  Color(0.5f, 0.5f, 0.28f)
#define DEFAULT_LINE_WIDTH 3.0f
#define DEFAULT_CAMERA_POSITION   Point(0.0f, 37.0f, -40)
#define DEFAULT_CAMERA_LOOKAT     Point(0.0f, 0.0f,  -5)
#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
#define DEFAULT_CAMERA_FOVY       45

//====================================
// MISC. CAMERA VIEWS (copy-paste and 
// uncomment only one of these to the 
// GUI defaults)
//====================================
// initial generic camera view
//#define DEFAULT_CAMERA_POSITION   Point(0.0f, 37.0f, -40)
//#define DEFAULT_CAMERA_LOOKAT     Point(0.0f, 0.0f,  -5)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
// initial zoom-in for curves scenario
//#define DEFAULT_CAMERA_POSITION   Point(3.35f, 10.5f, -0.11)
//#define DEFAULT_CAMERA_LOOKAT     Point(3.35f, 0.0f,  -0.10)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
// initial zoom-in for cross scenario
//#define DEFAULT_CAMERA_POSITION   Point(1.2f, 15.5f, 3.43)
//#define DEFAULT_CAMERA_LOOKAT     Point(1.2f, 0.0f,  3.44)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
// initial zoom-in for 4-way-obstacle
//#define DEFAULT_CAMERA_POSITION   Point(-0.45f, 26.0f, -0.15f)
//#define DEFAULT_CAMERA_LOOKAT     Point(-0.45f, 0f,    -0.13f)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
// new zoom-in for 4-way obstacle
//#define DEFAULT_CAMERA_POSITION   Point(9.21589f, 8.08322f, -1.93683f)
//#define DEFAULT_CAMERA_LOOKAT     Point(-0.45f, 0.0f, -0.13f)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
// for frogger zoomed in
//#define DEFAULT_CAMERA_POSITION   Point(-0.900609f, 7.41979f, -10.5049f)
//#define DEFAULT_CAMERA_LOOKAT     Point(-0.900609f, 0.0f, 0.940365f)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)
// for wall-squeeze
//#define DEFAULT_CAMERA_POSITION   Point(6.64612f, 9.94769f, 3.77235f)
//#define DEFAULT_CAMERA_LOOKAT     Point(0.573495f, 0.0f, -1.51921f)
//#define DEFAULT_CAMERA_UP         Vector(0.0f, 1.0f, 0.0f)

//====================================
// ENGINE DEFAULTS
//====================================
#ifdef _WIN32
#define DEFAULT_MODULE_SEARCH_PATH "./"
#define DEFAULT_TEST_CASE_SEARCH_PATH "../../../testcases/"
#else
#define DEFAULT_MODULE_SEARCH_PATH "../modules/"
#define DEFAULT_TEST_CASE_SEARCH_PATH "../../testcases/"
#endif

#define DEFAULT_ENGINE_DRIVER "glfw"
#define DEFAULT_COUT_REDIRECTION_FILENAME ""
#define DEFAULT_CERR_REDIRECTION_FILENAME ""
#define DEFAULT_CLOG_REDIRECTION_FILENAME ""
#define DEFAULT_NUM_THREADS 1
#define DEFAULT_NUM_FRAMES_TO_SIMULATE 0
#define DEFAULT_FIXED_FPS 20.0f
#define DEFAULT_MIN_VARIABLE_DT 0.001f
#define DEFAULT_MAX_VARIABLE_DT 0.2f
#define DEFAULT_CLOCK_MODE   "fixed-fast"

//====================================
// GRID DATABASE DEFAULTS
//====================================
#define DEFAULT_MAX_ITEMS_PER_GRID_CELL 7
#define DEFAULT_GRID_SIZE_X 200.0f
#define DEFAULT_GRID_SIZE_Z 200.0f
#define DEFAULT_NUM_GRID_CELLS_X 200
#define DEFAULT_NUM_GRID_CELLS_Z 200

//====================================
// GLFW ENGINE DRIVER DEFAULTS
//====================================
#define DEFAULT_CLOCK_PAUSED_ON_START false
#define DEFAULT_WINDOW_SIZE_X 1024
#define DEFAULT_WINDOW_SIZE_Y 768
#define DEFAULT_WINDOW_POSITION_X 0
#define DEFAULT_WINDOW_POSITION_Y 0
#define DEFAULT_WINDOW_TITLE "SteerSim"

//====================================
// BUILT-IN MODULES DEFAULTS
//====================================
#define DEFAULT_TEST_CASE_PLAYER_FILENAME "simple-1.xml"
#define DEFAULT_TEST_CASE_PLAYER_AI "dummyAI"





//====================================


SimulationOptions::SimulationOptions()
{
	//
	// initialize ALL hard-coded default values, these values will be
	// overridden if they are specified in the config file.
	//

#ifndef BUILD_IPHONE
#if !defined(__native_client__) && !defined(__FLASHPLAYER__)
	// keyboard config
	keyboardBindings.quit = DEFAULT_KEY_QUIT;
	keyboardBindings.printCameraInfo = DEFAULT_KEY_PRINT_CAMERA_INFO;
	keyboardBindings.toggleAntialiasing = DEFAULT_KEY_TOGGLE_ANTIALIASING;
	keyboardBindings.takeScreenshot = DEFAULT_KEY_TAKE_SCREENSHOT;
	keyboardBindings.startDumpingFrames = DEFAULT_START_DUMPING_FRAMES;
	keyboardBindings.stopDumpingFrames = DEFAULT_STOP_DUMPING_FRAMES;
	keyboardBindings.pause = DEFAULT_KEY_PAUSE;
	keyboardBindings.stepForward = DEFAULT_KEY_STEP_ONE_FRAME;
	keyboardBindings.stepBackward = DEFAULT_KEY_STEP_BACKWARD;
	keyboardBindings.speedupPlayback = DEFAULT_KEY_SPEEDUP_PLAYBACK;
	keyboardBindings.slowdownPlayback = DEFAULT_KEY_SLOWDOWN_PLAYBACK;
	keyboardBindings.resetPlaybackSpeed = DEFAULT_KEY_RESET_PLAYBACK_SPEED;
	keyboardBindings.restartPlayback = DEFAULT_KEY_RESTART_PLAYBACK;
    
	// mouse config
	mouseBindings.selectAgent = DEFAULT_MOUSE_BUTTON_TO_SELECT_AGENT;
	mouseBindings.moveCamera = DEFAULT_MOUSE_BUTTON_TO_MOVE_CAMERA;
	mouseBindings.rotateCamera = DEFAULT_MOUSE_BUTTON_TO_ROTATE_CAMERA;
	mouseBindings.zoomCamera = DEFAULT_MOUSE_BUTTON_TO_ZOOM_CAMERA;
#endif
#endif    
	// global options
	globalOptions.engineDriver = DEFAULT_ENGINE_DRIVER;
	globalOptions.coutRedirectionFilename = DEFAULT_COUT_REDIRECTION_FILENAME;
	globalOptions.cerrRedirectionFilename = DEFAULT_CERR_REDIRECTION_FILENAME;
	globalOptions.clogRedirectionFilename = DEFAULT_CLOG_REDIRECTION_FILENAME;

	// engine options
	engineOptions.moduleSearchPath = DEFAULT_MODULE_SEARCH_PATH;
	engineOptions.testCaseSearchPath = DEFAULT_TEST_CASE_SEARCH_PATH;
	engineOptions.startupModules.clear();
	engineOptions.numThreads = DEFAULT_NUM_THREADS;
	engineOptions.numFramesToSimulate = DEFAULT_NUM_FRAMES_TO_SIMULATE;
	engineOptions.fixedFPS = DEFAULT_FIXED_FPS;
	engineOptions.minVariableDt = DEFAULT_MIN_VARIABLE_DT;
	engineOptions.maxVariableDt = DEFAULT_MAX_VARIABLE_DT;
	engineOptions.clockMode = DEFAULT_CLOCK_MODE;

	// grid database options
	gridDatabaseOptions.maxItemsPerGridCell = DEFAULT_MAX_ITEMS_PER_GRID_CELL;
	gridDatabaseOptions.gridSizeX = DEFAULT_GRID_SIZE_X;
	gridDatabaseOptions.gridSizeZ = DEFAULT_GRID_SIZE_Z;
	gridDatabaseOptions.numGridCellsX = DEFAULT_NUM_GRID_CELLS_X;
	gridDatabaseOptions.numGridCellsZ = DEFAULT_NUM_GRID_CELLS_Z;

	// GUI options
	guiOptions.useAntialiasing = DEFAULT_ANTIALIASING;
	guiOptions.useVsync = DEFAULT_VSYNC;
	guiOptions.mouseRotationFactor = DEFAULT_MOUSE_ROTATE_FACTOR;
	guiOptions.mouseZoomFactor = DEFAULT_MOUSE_ZOOM_FACTOR;
	guiOptions.mouseMovementFactor = DEFAULT_MOUSE_MOVE_FACTOR;
	guiOptions.canUseMouseSelection = DEFAULT_CAN_USE_MOUSE_SELECTION;
	guiOptions.canUseMouseWheelZoom = DEFAULT_USE_MOUSE_WHEEL_ZOOM;
	guiOptions.cameraPosition = DEFAULT_CAMERA_POSITION;
	guiOptions.cameraLookAt = DEFAULT_CAMERA_LOOKAT;
	guiOptions.cameraUp = DEFAULT_CAMERA_UP;
	guiOptions.cameraFovy = DEFAULT_CAMERA_FOVY;
	guiOptions.backgroundColor = DEFAULT_BACKGROUND_COLOR;
	guiOptions.lineWidth = DEFAULT_LINE_WIDTH;

	// glfw engine driver options
	glfwEngineDriverOptions.pausedOnStart = DEFAULT_CLOCK_PAUSED_ON_START;
	glfwEngineDriverOptions.windowSizeX = DEFAULT_WINDOW_SIZE_X;
	glfwEngineDriverOptions.windowSizeY = DEFAULT_WINDOW_SIZE_Y;
	glfwEngineDriverOptions.windowPositionX = DEFAULT_WINDOW_POSITION_X;
	glfwEngineDriverOptions.windowPositionY = DEFAULT_WINDOW_POSITION_Y;
	glfwEngineDriverOptions.windowTitle = DEFAULT_WINDOW_TITLE;

	//
	// module options
	// for each module, initialize its module options, and insert that into the module options database.
	// NOTE CAREFULLY: module names are CASE-SENSITIVE!!!
	//
	OptionDictionary modOpts;
	moduleOptionsDatabase.clear();

	modOpts.clear();
	modOpts["testcase"] = DEFAULT_TEST_CASE_PLAYER_FILENAME;
	modOpts["ai"] = DEFAULT_TEST_CASE_PLAYER_AI;
	moduleOptionsDatabase["testCasePlayer"] = modOpts;
	/*
	modOpts.clear();
	moduleOptionsDatabase["recFilePlayer"] = modOpts;

	modOpts.clear();
	moduleOptionsDatabase["simpleAI"] = modOpts;

	modOpts.clear();
	moduleOptionsDatabase["dummyAI"] = modOpts;

	modOpts.clear();
	moduleOptionsDatabase["simulationRecorder"] = modOpts;

	modOpts.clear();
	moduleOptionsDatabase["metricsCollector"] = modOpts;

	modOpts.clear();
	moduleOptionsDatabase["steerBench"] = modOpts;

	modOpts.clear();
	moduleOptionsDatabase["steerBug"] = modOpts;
	*/

	_moduleOptionsXMLParser.init(&moduleOptionsDatabase);
	_startupModulesXMLParser.init(&engineOptions.startupModules);


	/*
	std::cout << "HARD-CODED DEFAULTS:\n";
	for (ModuleOptionsDatabase::iterator iter = moduleOptionsDatabase.begin(); iter != moduleOptionsDatabase.end(); ++iter) {
		std::cout << "module \"" << (*iter).first << "\"\n";
		for (OptionDictionary::iterator k = (*iter).second.begin(); k != (*iter).second.end(); ++k) {
			std::cout << "    " << (*k).first << " --> " << (*k).second << "\n";
		}
		std::cout << "\n";
	}
	*/

}

//
// loadOptionsFromConfigFile() - any values specified in the config file will override the original hard-coded defaults.
//
void SimulationOptions::loadOptionsFromConfigFile( const std::string & filename )
{
	XMLParser xmlOpts;
	_setupXMLStructure(xmlOpts);

	if (fileCanBeOpened(filename)) {
		xmlOpts.parseXMLFile(filename, true);
	}
	else {
		throw GenericException("Cannot find the specified config file \"" + filename + "\".");
	}

	/*
	// debugging output, keep it here for now
	std::cout << "CONFIG-FILE DEFAULTS:\n";
	for (ModuleOptionsDatabase::iterator iter = moduleOptionsDatabase.begin(); iter != moduleOptionsDatabase.end(); ++iter) {
		std::cout << "module \"" << (*iter).first << "\"\n";
		for (OptionDictionary::iterator k = (*iter).second.begin(); k != (*iter).second.end(); ++k) {
			std::cout << "    " << (*k).first << " --> " << (*k).second << "\n";
		}
		std::cout << "\n";
	}
	*/


	//
	// do error checking and option conversions next, try to be user-friendly where possible.
	//

	/// @todo add some path-cleanup and error checking here in SimulationOptions.cpp.

	// transform clockMode option to lower case
	std::transform(engineOptions.clockMode.begin(), engineOptions.clockMode.end(), engineOptions.clockMode.begin(), (int(*)(int))tolower);

	if ((engineOptions.clockMode != "fixed-fast") && (engineOptions.clockMode != "fixed-real-time") && (engineOptions.clockMode != "variable-real-time")) {
		std::cerr << "WARNING: Bad option value for clockMode in configuration file.\n         Valid options are: \"fixed-fast\", \"fixed-real-time\" or \"variable-real-time\".\n         For now, setting default to \"fixed-fast\", which may be overridden by command-line or GUI.";
		engineOptions.clockMode = "fixed-fast";
	}
}


void SimulationOptions::generateConfigFile( const std::string & filename )
{
	XMLParser xmlOpts;
	_setupXMLStructure(xmlOpts);

	if (fileCanBeOpened(filename)) {
		throw GenericException("Cannot generate config file \"" + filename + "\", file already exists.");
	}

	xmlOpts.writeXMLFile(filename);

	std::cout << "Default configuration written to " << filename << ".\n";
}

void SimulationOptions::_setupXMLStructure( Util::XMLParser & xmlOpts )
{
	XMLTag * root = xmlOpts.createRootTag("SteerSimOptions", "This file contains options for SteerSim.  Edit this file to your preference, and\nthen use the '-config' command line option to load these options in SteerSim.\nOptions specified by the command line will override options in this configuration file.");

	// primary option groups
	root->createChildTag("keyboard", "Maps various actions to keyboard input (config for keybaord not implemented yet!)");
	root->createChildTag("mouse", "Maps various actions to mouse input (config for mouse not implemented yet!)");
	XMLTag * guiTag = root->createChildTag("gui", "Options related to the openGL visualization and interaction.  Also, make sure to look at the engine driver options for more interface-related options.");
	XMLTag * globalTag = root->createChildTag("global", "Options related to the main execution of the steersim");
	XMLTag * engineTag = root->createChildTag("engine", "Options related to the simulation engine");
	XMLTag * gridDatabaseTag = root->createChildTag("gridDatabase", "Options related to the spatial database");
	XMLTag * engineDriversTag = root->createChildTag("engineDrivers", "Options related to engine drivers");
	root->createChildTag("modules", "Module-specific options.  Any options specified on the command-line will override the options specified here.  Modules specified here will not necessarily be loaded when started; for that use the startupModules option for the engine.", XML_DATA_TYPE_CONTAINER, NULL, &_moduleOptionsXMLParser );

	// option sub-groups
	engineDriversTag->createChildTag("commandLine", "Options for the command-line engine driver (currently there are no options for the command-line)");
	XMLTag * glfwEngineDriverTag = engineDriversTag->createChildTag("glfw", "Options for the GLFW engine driver");
	engineDriversTag->createChildTag("qt", "Options for the Qt engine driver (config for qt not implemented yet!)");

	// GUI options
	guiTag->createChildTag("useAntialiasing", "Set to \"true\" to remove jaggies, for smoother-looking visuals, but lower performance", XML_DATA_TYPE_BOOLEAN, &guiOptions.useAntialiasing);
	guiTag->createChildTag("useShadows", "DO NOT USE THIS VALUE.  It is only kept here for backwards compatibility.", XML_DATA_TYPE_BOOLEAN, &_dummyUseShadowsFlag);
	guiTag->createChildTag("useVsync", "Set to \"false\" for higher performance that is not synchronizeded with the display's refresh rate", XML_DATA_TYPE_BOOLEAN, &guiOptions.useVsync);
	guiTag->createChildTag("mouseRotationFactor", "Scaling factor for sensitivity of camera rotation when using mouse.", XML_DATA_TYPE_FLOAT, &guiOptions.mouseRotationFactor);
	guiTag->createChildTag("mouseZoomFactor", "Scaling factor for sensitivity of camera zoom when using mouse", XML_DATA_TYPE_FLOAT, &guiOptions.mouseZoomFactor);
	guiTag->createChildTag("mouseMovementFactor", "Scaling factor for sensitivity of camera movement when using mouse", XML_DATA_TYPE_FLOAT, &guiOptions.mouseMovementFactor);
	guiTag->createChildTag("canUseMouseSelection", "Set to \"true\" to be able to select agents with the mouse, \"false\" is recommended when using many many agents, because selection algorithm is brute-force and slow", XML_DATA_TYPE_BOOLEAN, &guiOptions.canUseMouseSelection);
	guiTag->createChildTag("canUseMouseWheelZoom", "Set to \"true\" to be able to zoom with the mouse wheel; this does not disable other possible ways to zoom the camera.", XML_DATA_TYPE_BOOLEAN, &guiOptions.canUseMouseWheelZoom);
	guiTag->createChildTag("cameraPosition", "Camera's physical position in the 3-D scene", XML_DATA_TYPE_XYZ, &guiOptions.cameraPosition);
	guiTag->createChildTag("cameraLookAt", "The 3-D point the camera will look at", XML_DATA_TYPE_XYZ, &guiOptions.cameraLookAt);
	guiTag->createChildTag("cameraUp", "The vector that represnts the upright orientation for the camera", XML_DATA_TYPE_XYZ, &guiOptions.cameraUp);
	guiTag->createChildTag("cameraVerticalFieldOfView", "The vertical field of view of the camera, in degrees (CURRENTLY HAS NO EFFECT)", XML_DATA_TYPE_FLOAT, &guiOptions.cameraFovy);
	guiTag->createChildTag("backgroundColor", "The background color of the openGL visualization", XML_DATA_TYPE_RGB, &guiOptions.backgroundColor);
	guiTag->createChildTag("lineWidth", "width of lines drawn in the GUI", XML_DATA_TYPE_FLOAT, &guiOptions.lineWidth);

	// global options
	globalTag->createChildTag("engineDriver", "The name of the engine driver to use, if not specified from command line", XML_DATA_TYPE_STRING, &globalOptions.engineDriver);
	globalTag->createChildTag("redirectCoutToFile", "If a filename is specified, std::cout will be redirected to that filename.  NOTE: Only std::cout will be redirected; low-level and C-style output will not be redirected.", XML_DATA_TYPE_STRING, &globalOptions.coutRedirectionFilename);
	globalTag->createChildTag("redirectCerrToFile", "If a filename is specified, std::cerr will be redirected to that filename.  NOTE: Only std::cerr will be redirected; low-level and C-style output will not be redirected.  Exceptions will be caught and redirected to both the new and the original std::cerr output.", XML_DATA_TYPE_STRING, &globalOptions.cerrRedirectionFilename);
	globalTag->createChildTag("redirectClogToFile", "If a filename is specified, std::clog will be redirected to that filename.  NOTE: Only std::clog will be redirected; low-level and C-style output will not be redirected.", XML_DATA_TYPE_STRING, &globalOptions.clogRedirectionFilename);

	// engine options
	engineTag->createChildTag("moduleSearchPath","The default directory to search for dynamic plug-in modules at runtime.", XML_DATA_TYPE_STRING, &engineOptions.moduleSearchPath);
	engineTag->createChildTag("testCaseSearchPath","The default directory to search for test cases at runtime.", XML_DATA_TYPE_STRING, &engineOptions.testCaseSearchPath);
	engineTag->createChildTag("startupModules", "The list of modules to use on startup.  Modules specified by the command line will be merged with this list.", XML_DATA_TYPE_CONTAINER, NULL, &_startupModulesXMLParser);
	engineTag->createChildTag("numThreads", "The default number of threads to run on the simulation", XML_DATA_TYPE_UNSIGNED_INT, &engineOptions.numThreads);
	engineTag->createChildTag("numFrames", "The default number of frames to simulate - 0 means run the entire simulation until all agents are disabled.", XML_DATA_TYPE_UNSIGNED_INT, &engineOptions.numFramesToSimulate);
	engineTag->createChildTag("fixedFPS", "The fixed frames-per-second for the simulation clock.  This value is used when simulationClockMode is \"fixed-fast\" or \"fixed-real-time\".", XML_DATA_TYPE_FLOAT, &engineOptions.fixedFPS);
	engineTag->createChildTag("minVariableDt", "The minimum time-step allowed when the clock is in \"variable-real-time\" mode.  If the proposed time-step is smaller, this value will be used instead, effectively limiting the max frame rate.", XML_DATA_TYPE_FLOAT, &engineOptions.minVariableDt);
	engineTag->createChildTag("maxVariableDt", "The maximum time-step allowed when the clock is in \"variable-real-time\" mode.  If the proposed time-step is larger, this value will be used instead, at the expense of breaking synchronization between simulation time and real-time.", XML_DATA_TYPE_FLOAT, &engineOptions.maxVariableDt);
	engineTag->createChildTag("clockMode", "can be either \"fixed-fast\" (fixed simulation frame rate, running as fast as possible), \"fixed-real-time\" (fixed simulation frame rate, running in real-time), or \"variable-real-time\" (variable simulation frame rate in real-time).", XML_DATA_TYPE_STRING, &engineOptions.clockMode);

	// grid database options
	gridDatabaseTag->createChildTag("maxItemsPerGridCell", "Max number of items a grid cell can contain", XML_DATA_TYPE_UNSIGNED_INT, &gridDatabaseOptions.maxItemsPerGridCell);
	gridDatabaseTag->createChildTag("sizeX", "Total size of the grid along the X axis", XML_DATA_TYPE_FLOAT, &gridDatabaseOptions.gridSizeX);
	gridDatabaseTag->createChildTag("sizeZ", "Total size of the grid along the Z axis", XML_DATA_TYPE_FLOAT, &gridDatabaseOptions.gridSizeZ);
	gridDatabaseTag->createChildTag("numCellsX", "Number of cells in the grid along the X axis", XML_DATA_TYPE_UNSIGNED_INT, &gridDatabaseOptions.numGridCellsX);
	gridDatabaseTag->createChildTag("numCellsZ", "Number of cells in the grid along the Z axis", XML_DATA_TYPE_UNSIGNED_INT, &gridDatabaseOptions.numGridCellsZ);

	// GLFW engine driver options
	glfwEngineDriverTag->createChildTag("startWithClockPaused", "Starts the clock paused if \"true\".", XML_DATA_TYPE_BOOLEAN, &glfwEngineDriverOptions.pausedOnStart);
	glfwEngineDriverTag->createChildTag("windowSizeX", "Width of the openGL window in pixels", XML_DATA_TYPE_UNSIGNED_INT, &glfwEngineDriverOptions.windowSizeX);
	glfwEngineDriverTag->createChildTag("windowSizeY", "Height of the openGL window in pixels", XML_DATA_TYPE_UNSIGNED_INT, &glfwEngineDriverOptions.windowSizeY);
	glfwEngineDriverTag->createChildTag("windowPositionX", "Position of the openGL window in x", XML_DATA_TYPE_UNSIGNED_INT, &glfwEngineDriverOptions.windowPositionX);
	glfwEngineDriverTag->createChildTag("windowPositionY", "Position of the openGL window in y", XML_DATA_TYPE_UNSIGNED_INT, &glfwEngineDriverOptions.windowPositionY);
	glfwEngineDriverTag->createChildTag("windowTitle", "Title of the openGL window", XML_DATA_TYPE_STRING, &glfwEngineDriverOptions.windowTitle);
}


//========================================
const SteerLib::OptionDictionary & SimulationOptions::getModuleOptions(const std::string & moduleName)
{
	ModuleOptionsDatabase::iterator optionsIter = moduleOptionsDatabase.find(moduleName);

	if ( optionsIter != moduleOptionsDatabase.end() ) {
		return moduleOptionsDatabase[moduleName];
	}
	else {
		throw Util::GenericException("Cannot find module options, unknown module \"" + moduleName + "\".");
	}

}


void SimulationOptions::mergeModuleOptions( const std::string & moduleName, const std::string & options)
{

	// split the options
	std::string tempOptions = options;

	std::vector< char * > optionList;
	_tokenizeModuleOptions( tempOptions, optionList);
	for (unsigned int i=0; i<optionList.size(); i++) {
		std::string optionValue = std::string(optionList[i]);
		std::string option = optionValue.substr(0, optionValue.find('='));
		std::string value = optionValue.substr(optionValue.find('=')+1, std::string::npos);
		if (option=="") {
			throw GenericException("Invalid empty option found in the list of options \"" + options + "\".");
		}
		// the following overrides existing values or creates a new 
		// map entry if the "option" key did not already exist.

		moduleOptionsDatabase[moduleName][option] = value;
	}
}

void SimulationOptions::_tokenizeModuleOptions( std::string & options,  std::vector<char*> & argv)
{
	argv.clear();
	if (options.length() > 0) {
		unsigned int numchars = options.length();
		std::replace_if( options.begin(), options.end(), _isDelimeter, '\0');
		char * cstr = const_cast<char*>(options.c_str());
		argv.emplace_back( &(cstr[0]) );
		for (unsigned int i = 1; i < numchars; i++) {
			if (cstr[i-1] == '\0')
				argv.emplace_back( &(cstr[i]) );
		}
	}
}


void ModuleOptionsXMLParser::startElement( Util::XMLTag * tag, const ticpp::Element * subRoot )
{
	// DO NOT CLEAR the module options here; instead the purpose is to MERGE options with the existing hard-coded 
	// defaults.  This is easy  do since the STL map's [] operator will allow us to overwrite existing values
	// as well as create new vaules if they did not already exist.

	// iterate over the children tags in the xml DOM
	ticpp::Iterator<ticpp::Element> moduleIter;
	for (moduleIter = moduleIter.begin(subRoot); moduleIter != moduleIter.end(); ++moduleIter ) {
		ticpp::Element * moduleRoot;
		moduleRoot = &(*moduleIter);

		std::string moduleName = moduleRoot->Value();

		ticpp::Iterator<ticpp::Element> optionIter;
		for (optionIter = optionIter.begin(moduleRoot); optionIter != optionIter.end(); ++optionIter ) {
			if (optionIter->FirstChildElement(false)) {
				throw GenericException("Error while parsing XML config file: invalid extra tag inside of a module option at line "+Util::toString(optionIter->FirstChildElement(false)->Row()));
			}
			std::string option = (*optionIter).Value();
			std::string value = (*optionIter).GetText(false);
			(*_modOpts)[moduleName][option] = value;
		}
	}
}

void ModuleOptionsXMLParser::outputFormattedXML(std::ostream &out, const std::string & indentation)
{
	std::map<std::string, std::map<std::string, std::string> >::iterator moduleIter;
	std::map<std::string, std::string>::iterator optionIter;

	for (moduleIter = (*_modOpts).begin(); moduleIter != (*_modOpts).end(); ++moduleIter) {
		std::string moduleName;
		moduleName = (*moduleIter).first;
		out << indentation << "<" << moduleName << ">\n";

		for (optionIter = (*_modOpts)[moduleName].begin(); optionIter != (*_modOpts)[moduleName].end(); ++optionIter) {
			out << indentation << "    <" << (*optionIter).first << ">" << (*optionIter).second << "</" << (*optionIter).first << ">\n";
		}
		out << indentation << "</" << moduleName << ">\n";
	}
}

void StartupModulesXMLParser::startElement( Util::XMLTag * tag, const ticpp::Element * subRoot )
{
	ticpp::Iterator<ticpp::Element> moduleIter;
	for (moduleIter = moduleIter.begin(subRoot); moduleIter != moduleIter.end(); ++moduleIter ) {
		if (moduleIter->FirstChildElement(false)) {
			throw GenericException("Error while parsing XML config file: invalid extra tag inside of a module option at line "+Util::toString(moduleIter->FirstChildElement(false)->Row()));
		}
		if ((*moduleIter).Value() != "module") {
			throw GenericException("Error while parsing XML config file: invalid tag at line "+Util::toString(moduleIter->FirstChildElement(false)->Row()) + ", only <module> is allowed.");
		}
		std::string value = (*moduleIter).GetText(false);

		(*_startupModules).insert(value);
	}
}

void StartupModulesXMLParser::outputFormattedXML(std::ostream &out, const std::string & indentation)
{
	std::set<std::string>::iterator moduleIter;
	
	if ((*_startupModules).size() != 0) {
		// if there already are modules loaded and specified, then use those
		for (moduleIter=(*_startupModules).begin(); moduleIter != (*_startupModules).end(); ++moduleIter) {
			out << indentation << "<module>" << (*moduleIter) << "</module>\n";
		}
	}
	else {
		// otherwise, output a simple default, so that the config file has an example of the syntax and usage.
		out << indentation << "<module>testCasePlayer</module>\n";
	}
}



