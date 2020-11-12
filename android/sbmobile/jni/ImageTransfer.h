#ifndef __IMAGE_TRANSFER__
#define __IMAGE_TRANSFER__
#include <string>
#if __cplusplus
extern "C"
{
#endif
	void imageColorTransfer(std::string srcImg, std::string srcMask, std::string tgtImg, std::string tgtMask, std::string outImage);
	void deformableMeshTextureReplace(std::string meshName, std::string textureName, std::string inputImageFileName);
	void replaceSubMesh(std::string deformableMeshName, std::string subMeshName, std::string inputMeshFileName);

#if __cplusplus
}
#endif
#endif