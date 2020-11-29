
#include "FaceViewer.h"
#include <FL/Fl_Value_Slider.H>
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sbm/action_unit.hpp>
#include <sbm/lin_win.h>
#include <sb/SBVHMsgManager.h>
#include <sb/SBFaceDefinition.h>
#include "SBUtilities.h"

FaceViewer::FaceViewer(int x, int y, int w, int h, char* name) : GenericViewer(x, y, w, h), Fl_Double_Window(x, y, w, h, name), SBWindowListener()
{
	begin();

	topGroup = new Fl_Group(0, 0, w, 40);
	topGroup->begin();	
		choiceCharacters = new Fl_Choice(100, 10, 100, 25, "Characters");
		choiceCharacters->callback(CharacterCB, this);
		buttonRefresh = new Fl_Button(220, 10, 60, 25, "Refresh");
		buttonRefresh->callback(RefreshCB, this);
		buttonReset = new Fl_Button(300, 10, 60, 25, "Reset");
		buttonReset->callback(ResetCB, this);
		buttonDefaultFace = new Fl_Button(380, 10, 100, 25, "Set Default Face");
		buttonDefaultFace->callback(DefaultFaceCB, this);
		buttonResetDefaultFace = new Fl_Button(500, 10, 100, 25, "Reset Default Face");
		buttonResetDefaultFace->callback(ResetDefaultFaceCB, this);

	topGroup->end();

	bottomGroup = new Fl_Scroll(0, 45, w, h - 40);
	bottomGroup->begin();

	bottomGroup->box(FL_DOWN_BOX);
		
	bottomGroup->end();	


	end();
	this->resizable(bottomGroup);

	FaceViewer::RefreshCB(this, this);
	lastCharacterName = "";

	if (choiceCharacters->size() > 0)
	{
		choiceCharacters->value(0);
		FaceViewer::CharacterCB(this, this);
	}

}

FaceViewer::~FaceViewer()
{
}

