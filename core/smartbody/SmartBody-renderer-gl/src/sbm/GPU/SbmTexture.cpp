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
#include <sb/SBTypes.h>

#if !defined(__FLASHPLAYER__) && !defined(__ANDROID__) && !defined(SB_IPHONE) && !defined(EMSCRIPTEN)
#include "GL/glew.h"
#include "sbm/GPU/SbmDeformableMeshGPU.h"
#endif
#include "SbmTexture.h"
#include "SbmShader.h"
//#include "SOIL/SOIL.h"

#define STB_DEFINE
#include "stb.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <sb/SBTypes.h>
#include <sb/SBAttribute.h>
#include <sb/SBScene.h>
#include "SBUtilities.h"
//#include "imdebug/imdebug.h"
#include <sr/sr_gl.h>

/************************************************************************/
/* Sbm Texture Manager                                                  */
/************************************************************************/
SbmTextureManager* SbmTextureManager::_singleton = nullptr;

SbmTextureManager::SbmTextureManager( ) {

}

SbmTextureManager::~SbmTextureManager( )
{
    releaseAllTextures();
}

void SbmTextureManager::updateEnvMaps()
{
	std::vector<std::string> hdrTexNames = getTextureNames(SbmTextureManager::TEXTURE_HDR_MAP);
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::StringAttribute* envMapAttr = dynamic_cast<SmartBody::StringAttribute*>(scene->getAttribute("Renderer.envMapName"));
	SmartBody::StringAttribute* envDiffuseMapAttr = dynamic_cast<SmartBody::StringAttribute*>(scene->getAttribute("Renderer.envDiffuseMapName"));

	if (envMapAttr)
		envMapAttr->setValidValues(hdrTexNames);
	if (envDiffuseMapAttr)
		envDiffuseMapAttr->setValidValues(hdrTexNames);
}

SBAPI void SbmTextureManager::deleteTexture(int type, const char* textureName)
{
	textureMap.erase(textureName);
}



void SbmTextureManager::releaseAllTextures()
{
	textureMap.clear();


#if USE_CUBE_MAP
    StrCubeTextureMap::iterator it;
    for(it = cubeTextureMap.begin(); it != cubeTextureMap.end(); ++it){
        SbmCubeMapTexture* tex = it->second;
        delete tex;
    }
    cubeTextureMap.clear();
#endif
}


std::vector<std::string> SbmTextureManager::getTextureNames( int type )
{
    std::vector<std::string> texNames;

    for (auto& entry : textureMap) {
    	if (entry.second.type == type) {
    		texNames.emplace_back(entry.first);
    	}
    }

    return texNames;
}



void SbmTextureManager::loadTexture(int iType, const char* textureName, const char* fileName )
{
    std::string strTex		= textureName;
	//SmartBody::util::log("Loading texture '%s'.", textureName);
    // Retrieves texture map type: DIFFUSE, SPECULAR, GLOSSY, or NORMAL

    // overwrite the existing textures
	/*if (texMap.find(strTex) != texMap.end())
	{
		SbmTexture* texture = texMap[strTex];
		SbmTexture* tempTex = new SbmTexture(textureName);
		if (!tempTex->loadImage(fileName))
		{
			//SmartBody::util::log("ERROR: Can't load image %s. Invalid path? Is it an 8-bit image?", fileName);
			delete tempTex;
			return;
		}
		texture->loadImage(fileName);
	}
	else //*/if (textureMap.find(strTex) == textureMap.end())
    {
        auto texture = std::make_shared<SbmTexture>();
        texture->data.textureName = textureName;
        if(!texture->loadImage(fileName))
        {
            SmartBody::util::log("ERROR: Can't load image %s from %s. Invalid path? Is it an 8-bit image?", textureName, fileName);
			return;
        }

        SmartBody::util::log("Just loaded texture %s from %s type %d", textureName, fileName, iType);

		textureMap[strTex] = {iType, texture};
    }

}

void SbmTextureManager::addTexture(std::string name, int type, std::shared_ptr<SbmTexture> texture) {
	textureMap.emplace(std::move(name), TextureHolder{type, std::move(texture)});
}


