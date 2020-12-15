#include "ExportWindow.h"
#include "sbm/SceneExporter.h"
#include <vector>
#include <string>
#include <sb/SBScene.h>
#include <FL/Fl_File_Chooser.H>
#include <fstream>
#include <sbm/lin_win.h>
#include "RootWindow.h"
#include <boost/filesystem.hpp>
#include <boost/version.hpp>

ExportWindow::ExportWindow(int x, int y, int w, int h, const char* name) : Fl_Double_Window(x, y, w, h, name)
{
	std::vector<std::string> aspects;
	aspects.emplace_back("scene");
	aspects.emplace_back("assets");
	aspects.emplace_back("cameras");
	aspects.emplace_back("lights");
	aspects.emplace_back("face definitions");
	//aspects.emplace_back("joint maps");
	aspects.emplace_back("lip syncing");
	aspects.emplace_back("blends and transitions");
	aspects.emplace_back("retargets");
	aspects.emplace_back("gesture maps");
	aspects.emplace_back("pawns");
	aspects.emplace_back("characters");
	aspects.emplace_back("services");
	aspects.emplace_back("positions");

	int curY = 10;
	int curX = 60;
	checkExportAsset = new Fl_Check_Button(curX, curY, 150, 25, _strdup("Export Assets"));
	checkExportAsset->when(FL_WHEN_CHANGED);
	checkExportAsset->callback(ChangeExportTypeCB,this);
	curY += 30;
	choiceExportType = new Fl_Choice(curX, curY, 150, 25, "Write To ");
	choiceExportType->add("Files");
	choiceExportType->add("Zip Archive");
	choiceExportType->value(0);
	choiceExportType->when(FL_WHEN_CHANGED);
	choiceExportType->callback(ChangeExportTypeCB,this);
	curY += 40;
	for (auto & str : aspects)
	{
			Fl_Check_Button* check = new Fl_Check_Button(curX, curY, 150, 25, _strdup(str.c_str()));
		check->value(1);
		curY += 25;	
		checkExport.emplace_back(check);
	}

	curY += 20;

	inputFile = new Fl_Input(60, curY, 150, 25, "Export File");
	buttonFile = new Fl_Button(220, curY, 25, 25, "...");
	buttonFile->callback(FileCB, this);
	curY += 30;

	Fl_Button* buttonExport = new Fl_Button(curX, curY, 80, 25, "Export");
	buttonExport->callback(ExportAllCB, this);
	exportToFolder = false;

	this->end();
}

ExportWindow::~ExportWindow()
{
}

void ExportWindow::FileCB(Fl_Widget* widget, void* data)
{
	ExportWindow* window = (ExportWindow*) data;
	bool isExportAsset = window->checkExportAsset->value();
	bool isExportZip = window->choiceExportType->value() == 1;

	std::string mediaPath = SmartBody::SBScene::getSystemParameter("mediapath");
	std::string fileName = "";
	if (window->exportToFolder)
		fileName = BaseWindow::chooseDirectory("Save To:", mediaPath);
	else if (isExportAsset && isExportZip)
		fileName = BaseWindow::chooseFile("Save To:", "Zip Archive\t*.zip\n", mediaPath);
	else
		fileName = BaseWindow::chooseFile("Save To:", "Python\t*.py\n", mediaPath);
	window->inputFile->value(fileName.c_str());
}


void ExportWindow::ExportAllCB( Fl_Widget* widget, void* data )
{
#if (BOOST_VERSION > 104400)	
	namespace fs =  boost::filesystem;
	//using boost::filesystem::dot;
	//using boost::filesystem::slash;
#else	
	namespace fs = boost::filesystem2;
	//using boost::filesystem2::dot;
	//using boost::filesystem2::slash;
#endif

	ExportWindow* window = (ExportWindow*) data;

	bool isExportAsset = window->checkExportAsset->value();
	bool isExportZip = window->choiceExportType->value() == 1;

	const char* fname = window->inputFile->value();
	if (!fname)
		return;

	std::vector<std::string> aspects;
	for (auto & iter : window->checkExport)
	{
		if (iter->value())
		{
			aspects.emplace_back(iter->label());
		}
	}
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	if (window->exportToFolder && isExportAsset)
	{
		SmartBody::exportScenePackage(Session::current->renderScene, fname);
	}
	else if (isExportZip && isExportAsset)
	{
		std::string fileBase = fs::basename(fname);
		fs::path fullFile(fname);
		fs::path filePath = fullFile.parent_path();
		SmartBody::exportScenePackage(Session::current->renderScene, filePath.string(), fileBase+".zip");
	}
	else
	{
		std::ofstream file(fname);
		if (!file.good())
		{
			fl_alert("Cannot save to file '%s'. No export done.", window->inputFile->value());
			return;
		}
		window->exportSceneScript(fname,aspects, false);
		fl_alert("Saved to: %s", fname);
	}
	//std::string exportData = SmartBody::SBScene::getScene()->exportScene(aspects, false);
	//file << exportData;
	//file.close();
	window->hide();
}

void ExportWindow::ExportCB(Fl_Widget* widget, void* data)
{
	ExportWindow* window = (ExportWindow*) data;

	const char* fname = window->inputFile->value();
	if (!fname)
		return;
	std::ofstream file(fname);
	if (!file.good())
	{
		fl_alert("Cannot save to file '%s'. No export done.", window->inputFile->value());
		return;
	}

	std::vector<std::string> aspects;
	for (auto & iter : window->checkExport)
	{
		if (iter->value())
		{
			aspects.emplace_back(iter->label());
		}
	}
	SmartBody::exportScene(Session::current->renderScene, file, aspects, "", false);
	file.close();

	fl_alert("Saved to: %s", fname);

	window->hide();

}

void ExportWindow::exportSceneScript( const std::string& filename, std::vector<std::string>& aspects, bool separateScriptFile /*= false*/ )
{
#if (BOOST_VERSION > 104400)	
	namespace fs =  boost::filesystem;
	//using boost::filesystem::dot;
	//using boost::filesystem::slash;
#else	
	namespace fs = boost::filesystem2;
	//using boost::filesystem2::dot;
	//using boost::filesystem2::slash;
#endif
	std::string fileBase = fs::basename(filename);
	fs::path fullFile(filename);
	fs::path filePath = fullFile.parent_path();	
	
	std::ofstream file;
	if (!separateScriptFile) // write all setup in one single file
	{
		std::ofstream file(filename.c_str());
		SmartBody::exportScene(Session::current->renderScene, file, aspects, "", false);
		file.close();
	}
	else // write each configuration aspect into separate file
	{
		for (auto & aspect : aspects)
		{
			std::string outFileName = filePath.string() + "/" + fileBase + "_" + aspect + ".py";
			std::vector<std::string> oneAspect;
			oneAspect.emplace_back(aspect);
			std::ofstream file(filename.c_str());
			SmartBody::exportScene(Session::current->renderScene, file, oneAspect, "", false);
			file.close();
		}
	}		
}

void ExportWindow::ChangeExportTypeCB( Fl_Widget* widget, void* data )
{
	ExportWindow* window = (ExportWindow*) data;
	bool isExportAsset = window->checkExportAsset->value();
	bool isExportZip = window->choiceExportType->value() == 1;

	if (isExportAsset && !isExportZip) // output to a folder
	{
		window->exportToFolder = true;
	}
	else
		window->exportToFolder = false;
}

