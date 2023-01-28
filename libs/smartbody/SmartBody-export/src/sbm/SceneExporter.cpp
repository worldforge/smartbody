/*
 Copyright (C) 2020 Erik Ogenvik
 Copyright (C) 2017 University of Southern California

 This file is part of Smartbody.

 SmartBody-lib is free software: you can redistribute it and/or
 modify it under the terms of the Lesser GNU General Public License
 as published by the Free Software Foundation, version 3 of the
 license.

 SmartBody-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 Lesser GNU General Public License for more details.

 You should have received a copy of the Lesser GNU General Public
 License along with SmarBody-lib.  If not, see:
     http://www.gnu.org/licenses/lgpl-3.0.txt
 */

#include "SceneExporter.h"
#include "sbm/SBRenderScene.h"
#include "sb/SBScene.h"
#include "sr/sr_camera.h"
#include "sb/SBAttribute.h"
#include "sb/SBCharacter.h"
#include "sb/SBRetargetManager.h"
#include "sb/SBRetarget.h"
#include "sb/SBAssetManager.h"
#include "sb/SBSkeleton.h"
#include "sb/SBSteerManager.h"
#include "sb/SBSteerAgent.h"
#include "sb/SBAnimationStateManager.h"
#include "sb/SBAnimationState.h"
#include "sb/SBJointMapManager.h"
#include "sb/SBPhonemeManager.h"
#include "sb/SBJointMap.h"
#include "sb/SBServiceManager.h"
#include "sb/SBPhoneme.h"
#include "sb/SBGestureMapManager.h"
#include "sb/SBAnimationTransition.h"
#include "sb/SBGestureMap.h"
#include "sb/SBRenderAssetManager.h"
#include "sb/SBSerializer.h"
#include "sbm/action_unit.hpp"
#include <sbm/ParserOpenCOLLADA.h>

#include <boost/filesystem.hpp>
#include <zip.h>
#include <boost/algorithm/string.hpp>

const std::string defautAssetPath = "Assets";

