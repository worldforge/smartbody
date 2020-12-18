


#include "ResourceWindow.h"
#include <sbm/lin_win.h>

#include <iostream>
#include <vector>
#include <math.h>

#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <boost/version.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <controllers/me_controller_tree_root.hpp>
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBMotion.h>
#include <sb/SBServiceManager.h>
#include <sb/SBJointMapManager.h>
#include <sb/SBJointMap.h>
#include <sb/SBGestureMap.h>
#include <sb/SBGestureMapManager.h>
#include <sb/SBAssetManager.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBBehaviorSetManager.h>
#include <sb/SBGestureMap.h>
#include <sb/nvbg.h>
#include <sbm/GPU/SbmTexture.h>
#include <sbm/action_unit.hpp>
#include <sbm/sr_path_list.h>
#include <sb/sbm_character.hpp>
#include "SBUtilities.h"
#include "FLTKListener.h"
#include "Session.h"

ResourceWindow::ResourceWindow(int x, int y, int w, int h, const char* name)
: Fl_Group(x, y, w, h, name), SBWindowListener(), SelectionListener()
,_firstTime(true)
{
	itemInfoWidget = nullptr;
	lastClickedItemPath = " ";
	emptyString = "";

	/*
	_itemNameList.emplace_back("controller");
	_itemNameList.emplace_back("physics");
	_itemNameList.emplace_back("neutral");
	_itemNameList.emplace_back("au");
	_itemNameList.emplace_back("viseme");
	_itemNameList.emplace_back("default");
	*/

	// create Tree Info Object
	int index = 0;
	this->begin();
	resourceTree = new Fl_TreeHorizontal(x, y, w , h );
	resourceTree->showroot(0);

	Fl_Tree_Item* pathTree = resourceTree->add("Paths");
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(pathTree, "paths"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add(pathTree,"Scripts"), "script path"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add(pathTree,"Motions"), "motion path"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add(pathTree,"Audio"), "audio path"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add(pathTree,"Mesh"), "mesh path"));

	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Scene"), "scene"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Available Scripts"), "scriptfiles"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Runtime Scripts"), "script"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Skeletons"), "skeleton"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Character Maps"), "jointmap"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Gesture Maps"), "gesturemap"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Lip Syncing"), "lipsync"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Motions"), "motion"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Blends"), "blend"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Transitions"), "transition"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Models"), "mesh"));	
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Environment Maps"), "envmap"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Face Definitions"), "facedefinition"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Event Handlers"), "eventhandler"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Pawns"), "pawn"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Characters"), "character"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Services"), "service"));
	_treeMap.insert(std::pair<Fl_Tree_Item*, std::string>(resourceTree->add("Behavior Sets"), "behaviorset"));
	
	resourceTree->callback(treeCallBack,this);	

	this->end();	

	this->resizable(resourceTree);
	updateGUI();	
	//resourceTree->close(resourceTree->root());	
	for (auto iter = _treeMap.begin();
		 iter != _treeMap.end();
		 iter++)
	{
		//(*iter).first->labelfont(FL_HELVETICA_BOLD);
//		(*iter).second.closecond->close();
	}

	windowShow();

	_specialNames[1] = "controller";
	_specialNames[2] = "skeleton";
	_specialNames[3] = "minibrain";
	_specialNames[4] = "physics";
	_specialNames[5] = "neutral";
	_specialNames[6] = "au";
	_specialNames[7] = "viseme";
	_specialNames[8] = "default";

	for (auto & _specialName : _specialNames)
	{
		_reverseSpecialNames.insert(std::pair<std::string, int>(_specialName.second, _specialName.first));
	}

	_dirty = true;
	
}

ResourceWindow::~ResourceWindow()
{
}

std::string ResourceWindow::getNameFromTree(Fl_Tree_Item* item)
{
	auto iter = _treeMap.find(item);
	if (iter != _treeMap.end())
		return (*iter).second;
	else
		return emptyString;
}

Fl_Tree_Item* ResourceWindow::getTreeFromName(const std::string& name)
{
	for (auto & iter : _treeMap)
	{
		if (name == iter.second)
			return iter.first;
	}

	return nullptr;
}

std::string ResourceWindow::getNameFromItem(Fl_Tree_Item* item)
{
	// find the item just under the root item
	std::string subname = "";
	Fl_Tree_Item* curItem = item;
	int numSubnames = 0;
	if (!curItem)
		return "";

	if (item == resourceTree->root())
		return "ROOT";

	while (curItem->parent() != resourceTree->root())
	{
		long specialNameIndex = (long) curItem->user_data();
		if (specialNameIndex > 0)
		{
			subname = "/" + _specialNames[specialNameIndex] + subname;
			numSubnames++;
		}
		curItem = curItem->parent();
	}

	return  getNameFromTree(curItem) + subname;
}

void ResourceWindow::label_viewer( std::string name )
{
	this->label(strdup(name.c_str()));
}

