
#include <FL/Fl_Native_File_Chooser.H>
#include <sbm/GPU/SbmShader.h>
#include "RootWindow.h"
#include "CharacterCreatorWindow.h"
#include <FL/Fl_Pack.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <sstream>
#include "boost/filesystem.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/version.hpp>
#include "CommandWindow.h"
#include <sb/SBSkeleton.h>
#include <sb/SBScene.h>
#include "sb/SBDebuggerClient.h"
#include <sb/SBSimulationManager.h>
#include <sb/SBAssetManager.h>
#include <sbm/Heightfield.h>
#include "SBPython.h"
#include <sb/SBVersion.hpp>
#include <sb/SBBehaviorSetManager.h>
#include "SBUtilities.h"
#include "SBRenderer.h" 
#include"resourceViewer/AttributeEditor.h"

#include "sbm/SceneExporter.h"


#include "SBGUIManager.h"
#include "sbm/SBRenderSceneListener.h"

#include "SBPythonClass.h"
#include "pythonbind/PythonSetup.h"


#define TEST_EXPORT_SMARTBODY_PACKAGE 1

BaseWindow* BaseWindow::sInstance = nullptr;


BaseWindow::BaseWindow(bool useEditor, int x, int y, int w, int h, const char* name)
: 	SrViewer(x, y, w, h),
	Fl_Double_Window(x, y, w, h, name),
 	mDebuggerClient(std::make_unique<SmartBody::SBDebuggerClient>(Session::current->scene)),
	 mSession(Session::current)
{
	sInstance = this;
	standaloneResourceWindow = nullptr;
	commandWindow = nullptr;
	bmlCreatorWindow = nullptr;
	this->begin();

	menubar = new Fl_Menu_Bar(0, 0, w, 30); 
	menubar->labelsize(10);
	menubar->add("&File/New", 0, NewCB, this, 0);
	menubar->add("&File/Load from script", 0, LoadCB, this, 0);
	//menubar->add("&File/Save to script", 0, SaveCB, this, nullptr);
	menubar->add("&File/Run Script...", 0, LoadSceneSettingCB, this, FL_MENU_DIVIDER);
//	menubar->add("&File/Save Scene Settings", 0, SaveSceneSettingCB, this, nullptr);
	menubar->add("&File/Export Scene", 0, ExportCB, this, 0);
#if TEST_EXPORT_SMARTBODY_PACKAGE
	//menubar->add("&File/Export to folder", 0, ExportPackageCB, (void*)0, nullptr);
	//menubar->add("&File/Export to .zip", 0, ExportPackageCB, (void*)1, nullptr);
	menubar->add("&File/Import Folder", 0, LoadPackageCB, this, FL_MENU_DIVIDER);
#endif
    menubar->add("&File/Remote Quick Connect", 0, QuickConnectCB, this, 0);
	menubar->add("&File/Remote Connect...", 0, LaunchConnectCB, this, 0);
	menubar->add("&File/Remote Disconnect", 0, DisconnectRemoteCB, this, FL_MENU_DIVIDER);
	menubar->add("&File/&Quit", 0, QuitCB, this, 0);
//	menubar->add("&File/Save Configuration...", 0, nullptr, 0, nullptr);
//	menubar->add("&File/Run Script...", 0, nullptr, 0, nullptr);
	menubar->add("&View/Character/Bones", 0, ModeBonesCB, this, 0);
	menubar->add("&View/Character/Geometry", 0, ModeGeometryCB, this, 0);
	menubar->add("&View/Character/Collision Geometry", 0, ModeCollisionGeometryCB, this, 0);
	menubar->add("&View/Character/Skin Weight", 0, ModeSkinWeightCB, this, 0);
	menubar->add("&View/Character/Deformable Geometry", 0, ModeDeformableGeometryCB, this, 0);
	menubar->add("&View/Character/GPU Deformable Geometry", 0, ModeGPUDeformableGeometryCB, this, 0);
	menubar->add("&View/Character/Axis", 0, ModeAxisCB, this, 0);
//	menubar->add("&View/Character/Show Selected", 0, ShowSelectedCB, this, nullptr);
	menubar->add("&View/Character/Eyebeams", 0, ModeEyebeamsCB, this, 0);
	menubar->add("&View/Character/Gaze Limits", 0, ModeGazeLimitCB, this, 0);
//	menubar->add("&View/Character/Eyelid calibration", 0, ModeEyelidCalibrationCB, this, nullptr);
	menubar->add("&View/Character/Bounding Volumes", 0, ShowBoundingVolumeCB, this, 0);
//	menubar->add("&View/Character/Dynamics/COM", 0, ModeDynamicsCOMCB, this, nullptr);
//	menubar->add("&View/Character/Dynamics/Support Polygon", 0, ModeDynamicsSupportPolygonCB, this, nullptr);
//	menubar->add("&View/Character/Dynamics/Masses", 0, ModeDynamicsMassesCB, this, nullptr);
//	menubar->add("&View/Character/Locomotion/Kinematic Footsteps", 0, KinematicFootstepsCB, this, nullptr);
//	menubar->add("&View/Character/Locomotion/Locomotion Footsteps", 0, LocomotionFootstepsCB, this, nullptr);
//	menubar->add("&View/Character/Locomotion/Velocity", 0, VelocityCB, this, nullptr);
//	menubar->add("&View/Character/Locomotion/Trajectory", 0, TrajectoryCB, this, nullptr);
	menubar->add("&View/Character/Show Trajectory", 0, TrajectoryCB, this, 0);
	menubar->add("&View/Character/Show Gesture", 0, GestureCB, this, 0);
	menubar->add("&View/Character/Show Joint Labels", 0, JointLabelCB, this, 0);
	menubar->add("&View/Character/Show Selected Object", 0, ShowSelectedCharacterCB, this, 0);
	menubar->add("&View/Pawns", 0, ShowPawns, this, 0);
	menubar->add("&View/Show Cameras", 0, ShowCamerasCB, this, 0);
	menubar->add("&View/Show Lights", 0, ShowLightsCB, this, 0);
	menubar->add("&View/Shadows/Shadow Map", 0, ShadowsMapCB, this, 0);
	menubar->add("&View/Shadows/Stencil Shadow", 0, ShadowsStencilCB, this, 0);
	menubar->add("&View/Shadows/Disable Shadow", 0, ShadowsNoneCB, this, 0);
	menubar->add("&View/Grid", 0, GridCB, this, 0);
	menubar->add("&View/Background Color", 0, BackgroundColorCB, this, 0);
	menubar->add("&View/Floor/Show Floor", 0, FloorCB, this, 0);
	menubar->add("&View/Floor/Floor Color", 0, FloorColorCB, this, 0);
	//menubar->add("&View/Reach Pose Examples", 0, ShowPoseExamples, this, nullptr);
	menubar->add("&View/Terrain/Shaded", 0, TerrainShadedCB, this, 0);
	menubar->add("&View/Terrain/Wireframe", 0, TerrainWireframeCB, this, 0);
	menubar->add("&View/Terrain/No Terrain", 0, TerrainNoneCB, this, 0);
	menubar->add("&View/NavigationMesh/NoMesh", 0, NavigationMeshNoneCB, this, 0);
	menubar->add("&View/NavigationMesh/Show RawMesh", 0, NavigationMeshRawMeshCB, this, 0);
	menubar->add("&View/NavigationMesh/Show NaviMesh", 0, NavigationMeshNaviMeshCB, this, 0);
	menubar->add("&View/Steer/Characters and Goals", 0, SteeringCharactersCB, this, 0);
	menubar->add("&View/Steer/All Steering", 0, SteeringAllCB, this, 0);
	menubar->add("&View/Steer/No Steering", 0, SteeringNoneCB, this, 0);

	menubar->add("&View/Collisions/Show Collision Info", 0, ShowCollisionCB, this, 0);
	menubar->add("&View/Collisions/Hide Collisions Info", 0, HideCollisionCB, this, 0);

	menubar->add("&Create/Character...", 0, CreateCharacterCB, this, 0);
	menubar->add("&Create/Pawn...", 0, CreatePawnCB, this, 0);
	menubar->add("&Create/Pawn from model...", 0, CreatePawnFromModelCB, this, 0);
	menubar->add("&Create/Light", 0, CreateLightCB, this, 0);
	menubar->add("&Create/Camera...", 0, CreateCameraCB, this, FL_MENU_DIVIDER);
	//deleteObjectMenuIndex = menubar->add("&Create/Delete Object", 0, nullptr, nullptr, FL_SUBMENU_POINTER);
	menubar->add("&Create/Delete Selected Object", 0, DeleteSelectionCB, this, 0);

	menubar->add("&Create/Terrain...", 0, CreateTerrainCB, this, 0); // should replace it with create navigation mesh.

//	setResolutionMenuIndex = menubar->add("&Settings/Set Resolution", 0, 0, 0, FL_SUBMENU_POINTER);
	menubar->add("&Settings/Default Media Path", 0, SettingsDefaultMediaPathCB, this, 0);
	menubar->add("&Settings/Internal Audio", 0, AudioCB, this, 0);

	menubar->add("&Camera/Save Camera View", 0, SaveCameraCB, this, 0 );
	//loadCameraMenuIndex = menubar->add("&Camera/Load Camera", 0, nullptr, nullptr, FL_SUBMENU_POINTER );
	//deleteCameraMenuIndex = menubar->add("&Camera/Delete Camera", 0, nullptr, nullptr, FL_SUBMENU_POINTER );
	menubar->add("&Camera/Reset", 0, CameraResetCB, this, 0);
	menubar->add("&Camera/Frame All", 0, CameraFrameCB, this, 0);
	menubar->add("&Camera/Frame Selected Object", 0, CameraFrameObjectCB, this, 0);
	menubar->add("&Camera/Face Camera", 0, FaceCameraCB, this, 0);
	menubar->add("&Camera/Character Cone Sight", 0, CameraCharacterShightCB, this, 0);
	menubar->add("&Camera/Track Character", 0, TrackCharacterCB, this, 0);
	menubar->add("&Camera/Rotate Around Selected", 0, RotateSelectedCB, this, 0);
   menubar->add("&Camera/Modes/Default", 0, SetDefaultCamera, this, 0);
   menubar->add("&Camera/Modes/Free Look", 0, SetFreeLookCamera, this, 0);
   menubar->add("&Camera/Modes/Follow Renderer", 0, SetFollowRendererCamera, this, 0);
   menubar->add("&Camera/Take Snapshot/JPG...", 0, SetTakeSnapshotCB, this, 0);
   menubar->add("&Camera/Take Snapshot/TGA...", 0, SetTakeSnapshot_tgaCB, this, 0);

//	menubar->add("&Window/Resource View", 0, LaunchResourceViewerCB, this, nullptr);
	menubar->add("&Window/Command Window", 0, LaunchConsoleCB, this, 0);
	menubar->add("&Window/Data Viewer", 0, LaunchDataViewerCB,this, 0);
//	menubar->add("&Window/BML Viewer", 0, LaunchBMLViewerCB, this, nullptr);
	menubar->add("&Window/Blend Viewer", 0, LaunchParamAnimViewerCB, this, 0);
	menubar->add("&Window/BML Creator", 0, LaunchBMLCreatorCB, this, 0);
	menubar->add("&Window/Face Viewer", 0, LaunchFaceViewerCB, this, 0);
	menubar->add("&Window/FaceShift Viewer", 0, LaunchFaceShiftViewerCB, this, 0);
	menubar->add("&Window/Lip Sync Viewer", 0, LaunchVisemeViewerCB, this, 0);
	//menubar->add("&Window/Retarget Creator", 0, LaunchRetargetCreatorCB, this, nullptr);
	//menubar->add("&Window/Behavior Sets", 0, LaunchBehaviorSetsCB, this, nullptr);
	menubar->add("&Window/Motion Editor", 0, LaunchMotionEditorCB, this, 0);
	menubar->add("&Window/Retarget Viewer", 0, LaunchJointMapViewerCB, this, 0);
	menubar->add("&Window/Pose Creator", 0, LaunchPoseCreatorCB, this, 0);
	menubar->add("&Window/Speech Relay", 0, LaunchSpeechRelayCB, this, 0);
	menubar->add("&Help/About", 0, HelpCB, this, 0);
	menubar->add("&Help/Documentation", 0, DocumentationCB, this, 0);
	menubar->add("&Help/Create Python API", 0, CreatePythonAPICB, this, 0);
	menubar->add("&Help/Switch Renderer", 0, SwitchRendererCB, this, 0);
	//menubar->add("&Scripts/Reload Scripts", 0, ReloadScriptsCB, this, nullptr);
	//menubar->add("&Scripts/Set Script Folder", 0, SetScriptDirCB, this, FL_MENU_DIVIDER);

	// disable the commands that are not yet functional
	/*
	Fl_Group* fileMenuOption = dynamic_cast<Fl_Group*>(menubar->child(0));
	if (fileMenuOption)
	{
		for (int c = 0; c < fileMenuOption->children(); c++)
		{
			fileMenuOption->child(c)->deactivate();
		}
	}
	*/


	int curY= 2;
	//Fl_Group* cameraGroup = new Fl_Group(10, curY, w, 25, nullptr);
	//cameraGroup->type(Fl_Pack::HORIZONTAL);

//
// 	cameraChoice = new Fl_Choice(curX, curY, 80, 25, "Camera");
// 	cameraChoice->when(FL_WHEN_NOT_CHANGED|FL_WHEN_CHANGED);
// 	cameraChoice->callback(ChooseCameraCB, this);
// 	updateCameraList();
// 	cameraChoice->value(0);
// 	curX += 85;
// 	saveCamera = new Fl_Button(curX, curY, 45, 25, "Save");
// 	saveCamera->callback(SaveCameraCB, this);
//
  	int curX = 500;
// 	deleteCamera = new Fl_Button(curX, curY, 45, 25, "Del");
// 	deleteCamera->callback(DeleteCameraCB, this);
//	windowSizes.emplace_back("640x360");
//	windowSizes.emplace_back("640x480");
//	windowSizes.emplace_back("720x480");
//	windowSizes.emplace_back("720x576");
//	windowSizes.emplace_back("800x600");
//	windowSizes.emplace_back("854x480");
//	windowSizes.emplace_back("960x600");
//	windowSizes.emplace_back("1024x576");
//	windowSizes.emplace_back("1024x768");
//	windowSizes.emplace_back("1280x720");
//	windowSizes.emplace_back("1280x768");
//	windowSizes.emplace_back("1366x768");
//	windowSizes.emplace_back("1280x800");
//	windowSizes.emplace_back("1280x1024");
//	windowSizes.emplace_back("1440x900");
//	windowSizes.emplace_back("1600x900");
//	windowSizes.emplace_back("1920x1080");
//
//	resolutionMenuList.clear();
//	Fl_Menu_Item defaultItem = {"Default", 0, ResizeWindowCB, this};
//	resolutionMenuList.emplace_back(defaultItem);
//	for (auto & windowSize : windowSizes)
//	{
//		Fl_Menu_Item resItem = { windowSize.c_str(), 0, ResizeWindowCB, this } ;
//		resolutionMenuList.emplace_back(resItem);
//	}
//
//	Fl_Menu_Item customItem = {"Custom...", 0, ResizeWindowCB, this};
//	Fl_Menu_Item tempItem = {nullptr};
//	resolutionMenuList.emplace_back(customItem);
//	resolutionMenuList.emplace_back(tempItem);
//
//	Fl_Menu_Item* menuList = const_cast<Fl_Menu_Item*>(menubar->menu());
////	Fl_Menu_Item& resolutionSubMenu = menuList[setResolutionMenuIndex];
////	resolutionSubMenu.user_data(&resolutionMenuList[0]);

	//cameraGroup->end();	

	curY += 28;

	cameraCount = 0;

	/*
	Fl_Pack* simGroup = new Fl_Pack(10, curY, 75, 25, nullptr);
	simGroup->begin();
	int curX = 0;

	buttonPlay = new Button(curX, 0, 25, 25, "@>");
	buttonPlay->callback(StartCB, this);
	buttonPlay->set_vertical();

	buttonPlaybackStepForward = new Button(curX, 0, 25, 25, "@>|");
	buttonPlaybackStepForward->callback(StepCB, this);
	buttonPlaybackStepForward->set_vertical();
	curX += 25;

	buttonStop = new Button(curX, 0, 25, 25, "@square");
	buttonStop->callback(StopCB, this);
	buttonStop->set_vertical();

	inputTimeStep = new FloatInput(curX, 0, 25, 25);
	inputTimeStep->value("0");
	inputTimeStep->callback(nullptr, this);
	curX += 25;	
	Fl_Output* spacer = new Fl_Output(curX, 0, 25, 25);
	simGroup->end();
	simGroup->resizable(spacer);

	curY += 30;
	*/

	// should the layout be single window or include the outliner and attribute window
	_layoutMode = 0;

	int leftBorderSize = 10;
	int rightBorderSize = 10;

	int leftGroupSize = 364;
	if (!useEditor)
		leftGroupSize = 0;
	_mainGroup = new Fl_Group(10, curY, w - 20, h - curY, "");
	_mainGroup->begin();

	_leftGroup = new Fl_Group(10, curY, leftGroupSize, h - curY);

	// add the outliner
	int outlinerWidth = 364;
	if (!useEditor)
		outlinerWidth = 0;
	int outlinerHeight = (int) (h - curY) / 2  - 10;
	resourceWindow = new ResourceWindow(leftBorderSize, curY, outlinerWidth-10 , outlinerHeight, "");
	resourceWindow->box(FL_UP_BOX);
	// add the attribute window
	int attributeEditorWidth = 364;
	_attributeEditor = new AttributeEditor(leftBorderSize, curY + outlinerHeight + 10, attributeEditorWidth - leftBorderSize, outlinerHeight, "");
	_attributeEditor->box(FL_UP_BOX);

	_leftGroup->end();

	// add the viewer
	int viewerWidth = 640 ;
	if (!useEditor)
		viewerWidth = w - 2 * leftBorderSize;
	int viewerHeight = h - curY - 10;
	std::string renderer = "custom";

	customViewer = new FltkViewer(outlinerWidth + leftBorderSize, curY, viewerWidth, viewerHeight, nullptr);
	curViewer = customViewer;

	curViewer->box(FL_UP_BOX);
	curViewer->baseWin = this;

	_mainGroup->end();
	_mainGroup->resizable(curViewer);

	this->end();

	this->resizable(_mainGroup);

	const boost::filesystem::path& curDir = boost::filesystem::current_path();
	scriptFolder = curDir.string();
	scriptFolder.append("/scripts");

	ReloadScriptsCB(nullptr, this);

	characterCreator = nullptr;

	resWindow = nullptr;

	visemeViewerWindow = nullptr;

	monitorConnectWindow = nullptr;

	motionEditorWindow = nullptr;

	retargetCreatorWindow = nullptr;

	faceViewerWindow = nullptr;
	faceShiftViewerWindow = nullptr;
	bmlViewerWindow = nullptr;
	dataViewerWindow = nullptr;
	
	panimationWindow = nullptr;	
	exportWindow = nullptr;
	poseCreator = nullptr;

	if (!useEditor)
	{
		// in non-editor mode, remove all widgets except for the viewer
		_leftGroup->hide();
		menubar->hide();
	}
}