namespace {

/**
 * https://svn.boost.org/trac/boost/ticket/1976#comment:2
 * 
 * "The idea: uncomplete(/foo/new, /foo/bar) => ../new
 *  The use case for this is any time you get a full path (from an open dialog, perhaps)
 *  and want to store a relative path so that the group of files can be moved to a different
 *  directory without breaking the paths. An IDE would be a simple example, so that the
 *  project file could be safely checked out of subversion."
 * 
 * ALGORITHM:
 *  iterate path and base
 * compare all elements so far of path and base
 * whilst they are the same, no write to output
 * when they change, or one runs out:
 *   write to output, ../ times the number of remaining elements in base
 *   write to output, the remaining elements in path
 */
boost::filesystem::path naive_uncomplete(boost::filesystem::path const& p, boost::filesystem::path const& base) {
	using boost::filesystem::path;


	if (p == base)
		return "./";
	/*!! this breaks stuff if path is a filename rather than a directory,
		 which it most likely is... but then base shouldn't be a filename so... */

	path from_path, from_base, output;

	path::iterator path_it = p.begin(), path_end = p.end();
	path::iterator base_it = base.begin(), base_end = base.end();

	// check for emptiness
	if ((path_it == path_end) || (base_it == base_end))
		throw std::runtime_error("path or base was empty; couldn't generate relative path");

#ifdef WIN32
	// drive letters are different; don't generate a relative path
	if (*path_it != *base_it)
		return p;

	// now advance past drive letters; relative paths should only go up
	// to the root of the drive and not past it
	++path_it, ++base_it;
#endif

	// Cache system-dependent dot, double-dot and slash strings
	const std::string _dot = ".";//std::string(1, dot<path>::value);
	const std::string _dots = "..";//std::string(2, dot<path>::value);
	const std::string _sep = "/";//std::string(1, slash<path>::value);

	// iterate over path and base
	while (true) {

		// compare all elements so far of path and base to find greatest common root;
		// when elements of path and base differ, or run out:
		if ((path_it == path_end) || (base_it == base_end) || (*path_it != *base_it)) {

			// write to output, ../ times the number of remaining elements in base;
			// this is how far we've had to come down the tree from base to get to the common root
			for (; base_it != base_end; ++base_it) {
				if (*base_it == _dot)
					continue;
				else if (*base_it == _sep)
					continue;

				output /= "../";
			}

			// write to output, the remaining elements in path;
			// this is the path relative from the common root
			path::iterator path_it_start = path_it;
			for (; path_it != path_end; ++path_it) {

				if (path_it != path_it_start)
					output /= "/";

				if (*path_it == _dot)
					continue;
				if (*path_it == _sep)
					continue;

				output /= *path_it;
			}

			break;
		}

		// add directory level to both paths and continue iteration
		from_path /= path(*path_it);
		from_base /= path(*base_it);

		++path_it, ++base_it;
	}

	return output;
}

#if !defined(EMSCRIPTEN)

void writeFileToZip(zipFile& zf, const std::string& readFileName, const std::string& fileNameInZip) {
	//SmartBody::util::log("writeFileToZip, srcFile = %s, fileInZip = %s",readFileName.c_str(),fileNameInZip.c_str());
	const int bufferSize = 16384;
	int size_buf = bufferSize;
	void* buf = nullptr;
	buf = (void*) malloc(size_buf);
	zip_fileinfo zi;
	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour = zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
	zi.dosDate = 0;
	zi.internal_fa = 0;
	zi.external_fa = 0;
	FILE* fp = fopen(readFileName.c_str(), "rb");
	int err = zipOpenNewFileInZip(zf, fileNameInZip.c_str(), &zi, nullptr, 0, nullptr, 0, nullptr, 0, Z_DEFAULT_COMPRESSION);
	int size_read = 0;
	do {
		err = ZIP_OK;
		size_read = (int) fread(buf, 1, size_buf, fp);
		if (size_read > 0) {
			err = zipWriteInFileInZip(zf, buf, size_read);
		}

	} while ((err == ZIP_OK) && (size_read > 0));
	err = zipCloseFileInZip(zf);
	fclose(fp);
	free(buf);
}

void writeDirToZip(zipFile& zf, std::string sourceFolder, const std::string& folderNameInZip) {
	boost::filesystem::path source(sourceFolder);
	for (
			boost::filesystem::directory_iterator file(source);
			file != boost::filesystem::directory_iterator(); ++file
			) {
		try {
			boost::filesystem::path current(file->path());
			std::string curFileName = current.filename().string();
			if (boost::filesystem::is_directory(current)) {
				// Found directory: Recursion
				writeDirToZip(zf, current.string(), folderNameInZip + "/" + curFileName);
			} else {
				// Found file: Copy
				writeFileToZip(zf, current.string(), folderNameInZip + "/" + curFileName);
			}
		}
		catch (boost::filesystem::filesystem_error const& e) {
			std::cerr << e.what() << std::endl;
		}
	}
}


bool find_file(const boost::filesystem::path& dir_path, const std::string& file_name, boost::filesystem::path& path_found) {
	using boost::filesystem::path;
	using boost::filesystem::directory_iterator;

	if (!exists(dir_path)) return false;
	directory_iterator end_itr; // default construction yields past-the-end
	for (directory_iterator itr(dir_path);
		 itr != end_itr;
		 ++itr) {
		if (is_directory(itr->status())) {
			if (find_file(itr->path(), file_name, path_found)) return true;
		} else if (itr->path().filename() == file_name) // see below
		{
			path_found = itr->path();
			return true;
		}
	}
	return false;
}


bool copyDir(boost::filesystem::path const& source, boost::filesystem::path const& destination) {
	namespace fs = boost::filesystem;
	try {
		// Check whether the function call is valid
		if (
				!fs::exists(source) ||
				!fs::is_directory(source)
				) {
			std::cerr << "Source directory " << source.string()
					  << " does not exist or is not a directory." << std::endl;
			return false;
		}
		if (fs::exists(destination)) {
			// still try to overwrite ?
// 			std::cerr << "Destination directory " << destination.string()
// 				<< " already exists." << std::endl
// 				;
// 			return false;
		} else if (!fs::create_directories(destination)) // create destination dir
		{
			std::cerr << "Unable to create destination directory"
					  << destination.string() << std::endl;
			return false;
		}
	}
	catch (fs::filesystem_error const& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	// Iterate through the source directory
	for (
			fs::directory_iterator file(source);
			file != fs::directory_iterator(); ++file
			) {
		try {
			fs::path current(file->path());
			if (fs::is_directory(current)) {
				// Found directory: Recursion
				if (
						!copyDir(
								current,
								destination / current.filename()
						)
						) {
					return false;
				}
			} else if (!fs::exists(destination / current.filename())) {
				// Found file: Copy
				fs::copy_file(
						current,
						destination / current.filename(), fs::copy_option::none);
			}
		}
		catch (fs::filesystem_error const& e) {
			std::cerr << e.what() << std::endl;
		}
	}
	return true;
}


}

namespace SmartBody {
SBAPI void saveSceneSettings(SBRenderScene& renderScene, std::ostream& ostream) {
	auto& scene = renderScene.mScene;
	ostream << "# Autogenerated by SmartBody\n";
	// save all default cameras
	std::vector<std::string> cameras = renderScene.getCameraNames();
	for (auto& cameraIter : cameras) {
		if (cameraIter == "cameraDefault")
			continue; // don't save default camera
		SrCamera* camera = renderScene.getCamera(cameraIter);
		ostream << "obj = scene.getCamera(\"" << camera->getName() << "\")\n";
		ostream << "if obj == None:\n";
		ostream << "\tobj = scene.createCamera(\"" << camera->getName() << "\")\n";
		ostream << "obj.setEye(" << camera->getEye().x << ", " << camera->getEye().y << ", " << camera->getEye().z << ")\n";
		ostream << "obj.setCenter(" << camera->getCenter().x << ", " << camera->getCenter().y << ", " << camera->getCenter().z << ")\n";
		ostream << "obj.setUpVector(SrVec(" << camera->getUpVector().x << ", " << camera->getUpVector().y << ", " << camera->getUpVector().z << "))\n";
		ostream << "obj.setScale(" << camera->getScale() << ")\n";
		ostream << "obj.setFov(" << camera->getFov() << ")\n";
		ostream << "obj.setFarPlane(" << camera->getFarPlane() << ")\n";
		ostream << "obj.setNearPlane(" << camera->getNearPlane() << ")\n";
		ostream << "obj.setAspectRatio(" << camera->getAspectRatio() << ")\n";

		std::vector<std::string> attributeNames = camera->getAttributeNames();
		for (auto& attributeName : attributeNames) {
			SmartBody::SBAttribute* attr = camera->getAttribute(attributeName);
			if (!attr->isDefaultValue()) {
				std::string attrWrite = attr->write();
				ostream << attrWrite;
			}
		}
	}

	// save all pawns (including light pawns) position & orientation.
	const std::vector<std::string>& pawns = renderScene.mScene.getPawnNames();
	for (const auto& pawnIter : pawns) {
		SBPawn* pawn = scene.getPawn(pawnIter);
		auto* camera = dynamic_cast<SrCamera*>(pawn);
		if (camera)
			continue; // already wrote out pawns
		ostream << "\n# ---- pawn: " << pawn->getName() << "\n";
		ostream << "obj = scene.getPawn(\"" << pawn->getName() << "\")\n";
		ostream << "if obj == None:\n";
		ostream << "\tobj = scene.createPawn(\"" << pawn->getName() << "\")\n";
		SrVec position = pawn->getPosition();
		ostream << "obj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		SrVec hpr = pawn->getHPR();
		ostream << "obj.setHPR(SrVec(" << hpr[0] << ", " << hpr[1] << ", " << hpr[2] << "))\n";
		// attributes
		std::vector<std::string> attributeNames = pawn->getAttributeNames();
		for (auto& attributeName : attributeNames) {
			SmartBody::SBAttribute* attr = pawn->getAttribute(attributeName);
			if (!attr->isDefaultValue()) {
				std::string attrWrite = attr->write();
				ostream << attrWrite;
			}
		}
	}

	// restore all character position/orientation
	auto& characters = scene.getCharacterNames();
	for (const auto& characterIter : characters) {
		SBCharacter* character = scene.getCharacter(characterIter);
		ostream << "\n# ---- character: " << character->getName() << "\n";
		ostream << "obj = scene.getCharacter(\"" << character->getName() << "\")\n";
		ostream << "if obj != None:\n";
		SrVec position = character->getPosition();
		ostream << "\tobj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		SrVec hpr = character->getHPR();
		ostream << "\tobj.setHPR(SrVec(" << hpr[0] << ", " << hpr[1] << ", " << hpr[2] << "))\n";
	}

}

SBAPI void save(SBRenderScene& renderScene,
				SBSteerManager* steerManager,
				std::ostream& ostream,
				bool remoteSetup,
				const std::string& mediaPath) {
	ostream << "# Autogenerated by SmartBody\n";

	saveScene(renderScene, ostream, remoteSetup);
	saveAssets(renderScene, ostream, remoteSetup, mediaPath);
	saveCameras(renderScene, ostream, remoteSetup);
	saveLights(renderScene, ostream, remoteSetup);
	saveFaceDefinitions(renderScene, ostream, remoteSetup);
	//saveJointMaps(renderScene, ostream, remoteSetup); // need to apply joint map before apply any motion operations
	saveLipSyncing(renderScene, ostream, remoteSetup);
	saveBlends(renderScene, ostream, remoteSetup);
	saveRetargets(renderScene, ostream, remoteSetup);
	saveGestureMaps(renderScene, ostream, remoteSetup);
	savePawns(renderScene, ostream, remoteSetup);
	saveCharacters(renderScene, steerManager, ostream, remoteSetup);
	saveServices(renderScene, ostream, remoteSetup);

}

SBAPI void exportScene(SBRenderScene& renderScene,
					   SBSteerManager* steerManager,
					   std::ostream& ostream,
					   const std::vector<std::string>& aspects,
					   const std::string& mediaPath,
					   bool remoteSetup) {
	ostream << "# Autogenerated by SmartBody\n";

	std::set<std::string> set;
	for (const auto& aspect : aspects) {
		set.insert(aspect);
	}

	if (set.find("scene") != set.end())
		saveScene(renderScene, ostream, remoteSetup);
	if (set.find("assets") != set.end())
		saveAssets(renderScene, ostream, remoteSetup, "");
	if (set.find("cameras") != set.end())
		saveCameras(renderScene, ostream, remoteSetup);
	if (set.find("lights") != set.end())
		saveLights(renderScene, ostream, remoteSetup);
	if (set.find("face definitions") != set.end())
		saveFaceDefinitions(renderScene, ostream, remoteSetup);
	//if (set.find("joint maps") != set.end())
	//	saveJointMaps(ostream, remoteSetup);
	if (set.find("lip syncing") != set.end())
		saveLipSyncing(renderScene, ostream, remoteSetup);
	if (set.find("blends and transitions") != set.end())
		saveBlends(renderScene, ostream, remoteSetup);
	if (set.find("retargets") != set.end())
		saveRetargets(renderScene, ostream, remoteSetup);
	if (set.find("gesture maps") != set.end())
		saveGestureMaps(renderScene, ostream, remoteSetup);
	if (set.find("pawns") != set.end())
		savePawns(renderScene, ostream, remoteSetup);
	if (set.find("characters") != set.end())
		saveCharacters(renderScene, steerManager, ostream, remoteSetup);
	if (set.find("services") != set.end())
		saveServices(renderScene, ostream, remoteSetup);

}


SBAPI void saveScene(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup) {
	ostream << "# -------------------- cameras\n";
	ostream << "scene.setScale(" << renderScene.mScene.getScale() << ")\n";

	ostream << "# -------------------- scene\n";
	ostream << "obj = scene\n";
	// scene attributes
	std::vector<std::string> attributeNames = renderScene.mScene.getAttributeNames();
	for (auto& attributeName : attributeNames) {
		SmartBody::SBAttribute* attr = renderScene.mScene.getAttribute(attributeName);
		if (!attr->isDefaultValue()) {
			std::string attrWrite = attr->write();
			ostream << attrWrite;
		}
	}

}

void saveRetargets(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup) {
	ostream << "# -------------------- on-line retargeting setup\n";
	ostream << "print \"Save Retargets\"\n";
	if (remoteSetup) // don't save retarget instance for remote setup
		return;

	auto* retargetManager = renderScene.mScene.getRetargetManager();
	ostream << "retargetManager = scene.getRetargetManager()\n";
	auto retargetNames = retargetManager->getRetargetNames();
	for (auto& sp : retargetNames) {
		SmartBody::SBRetarget* retarget = retargetManager->getRetarget(sp.first, sp.second);
		ostream << "retarget = retargetManager.getRetarget(\"" << sp.first << "\",\"" << sp.second << "\")\n";
		ostream << "if retarget == None:\n";
		ostream << "\tretarget = retargetManager.createRetarget(\"" << sp.first << "\",\"" << sp.second << "\")\n";
		std::vector<std::string> endJoints = retarget->getEndJointNames();
		std::vector<std::string> relativeJoints = retarget->getRelativeJointNames();
		ostream << "endJoints = StringVec()\n";
		for (auto& endJoint : endJoints)
			ostream << "endJoints.append(\"" << endJoint << "\")\n";
		ostream << "relativeJoints = StringVec()\n";
		for (unsigned int k = 0; k < endJoints.size(); k++)
			ostream << "relativeJoints.append(\"" << relativeJoints[k] << "\")\n";

		ostream << "retarget.initRetarget(endJoints, relativeJoints)\n";
	}
}

SBAPI void exportScenePackage(SBRenderScene& renderScene, SBSteerManager* steerManager, const std::string& outDir, const std::string& outZipArchiveName) {
	std::string mediaPath = renderScene.mScene.getMediaPath();
	std::vector<std::string> motionNames = renderScene.mScene.getMotionNames();
	std::vector<std::string> skelNames = renderScene.mScene.getSkeletonNames();


	bool writeToZip = false;
	if (!outZipArchiveName.empty())
		writeToZip = true;

	boost::filesystem::path newOutPath(outDir);
	boost::filesystem::path mePath(mediaPath);
	boost::filesystem::path tempPath(mePath.parent_path().string() + "/tempExportFiles");
	if (!boost::filesystem::exists(newOutPath)) {
		boost::filesystem::create_directories(newOutPath);
	}

	if (!boost::filesystem::exists(tempPath)) {
		boost::filesystem::create_directories(tempPath);
	}

	std::string initScriptFile = "initScene.py";
	std::string scriptFileLocation = outDir + "/" + initScriptFile;
	std::ofstream file(scriptFileLocation.c_str());
	if (file.good()) {
		save(renderScene, steerManager, file, false, "__FILE__");
		file.close();
	} else {
		SmartBody::util::log("Fail to save scene. Abort export operation");
		file.close();
		return;
	}

	zipFile zf = nullptr;
	if (writeToZip) {
		zf = zipOpen((outDir + "/" + outZipArchiveName).c_str(), 0);
		writeFileToZip(zf, scriptFileLocation, initScriptFile);
	}

	auto& assetStore = renderScene.mScene.getAssetStore();
	SmartBody::SBAssetManager* assetManager = renderScene.mScene.getAssetManager();
	auto& renderAssetManager = renderScene._renderAssetManager;
	for (auto& motionName : motionNames) {
		SmartBody::SBMotion* motion = renderScene.mScene.getMotion(motionName);
		if (!motion) {
			SmartBody::util::log("Motion %s cannot be found.", motionName.c_str());
			continue;
		}
		if (motion->getTransformDepth() > 0) // not an original motion
			continue;

		if (motion->getFullFilePath().empty()) {
			std::string motionFullPath = tempPath.string() + "/" + motion->getName() + ".skm";
			SmartBody::util::log("Skeleton %s is not loaded from a file, save the skeleton to temp directory %s", motion->getName().c_str(), motionFullPath.c_str());
			SBSerializer::saveToSkm(*motion, motionFullPath);
			motion->setFullFilePath(motionFullPath);
		}

		boost::filesystem::path motionFile = boost::filesystem::path(motion->getFullFilePath());
		boost::filesystem::path motionPath = motionFile.parent_path();
		std::string motionFileExt = boost::filesystem::extension(motionFile);
		std::string assetLocation = assetStore.findAsset("motion", motion->getName() + motionFileExt);

		boost::filesystem::path diffPath;
		boost::filesystem::path outPath;
		diffPath = naive_uncomplete(motionPath, mePath);

		if (tempPath == motionPath) {
			diffPath = boost::filesystem::path(defautAssetPath);
		} else if (assetLocation.empty()) {
			boost::filesystem::path foundPath;
			if (!find_file(mePath, motionFile.filename().string(), foundPath)) // only set to defaultAssetPath if can not find the file under media directory
				diffPath = boost::filesystem::path(defautAssetPath);
		}
// 		else if (assetLocation != "")
// 		{
// 			boost::filesystem::path mePath(mediaPath);
// 			boost::filesystem::path assetPath(assetLocation);
// 			diffPath = naive_uncomplete(assetPath,mePath);
// 		}
		outPath = boost::filesystem::path(outDir + "/" + diffPath.string());

		if (!writeToZip) {
			if (!boost::filesystem::exists(outPath)) {
				boost::filesystem::create_directories(outPath);
			}
			std::string newFileName = outPath.string() + "/" + motionFile.filename().string();
			if (!boost::filesystem::exists(newFileName))
				boost::filesystem::copy_file(motionFile, boost::filesystem::path(newFileName), boost::filesystem::copy_option::none);
		} else {
			std::string zipFileName = diffPath.string() + "/" + motionFile.filename().string();
			writeFileToZip(zf, motionFile.string(), zipFileName);
		}
	}

	for (auto& skelName : skelNames) {
		auto skel = renderScene.mScene.getSkeleton(skelName);
		if (!skel) {
			SmartBody::util::log("Skeleton %s cannot be found.", skelName.c_str());
			continue;
		}

		if (skel->getFullFilePath().empty()) {
			std::string skelFullPath = tempPath.string() + "/" + skel->getName();
			SmartBody::util::log("Skeleton %s is not loaded from a file, save the skeleton to temp directory %s", skel->getName().c_str(), skelFullPath.c_str());
			skel->save(skelFullPath);
			skel->setFullFilePath(skelFullPath);
		}

		boost::filesystem::path skelFile = boost::filesystem::path(skel->getFullFilePath());
		boost::filesystem::path skelDir = skelFile.parent_path();
		std::string assetLocation = assetStore.findAsset("motion", skel->getName());
		boost::filesystem::path diffPath;
		boost::filesystem::path outPath;
		diffPath = naive_uncomplete(skelDir, mePath);
		//SmartBody::util::log("diffPath after uncomplete = '%s'",diffPath.string().c_str());
		if (tempPath == skelDir) {
			diffPath = boost::filesystem::path(defautAssetPath);
		} else if (assetLocation.empty()) {
			boost::filesystem::path foundPath;
			if (!find_file(mePath, skelFile.filename().string(), foundPath)) // only set to defaultAssetPath if can not find the file under media directory
				diffPath = boost::filesystem::path(defautAssetPath);
		}
// 		else if (assetLocation != "")
// 		{
// 			boost::filesystem::path mePath(mediaPath);
// 			boost::filesystem::path assetPath(assetLocation);
// 			diffPath = naive_uncomplete(assetPath,mePath);
// 		}
		//SmartBody::util::log("final diffpath = '%s'",diffPath.string().c_str());
		outPath = boost::filesystem::path(outDir + "/" + diffPath.string());
		//SmartBody::util::log("skeleton = %s, file = %s, outpath = %s", skel->getName().c_str(), skelFile.string().c_str(), outPath.string().c_str());
		if (!writeToZip) {
			if (!boost::filesystem::exists(outPath)) {
				boost::filesystem::create_directories(outPath);
			}
			std::string newFileName = outPath.string() + "/" + skelFile.filename().string();
			if (!boost::filesystem::exists(newFileName))
				boost::filesystem::copy_file(skelFile, boost::filesystem::path(newFileName), boost::filesystem::copy_option::none);
		} else {
			std::string zipFileName = diffPath.string() + "/" + skelFile.filename().string();
			writeFileToZip(zf, skelFile.string(), zipFileName);
		}
	}
	//SmartBody::util::log("finish copying skeleton files");
	std::vector<std::string> deformableMeshNames = renderAssetManager.getDeformableMeshNames();
	for (auto meshName : deformableMeshNames) {
		DeformableMesh* mesh = renderAssetManager.getDeformableMesh(meshName);
		if (!mesh) {
			SmartBody::util::log("Mesh %s cannot be found.", meshName.c_str());
			continue;
		}

		std::string meshBaseName = boost::filesystem::basename(meshName);

		if (mesh->getFullFilePath().empty()) // the mesh is not loaded from a file, so write it to a temp directory first
			//if (true)
		{
			std::vector<std::string> moNames;
			std::string meshSkName = mesh->skeletonName;
			std::string exportMeshFullPath = tempPath.string() + "/" + meshBaseName;
			boost::filesystem::path meshPath(exportMeshFullPath);
			if (!boost::filesystem::exists(meshPath)) {
				boost::filesystem::create_directories(meshPath);
			}
			SmartBody::util::log("mesh %s is not loaded from a file. Export the mesh to %s", meshName.c_str(), exportMeshFullPath.c_str());
			ParserOpenCOLLADA::exportCollada(renderAssetManager, exportMeshFullPath, meshSkName, meshName, moNames, true, true, false, 1.0);
			//SmartBody::util::log("finish exporting mesh as Collada files");
			mesh->setFullFilePath(exportMeshFullPath + "/" + meshName);
			//continue;
		}

		boost::filesystem::path meshFile = boost::filesystem::path(mesh->getFullFilePath());
		boost::filesystem::path meshDir = meshFile.parent_path();
		std::string assetLocation = assetStore.findAsset("mesh", meshName);
		boost::filesystem::path diffPath, outPath;

		diffPath = naive_uncomplete(meshDir, mePath);

		if (tempPath == meshDir.parent_path()) {
			diffPath = boost::filesystem::path(defautAssetPath + "/" + meshBaseName);
		} else if (assetLocation.empty()) {
			boost::filesystem::path foundPath;
			if (!find_file(mePath, meshFile.filename().string(), foundPath)) // only set to defaultAssetPath if can not find the file under media directory
				diffPath = boost::filesystem::path(defautAssetPath + "/" + meshBaseName);
		}

		outPath = boost::filesystem::path(outDir + "/" + diffPath.string());

		if (!writeToZip) {
			copyDir(meshDir, outPath);
		} else {
			writeDirToZip(zf, meshDir.string(), diffPath.string());
		}
	}
	//SmartBody::util::log("finish copying deformable mesh files");
	/*
	const std::vector<std::string>& characters = getCharacterNames();
	for (unsigned int i=0;i<characters.size();i++)
	{
		std::string charName = characters[i];
		SmartBody::SBCharacter* sbChar = getCharacter(charName);
		if (!sbChar) continue;

		std::string meshName = sbChar->getStringAttribute("deformableMesh");
		if (meshName == "") // otherwise we will copy all mesh directories ...
			continue;
		boost::filesystem::path path(meshName);
		std::string meshDir = boost::filesystem::basename(path);

		std::vector<std::string> meshPaths = SmartBody::SBScene::getScene()->getAssetManager()->getAssetPaths("mesh");
		bool hasMeshPath = false;
		for (size_t m = 0; m < meshPaths.size(); m++)
		{
			std::string meshPath = meshPaths[m];
			boost::filesystem::path curpath( meshPath + "/" + meshDir );
			if (!boost::filesystem::is_directory(curpath))
			{

				// should we write out the mesh instead ?
				//continue;
			}
			//curpath /= std::string(meshDir);

			boost::filesystem::path mePath(mediaPath);
			boost::filesystem::path diffPath = naive_uncomplete(curpath,mePath);
			boost::filesystem::path newMeshPath(outDir+"/"+diffPath.string());

			//SmartBody::util::log("curPath = %s, newMeshPath = %s", curpath.string().c_str(), newMeshPath.string().c_str());
			//path targetPath()
			// copy dir
			hasMeshPath = true;
			if (!writeToZip)
			{
				copyDir(curpath,newMeshPath);
			}
			else
			{
				writeDirToZip(zf,curpath.string(),diffPath.string());
			}
		}
	}
	*/

#if 0
	// save motions
	//SmartBody::util::log("Num of motionNames = %d",motionNames.size());
	for (unsigned int i=0;i<motionNames.size();i++)
	{
		SmartBody::SBMotion* motion = getMotion(motionNames[i]);
		//SmartBody::util::log("motionName = %s",motionNames[i].c_str());

		if (!motion)
		{
			SmartBody::util::log("Motion %s cannot be found.", motionNames[i].c_str());
			continue;
		}

		boost::filesystem::path motionFile(motion->filename());
		boost::filesystem::path motionPath = motionFile.parent_path();
		//SmartBody::util::log("motion %d, motionName = %s, filename = %s",motionNames[i].c_str(), motion->filename().c_str());
		if (motionPath.empty()) // don't care about empty path
			continue;
		if (motion->getTransformDepth() > 0) // not an original motion
			continue;

		boost::filesystem::path mePath(mediaPath);
		boost::filesystem::path diffPath = naive_uncomplete(motionPath,mePath);

		if (diffPath.is_absolute()) // the target path is not under mePath
		{
			diffPath = boost::filesystem::path("Assets");
		}
		boost::filesystem::path newPath(outDir+"/"+diffPath.string());

		//SmartBody::util::log("motionpath = %s, mediapath = %s, diffpath = %s, filename = %s", motionFile.directory_string().c_str(), mePath.native_directory_string().c_str(), diffPath.string().c_str(), motionFile.filename().c_str());
		//SmartBody::util::log("new Path = %s, newFileName = %s",newPath.string().c_str(), newFileName.c_str());
		//motion->saveToSkm(newFileName);
		if (!writeToZip)
		{
			if (!boost::filesystem::exists(newPath))
			{
				boost::filesystem::create_directories(newPath);
			}
#if (BOOST_VERSION > 104400)
			std::string newFileName = newPath.string()+"/"+motionFile.filename().string();
#else
			std::string newFileName = newPath.string()+"/"+motionFile.filename();
#endif
			boost::filesystem::copy_file(motionFile,boost::filesystem::path(newFileName), boost::filesystem::copy_option::none);
		}
else
		{
#if (BOOST_VERSION > 104400)
			std::string zipFileName = diffPath.string()+"/"+motionFile.filename().string();
#else
			std::string zipFileName = diffPath.string()+"/"+motionFile.filename();
#endif
			writeFileToZip(zf, motionFile.string(), zipFileName);
		}
	}

	// save skeletons
	for (unsigned int i=0;i<skelNames.size();i++)
	{
		auto skel = getSkeleton(skelNames[i]);
		boost::filesystem::path skelFile(skel->getFileName());
		boost::filesystem::path skelPath = skelFile.parent_path();
		if (skelPath.empty()) // don't care about empty path
			continue;

		boost::filesystem::path mePath(mediaPath);
		boost::filesystem::path diffPath = naive_uncomplete(skelPath,mePath);
		if (diffPath.is_absolute()) // the target path is not under mePath
		{
			diffPath = boost::filesystem::path("Assets");
		}
		boost::filesystem::path newPath(outDir+"/"+diffPath.string());

		  //SmartBody::util::log("motionpath = %s, mediapath = %s, diffpath = %s", skelPath.directory_string().c_str(), mePath.directory_string().c_str(), diffPath.directory_string().c_str());
		//skel->save(newFileName);
		//copy_file(path(skelFile),path(newFileName), boost::filesystem::copy_option::none);

		if (!writeToZip)
		{
			if (!boost::filesystem::exists(newPath))
			{
				boost::filesystem::create_directories(newPath);
			}
#if (BOOST_VERSION > 104400)
			std::string newFileName = newPath.string()+"/"+skelFile.filename().string();
#else
			std::string newFileName = newPath.string()+"/"+skelFile.filename();
#endif

			boost::filesystem::copy_file(skelFile,boost::filesystem::path(newFileName), boost::filesystem::copy_option::none);
		}
		else
		{
#if (BOOST_VERSION > 104400)
			std::string zipFileName = diffPath.string()+"/"+skelFile.filename().string();
#else
			std::string zipFileName = diffPath.string()+"/"+skelFile.filename();
#endif
			writeFileToZip(zf, skelFile.string(), zipFileName);
		}
	}

#if 1
	// save character meshes
	const std::vector<std::string>& characters = getCharacterNames();
	for (unsigned int i=0;i<characters.size();i++)
	{
		std::string charName = characters[i];
		SmartBody::SBCharacter* sbChar = getCharacter(charName);
		if (!sbChar) continue;

		std::string meshName = sbChar->getStringAttribute("deformableMesh");
		if (meshName == "") // otherwise we will copy all mesh directories ...
			continue;
		boost::filesystem::path path(meshName);
		std::string meshDir = boost::filesystem::basename(path);

		std::vector<std::string> meshPaths = SmartBody::SBScene::getScene()->getAssetManager()->getAssetPaths("mesh");
		bool hasMeshPath = false;
		for (size_t m = 0; m < meshPaths.size(); m++)
		{
			std::string meshPath = meshPaths[m];
			boost::filesystem::path curpath( meshPath + "/" + meshDir );
			if (!boost::filesystem::is_directory(curpath))
			{

				// should we write out the mesh instead ?
				//continue;
			}
			//curpath /= std::string(meshDir);

			boost::filesystem::path mePath(mediaPath);
			boost::filesystem::path diffPath = naive_uncomplete(curpath,mePath);
			boost::filesystem::path newMeshPath(outDir+"/"+diffPath.string());

			//SmartBody::util::log("curPath = %s, newMeshPath = %s", curpath.string().c_str(), newMeshPath.string().c_str());
			//path targetPath()
			// copy dir
			hasMeshPath = true;
			if (!writeToZip)
			{
				copyDir(curpath,newMeshPath);
			}
			else
			{
				writeDirToZip(zf,curpath.string(),diffPath.string());
			}
		}
	}
#else
	std::vector<std::string> meshNames = assetManager->getDeformableMeshNames();
	for (unsigned int i=0;i<meshNames.size();i++)
	{
		std::string meshName = meshNames[i];
		DeformableMesh* mesh = assetManager->getDeformableMesh(meshName);
		std::vector<std::string> meshPaths = SmartBody::SBScene::getScene()->getAssetManager()->getAssetPaths("mesh");
		bool hasMeshPath = false;
		boost::filesystem::path curpath, curMeshPath;
		for (size_t m = 0; m < meshPaths.size(); m++)
		{
			std::string meshPath = meshPaths[m];
			curpath = boost::filesystem::path( meshPath + "/" + meshName );
			if (boost::filesystem::is_directory(curpath))
			{
				curMeshPath = curpath;
				hasMeshPath = true;
				break;
			}
		}

		if (!hasMeshPath) // file does not exist
		{
			curMeshPath = boost::filesystem::path(getMediaPath()+ "/temp/" + meshName); // create a temp directory to write out the mesh
			boost::filesystem::create_directory(curMeshPath);
			ParserOgre::exportOgreXMLMesh(mesh, meshName, curMeshPath.string());
		}

		boost::filesystem::path mePath(mediaPath);
		boost::filesystem::path diffPath = naive_uncomplete(curpath,mePath);
		boost::filesystem::path newMeshPath(outDir+"/"+diffPath.string());

		if (!writeToZip)
		{
			copyDir(curMeshPath,newMeshPath);
		}
		else
		{
			writeDirToZip(zf,curMeshPath.string(),diffPath.string());
		}
	}
#endif
#endif

	if (writeToZip) {
		int errClose = zipClose(zf, nullptr);
		boost::filesystem::path initScrptPath(scriptFileLocation);
		if (boost::filesystem::exists(initScrptPath)) {
			boost::filesystem::remove(initScrptPath);
		}
	}

	if (boost::filesystem::exists(tempPath)) {
		boost::filesystem::remove_all(tempPath);
	}
}

#endif

void saveAssets(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup, const std::string& outMediaPath) {
	ostream << "# -------------------- media and asset paths\n";
	// mediapath
	ostream << "print \"Save Assets\"\n";
	std::string systemMediaPath = renderScene.mScene.getMediaPath();
	if (outMediaPath == "__FILE__") {
		ostream << "import inspect\n";
		ostream << "scriptFilename = inspect.getframeinfo(inspect.currentframe()).filename\n";
		ostream << "fileMediaPath = os.path.dirname(os.path.abspath(scriptFilename))\n";
		ostream << "scene.setMediaPath(fileMediaPath)\n";
	} else if (!outMediaPath.empty()) {
		ostream << "scene.setMediaPath(\"" << outMediaPath << "\")\n";
	} else {
		ostream << "scene.setMediaPath(\"" << systemMediaPath << "\")\n";
	}

	// asset paths
	std::vector<std::string>::iterator iter;
	std::vector<std::string> motionPaths = renderScene.mScene.getLocalAssetPaths("motion");
	std::vector<std::string> motionNames = renderScene.mScene.getMotionNames();
	std::vector<std::string> skelNames = renderScene.mScene.getSkeletonNames();
	std::set<std::string> extraAssetPathSet;

	//exportScenePackage("../../../../temp/","test1.zip");

	SmartBody::SBAssetManager* assetManager = renderScene.mScene.getAssetManager();
	auto& assetStore = renderScene.mScene.getAssetStore();
	// feng : since we may have use "loadAssetsFromPath" to load the motions, we should infer all other motion paths from existing motions
	for (auto& motionName : motionNames) {
		SmartBody::SBMotion* motion = renderScene.mScene.getMotion(motionName);
		boost::filesystem::path motionFile(motion->filename());
		boost::filesystem::path motionPath = motionFile.parent_path();
		if (motionPath.empty()) // don't care about empty path
			continue;
		std::string ext = boost::filesystem::extension(motionFile);
		std::string base = boost::filesystem::basename(motionFile);
		std::string assetName = base + ext;
		if (remoteSetup) {
			continue;
		}
		std::string assetExist = assetStore.findAsset("motion", assetName);
		boost::filesystem::path mePath(systemMediaPath);
		boost::filesystem::path diffPath = naive_uncomplete(motionPath, mePath);
		if (diffPath.is_absolute()) // the path is not under media path, skip
		{
			continue;
		}
		//SmartBody::util::log("motionpath = %s, mediapath = %s, diffpath = %s", motionFile.directory_string().c_str(), mePath.string().c_str(), diffPath.string().c_str());
		auto st = std::find(motionPaths.begin(), motionPaths.end(), diffPath.string());
		//if (st == motionPaths.end())
		if (assetExist.empty()) // can not find this asset in the motion path
		{
			extraAssetPathSet.insert(diffPath.string());
		}
	}

	for (auto& skelName : skelNames) {
		auto skel = renderScene.mScene.getSkeleton(skelName);
		boost::filesystem::path skelFile(skel->getFileName());
		boost::filesystem::path skelPath = skelFile.parent_path();
		if (skelPath.empty()) // don't care about empty path
			continue;

		if (remoteSetup) {
			continue;
		}

		std::string ext = boost::filesystem::extension(skelFile);
		std::string base = boost::filesystem::basename(skelFile);
		std::string assetName = base + ext;
		std::string assetExist = assetStore.findAsset("motion", assetName);
		boost::filesystem::path mePath(systemMediaPath);
		boost::filesystem::path diffPath = naive_uncomplete(skelPath, mePath);
		//SmartBody::util::log("motionpath = %s, mediapath = %s, diffpath = %s", skelPath.directory_string().c_str(), mePath.directory_string().c_str(), diffPath.directory_string().c_str());
		if (diffPath.is_absolute()) // the path is not under media path, skip
		{
			continue;
		}
		auto st = std::find(motionPaths.begin(), motionPaths.end(), diffPath.string());
		//if (st == motionPaths.end())
		if (assetExist.empty()) // can not find this asset in the skeleton path
		{
			extraAssetPathSet.insert(diffPath.string());
		}
	}

	for (iter = motionPaths.begin(); iter != motionPaths.end(); iter++) {
		const std::string& path = (*iter);
		ostream << R"(scene.addAssetPath("motion", ")" << path << "\")\n";
	}

