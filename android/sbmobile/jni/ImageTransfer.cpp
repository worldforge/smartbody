#include "ImageTransfer.h"
#include <algorithm>
#include <sb/SBScene.h>
#include "SBUtilities.h"
#include <sb/SBAssetManager.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <sbm/GPU/SbmTexture.h>
#include <sr/sr_mat.h>

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



//inline float clamp(float x, float a, float b) { return std::max(a, std::min(b, x)); }
inline float saturate(float x) { 
	//return clamp(x, 0.f, 1.f); 
	return std::max(0.f, std::min(1.f, x));
}

void lab2rgb(float* lab, unsigned char* rgb, unsigned int size)
{
	float temp1[16] = { 1, 1, 1, 0,
		1, 1, -1, 0,
		1, -2, 0, 0,
		0, 0,  0, 0 };
	float temp2[16] = { sqrt(3.f) / 3.f, 0, 0, 0,
		0, sqrt(6.f) / 6.f, 0 , 0,
		0, 0, sqrt(2.f) / 2.f , 0,
		0, 0,  0,  0 };
	SrMat m1_lab2rgb = SrMat(temp1);
	SrMat m2_lab2rgb = SrMat(temp2);

	float aLMS2RGB[16] = { 4.4679f, -3.5873f, 0.1193f , 0,
		-1.2186f, +2.3809f, -0.1624f, 0,
		0.0497f, -0.2439f, 1.2045f , 0,
		0,        0,        0,     0 };
	SrMat mLMS2RGB = SrMat(aLMS2RGB);

	for (unsigned int i = 0; i < size; i = i + 4)
	{
		float l = lab[i];
		float a = lab[i + 1];
		float b = lab[i + 2];
		SrVec lab(l, a, b);

		//Convert to LMS
		SrVec LMS = m1_lab2rgb * m2_lab2rgb * lab;
		LMS = SrVec(pow(10, LMS.x), pow(10, LMS.y), pow(10, LMS.z));//pow(SrVec(10), LMS);
																	//Convert to RGB
		auto rgbv = mLMS2RGB * LMS;
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
		0, 0, 0, 0 };
	float temp2[16] = { 1, 1, 1, 0,
		1, 1, -2, 0,
		1, -1, 0, 0,
		0, 0, 0, 0 };

	SrMat m1_rgb2lab = SrMat(temp1);
	SrMat m2_rgb2lab = SrMat(temp2);

	float aRGB2LMS[16] = { 0.3811f, 0.5783f, 0.0402f, 0,
		0.1967f, 0.7244f, 0.0782f, 0,
		0.0241f, 0.1288f, 0.8444f, 0,
		0,       0,       0,       0 };
	SrMat tmRGB2LMS = SrMat(aRGB2LMS);

	for (unsigned int i = 0; i < size; i = i + 4)
	{
		// log10(0) = -inf, so gotta use FLT_TRUE_MIN
		// otherwise, image stats are fucked up
		float r = std::max(FLT_MIN, rgb[i] / 255.f);
		float g = std::max(FLT_MIN, rgb[i + 1] / 255.f);
		float b = std::max(FLT_MIN, rgb[i + 2] / 255.f);
		SrVec rgb(r, g, b);

		//Convert to LMS
		SrVec LMS = tmRGB2LMS * rgb;
		LMS = SrVec(log10(LMS.x), log10(LMS.y), log10(LMS.z));

		//Convert to lab
		SrVec labv = m1_rgb2lab * m2_rgb2lab  * LMS;

		lab[i] = labv.x;
		lab[i + 1] = labv.y;
		lab[i + 2] = labv.z;
	}
}

void computeImageMeanAndStd(float* lab, unsigned char* mask, int imgSize, SrVec& outMean, SrVec& outStd)
{
	int pixCount = 0;
	SrVec imgMean = SrVec(0, 0, 0), imgStd = SrVec(0, 0, 0);
	for (unsigned int i = 0; i < imgSize; i++)
	{
		int labIdx = i * 4;
		if (mask[i] == 0) continue;
		SrVec pix = SrVec(lab[labIdx], lab[labIdx + 1], lab[labIdx + 2]);
		//SmartBody::util::log("Pix %d = %s", i, pix.toString().c_str());
		imgMean += pix;
		pixCount++;
	}

	//SmartBody::util::log("Image Pix Count = %d", pixCount);
	imgMean /= pixCount;

	for (unsigned int i = 0; i < imgSize; i++)
	{
		int labIdx = i * 4;
		if (mask[i] == 0) continue;
		for (int k = 0; k < 3; k++)
			imgStd[k] += pow(lab[labIdx + k] - imgMean[k], 2);
	}

	for (int k = 0; k < 3; k++)
		imgStd[k] = sqrtf(imgStd[k] / pixCount);
	outMean = imgMean;
	outStd = imgStd;

	//SmartBody::util::log("Image Mean = %s, Std = %s", outMean.toString().c_str(), outStd.toString().c_str());
}