BaseWindow::~BaseWindow() {
	sInstance = nullptr;
	delete customViewer;
	delete commandWindow;
	delete characterCreator;
	delete visemeViewerWindow;
	delete monitorConnectWindow;
	delete motionEditorWindow;
	delete retargetCreatorWindow;
	delete faceViewerWindow;
	delete faceViewerWindow;
	delete bmlViewerWindow;
	delete bmlCreatorWindow;
	delete dataViewerWindow;
	delete resourceWindow;
	delete panimationWindow;

}

BaseWindow& BaseWindow::getInstance()
{
	return *sInstance;
}

BaseWindow* BaseWindow::getInstancePtr()
{
	return sInstance;
}

void BaseWindow::changeLayoutMode(int mode)
{
	if (mode == 0)
	{
		// resource, attribute and viewer

	}
	else
	{
		// resource and attribute in separate window, viewer takes up entire window
		int leftX = _leftGroup->x();
		int leftY = _leftGroup->y();
		int leftW = _leftGroup->w();
		int leftH = _leftGroup->h();


		int children = this->children();
		for (int c = 0; c < children; c++)
		{
			Fl_Widget* widget = this->child(c);
			const char* label = widget->label();
		}

		int numChildren = _mainGroup->children();
		for (int c = 0; c < numChildren; c++)
		{
			_mainGroup->remove(0);
		}

		numChildren = _mainGroup->children();
		for (int c = 0; c < numChildren; c++)
		{
			_mainGroup->remove(0);
		}

		if (!standaloneResourceWindow)
		{
			standaloneResourceWindow = new Fl_Double_Window(leftX + 10, leftY + 10, leftW + 20, leftH + 20, "Resources and Attributes");
		}
		standaloneResourceWindow->add(_leftGroup);
		standaloneResourceWindow->resizable(_leftGroup);
		standaloneResourceWindow->resize(leftX + 10, leftY + 10, leftW + 20, leftH + 20);
		standaloneResourceWindow->redraw();
		standaloneResourceWindow->show();
		this->redraw();


		// resize the main window according to the viewer size
		int viewerX = curViewer->x();
		int viewerY = curViewer->y();
		int viewerW = curViewer->w();
		int viewerH = curViewer->h();

		int curX = x();
		int curY = y();
		curViewer->resize(10, 28, viewerW, viewerH);

		_mainGroup->add(curViewer);
		this->resizable(_mainGroup);
		_mainGroup->resizable(curViewer);

		_mainGroup->resize(curX + 10, curY + 28, viewerW, viewerH);
		this->resize(curX, curY, viewerW + 20, viewerH + 28);
	
		curViewer->damage(FL_DAMAGE_ALL);
		_mainGroup->damage(FL_DAMAGE_ALL);
		this->damage(FL_DAMAGE_ALL);
		
		
		

		this->redraw();





	}
}