void SbmTextureManager::createWhiteTexture(const char* textureName, int width, int height)
{
	createColorTexture(textureName, SrColor::white, width, height);
#if 0
    StrTextureMap& texMap	= findMap(SbmTextureManager::TEXTURE_DIFFUSE);

    // If the texture does not exist in the texture map, create a new one
    if (texMap.find(std::string(textureName)) == texMap.end())
    {
        SbmTexture* texture = new SbmTexture(textureName);
        texture->createWhiteTexture(width, height);
        texMap[std::string(textureName)] = texture;
        texture->buildTexture(false);
    }
#endif
}


SBAPI void SbmTextureManager::createBlackTexture(const char* textureName, int width /*= 1*/, int height /*= 1*/)
{
	createColorTexture(textureName, SrColor::black, width, height);
}

SBAPI void SbmTextureManager::createColorTexture(const char* textureName, SrColor initColor, int width /*= 1*/, int height /*= 1*/)
{
	// If the texture does not exist in the texture map, create a new one
	if (textureMap.find(std::string(textureName)) == textureMap.end())
	{
		auto texture = std::make_shared<SbmTexture>();
		texture->data.createEmptyTexture(width, height, 4, GL_UNSIGNED_BYTE, initColor);
		textureMap[std::string(textureName)] = {SbmTextureManager::TEXTURE_DIFFUSE, texture};
		texture->buildTexture(false);
	}
}

SBAPI std::shared_ptr<SbmTexture> SbmTextureManager::createTexture(int type, const char* textureName)
{
	std::string strTex = textureName;
	auto I = textureMap.find(strTex);
	if (I != textureMap.end())
		return I->second.texture;

	auto tex = std::make_shared<SbmTexture>();
	tex->data.textureName = textureName;
	textureMap[strTex] = {SbmTextureManager::TEXTURE_DIFFUSE, tex};
	return tex;
}

SBAPI GLuint SbmTextureManager::createFBO( const char* fboName, bool recreateFBO )
{
    std::string strFBO = fboName;
    if (FBOMap.find(strFBO) == FBOMap.end() || recreateFBO)
    {
        GLuint fboID;
        glGenFramebuffers(1, &fboID);
        FBOMap[strFBO] = fboID;
    }
	return FBOMap[strFBO];
}


SBAPI GLuint SbmTextureManager::findFBO( const char* fboName )
{
    std::string strFBO = fboName;
    if (FBOMap.find(strFBO) != FBOMap.end())
    {
        return FBOMap[strFBO];
    }
    return 0;
}


void SbmTextureManager::updateTexture()
{
	for (auto& entry: textureMap) {
		if (!entry.second.texture->data.finishBuild) {
			entry.second.texture->buildTexture();
		}
	}

#if USE_CUBE_MAP
    StrCubeTextureMap::iterator it;
    for(it = cubeTextureMap.begin(); it != cubeTextureMap.end(); ++it){
        SbmCubeMapTexture *tex = it->second;
        if(!tex->hasBuild())
            tex->buildCubeMapTexture();
    }
#endif
}


SBAPI void SbmTextureManager::reloadTexture()
{
	for (auto& entry: textureMap) {
		entry.second.texture->buildTexture();
	}


	// recreate FBO when reloading texture
	for (auto& entry: FBOMap) {
		GLuint fboID;
		glGenFramebuffers(1, &fboID);
		entry.second = fboID;
	}


#if USE_CUBE_MAP
    StrCubeTextureMap::iterator it;
    for(it = cubeTextureMap.begin(); it != cubeTextureMap.end(); ++it){
        SbmCubeMapTexture *tex = it->second;
        tex->buildCubeMapTexture();

    }
#endif
}




std::shared_ptr<SbmTexture> SbmTextureManager::findTexture(const char* textureName )
{
    std::string strTex = textureName;
    //SmartBody::util::log("Tex name: %s\tType: %d", strTex.c_str(), type);
    auto I = textureMap.find(strTex);
    if (I != textureMap.end()) {
		return I->second.texture;
	}
    return {};
}
#if USE_CUBE_MAP
SbmCubeMapTexture* SbmTextureManager::findCubeMapTexture(const char* cubeMapName){
    std::string strTex = cubeMapName;
    if(cubeTextureMap.find(strTex) != cubeTextureMap.end())
        return cubeTextureMap[strTex];
    return nullptr;
}