	std::vector<std::string> scriptPaths = renderScene.mScene.getLocalAssetPaths("script");
	for (iter = scriptPaths.begin(); iter != scriptPaths.end(); iter++) {
		const std::string& path = (*iter);
		ostream << R"(scene.addAssetPath("script", ")" << path << "\")\n";
	}

	std::vector<std::string> audioPaths = renderScene.mScene.getLocalAssetPaths("audio");
	for (iter = audioPaths.begin(); iter != audioPaths.end(); iter++) {
		const std::string& path = (*iter);
		ostream << R"(scene.addAssetPath("audio", ")" << path << "\")\n";
	}

	std::vector<std::string> meshPaths = renderScene.mScene.getLocalAssetPaths("mesh");
	for (iter = meshPaths.begin(); iter != meshPaths.end(); iter++) {
		const std::string& path = (*iter);
		ostream << R"(scene.addAssetPath("mesh", ")" << path << "\")\n";
	}
	if (remoteSetup) // to-do : different treatment when saving setup script for remote connection
	{
		// need to go through all skeleton, and explicitly create those skeletons from script
		std::vector<std::string> skeletonNames = renderScene.mScene.getSkeletonNames();
		const std::vector<std::string>& charNames = renderScene.mScene.getCharacterNames();
		std::map<std::string, std::string> charSkelMap;

		for (const auto& skelName : skeletonNames) {
			auto skel = renderScene.mScene.getSkeleton(skelName);
			if (skel) {
				std::string skelStr = skel->saveToString();
				ostream << "tempSkel = scene.addSkeletonDefinition(\"" << skelName << "\")\n";
				std::string skelSaveStr = skel->saveToString();
				//skelSaveStr.replace('\n',)
				boost::replace_all(skelSaveStr, "\n", "\\n");
				boost::replace_all(skelSaveStr, "\"", "");
				SmartBody::util::log("Skeleton %s :\n%s", skelName.c_str(), skelSaveStr.c_str());
				ostream << "tempSkel.loadFromString(\"" << skelSaveStr << "\")\n";
				charSkelMap[skelName] = skelName;
			}
		}

		for (const auto& charName : charNames) {
			SBCharacter* sbChar = renderScene.mScene.getCharacter(charName);
			if (!sbChar)
				continue;
			std::string skelName = sbChar->getSkeleton()->getName();
			auto skel = renderScene.mScene.getSkeleton(skelName);
			if (skel && charSkelMap.find(skelName) == charSkelMap.end()) {
				std::string skelStr = skel->saveToString();
				ostream << "tempSkel = scene.addSkeletonDefinition(\"" << skelName << "\")\n";
				std::string skelSaveStr = skel->saveToString();
				//skelSaveStr.replace('\n',)
				boost::replace_all(skelSaveStr, "\n", "\\n");
				boost::replace_all(skelSaveStr, "\"", "");
				//SmartBody::util::log("Skeleton %s :\n%s",skelName.c_str(),skelSaveStr.c_str());
				SmartBody::util::log("Skeleton %s :\n", skelName.c_str());
				ostream << "tempSkel.loadFromString(\"" << skelSaveStr << "\")\n";
				charSkelMap[skelName] = charName;
			}
		}

		for (const auto& moName : motionNames) {
			SBMotion* motion = renderScene.mScene.getMotion(moName);
			if (motion) {
				// add motion definition
				ostream << "tempMotion = scene.createMotion(\"" << moName << "\")\n";
				ostream << "tempMotion.setEmptyMotion(" << motion->getDuration() << ", " << motion->getNumFrames() << ")\n";
				// add sync points
				ostream << "tempMotion.setSyncPoint(\"start\"," << motion->getTimeStart() << ")\n";
				ostream << "tempMotion.setSyncPoint(\"ready\"," << motion->getTimeReady() << ")\n";
				ostream << "tempMotion.setSyncPoint(\"stroke_start\"," << motion->getTimeStrokeStart() << ")\n";
				ostream << "tempMotion.setSyncPoint(\"stroke\"," << motion->getTimeStroke() << ")\n";
				ostream << "tempMotion.setSyncPoint(\"stroke_stop\"," << motion->getTimeStrokeEnd() << ")\n";
				ostream << "tempMotion.setSyncPoint(\"relax\"," << motion->getTimeRelax() << ")\n";
				ostream << "tempMotion.setSyncPoint(\"stop\"," << motion->getTimeStop() << ")\n";
				// add meta data tag
				for (auto& tagName : motion->getMetaDataTags()) {
					ostream << "tempMotion.addMetaData(\"" << tagName << "\",\"" << motion->getMetaDataString(tagName) << "\")\n";
				}
			}
		}
	} else // loading the actual assets
	{
		std::set<std::string>::iterator si;
		for (si = extraAssetPathSet.begin();
			 si != extraAssetPathSet.end();
			 si++) {
			const std::string& path = (*si);
			ostream << "scene.loadAssetsFromPath(\"" << path << "\")\n";
		}
		// load the extra assets in asset directory
		ostream << "scene.loadAssetsFromPath(\"" << defautAssetPath << "\")\n";
		ostream << "# -------------------- load assets\n";
		ostream << "scene.loadAssets()\n";
	}

	// this is a hack to save joint map here, should probably move motion transformation into another function ?
	saveJointMaps(renderScene, ostream, remoteSetup);
	// create any mirrored assets
	std::vector<std::string> motions = renderScene.mScene.getMotionNames();