std::string BaseWindow::chooseFile(const std::string& label, const std::string& filter, const std::string& defaultDirectory)
{
	Fl_Native_File_Chooser fnfc;
	fnfc.title(label.c_str());
	fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
	fnfc.filter(filter.c_str());
	fnfc.directory(defaultDirectory.c_str()); 

	std::string ret;

	switch ( fnfc.show() )
	{
	case -1: 
		return "";
		break;
	case  1: 
		return "";
		break;
		
	default: 
		ret = fnfc.filename(); 
		break;  
	}

	return ret;
}

std::string BaseWindow::chooseDirectory(const std::string& label, const std::string& defaultDirectory)
{
	Fl_Native_File_Chooser fnfc;
	fnfc.title(label.c_str());
	fnfc.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
	fnfc.directory(defaultDirectory.c_str()); 

	std::string ret;

	switch ( fnfc.show() )
	{
	case -1: 
		return "";
		break;
	case  1: 
		return "";
		break;
		
	default: 
		ret = fnfc.filename(); 
		break;  
	}

	return ret;
}


SbmCharacter* BaseWindow::getSelectedCharacter()
{
	 SbmPawn* selectedPawn = curViewer->getObjectManipulationHandle().get_selected_pawn();
	 if (!selectedPawn)
		 return nullptr;

	 SbmCharacter* character = dynamic_cast<SbmCharacter*> (selectedPawn);
	 return character;
}

void BaseWindow::show_viewer()
{
	#if !defined (__ANDROID__) && !defined(SBM_IPHONE)
		SbmShaderManager::singleton().setViewer(this);
	#endif	
	show();
	curViewer->show_viewer();	
}

void BaseWindow::hide_viewer()
{
	if (this->shown())
		this->hide();
}

void BaseWindow::set_camera ( const SrCamera* cam )
{
   curViewer->set_camera(cam);
}

SrCamera* BaseWindow::get_camera()
{
	return curViewer->get_camera();
}

void BaseWindow::render () 
{ 
	curViewer->redraw();
} 

void BaseWindow::root(SrSn* r)
{
	//fltkViewer->root(r);
}

SrSn* BaseWindow::root()
{
	return mSession->scene.getRootGroup();
}

void BaseWindow::resetWindow()
{
	if (commandWindow)
	{
		commandWindow->hide();
		delete commandWindow;
		commandWindow = nullptr;
	}
	if (bmlCreatorWindow)
	{
		bmlCreatorWindow->hide();
		delete bmlCreatorWindow;
		bmlCreatorWindow = nullptr;
	}
	if (retargetCreatorWindow)
	{
		retargetCreatorWindow->hide();
		delete retargetCreatorWindow;
		retargetCreatorWindow = nullptr;
	}

	if (curViewer->_retargetStepWindow)
	{
		curViewer->_retargetStepWindow->hide();
		delete curViewer->_retargetStepWindow;
		curViewer->_retargetStepWindow = nullptr;
	}

	if (visemeViewerWindow)
	{
		visemeViewerWindow->hide();
		delete visemeViewerWindow;
		visemeViewerWindow = nullptr;
	}
	if (monitorConnectWindow)
	{
		monitorConnectWindow->hide();
		delete monitorConnectWindow;
		monitorConnectWindow = nullptr;
	}
	if (motionEditorWindow)
	{
		motionEditorWindow->hide();
		delete motionEditorWindow;
		motionEditorWindow = nullptr;
	}

	if (characterCreator)
	{
		characterCreator->hide();
		delete characterCreator;
		characterCreator = nullptr;
	}
	if (visemeViewerWindow)
	{
		visemeViewerWindow->hide();
		delete visemeViewerWindow;
		visemeViewerWindow = nullptr;
	}

	if (faceViewerWindow)
	{
		faceViewerWindow->hide();
		delete faceViewerWindow;
		faceViewerWindow = nullptr;
	}
	if (bmlViewerWindow)
	{
		bmlViewerWindow->hide();
		delete bmlViewerWindow;
		bmlViewerWindow = nullptr;
	}
	if (dataViewerWindow)
	{
		dataViewerWindow->hide();
		delete dataViewerWindow;
		dataViewerWindow = nullptr;
	}
	if (panimationWindow)
	{
		panimationWindow->hide();
		delete panimationWindow;
		panimationWindow = nullptr;
	}
	if (exportWindow)
	{
		exportWindow->hide();
		delete exportWindow;
		exportWindow = nullptr;
	}
}

void BaseWindow::ResetScene()
{
	std::string mediaPath = SmartBody::SBScene::getSystemParameter("mediapath");
	resetWindow();
	SBGUIManager::singleton().resetGUI();
	curViewer->resetViewer();

	//Make a copy.
	std::vector<SmartBody::SBSceneListener*> listeners;

	for (auto& entry: mSession->scene.getSceneListeners()) {
		//HACK: we need to remove the RenderScene listener, as that will be destroyed when the session is destroyed.
		if (!dynamic_cast<SmartBody::SBRenderSceneListener*>(entry)) {
			listeners.emplace_back(entry);
		}
	}

	//HACK: To avoid having the UI crash when trying to handle the updates emitted when clearing the scene we'll just
	//remove all listeners here and rely on the UI correctly listening to "OnSimulationStart" to reset itself.
	mSession->scene.removeAllSceneListeners();

	delete Session::current;
	Session::current = new Session();
	mDebuggerClient = std::make_unique<SmartBody::SBDebuggerClient>(Session::current->scene);

	mSession = Session::current;

	SmartBody::PythonInterface::renderScene = &mSession->renderScene;

	SBBaseRenderer::singleton().resetRenderer();
	SBRenderer::singleton().resetRenderer();

	mSession->scene.startFileLogging("./smartbody.log");
	for (auto & listener : listeners)
	{
		mSession->scene.addSceneListener(listener);
	}

	for (auto & listener : listeners)
	{
		listener->OnSimulationStart();
	}
	
//	mSession->scene.setViewer(this);
	root(mSession->scene.getRootGroup());
	SbmShaderManager::singleton().setViewer(this);

	mSession->scene.getSimulationManager()->setupTimer();

	SrCamera* camera = mSession->renderScene.createCamera("cameraDefault");
	camera->reset();

	// setup python
	auto& scene = Session::current->scene;

	if (!mediaPath.empty()) {
		scene.setMediaPath(mediaPath);
	} else {
		scene.setMediaPath(SMARTBODY_DATADIR "/smartbody/data");
	}

	mSession->vhmMsgManager.setEnable(true);
	updateObjectList();

	this->curViewer->registerUIControls();
}


void BaseWindow::LoadPackageCB( Fl_Widget* widget, void* data )
{
	auto* window = (BaseWindow*) data;

	namespace fs = boost::filesystem;
	std::string mediaPath = SmartBody::SBScene::getSystemParameter("mediapath");
	std::string chooserTitle = "Choose Package Directory";
	std::string dirName = window->chooseDirectory(chooserTitle, mediaPath);
	std::string initScriptName = dirName+"/initScene.py";
	if (!fs::exists(fs::path(initScriptName)))
	{
		SmartBody::util::log("Package directory doesn't have 'initScene.py'. Abort loading.");
		return;
	}
	
	window->ResetScene();
	window->mSession->scene.setMediaPath(dirName);
	window->mSession->scene.addAssetPath("script", dirName);
	window->mSession->scene.runScript("initScene");
}

void BaseWindow::LoadCB(Fl_Widget* widget, void* data)
{
	auto* window = (BaseWindow*) data;
	std::string mediaPath = SmartBody::SBScene::getSystemParameter("mediapath");

	std::string file = window->chooseFile("Load File:", "Python\t*.py\n", mediaPath + "/examples");
	if (file.empty())
		return;

    window->ResetScene();

	std::string filebasename = boost::filesystem::basename(file);
	std::string fileextension = boost::filesystem::extension(file);

	std::string finalFile = filebasename + "." + fileextension;
	size_t filenameSize = finalFile.size();

	std::string fullfilename = std::string(file);

	std::string path = fullfilename.substr(0, fullfilename.size() - filenameSize);
	SmartBody::util::log("Path = %s, script = %s", path.c_str(), filebasename.c_str());
	window->mSession->scene.addAssetPath("script", path);
	window->mSession->scene.runScript(filebasename);
}

void BaseWindow::SaveCB(Fl_Widget* widget, void* data)
{
	auto* window = (BaseWindow*) data;

	std::string mediaPath = SmartBody::SBScene::getSystemParameter("mediapath");

	std::string saveFile = window->chooseFile("Save File:", "Python\t*.py\n", mediaPath);
	if (saveFile.empty())
		return;
	
	auto& scene = window->mSession->scene;
	std::ofstream file(saveFile.c_str());
	if (!file.good())
	{
		std::string message = "Cannot save to file '";
		message.append(saveFile);
		message.append("'");
		fl_alert(message.c_str());
	} else {
		SmartBody::save(window->mSession->renderScene, &Session::current->steerManager, file);
	}
	file.close();
	
	std::string scenePrompt = "Scene saved to file '";
	scenePrompt.append(saveFile);
	scenePrompt.append("'");
	fl_message(scenePrompt.c_str());
}

void BaseWindow::ExportPackageCB( Fl_Widget* widget, void* data )
{
	auto* window = (BaseWindow*) data;
	int useZip = (long)data;
	std::string mediaPath = SmartBody::SBScene::getSystemParameter("mediapath");
	auto& scene = window->mSession->scene;
	auto& renderScene = window->mSession->renderScene;

	std::string chooserTitle = "Choose Output Directory";
	std::string fileName; 
	if (!useZip)
	{
		fileName = BaseWindow::chooseDirectory(chooserTitle, mediaPath);
	}
	else
	{
		std::string fname = window->chooseFile("Save Scene File:", "Zip files\t*.zip\n", mediaPath);
		if (fname.empty())
			return;

		fileName = fname;
	}
	if (!fileName.empty())
	{
		//LOG("Select filename = %s",fileName.c_str());
		if (!useZip)
		{
			SmartBody::exportScenePackage(renderScene, &Session::current->steerManager, fileName);
		}
		else
		{
			boost::filesystem::path filePath(fileName);
			SmartBody::exportScenePackage(renderScene, &Session::current->steerManager, filePath.parent_path().string(),boost::filesystem::basename(fileName)+".zip");
		}
	}
}

void BaseWindow::ExportCB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->exportWindow)
	{
		rootWindow->exportWindow = new ExportWindow(rootWindow->x() + 50, rootWindow->y() + 50, 300, 600, "Export");
	}
	rootWindow->exportWindow->show();
}

