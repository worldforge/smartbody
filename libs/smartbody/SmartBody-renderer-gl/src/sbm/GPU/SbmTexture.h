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

#pragma once
#include <map>
#include <string>
#include <vector>
#include <sb/SBTypes.h>
#include <sb/SBAsset.h>
#include <sr/sr_gl.h>
#include <sr/sr_color.h>
#include <memory>

class SbmTexture;
struct SbmTextureData;
typedef std::map<std::string, std::shared_ptr<SbmTexture>> StrTextureMap;

#if defined(EMSCRIPTEN)
#define USE_CUBE_MAP 1
#else
#define USE_CUBE_MAP 0
#endif

//support for cube map
#if USE_CUBE_MAP
class SbmCubeMapTexture;
typedef std::map<std::string,SbmCubeMapTexture*> StrCubeTextureMap;
#endif


class SbmTextureManager
{
public:
	enum { TEXTURE_DIFFUSE = 0, TEXTURE_NORMALMAP, TEXTURE_SPECULARMAP, TEXTURE_GLOSSYMAP, TEXTURE_RENDER_TARGET, TEXTURE_HDR_MAP, TEXTURE_CUBEMAP};
protected:
	struct TextureHolder {
		int type;
		std::shared_ptr<SbmTexture> texture;
	};
	std::map<std::string, TextureHolder> textureMap;
//	StrTextureMap normalTexMap;
//	StrTextureMap specularTexMap;
//	StrTextureMap glossyTexMap;
//	StrTextureMap renderTargetTexMap;
//	StrTextureMap hdrTexMap;
	std::map<std::string, GLuint> FBOMap;
private:
	static SbmTextureManager* _singleton;
	SbmTextureManager();
	~SbmTextureManager();
public:
	SBAPI static SbmTextureManager& singleton() 
	{
		if (!_singleton)
			_singleton = new SbmTextureManager();
		return *_singleton;			
	}

	SBAPI static void destroy_singleton() {

			delete _singleton;
		_singleton = nullptr;
	}	
	SBAPI std::shared_ptr<SbmTexture> findTexture(const char* textureName);
	SBAPI GLuint findFBO(const char* fboName);
	SBAPI void loadTexture(int type, const char* textureName, const char* fileName);
    SBAPI void addTexture(std::string name, int type, std::shared_ptr<SbmTexture> texture);
	SBAPI void updateTexture();
	SBAPI void reloadTexture();
	SBAPI std::vector<std::string> getTextureNames(int type);

	// Creates a 1x1 white texture
	SBAPI std::shared_ptr<SbmTexture> createTexture(int type, const char* textureName);
	SBAPI void deleteTexture(int type, const char* textureName);
	SBAPI void createWhiteTexture(const char* textureName, int width = 1, int height = 1);	
	SBAPI void createBlackTexture(const char* textureName, int width = 1, int height = 1);
	SBAPI void createColorTexture(const char* textureName, SrColor initColor, int width = 1, int height = 1);
	
	SBAPI GLuint createFBO(const char* fboName, bool recreateFBO = true);

	SBAPI void updateEnvMaps(SmartBody::SBScene& scene);
	void releaseAllTextures();	
protected:

//Zengrui: add some additional functions for cube-map texture
#if USE_CUBE_MAP
protected:
	StrCubeTextureMap cubeTextureMap;
public:
	SBAPI SbmCubeMapTexture* findCubeMapTexture(const char* cubeMapName);
	SBAPI void loadCubeMapTextures(const std::string cubeMapName, const std::vector<std::string> &textureNames, const std::vector<std::string> &textureFileNames);
#endif
};


struct SbmTextureData {
	~SbmTextureData();
	enum RotateEnum { ROTATE_NONE = 0, ROTATE_90, ROTATE_180, ROTATE_270 };
	std::string textureName;
	std::string textureFileName;
	int width = 1, height= 1;
	int channels = 4; // num of channels in the image
	std::vector<unsigned char> imgBuffer;
	bool finishBuild = false;
	bool transparentTexture = false;
	bool buildMipMap = false;
	GLuint texID = 0;
	GLint internal_format = GL_RGBA8;
	GLenum texture_format = GL_RGBA;
	GLenum dataType = GL_UNSIGNED_BYTE;
	RotateEnum texRotate = ROTATE_NONE;