void FaceViewer::CharacterCB(Fl_Widget* widget, void* data)
{
	FaceViewer* faceViewer = (FaceViewer*) data;

	const Fl_Menu_Item* menu = faceViewer->choiceCharacters->menu();
	std::string curCharacterName = menu[faceViewer->choiceCharacters->value()].label();

	bool showAUs = true;
	bool showVisemes = true;
	std::map<std::string, bool> showMorphs;

	if (curCharacterName == faceViewer->lastCharacterName)
	{
		showAUs = faceViewer->checkShowAUs->value();
		showVisemes = faceViewer->checkShowVisemes->value();

		for (unsigned int i = 0; i < faceViewer->checkShowMorphs.size(); i++)
		{
			showMorphs.insert(std::pair<std::string, bool>(faceViewer->checkShowMorphs[i]->label(), faceViewer->checkShowMorphs[i]->value()));
		}
	}

	faceViewer->bottomGroup->clear();
	faceViewer->_sliders.clear();
	faceViewer->_weights.clear();
	faceViewer->checkShowMorphs.clear();

	int curY = faceViewer->bottomGroup->y() + 25;

	faceViewer->checkShowAUs = new Fl_Check_Button(10, curY, 100, 25, "Show AUs");
	faceViewer->checkShowAUs->callback(ShowAUsCB, faceViewer);
	faceViewer->checkShowAUs->value(showAUs);
	faceViewer->bottomGroup->add(faceViewer->checkShowAUs);

	curY += 25;

	faceViewer->checkShowVisemes = new Fl_Check_Button(10, curY, 100, 25, "Show Visemes");
	faceViewer->checkShowVisemes->callback(ShowVisemesCB, faceViewer);
	faceViewer->checkShowVisemes->value(showVisemes);
	faceViewer->bottomGroup->add(faceViewer->checkShowVisemes);

	curY += 25;

	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(curCharacterName);
	if (!character)
	{
		faceViewer->lastCharacterName = "";
		return;
	}


	// blend shapes/morph targets
	if (character->getSkeleton())
	{
		std::set<std::string> morphControllers;
		std::vector<std::string> attrNames = character->getAttributeNames();
		for (unsigned int a = 0; a < attrNames.size(); a++)
		{
			if (attrNames[a].find("blendShape.controller") != std::string::npos)
				morphControllers.insert(character->getStringAttribute(attrNames[a]));
		}
		if (morphControllers.size() == 0)
		{
			Fl_Check_Button* checkMorph = new Fl_Check_Button(10, curY, 100, 25, "Show Morphs");
			checkMorph->callback(ShowMorphsCB, faceViewer);
			checkMorph->value(true);
			faceViewer->checkShowMorphs.emplace_back(checkMorph);
			faceViewer->bottomGroup->add(checkMorph);
		}
		else
		{
			int counter = 0;
			for (std::set<std::string>::iterator iterator = morphControllers.begin();
				iterator != morphControllers.end();
				iterator++)
			{
				Fl_Check_Button* checkMorph = new Fl_Check_Button(10, curY, 100, 25, _strdup((*iterator).c_str()));
				checkMorph->callback(ShowMorphsCB, faceViewer);
				bool found = false;
				for (std::map<std::string, bool>::iterator iterator2 = showMorphs.begin();
					iterator2 != showMorphs.end();
					iterator2++)
				{
					if ((*iterator2).first == (*iterator))
					{
						checkMorph->value((*iterator2).second);
						found = true;
						break;
					}
				}
				if (!found)
					checkMorph->value(true);

				faceViewer->checkShowMorphs.emplace_back(checkMorph);
				faceViewer->bottomGroup->add(checkMorph);
				counter++;

				curY += 25;
			}
		}
	}
	curY += 25;

	faceViewer->lastCharacterName = curCharacterName;

	SmartBody::SBFaceDefinition* faceDefinition = character->getFaceDefinition();
	if (faceDefinition && showAUs)
	{
		const std::vector<int>& auNums = faceDefinition->getAUNumbers();
		for (size_t a = 0; a < auNums.size(); a++)
		{
			ActionUnit* au = faceDefinition->getAU(auNums[a]);
			if (au->is_left())
			{
				Fl_Value_Slider* slider = new Fl_Value_Slider(200, curY, 150, 25, _strdup(au->getLeftName().c_str()));
				slider->type(FL_HORIZONTAL);
				slider->align(FL_ALIGN_LEFT);
				slider->range(0.0, 1.0);
				slider->callback(FaceCB, faceViewer);
				faceViewer->bottomGroup->add(slider);
				faceViewer->_sliders.emplace_back(slider);
				faceViewer->_weights.emplace_back(nullptr);
				curY += 30;
			}

			if (au->is_right())
			{
				Fl_Value_Slider* slider = new Fl_Value_Slider(200, curY, 150, 25, _strdup(au->getRightName().c_str()));
				slider->type(FL_HORIZONTAL);
				slider->align(FL_ALIGN_LEFT);
				slider->range(0.0, 1.0);
				slider->callback(FaceCB, faceViewer);
				faceViewer->bottomGroup->add(slider);
				faceViewer->_sliders.emplace_back(slider);
				faceViewer->_weights.emplace_back(nullptr);
				curY += 30;
			}

			if (au->is_bilateral())
			{
				Fl_Value_Slider* slider = new Fl_Value_Slider(200, curY, 150, 25, _strdup(au->getBilateralName().c_str()));
				slider->type(FL_HORIZONTAL);
				slider->align(FL_ALIGN_LEFT);
				slider->range(0.0, 1.0);
				slider->callback(FaceCB, faceViewer);
				faceViewer->bottomGroup->add(slider);
				faceViewer->_sliders.emplace_back(slider);
				faceViewer->_weights.emplace_back(nullptr);
				curY += 30;
			}


		}
	}

	if (faceDefinition && showVisemes)
	{
		const std::vector<std::string>& visemeNames = faceDefinition->getVisemeNames();
		for (size_t v = 0; v < visemeNames.size(); v++)
		{
			Fl_Value_Slider* slider = new Fl_Value_Slider(200, curY, 150, 25, _strdup(visemeNames[v].c_str()));
			slider->type(FL_HORIZONTAL);
			slider->align(FL_ALIGN_LEFT);
			slider->range(0.0, 1.0);
			slider->callback(FaceCB, faceViewer);
			faceViewer->bottomGroup->add(slider);
			faceViewer->_sliders.emplace_back(slider);
			double maxWeight = 1.0;
			SmartBody::SBAttribute* maxWeightAttribute = character->getAttribute("maxVisemeWeight");
			if (!maxWeightAttribute)
				maxWeight = 1.0;
			else
				maxWeight = character->getDoubleAttribute("maxVisemeWeight");
			std::string weightLabel = visemeNames[v] + " weight";
			Fl_Value_Slider* weightSlider = new Fl_Value_Slider(430, curY, 100, 25, _strdup(weightLabel.c_str()));
			weightSlider->type(FL_HORIZONTAL);
			weightSlider->align(FL_ALIGN_LEFT);
			weightSlider->range(0.0, maxWeight);
			weightSlider->callback(FaceWeightCB, faceViewer);
			faceViewer->bottomGroup->add(weightSlider);
			// set the initial weight
			float initialWeight = faceDefinition->getVisemeWeight(visemeNames[v]);
			weightSlider->value(initialWeight);
			faceViewer->_weights.emplace_back(weightSlider);

			curY += 25;
		}
	}

	// blend shapes/morph targets
	if (character->getSkeleton())
	{
		for (int jointCounter = 0; jointCounter < character->getSkeleton()->getNumJoints(); ++jointCounter)
		{
			if (character->getSkeleton()->getJoint(jointCounter)->getJointType() == SkJoint::TypeBlendShape)
			{
				std::string morphName = character->getSkeleton()->getJoint(jointCounter)->getName();
				std::stringstream strstr;
				strstr << "blendShape.controller." << morphName;
				std::string controllerName = character->getStringAttribute(strstr.str());
				bool showThisMorph = true;
				if (controllerName != "")
				{
					std::map<std::string, bool>::iterator iter = showMorphs.find(controllerName);
					if (iter != showMorphs.end())
						showThisMorph = (*iter).second;
				}

				if (showThisMorph)
				{
					Fl_Value_Slider* slider = new Fl_Value_Slider(200, curY, 150, 25, _strdup(character->getSkeleton()->getJoint(jointCounter)->getName().c_str()));
					slider->type(FL_HORIZONTAL);
					slider->align(FL_ALIGN_LEFT);
					slider->range(0.0, 1.0);
					slider->callback(FaceCB, faceViewer);
					faceViewer->bottomGroup->add(slider);
					faceViewer->_sliders.emplace_back(slider);
					faceViewer->_weights.emplace_back(nullptr);
					curY += 30;
				}
			}
		}
	}

	faceViewer->updateGUI();
	faceViewer->bottomGroup->damage(FL_DAMAGE_ALL);

	faceViewer->redraw();

}