void BaseWindow::SaveSceneSettingCB( Fl_Widget* widget, void* data )
{
	auto* window = (BaseWindow*) data;
	
	std::string mediaPath = SmartBody::SBScene::getSystemParameter("mediapath");

	std::string saveFile = window->chooseFile("Save File:", "Python\t*.py\n", mediaPath);
	if (saveFile.empty())
		return;

	auto& scene = window->mSession->scene;
	std::stringstream ss;

	std::ofstream file(saveFile.c_str());
	if (!file.good())
	{
		std::string message = "Cannot save to file '";
		message.append(saveFile);
		message.append("'");
		fl_alert(message.c_str());
	} else {
		SmartBody::saveSceneSettings(window->mSession->renderScene, file);
	}
	file.close();

	std::string scenePrompt = "Scene saved to file '";
	scenePrompt.append(saveFile);
	scenePrompt.append("'");
	fl_message(scenePrompt.c_str());
}

void BaseWindow::LoadSceneSettingCB( Fl_Widget* widget, void* data )
{
	auto* window = (BaseWindow*) data;

	auto& scene = window->mSession->scene;
	std::string mediaPath = scene.getMediaPath();

	std::string file = window->chooseFile("Load File:", "Python\t*.py\n", mediaPath);
	if (file.empty())
		return;

	if (!mediaPath.empty())
		scene.setMediaPath(mediaPath);
	std::string filebasename = boost::filesystem::basename(file);
	std::string fileextension = boost::filesystem::extension(file);
	std::string fullfilename = std::string(file);
	size_t pos = fullfilename.find(filebasename);
	std::string path = fullfilename.substr(0, pos - 1);
	scene.addAssetPath("script", path);
	scene.runScript(filebasename);

}

void BaseWindow::RunCB(Fl_Widget* widget, void* data)
{
}

void BaseWindow::LaunchBMLViewerCB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->bmlViewerWindow)
	{
		rootWindow->bmlViewerWindow = new BehaviorWindow(rootWindow->x() + 50, rootWindow->y() + 50, 800, 600, "BML Viewer");
		rootWindow->bmlViewerWindow->show_viewer();
	}
	rootWindow->bmlViewerWindow->show();
}

void BaseWindow::LaunchParamAnimViewerCB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->panimationWindow)
	{
		rootWindow->panimationWindow = new PanimationWindow(rootWindow->x() + 50, rootWindow->y() + 50, 800, 800, "Blend Viewer");
	}
	rootWindow->panimationWindow->show();
}

void BaseWindow::LaunchDataViewerCB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->dataViewerWindow)
	{
		rootWindow->dataViewerWindow = new ChannelBufferWindow(rootWindow->x() + 50, rootWindow->y() + 50, 800, 600, "Data Viewer");
	}
	rootWindow->dataViewerWindow->show();
}

void BaseWindow::LaunchResourceViewerCB( Fl_Widget* widget, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->_layoutMode == 0)
	{
		rootWindow->_layoutMode = 1;
	}
	else
	{
		rootWindow->_layoutMode = 0;
	}
	rootWindow->changeLayoutMode(rootWindow->_layoutMode);
	
}

void BaseWindow::LaunchFaceViewerCB( Fl_Widget* widget, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->faceViewerWindow)
	{
		rootWindow->faceViewerWindow = new FaceViewer(rootWindow->x() + 50, rootWindow->y() + 50, 800, 600, "Face Viewer");
	}
	rootWindow->faceViewerWindow->show();
}

void BaseWindow::LaunchFaceShiftViewerCB( Fl_Widget* widget, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->faceShiftViewerWindow)
	{
		rootWindow->faceShiftViewerWindow = new FaceShiftViewer(rootWindow->x() + 50, rootWindow->y() + 50, 800, 600, "FaceShift Viewer");
	}
	rootWindow->faceShiftViewerWindow->show();
}

void BaseWindow::LaunchPoseCreatorCB( Fl_Widget* widget, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->poseCreator)
	{
		rootWindow->poseCreator = new PoseCreator(rootWindow->x() + 50, rootWindow->y() + 50, 800, 600, "Pose Creator");
	}
	rootWindow->poseCreator->show();
}


void BaseWindow::LaunchSpeechRelayCB( Fl_Widget* widget, void* data )
{
	std::string speechRelayCommand;
	speechRelayCommand = SmartBody::SBScene::getSystemParameter("speechrelaycommand");
	if (speechRelayCommand.empty())
	{
#ifdef WIN32
		speechRelayCommand = "start ..\\..\\..\\..\\bin\\TtsRelay\\bin\\x86\\Release\\TtsRelayGui.exe";
		// run the speech relay launcher script
#else
		speechRelayCommand = "../../../../core/FestivalRelay/speechrelay.sh&";
#endif
	}
	int ret = system(speechRelayCommand.c_str());
	if (ret == -1)
	{
		SmartBody::util::log("Speech relay command failed: %s", speechRelayCommand.c_str());
	}
}

void BaseWindow::NewCB(Fl_Widget* widget, void* data)
{
	auto* window = (BaseWindow*) data;
	int confirm = fl_choice("This will reset the current session.\nContinue?", "No", "Yes", nullptr);
	if (confirm == 1)
	{
		window->ResetScene(); // should call the same function to be consistent
	}
}

void BaseWindow::QuitCB(Fl_Widget* widget, void* data)
{
	int confirm = fl_choice("This will quit SmartBody.\nContinue?", "No", "Yes", nullptr);
	if (confirm == 1)
	{
		auto* window = (BaseWindow*) data;
		Session::current->scene.getSimulationManager()->stop();
	}
}

void BaseWindow::QuickConnectCB(Fl_Widget* widget, void* data)
{
   auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->monitorConnectWindow)
	{
		rootWindow->monitorConnectWindow = new MonitorConnectWindow(150, 150, 320, 400, "Monitor Connect", true, *rootWindow->mDebuggerClient);
	}
}

void BaseWindow::LaunchConnectCB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->monitorConnectWindow)
	{
		rootWindow->monitorConnectWindow = new MonitorConnectWindow(150, 150, 320, 400, "Monitor Connect", false, *rootWindow->mDebuggerClient);
	}

	rootWindow->monitorConnectWindow->show();	
}

void BaseWindow::DisconnectRemoteCB(Fl_Widget* widget, void* data)
{
    auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->mDebuggerClient->Disconnect();
	rootWindow->ResetScene();
}

void BaseWindow::LaunchConsoleCB(Fl_Widget* widget, void* data)
{
	// console doesn't receive commands - why?
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->commandWindow)
	{
		rootWindow->commandWindow = new CommandWindow(150, 150, 640, 480, "Commands");
	}

	rootWindow->commandWindow->show();
}

void BaseWindow::LaunchBMLCreatorCB(Fl_Widget* widget, void* data)
{
	// console doesn't receive commands - why?
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->bmlCreatorWindow)
	{
		rootWindow->bmlCreatorWindow = new BMLCreatorWindow(150, 150, 800, 600, "BML Commands");
	}

	rootWindow->bmlCreatorWindow->show();
}

void BaseWindow::LaunchRetargetCreatorCB(Fl_Widget* widget, void* data)
{
	// console doesn't receive commands - why?
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->retargetCreatorWindow)
	{
		rootWindow->retargetCreatorWindow = new RetargetCreatorWindow(150, 150, 800, 600, "Retarget Creator");
	}
	rootWindow->retargetCreatorWindow->show();
}

// void BaseWindow::LaunchBehaviorSetsCB(Fl_Widget* widget, void* data)
// {
// 	// console doesn't receive commands - why?
// 	auto* rootWindow = static_cast<BaseWindow*>(data);
// 	if (!rootWindow->behaviorSetViewer)
// 	{
// 		rootWindow->behaviorSetViewer = new RetargetViewer(150, 150, 320, 520, "Behavior Sets");
// 	}
// 	rootWindow->behaviorSetViewer->show();
// }


void BaseWindow::LaunchJointMapViewerCB( Fl_Widget* widget, void* data )
{
	// console doesn't receive commands - why?
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	SmartBody::SBBehaviorSetManager* manager = scene.getBehaviorSetManager();
	if (manager->getNumBehaviorSets() == 0)
	{
		// look for the behavior set directory under the media path
		scene.addAssetPath("script", "behaviorsets");
		scene.runScript("default-behavior-sets.py");

		if (manager->getNumBehaviorSets() == 0)
		{
			SmartBody::util::log("Can not find any behavior sets under path %s/behaviorsets.", scene.getMediaPath().c_str());
		}
		else
		{
			SmartBody::util::log("Found %d behavior sets under path %s/behaviorsets", manager->getNumBehaviorSets(), scene.getMediaPath().c_str());
		}
	}
	if (rootWindow->curViewer && !rootWindow->curViewer->_retargetStepWindow)
	{
		rootWindow->curViewer->_retargetStepWindow = new RetargetStepWindow(150, 150, 1024, 500, "Rigging and Retargeting");
	}
	rootWindow->curViewer->_retargetStepWindow->setApplyType(true);
	rootWindow->curViewer->_retargetStepWindow->show();
}


void BaseWindow::LaunchMotionEditorCB(Fl_Widget* widget, void* data)
{
	// console doesn't receive commands - why?
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->motionEditorWindow)
	{
		rootWindow->motionEditorWindow = new MotionEditorWindow(150, 150, 425, 725, "Motion Editor");
	}
	rootWindow->motionEditorWindow->show();
}

void BaseWindow::LaunchVisemeViewerCB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (!rootWindow->visemeViewerWindow)
	{
		rootWindow->visemeViewerWindow = new VisemeViewerWindow(150, 150, 800, 800, "Viseme Configuration");
	}

	rootWindow->visemeViewerWindow->show();
}




void BaseWindow::StartCB(Fl_Widget* widget, void* data)
{
}

void BaseWindow::StopCB(Fl_Widget* widget, void* data)
{
}

void BaseWindow::StepCB(Fl_Widget* widget, void* data)
{

}

void BaseWindow::PauseCB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	scene.command((char*)"time pause");
}

void BaseWindow::ResetCB(Fl_Widget* widget, void* data)
{
	//mSession->scene.command((char*)"reset");
	auto* window = (BaseWindow*) data;
	//window->resetWindow();	
	window->ResetScene();
}

void BaseWindow::CameraResetCB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	SrCamera* camera = rootWindow->mSession->renderScene.getActiveCamera();
	if (!camera)
		return;
	camera->reset();	
}