#if 0
	for (std::vector<std::string>::iterator iter = motions.begin();
		 iter != motions.end();
		 iter++)
	{
		SBMotion* mirroredMotion = renderScene.mScene.getMotion(*iter);
		StringAttribute* mirroredMotionAttr = dynamic_cast<StringAttribute*>(mirroredMotion->getAttribute("mirrorMotion"));
		if (mirroredMotionAttr) // the motion is generated from mirroring
		{
			ostream << "motion = scene.getMotion(\"" << mirroredMotionAttr->getValue() << "\")\n";
			// make sure the skeleton exists
			StringAttribute* mirroredSkeletonAttr = dynamic_cast<StringAttribute*>(mirroredMotion->getAttribute("mirrorSkeleton"));
			if (mirroredSkeletonAttr)
			{
				const std::string& skeletonName = mirroredSkeletonAttr->getValue();
				ostream << "mirrorSkeleton = scene.getSkeleton(\"" << skeletonName << "\")\n";
				ostream << "if mirrorSkeleton is not None:\n";
				ostream << "\tmirroredMotion = motion.mirror(\"" << mirroredMotion->getName() << "\", \"" << skeletonName << "\")\n";
			}
		}
	}
#else
	std::vector<SmartBody::SBMotion*> motionList;
	motionList.reserve(motions.size());
	for (auto& motion : motions)
		motionList.emplace_back(renderScene.mScene.getMotion(motion));
	// sort motion array according to its transformation depth level
	std::sort(motionList.begin(), motionList.end(), SmartBody::motionComp);
	// process motion from low depth to high depth to capture all dependency of motion operations

	for (auto& motion : motionList) {
		StringAttribute* mirroredMotionAttr = dynamic_cast<StringAttribute*>(motion->getAttribute("mirrorMotion"));
		StringAttribute* smoothedMotionAttr = dynamic_cast<StringAttribute*>(motion->getAttribute("smoothMotion"));
		if (mirroredMotionAttr) {
			ostream << "motion = scene.getMotion(\"" << mirroredMotionAttr->getValue() << "\")\n";
			// make sure the skeleton exists
			StringAttribute* mirroredSkeletonAttr = dynamic_cast<StringAttribute*>(motion->getAttribute("mirrorSkeleton"));
			if (mirroredSkeletonAttr) {
				const std::string& skeletonName = mirroredSkeletonAttr->getValue();
				ostream << "mirrorSkeleton = scene.getSkeleton(\"" << skeletonName << "\")\n";
				ostream << "if mirrorSkeleton is not None:\n";
				ostream << "\tmirroredMotion = motion.mirror(\"" << motion->getName() << "\", \"" << skeletonName << "\")\n";
			}
		} else if (smoothedMotionAttr) {
			ostream << "motion = scene.getMotion(\"" << smoothedMotionAttr->getValue() << "\")\n";
			DoubleAttribute* smoothIntervalAttr = dynamic_cast<DoubleAttribute*>(motion->getAttribute("smoothInterval"));
			if (smoothIntervalAttr) {
				ostream << "smoothMotion = motion.smoothCycle(\"" << motion->getName() << "\", " << std::to_string(smoothIntervalAttr->getValue()) << ")\n";
			}
		}
	}

	for (auto& skName : skelNames) {
		auto sbSk = renderScene.mScene.getSkeleton(skName);
		// rescale the skeleton to right size if necessary
		ostream << "sbSk = scene.getSkeleton(\"" << skName << "\")\n";
		//ostream << "print 'numSkeleton = ' + str(scene.getNumSkeletons())\n";
		ostream << "if sbSk != None:\n";
		ostream << "\tsbSk.rescale(" << std::to_string(sbSk->getScale()) << ")\n";
	}