void FaceViewer::ShowAUsCB(Fl_Widget* widget, void* data)
{
	RefreshCB(widget, data);
}

void FaceViewer::ShowVisemesCB(Fl_Widget* widget, void* data)
{
	RefreshCB(widget, data);
}

void FaceViewer::ShowMorphsCB(Fl_Widget* widget, void* data)
{
	RefreshCB(widget, data);
}


void FaceViewer::RefreshCB(Fl_Widget* widget, void* data)
{
	FaceViewer* faceViewer = (FaceViewer*) data;
	faceViewer->choiceCharacters->clear();
	const std::vector<std::string>& charNames = SmartBody::SBScene::getScene()->getCharacterNames();
	for (size_t i = 0; i < charNames.size(); i++)
	{
		faceViewer->choiceCharacters->add(charNames[i].c_str());
	}

	if (charNames.size() > 0)
	{
		faceViewer->choiceCharacters->activate();
		faceViewer->choiceCharacters->value(0);
		FaceViewer::CharacterCB(widget, data);
	}
	else
	{
		faceViewer->choiceCharacters->deactivate();
	}
	faceViewer->redraw();
}

void FaceViewer::ResetCB(Fl_Widget* widget, void* data)
{
	FaceViewer* faceViewer = (FaceViewer*) data;

	const Fl_Menu_Item* menu = faceViewer->choiceCharacters->menu();
	SbmCharacter* character = SmartBody::SBScene::getScene()->getCharacter(menu[faceViewer->choiceCharacters->value()].label());
	if (character)
	{
		int numSliders = faceViewer->bottomGroup->children();
		for (int c = 0; c < numSliders; c++)
		{
			Fl_Value_Slider* slider = dynamic_cast<Fl_Value_Slider*>(faceViewer->bottomGroup->child(c));
			if (slider)
			{
				std::string name = slider->label();
				if (name.find(" weight") == std::string::npos)
					slider->value(0);

				//std::string message = SmartBody::util::format("char %s viseme %s %f", faceViewer->choiceCharacters->menu()[faceViewer->choiceCharacters->value()].label(), name.c_str(), slider->value());
				std::string message = SmartBody::util::format("char %s viseme clear", faceViewer->choiceCharacters->menu()[faceViewer->choiceCharacters->value()].label());
				if (!SmartBody::SBScene::getScene()->isRemoteMode())
				{
					SmartBody::SBScene::getScene()->command(message);
				}
				else
				{
					SmartBody::SBScene::getScene()->getVHMsgManager()->send2("sbm", message.c_str());
				}
			}

		}

	}
}