void ResourceWindow::show()
{
	SBWindowListener::windowShow();
	Fl_Group::show();
}

void ResourceWindow::hide()
{
	SBWindowListener::windowHide();
	Fl_Group::hide();
}


bool ResourceWindow::processedDragAndDrop( std::string& dndText )
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	boost::filesystem::path dndPath(dndText);
	std::string fullPathName = dndText;
	std::string filebasename = boost::filesystem::basename(dndText);
	std::string fileextension = boost::filesystem::extension(dndText);					
	std::string fullPath = dndPath.parent_path().string();

	if (fileextension == ".py")
	{
#ifdef WIN32
		fullPath = SmartBody::util::replace(fullPath, "\\", "/");
#endif
		// script file, run it
		scene->addAssetPath("script", fullPath);
		scene->runScript(filebasename + fileextension);
	}
	else
	{
		scene->getAssetStore().loadAssetsFromPath(dndText);
	}
	
	updateGUI();

	return true;
}


int ResourceWindow::handle( int event )
{
	int ret = Fl_Group::handle(event);
	std::string dndText;
	switch ( event ) {
		case FL_DND_RELEASE:
			//SmartBody::util::log("DND Release");
			ret = 1;
			break;
		case FL_DND_ENTER:          // return(1) for these events to 'accept' dnd
			//SmartBody::util::log("DND Enter");
			//Fl::belowmouse(this); // send the leave events first
			//Fl::focus(this);
			//handle(FL_FOCUS);		
			ret = 1;
			break;
		case FL_DND_DRAG:			
			ret = 1;
			break;
		case FL_DND_LEAVE:
			//SmartBody::util::log("DND Leave");
			ret = 1;
			break;	  
		case FL_PASTE:              // handle actual drop (paste) operation		   
			{
				//label(Fl::event_text());
				//fprintf(stderr, "PASTE: %s\n", Fl::event_text());
				//SmartBody::util::log("PASTE: %s\n", Fl::event_text());
				dndText = Fl::event_text();
				bool hasAsset = processedDragAndDrop(dndText);
				ret = 1;				
			}
			break;	
		case FL_PUSH:  
		{// do 'copy/dnd' when someone clicks on box
			if (Fl::event_button() == 2)
			{
				SmartBody::util::log("press middle button");
				SmartBody::util::log("lastClickedItemPath = %s",lastClickedItemPath.c_str());
				Fl_Tree_Item* lastItem = resourceTree->find_item(lastClickedItemPath.c_str());	
				if (lastItem)
				{
					const char* itemType = (const char*)lastItem->user_data();
					bool sendDND = false;
					std::string dndMsg = "";
					if (strcmp(itemType, "skeleton") == 0)
					{
						std::string skName = lastItem->label();
						dndMsg = "SKELETON:";
						dndMsg += skName;
						sendDND = true;
					}
					else if (strcmp(itemType, "pawn") == 0)
					{
						dndMsg = "PAWN:dummy";
						sendDND = true;
					}
					
					//Fl::copy("message",7,0);
					if (sendDND)
					{
						Fl::copy(dndMsg.c_str(),dndMsg.length(),0);
						Fl::dnd();
					}					
					ret = 1;
				}			
			}			
			break;
		}
		case FL_KEYDOWN:  
		{
		  switch (Fl::event_key())
		  {
			case FL_Delete:
				{
					// check pawns and characters for selection
					Fl_Tree_Item* tree = getTreeFromName("pawn");
					int numChildren = tree->children();
					for (int c = 0; c < numChildren; c++)
					{
						Fl_Tree_Item* child = tree->child(c);
						if (child->is_selected())
						{
							const char* name = child->label();
							int confirm = fl_choice(SmartBody::util::format("Are you sure you want to delete '%s'?",name).c_str(), "No", "Yes", nullptr);
							if (confirm == 0)
								return 0;
							SmartBody::SBScene::getScene()->removePawn(name);
							updateGUI();
							return 1;
						}
					}

					tree = getTreeFromName("character");
					numChildren = tree->children();
					for (int c = 0; c < numChildren; c++)
					{
						Fl_Tree_Item* child = tree->child(c);
						if (child->is_selected())
						{
							const char* name = child->label();
							int confirm = fl_choice(SmartBody::util::format("Are you sure you want to delete '%s'?",name).c_str(), "No", "Yes", nullptr);
							if (confirm == 0)
								return 0;
							SmartBody::SBScene::getScene()->removeCharacter(name);
							updateGUI();
							return 1;
						}
					}
				
				}
				break;
			default:
				break;
		  }
		}
	}
	return ret;
}

void ResourceWindow::update()
{
		
	if (0) //mcu.resourceDataChanged) // don't update the tree if nothing happens
	{		
		updateGUI();
		if (itemInfoWidget)
		{
			itemInfoWidget->updateWidget();
		}
		this->redraw();				
	}	
}