void imageColorTransfer(std::string srcImg, std::string srcMask, std::string tgtImg, std::string tgtMask, std::string outImage)
{
	stbi_set_flip_vertically_on_load(false);
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
	for (int i = 0; i < srcSize; i++)
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
	int imageWriteSuccess = stbi_write_png(outImage.c_str(), srcWidth, srcHeight, 4, srcBuf, srcWidth * 4);
	delete [] srcLab;
	delete [] tgtLab;
	SmartBody::util::log("Writing PNG %s, result = %d", outImage.c_str(), imageWriteSuccess);
}

void replaceSubMesh(std::string deformableMeshName, std::string subMeshName, std::string inputMeshFileName)
{
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	DeformableMesh* mesh = assetManager->getDeformableMesh(deformableMeshName);
	if (!mesh)
	{
		SmartBody::util::log("Error repalce submesh :: mesh '%s' does not exist.", deformableMeshName.c_str());
		return;
	}

	int subMeshIdx = mesh->getMesh(subMeshName);
	if (subMeshIdx == -1)
	{
		SmartBody::util::log("Error repalce submesh :: submesh '%s' does not exist.", subMeshName.c_str());
		return;
	}

	// copy new model to replace the original submesh
	SmartBody::util::log("Replacing submesh '%s', submesh index = %d", subMeshName.c_str(), subMeshIdx);
	SrModel inputModel;
	inputModel.import_ply(inputMeshFileName.c_str());
	//SrModel& staticBaseModel = mesh->dMeshStatic_p[subMeshIdx]->shape();
	//SrModel& dynamicBaseModel = mesh->dMeshDynamic_p[subMeshIdx]->shape();
	mesh->dMeshStatic_p[subMeshIdx]->shape(inputModel);
	mesh->dMeshDynamic_p[subMeshIdx]->shape(inputModel);
	mesh->dMeshStatic_p[subMeshIdx]->shape().name = subMeshName.c_str();
	mesh->dMeshDynamic_p[subMeshIdx]->shape().name = subMeshName.c_str();

	// update skin weights with new vertices
	SkinWeight* subSkin = mesh->skinWeights[subMeshIdx];
	subSkin->bindWeight.clear();
	subSkin->jointNameIndex.clear();
	subSkin->numInfJoints.clear();
	subSkin->weightIndex.clear();
	subSkin->bindWeight.push_back(1.0f);
	for (unsigned int i = 0; i < inputModel.V.size(); i++)
	{
		subSkin->jointNameIndex.push_back(0);
		subSkin->numInfJoints.push_back(1);
		subSkin->weightIndex.push_back(0);
	}
	SbmDeformableMeshGPU* gpuMesh = (SbmDeformableMeshGPU*)mesh;
	gpuMesh->rebuildVertexBuffer(true);
	gpuMesh->rebuildVertexBufferGPU(true); // rebuild vertex buffers
										   // update mesh instance
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::vector < std::string > pawnNames = scene->getPawnNames();
	for (unsigned int i = 0; i < pawnNames.size(); i++)
	{
		SmartBody::SBPawn* pawn = scene->getPawn(pawnNames[i]);
		DeformableMeshInstance* pawnMeshInst = pawn->getActiveMesh();
		if (pawnMeshInst && pawnMeshInst->getDeformableMesh() == mesh) // need to update mesh instance for this pawn
		{
			SmartBody::util::log("Updating deformable model in pawn '%s'", pawnNames[i].c_str());
			SbmDeformableMeshGPUInstance* gpuMeshInst = (SbmDeformableMeshGPUInstance*)pawnMeshInst;
			gpuMeshInst->setDeformableMesh(gpuMesh);
			gpuMeshInst->initBuffer();
		}
	}
}


void deformableMeshTextureReplace(std::string meshName, std::string textureName, std::string inputImageFileName)
{
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	DeformableMesh* mesh = assetManager->getDeformableMesh(meshName);
	if (!mesh)
	{
		SmartBody::util::log("Error replacing texture '%s', mesh '%s' doesn't exist.", textureName.c_str(), meshName.c_str());
		return;
	}

	bool meshTextureExist = false;
	std::string finalTextureName = "";
	std::string combinedName = "";
	for (unsigned int i = 0; i < mesh->subMeshList.size(); i++)
	{
		SbmSubMesh* subMesh = mesh->subMeshList[i];
		//if (subMesh->texName == textureName)
		//	meshTextureExist = true;
		std::string textureNameStr = subMesh->texName;
		int pos = textureNameStr.find("|");
		if (pos != std::string::npos)
			textureNameStr = textureNameStr.substr(pos + 1);
		
		SmartBody::util::log("Submesh texture name is %s", subMesh->texName.c_str());
		if (textureNameStr.find(textureName) != std::string::npos) // texture name exists in the submesh
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
	SbmTexture* texture = SbmTextureManager::singleton().findTexture(SbmTextureManager::TEXTURE_DIFFUSE, finalTextureName.c_str());
	if (!texture)
	{
		SmartBody::util::log("Error replacing texture '%s', texture doesn't exist.", finalTextureName.c_str(), meshName.c_str());
		return;
	}

	texture->loadImage(inputImageFileName.c_str());
	texture->buildTexture();
}

