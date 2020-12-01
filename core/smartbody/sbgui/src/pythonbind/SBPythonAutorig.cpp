
#include "SBPythonAutoRig.h"
#include "SBInterfaceListener.h"
#include <sb/SBObject.h>
#include <sb/SBScene.h>
#include <sb/SBAssetManager.h>
#include <sb/SBPawn.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <sbm/ParserOpenCOLLADA.h>
#include <autorig/SBAutoRigManager.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <vector>
#include <string>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/args.hpp>
#include <boost/python.hpp>

#include <RootWindow.h>
#include <fltk_viewer.h>

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"





struct SBInterfaceListenerWrap : SBInterfaceListener, boost::python::wrapper<SBInterfaceListener>
{
	virtual void onStart()
	{
		if (boost::python::override o = this->get_override("onStart"))
		{
			try {
				o();
			} catch (...) {
				PyErr_Print();
			}
		}

		SBInterfaceListener::onStart();
	};

	void default_onStart()
	{
		SBInterfaceListener::onStart();
	}

	virtual bool onMouseClick(int x, int y, int button)
	{
		if (boost::python::override o = this->get_override("onMouseClick"))
		{
			try {
				return o(x, y, button);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBInterfaceListener::onMouseClick(x, y, button);
	};

	bool default_onMouseClick(int x, int y, int button)
	{
		return SBInterfaceListener::onMouseClick(x, y, button);
	}

	virtual bool onMouseMove(int x, int y)
	{
		if (boost::python::override o = this->get_override("onMouseMove"))
		{
			try {
				return o(x, y);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBInterfaceListener::onMouseMove(x, y);
	};

	bool default_onMouseMove(int x, int y)
	{
		return SBInterfaceListener::onMouseMove(x, y);
	}

	virtual bool onMouseRelease(int x, int y, int button)
	{
		if (boost::python::override o = this->get_override("onMouseRelease"))
		{
			try {
				return o(x, y);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBInterfaceListener::onMouseRelease(x, y, button);
	};

	bool default_onMouseRelease(int x, int y, int button)
	{
		return SBInterfaceListener::onMouseRelease(x, y, button);
	}

	virtual bool onMouseDrag(int x, int y)
	{
		if (boost::python::override o = this->get_override("onMouseDrag"))
		{
			try {
				return o(x, y);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBInterfaceListener::onMouseDrag(x, y);
	};

	bool default_onMouseDrag(int x, int y)
	{
		return SBInterfaceListener::onMouseDrag(x, y);
	}

	virtual bool onKeyboardPress(char c)
	{
		if (boost::python::override o = this->get_override("onKeyboardPress"))
		{
			try {
				return o(c);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBInterfaceListener::onKeyboardPress(c);
	};

	bool default_onKeyboardPress(char c)
	{
		return SBInterfaceListener::onKeyboardPress(c);
	}

	virtual bool onKeyboardRelease(char c)
	{
		if (boost::python::override o = this->get_override("onKeyboardRelease"))
		{
			try {
				return o(c);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBInterfaceListener::onKeyboardRelease(c);
	};

	bool default_onKeyboardRelease(char c)
	{
		return SBInterfaceListener::onKeyboardRelease(c);
	}

	virtual void onEnd()
	{
		if (boost::python::override o = this->get_override("onEnd"))
		{
			try {
				o();
			} catch (...) {
				PyErr_Print();
			}
		}

		SBInterfaceListener::onEnd();
	};

	void default_onEnd()
	{
		SBInterfaceListener::onEnd();
	}

};




void setPawnMesh(const std::string& pawnName, const std::string& meshName, SrVec meshScale)
{
	auto& renderAssetManager = Session::current->renderAssetManager;
	auto& renderScene = Session::current->renderScene;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBAssetManager* assetManager = scene->getAssetManager();
	auto renderable = renderScene.getRenderable(pawnName);
	if (!renderable || !renderable->pawn)
		return;

	DeformableMesh* mesh = renderAssetManager.getDeformableMesh(meshName);
	if (!mesh)
	{
		return;
	}
	if (mesh)
	{
		renderable->staticMeshInstance = std::make_unique<SbmDeformableMeshGPUInstance>();
		renderable->staticMeshInstance->setToStaticMesh(true);
		renderable->staticMeshInstance->setDeformableMesh(mesh);
		//meshInsance->setSkeleton(pawn->getSkeleton());	
		renderable->staticMeshInstance->setPawn(renderable->pawn);
		renderable->staticMeshInstance->setMeshScale(meshScale);
	}
}

void saveDeformableMesh(const std::string& meshName, const std::string& skelName, const std::string& outDir)
{
	auto& renderAssetManager = Session::current->renderAssetManager;
	std::vector<std::string> moNames;
	double scale = 1.0;
	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(skelName);
	if (character)
	{
		SrVec scale3 = character->getVec3Attribute("deformableMeshScale");
		scale = scale3.x;
	}

	ParserOpenCOLLADA::exportCollada(renderAssetManager, outDir, skelName, meshName, moNames, true, true, false, scale);
}

void saveDeformableMeshScale(const std::string& meshName, const std::string& skelName, const std::string& outDir, float meshScale)
{
	auto& renderAssetManager = Session::current->renderAssetManager;
	std::vector<std::string> moNames;
	double scale = meshScale;
	ParserOpenCOLLADA::exportCollada(renderAssetManager, outDir, skelName, meshName, moNames, true, true, false, scale);
}

inline float clamp(float x, float a, float b) { return std::max(a, std::min(b, x)); }
inline float saturate(float x) { return clamp(x, 0.f, 1.f); }

void lab2rgb(float* lab, unsigned char* rgb, unsigned int size)
{
	float temp1[16] = { 1, 1, 1, 0,
					   1, 1, -1, 0,
					   1, -2, 0, 0,
					   0, 0,  0, 0 };
	float temp2[16] = { sqrt(3.f) / 3.f, 0, 0, 0,
						0, sqrt(6.f) / 6.f, 0 , 0,
						0, 0, sqrt(2.f) / 2.f , 0,
						0, 0,  0,  0};
	SrMat m1_lab2rgb = SrMat(temp1);
	SrMat m2_lab2rgb = SrMat(temp2);

	float aLMS2RGB[16] = { 4.4679f, -3.5873f, 0.1193f , 0,
						 -1.2186f, +2.3809f, -0.1624f, 0,
						  0.0497f, -0.2439f, 1.2045f , 0,
							0,        0,        0,     0};
	SrMat mLMS2RGB = SrMat(aLMS2RGB);

	m1_lab2rgb.transpose();
	m2_lab2rgb.transpose();
	mLMS2RGB.transpose();

	for (unsigned int i = 0; i < size; i = i + 4)
	{
		float l = lab[i];
		float a = lab[i + 1];
		float b = lab[i + 2];
		SrVec lab(l, a, b);

		//Convert to LMS
		//SrVec LMS = m1_lab2rgb * m2_lab2rgb * lab;
		SrVec LMS = lab * m2_lab2rgb * m1_lab2rgb;
		LMS = SrVec(pow(10, LMS.x), pow(10, LMS.y), pow(10, LMS.z));//pow(SrVec(10), LMS);
		//Convert to RGB
		//auto rgbv = mLMS2RGB * LMS;
		auto rgbv = LMS * mLMS2RGB;
		rgb[i] = static_cast<unsigned char>(255.*saturate(rgbv.x));
		rgb[i + 1] = static_cast<unsigned char>(255.*saturate(rgbv.y));
		rgb[i + 2] = static_cast<unsigned char>(255.*saturate(rgbv.z));
	}
}

void rgb2lab(unsigned char* rgb, float* lab, unsigned int size)
{
	float temp1[16] = { 1.f / sqrt(3.f) , 0, 0, 0,  
					   0, 1.f / sqrt(6.f), 0 , 0, 
					   0, 0, 1.f / sqrt(2.f), 0,
					   0, 0, 0, 0};
	float temp2[16] = { 1, 1, 1, 0, 
						1, 1, -2, 0,
						1, -1, 0, 0,
						0, 0, 0, 0};

	SrMat m1_rgb2lab = SrMat(temp1);
	SrMat m2_rgb2lab = SrMat(temp2);

	

	float aRGB2LMS[16] = { 0.3811f, 0.5783f, 0.0402f, 0, 
						  0.1967f, 0.7244f, 0.0782f, 0,
						  0.0241f, 0.1288f, 0.8444f, 0,
						  0,       0,       0,       0};
	SrMat tmRGB2LMS = SrMat(aRGB2LMS);

	m1_rgb2lab.transpose();
	m2_rgb2lab.transpose();
	tmRGB2LMS.transpose();
	for (unsigned int i = 0; i < size; i = i + 4)
	{
		// log10(0) = -inf, so gotta use FLT_TRUE_MIN
		// otherwise, image stats are fucked up
		float r = std::max(std::numeric_limits<float>::min(), rgb[i] / 255.f);
		float g = std::max(std::numeric_limits<float>::min(), rgb[i + 1] / 255.f);
		float b = std::max(std::numeric_limits<float>::min(), rgb[i + 2] / 255.f);
		SrVec rgb(r, g, b);

		//Convert to LMS
		//SrVec LMS = tmRGB2LMS * rgb;
		SrVec LMS = rgb * tmRGB2LMS;
		LMS = SrVec(log10(LMS.x), log10(LMS.y), log10(LMS.z));

		//Convert to lab
		//SrVec labv = m1_rgb2lab * m2_rgb2lab  * LMS;
		SrVec labv = LMS * m2_rgb2lab * m1_rgb2lab;

		lab[i] = labv.x;
		lab[i + 1] = labv.y;
		lab[i + 2] = labv.z;
	}
}

void computeImageMeanAndStd(float* lab, unsigned char* mask, int imgSize, SrVec& outMean, SrVec& outStd)
{
	int pixCount = 0;
	SrVec imgMean = SrVec(0,0,0), imgStd = SrVec(0,0,0);
	for (unsigned int i = 0; i < imgSize; i++)
	{
		int labIdx = i * 4;
		if (mask[i] == 0) continue;
		SrVec pix = SrVec(lab[labIdx], lab[labIdx + 1], lab[labIdx + 2]);
		//SmartBody::util::log("Pix %d = %s", i, pix.toString().c_str());
		imgMean += pix;
		pixCount++;
	}

	SmartBody::util::log("Image Pix Count = %d", pixCount);
	imgMean /= pixCount;

	for (unsigned int i = 0; i < imgSize; i++)
	{
		int labIdx = i * 4;
		if (mask[i] == 0) continue;
		for (int k=0;k<3;k++)
			imgStd[k] += pow(lab[labIdx+k] - imgMean[k], 2);
	}

	for (int k = 0; k < 3; k++)
		imgStd[k] = sqrtf(imgStd[k] / pixCount);
	outMean = imgMean;
	outStd = imgStd;

	SmartBody::util::log("Image Mean = %s, Std = %s", outMean.toString().c_str(), outStd.toString().c_str());
}

void deformableMeshTextureReplace(const std::string& meshName, const std::string& textureName, std::string inputImageFileName)
{
	auto& renderAssetManager = Session::current->renderAssetManager;
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	DeformableMesh* mesh = renderAssetManager.getDeformableMesh(meshName);
	if (!mesh)
	{
		SmartBody::util::log("Error replacing texture '%s', mesh '%s' doesn't exist.", textureName.c_str(), meshName.c_str());
		return;
	}

	bool meshTextureExist = false;
	std::string finalTextureName;
	for (auto subMesh : mesh->subMeshList)
	{
			//if (subMesh->texName == textureName)
		//	meshTextureExist = true;
		if (subMesh->texName.find(textureName) != std::string::npos) // texture name exists in the submesh
		{
			finalTextureName = subMesh->texName;
			meshTextureExist = true;
			break;
		}
	}
	if (!meshTextureExist)
	{
		SmartBody::util::log("Error replacing texture '%s', texture doesn't exist in mesh '%s'.", textureName.c_str(), meshName.c_str());
		return;
	}

	SmartBody::util::log("Found texture '%s' in the deformable mesh '%s'.", finalTextureName.c_str(), meshName.c_str());
	// replace textures with new image files
	auto texture = SbmTextureManager::singleton().findTexture(finalTextureName.c_str());
	if (!texture)
	{
		SmartBody::util::log("Error replacing texture '%s', texture doesn't exist.", finalTextureName.c_str(), meshName.c_str());
		return;
	}

	texture->loadImage(inputImageFileName.c_str());
	texture->buildTexture();
}

void imageColorTransfer(std::string srcImg, std::string srcMask, std::string tgtImg, std::string tgtMask, std::string outImage)
{
	int srcWidth, srcHeight, srcChannel;
	int forceImgChannel = 4;
	int forceMaskChannel = 1;
	unsigned char* srcBuf = stbi_load(srcImg.c_str(), &srcWidth, &srcHeight, &srcChannel, forceImgChannel);
	unsigned char* srcMaskBuf = stbi_load(srcMask.c_str(), &srcWidth, &srcHeight, &srcChannel, forceMaskChannel);

	int tgtWidth, tgtHeight, tgtChannel;
	unsigned char* tgtBuf = stbi_load(tgtImg.c_str(), &tgtWidth, &tgtHeight, &tgtChannel, forceImgChannel);
	unsigned char* tgtMaskBuf = stbi_load(tgtMask.c_str(), &tgtWidth, &tgtHeight, &tgtChannel, forceMaskChannel);

	int srcSize = srcHeight*srcWidth;
	int tgtSize = tgtHeight*tgtWidth;
	float* srcLab = new float[srcSize*forceImgChannel];
	float* tgtLab = new float[tgtSize*forceImgChannel];

	rgb2lab(srcBuf, srcLab, srcSize*forceImgChannel);
	rgb2lab(tgtBuf, tgtLab, tgtSize*forceImgChannel);

	SrVec srcMean, srcStd, tgtMean, tgtStd;
	computeImageMeanAndStd(srcLab, srcMaskBuf, srcSize, srcMean, srcStd);
	computeImageMeanAndStd(tgtLab, tgtMaskBuf, tgtSize, tgtMean, tgtStd);

	SrVec newStdRato;
	for (int k = 0; k < 3; k++)
		newStdRato[k] = tgtStd[k] / srcStd[k];
	for (int i = 0; i < srcSize; i ++)
	{
		if (srcMaskBuf[i] == 0) continue;
		int labIdx = i * 4;
		for (int k = 0; k < 3; k++)
		{
			// adjust source image in LAB space based on target image's Mean and Std
			srcLab[labIdx + k] = (srcLab[labIdx + k] - srcMean[k])*newStdRato[k] + tgtMean[k];
		}
	}
	lab2rgb(srcLab, srcBuf, srcSize*forceImgChannel);
	int imageWriteSuccess = stbi_write_png(outImage.c_str(), srcWidth, srcHeight, 4, srcBuf, srcWidth*4);
	SmartBody::util::log("Writing PNG %s, result = %d", outImage.c_str(), imageWriteSuccess);
}

void addModelToMesh(std::string templateMeshName, std::string modelFile, std::string newModelName, std::string rigidBindJoint, std::string bindPoseCopySubMeshName)
{
	auto& renderAssetManager = Session::current->renderAssetManager;
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	DeformableMesh* mesh = renderAssetManager.getDeformableMesh(templateMeshName);
	if (!mesh)
	{
		SmartBody::util::log("Cannot find template mesh %s in order to add mesh via file %s.", templateMeshName.c_str(), modelFile.c_str());
		return;
	}

	// load the new model
	SrModel model;
	bool ok = model.import_ply(modelFile.c_str());
	if (!ok)
	{
		SmartBody::util::log("Cannot find new model %s to add to mesh %s.", modelFile.c_str(), templateMeshName.c_str());
		return;
	}

	SrSnModel* modelSrSn = new SrSnModel();
	modelSrSn->shape(model);
	modelSrSn->shape().name = newModelName.c_str();
	modelSrSn->ref();

	// does a mesh with this name already exist? If so, replace it
	bool found = false;
	for (size_t m = 0; m < mesh->dMeshStatic_p.size(); m++)
	{
		SrSnModel* staticModel = mesh->dMeshStatic_p[m];
		SrModel& s = staticModel->shape();
		std::string curShapeName = (const char*) s.name;
		if (curShapeName == newModelName)
		{
			// model already exists, replace it
			SrSnModel* curModel = mesh->dMeshStatic_p[m];
			curModel->unref();
			mesh->dMeshStatic_p[m] = modelSrSn;
			mesh->dMeshStatic_p[m]->changed(true);
			SrSnModel* curDynamicModel = mesh->dMeshDynamic_p[m];
			curDynamicModel->unref();
			mesh->dMeshDynamic_p[m] = modelSrSn;
			mesh->dMeshDynamic_p[m]->changed(true);
			found = true;
		}

	}

	if (!found)
	{
		mesh->dMeshStatic_p.emplace_back(modelSrSn);
		mesh->dMeshDynamic_p.emplace_back(modelSrSn);
	}

	if (mesh->skinWeights.size() == 0)
	{
		SmartBody::util::log("Mesh %s has no skin weights, model %s will be added but not skinned.", templateMeshName.c_str(), modelFile.c_str());
		return;
	}
	
	// copy the skin weights from the proper mesh using a joint, bind matrix
	SkinWeight* existingSkinWeights = mesh->skinWeights[0];
	
	SkinWeight* modelSkin = new SkinWeight();
	
	SrMat existingBindShapeMat = existingSkinWeights->bindShapeMat;
	SrMat modelBindPose;
	std::string jointName = rigidBindJoint;

	for (unsigned int i = 0; i < existingSkinWeights->infJointName.size(); i++)
	{
		if (existingSkinWeights->infJointName[i] == jointName)
		{
			modelBindPose = existingSkinWeights->bindPoseMat[i];
			break;
		}
	}

	modelSkin->bindShapeMat = existingBindShapeMat;
	modelSkin->bindPoseMat.emplace_back(modelBindPose);
	modelSkin->infJointName.emplace_back(rigidBindJoint);
	modelSkin->sourceMesh = newModelName;
	modelSkin->bindWeight.emplace_back(1.0f);
	for (unsigned int i = 0; i < model.V.size(); i++)
	{
		modelSkin->jointNameIndex.emplace_back(0);
		modelSkin->numInfJoints.emplace_back(1);
		modelSkin->weightIndex.emplace_back(0);
	}
	mesh->skinWeights.emplace_back(modelSkin);
}

void addBlendshapeToModel(std::string templateMeshName, std::string modelFile, std::string shapeName, std::string submeshName)
{
	auto& renderAssetManager = Session::current->renderAssetManager;
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	DeformableMesh* mesh = renderAssetManager.getDeformableMesh(templateMeshName);
	if (!mesh)
	{
		SmartBody::util::log("Cannot find template mesh %s in order to add blendshape via file %s.", templateMeshName.c_str(), modelFile.c_str());
		return;
	}

	// load the new model
	SrModel blendshapeModel;
	bool ok = blendshapeModel.import_ply(modelFile.c_str());
	if (!ok)
	{
		SmartBody::util::log("Cannot find new model %s to add as blendshape to mesh %s.", modelFile.c_str(), templateMeshName.c_str());
		return;
	}

	for (size_t m = 0; m < mesh->dMeshStatic_p.size(); m++)
	{
		SrSnModel* staticModel = mesh->dMeshStatic_p[m];
		SrModel& s = staticModel->shape();
		std::string curShapeName = (const char*) s.name;
		if (curShapeName == submeshName)
		{
			// make sure that the blendshape matches the existing model
			if (s.V.size() != blendshapeModel.V.size())
			{
				SmartBody::util::log("Submesh %s has %d vertice, candidate blendshape %s has %d vertices, so cannot create the blendshape.", shapeName.c_str(), s.V.size(), modelFile.c_str(), blendshapeModel.V.size());
				return;
			}
			// loop through the morph target to see if this controller already exists
			auto mapIter = mesh->blendShapeMap.find(submeshName);
			if (mapIter == mesh->blendShapeMap.end())
			{
				// no controller for this exists yet, set one up
				SrSnModel* baseModel = new SrSnModel();
				baseModel->shape(blendshapeModel);
				baseModel->shape().name = shapeName.c_str();
				baseModel->changed(true);
				baseModel->visible(false);
				baseModel->ref();

				// since this is the base model, overwrite the submesh's V and N
				mesh->dMeshStatic_p[m]->shape().V = baseModel->shape().V;
				mesh->dMeshStatic_p[m]->shape().N = baseModel->shape().N;

				std::vector<SrSnModel*> modelList;
				modelList.emplace_back(baseModel);
				mesh->blendShapeMap.insert(std::pair<std::string, std::vector<SrSnModel*> >(submeshName, modelList));
				std::vector<std::string> morphTargetList;
				morphTargetList.emplace_back(shapeName);
				mesh->morphTargets.insert(std::pair<std::string, std::vector<std::string> >(submeshName, morphTargetList));
			}
			else
			{
				// controller exists, see if the shape also exists
				bool found = false;
				std::vector<SrSnModel*>& existingShapeModels = (*mapIter).second;
				for (auto & existingShapeModel : existingShapeModels)
				{
					std::string modelName = (const char*) existingShapeModel->shape().name;
					if (modelName == shapeName)
					{
						// shape already exists
						// replace it if the vertices match
						existingShapeModel->shape().V = blendshapeModel.V;
						existingShapeModel->shape().N = blendshapeModel.N;

						found = true;
						break;
					}
				}
				if (!found)
				{
					SrSnModel* baseModel = new SrSnModel();
					baseModel->shape(blendshapeModel);
					baseModel->shape().name = shapeName.c_str();
					baseModel->changed(true);
					baseModel->visible(false);
					baseModel->ref();

					existingShapeModels.emplace_back(baseModel);
					auto morphNameIter = mesh->morphTargets.find(submeshName);
					if (morphNameIter == mesh->morphTargets.end())
					{
						SmartBody::util::log("Couldn't find controller name %s in morph target list, strange...", submeshName.c_str());
						return;
					}
					(*morphNameIter).second.emplace_back(shapeName);
				}

			}

		}
	}
}

void createCustomMeshFromBlendshapes(std::string templateMeshName, std::string blendshapesDir, std::string baseMeshName, std::string hairMeshName, std::string outMeshName)
{
	auto& renderAssetManager = Session::current->renderAssetManager;
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	DeformableMesh* mesh = renderAssetManager.getDeformableMesh(templateMeshName);
	if (!mesh)
	{
		SmartBody::util::log("Error creating custom mesh from blendshapes :: mesh '%s' does not exist.", templateMeshName.c_str());
		return;
	}

	for (auto iter = mesh->blendShapeMap.begin();
		iter != mesh->blendShapeMap.end();
		iter++)
	{
		std::vector<SrSnModel*>& targets = (*iter).second;
		for (size_t t = 0; t < targets.size(); t++) // ignore first target since it is a base mesh
		{
			if (targets[t] == nullptr)
				continue;
			SrModel& curModel = targets[t]->shape();
			SrModel newShape;
			std::string blendshapeName = (const char*) curModel.name;

			if (t == 0)
				blendshapeName = baseMeshName;
			//SmartBody::util::log("mesh '%s', blendShapeName = '%s'", templateMeshName.c_str(), blendshapeName.c_str());
			std::string meshFileName = blendshapesDir + "/" + blendshapeName;
			newShape.import_ply(meshFileName.c_str());
			if (newShape.V.size() == curModel.V.size())
			{
				curModel.V = newShape.V;
				curModel.N = newShape.N;
			}
			if (t == 0)
			{
				SrModel& staticBaseModel = mesh->dMeshStatic_p[0]->shape();
				staticBaseModel.V = newShape.V;
				staticBaseModel.N = newShape.N;

				SrModel& dynamicBaseModel = mesh->dMeshDynamic_p[0]->shape();
				dynamicBaseModel.V = newShape.V;
				dynamicBaseModel.N = newShape.N;

				SrModel hairModel;
				std::string hairFileName = blendshapesDir + "/" + hairMeshName;
				hairModel.import_ply(hairFileName.c_str());

				SrSnModel* hairSrSn = new SrSnModel();
				std::string hairName = "HairMesh";
				hairSrSn->shape(hairModel);
				hairSrSn->shape().name = hairName.c_str();
				mesh->dMeshStatic_p.emplace_back(hairSrSn);

				SkinWeight* hairSkin = new SkinWeight();
				SkinWeight* headSkin = mesh->skinWeights[0];
				SrMat hairBindShape = headSkin->bindShapeMat;
				SrMat hairBindPose;
				std::string headJointName = "Head";
				SmartBody::util::log("headSkin bindShape Mat = %s", hairBindShape.toString().c_str());
				for (unsigned int i = 0; i < headSkin->infJointName.size(); i++)
				{
					if (headSkin->infJointName[i] == headJointName)
					{
						//SmartBody::util::log("headSkin inf joint %d : '%s'", i, headSkin->infJointName[i].c_str());
						hairBindPose = headSkin->bindPoseMat[i];
						//SmartBody::util::log("headSkin bindPose Mat %d = %s", i, bindPose.toString().c_str());
					}
				}

				hairSkin->bindShapeMat = hairBindShape;
				hairSkin->bindPoseMat.emplace_back(hairBindPose);
				hairSkin->infJointName.emplace_back(headJointName);
				hairSkin->sourceMesh = hairName;
				hairSkin->bindWeight.emplace_back(1.0f);
				for (unsigned int i = 0; i < hairModel.V.size(); i++)
				{
					hairSkin->jointNameIndex.emplace_back(0);
					hairSkin->numInfJoints.emplace_back(1);
					hairSkin->weightIndex.emplace_back(0);
				}
				mesh->skinWeights.emplace_back(hairSkin);
			}
		}
	}

	// handle hair mesh
	


	std::string outputMeshFile = blendshapesDir + "/" + outMeshName;
	mesh->saveToDmb(outputMeshFile);
	// load base model
	//mesh->rebuildVertexBuffer(true);
}

//	Callback function for Python module Misc to run the checkVisibility function
std::vector<std::string> checkVisibility(const std::string& character)
{
	bool DEBUG_CHECK_VISIBILITY			= true;

	auto& renderScene = Session::current->renderScene;
	SmartBody::SBScene* scene			= SmartBody::SBScene::getScene();

	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window && window->curViewer)
		window->curViewer->make_current(); // make sure the OpenGL context is current

	std::vector<std::string> visible	= renderScene.checkVisibility(character);
	
	if(DEBUG_CHECK_VISIBILITY) {
		SmartBody::util::log ("Visible pawns from %s: ", character.c_str());
		for(const auto & i : visible)
		{
			SmartBody::util::log("%s, ", i.c_str());
		}
	}

	return visible;
}

//	Callback function for Python module Misc to run the checkVisibility function
std::vector<std::string> checkVisibility_current_view()
{
	bool DEBUG_CHECK_VISIBILITY			= true;
	
	SmartBody::SBScene* scene			= SmartBody::SBScene::getScene();
	auto& renderScene = Session::current->renderScene;

	// make current
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window && window->curViewer) {
		std::vector<std::string> visible = renderScene.checkVisibility_current_view();
		if(DEBUG_CHECK_VISIBILITY) {
			SmartBody::util::log("Visible pawns: ");
			for( std::vector<std::string>::const_iterator i = visible.begin(); i != visible.end(); ++i)  {
				SmartBody::util::log("%s, ", i);
			}
		}

		return visible;
	} else {
		return {};
	}
}

void addPoint(const std::string& pointName, SrVec point, SrVec color, int size)
{
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window->curViewer)
	{
		window->curViewer->addPoint(pointName, point, color, size);
	}
}

void removePoint(const std::string& pointName)
{
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window->curViewer)
	{
		window->curViewer->removePoint(pointName);
	}
}

void removeAllPoints()
{
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window->curViewer)
	{
		window->curViewer->removeAllPoints();
	}
}

void addLine(const std::string& lineName, std::vector<SrVec>& points, SrVec color, int width)
{
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window->curViewer)
	{
		window->curViewer->addLine(lineName, points, color, width);
	}
}

void removeLine(const std::string& lineName)
{
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window->curViewer)
	{
		window->curViewer->removeLine(lineName);
	}
}

void removeAllLines()
{
	BaseWindow* window = dynamic_cast<BaseWindow*>(SmartBody::SBScene::getScene()->getViewer());
	if (window->curViewer)
	{
		window->curViewer->removeAllLines();
	}
}

BOOST_PYTHON_MODULE(GUIInterface)
{
	boost::python::def("addPoint", addPoint, "addPoint");
	boost::python::def("removePoint", removePoint, "removePoint");
	boost::python::def("removeAllPoints", removePoint, "removeAllPoints");
	boost::python::def("addLine", addLine, "addLine");
	boost::python::def("removeLine", removeLine, "removeLine");
	boost::python::def("removeAllLines", removeAllLines, "removeAllLines");



	boost::python::class_<SBInterfaceListenerWrap, boost::noncopyable> ("SBInterfaceListener")
		.def(boost::python::init<>())
		.def("onStart", &SBInterfaceListener::onStart, "onStart")
		.def("onMouseClick", &SBInterfaceListener::onMouseClick, &SBInterfaceListenerWrap::default_onMouseClick, "onMouseClick")
		.def("onMouseMove", &SBInterfaceListener::onMouseMove, &SBInterfaceListenerWrap::default_onMouseMove, "onMouseMove")
		.def("onMouseRelease", &SBInterfaceListener::onMouseRelease, &SBInterfaceListenerWrap::default_onMouseRelease, "onMouseRelease")
		.def("onMouseDrag", &SBInterfaceListener::onMouseDrag, &SBInterfaceListenerWrap::default_onMouseDrag, "onMouseDrag")
		.def("onKeyboardPress", &SBInterfaceListener::onKeyboardPress, &SBInterfaceListenerWrap::default_onKeyboardPress, "onKeyboardPress")
		.def("onKeyboardRelease", &SBInterfaceListener::onKeyboardRelease, &SBInterfaceListenerWrap::default_onKeyboardRelease, "onKeyboardRelease")
		.def("onEnd", &SBInterfaceListener::onEnd, "onEnd")
	;

	boost::python::def("getInterfaceManager", SBInterfaceManager::getInterfaceManager, boost::python::return_value_policy<boost::python::reference_existing_object>(),"Gets the interface manager.");
	
	boost::python::class_<SBInterfaceManager, boost::noncopyable> ("SBInterfaceManager")
		.def("addInterfaceListener", &SBInterfaceManager::addInterfaceListener, "Adds an interface listener.")
		.def("removeInterfaceListener", &SBInterfaceManager::removeInterfaceListener, "Removes an interface listener.")
		.def("convertScreenSpaceTo3D", &SBInterfaceManager::convertScreenSpaceTo3D, "Converts screen space to 3D space given a point on a plane and a normal to that plane.")
		.def("getSelectedObject", &SBInterfaceManager::getSelectedObject, "Returns the name of the currently selection object.")
;
}


BOOST_PYTHON_MODULE(AutoRig)
{	
	boost::python::def("saveDeformableMesh", saveDeformableMesh, "Save the deformable model to the target directory");
	boost::python::def("saveDeformableMeshScale", saveDeformableMeshScale, "Save the deformable model with scaling factor to the target directory");
	boost::python::def("setPawnMesh", setPawnMesh, "Set the deformable model to the target pawn");
	boost::python::def("createCustomMeshFromBlendshapes", createCustomMeshFromBlendshapes, "create a new custom mesh with a different set of blendshapes.");
	boost::python::def("addModelToMesh", addModelToMesh, "Adds a submesh to a model.");
	boost::python::def("addBlendshapeToModel", addBlendshapeToModel, "Adds a blendshape to a mesh, or replaces the blendshape within the mesh.");
	boost::python::def("imageColorTransfer", imageColorTransfer, "color transfer of source image using the color styles of target image.");
	boost::python::def("deformableMeshTextureReplace", deformableMeshTextureReplace, "color transfer of source image using the color styles of target image.");

	


	boost::python::class_<SBAutoRigManager>("SBAutoRigManager")
		.def("getAutoRigManager", &SBAutoRigManager::singletonPtr, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Get the autorigging manager")
		.staticmethod("getAutoRigManager")
		.def("buildAutoRiggingFromPawnMesh", &SBAutoRigManager::buildAutoRiggingFromPawnMesh, "Build the rigging from a pawn with mesh")
		;
		//.def("setHPRSmooth", &SBAutoRigManager::setHPRSmooth, "Sets the heading, pitch and roll of the character's world offset. The character will be rotated smoothly overtime to avoid popping.")
		//;	
}

BOOST_PYTHON_MODULE(Misc)
{	
	boost::python::def("checkVisibility", checkVisibility, boost::python::return_value_policy<boost::python::return_by_value>(), "Lists visible pawns for a given character");
	boost::python::def("checkVisibility_current_view", checkVisibility_current_view, boost::python::return_value_policy<boost::python::return_by_value>(), "Lists visible pawns from current viewport");

}

void initGUIInterfacePythonModule()
{
	PyImport_AppendInittab("GUIInterface", &PyInit_GUIInterface);
}

void initMiscPythonModule()
{
	PyImport_AppendInittab("Misc", &PyInit_Misc);
}

void initAutoRigPythonModule()
{
	PyImport_AppendInittab("AutoRig", &PyInit_AutoRig);
}