void ResourceWindow::draw()
{	
	if (_dirty)
	{
		updateGUI();
	}
	Fl_Group::draw();
}

void ResourceWindow::resize( int x, int y, int w, int h )
{
	Fl_Group::resize(x, y, w, h);
}

void ResourceWindow::updateGUI()
{

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBAssetManager* assetManager = scene->getAssetManager();
	auto& renderAssetManager = Session::current->renderAssetManager;

	resourceTree->sortorder(FL_TREE_SORT_ASCENDING);	
	// update path tree	
	updatePath(getTreeFromName("script path"), SmartBody::SBScene::getScene()->getAssetPaths("script"));	
	updatePath(getTreeFromName("motion path"), SmartBody::SBScene::getScene()->getAssetPaths("motion"));	
	updatePath(getTreeFromName("audio path"), SmartBody::SBScene::getScene()->getAssetPaths("audio"));	
	updatePath(getTreeFromName("mesh path"), SmartBody::SBScene::getScene()->getAssetPaths("mesh"));	
	

	// update sequence file list

	const std::vector<std::string> scriptPaths = SmartBody::SBScene::getScene()->getAssetPaths("script");
	resourceTree->clear_children(getTreeFromName("scriptfiles"));
	for (const auto & scriptPath : scriptPaths)
	{
		updateScriptFiles(getTreeFromName("scriptfiles"), scriptPath);
	}	

	// update runtime scripts
	resourceTree->clear_children(getTreeFromName("script"));
	std::vector<std::string> scriptName = scene->getScriptNames();
	for (auto & i : scriptName)
	{
		SmartBody::SBScript* script = scene->getScript(i);
		updateScript(getTreeFromName("script"), script);
	}

	// update skeleton
	resourceTree->clear_children(getTreeFromName("skeleton"));
	std::vector<std::string> skeletons = scene->getSkeletonNames();
	for (auto & skeleton : skeletons)
	{
		auto skel = scene->getSkeleton(skeleton);
		updateSkeleton(getTreeFromName("skeleton"), skel.get());
	}

	// update joint maps
	resourceTree->clear_children(getTreeFromName("jointmap"));	
	SmartBody::SBJointMapManager* jointMapManager = scene->getJointMapManager();
	std::vector<std::string> jointMapNames = jointMapManager->getJointMapNames();
	for (auto & jointMapName : jointMapNames)
	{
		Fl_Tree_Item* boneMapItem = resourceTree->add(getTreeFromName("jointmap"), jointMapName.c_str());
		updateJointMap(boneMapItem, jointMapManager->getJointMap(jointMapName));
	}

	// update gesture maps
	resourceTree->clear_children(getTreeFromName("gesturemap"));
	SmartBody::SBGestureMapManager* gestureMapManager = scene->getGestureMapManager();
	std::vector<std::string> gestureMapNames = gestureMapManager->getGestureMapNames();
	for (auto & gestureMapName : gestureMapNames)
	{
		Fl_Tree_Item* gestureMapItem = resourceTree->add(getTreeFromName("gesturemap"), gestureMapName.c_str());
		updateGestureMap(gestureMapItem, gestureMapManager->getGestureMap(gestureMapName));
	}

	// update motion map
	resourceTree->clear_children(getTreeFromName("motion"));
	std::vector<std::string> motionNames = scene->getMotionNames();
	for (auto & motionName : motionNames)
	{
		//resourceTree->add(treeItemList[ITEM_MOTION],mi->first.c_str());
		SmartBody::SBMotion * motion = scene->getMotion(motionName);
		if (motion)
			updateMotion(getTreeFromName("motion"), motion);
	}

	SmartBody::SBAnimationBlendManager* blendManager = scene->getBlendManager();
	// update animation blend map
	resourceTree->clear_children(getTreeFromName("blend"));
	std::vector<std::string> blendNames = blendManager->getBlendNames();
	for (auto & blendName : blendNames)
	{
		//resourceTree->add(treeItemList[ITEM_MOTION],mi->first.c_str());
		SmartBody::SBAnimationBlend * blend = blendManager->getBlend(blendName);
		updateAnimationBlend(getTreeFromName("blend"), blend);
	}

	// update blend transition map
	resourceTree->clear_children(getTreeFromName("transition"));
	std::vector<std::string> transitionNames = blendManager->getTransitionNames();
	for (auto & transitionName : transitionNames)
	{
		//resourceTree->add(treeItemList[ITEM_MOTION],mi->first.c_str());
		SmartBody::SBAnimationTransition * transition = blendManager->getTransitionByName(transitionName);
		updateBlendTransition(getTreeFromName("transition"), transition);
	}

	// update mesh map
	resourceTree->clear_children(getTreeFromName("mesh"));
	std::vector<std::string> meshNames = renderAssetManager.getDeformableMeshNames();
	for (auto & meshName : meshNames)
	{
		DeformableMesh* mesh = renderAssetManager.getDeformableMesh(meshName);
		Fl_Tree_Item* meshItem = resourceTree->add(getTreeFromName("mesh"), mesh->getName().c_str());
		updateMesh(meshItem, mesh);
	}

	// update environment map
	resourceTree->clear_children(getTreeFromName("envmap"));
	SbmTextureManager& texManager = SbmTextureManager::singleton();
	std::vector<std::string> envMaps = texManager.getTextureNames(SbmTextureManager::TEXTURE_HDR_MAP);
	for (auto & envMap : envMaps)
	{
		auto texture = texManager.findTexture(envMap.c_str());
		Fl_Tree_Item* meshItem = resourceTree->add(getTreeFromName("envmap"), texture->getName().c_str());
		updateEnvMap(meshItem, texture.get());
	}

	// update face definition map
	resourceTree->clear_children(getTreeFromName("facedefinition"));
	std::vector<std::string> faceNames = scene->getFaceDefinitionNames();
	for (auto & faceName : faceNames)
	{
		//resourceTree->add(treeItemList[ITEM_MOTION],mi->first.c_str());
		SmartBody::SBFaceDefinition * face = scene->getFaceDefinition(faceName);
		if (!face)
			continue;
		Fl_Tree_Item* faceTree = resourceTree->add(getTreeFromName("facedefinition"), face->getName().c_str());
		updateFaceDefinition(faceTree, face);
	}

	// update event handler list
	SmartBody::SBEventManager* eventManager = SmartBody::SBScene::getScene()->getEventManager();
	std::map<std::string, SmartBody::SBEventHandler*>& eventMap = eventManager->getEventHandlers();
	std::map<std::string, SmartBody::SBEventHandler*>::iterator ei;
	resourceTree->clear_children(getTreeFromName("eventhandler"));
	for ( ei  = eventMap.begin();
		  ei != eventMap.end();
		  ei++)
	{
		updateEventHandler(getTreeFromName("eventhandler"), ei->second);
	}
	// Below are instance objects :

	// update pawn objects
	resourceTree->clear_children(getTreeFromName("pawn"));
	const std::vector<std::string>& pawnNames = scene->getPawnNames();
	for (const auto & pawnName : pawnNames)
	{
		SmartBody::SBPawn* pawn = scene->getPawn(pawnName);
		updatePawn(getTreeFromName("pawn"), pawn);
	}

	// update characters
	resourceTree->clear_children(getTreeFromName("character"));
	const std::vector<std::string>& charNames = scene->getCharacterNames();
	for (const auto & charName : charNames)
	{
		SmartBody::SBCharacter* character = scene->getCharacter(charName);
		resourceTree->sortorder(FL_TREE_SORT_ASCENDING);
		updateCharacter(getTreeFromName("character"), character);
	}

	
// 	for (SBPhysicsObjMap::iterator iter = phySim->getPhysicsObjMap().begin();
// 		 iter != phySim->getPhysicsObjMap().end();
// 		 iter++)
// 	{
// 		SBPhysicsObj* obj = (*iter).second;
// 		if (dynamic_cast<SbmJointObj*>(obj) == nullptr)
// 		{
// 
// 		}
// 	}

	// update services
	SmartBody::SBServiceManager* serviceManager = scene->getServiceManager();
	std::map<std::string, SmartBody::SBService*>& serviceMap = serviceManager->getServices();

	resourceTree->clear_children(getTreeFromName("service"));
	for (auto & iter : serviceMap)
	{
		SmartBody::SBService* service = iter.second;
		resourceTree->sortorder(FL_TREE_SORT_ASCENDING);	
		auto* phyManager = dynamic_cast<SmartBody::SBPhysicsManager*>(service);
		if (phyManager)
			updatePhysicsManager(getTreeFromName("service"),phyManager);
		else
			updateService(getTreeFromName("service"), service);
	}

	// update behavior sets
	resourceTree->clear_children(getTreeFromName("behaviorset"));
	std::map<std::string, SmartBody::SBBehaviorSet*>& behaviorSets = scene->getBehaviorSetManager()->getBehaviorSets();
	for (auto & iter : behaviorSets)
	{
		SmartBody::SBBehaviorSet* behaviorSet = iter.second;
		updateBehaviorSet(getTreeFromName("behaviorset"), behaviorSet);
	}


	_dirty = false;

	if (_firstTime)
	{
		hideTree();
		_firstTime = false;
	}
}


