/*************************************************************
Copyright (C) 2017 University of Southern California

This file is part of Smartbody.

Smartbody is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Smartbody is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Smartbody.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************/

#include "SBAssetManager.h"
#include <sb/SBSkeleton.h>
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include <sb/SBSceneListener.h>
#include <sb/SBNavigationMesh.h>
#include "SBUtilities.h"
#include <sbm/lin_win.h>
#include <sbm/sr_path_list.h>
#include <sbm/sbm_constants.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#if defined(EMSCRIPTEN)
#include <emscripten.h>
#endif

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#ifndef _MAX_PATH
#define _MAX_PATH 1024
#endif
#endif

#define DEBUG_LOAD_PATHS2 (0)

namespace SmartBody {

SBAssetManager::SBAssetManager(SBAssetStore& store)
:
_store(store),
uniqueSkeletonId(0),
_motionCounter(0),
_skeletonCounter(0)
{

	createDoubleAttribute("globalSkeletonScale", 1,true,"",30,false,false,false,"Multiplier when loading all skeletons. ");
	createDoubleAttribute("globalMotionScale", 1,true,"",30,false,false,false,"Multiplier when loading all motions.");

	uniqueSkeletonId = 0;

	_motionCounter = 0;
	_skeletonCounter = 0;
	store.addAssetProcessor(this);
}

SBAssetManager::~SBAssetManager()
{
	_store.removeAssetProcessor(this);
	removeAllAssets();
}


void SBAssetManager::removeAllAssets()
{
	_motions.clear();
	_skeletons.clear();

}

double SBAssetManager::getGlobalMotionScale()
{
	return getDoubleAttribute("globalMotionScale");
}

void SBAssetManager::setGlobalMotionScale(double val)
{
	setDoubleAttribute("globalMotionScale", val);
}

double SBAssetManager::getGlobalSkeletonScale()
{
	return getDoubleAttribute("globalSkeletonScale");
}

void SBAssetManager::setGlobalSkeletonScale(double val)
{
	setDoubleAttribute("globalSkeletonScale", val);
}



boost::intrusive_ptr<SBSkeleton> SBAssetManager::createSkeleton(const std::string& skeletonDefinition)
{
	if (skeletonDefinition.empty())
	{
		boost::intrusive_ptr<SBSkeleton> skeleton(new SBSkeleton());
		std::stringstream strstr;
		strstr << "skeleton" << uniqueSkeletonId;
		uniqueSkeletonId++;
		skeleton->setName(strstr.str());
		skeleton->skfilename(strstr.str().c_str());
		auto result = _skeletons.emplace(strstr.str(), std::move(skeleton));
		return result.second ? result.first->second.get() : nullptr;

	}

	auto templateSkeleton = this->getSkeleton(skeletonDefinition);
	if (templateSkeleton)
	{
		return {new SBSkeleton(templateSkeleton.get())};
	}
	else
	{
		return {new SBSkeleton(skeletonDefinition)};
	}
}


boost::intrusive_ptr<SBSkeleton> SBAssetManager::getSkeleton(const std::string& name)
{

	auto iter = _skeletons.find(name);
	if (iter != _skeletons.end()) {
		return iter->second.get();
	} else {
		return nullptr;
	}
}

void SBAssetManager::processAssets(std::vector<std::unique_ptr<SBAsset>>& assets) {
// place the assets in their proper place
	for (auto& asset : assets) {
		if (asset) {
			auto* motion = dynamic_cast<SBMotion*>(asset.get());
			if (motion) {
				SBMotion* existingMotion = this->getMotion(motion->getName());
				if (existingMotion) {
					std::string name = this->getAssetNameVariation(existingMotion);
					SmartBody::util::log("Motion named %s already exist, changing name to %s", motion->getName().c_str(), name.c_str());
					motion->setName(name);
				}
				asset.release();
				this->addMotion(std::unique_ptr<SBMotion>(motion));
				addAssetHistory("MOTION " + motion->getName());
				continue;
			}
			auto* skeleton = dynamic_cast<SmartBody::SBSkeleton*>(asset.get());
			if (skeleton) {
				auto existingSkeleton = this->getSkeleton(skeleton->getName());
				if (existingSkeleton) {
					std::string name = this->getAssetNameVariation(existingSkeleton.get());
					SmartBody::util::log("Skeleton named %s already exist, changing name to %s", skeleton->getName().c_str(), name.c_str());
					skeleton->setName(name);
				}
				asset.release();
				this->addSkeleton({skeleton});
				skeleton->ref();
				addAssetHistory("SKELETON " + skeleton->getName());
				continue;
			}
		}
	}
}

boost::intrusive_ptr<SBSkeleton> SBAssetManager::addSkeletonDefinition(const std::string& skelName )
{
	auto existingSkeleton = this->getSkeleton(skelName);
	if (existingSkeleton)
	{
		SmartBody::util::log("Skeleton named %s already exists, new skeleton will not be created.", skelName.c_str());
		return nullptr;
	}
	boost::intrusive_ptr<SBSkeleton> sbSkel(new SBSkeleton());
	sbSkel->setName(skelName);
	sbSkel->skfilename(skelName.c_str());
	auto result = _skeletons.emplace(sbSkel->getName(), std::move(sbSkel));

	if (result.second) {
		std::vector<SBSceneListener*>& listeners = SmartBody::SBScene::getScene()->getSceneListeners();
		for (auto & listener : listeners)
		{
			listener->OnObjectCreate(result.first->second.get());
		}

		return result.first->second.get();

	} else {
		return nullptr;
	}
}

void SBAssetManager::removeSkeletonDefinition(const std::string& skelName )
{
	auto iter = _skeletons.find(skelName);
	if (iter == _skeletons.end())
	{
		SmartBody::util::log("Skeleton named %s does not exist.", skelName.c_str());
		return;
	}

	auto& existingSkeleton = (*iter).second;

	std::vector<SBSceneListener*>& listeners = SmartBody::SBScene::getScene()->getSceneListeners();
	for (auto & listener : listeners)
	{
		listener->OnObjectDelete(existingSkeleton.get());
	}

	_skeletons.erase(iter);
}

SBAPI void SBAssetManager::addSkeleton(boost::intrusive_ptr<SBSkeleton> skeleton)
{
	_skeletons[skeleton->getName()] = std::move(skeleton);
}

SBMotion* SBAssetManager::createMotion(const std::string& motionName)
{
	auto iter = _motions.find(motionName);
	if (iter != _motions.end())
	{
		SmartBody::util::log("Motion named %s already exists, new motion will not be created.", motionName.c_str());
		return nullptr;
	}

	auto motion = std::make_unique<SBMotion>();
	motion->setName(motionName);
	auto result = _motions.emplace(motionName, std::move(motion));

	if (result.second) {
		std::vector<SBSceneListener*>& listeners = SmartBody::SBScene::getScene()->getSceneListeners();
		for (auto & listener : listeners)
		{
			listener->OnObjectCreate(result.first->second.get());
		}

		return result.first->second.get();
	} else {
		return nullptr;
	}
}

SBAPI bool SBAssetManager::addMotion(std::unique_ptr<SmartBody::SBMotion> motion)
{
	auto iter = _motions.find(motion->getName());
	if (iter != _motions.end())
	{
		SmartBody::util::log("Motion named %s already exists, new motion will not be added.", motion->getName().c_str());
		return false;
	}
	auto result = _motions.emplace(motion->getName(), std::move(motion));
	if (result.second) {
		std::vector<SBSceneListener*>& listeners = SmartBody::SBScene::getScene()->getSceneListeners();
		for (auto & listener : listeners)
		{
			listener->OnObjectCreate(result.first->second.get());
		}

		return true;
	} else {
		return false;
	}
}

SBMotion* SBAssetManager::addMotionDefinition(const std::string& name, double duration, int numFrames)
{
	auto iter = _motions.find(name);
	if (iter != _motions.end())
	{
		SmartBody::util::log("Motion named %s already exists, new motion will not be added.", name.c_str());
		return nullptr;
	}

	auto sbMotion = std::make_unique<SBMotion>();
	if (numFrames <= 2 && duration > 0)
	{
		sbMotion->insert_frame(0,0.f);
		sbMotion->insert_frame(1,(float)duration);
	}
	else if (numFrames > 2 && duration > 0)// motion frame > 2
	{
		float deltaT = (float)duration/(numFrames-1);
		for (int i=0;i<numFrames;i++)
		{
			sbMotion->insert_frame(i,deltaT*i);
		}
	}
	sbMotion->setName(name);
	auto motionPtr = sbMotion.get();
	auto result = this->addMotion(std::move(sbMotion));
	if (result) {
		return motionPtr;
	} else {
		return nullptr;
	}
}


SBAPI void SBAssetManager::removeMotion(SmartBody::SBMotion* motion)
{
	auto iter = _motions.find(motion->getName());
	if (iter != _motions.end())
	{
		std::vector<SBSceneListener*>& listeners = SmartBody::SBScene::getScene()->getSceneListeners();
		for (auto & listener : listeners)
		{
			listener->OnObjectDelete(motion);
		}
		_motions.erase(iter);
	}
}

SBMotion* SBAssetManager::getMotion(const std::string& name)
{
	auto iter = _motions.find(name);
	if (iter != _motions.end())
		return (*iter).second.get();
	else
		return nullptr;
}

int SBAssetManager::getNumMotions()
{
	return _motions.size();
}

std::vector<std::string> SBAssetManager::getMotionNames()
{
	std::vector<std::string> ret;

	for(auto & _motion : _motions)
	{
		SBMotion* motion = _motion.second.get();
		ret.emplace_back(motion->getName());
	}

	return ret;
}

int SBAssetManager::getNumSkeletons()
{
	return _skeletons.size();
}


std::vector<std::string> SBAssetManager::getSkeletonNames()
{
	std::vector<std::string> ret;

	for(auto & _skeleton : _skeletons)
	{
		ret.emplace_back(_skeleton.first);
	}

	return ret;
}


FILE* SBAssetManager::open_sequence_file( const char *seq_name, std::string& fullPath ) {

	FILE* file_p = nullptr;

	char buffer[ MAX_FILENAME_LEN ];
	char label[ MAX_FILENAME_LEN ];
	// add the .seq extension if necessary
	std::string candidateSeqName = seq_name;
	if (candidateSeqName.find(".seq") == std::string::npos)
	{
		candidateSeqName.append(".seq");
	}
	sprintf( label, "%s", candidateSeqName.c_str());
	// current path containing .exe
	char CurrentPath[_MAX_PATH];
	_getcwd(CurrentPath, _MAX_PATH);

	auto seq_paths = _store.getPathList("seq");
	if (seq_paths) {

		seq_paths->reset();
		std::string filename = seq_paths->next_filename(buffer, candidateSeqName.c_str());
		//filename = mcn_return_full_filename_func( CurrentPath, filename );
		//SmartBody::util::log("seq name = %s, filename = %s\n",seq_name,filename.c_str());

		while (!filename.empty()) {
			file_p = fopen(filename.c_str(), "r");
			if (file_p != nullptr) {

				fullPath = filename;
				break;
			}
			filename = seq_paths->next_filename(buffer, candidateSeqName.c_str());
			//filename = mcn_return_full_filename_func( CurrentPath, filename );
		}
		if (file_p == nullptr) {
			// Could not find the file as named.  Perhap it excludes the extension
			sprintf(label, "%s.seq", seq_name);
			seq_paths->reset();
			filename = seq_paths->next_filename(buffer, candidateSeqName.c_str());
			//filename = mcn_return_full_filename_func( CurrentPath, filename );
			while (!filename.empty()) {
				if ((file_p = fopen(filename.c_str(), "r")) != nullptr) {

					fullPath = filename;
					break;
				}
				filename = seq_paths->next_filename(buffer, candidateSeqName.c_str());
				//filename = mcn_return_full_filename_func( CurrentPath, filename );
			}
		}
	}
	// return empty string if file not found
	return file_p;
}

SBAPI std::string SBAssetManager::getAssetNameVariation(SBAsset* asset)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

	int* counter = nullptr;
	auto* motion = dynamic_cast<SBMotion*>(asset);
	if (motion)
	{
		while (true)
		{
			_motionCounter++;
			std::stringstream strstr;
			strstr << motion->getName() << "xxx" << _motionCounter;
			if (!scene->getMotion(strstr.str()))
			{
				return strstr.str();
			}
		}
	}
	auto* skeleton = dynamic_cast<SBSkeleton*>(asset);
	if (skeleton)
	{
		while (true)
		{
			_skeletonCounter++;
			std::stringstream strstr;
			strstr << skeleton->getName() << _skeletonCounter;
			if (!scene->getSkeleton(strstr.str()))
			{
				return strstr.str();

			}
		}
	}


	return  "xxxxxxx";

}

std::string SBAssetManager::findFileName(const std::string& type, const std::string& filename) {
	return _store.findFileName(type, filename);
}

std::vector<std::string>& SBAssetManager::getAssetHistory() {
	return _history.getAssetHistory();
}

void SBAssetManager::clearAssetHistory() {
	_history.clearAssetHistory();
}

void SBAssetManager::addAssetHistory(const std::string& str) {
	_history.addAssetHistory(str);
}


}