void SbmTextureManager::loadCubeMapTextures(const std::string cubeMapName, const std::vector<std::string> &textureNames, const std::vector<std::string> &textureFileNames){
    if(textureNames.size() != 6 || textureFileNames.size() != 6){
        SmartBody::util::log("Textures provided are not enough to build cube map!\n");
        return;
    }

    // If the texture does not exist in the texture map, create a new one
    if (cubeTextureMap.find(cubeMapName) == cubeTextureMap.end())
    {
        SbmCubeMapTexture* texture = new SbmCubeMapTexture(textureNames, textureFileNames);
        for(int i = 0; i < textureNames.size(); ++i){
            if(!texture->loadImage(textureFileNames[i].c_str()))
            {
                SmartBody::util::log("ERROR: Can't load image %s. Invalid path? Is it an 8-bit image?", textureFileNames[i].c_str());
            }
        }

        cubeTextureMap[cubeMapName] = texture;
    }

}
#endif
/************************************************************************/
/* Sbm Texture                                                          */
/************************************************************************/

SbmTextureData::~SbmTextureData() {
	if (texID != 0)
		glDeleteTextures(1, &texID);
}


SbmTexture::SbmTexture() = default;


SbmTexture::SbmTexture( const char* texName )
{
	data.textureName = texName;
}


SbmTexture::~SbmTexture()= default;


bool SbmTextureData::loadHDRImage(const char* fileName)
{
	stbi_set_flip_vertically_on_load(true);
	int tmpWidth, tmpHeight, nrComponents;
	float *data = stbi_loadf(fileName, &tmpWidth, &tmpHeight, &nrComponents, 0);
	if (!data) return false;

	width = tmpWidth;
	height = tmpHeight;
	dataType = GL_FLOAT;
	int dataSize = width*height*nrComponents * 4;
	imgBuffer.resize(dataSize);
	auto* ucdata = (unsigned char*)data;
	for (unsigned int i = 0; i < dataSize; i++)
	{
		imgBuffer[i] = ucdata[i];
	}
	stbi_image_free(data);
	return true;
}

bool SbmTextureData::loadImage( const char* fileName )
{
#if 0
	if (stbi_is_hdr(fileName)) // is an HDR image, load into float point buffer
	{
		return loadHDRImage(fileName);		
	}
#endif

	stbi_hdr_to_ldr_gamma(2.2f);
	stbi_hdr_to_ldr_scale(1.0f);
	int transparentPixel = 0;
	stbi_set_flip_vertically_on_load(true);
	std::unique_ptr<unsigned char[]> tmpBuffer(stbi_load(fileName, &width, &height, &channels, 0));
	if (!tmpBuffer)
	{
		SmartBody::util::log("Image %s failed to load.", fileName);
		return false;
	}
	std::vector<unsigned char> buffer;
	buffer.resize(width*height*channels);
	std::memcpy(buffer.data(), tmpBuffer.get(), buffer.size());
#if 0
    buffer = SOIL_load_image(fileName, &width, &height, &channels, SOIL_LOAD_AUTO);
	if (width < 0 || height < 0 || channels < 0)
	{
		const char* errorString = SOIL_last_result();
		SmartBody::util::log("Image %s failed to load: [%s]", fileName, errorString);
		return false;
	}
    else
	{
        SmartBody::util::log("Loading image       :%s\t%d\t%d\t%d", fileName, width, height, channels );
    }
    //std::string testOutFileName = fileName;
    //testOutFileName += ".bmp";
    //SOIL_save_image(testOutFileName.c_str(),SOIL_SAVE_TYPE_BMP,width,height,channels,buffer);
    int transparentPixel = 0;
    // invert the image in y-axis
    for(int j = 0; j*2 < height; ++j )
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;
        for(int i = width * channels ; i > 0; --i )
        {
            unsigned char temp = buffer[index1];
            buffer[index1] = buffer[index2];
            buffer[index2] = temp;
            ++index1;
            ++index2;
        }
    }
#endif

    if (channels == 4)
    {
        for (int j=0;j<height;j++)
        {
            for (int i=0;i<width;i++)
            {
                unsigned char alphaVal = buffer[j*width*channels+i*channels+3];
                if (alphaVal < 255)
                    transparentPixel++;
            }
        }
    }


    if (transparentPixel*50 > height*width)
    {
		transparentTexture = true;
        SmartBody::util::log("Texture %s is transparent.",fileName);
    }
    else
    {
        SmartBody::util::log("Texture %s is opaque",fileName);
    }

	imgBuffer = buffer;

    // set the texture file name
	textureFileName = fileName;
	finishBuild = false;


	SmartBody::util::log("Texture %s is %dx%d", fileName, width, height);
    return true;
}