void ResourceWindow::updatePhysicsManager( Fl_Tree_Item* tree, SmartBody::SBPhysicsManager* phyService )
{/*
	SmartBody::SBPhysicsSim* phySim = phyService->getPhysicsEngine();
	Fl_Tree_Item* item = resourceTree->add(tree, phyService->getName().c_str());
	item->user_data((void*)ITEM_PHYSICS);
	resourceTree->sortorder(FL_TREE_SORT_NONE);

	for (SmartBody::SBPhysicsCharacterMap::iterator iter = phySim->getCharacterMap().begin();
		iter != phySim->getCharacterMap().end();
		iter++)
	{
		SmartBody::SBPhysicsCharacter* phyChar = (*iter).second;
		resourceTree->sortorder(FL_TREE_SORT_ASCENDING);
		updatePhysicsCharacter(item,phyChar);
	}

	for (SmartBody::SBPhysicsPawnMap::iterator iter = phySim->getPawnObjMap().begin();
		 iter != phySim->getPawnObjMap().end();
		 iter++)
	{
		SmartBody::SBPhysicsObj* phyObj = (*iter).second;
		Fl_Tree_Item* phyObjItem = resourceTree->add(item, phyObj->getName().c_str());
		phyObjItem->user_data((void*)ITEM_PHYSICS);
	}
	*/
}


