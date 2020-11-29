#ifndef _AUTORIGVIEWER_
#define _AUTORIGVIEWER_

#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Input_Choice.H>
#include <FL/Fl_Button.H>
#include <sr/sr_camera.h>
#include <sk/sk_scene.h>
#include <sr/sr_sa_gl_render.h>
#include <sr/sr_light.h>
#include <sb/SBSkeleton.h>
#include <sb/SBMotion.h>
#include <string>
#include <jointmapviewer/JointMapViewer.h>
#include "autorig/SBAutoRigManager.h"

class RetargetStepWindow;

class ModelViewer : public MouseViewer
{
public:
	ModelViewer(int x, int y, int w, int h, char* name);
	~ModelViewer() override;
	void setModel(SrModel& model);
	void focusOnModel();
	void draw() override;

protected:
	void updateFancyLights();
	SrSnModel* _model;
};

class SkinViewer : public MouseViewer
{
public:
	SkinViewer(int x, int y, int w, int h, char* name);
	~SkinViewer() override;
	void setSkeleton(SmartBody::SBSkeleton* sk);
	void setDeformableMesh(DeformableMesh* mesh);
	void draw() override;
	void focusOnSkeleton();
protected:
	void drawSkinWeight();
	SkScene* skeletonScene;
	SrSaGlRender renderFunction;
	SmartBody::SBSkeleton* skeleton;
	DeformableMesh* mesh;
};

class AutoRigViewer : public Fl_Double_Window, AutoRigCallBack
{
	public:
		AutoRigViewer(int x, int y, int w, int h, char* name);
		
		~AutoRigViewer() override;
		
		void draw() override;
		void applyAutoRig(int riggingType = 0);
		static void ApplyAutoRigCB(Fl_Widget* widget, void* data);
		void updateAutoRigViewer();
		void setCharacterName(const std::string& name);

		void setRetargetStepWindow(RetargetStepWindow* val) { retargetStepWindow = val; }

		void voxelComplete(SrModel& voxelModel) override;
		void skeletonComplete(SmartBody::SBSkeleton* sk) override;
		void skinComplete(DeformableMesh* defMesh) override;

		std::string _characterName;
		std::string _deletePawnName;
		Fl_Choice* _choiceVoxelRigging;
		Fl_Button* _buttonAutoRig;	

		ModelViewer* modelViewer;
		SkinViewer*  skinViewer;
	protected:
		RetargetStepWindow* retargetStepWindow;		
};
#endif