#endif

	for (auto& motionName : motions) {
		SBMotion* motion = renderScene.mScene.getMotion(motionName);
		ostream << "tempMotion = scene.getMotion(\"" << motion << "\")\n";
		ostream << "tempMotion.setMotionSkeletonName(\"" << motion->getMotionSkeletonName() << "\")\n";
	}


}

void saveCameras(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup) {
	ostream << "# -------------------- cameras\n";
	ostream << "print \"Save Cameras\"\n";
	// save all default cameras
	std::vector<std::string> cameras = renderScene.getCameraNames();
	for (auto& cameraIter : cameras) {
		SrCamera* camera = renderScene.getCamera(cameraIter);
		ostream << "obj = scene.getCamera(\"" << camera->getName() << "\")\n";
		ostream << "if obj == None:\n";
		ostream << "\tobj = scene.createCamera(\"" << camera->getName() << "\")\n";
		ostream << "obj.setEye(" << camera->getEye().x << ", " << camera->getEye().y << ", " << camera->getEye().z << ")\n";
		ostream << "obj.setCenter(" << camera->getCenter().x << ", " << camera->getCenter().y << ", " << camera->getCenter().z << ")\n";
		ostream << "obj.setUpVector(SrVec(" << camera->getUpVector().x << ", " << camera->getUpVector().y << ", " << camera->getUpVector().z << "))\n";
		ostream << "obj.setScale(" << camera->getScale() << ")\n";
		ostream << "obj.setFov(" << camera->getFov() << ")\n";
		ostream << "obj.setFarPlane(" << camera->getFarPlane() << ")\n";
		ostream << "obj.setNearPlane(" << camera->getNearPlane() << ")\n";
		ostream << "obj.setAspectRatio(" << camera->getAspectRatio() << ")\n";

		std::vector<std::string> attributeNames = camera->getAttributeNames();
		for (auto& attributeName : attributeNames) {
			SmartBody::SBAttribute* attr = camera->getAttribute(attributeName);
			if (!attr->isDefaultValue()) {
				std::string attrWrite = attr->write();
				ostream << attrWrite;
			}
		}
	}
}