void ResourceWindow::updateFaceDefinition( Fl_Tree_Item* tree, SmartBody::SBFaceDefinition* faceDefinition )
{
	std::string neutralMotionName = "NA";
	if (faceDefinition->getFaceNeutral())
	{
		neutralMotionName = faceDefinition->getFaceNeutral()->getName();		
	}
	Fl_Tree_Item* neutralMotionTree = resourceTree->add(tree,"Neutral Expression");
	neutralMotionTree->close();

	Fl_Tree_Item* item = resourceTree->add(neutralMotionTree,neutralMotionName.c_str());

	// update action unit tree
	Fl_Tree_Item* auTree = resourceTree->add(tree,"Action Units (AUs)");
	auTree->close();
	int numAUs = faceDefinition->getNumAUs();
	for (int a = 0; a < numAUs; a++)
	{
		int auNUm = faceDefinition->getAUNum(a);
		ActionUnit* au = faceDefinition->getAU(auNUm);
		std::string auName = "Au " + boost::lexical_cast<std::string>(faceDefinition->getAUNum(a));
		
		Fl_Tree_Item* auItem = resourceTree->add(auTree,auName.c_str());
		auItem->close();
		std::string auType = "bilateral:";
		if (au->is_bilateral())
		{	
			Fl_Tree_Item* item = nullptr;
			if (au->left)
				item = resourceTree->add(auItem,(auType+ au->left->getName()).c_str());
			else
				item = resourceTree->add(auItem, auType.c_str());
		}
		else 
		{
			if (au->is_left())
			{
				auType = "left:";
				Fl_Tree_Item* item = nullptr;
				if (au->left)
				{
					item = resourceTree->add(auItem,(auType+au->left->getName()).c_str());
				}
				else
				{
					item = resourceTree->add(auItem, auType.c_str());
				}
			}
			if (au->is_right())
			{
				auType = "right:";
				if (au->right)
				{
					item = resourceTree->add(auItem,(auType+au->right->getName()).c_str());
				}
				else
				{
					item = resourceTree->add(auItem, auType.c_str());
				}
			}
		}		
	}

	// update viseme tree
	Fl_Tree_Item* visemeTree = resourceTree->add(tree,"Visemes");	
	visemeTree->close();
	int numVisemes = faceDefinition->getNumVisemes();
	for (int v = 0; v < numVisemes; v++)
	{
		std::string visemeName = faceDefinition->getVisemeName(v);
		std::string motionName = "";
		if (faceDefinition->getVisemeMotion(visemeName))
			motionName = faceDefinition->getVisemeMotion(visemeName)->getName();
		Fl_Tree_Item* item = resourceTree->add(visemeTree,(visemeName+"-->"+motionName).c_str());
	}
}

void ResourceWindow::updatePath( Fl_Tree_Item* tree, const std::vector<std::string>& pathList )
{
	resourceTree->clear_children(tree);	
	for (const auto & p : pathList)
	{
		Fl_Tree_Item* item = resourceTree->add(tree, p.c_str());
		item->user_data(tree->user_data());
	}	
}

void ResourceWindow::updateBehaviorSet( Fl_Tree_Item* tree, SmartBody::SBBehaviorSet* behaviorSet )
{
	Fl_Tree_Item* item = resourceTree->add(tree, behaviorSet->getName().c_str());
}

void ResourceWindow::updateScript( Fl_Tree_Item* tree, SmartBody::SBScript* script )
{
	Fl_Tree_Item* item = resourceTree->add(tree, script->getName().c_str());
}

void ResourceWindow::updateScriptFiles( Fl_Tree_Item* tree, std::string pname )
{	
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& scriptPaths = scene->getAssetStore().getAssetPaths("script");
	
	for (const auto & scriptPath : scriptPaths)
	{
		boost::filesystem::path path(scriptPath);
		if (boost::filesystem::is_directory(path))
		{
			boost::filesystem::directory_iterator end;
			for (boost::filesystem::directory_iterator iter(path); iter != end; iter++)
			{
				const boost::filesystem::path& cur = (*iter);
				std::string ext = boost::filesystem::extension( cur );
				if (ext == ".seq" || ext == ".SEQ" || ext == ".py" || ext == ".PY")
				{
					std::string fileName = boost::filesystem::basename(cur);
					if (tree->find_child(fileName.c_str()) == -1) // if the script name does not exist in the tree list
					{
						Fl_Tree_Item* item = resourceTree->add(tree, fileName.c_str());						
					}
				}
			}
		}
	}
}