void FaceViewer::DefaultFaceCB(Fl_Widget* widget, void* data)
{
	FaceViewer* faceViewer = (FaceViewer*) data;

	std::vector<std::string> defaultFacePoses;
	std::vector<float> defaultFaceValues;

	const Fl_Menu_Item* menu = faceViewer->choiceCharacters->menu();
	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(menu[faceViewer->choiceCharacters->value()].label());
	if (character)
	{
		int numSliders = faceViewer->bottomGroup->children();
		for (int c = 0; c < numSliders; c++)
		{
			Fl_Value_Slider* slider = dynamic_cast<Fl_Value_Slider*>(faceViewer->bottomGroup->child(c));
			if (slider)
			{
				std::string name = slider->label();
				defaultFacePoses.emplace_back(name);
				defaultFaceValues.emplace_back((float)slider->value());

			}

		}

		SmartBody::SBFaceDefinition* faceDefinition = character->getFaceDefinition();
		if (faceDefinition)
		{
			if (!SmartBody::SBScene::getScene()->isRemoteMode())
			{
				faceDefinition->setDefaultFacePose(defaultFacePoses, defaultFaceValues);
				character->updateDefaultFacePose();
			}
			else
			{
				SmartBody::util::log("Default face poses cannot be set in remote mode.");
			}
		}

	}
}


void FaceViewer::ResetDefaultFaceCB(Fl_Widget* widget, void* data)
{
	FaceViewer* faceViewer = (FaceViewer*) data;

	const Fl_Menu_Item* menu = faceViewer->choiceCharacters->menu();
	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(menu[faceViewer->choiceCharacters->value()].label());
	if (character)
	{
		SmartBody::SBFaceDefinition* faceDefinition = character->getFaceDefinition();
		if (faceDefinition)
		{
			if (!SmartBody::SBScene::getScene()->isRemoteMode())
			{
				std::vector<std::string> defaultFacePoses;
				std::vector<float> defaultFaceValues;
				faceDefinition->setDefaultFacePose(defaultFacePoses, defaultFaceValues);
				character->updateDefaultFacePose();
			}
			else
			{
				SmartBody::util::log("Reset default face poses cannot be set in remote mode.");
			}
			
		}
	}
}

void FaceViewer::FaceCB(Fl_Widget* widget, void* data)
{
	FaceViewer* faceViewer = (FaceViewer*) data;

	Fl_Value_Slider* slider = dynamic_cast<Fl_Value_Slider*>(widget);
	if (!slider)
		return;

	std::string name = slider->label();
#if 0
	std::string message = SmartBody::util::format("char %s viseme %s %f", faceViewer->choiceCharacters->menu()[faceViewer->choiceCharacters->value()].label(), name.c_str(), slider->value());
	if (!SmartBody::SBScene::getScene()->isRemoteMode())
	{
		SmartBody::SBScene::getScene()->command(message);
	}
	else
	{
		std::string sendStr = "send sbm " + message;
		SmartBody::SBScene::getScene()->command(sendStr);
	}
#else
	std::vector<std::string> charNames = SmartBody::SBScene::getScene()->getCharacterNames();
	for (unsigned int i=0;i<charNames.size();i++)
	{
		std::string message = SmartBody::util::format("char %s viseme %s %f", charNames[i].c_str(), name.c_str(), slider->value());
		if (!SmartBody::SBScene::getScene()->isRemoteMode())
		{
			SmartBody::SBScene::getScene()->command(message);
		}
		else
		{
			std::string sendStr = "send sbm " + message;
			SmartBody::SBScene::getScene()->command(sendStr);
		}
	}
#endif
}