void BaseWindow::CameraFrameCB(Fl_Widget* widget, void* data)
{
	//mSession->scene.command((char*)"camera frame");
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	auto& renderScene = rootWindow->mSession->renderScene;
	SrBox sceneBox;
	SrCamera* camera = renderScene.getActiveCamera();
	if (!camera) return;

	const std::vector<std::string>& pawnNames =  scene.getPawnNames();
	for (const auto & pawnName : pawnNames)
	{
		SmartBody::SBPawn* pawn = scene.getPawn(pawnName);
		bool visible = pawn->getBoolAttribute("visible");
		if (!visible)
			continue;
		SrBox box = pawn->getSkeleton()->getBoundingBox();
		if (box.volume() < .0001)
		{
			double val = 1.0 / scene.getScale() * .5;
			box.grows((float) val, (float) val, (float) val);
		}
		sceneBox.extend(box);
	}
	camera->view_all(sceneBox, camera->getFov());	
	float scale = 1.f/scene.getScale();
	float znear = 0.01f*scale;
	float zfar = 1000.0f*scale;
	camera->setNearPlane(znear);
	camera->setFarPlane(zfar);
}

void BaseWindow::CameraFrameObjectCB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	auto& renderScene = rootWindow->mSession->renderScene;
	SbmPawn* pawn = rootWindow->curViewer->getObjectManipulationHandle().get_selected_pawn();
	if (!pawn)
	{
		pawn = rootWindow->getSelectedCharacter();
		if (!pawn)
			return;
	}

	//mSession->scene.command((char*)"camera frame");
	SrBox sceneBox;
	SrCamera* camera = renderScene.getActiveCamera();
	if (!camera) return;

	SrBox box = pawn->getSkeleton()->getBoundingBox();
	if (box.volume() < .0001)
	{
			double val = 1.0 / scene.getScale() * .5;
			box.grows((float) val, (float) val, (float) val);
	}

	sceneBox.extend(box);

	camera->view_all(sceneBox, camera->getFov());	
	float scale = 1.f/scene.getScale();
	float znear = 0.01f*scale;
	float zfar = 1000.0f*scale;
	camera->setNearPlane(znear);
	camera->setFarPlane(zfar);
}


void BaseWindow::RotateSelectedCB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	
	SbmPawn* pawn = rootWindow->curViewer->getObjectManipulationHandle().get_selected_pawn();
	if (!pawn)
	{
		pawn = rootWindow->getSelectedCharacter();
		if (!pawn)
			return;
	}

	SrCamera* camera = rootWindow->mSession->renderScene.getActiveCamera();
	if (!camera)
		return;
	float x,y,z,h,p,r;
	pawn->get_world_offset(x, y, z, h, p, r);
	camera->setCenter(x, y, z);
	float scale = 1.f/rootWindow->mSession->scene.getScale();
	float znear = 0.01f*scale;
	float zfar = 1000.0f*scale;
	camera->setNearPlane(znear);
	camera->setFarPlane(zfar);
}

void BaseWindow::SetDefaultCamera(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->getData()->cameraMode = FltkViewer::Default;
	rootWindow->mSession->renderScene.SetCameraLocked(false);
}

void BaseWindow::SetFreeLookCamera(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->getData()->cameraMode = FltkViewer::FreeLook;
	rootWindow->mSession->renderScene.SetCameraLocked(false);
}

void BaseWindow::SetFollowRendererCamera(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->mSession->scene.isRemoteMode())
    {
	  rootWindow->curViewer->getData()->cameraMode = FltkViewer::FollowRenderer;
	   rootWindow->mSession->renderScene.SetCameraLocked(true);
   }
}

// Callback function for Camera->Character Camera Sight to set camera viewpoint as the character viewpoint
void BaseWindow::CameraCharacterShightCB(Fl_Widget* widget, void* data) 
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	auto& renderScene = rootWindow->mSession->renderScene;

	SrCamera* camera = renderScene.getActiveCamera();
	if (!camera)
		return;

	// If coneOfSight is being disabled
	if (renderScene.hasConeOfSight()) {
		SbmCharacter* character = rootWindow->getSelectedCharacter();
		// If an object is selected, camera frames it
		if(character)
			CameraFrameObjectCB(widget, data);
		// If non object is selected, camera frames all scene
		else
			CameraFrameCB(widget, data);
		renderScene.removeConeOfSight();
		SmartBody::util::log("Camera sight: OFF");
	} else {
		SbmCharacter* character = rootWindow->getSelectedCharacter();
		if(character) {
			if(renderScene.setCameraConeOfSight(character->getName())) {
				// Renders eye beams
				rootWindow->curViewer->getData()->eyeBeamMode = FltkViewer::ModeEyeBeams;
				SmartBody::util::log("Camera sight: ON");
			}
		} else {
			SmartBody::util::log("No character selected. Can't enable coneOfSight. ");
		}
	}
}

void BaseWindow::FaceCameraCB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	auto& renderScene = rootWindow->mSession->renderScene;

	SbmCharacter* character = rootWindow->getSelectedCharacter();
	if (!character)
		return;
	
	// position the camera such that the character's face appears in the frame
	SrBox faceBox;
	SrCamera* camera = renderScene.getActiveCamera();
	if (!camera)
		return;

	auto skeleton = character->getSkeleton();
	float height = skeleton->getCurrentHeight();
	SkJoint* joint = skeleton->search_joint("eyeball_left");
	SkJoint* joint2 = skeleton->search_joint("eyeball_right");
	SkJoint* baseJoint = skeleton->search_joint("base");

	skeleton->update_global_matrices();
	if (joint && joint2 && baseJoint)
	{
		joint->update_gmat();
		const SrMat& gmat = joint->gmat();
		SrVec point(gmat.get(3, 0), gmat.get(3, 1), gmat.get(3, 2));
		faceBox.extend(point);

		joint2->update_gmat();
		const SrMat& gmat2 = joint2->gmat();
		SrVec point2(gmat2.get(3, 0), gmat2.get(3, 1), gmat2.get(3, 2));
		faceBox.extend(point2);

		// get the facing vector of the character's body
		baseJoint->update_gmat();
		SkJointQuat* quat = baseJoint->quat();
		SrVec facing(.0f, .0f, 1.0f);
		SrVec facingVector = facing * quat->value();
		facingVector.y = .0f;
		facingVector.normalize();

		float max = faceBox.max_size();
		float min = faceBox.min_size();

		SrVec tmpCenter = (point + point2) / 2.0f;
		camera->setCenter(tmpCenter.x, tmpCenter.y, tmpCenter.z);
		SrVec tmp = camera->getCenter() + height / 4.0f * facingVector;
		camera->setEye(tmp.x, tmp.y, tmp.z);
		float scale = 1.f/scene.getScale();
		float znear = 0.01f*scale;
		float zfar = 1000.0f*scale;
		camera->setNearPlane(znear);
		camera->setFarPlane(zfar);
	}
	else
	{
		return;
	}
}

void BaseWindow::RunScriptCB(Fl_Widget* w, void* data)
{
	fl_alert("Not implemented");
	/*
	auto* rootWindow = static_cast<BaseWindow*>(data);

	// determine which script was selected
	Fl_Widget* widget = w;

	std::string filename = "";

	Fl_Menu_Bar* curWidget = widget;
	while (curWidget->parent() != rootWindow->menubar)
	{
		filename.insert(0, curWidget->label());
		filename.insert(0, "/");
		curWidget = curWidget->parent();
	}

	const boost::filesystem::path& scriptPath(rootWindow->scriptFolder);
	std::string scriptName = scriptPath.string();
	scriptName.append(filename);
	rootWindow->runScript(scriptName);
	*/
}

void BaseWindow::ReloadScriptsCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);

	std::string buff;
	const boost::filesystem::path& curDir = rootWindow->scriptFolder;
	buff.append(curDir.string());
	rootWindow->reloadScripts(buff);
}

void BaseWindow::SetScriptDirCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);

	const char* directory = fl_dir_chooser("Select the script folder:", rootWindow->scriptFolder.c_str());
	if (!directory)
		return;

	rootWindow->scriptFolder = directory;
}

void BaseWindow::runScript(const std::string& filename)
{
	std::ifstream file(filename.c_str());
	if (!file.good())
	{
		std::string message = "Filename '";
		message.append(filename);
		message.append("' is not a valid file.");
		fl_alert(message.c_str());
		file.close();
	}

	SbmCharacter* character = getSelectedCharacter();
	std::string selectedCharacterName;
	if (character)
		selectedCharacterName = character->getName();
	SbmPawn* pawn = curViewer->getObjectManipulationHandle().get_selected_pawn();
	std::string selectedTargetName = "";
	if (pawn)
		selectedTargetName = pawn->getName();

	char line[8192];
	while(!file.eof() && file.good())
	{
		file.getline(line, 8192, '\n');
                // remove any trailing \r
                if (line[strlen(line) - 1] == '\r')
                        line[strlen(line) - 1] = '\0';
		if (strlen(line) == 0) // ignore blank lines
			continue;

		std::string lineStr = line;
		// process special handlers
		int inputPos = lineStr.find("$INPUT(");
		if (inputPos != std::string::npos)
		{
			// find the final ')'
			int parenPos = lineStr.find(")", inputPos);
			if (parenPos == std::string::npos)
				parenPos = lineStr.length() - 1;
			std::string text = lineStr.substr(inputPos + 7, parenPos - inputPos);
			const char* response = fl_input(text.c_str());
			std::string responseStr = "";
			if (response)
				responseStr = response;
			
			// reassemble the line
			std::string begin = lineStr.substr(0, inputPos);
			std::string end = lineStr.substr(parenPos);
			lineStr = begin;
			lineStr.append(response);
			lineStr.append(end);
		}

		// replace any variables
		boost::replace_all(lineStr, "$CHARACTER", selectedCharacterName);
		boost::replace_all(lineStr, "$TARGET", selectedTargetName);

		mSession->scene.command((char*)lineStr.c_str());
	}
	file.close();
}

void BaseWindow::reloadScripts(std::string scriptsDir)
{
	//LOG("Not yet implemented");
	/*
	// erase the old scripts menu
	for (int x = menubar->size() - 1; x >= 0; x--)
	{
		if (strcmp( menubar->menu()[x].label(), "&Scripts") == 0)
		{
			menubar->value(clear();
		}
	}

	// create the new menu
	menubar->add("Scripts/Reload Scripts", 0, BaseWindow::ReloadScriptsCB, this, nullptr);
	menubar->add("Scripts/Set Script Folder", 0, BaseWindow::SetScriptDirCB, this, FL_MENU_DIVIDER);
	reloadScriptsByDir(scriptsDir, "");
	*/
}