void SbmTextureData::buildTexture(bool buildMipMap, bool recreateTexture)
{
	//SmartBody::util::log("Building texture '%s'", this->getName().c_str());
    //SmartBody::util::log("Start Build Texture");
#if !defined(__native_client__)
    //SbmShaderProgram::printOglError("SbmTexture.cpp:10");
    GLuint iType = GL_TEXTURE_2D;
#if !defined(EMSCRIPTEN) && !defined(__ANDROID__) && !defined(SB_IPHONE)   //OpenGL ES 2.0 no EMUN for GL_TEXTURE_2D
    myGLEnable(GL_TEXTURE_2D);
#endif
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#if !defined(EMSCRIPTEN) && !defined(__ANDROID__) && !defined(SB_IPHONE)
    myGLEnable(iType);
#endif

	if (texID == 0 || recreateTexture)
	{
		glDeleteTextures(1, &texID);
		glGenTextures(1, &texID);
	}

    glBindTexture(iType,texID);

#if defined(__ANDROID__) || defined(SB_IPHONE)
#define GL_CLAMP GL_CLAMP_TO_EDGE
#ifndef GL_RGB8
#define GL_RGB8 GL_RGB
#endif
#ifndef GL_RGBA8
#define GL_RGBA8 GL_RGBA
#endif
#endif

    //glTexParameteri(iType,GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(iType,GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(iType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(iType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //SmartBody::util::log("After Texture parameters : GL_TEXTURE_WRAP_S");

#if !defined (__FLASHPLAYER__) && !defined(__ANDROID__) && !defined(SB_IPHONE)
    if (buildMipMap)
        glTexParameteri(iType, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    else
        glTexParameteri(iType, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
#if !defined(EMSCRIPTEN)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#endif
#else
    glTexParameteri(iType, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
#endif

	//glTexParameteri(iType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(iType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(iType, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	bool isFloatTexture = (dataType == GL_FLOAT);
	//if (isFloatTexture)
	//	SmartBody::util::log("Texture %s, is float texture.", getName().c_str());
    if (channels == 3)
    {
#if !defined(EMSCRIPTEN)
        internal_format = isFloatTexture ? GL_RGB32F : GL_RGB8;
        texture_format = GL_RGB;
#else
        internal_format = GL_RGB;
        texture_format = GL_RGB;
#endif
    }
    else if (channels == 4)
    {
#if !defined(EMSCRIPTEN)
        internal_format = isFloatTexture ? GL_RGBA32F : GL_RGBA8;
        texture_format = GL_RGBA;
#else
        internal_format = GL_RGBA;
        texture_format = GL_RGBA;
#endif
    }
	else if (channels == 1) // single channel
	{
		internal_format = isFloatTexture ? GL_DEPTH_COMPONENT32F : GL_R8;
		texture_format = GL_DEPTH_COMPONENT;
	}
	unsigned char* buffer = nullptr;
	if (!imgBuffer.empty())
		buffer = imgBuffer.data();
    //glTexImage2D(iType,0,texture_format,width,height,0,texture_format,GL_UNSIGNED_BYTE,buffer);
#if !defined (__FLASHPLAYER__) && !defined(__ANDROID__) && !defined(SB_IPHONE) && !defined(__linux__) &!defined(EMSCRIPTEN)
    //if (buildMipMap)
    //    gluBuild2DMipmaps(iType, channels, width, height, texture_format, dataType, buffer);
    //else
	GLint texSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);

	SmartBody::util::log("build texture image buffer size = %d, maxTexture Size = %d", imgBuffer.size(), texSize);
    glTexImage2D(iType,0,internal_format, width,height,0,texture_format, dataType, buffer);
	//if (buildMipMap)
	//	glGenerateMipmap(GL_TEXTURE_2D);
#elif defined(EMSCRIPTEN)
    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	if (buildMipMap)
		glGenerateMipmap(GL_TEXTURE_2D);
#else
	//SmartBody::util::log("texName = %s, iType = %d, width = %d, height = %d", textureName.c_str(), iType, width, height);
    glTexImage2D(iType,0,texture_format,width,height,0,texture_format,GL_UNSIGNED_BYTE, buffer);
	if (buildMipMap)
		glGenerateMipmap(GL_TEXTURE_2D);
#endif

#if !defined(__ANDROID__) && !defined(SB_IPHONE) && !defined(EMSCRIPTEN)
    GLclampf iPrority = 1.0;
    glPrioritizeTextures(1,&texID,&iPrority);
#endif
    glBindTexture(iType,0);
    finishBuild = true;
#endif
}

SBAPI void SbmTextureData::rotateTexture(RotateEnum rotate)
{
	if (rotate == ROTATE_NONE) return; // do nothing if there is no rotation

	std::vector<unsigned char> imgCopy = imgBuffer;
	int newWidth = width , newHeight = height;
	if (rotate == ROTATE_90 || rotate == ROTATE_270)
	{
		std::swap(newWidth, newHeight);
	}
	int bytesPerChannel = 1; // assume unsigned byte
	if (dataType == GL_FLOAT)
		bytesPerChannel = 4; // set to float for g-buffer application
	int pixelSize = bytesPerChannel*channels;
	int ti = 0, tj = 0;
	for (int i=0; i<newHeight;i++)
		for (int j = 0; j < newWidth; j++)
		{
			if (rotate == ROTATE_270)
			{
				ti = newWidth - j - 1;
				tj = i;
			}
			else if (rotate == ROTATE_180)
			{
				ti = newHeight - i - 1;
				tj = j;
			}
			else if (rotate == ROTATE_90)
			{
				ti = j;
				tj = newHeight - i - 1;
			}
			for (unsigned int k = 0; k < pixelSize; k++)
			{
				imgBuffer[(i*newWidth + j)*pixelSize + k] = imgCopy[(ti*width + tj)*pixelSize + k];
			}
		}
	width = newWidth;
	height = newHeight;
}

unsigned char* SbmTexture::getBuffer()
{
    if (data.imgBuffer.empty())
    	return nullptr;

    return data.imgBuffer.data();
}

int SbmTexture::getBufferSize()
{
    return data.imgBuffer.size();
}

void SbmTexture::setBuffer(unsigned char* buffer, int size)
{
	data.imgBuffer.clear();
    for (int i = 0; i < size; ++i)
    {
		data.imgBuffer.emplace_back(buffer[i]);
    }
}

void SbmTextureData::setTextureSize(int w, int h, int numChannels)
{
    width		= w;
    height		= h;
    channels	= numChannels;
}

void SbmTextureData::createWhiteTexture(int w, int h)
{
#if 0
    unsigned char* data;

    width		= w;
    height		= h;
    channels	= 4;

    data = new unsigned char[width * height * channels * sizeof(unsigned char)];

    for(int i = 0; i < (int)(width * height * channels * sizeof(unsigned char)); i++)
    {
        data[i] = 255;
    }

    imgBuffer.resize(width*height*channels);

    for (int i=0;i<width*height*channels;i++)
    {
        imgBuffer[i] = data[i];
    }
#endif
	createEmptyTexture(w, h, 4, GL_UNSIGNED_BYTE, SrColor::white);
    //textureFileName		= "white";
    //textureName			= "white";
}


void SbmTextureData::createEmptyTexture(int w /*= 1*/, int h /*= 1*/, int numChannels /*= 1*/, GLenum type /*= GL_UNSIGNED_BYTE*/, SrColor initColor)
{
	unsigned char* data;

	width = w;
	height = h;
	channels = numChannels;
	dataType = type;
	int bytesPerChannel = 1; // assume unsigned byte
	if (dataType == GL_FLOAT)
		bytesPerChannel = 4; // set to float for g-buffer application


	data = new unsigned char[width * height * channels * sizeof(unsigned char)*bytesPerChannel];

	if (dataType == GL_FLOAT)
	{
		float color[4];
		initColor.get(color);
		float* floatData = (float*)data;
		for (unsigned int i = 0; i < width*height; i++)
		{
			for (int j = 0; j < channels; j++)
			{
				floatData[i*channels + j] = color[j];
			}
		}
	}
	else // (dataType == GL_UNSIGNED_BYTE)
	{
		unsigned char color[4];
		initColor.get(color);
		for (unsigned int i = 0; i < width*height; i++)
		{
			for (int j = 0; j < channels; j++)
			{
				data[i*channels + j] = color[j];
			}
		}
	}

	imgBuffer.resize(width*height*channels*bytesPerChannel);

	for (int i = 0; i < width*height*channels; i++)
	{
		imgBuffer[i] = data[i];
	}
	delete [] data;
}



void SbmTextureData::bakeAlphaIntoTexture(SbmTexture* alphaTex, bool useAlphaBlend)
{
	unsigned char* alphaBuf = alphaTex->getBuffer();

	if (alphaTex->getWidth() != width || alphaTex->getHeight() != height)
	{
		SmartBody::util::log("Warning! Alpha texture size (%d, %d) do not match diffuse texture '%s' size (%d, %d)", alphaTex->getWidth(), alphaTex->getHeight(), textureName.c_str(), width, height);
		return;
	}

	if (channels != 4) // set back to 4 channel texture
	{
		std::vector<unsigned char> tempBuffer;
		tempBuffer.resize(width*height * 4);
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				for (int k=0;k<channels;k++)
					tempBuffer[j*width*4 + i*4 + k] = imgBuffer[j*width*channels + i*channels + k];
			}
		}
		imgBuffer = tempBuffer;
		channels = 4;
	}

	int numAlphaChannel = alphaTex->getNumChannels();
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			float transparency = 1.0f;

			float transr = 1.0f;
			float transg = 1.0f;
			float transb = 1.0f;
			float transa = 1.0f;

			float tr = alphaBuf[j * width*4 + i * 4 + 0];
			float tg = alphaBuf[j * width*4 + i * 4 + 1];
			float tb = alphaBuf[j * width*4 + i * 4 + 2];
			float ta = alphaBuf[j * width*4 + i * 4 + 3];

			float fr = imgBuffer[j*width * 4 + i * 4 + 0];
			float fg = imgBuffer[j*width * 4 + i * 4 + 1];
			float fb = imgBuffer[j*width * 4 + i * 4 + 2];
			float fa = imgBuffer[j*width * 4 + i * 4 + 3];

			//float luminance = (transr * .212671f) +
				//(transg * .715160f) +
				//(transb * .072169f);

			float luminance = (tr * .212671f) +
				(tg * .715160f) +
			(tb * .072169f);

			if (useAlphaBlend)
			{
				// A_ONE
				//imgBuffer[j*width * 4 + i * 4 + 0] = fr * 1.0f - (transa * transparency) + tr * (transr * transparency);
				//imgBuffer[j*width * 4 + i * 4 + 1] = fg * 1.0f - (transa * transparency) + tg * (transg * transparency);
				//imgBuffer[j*width * 4 + i * 4 + 2] = fb * 1.0f - (transa * transparency) + tb * (transb * transparency);
				//imgBuffer[j*width * 4 + i * 4 + 3] = fa * 1.0f - (transa * transparency) + ta * (transa * transparency);
				//imgBuffer[j*width * 4 + i * 4 + 0] = fr * tr;
				//imgBuffer[j*width * 4 + i * 4 + 1] = fg * tb;
				//imgBuffer[j*width * 4 + i * 4 + 2] = fb * tg;
				//imgBuffer[j*width * 4 + i * 4 + 3] = fa * ta;

				imgBuffer[j*width * 4 + i * 4 + 3] = 1.0f - fa;
			}
			else
			{
				// RGB_ZERO
				//imgBuffer[j*width * 4 + i * 4 + 0] = fr * (transr * transparency) + tr * (1.0f - transr * transparency);
				//imgBuffer[j*width * 4 + i * 4 + 1] = fg * (transg * transparency) + tg * (1.0f - transg * transparency);
				//imgBuffer[j*width * 4 + i * 4 + 2] = fb * (transb * transparency) + tb * (1.0f - transb * transparency);
				//imgBuffer[j*width * 4 + i * 4 + 3] = fa * (luminance * transparency) + ta * (1.0f - luminance * transparency);

				imgBuffer[j*width * 4 + i * 4 + 3] = luminance;

			}


			//imgBuffer[j*width * 4 + i * 4 + 3] = 1.0;


		}
	}
	transparentTexture = true;
}