void ResourceWindow::updateJointMap( Fl_Tree_Item* tree, SmartBody::SBJointMap* jointMap )
{
	for (int i=0;i<jointMap->getNumMappings();i++)
	{
		std::string key = jointMap->getSource(i);
		std::string target = jointMap->getTarget(i);

		
		Fl_Tree_Item* item = resourceTree->add(tree,(key+"-->"+target).c_str());	
		//resourceTree->add(item,target.c_str());
	}
}

void ResourceWindow::updateGestureMap( Fl_Tree_Item* tree, SmartBody::SBGestureMap* gestureMap )
{
	for (int i=0;i<gestureMap->getNumMappings();i++)
	{
		SmartBody::SBGestureMap::GestureInfo& info = gestureMap->getGestureByIndex(i);

		std::stringstream strstr;
		strstr << info._lexeme << ", " << info._type << ", " << info._hand << ", " << info._style << ", " << info._posture << "-> " << info._animation;
		Fl_Tree_Item* item = resourceTree->add(tree, strstr.str().c_str());	
	}
}

void ResourceWindow::updateEventHandler( Fl_Tree_Item* tree, SmartBody::SBEventHandler* handler )
{
	Fl_Tree_Item* item = resourceTree->add(tree, handler->getName().c_str());
}

void ResourceWindow::updateSkeleton( Fl_Tree_Item* tree, SmartBody::SBSkeleton* skel )
{
#if (BOOST_VERSION > 104400)
	std::string ext = boost::filesystem::extension( skel->skfilename() );
#else
	std::string ext = boost::filesystem2::extension( skel->skfilename() );
#endif	
	Fl_Tree_Item* item = resourceTree->add(tree,skel->getName().c_str());
	//item->user_data((void*)ITEM_SKELETON);
}


void ResourceWindow::updateMesh( Fl_Tree_Item* tree, DeformableMesh* mesh )
{
	for (int i=0; i< mesh->getNumMeshes();i++)
	{
		std::string meshName = mesh->getMeshName(i);
		
		Fl_Tree_Item* item = resourceTree->add(tree,meshName.c_str());	
		// for each mesh, get the materials associated with it
		SrModel& model = mesh->getStaticModel(i);
		for (int m = 0; m < model.M.size(); m++)
		{
			std::string materialName = model.mtlnames[m];
			Fl_Tree_Item* materialItem = resourceTree->add(item, materialName.c_str());	
		}
		if (!mesh->blendShapeMap.empty())
		{
			Fl_Tree_Item* morphTargetsItem = resourceTree->add(item, "morph targets");
			// display the blend shapes
			for (auto & blendShapeIter : mesh->blendShapeMap)
			{
				std::string baseShape = blendShapeIter.first;
				if (strcmp(meshName.c_str(), baseShape.c_str()) != 0)
				{
					continue;
				}

				std::vector<SrSnModel*>& models = blendShapeIter.second;
				for (auto & model : models)
				{
					if (model)
						resourceTree->add(morphTargetsItem, (const char*)model->shape().name);
				}
			}
		}
	}

}

void ResourceWindow::updateEnvMap(Fl_Tree_Item* tree, SbmTexture* texture)
{
}

void ResourceWindow::updateAnimationBlend( Fl_Tree_Item* tree, SmartBody::SBAnimationBlend* blend )
{
	Fl_Tree_Item* item = resourceTree->add(tree, blend->stateName.c_str());
//	item->user_data((void*)ITEM_ANIMATION_BLEND);

}

void ResourceWindow::updateBlendTransition( Fl_Tree_Item* tree, SmartBody::SBAnimationTransition* transition )
{	
	std::string transitionName = transition->getTransitionName();
	Fl_Tree_Item* item = resourceTree->add(tree, transitionName.c_str());
//	item->user_data((void*)ITEM_BLEND_TRANSITION);
}


void ResourceWindow::updateMotion( Fl_Tree_Item* tree, SmartBody::SBMotion* motion )
{
	Fl_Tree_Item* item = resourceTree->add(tree, motion->getName().c_str());
//	item->user_data((void*)ITEM_MOTION);
}

void ResourceWindow::updatePawn( Fl_Tree_Item* tree, SmartBody::SBPawn* pawn )
{
	if (dynamic_cast<SbmCharacter*>(pawn) != nullptr)
		return; // this is actually a character

	Fl_Tree_Item* item = resourceTree->add(tree,pawn->getName().c_str());
//	item->user_data((void*)ITEM_PAWN);
}