void savePawns(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup) {
	ostream << "# -------------------- pawns and characters\n";
	ostream << "print \"Save Pawns\"\n";
	// pawns
	const std::vector<std::string>& pawns = renderScene.mScene.getPawnNames();
	for (const auto& pawnIter : pawns) {
		SBPawn* pawn = renderScene.mScene.getPawn(pawnIter);
		auto* camera = dynamic_cast<SrCamera*>(pawn);
		auto* sbChar = dynamic_cast<SmartBody::SBCharacter*>(pawn);
		if (sbChar) // we will handle character in saveCharacters
			continue;
		if (camera)
			continue; // already wrote out pawns
		if (pawn->getName().find("light") == 0) {
			// already wrote lights
			continue;
		}
		ostream << "\n# ---- pawn: " << pawn->getName() << "\n";
		ostream << "obj = scene.getPawn(\"" << pawn->getName() << "\")\n";
		ostream << "if obj == None:\n";
		ostream << "\tobj = scene.createPawn(\"" << pawn->getName() << "\")\n";
		SrVec position = pawn->getPosition();
		ostream << "obj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		SrQuat orientation = pawn->getOrientation();
		ostream << "obj.setOrientation(SrQuat(" << orientation.w << ", " << orientation.x << ", " << orientation.y << ", " << orientation.z << "))\n";
		// attributes
		std::vector<std::string> attributeNames = pawn->getAttributeNames();
		for (auto& attributeName : attributeNames) {
			SmartBody::SBAttribute* attr = pawn->getAttribute(attributeName);
			if (!attr->isDefaultValue()) {
				std::string attrWrite = attr->write();
				ostream << attrWrite;
			}
		}
	}
}

void savePositions(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup) {
	ostream << "# -------------------- pawn positions\n";
	ostream << "print \"Save Positions\"\n";
	const std::vector<std::string>& pawns = renderScene.mScene.getPawnNames();
	for (const auto& pawnIter : pawns) {
		SBPawn* pawn = renderScene.mScene.getPawn(pawnIter);
		auto* camera = dynamic_cast<SrCamera*>(pawn);
		if (camera)
			continue; // already wrote out pawns
		if (pawn->getName().find("light") == 0) {
			// already wrote lights
			continue;
		}
		ostream << "obj = scene.getPawn(\"" << pawn->getName() << "\")\n";
		SrVec position = pawn->getPosition();
		SrQuat orientation = pawn->getOrientation();
		ostream << "if obj is not None:\n";
		ostream << "\tobj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		ostream << "\tobj.setOrientation(SrQuat(" << orientation.w << ", " << orientation.x << ", " << orientation.y << ". " << orientation.z << "))\n";
	}

	ostream << "# -------------------- character positions\n";
	const std::vector<std::string>& characters = renderScene.mScene.getCharacterNames();
	for (const auto& characterIter : characters) {
		SBCharacter* character = renderScene.mScene.getCharacter(characterIter);
		SrVec position = character->getPosition();
		SrQuat orientation = character->getOrientation();
		ostream << "obj = scene.getCharacter(\"" << character->getName() << "\", \"" << character->getType() << "\")\n";
		ostream << "if obj is not None:\n";
		ostream << "\tobj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		ostream << "\tobj.setOrientation(SrQuat(" << orientation.w << ", " << orientation.x << ", " << orientation.y << ". " << orientation.z << "))\n";
	}
}