bool SbmTexture::loadImage(const char* fileName) {
	return data.loadImage(fileName);
}

bool SbmTexture::loadHDRImage(const char* fileName) {
	return data.loadHDRImage(fileName);
}

void SbmTexture::buildTexture(bool buildMipMap, bool recreateTexture) {
	data.buildTexture(buildMipMap, recreateTexture);

}

void SbmTexture::rotateTexture(SbmTextureData::RotateEnum rotate) {
	data.rotateTexture(rotate);
}

void SbmTexture::setTextureSize(int w, int h, int numChannels) {
	data.setTextureSize(w, h, numChannels);
}

void SbmTexture::bakeAlphaIntoTexture(SbmTexture* alphaTex, bool useAlphaBlend) { data.bakeAlphaIntoTexture(alphaTex, useAlphaBlend); }

void SbmTexture::createWhiteTexture(int w, int h) {
	data.createWhiteTexture(w, h);
}

void SbmTexture::createEmptyTexture(int w, int h, int numChannels, GLenum type, SrColor initColor) {
	data.createEmptyTexture(w, h, numChannels, type, initColor);

}

#if USE_CUBE_MAP
SbmCubeMapTexture::SbmCubeMapTexture(const std::vector<std::string>& texNames, const std::vector<std::string> &fileNames){
    if(texNames.size() != 6 || fileNames.size())
        SmartBody::util::log("Wrong number of textures!");
    textureNames = texNames;
    textureFileNames = fileNames;
    texID				= 0;
    buffer				= nullptr;
    finishBuild			= false;
    transparentTexture	= false;
    width				= -1;
    height				= -1;
    channels			= -1;

}
SbmCubeMapTexture::~SbmCubeMapTexture(){
    if (buffer)
        delete [] buffer;
    if (texID != 0)
        glDeleteTextures(1,&texID);
}