void BaseWindow::reloadScriptsByDir(std::string scriptsDir, std::string parentStr)
{
#ifdef WIN32
	// eliminate the current list
	char path[2048];
	WIN32_FIND_DATA fd;
	DWORD dwAttr = FILE_ATTRIBUTE_DIRECTORY;
	//if( !bCountHidden) dwAttr |= FILE_ATTRIBUTE_HIDDEN;
	sprintf( path, "%s/*", scriptsDir.c_str());
	HANDLE hFind = FindFirstFile( path, &fd);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		int count = 0;
		do
		{
			if( !(fd.dwFileAttributes & dwAttr))
			{
				// add the name to the root window
				char entry[512];
				sprintf(entry, "Scripts/%s%s", parentStr.c_str(), fd.cFileName);
				menubar->add(entry, 0, BaseWindow::RunScriptCB, this, 0);
			}
			else 
			{
				if (!strcmp(fd.cFileName, "..") == 0 && !strcmp(fd.cFileName, ".") == 0)
				{
					// recurse into this directory
					char newdir[1024];
					sprintf(newdir, "%s/%s", scriptsDir.c_str(), fd.cFileName);
					char newParentStr[1024];
					sprintf(newParentStr, "%s%s/", parentStr.c_str(), fd.cFileName);

					std::string nextDir = newParentStr;
					reloadScriptsByDir(newdir, nextDir);
				}
			}
		} while( FindNextFile( hFind, &fd));
		FindClose( hFind);
	}
#else
	char buff[8192];
	//danceInterp::getDirectoryListing(buff, 8192, (char*) scriptsDir);
	char* token = strtok(buff, " ");
	char scriptName[512];
	std::vector<std::string> allentries;
	while (token != nullptr)
	{
		std::string s = token;
		allentries.emplace_back(s);
		token = strtok(nullptr, " ");
	}

	for (auto & entry : allentries)
	{
		char absfilename[2048];
		sprintf(absfilename, "%s%s", scriptsDir.c_str(), entry.c_str());

#ifdef __APPL__
		if (!filename_isdir(absfilename))
		{

			if (filename_match(allentries[x].c_str(), "*.py"))
			{
				strncpy(scriptName, allentries[x].c_str(),  strlen(allentries[x].c_str()) - 3);
				scriptName[strlen(allentries[x].c_str()) - 3] = '\0';
				// add the name to the root window
				char entry[512];
				sprintf(entry, "Scripts/%s%s", parentStr.c_str(), scriptName);
				menubar->add(entry, 0, BaseWindow::RunScriptCB, 0, 0);
			}
		}
		else
		{
			if (!strcmp(allentries[x].c_str(), "..") == 0 && !strcmp(allentries[x].c_str(),  ".") == 0)
			{
				// recurse into this directory
				char newdir[1024];
				sprintf(newdir, "%s/%s", scriptsDir.c_str(), allentries[x].c_str());
				char newParentStr[1024];
				sprintf(newParentStr, "%s%s/", parentStr.c_str(), allentries[x].c_str());
				reloadScriptsByDir(newdir, newParentStr);
			}
		}
#endif

	}

#endif
}

void BaseWindow::ModeBonesCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;

	const std::vector<std::string>& characters = scene.getCharacterNames();
	for (const auto & c : characters)
	{
		SmartBody::SBCharacter* character = scene.getCharacter(c);
		character->setStringAttribute("displayType", "bones");
	}

// #if !NO_OGRE_VIEWER_CMD
// 	auto* rootWindow = static_cast<BaseWindow*>(data);
// 	rootWindow->fltkViewer->menu_cmd(FltkViewer::CmdCharacterShowBones, nullptr);	
// #endif
}

void BaseWindow::ModeGeometryCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;

	const std::vector<std::string>& characters = scene.getCharacterNames();
	for (const auto & c : characters)
	{
		SmartBody::SBCharacter* character = scene.getCharacter(c);
		character->setStringAttribute("displayType", "visgeo");
	}
}

void BaseWindow::ModeCollisionGeometryCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;

	const std::vector<std::string>& characters = scene.getCharacterNames();
	for (const auto & c : characters)
	{
		SmartBody::SBCharacter* character = scene.getCharacter(c);
		character->setStringAttribute("displayType", "colgeo");
	}
}


void BaseWindow::ModeSkinWeightCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;

	const std::vector<std::string>& characters = scene.getCharacterNames();
	for (const auto & c : characters)
	{
		SmartBody::SBCharacter* character = scene.getCharacter(c);
		character->setStringAttribute("displayType", "skinWeight");
	}
// #if !NO_OGRE_VIEWER_CMD
// 	auto* rootWindow = static_cast<BaseWindow*>(data);
// 	rootWindow->fltkViewer->menu_cmd(FltkViewer::CmdCharacterShowSkinWeight, nullptr);	
// #endif
}

void BaseWindow::ModeDeformableGeometryCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;

	const std::vector<std::string>& characters = scene.getCharacterNames();
	for (const auto & c : characters)
	{
		SmartBody::SBCharacter* character = scene.getCharacter(c);
		character->setStringAttribute("displayType", "mesh");
	}

// #if !NO_OGRE_VIEWER_CMD
// 	auto* rootWindow = static_cast<BaseWindow*>(data);
// 	rootWindow->fltkViewer->menu_cmd(FltkViewer::CmdCharacterShowDeformableGeometry, nullptr);	
// #endif
}

void BaseWindow::ModeGPUDeformableGeometryCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;

	const std::vector<std::string>& characters = scene.getCharacterNames();
	for (const auto & c : characters)
	{
		SmartBody::SBCharacter* character = scene.getCharacter(c);
		character->setStringAttribute("displayType", "GPUmesh");
	}

// #if !NO_OGRE_VIEWER_CMD
// 	auto* rootWindow = static_cast<BaseWindow*>(data);
// 	rootWindow->fltkViewer->menu_cmd(FltkViewer::CmdCharacterShowDeformableGeometryGPU, nullptr);	
// #endif
}

void BaseWindow::ModeAxisCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;

	const std::vector<std::string>& characters = scene.getCharacterNames();
	for (const auto & c : characters)
	{
		SmartBody::SBCharacter* character = scene.getCharacter(c);
		character->setStringAttribute("displayType", "axis");
	}

// #if !NO_OGRE_VIEWER_CMD
// 	auto* rootWindow = static_cast<BaseWindow*>(data);
// 	rootWindow->fltkViewer->menu_cmd(FltkViewer::CmdCharacterShowAxis, nullptr);	
// #endif
}

void BaseWindow::ModeEyebeamsCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);

	if (rootWindow->curViewer->getData()->eyeBeamMode)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdNoEyeBeams, nullptr);
	else
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdEyeBeams, nullptr);
}

void BaseWindow::ModeGazeLimitCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);

	if (rootWindow->curViewer->getData()->gazeLimitMode)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdNoGazeLimit, nullptr);
	else
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdGazeLimit, nullptr);
}

void BaseWindow::ModeEyelidCalibrationCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->curViewer->getData()->eyeLidMode == FltkViewer::ModeNoEyeLids)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdEyeLids, nullptr);
	else
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdNoEyeLids, nullptr);
}

void BaseWindow::ShowSelectedCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);

	rootWindow->curViewer->menu_cmd(FltkViewer::CmdShowSelection, nullptr);
}

void BaseWindow::ShadowsCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->curViewer->getData()->shadowmode == FltkViewer::ModeNoShadows)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdShadows, nullptr);
	else
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdNoShadows, nullptr);
}


void BaseWindow::ShadowsNoneCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->getData()->shadowmode = FltkViewer::ModeNoShadows;
	rootWindow->curViewer->updateOptions();
}

void BaseWindow::ShadowsMapCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->getData()->shadowmode = FltkViewer::ModeShadowMap;
	rootWindow->curViewer->updateOptions();
}

void BaseWindow::ShadowsStencilCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->getData()->shadowmode = FltkViewer::ModeShadowStencil;
	rootWindow->curViewer->updateOptions();
}

void BaseWindow::FloorCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->getData()->showFloor = !rootWindow->curViewer->getData()->showFloor;
	rootWindow->curViewer->updateOptions();
}

void BaseWindow::FloorColorCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdFloorColor, nullptr);	

}

void BaseWindow::BackgroundColorCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdBackground, nullptr);
	rootWindow->curViewer->updateOptions();
}

void BaseWindow::TerrainShadedCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->curViewer->getData()->terrainMode != FltkViewer::ModeTerrain)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdTerrain, nullptr);
}

void BaseWindow::TerrainWireframeCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->curViewer->getData()->terrainMode != FltkViewer::ModeTerrainWireframe)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdTerrainWireframe, nullptr);
}
void BaseWindow::TerrainNoneCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->curViewer->getData()->terrainMode != FltkViewer::ModeNoTerrain)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdNoTerrain, nullptr);
}

void BaseWindow::NavigationMeshNaviMeshCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->curViewer->getData()->navigationMeshMode != FltkViewer::ModeNavigationMesh)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdNavigationMesh, nullptr);
}

void BaseWindow::NavigationMeshRawMeshCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->curViewer->getData()->navigationMeshMode != FltkViewer::ModeRawMesh)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdRawMesh, nullptr);

}

void BaseWindow::NavigationMeshNoneCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->curViewer->getData()->navigationMeshMode != FltkViewer::ModeNoNavigationMesh)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdNoNavigationMesh, nullptr);
}

void BaseWindow::ShowPawns(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->curViewer->getData()->pawnmode != FltkViewer::ModePawnShowAsSpheres)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdPawnShowAsSpheres, nullptr);
	else
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdNoPawns, nullptr);
}

void BaseWindow::ModeDynamicsCOMCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->curViewer->getData()->dynamicsMode != FltkViewer::ModeShowCOM)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdShowCOM, nullptr);
}

void BaseWindow::ModeDynamicsSupportPolygonCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->curViewer->getData()->dynamicsMode != FltkViewer::ModeShowCOMSupportPolygon)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdShowCOMSupportPolygon, nullptr);
}

void BaseWindow::ModeDynamicsMassesCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->curViewer->getData()->dynamicsMode != FltkViewer::ModeShowMasses)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdShowMasses, nullptr);
}


void BaseWindow::ShowBoundingVolumeCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdShowBoundingVolume, nullptr);
}

void BaseWindow::SettingsDefaultMediaPathCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;

	std::string path = scene.getSystemParameter("mediapath");

	const char* result = fl_input("Default Media Path", path.c_str());
	if (result)
	{
		scene.setSystemParameter("mediapath", result);
	}
}

void BaseWindow::AudioCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	
	const bool val = scene.getBoolAttribute("internalAudio");
	scene.setBoolAttribute("internalAudio", !val);
}

void BaseWindow::CreateCharacterCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;

	std::vector<std::string> skeletonNames = scene.getSkeletonNames();
	
	if (!rootWindow->characterCreator)
		rootWindow->characterCreator = new CharacterCreatorWindow(rootWindow->x() + 20, rootWindow->y() + 20, 480, 150, strdup("Create a Character"));
	
	
	std::string characterName = "char" + std::to_string(rootWindow->characterCreator->numCharacter);
	rootWindow->characterCreator->inputName->value(characterName.c_str());
	rootWindow->characterCreator->setSkeletons(skeletonNames);
	rootWindow->characterCreator->show();	
}

void BaseWindow::CreatePawnCB(Fl_Widget* w, void* data)
{
//#if !NO_OGRE_VIEWER_CMD
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->create_pawn();
//#endif
}

