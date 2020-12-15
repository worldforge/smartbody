
#include "GL/glew.h"
#include "SBFrameBufferObject.h"
#include "SBUtilities.h"



SBFrameBufferObject::SBFrameBufferObject()
{

}

SBFrameBufferObject::~SBFrameBufferObject()
{

}

void SBFrameBufferObject::initFBO(const std::string& bufferName)
{
	SbmTextureManager& texManager = SbmTextureManager::singleton();
	fboName = bufferName;
	fboID  = texManager.createFBO(fboName.c_str());	
}

void SBFrameBufferObject::attachTexture(SbmTexture* tex, GLenum attachment)
{	
	bindFBO();
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex->getID(), 0);
	if (attachment != GL_DEPTH_ATTACHMENT)
		attachmentMap[attachment] = tex->getID();
	unbindFBO();
}

void SBFrameBufferObject::bindFBO()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);
}

void SBFrameBufferObject::unbindFBO()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

bool SBFrameBufferObject::setDrawBufferDefault()
{
	std::vector<GLenum> drawBuffers;
	std::map<GLenum, GLuint>::iterator mi;
	for (mi = attachmentMap.begin();
		 mi != attachmentMap.end();
		 mi++)
	{
		//SmartBody::util::log("Draw Buffer = %d", mi->first);
		drawBuffers.emplace_back(mi->first);
	}
	//SmartBody::util::log("Draw Buffer size = %d", drawBuffers.size());
	return setDrawBuffer(drawBuffers);	
}

bool SBFrameBufferObject::setDrawBuffer(std::vector<GLenum>& drawBuffers)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);
	glDrawBuffers(drawBuffers.size(),(GLenum*)&drawBuffers[0]);

	GLenum frameStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	if (frameStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		SmartBody::util::log("SBFrameBufferObject error, name = %s, status: 0x%x\n", fboName.c_str(), frameStatus);		
		return false;
	}		
	//SmartBody::util::log("SBFrameBufferObject::set Draw buffer successfully.");
	return true;
}

void SBFrameBufferObject::drawTextureQuad(SbmTexture* tex)
{
	if (!tex) return;

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glColor3f(1.f, 1.f, 1.f);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, tex->getID());

	drawScreenQuad();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void SBFrameBufferObject::drawScreenQuad()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//glPushAttrib(GL_VIEWPORT_BIT);
	//glPushAttrib(GL_TEXTURE_BIT);
	glLoadIdentity();

	float zDepth = -0.5f;
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-1.0f, 1.0f, zDepth);

	glTexCoord2f(0, 0);
	glVertex3f(-1.0f, -1.0f, zDepth);

	glTexCoord2f(1, 0);
	glVertex3f(1.0f, -1.0f, zDepth);

	glTexCoord2f(1, 1);
	glVertex3f(1.0f, 1.0f, zDepth);
	glEnd();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();                              // Pops ENABLE_BIT
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