bool SbmCubeMapTexture::loadImage(const char* fileName){

	buffer = SOIL_load_image(fileName, &width, &height, &channels, SOIL_LOAD_AUTO);
    if (width < 0 || height < 0 || channels < 0)
        return false;
    else {
        SmartBody::util::log("Loading image       :%s\t%d\t%d\t%d", fileName, width, height, channels );
    }

    int transparentPixel = 0;
    // invert the image in y-axis
    for(int j = 0; j*2 < height; ++j )
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;
        for(int i = width * channels ; i > 0; --i )
        {
            unsigned char temp = buffer[index1];
            buffer[index1] = buffer[index2];
            buffer[index2] = temp;
            ++index1;
            ++index2;
        }
    }

    if (channels == 4)
    {
        for (int j=0;j<height;j++)
        {
            for (int i=0;i<width;i++)
            {
                unsigned char alphaVal = buffer[j*width*channels+i*channels+3];
                if (alphaVal < 250)
                    transparentPixel++;
            }
        }
    }


    if (transparentPixel*20 > height*width)
        transparentTexture = true;

    //append the new image to the image buffer
    size_t offset = imgBuffer.size();
    imgBuffer.resize(offset + width * height * channels);

    for (size_t i=0;i<width*height*channels;i++)
    {
        imgBuffer[i + offset] = buffer[i];
    }

    //TODO: set the texture file name

    SOIL_free_image_data(buffer);
    buffer = nullptr;
    return true;
}
void SbmCubeMapTexture::buildCubeMapTexture(bool buildMipMap){
    if (!getBuffer()) return;
    GLuint iType = GL_TEXTURE_CUBE_MAP;
    glGenTextures ( 1, &texID );
    // Bind the texture object
    glBindTexture ( iType, texID );

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //we bind them into one texId
    glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    if (channels == 3)
    {
        internal_format = GL_RGB;
        texture_format = GL_RGB;
    }
    else if (channels == 4)
    {
        internal_format = GL_RGBA;
        texture_format = GL_RGBA;
    }
    //loading texture to the cube

    for(int i = 0, offset = 0; i < 6; ++i, offset += width * height * channels){
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, texture_format, width, height, 0, texture_format, GL_UNSIGNED_BYTE, &(imgBuffer[offset]));
    }
    //glBindTexture(iType, 0);
    finishBuild = true;
}
unsigned char* SbmCubeMapTexture::getBuffer()
{
    if (imgBuffer.size() == 0) return nullptr;

    return &imgBuffer[0];
}

int SbmCubeMapTexture::getBufferSize()
{
    return imgBuffer.size();
}

void SbmCubeMapTexture::setBuffer(unsigned char* buffer, int size)
{
    imgBuffer.clear();
    for (int i = 0; i < size; ++i)
    {
        imgBuffer.emplace_back(buffer[i]);
    }
}

void SbmCubeMapTexture::setTextureSize(int w, int h, int numChannels)
{
    width		= w;
    height		= h;
    channels	= numChannels;
}

#endif