void ResourceWindow::updatePhysicsCharacter( Fl_Tree_Item* tree, SmartBody::SBPhysicsCharacter* phyChar )
{
	Fl_Tree_Item* item = resourceTree->add(tree,phyChar->getPhysicsCharacterName().c_str());
//	item->user_data((void*)ITEM_PHYSICS);
	resourceTree->sortorder(FL_TREE_SORT_NONE);	
	std::vector<SmartBody::SBPhysicsJoint*> jointList = phyChar->getPhyJointList();
	for (auto phyJoint : jointList)
	{
			Fl_Tree_Item* jointItem = resourceTree->add(item,phyJoint->getSBJoint()->getName().c_str());
//		jointItem->user_data((void*)ITEM_PHYSICS);
		//Fl_Tree_Item* rigidBodyItem = resourceTree->add(jointItem,"body");
		//rigidBodyItem->user_data((void*)ITEM_PHYSICS);
	}

}

void ResourceWindow::updateCharacter( Fl_Tree_Item* tree, SmartBody::SBCharacter* character )
{
	SmartBody::SBCharacter* sbcharacter = dynamic_cast<SmartBody::SBCharacter*>(character);
	Fl_Tree_Item* item = resourceTree->add(tree,character->getName().c_str());
	item->user_data((void*) addSpecialName(character->getName()));
	resourceTree->sortorder(FL_TREE_SORT_NONE);		
	Fl_Tree_Item* skeletonFolder = resourceTree->add(item,"skeleton");	
	skeletonFolder->user_data((void*) _reverseSpecialNames["skeleton"]); 
	auto sbSk = sbcharacter->getSkeleton();
	if (sbSk)
	{
		Fl_Tree_Item* charSkItem = resourceTree->add(skeletonFolder, sbSk->getName().c_str());
	}
	Fl_Tree_Item* controllerFolder = resourceTree->add(item,"controllers");	
	controllerFolder->user_data((void*) _reverseSpecialNames["controller"]); 
	controllerFolder->close();
	// add controllers
	auto& ctTree = character->ct_tree_p ;
	if( ctTree )
	{
		int n = ctTree->count_controllers();
		for (int c = 0; c < n; c++)
		{
			//SmartBody::util::log( "%s", ctTree->controller(c)->name() );
			std::string controllerName = ctTree->controller(c)->getName();
			Fl_Tree_Item* ctrlItem = resourceTree->add(controllerFolder, controllerName.c_str());
//			ctrlItem->user_data((void*)ITEM_CONTROLLER);
		}
	}
	/*
	// add gesture map
	Fl_Tree_Item* gestureFolder = resourceTree->add(item,"gestures");	
	gestureFolder->user_data((void*)-1);
	gestureFolder->close();
	// add individual gesture mappings
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

	SBGestureMap* gestureMap = scene->getGestureMapManager()->getGestureMap(sbcharacter->getName());
	if (gestureMap)
	{
		std::string lexeme;
		std::string type;
		std::string hand;
		std::string style;
		std::string posture;

		gestureMap->getGestureByInfo(lexeme, type, hand, style, posture);
		Fl_Tree_Item* gestureItem = resourceTree->add(gestureFolder, lexeme.c_str());
		gestureItem->user_data((void*)ITEM_GESTUREMAP);
	}
	*/

	// add NVBG
	Fl_Tree_Item* nvbgItem = resourceTree->add(item, "minibrain");
	nvbgItem->user_data((void*) _reverseSpecialNames["minibrain"]); 
	SmartBody::Nvbg* nvbg = character->getNvbg();
	if (nvbg)
	{
		nvbgItem = resourceTree->add(item, nvbg->getName().c_str());
	}
}


void ResourceWindow::updateService( Fl_Tree_Item* tree, SmartBody::SBService* service )
{
	Fl_Tree_Item* item = resourceTree->add(tree, service->getName().c_str());
//	item->user_data((void*)ITEM_SERVICES);
	resourceTree->sortorder(FL_TREE_SORT_NONE);	
}

void ResourceWindow::refreshUI( Fl_Widget* widget, void* data )
{
	ResourceWindow* window = (ResourceWindow*)data;
	window->updateGUI();	
}

void ResourceWindow::treeCallBack( Fl_Widget* widget, void* data )
{
	//SmartBody::util::log("Tree call back....");

	Fl_Tree      *tree = (Fl_Tree*)widget;
	Fl_Tree_Item *item = (Fl_Tree_Item*)tree->callback_item();	// get selected item
	ResourceWindow* window = (ResourceWindow*)data;
	if (tree->callback_reason() == FL_TREE_REASON_SELECTED)
	{
		std::stringstream strstr;
		strstr <<  window->getNameFromItem(item);
		long specialNameIndex = (long) item->user_data();
		if (specialNameIndex == 0) // assume any item with a special name doesn't need an additional label
			strstr << "/" << item->label();
			
		SBSelectionManager::getSelectionManager()->select(strstr.str());
	}	
	if (tree->callback_reason() == FL_TREE_REASON_DESELECTED)
	{
		std::stringstream strstr;
		strstr <<  window->getNameFromItem(item) << "/" << item->label();
			
		//SBSelectionManager::getSelectionManager()->select("");
	}
}