void saveCharacters(SBRenderScene& renderScene, SBSteerManager* steerManager, std::ostream& ostream, bool remoteSetup) {
	// characters
	ostream << "print \"Save Characters\"\n";
	auto& characters = renderScene.mScene.getCharacterNames();
	for (const auto& characterIter : characters) {
		auto* character = renderScene.mScene.getCharacter(characterIter);
		ostream << "\n# ---- character: " << character->getName() << "\n";
		ostream << "obj = scene.getCharacter(\"" << character->getName() << "\")\n";
		ostream << "if obj == None:\n";
		ostream << "\tobj = scene.createCharacter(\"" << character->getName() << "\", \"" << character->getType() << "\")\n";
		ostream << "print 'character skeleton rescale'\n";
		ostream << "skeleton = scene.createSkeleton(\"" << character->getSkeleton()->getName() << "\")\n";
		ostream << "skeleton.rescale(" << character->getSkeleton()->getScale() << ")\n";
		ostream << "obj.setSkeleton(skeleton)\n";
		SrVec position = character->getPosition();
		ostream << "obj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		SrQuat orientation = character->getOrientation();
		ostream << "obj.setOrientation(SrQuat(" << orientation.w << ", " << orientation.x << ", " << orientation.y << ", " << orientation.z << "))\n";

		// face definition
		SBFaceDefinition* faceDef = character->getFaceDefinition();
		if (faceDef)
			ostream << "obj.setFaceDefinition(scene.getFaceDefinition(\"" << faceDef->getName() << "\"))\n";

		// controllers
		ostream << "obj.createStandardControllers()\n";
		// attributes
		std::vector<std::string> attributeNames = character->getAttributeNames();
		for (auto& attributeName : attributeNames) {
			SmartBody::SBAttribute* attr = character->getAttribute(attributeName);
			if (!attr->isDefaultValue()) {
				std::string attrWrite = attr->write();
				ostream << attrWrite;
			}
		}

		// reach
		ostream << "# -------------------- reaching for character " << character->getName() << "\n";
		// reach
		SBReachManager* reachManager = renderScene.mScene.getReachManager();
		SBReach* reach = character->getReach();
		if (reach) {
			ostream << "reach = scene.getReachManager().createReach(\"" << character->getName() << "\")\n";
			std::string interpType = reach->getInterpolatorType();
			ostream << "reach.setInterpolatorType(\"" << interpType << "\")\n";
			// motions
			const std::vector<std::string>& leftMotionNames = reach->getMotionNames("left");
			for (const auto& leftMotionName : leftMotionNames) {
				ostream << R"(reach.addMotion("left", scene.getMotion(")" << leftMotionName << "\"))\n";
			}
			const std::vector<std::string>& rightMotionNames = reach->getMotionNames("right");
			for (const auto& rightMotionName : rightMotionNames) {
				ostream << R"(reach.addMotion("right", scene.getMotion(")" << rightMotionName << "\"))\n";
			}
			// point hand
			SBMotion* m = nullptr;
			m = reach->getPointHandMotion("left");
			if (m) {
				ostream << R"(reach.setPointHandMotion("left", scene.getMotion(")" << m->getName() << "\"))\n";
			}
			m = reach->getPointHandMotion("right");
			if (m) {
				ostream << R"(reach.setPointHandMotion("right", scene.getMotion(")" << m->getName() << "\"))\n";
			}
			// grab hand
			m = reach->getGrabHandMotion("left");
			if (m) {
				ostream << R"(reach.setGrabHandMotion("left", scene.getMotion(")" << m->getName() << "\"))\n";
			}
			m = reach->getGrabHandMotion("right");
			if (m) {
				ostream << R"(reach.setGrabHandMotion("right", scene.getMotion(")" << m->getName() << "\"))\n";
			}
			// release hand
			m = reach->getReleaseHandMotion("left");
			if (m) {
				ostream << R"(reach.setReleaseHandMotion("left", scene.getMotion(")" << m->getName() << "\"))\n";
			}
			m = reach->getReleaseHandMotion("right");
			if (m) {
				ostream << R"(reach.setReleaseHandMotion("right", scene.getMotion(")" << m->getName() << "\"))\n";
			}
			// reach hand
			m = reach->getReachHandMotion("left");
			if (m) {
				ostream << R"(reach.setReachHandMotion("left", scene.getMotion(")" << m->getName() << "\"))\n";
			}
			m = reach->getReachHandMotion("right");
			if (m) {
				ostream << R"(reach.setReachHandMotion("right", scene.getMotion(")" << m->getName() << "\"))\n";
			}
			ostream << "reach.build(scene.getCharacter(\"" << character->getName() << "\"))\n";
		} else {
			ostream << "# -- no reach\n";
		}

		if (steerManager) {
			// steering
			ostream << "# --- steering for character " << character->getName() << "\n";
			SBSteerAgent* steerAgent = steerManager->getSteerAgent(character->getName());
			if (steerAgent) {
				ostream << "steeragent = scene.getSteerManager().getSteerAgent(\"" << character->getName() << "\")\n";
				ostream << "if steeragent is None:\n";
				ostream << "\tsteeragent = scene.getSteerManager().createSteerAgent(\"" << character->getName() << "\")\n";
				ostream << "\tsteeragent.setSteerStateNamePrefix(\"" << steerAgent->getSteerStateNamePrefix() << "\")\n";
				ostream << "\tsteeragent.setSteerType(\"" << steerAgent->getSteerType() << "\")\n";
			} else {
				ostream << "# --- no steering for character " << character->getName() << "\n";
			}
		}
		// set character posture
		ostream << "bml.execBML('" << character->getName() << "', '<body posture=\"" << character->getPostureName() << "\"/>')\n";
	}

	// enable steering
	if (steerManager && !steerManager->getSteerAgentNames().empty()) {
		ostream << "scene.getServiceManager().getService(\"steering\").setBoolAttribute(\"enable\", True)\n";
	}
}

void saveLights(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup) {
	ostream << "# -------------------- lights\n";
	ostream << "print \"Save Lights\"\n";
	// lights
	const std::vector<std::string>& pawns = renderScene.mScene.getPawnNames();
	for (const auto& pawnIter : pawns) {
		SBPawn* pawn = renderScene.mScene.getPawn(pawnIter);
		auto* camera = dynamic_cast<SrCamera*>(pawn);
		if (camera)
			continue;
		if (pawn->getName().find("light") != 0) {
			continue;
		}
		ostream << "\n# ---- light: " << pawn->getName() << "\n";
		ostream << "obj = scene.getPawn(\"" << pawn->getName() << "\")\n";
		ostream << "if obj == None:\n";
		ostream << "\tobj = scene.createPawn(\"" << pawn->getName() << "\")\n";
		SrVec position = pawn->getPosition();
		ostream << "obj.setPosition(SrVec(" << position[0] << ", " << position[1] << ", " << position[2] << "))\n";
		SrQuat orientation = pawn->getOrientation();
		ostream << "obj.setOrientation(SrQuat(" << orientation.w << ", " << orientation.x << ", " << orientation.y << ", " << orientation.z << "))\n";

		// attributes
		std::vector<std::string> attributeNames = pawn->getAttributeNames();
		for (auto& attributeName : attributeNames) {
			SmartBody::SBAttribute* attr = pawn->getAttribute(attributeName);
			if (true) // !attr->isDefaultValue())
			{
				std::string attrWrite = attr->write();
				ostream << attrWrite;
			}
		}
	}
}

void saveBlends(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup) {
	ostream << "# -------------------- blends and transitions\n";
	ostream << "print \"Save Blends\"\n";
	// blends & transitions
	auto* blendManager = renderScene.mScene.getBlendManager();
	std::vector<std::string> blends = blendManager->getBlendNames();
	for (auto& blendIter : blends) {
		auto* blend = blendManager->getBlend(blendIter);
		std::string blendString = blend->saveToString();
		ostream << blendString;
		ostream << "\n";
	}

	int numTransitions = blendManager->getNumTransitions();
	std::vector<std::string> transitions = blendManager->getBlendNames();
	for (int t = 0; t < numTransitions; t++) {
		auto* transition = blendManager->getTransitionByIndex(t);
		std::string transitionString = transition->saveToString();
		ostream << transitionString;
		ostream << "\n";
	}
}