void BaseWindow::CreatePawnFromModelCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	std::string pawnName = rootWindow->curViewer->create_pawn();
	SmartBody::SBPawn* pawn = rootWindow->mSession->scene.getPawn(pawnName);
	if (!pawn)
		return;

	const std::string& currentSelection = SBSelectionManager::getSelectionManager()->getCurrentSelection();
	SmartBody::util::log(currentSelection.c_str());
	// get the first model
	std::vector<std::string> meshes = rootWindow->mSession->renderAssetManager.getDeformableMeshNames();
	if (!meshes.empty())
	{
		pawn->setStringAttribute("mesh", meshes.front());
		pawn->setDoubleAttribute("rotY",180.0);
		pawn->setDoubleAttribute("rotZ",-90.0);
		auto renderable = rootWindow->mSession->renderScene.getRenderable(pawn->getName());
		if (renderable) {
			auto mesh = renderable->getActiveMesh();
			if (mesh) {
				mesh->setVisibility(2);
			}
		}
	}
	
}

void BaseWindow::CreateLightCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);

	SmartBody::SBScene& scene = rootWindow->mSession->scene;
	int highestLightNum = 0;
	const std::vector<std::string>& pawnNames = scene.getPawnNames();
	for (const auto & pawnName : pawnNames)
	{
			if (pawnName.find("light") == 0)
		{
			std::string lightNumStr = pawnName.substr(5, pawnName.size());
			int lightNum = atoi(lightNumStr.c_str());
			if (lightNum >= highestLightNum)
				highestLightNum = lightNum + 1;
		}

	}
	std::stringstream strstr;
	strstr << "light = scene.createPawn(\"light" << highestLightNum << "\")\n";
	strstr << "light.createBoolAttribute(\"enabled\", True, True, \"LightParameters\", 200, False, False, False, \"Is the light enabled?\")\n";
	strstr << "light.createBoolAttribute(\"lightIsDirectional\", True, True, \"LightParameters\", 205, False, False, False, \"Is the light directional?\")\n";
	strstr << "light.createBoolAttribute(\"lightIsPoint\", False, True, \"LightParameters\", 206, False, False, False, \"Is the light a point light?\")\n";
	strstr << "light.createVec3Attribute(\"lightDiffuseColor\", 1, .95, .8, True, \"LightParameters\", 210, False, False, False, \" Diffuse light color\")\n";
	strstr << "light.createVec3Attribute(\"lightAmbientColor\", 0, 0, 0, True, \"LightParameters\", 220, False, False, False, \" Ambient light color\")\n";
	strstr << "light.createVec3Attribute(\"lightSpecularColor\", 0, 0, 0, True, \"LightParameters\", 230, False, False, False, \"Specular light color\")\n";
	strstr << "light.createDoubleAttribute(\"lightSpotExponent\", 0, True, \"LightParameters\", 240, False, False, False, \" Spotlight exponent.\")\n";
	strstr << "light.createVec3Attribute(\"lightSpotDirection\", 0, 0, -1, True, \"LightParameters\", 250, False, False, False, \"Spotlight direction\")\n";
	strstr << "light.createDoubleAttribute(\"lightSpotCutoff\", 180, True, \"LightParameters\", 260, False, False, False, \"Spotlight cutoff angle\")\n";
	strstr << "light.createDoubleAttribute(\"lightConstantAttenuation\", 1, True, \"LightParameters\", 270, False, False, False, \"Constant attenuation\")\n";
	strstr << "light.createDoubleAttribute(\"lightLinearAttenuation\", 1, True, \"LightParameters\", 280, False, False, False, \" Linear attenuation.\")\n";
	strstr << "light.createDoubleAttribute(\"lightQuadraticAttenuation\", 0, True, \"LightParameters\", 290, False, False, False, \"Quadratic attenuation\")\n";
	strstr << "light.createBoolAttribute(\"lightCastShadow\", True, True, \"LightParameters\", 300, False, False, False, \"Does the light cast shadow?\")\n";
	strstr << "light.createIntAttribute(\"lightShadowMapSize\", 1024, True, \"LightParameters\", 310, False, False, False, \"Size of the shadow map\")\n";
	strstr << "light.setBoolAttribute(\"visible\", False)\n";	
	scene.run(strstr.str());
}

void BaseWindow::CreateCameraCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	auto& renderScene = rootWindow->mSession->renderScene;
	std::string cameraName = "camera";
	cameraName += boost::lexical_cast<std::string>(rootWindow->cameraCount++);
	const char* userCamName = fl_input("Camera name:", cameraName.c_str());
	if (!userCamName)
	{
		rootWindow->cameraCount--;
		return;
	}

	std::string cameraNameStr = userCamName;
	SrCamera* camera = renderScene.createCamera(cameraNameStr);

	if (!camera)
	{
		fl_alert("Camera with name '%s' cannot be created.", cameraNameStr.c_str());
		return;
	}
	float scale = 1.f/scene.getScale();
	SrVec camEye = SrVec(0,1.66f,1.85f)*scale;
	SrVec camCenter = SrVec(0,0.92f,0)*scale;	
	float znear = 0.01f*scale;
	float zfar = 1000.0f*scale;
	camera->setEye(camEye[0],camEye[1],camEye[2]);
	camera->setCenter(camCenter[0],camCenter[1],camCenter[2]);
	camera->setNearPlane(znear);
	camera->setFarPlane(zfar);

	if (!renderScene.getActiveCamera())
		renderScene.setActiveCamera(camera);
}

void BaseWindow::CreateTerrainCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;

	std::string mediaPath = SmartBody::SBScene::getSystemParameter("mediapath");
	std::string terrainFile = rootWindow->chooseFile("Load Terrain:", "PPM files\t*.ppm\n", mediaPath);
	if (terrainFile.empty())
		return;

	std::string terrainCommand = "terrain load ";
	terrainCommand.append(terrainFile);
	scene.command((char*)terrainCommand.c_str());
	if (scene.getHeightfield())
	{
		scene.getHeightfield()->set_scale( 5000.0f, 300.0f, 5000.0f );
		scene.getHeightfield()->set_auto_origin();
	}
}

//	Callback function to enable screengrab in JPG format, per frame
void BaseWindow::SetTakeSnapshotCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	
	if(!rootWindow->curViewer->getData()->saveSnapshot)
	{
		//TODO: make work on all OSes
//		FltkViewerData* data	= rootWindow->curViewer->getData();
//		std::string framesPath	= "C:/tmp/frames/";
//		const char* userInput	= fl_input("Path to store JPG files:", framesPath.c_str());
//		data->snapshotPath		= userInput;
		
	}
	else
	{
		fl_message("Store JPG: Disabled");
	}

	rootWindow->curViewer->getData()->saveSnapshot  = !rootWindow->curViewer->getData()->saveSnapshot;
}


//	Callback function to enable screengrab in TGA format, per frame
void BaseWindow::SetTakeSnapshot_tgaCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
		
	if(!rootWindow->curViewer->getData()->saveSnapshot_tga)
	{
		FltkViewerData* viewerData	= rootWindow->curViewer->getData();
		std::string framesPath	= "C:/tmp/frames/";
		const char* userInput	= fl_input("Path to store TGA files:", framesPath.c_str());
		viewerData->snapshotPath		= userInput;
	}
	else
	{
		fl_message("Store TGA: Disabled");
	}
	
	rootWindow->curViewer->getData()->saveSnapshot_tga  = !rootWindow->curViewer->getData()->saveSnapshot_tga;
}

void BaseWindow::TrackCharacterCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	auto& renderScene = rootWindow->mSession->renderScene;
	
	if (renderScene.hasCameraTrack())
	{
		renderScene.removeCameraTrack();
		return;
	}

	// track the selected character
	SbmCharacter* character = rootWindow->getSelectedCharacter();
	if (!character)
		return;

	if (!character->getSkeleton())
		return;

	auto& joint = character->getSkeleton()->joints()[0];

// 	std::string trackCommand = "camera track ";
// 	trackCommand.append(character->getName());
// 	trackCommand.append(" ");
// 	trackCommand.append(joint->name());
	//mSession->scene.command((char*)trackCommand.c_str());	
	renderScene.setCameraTrack(character->getName(), joint->jointName());
}

void BaseWindow::KinematicFootstepsCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdShowKinematicFootprints, nullptr);
}

void BaseWindow::TrajectoryCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdShowTrajectory, nullptr);	
}

void BaseWindow::GestureCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdShowGesture, nullptr);	
}

void BaseWindow::JointLabelCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdShowJoints, nullptr);	
}

void BaseWindow::SteeringCharactersCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdSteerCharactersGoalsOnly, nullptr);
}

void BaseWindow::SteeringAllCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdSteerAll, nullptr);
}

void BaseWindow::SteeringNoneCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdNoSteer, nullptr);	
}

void BaseWindow::ShowCollisionCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdCollisionShow, nullptr);	
}

void BaseWindow::HideCollisionCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdCollisionHide, nullptr);
}

void BaseWindow::LocomotionFootstepsCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdShowLocomotionFootprints, nullptr);
}

void BaseWindow::VelocityCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->menu_cmd(FltkViewer::CmdShowVelocity, nullptr);
}

void BaseWindow::GridCB(Fl_Widget* w, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	SmartBody::SBScene& scene = rootWindow->mSession->scene;
	bool value = scene.getBoolAttribute("GUI.ShowGrid");
	scene.setBoolAttribute("GUI.ShowGrid", !value);
}

void BaseWindow::ShowPoseExamples( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	if (rootWindow->curViewer->getData()->reachRenderMode != FltkViewer::ModeShowExamples)
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdReachShowExamples, nullptr);
	else
		rootWindow->curViewer->menu_cmd(FltkViewer::CmdReachNoExamples, nullptr);
}

void BaseWindow::CreatePythonAPICB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;

	std::string mediaPath = SmartBody::SBScene::getSystemParameter("mediapath");
	
	std::string docFile = rootWindow->chooseFile("Save documentation to:", "HTML files\t*.{html,htm}\n", mediaPath);
	if (docFile == "")
		return;

#if (BOOST_VERSION > 104400)
	boost::filesystem::path file = boost::filesystem::absolute( docFile );
#else
	boost::filesystem::path file = boost::filesystem::complete( docFile );
#endif
	
	std::string outFile = file.string();
	std::string cleanedFile = SmartBody::util::replace(outFile, "\\", "/");

	std::stringstream strstr;
	strstr << "from pydoc import *\n";
	strstr << "d = HTMLDoc()\n";
	strstr << "content = d.docmodule(sys.modules[\"SmartBody\"])\n";
	strstr << "import io\n";
	strstr << "f = io.open('" << cleanedFile << "', 'w')\n";
	strstr << "f.write(unicode(content))\n";
	strstr << "f.close()\n";

	scene.run(strstr.str());
}