	SBAPI bool loadImage(const char* fileName);
	SBAPI bool loadHDRImage(const char* fileName);
	SBAPI void buildTexture(bool buildMipMap = false, bool recreateTexture = true);
	SBAPI void rotateTexture(RotateEnum rotate);

	SBAPI void setTextureSize(int w, int h, int numChannels);
	SBAPI void bakeAlphaIntoTexture(SbmTexture* alphaTex, bool useAlphaBlend);

	SBAPI void createWhiteTexture(int w = 1, int h = 1);
	SBAPI void createEmptyTexture(int w , int h, int numChannels, GLenum type = GL_UNSIGNED_BYTE, SrColor initColor = SrColor::white);
};

class SbmTexture : public SmartBody::SBAsset// simple place holder for OpenGL texture
{
public:
	SbmTextureData data;
	SBAPI explicit SbmTexture();
	SBAPI explicit SbmTexture(const char* texName);
	SBAPI ~SbmTexture();

//	void cleanTexture();

	SBAPI bool hasBuild() const { return data.finishBuild; }
	SBAPI bool isTransparent() const { return data.transparentTexture; }
	bool isBuildMipMap() const { return data.buildMipMap; }
	void setBuildMipMap(bool val) { data.buildMipMap = val; }

	SBAPI const std::string& getName() const override { return data.textureName; }
	SBAPI const std::string& getFileName() const { return data.textureFileName; }
	SBAPI GLuint getID() const { return data.texID; }
	SBAPI bool loadImage(const char* fileName);	

	SBAPI bool loadHDRImage(const char* fileName);

	SBAPI void buildTexture(bool buildMipMap = false, bool recreateTexture = true);
	SBAPI void rotateTexture(SbmTextureData::RotateEnum rotate);

	SBAPI unsigned char* getBuffer();
	SBAPI int getBufferSize();
	SBAPI int getWidth() const { return data.width; }
	SBAPI int getHeight() const { return data.height; }
	SBAPI int getNumChannels() const { return data.channels; }

	SBAPI void setBuffer(unsigned char* buffer, int size);
	SBAPI void setTextureSize(int w, int h, int numChannels);

	SBAPI void bakeAlphaIntoTexture(SbmTexture* alphaTex, bool useAlphaBlend);
	// Creates a 1x1 white texture
	SBAPI void createWhiteTexture(int w = 1, int h = 1);	
	SBAPI void createEmptyTexture(int w , int h, int numChannels, GLenum type = GL_UNSIGNED_BYTE, SrColor initColor = SrColor::white);		

	
};
#if USE_CUBE_MAP
class SbmCubeMapTexture // simple place holder for OpenGL ES cubemap texture
{
protected:
	std::vector<std::string> textureNames;
	std::vector<std::string> textureFileNames;
	int width, height;
	int channels; // num of channels in the image	
	unsigned char* buffer;
	std::vector<unsigned char> imgBuffer;
	bool finishBuild;
	bool transparentTexture;
	GLuint texID;	
	GLuint internal_format, texture_format;	
	GLenum dataType;
public:
	SBAPI SbmCubeMapTexture(const std::vector<std::string> &textureNames, const std::vector<std::string> &fileNames);
	SBAPI ~SbmCubeMapTexture();
	SBAPI bool hasBuild() { return finishBuild; }
	SBAPI bool isTransparent() { return transparentTexture; }
	SBAPI const std::vector<std::string>& getNames() { return textureNames; }
	SBAPI const std::vector<std::string>& getFileName() { return textureFileNames; }
	SBAPI GLuint getID() { return texID; }
	SBAPI bool loadImage(const char* fileName);	
	SBAPI bool loadHDRImage(const char* fileName);
	SBAPI void buildCubeMapTexture(bool buildMipMap = false);
	SBAPI unsigned char* getBuffer();
	SBAPI int getBufferSize();
	SBAPI int getWidth() const { return width; }	
	SBAPI int getHeight() const { return height; }
	SBAPI int getNumChannels() const { return channels; }	

	SBAPI void setBuffer(unsigned char* buffer, int size);
	SBAPI void setTextureSize(int w, int h, int numChannels);

	// Creates a 1x1 white texture
	SBAPI void createWhiteTexture(){};
};
#endif