void saveJointMaps(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup) {
	auto& scene = renderScene.mScene;
	ostream << "# -------------------- joint maps\n";
	ostream << "print \"Save Joint Maps\"\n";
	// joint maps
	SBJointMapManager* jointMapManager = renderScene.mScene.getJointMapManager();
	std::vector<std::string> jointMapNames = jointMapManager->getJointMapNames();
	for (auto& jointMapName : jointMapNames) {
		SBJointMap* jointMap = jointMapManager->getJointMap(jointMapName);

		ostream << "jointMap = scene.getJointMapManager().getJointMap(\"" << jointMapName << "\")\n";
		ostream << "if jointMap == None:\n";
		ostream << "\tjointMap = scene.getJointMapManager().createJointMap(\"" << jointMapName << "\")\n";

		int numMappings = jointMap->getNumMappings();
		for (int m = 0; m < numMappings; m++) {
			std::string target = jointMap->getTarget(m);
			std::string source = jointMap->getSource(m);
			ostream << "jointMap.setMapping(\"" << source << "\", \"" << target << "\")\n";
		}
	}

	ostream << "# -------------------- applying joint maps\n";
	std::vector<std::string> mappedMotions = scene.getMotionNames();
	std::vector<std::string> mappedSkeletons = scene.getSkeletonNames();

	for (auto& jointMapName : jointMapNames) {
		SBJointMap* jointMap = jointMapManager->getJointMap(jointMapName);

		ostream << "jointMap = scene.getJointMapManager().getJointMap(\"" << jointMapName << "\")\n";


#if 0
		std::vector<std::string>& mappedMotions = jointMap->getMappedMotions();
		for (std::vector<std::string>::iterator iter = mappedMotions.begin();
			 iter != mappedMotions.end();
			 iter++)
		{
			ostream << "jointMap.applyMotion(scene.getMotion(\"" << (*iter) << "\"))\n";
		}

		std::vector<std::string>& mappedSkeletons = jointMap->getMappedSkeletons();
		for (std::vector<std::string>::iterator iter = mappedSkeletons.begin();
			 iter != mappedSkeletons.end();
			 iter++)
		{
			ostream << "jointMap.applySkeleton(scene.getSkeleton(\"" << (*iter) << "\"))\n";
		}
#else
		for (auto& mappedMotion : mappedMotions) {
			SmartBody::SBMotion* motion = scene.getMotion(mappedMotion);
			//FIXME: keep joint map reference in motion
//			if (motion && motion->channels().getJointMapName() == jointMapName)
//				ostream << "jointMap.applyMotion(scene.getMotion(\"" << mappedMotion << "\"))\n";
		}

		for (auto& mappedSkeleton : mappedSkeletons) {
			//FIXME: keep joint map reference in skeleton
//			auto skel = scene->getSkeleton(mappedSkeleton);
//			if (skel && skel->getJointMapName() == jointMapName)
//				ostream << "jointMap.applySkeleton(scene.getSkeleton(\"" << mappedSkeleton << "\"))\n";
		}
#endif
	}
}

void saveFaceDefinitions(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup) {
	ostream << "# -------------------- face definitions\n";
	ostream << "print \"Save Face Definitions\"\n";
	// face definitions
	std::vector<std::string> faceDefinitions = renderScene.mScene.getFaceDefinitionNames();
	for (auto& faceDefName : faceDefinitions) {
		SmartBody::SBFaceDefinition* faceDef = renderScene.mScene.getFaceDefinition(faceDefName);
		std::string faceDefinitionName = "face";
		faceDefinitionName.append(faceDef->getName());
		ostream << faceDefinitionName << " = scene.getFaceDefinition(\"" << faceDefName << "\")\n";
		ostream << "if " << faceDefinitionName << " == None:\n";
		ostream << "\t" << faceDefinitionName << " = scene.createFaceDefinition(\"" << faceDefName << "\")\n";

		SkMotion* neutral = faceDef->getFaceNeutral();
		if (neutral) {
			ostream << faceDefinitionName << ".setFaceNeutral(\"" << neutral->getName() << "\")\n";
		}

		std::vector<std::string> visemeNames = faceDef->getVisemeNames();
		for (auto& viseme : visemeNames) {
			ostream << faceDefinitionName << ".setViseme(\"" << viseme << "\", ";
			SkMotion* visemeMotion = faceDef->getVisemeMotion(viseme);
			if (visemeMotion) {
				ostream << "\"" + visemeMotion->getName() + "\")\n";
			} else {
				ostream << "\"\")\n";
			}
		}

		std::vector<int> auNum = faceDef->getAUNumbers();
		for (int& auIter : auNum) {
			int num = auIter;
			ActionUnit* au = faceDef->getAU(auIter);
			ostream << faceDefinitionName << ".setAU(" << num << ", ";
			SkMotion* motion = nullptr;
			if (au->is_bilateral()) {
				ostream << R"("both", ")";
				motion = au->left;
			} else {

				if (au->is_left()) {
					ostream << R"("left", ")";
					motion = au->left;
				} else {
					ostream << R"("right", ")";
					motion = au->right;
				}
			}
			if (motion) {
				ostream << motion->getName();
			}
			ostream << "\")\n";

		}
	}
}

void saveGestureMaps(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup) {
	ostream << "# -------------------- gesture maps\n";
	ostream << "print \"Save Gesture Maps\"\n";
	auto gestureMapNames = renderScene.mScene.getGestureMapManager()->getGestureMapNames();
	for (const auto& gestureMapName : gestureMapNames) {
		auto* gestureMap = renderScene.mScene.getGestureMapManager()->getGestureMap(gestureMapName);
		ostream << "gestureMap = scene.getGestureMapManager().createGestureMap(\"" << gestureMapName << "\")\n";
		int numMappings = gestureMap->getNumMappings();
		for (int m = 0; m < numMappings; m++) {
			auto& info = gestureMap->getGestureByIndex(m);
			ostream << "gestureMap.addGestureMapping(\"" << info._animation << "\", \"" << info._lexeme << "\", \"" << info._type << "\", \""
					<< info._hand << "\", \"" << info._style << "\", \"" << info._posture << "\")\n";

		}
	}

}

void saveLipSyncing(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup) {
	ostream << "# -------------------- lip syncing\n";
	ostream << "print \"Save Lip Syncing\"\n";
	// diphones
	SBPhonemeManager* diphoneManager = renderScene.mScene.getDiphoneManager();
	std::vector<std::string> diphoneMapNames = diphoneManager->getDiphoneMapNames();
	ostream << "diphoneMapManager = scene.getDiphoneManager()\n";
	for (auto& diphoneMapName : diphoneMapNames) {
		auto& diphones = diphoneManager->getDiphones(diphoneMapName);
		for (auto& diphone : diphones) {
			const std::string& fromPhoneme = diphone->getFromPhonemeName();
			const std::string& toPhoneme = diphone->getToPhonemeName();
			ostream << "diphone = diphoneMapManager.createDiphone(\"" << fromPhoneme << "\", \"" << toPhoneme << "\", \"" << diphoneMapName << "\")\n";
			std::vector<std::string> visemes = diphone->getVisemeNames();
			for (auto& viseme : visemes) {
				std::vector<float>& keys = diphone->getKeys(viseme);
				for (size_t x = 0; x < keys.size(); x++) {
					if (x + 1 < keys.size()) {
						ostream << "diphone.addKey(\"" << viseme << "\", " << keys[x] << ", " << keys[x + 1] << ")\n";
					}
					x++;
				}
				ostream << "\n";
			}
		}
	}
}

void saveServices(SBRenderScene& renderScene, std::ostream& ostream, bool remoteSetup) {
	ostream << "# -------------------- Services\n";
	// services
	SmartBody::SBServiceManager* serviceManager = renderScene.mScene.getServiceManager();
	auto serviceNames = serviceManager->getServiceNames();
	for (auto& serviceName : serviceNames) {
		auto* service = serviceManager->getService(serviceName);
		ostream << "obj = scene.getServiceManager().getService(\"" << service->getName() << "\")\n";
		std::vector<std::string> attributeNames = service->getAttributeNames();
		for (auto& attributeName : attributeNames) {
			SmartBody::SBAttribute* attr = service->getAttribute(attributeName);
			if (!attr->isDefaultValue()) {
				std::string attrWrite = attr->write();
				ostream << attrWrite;
			}
		}

	}

}

void exportCharacter(SBRenderScene& renderScene, const std::string& charName, std::string outDir) {
	SmartBody::SBCharacter* sbChar = renderScene.mScene.getCharacter(charName);
	if (!sbChar) return;
	boost::filesystem::path newOutPath(outDir);
	if (!boost::filesystem::exists(newOutPath)) {
		boost::filesystem::create_directories(newOutPath);
	}
	std::vector<std::string> motions;
	SrVec scale = sbChar->getVec3Attribute("deformableMeshScale");
	ParserOpenCOLLADA::exportCollada(renderScene._renderAssetManager, outDir, sbChar->getSkeleton()->getName(), sbChar->getStringAttribute("deformableMesh"), motions, true, true, false, scale.x);

}


}