void BaseWindow::SwitchRendererCB(Fl_Widget* widget, void* data)
{
	BaseWindow* window = static_cast<BaseWindow*>(data);

	// find the .smartbodysettings file
	std::string settingsPathStr = SmartBody::SBScene::getSystemParameter(".smartbodysettings");
	boost::filesystem::path settingsPath(settingsPathStr);
	if (boost::filesystem::exists(settingsPath))
	{
		std::ifstream fin;
		fin.open(settingsPath.string());
		if (!fin.good())
		{
			fl_alert("Could not open .smartbodysettings file %s.", settingsPath.string().c_str());
			return;
		}

		// read each line of the file
		std::vector<std::string> lines;
		while (!fin.eof())
		{
			// read an entire line into memory
			char buf[4096];
			fin.getline(buf, 4096);

			std::string curLine = buf;
			if (curLine.substr(0, 9) == "renderer=")
			{
				std::string curRenderer = curLine.substr(9);
				if (curRenderer == "custom")
				{
					lines.emplace_back("renderer=ogre");
				}
				else
				{
					lines.emplace_back("renderer=custom");
				}
			}
			else
			{
				lines.emplace_back(curLine);
			}
		}
		fin.close();

		std::ofstream fout;
		fout.open(settingsPathStr.c_str());
		if (fout.good())
		{
			for (size_t l = 0; l < lines.size(); l++)
			{
				fout << lines[l] << std::endl;
			}
		}
		fout.close();
	}
	fl_alert("Please restart sbgui to activate the new renderer.");
}

void BaseWindow::DocumentationCB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	auto& renderScene = rootWindow->mSession->renderScene;

	std::stringstream strstr;
	strstr << "import webbrowser\n";
	strstr << "url = \"http://smartbody.ict.usc.edu/documentation\"\n";
	strstr << "webbrowser.open(url)\n";

	scene.run(strstr.str());
}

void BaseWindow::HelpCB(Fl_Widget* widget, void* data)
{
	auto* rootWindow = static_cast<BaseWindow*>(data);

	std::string version = SmartBody::getVersion();

	fl_alert("%s", version.c_str());
}

void BaseWindow::ResizeWindowCB(Fl_Widget* widget, void* data)
{

	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto* resChoice = static_cast<Fl_Choice*>(widget);

	int origX = rootWindow->curViewer->x();
	int origY = rootWindow->curViewer->y();

	auto windowIndex = (size_t) data;	
	
	std::vector<std::string> tokens;	
	const Fl_Menu_Item* menuItem = ((Fl_Menu_*)widget)->mvalue();
	std::string resStr = menuItem->label();
	if (resStr == "Default")
	{
		rootWindow->curViewer->resize(origX,origY,800,600);
		return;
	}
	else if (resStr == "Custom...")
	{
		if (!rootWindow->resWindow)
		{
			rootWindow->resWindow = new ResolutionWindow(rootWindow->x() + 20, rootWindow->y() + 20, 480, 150, strdup("Change Resolution"));
			rootWindow->resWindow->baseWin = rootWindow;
		}
		rootWindow->resWindow->show();
		return;
	}

	SmartBody::util::tokenize(resStr, tokens, "x");
	if (tokens.size() < 2)
		return;

	int width = atoi(tokens[0].c_str());
	int height = atoi(tokens[1].c_str());

	// resize the main window

	int windowX = rootWindow->x();
	int windowY = rootWindow->y();
	int leftX = rootWindow->_leftGroup->x();
	int leftY = rootWindow->_leftGroup->y();
	int leftW = rootWindow->_leftGroup->w();
	int leftH = rootWindow->_leftGroup->h();

	//std::cout << width << " " << height << std::endl;
	rootWindow->curViewer->resize(rootWindow->curViewer->x(), rootWindow->curViewer->y(), width, height);
	rootWindow->curViewer->damage(1);
	rootWindow->resize(rootWindow->x(), rootWindow->y(), width + leftW + 20, height + leftH + 20);
}

void BaseWindow::SaveCameraCB( Fl_Widget* widget, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	auto& renderScene = rootWindow->mSession->renderScene;
	
	std::string cameraName = "camera";
	cameraName += std::to_string(rootWindow->cameraCount++);
	const char* userCamName = fl_input("Camera name:", cameraName.c_str());
	if (!userCamName)
	{
		rootWindow->cameraCount--;
		return;
	}

	std::string cameraNameStr = userCamName;
	SrCamera* camera = renderScene.createCamera(cameraNameStr);
	if (!camera)
	{
		fl_alert("Camera with name '%s' cannot be created.", cameraNameStr.c_str());
		return;
	}

	camera->copyCamera(renderScene.getActiveCamera());

	rootWindow->updateCameraList();
	//window->cameraList.emplace_back(camera);
	//window->updateCameraList();
}

void BaseWindow::DeleteCameraCB( Fl_Widget* widget, void* data )
{
	const Fl_Menu_Item* menuItem = ((Fl_Menu_*)widget)->mvalue();
	std::string camName = menuItem->label();
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& renderScene = rootWindow->mSession->renderScene;

	SrCamera* camera = renderScene.getCamera(camName);
	if (!camera)
	{
		fl_alert("No camera named '%s' found, cannot remove it.", camName.c_str());
		return;
	}
	renderScene.removeCamera(camera);
	rootWindow->updateCameraList();
}


void BaseWindow::DeleteObjectCB( Fl_Widget* widget, void* data )
{
	const Fl_Menu_Item* menuItem = ((Fl_Menu_*)widget)->mvalue();
	std::string objName = menuItem->label();
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;

	SmartBody::SBPawn* pawn = scene.getPawn(objName);
	SmartBody::SBCharacter* sbChar = scene.getCharacter(objName);
	if (!pawn && !sbChar)
	{
		fl_alert("No object named '%s' found, cannot remove it.", objName.c_str());
		return;
	}

	int confirm = fl_choice(SmartBody::util::format("Are you sure you want to delete '%s'?", objName.c_str()).c_str(), "No", "Yes", nullptr);
	if (confirm == 0)
		return ;
	if (sbChar)
		scene.removeCharacter(objName);
	else if (pawn)
		scene.removePawn(objName);
	//mSession->scene.removeCamera(camera);
	rootWindow->updateObjectList();
}

void BaseWindow::ChooseCameraCB( Fl_Widget* widget, void* data )
{
	const Fl_Menu_Item* menuItem = ((Fl_Menu_*)widget)->mvalue();
	//LOG("load camera %s", menuItem->label());
	std::string camName = menuItem->label();
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& renderScene = rootWindow->mSession->renderScene;

	SrCamera* cam = renderScene.getCamera(camName);
	if (!cam)
	{
		fl_alert("No camera with name '%s' found.", camName.c_str());
		return;
	}

	renderScene.setActiveCamera(cam);
// 	Fl_Choice* choice = (Fl_Choice*)widget;
// 	int cameraIdx = choice->value() - 1;	
// 	if (cameraIdx >=0 && cameraIdx < (int)window->cameraList.size())
// 	{
// 		SrCamera* cam = window->cameraList[cameraIdx];		
// 		//window->set_camera(cam);
// 		window->get_camera()->copyCamera(cam);
// 	}
}


void BaseWindow::updateObjectList(std::string deleteObjectName)
{
	auto* menuList = const_cast<Fl_Menu_Item*>(menubar->menu());
	Fl_Menu_Item& deleteobjSubMenu = menuList[deleteObjectMenuIndex];	
	deleteObjectList.clear();
	std::vector<std::string> characterNames = mSession->scene.getCharacterNames();
	for (unsigned int i=0;i<characterNames.size();i++)
	{		
		const std::string charName = characterNames[i];
		if (charName == deleteObjectName)
			continue;
		int flag = (i==characterNames.size()-1) ? FL_MENU_DIVIDER : 0;
		Fl_Menu_Item temp_DeleteObj = { strdup(charName.c_str()), 0, DeleteObjectCB, this, flag };		
		deleteObjectList.emplace_back(temp_DeleteObj);
	}

	std::vector<std::string> pawnNames = mSession->scene.getPawnNames();
	for (const auto& pawnName : pawnNames)
	{		
			SmartBody::SBCharacter* sbChar = mSession->scene.getCharacter(pawnName);
		if (sbChar)
			continue;

		if (pawnName == deleteObjectName)
			continue;
		Fl_Menu_Item temp_DeleteObj = { strdup(pawnName.c_str()), 0, DeleteObjectCB, this };		
		deleteObjectList.emplace_back(temp_DeleteObj);
	}

	Fl_Menu_Item temp = {nullptr};
	deleteObjectList.emplace_back(temp);

	deleteobjSubMenu.user_data(&deleteObjectList[0]);
}

void BaseWindow::updateCameraList()
{
	/*
	cameraChoice->clear();
	cameraChoice->add("-----");
	for (unsigned int i=0;i<cameraList.size();i++)
	{
		std::string cameraName = "cam";
		cameraName += boost::lexical_cast<std::string>(i);
		cameraChoice->add(cameraName.c_str());
	}
	*/
	auto* menuList = const_cast<Fl_Menu_Item*>(menubar->menu());
	Fl_Menu_Item& loadCameraSubMenu = menuList[loadCameraMenuIndex];	
	Fl_Menu_Item& deleteCameraSubMenu = menuList[deleteCameraMenuIndex];	
	loadCameraList.clear();
	deleteCameraList.clear();
	if (Session::current) {
		std::vector<std::string> cameraNames = Session::current->renderScene.getCameraNames();
		for (auto& cameraName : cameraNames) {
			const std::string& camName = cameraName;
			Fl_Menu_Item temp_LoadCam = {strdup(camName.c_str()), 0, ChooseCameraCB, this};
			loadCameraList.emplace_back(temp_LoadCam);

			Fl_Menu_Item temp_DeleteCam = {strdup(camName.c_str()), 0, DeleteCameraCB, this};
			deleteCameraList.emplace_back(temp_DeleteCam);
		}
	}
	Fl_Menu_Item temp = {nullptr};
	loadCameraList.emplace_back(temp);
	deleteCameraList.emplace_back(temp);

	loadCameraSubMenu.user_data(&loadCameraList[0]);
	deleteCameraSubMenu.user_data(&deleteCameraList[0]);
}

void BaseWindow::ShowCamerasCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	bool value = scene.getBoolAttribute("GUI.ShowCameras");
	scene.setBoolAttribute("GUI.ShowCameras", !value);

}

void BaseWindow::ShowLightsCB( Fl_Widget* w, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	auto& scene = rootWindow->mSession->scene;
	bool value = scene.getBoolAttribute("GUI.ShowLights");
	scene.setBoolAttribute("GUI.ShowLights", !value);
}

void BaseWindow::ShowSelectedCharacterCB( Fl_Widget* w, void* data )
{
	static bool showSelected = true;
	showSelected = !showSelected;
	ObjectManipulationHandle::renderSelectedBoundingBox = showSelected;
}

void BaseWindow::DeleteSelectionCB( Fl_Widget* widget, void* data )
{
	auto* rootWindow = static_cast<BaseWindow*>(data);
	rootWindow->curViewer->deleteSelectedObject(0);
}

