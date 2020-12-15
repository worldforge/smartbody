#ifndef _POSECREATOR_H_
#define _POSECREATOR_H_

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Value_Slider.H>
#include "GenericViewer.h"
#include <SBWindowListener.h>

class PoseCreator : public GenericViewer, public Fl_Double_Window, public SBWindowListener
{
	public:
		PoseCreator(int x, int y, int w, int h, const char* name);
		~PoseCreator();

		virtual void show();
		virtual void hide();

		virtual void show_viewer();
		virtual void hide_viewer();

		virtual void updateGUI();

		//virtual void OnSimulationUpdate();

		static void OnChoiceCharacterList(Fl_Widget* widget, void* data);
	
		Fl_Group* topGroup;
		Fl_Scroll* bottomGroup;
		std::vector<Fl_Value_Slider*> _sliders;
		std::vector<Fl_Value_Slider*> _weights;

		Fl_Choice* _choiceCharacterList;
		std::string _selectedCharacter;
};

class PoseCreatorFactory : public GenericViewerFactory
{
	public:
		PoseCreatorFactory();

		virtual GenericViewer* create(int x, int y, int w, int h);
		virtual void destroy(GenericViewer* viewer);
};


#endif
