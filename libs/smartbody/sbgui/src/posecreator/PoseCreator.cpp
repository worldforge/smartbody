
#include "PoseCreator.h"
#include <FL/Fl_Value_Slider.H>
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sbm/action_unit.hpp>
#include <sbm/lin_win.h>
#include <sb/SBVHMsgManager.h>
#include <sb/SBFaceDefinition.h>

PoseCreator::PoseCreator(int x, int y, int w, int h, const char* name) : Fl_Double_Window(x, y, w, h), GenericViewer(x, y, w, h), SBWindowListener()
{
	_selectedCharacter = "";
	this->label("Pose Creator");
	begin();

	topGroup = new Fl_Group(0, 0, w, 40);
	topGroup->begin();	

	this->begin();
		_choiceCharacterList = new Fl_Choice(80, 10, 100, 20, "Characters");
		_choiceCharacterList->callback(OnChoiceCharacterList, this);


	topGroup->end();

	bottomGroup = new Fl_Scroll(0, 45, w, h - 40);
	bottomGroup->begin();	
	bottomGroup->box(FL_DOWN_BOX);
		
	bottomGroup->end();	


	end();
	this->resizable(bottomGroup);

//	FaceShiftViewer::RefreshCB(this, this);
}

PoseCreator::~PoseCreator()
{
}

void PoseCreator::updateGUI()
{
}

void PoseCreator::OnChoiceCharacterList(Fl_Widget* widget, void* data)
{
	PoseCreator* editor = (PoseCreator*) data;

	int selected = editor->_choiceCharacterList->value();
	if (selected >= 0)
	{
		editor->_selectedCharacter = editor->_choiceCharacterList->text(selected);
	}
	
}

void PoseCreator::show_viewer()
{
	show();
}

void PoseCreator::hide_viewer()
{
	hide();
}

void PoseCreator::show()
{
	SBWindowListener::windowShow();
	Fl_Double_Window::show();
}
void PoseCreator::hide()
{
	SBWindowListener::windowHide();
	Fl_Double_Window::hide();
}



GenericViewer* PoseCreatorFactory::create(int x, int y, int w, int h)
{
	PoseCreator* poseCreator = new PoseCreator(x, y, w, h, (char*) "Pose Creator");
	return poseCreator;
}

void PoseCreatorFactory::destroy(GenericViewer* viewer)
{
	delete viewer;
}

PoseCreatorFactory::PoseCreatorFactory()
{

}
