#pragma once
#include <sbm/GPU/SbmTexture.h>

class SBFrameBufferObject
{
public:
	SBFrameBufferObject();
	~SBFrameBufferObject();

	void initFBO(const std::string& bufferName);
	void attachTexture(SbmTexture* tex, GLenum attachment);
	void bindFBO();
	void unbindFBO();
	bool setDrawBufferDefault();
	bool setDrawBuffer(std::vector<GLuint>& drawBuffers);
	GLuint getFBO() { return fboID;  }

	static void drawTextureQuad(SbmTexture* tex);
	static void drawScreenQuad();

protected:
	std::string fboName;
	std::map<GLenum, GLuint> attachmentMap;
	GLuint fboID;

};