void ResourceWindow::selectPawn(const std::string& name)
{
	Fl_Tree_Item* tree = getTreeFromName("pawn");

	for (int c = 0; c < tree->children(); c++)
	{
		Fl_Tree_Item* child = tree->child(c);
		if (strcmp(child->label(), name.c_str()) == 0)
		{
			if (child->is_selected())
				return;
			resourceTree->deselect_all();
			child->select();
			resourceTree->redraw();
			return;
		}
	}

	tree = getTreeFromName("character");
	for (int c = 0; c < tree->children(); c++)
	{
		Fl_Tree_Item* child = tree->child(c);
		if (strcmp(child->label(), name.c_str()) == 0)
		{
			if (child->is_selected())
				return;
			resourceTree->deselect_all();
			child->select();
			resourceTree->redraw();
			return;
		}
	}

}


void ResourceWindow::notify( SmartBody::SBSubject* subject )
{
	this->refreshUI(this, this);
}


void ResourceWindow::OnSelect(const std::string& value)
{
	if (value == "")
	{
		// deselect all
		resourceTree->select_only(nullptr);
	}
	SmartBody::SBObject* object = SmartBody::SBScene::getScene()->getObjectFromString(value);
	if (!object)
		return;

	Fl_Tree_Item* item = nullptr;
	auto* character = dynamic_cast<SmartBody::SBCharacter*>(object);
	if (character)
	{
		// select a character
		for (auto & iter : _treeMap)
		{
			if (iter.second == "character")
			{
				item = iter.first;
				// make sure that the parent is open so that the selection can be seen
				resourceTree->open(item);
			}
		}
		if (!item)
			return;
	}
	else
	{
		SmartBody::SBPawn* pawn = dynamic_cast<SmartBody::SBPawn*>(object);
		if (pawn)
		{
			// select a pawn
			for (auto & iter : _treeMap)
			{
				if (iter.second == "pawn")
				{
					item = iter.first;
					// make sure that the parent is open so that the selection can be seen
					resourceTree->open(item);
				}
			}
			if (!item)
				return;
		}
	}

	if (!item)
		return;

	int numChildren = item->children();
	for (int c = 0; c < numChildren; c++)
	{
		Fl_Tree_Item* child = item->child(c);
		if (object->getName() == child->label())
		{
			resourceTree->select_only(child);
		}
	}

}

void ResourceWindow::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{
	_dirty = true;
	this->redraw();
}

void ResourceWindow::OnCharacterDelete( const std::string & name )
{
	removeSpecialName(name);
	_dirty = true;
	this->redraw();
}

void ResourceWindow::OnCharacterUpdate( const std::string & name )
{
	_dirty = true;
	this->redraw();
}
      
void ResourceWindow::OnPawnCreate( const std::string & name )
{
	_dirty = true;
	this->redraw();
}

void ResourceWindow::OnPawnDelete( const std::string & name )
{
	_dirty = true;
	this->redraw();
}

void ResourceWindow::OnObjectCreate( SmartBody::SBObject* object )
{
	_dirty = true;
	this->redraw();
}

void ResourceWindow::OnObjectDelete( SmartBody::SBObject* object )
{
	_dirty = true;
	this->redraw();
}

void ResourceWindow::OnSimulationStart()
{
	_dirty = true;
	_firstTime = true;
	this->redraw();
}

size_t ResourceWindow::addSpecialName(const std::string& name)
{
	auto iter = _reverseSpecialNames.find(name);
	if (iter != _reverseSpecialNames.end())
	{
		return (*iter).second;
	}

	size_t index = _specialNames.size() + 1;
	_specialNames.insert(std::pair<int, std::string>(index, name));
	_reverseSpecialNames.insert(std::pair<std::string, int >(name, index));

	return index;
}

void ResourceWindow::removeSpecialName(const std::string& name)
{
	auto iter = _reverseSpecialNames.find(name);
	if (iter != _reverseSpecialNames.end())
	{
		size_t val = (*iter).second;
		_reverseSpecialNames.erase(iter);
		auto iter2 = _specialNames.find(val);
		_specialNames.erase(iter2);

	
	}
}

void ResourceWindow::hideTree()
{
	// show only top-level items
	Fl_Tree_Item* root = resourceTree->first();
	int numChildren = root->children();
	for (int c = 0; c < numChildren; c++)
	{
		Fl_Tree_Item* item = root->child(c);
		item->close();
	}
}

void ResourceWindow::showTree()
{
	// show everything under the top-level items
	Fl_Tree_Item* root = resourceTree->first();
	int numChildren = root->children();
	for (int c = 0; c < numChildren; c++)
	{
		Fl_Tree_Item* item = root->child(c);
		item->open();
	}
}