void FaceViewer::FaceWeightCB(Fl_Widget* widget, void* data)
{
	FaceViewer* faceViewer = (FaceViewer*) data;

	Fl_Value_Slider* weightSlider = dynamic_cast<Fl_Value_Slider*>(widget);
	if (!weightSlider)
		return;

	// get the name of the viseme
	std::string labelName = (const char*) weightSlider->label(); 
	std::string visemeName = "";
	int pos = labelName.find_last_of(" ");
	if (pos != std::string::npos)
	{
		visemeName = labelName.substr(0, pos);
	}

#if 1
	std::string message = SmartBody::util::format("char %s visemeweight %s %f", faceViewer->choiceCharacters->menu()[faceViewer->choiceCharacters->value()].label(), visemeName.c_str(), weightSlider->value());
	if (!SmartBody::SBScene::getScene()->isRemoteMode())
	{
		SmartBody::SBScene::getScene()->command(message);
	}
	else
	{
		std::string sendStr = "send sbm " + message;
		SmartBody::SBScene::getScene()->command(sendStr);
	}
#else
	std::vector<std::string> charNames = SmartBody::SBScene::getScene()->getCharacterNames();
	for (unsigned int i=0;i<charNames.size();i++)
	{
		std::string message = SmartBody::util::format("char %s visemeweight %s %f", charNames[i].c_str(), visemeName.c_str(), weightSlider->value());
		if (!SmartBody::SBScene::getScene()->isRemoteMode())
		{
			SmartBody::SBScene::getScene()->command(message);
		}
		else
		{
			std::string sendStr = "send sbm " + message;
			SmartBody::SBScene::getScene()->command(sendStr);
		}
	}
#endif
}

void FaceViewer::updateGUI()
{
	if (!choiceCharacters->menu())
		return;

	std::string selectedCharacter = "";
	if (choiceCharacters->menu()->size() > 0)
	{
		selectedCharacter = choiceCharacters->menu()[choiceCharacters->value()].label();
	}

	if (selectedCharacter == "")
		return;

	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(selectedCharacter);
	if (character)
	{	
		SmartBody::SBSkeleton* skeleton = character->getSkeleton();
		SmartBody::SBFaceDefinition* faceDefinition = character->getFaceDefinition();

		for (size_t s = 0; s < _sliders.size(); s++)
		{
			Fl_Value_Slider* slider = _sliders[s];
			SmartBody::SBJoint* joint = skeleton->getJointByName(slider->label());
			if (joint)
			{
				SrVec position = joint->getPosition();
				if (slider->value() != position.x)
					slider->value(position.x);
			}

			if (_weights[s])
			{
				float weight = faceDefinition->getVisemeWeight(slider->label());
				_weights[s]->value(weight);
			}
		}
	}

}


void FaceViewer::show_viewer()
{
	show();
}

void FaceViewer::hide_viewer()
{
	hide();
}

void FaceViewer::show()
{
	SBWindowListener::windowShow();
	Fl_Double_Window::show();
}
void FaceViewer::hide()
{
	SBWindowListener::windowHide();
	Fl_Double_Window::hide();
}

void FaceViewer::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{
	std::string selectedCharacter = "";
	if (!choiceCharacters->menu())
		return;
	if (!choiceCharacters)
		return;
	if (choiceCharacters->menu()->size() > 0)
	{
		selectedCharacter = choiceCharacters->menu()[choiceCharacters->value()].label();
	}
	RefreshCB(buttonRefresh, this);

	if (selectedCharacter != "")
	{
		for (int c = 0; c < choiceCharacters->menu()->size(); c++)
		{
			if (choiceCharacters->menu()[c].label() == nullptr)
				continue;

			if (selectedCharacter == choiceCharacters->menu()[c].label())
			{
				choiceCharacters->value(c);
				CharacterCB(choiceCharacters, this);
			}
		}
	}
	else
	{
		choiceCharacters->value(0);
	}

}

void FaceViewer::OnCharacterDelete( const std::string & name )
{
	std::string selectedCharacter = "";
	if (!choiceCharacters->menu())
		return;

	if (choiceCharacters->menu()->size() > 0)
	{
		selectedCharacter = choiceCharacters->menu()[choiceCharacters->value()].label();
	}
	if (selectedCharacter == name)
	{
		choiceCharacters->deactivate();
		FaceViewer::CharacterCB(this, this);	
	}
}


void FaceViewer::OnCharacterUpdate( const std::string & name )
{
	OnCharacterCreate(name, "");
}

void FaceViewer::OnSimulationUpdate()
{
	updateGUI();
}






GenericViewer* FaceViewerFactory::create(int x, int y, int w, int h)
{
	FaceViewer* faceViewer = new FaceViewer(x, y, w, h, (char*)"Face View");
	return faceViewer;
}

void FaceViewerFactory::destroy(GenericViewer* viewer)
{
	delete viewer;
}

FaceViewerFactory::FaceViewerFactory()
{

}
