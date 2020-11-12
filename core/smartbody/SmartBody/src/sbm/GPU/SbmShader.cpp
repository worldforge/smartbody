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

#if !defined(__FLASHPLAYER__) && !defined(__ANDROID__) && !defined(EMSCRIPTEN) && !defined(SB_IPHONE)
#include "external/glew/glew.h"
#endif
#if defined(__ANDROID__)
#include <GLES3/gl3.h>
#endif
#include "SbmShader.h"
#include <sb/SBUtilities.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#endif



#if !defined(EMSCRIPTEN)
/************************************************************************/
/* Shader program class                                                 */
/************************************************************************/


SbmShaderManager& SbmShaderManager::singleton() 
{
	if (!_singleton)
		_singleton = new SbmShaderManager();
	return *_singleton;			
}

void SbmShaderManager::destroy_singleton() {
	if( _singleton )
		delete _singleton;
	_singleton = NULL;
}

int SbmShaderManager::getShaderSupport() { return shaderSupport; }


SBAPI void SbmShaderManager::releaseAllShaders()
{
	std::map<std::string, SbmShaderProgram*>::iterator vi;
	for (vi = shaderMap.begin();
		vi != shaderMap.end();
		vi++)
	{
		SbmShaderProgram* program = vi->second;
		delete program;
	}
	shaderMap.clear();
}

SbmShaderProgram::SbmShaderProgram()
{
	vsID = -1; 
	fsID = -1;
	programID = -1;
	isBuilt = false;	
}


SbmShaderProgram::~SbmShaderProgram()
{
#if !defined(__FLASHPLAYER__)
	if (programID > 0 )
		glDeleteProgram(programID);
	if (vsID > 0)
		glDeleteShader(vsID);
	if (fsID > 0)
		glDeleteShader(fsID);
#endif
	isBuilt = false;
}


void SbmShaderProgram::initShaderProgram( const char* vsName, const char* fsName )
{
	// we can initialize the shader name first, before there is a opengl context
	if (vsName && strlen(vsName) > 0)
	{
		vsFilename = vsName;
		char *vs = NULL;		
		vs = textFileRead(vsName);	
		if (vs)
			vsShaderStr = vs;		
		free(vs);	

	}
	if (fsName && strlen(fsName) > 0)
	{
		fsFilename = fsName;
		char *fs = NULL;		
		fs = textFileRead(fsName);	
		if (fs)
			fsShaderStr = fs;		
		free(fs);	
	}
}

void SbmShaderProgram::initShaderProgramStr( const char* shaderVS, const char* shaderFS )
{
	if (shaderVS)
		vsShaderStr = shaderVS;
	if (shaderFS)
		fsShaderStr = shaderFS;
}

void SbmShaderProgram::buildShader()
{
#if !defined(__FLASHPLAYER__)
	// build the shader after there is an opengl context
	vsID = -1;
	fsID = -1;
	//printOglError("buildShader::start");
	if (vsShaderStr.size() > 0)
	{
		//SmartBody::util::log("Create Vertex Shader");
		//SmartBody::util::log("Vertex Shader Text = %s", vsShaderStr.c_str());
		vsID = glCreateShader(GL_VERTEX_SHADER);
		//printOglError("buildShader::glCreateShader vs");
		//SmartBody::util::log("vsID = %d", vsID);
		loadShaderStr(vsID,vsShaderStr.c_str());
		//printOglError("buildShader::loadVertexShader");
		//SmartBody::util::log("After Build Vertex Shader");
	}	
	
	if (fsShaderStr.size() > 0)
	{
		//SmartBody::util::log("Create Fragment Shader");
		//SmartBody::util::log("Fragment Shader Text = %s", fsShaderStr.c_str());
		fsID = glCreateShader(GL_FRAGMENT_SHADER);
		//printOglError("buildShader::glCreateShader fs");
		loadShaderStr(fsID,fsShaderStr.c_str());
		//SmartBody::util::log("fsID = %d", fsID);
		//printOglError("buildShader::loadFragmentShader");
		//SmartBody::util::log("After Build Fragment Shader");
	}

	printShaderInfoLog(vsID);
	//printOglError("buildShader::printShaderInfoLog, vs");
	printShaderInfoLog(fsID);
	//printOglError("buildShader::printShaderInfoLog, fs");
    
	programID = glCreateProgram();
	//printOglError("buildShader::glCreateProgram");
	///SmartBody::util::log("programID = %d", programID);
	if (vsID != -1)
	{
		glAttachShader(programID, vsID);
		//printOglError("buildShader::glCreateProgram");
	}
	if (fsID != -1)
	{
		glAttachShader(programID, fsID);
		//printOglError("buildShader::glCreateProgram");
	}
	glLinkProgram(programID);
	//printOglError("buildShader::linkProgram");
	printProgramInfoLog(programID);
	isBuilt = true;	
#endif
}


void SbmShaderProgram::loadShader(GLuint sID,  const char* shaderFileName )
{
	char *vs = NULL;		
	vs = textFileRead(shaderFileName);	
	if (!vs) return;
	const char* vv = vs;
	loadShaderStr(sID,vv);
	free(vs);	
}


void SbmShaderProgram::loadShaderStr( GLuint sID, const char* shaderStr )
{		
	glShaderSource(sID, 1, &shaderStr,NULL);	
	//SmartBody::util::log("loadShaderStr, sdi = %d, shaderStr = %s", sID, shaderStr);
	//printOglError("loadShaderStr::glShaderSource");
	glCompileShader(sID);
	//printOglError("loadShaderStr::glCompileShader");
	//SmartBody::util::log("Finish compile shader, sid = %d", sID);
}


char * SbmShaderProgram::textFileRead(const char *fn )
{
	FILE *fp;
	char *content = NULL;
	int count=0;
	SmartBody::util::log("Loading text files...%s", fn);
	if (fn != NULL) {
		fp = fopen(fn,"rt");
		if (fp != NULL) {
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		} 
		else
		{
			SmartBody::util::log("*** ERROR: Could not open file %s", fn);
		}
	}
	return content;
}

void SbmShaderProgram::printShaderInfoLog( GLuint obj )
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
	//SmartBody::util::log("printShaderInfoLog, infologLength = %d", infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		SmartBody::util::log("%s\n",infoLog);
		free(infoLog);
	}
}

void SbmShaderProgram::printProgramInfoLog( GLuint obj )
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
	//SmartBody::util::log("printProgramInfoLog, infologLength = %d", infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		SmartBody::util::log("%s\n",infoLog);
		free(infoLog);
	}
}

void SbmShaderProgram::printOglError(const char* tag)
{
	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
#if 1
	while (glErr != GL_NO_ERROR)
	{
#if !defined(__FLASHPLAYER__) && !defined(__ANDROID__) && !defined(SB_IPHONE)
		SmartBody::util::log("glError %s: %s\n", tag,gluErrorString(glErr));
#elif defined(__ANDROID__) || defined(SB_IPHONE)
		std::string error;
		switch (glErr) {
		case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}
		SmartBody::util::log("glError %s: %s\n", tag, error.c_str());
#endif
		retCode = 1;
		glErr = glGetError();
	}
#endif
}
/************************************************************************/
/* Shader Manager                                                       */
/************************************************************************/
SbmShaderManager* SbmShaderManager::_singleton = NULL;
int SbmShaderManager::shaderSupport = SUPPORT_OPENGL_2_0;

SbmShaderManager::SbmShaderManager(void)
{
	viewer = NULL;
	shaderInit = false;
#if defined(__ANDROID__) || defined(SB_IPHONE)
	shaderSupport = SUPPORT_OPENGL_2_0;
#else
	shaderSupport = SUPPORT_OPENGL_2_0;
#endif
}

void SbmShaderManager::setViewer( SrViewer* vw )
{
	if (vw == NULL)
	{
		shaderInit = false;
	}
	viewer = vw;	
}

SbmShaderManager::~SbmShaderManager(void)
{
	releaseAllShaders();
}

bool SbmShaderManager::initOpenGL()
{
#if defined(__ANDROID__) || defined(SB_IPHONE)
	return true;
#else
	if (!viewer)
		return false;
	viewer->makeGLContext();
	return true;
#endif
}

bool SbmShaderManager::initGLExtension()
{	
	if (shaderInit && shaderSupport != NO_GPU_SUPPORT) // already initialize glew
		return true;

    if (shaderInit && shaderSupport == NO_GPU_SUPPORT)
        return false;

#if !defined(__FLASHPLAYER__) && !defined(__ANDROID__) && !defined(SB_IPHONE)
	if (!viewer)
		return false;
        static int counter = 0;

	//viewer->makeGLContext();

	glewInit();

/*
#if !defined(MAC_BUILD)	
    if (glewIsSupported("GL_VERSION_3_0"))
	{
        SmartBody::util::log("Ready for OpenGL 3.0\n");
		shaderInit = true;
		shaderSupport = SUPPORT_OPENGL_3_0;
		return true;
	}
    else 
#endif
	*/
	return checkShaderInit(counter);

    //return false;
#elif defined(__ANDROID__) || defined(SB_IPHONE)
	shaderInit = true;
	return true;
#endif
}

bool SbmShaderManager::checkShaderInit(int &counter)
{
#if !defined(__ANDROID__) && !defined(SB_IPHONE)
	if (glewIsSupported("GL_VERSION_4_0"))
	{
		SmartBody::util::log("Ready for OpenGL 4.0.\n");
		shaderInit = true;
		shaderSupport = SUPPORT_OPENGL_2_0;
		return true;
	}
	else if (glewIsSupported("GL_VERSION_2_0") || glewIsSupported("GL_VERSION_3_0"))
	{
		SmartBody::util::log("Ready for OpenGL 2.0.\n");
		shaderInit = true;
		shaderSupport = SUPPORT_OPENGL_2_0;
		return true;
	}
	else {
		//if (counter == 3)
		SmartBody::util::log("OpenGL 2.0 not supported. GPU Shader will be disabled.\n");
		//exit(1);
		counter++;
		if (counter == 3)
			shaderInit = true;
		return false;
	}
#else
	SmartBody::util::log("Ready for OpenGL 3.0.\n");
	shaderInit = true;
	shaderSupport = SUPPORT_OPENGL_2_0;

	return true;
#endif
}

void SbmShaderManager::addShader(const std::string& entryName, const std::string& vsName, const std::string& fsName, bool shaderFile )
{
    if (shaderSupport == NO_GPU_SUPPORT)
        return;

	std::string keyName = entryName;
	if (shaderMap.find(keyName) != shaderMap.end())
	{
		SbmShaderProgram* tempS = shaderMap[keyName];
		delete tempS;
	}
	
    SbmShaderProgram* program = new SbmShaderProgram();
	if (shaderFile)
		program->initShaderProgram(vsName.c_str(),fsName.c_str());
	else
	{		
		program->initShaderProgramStr(vsName.c_str(),fsName.c_str());
	}
	shaderMap[keyName] = program;
}

void SbmShaderManager::buildShaders()
{	
	std::map<std::string,SbmShaderProgram*>::iterator vi;
	for ( vi  = shaderMap.begin();
		  vi != shaderMap.end();
		  vi++)
	{
		SbmShaderProgram* program = vi->second;
		if (!program->finishBuild())
		{
			SmartBody::util::log("Building shader '%s'", vi->first.c_str());
			program->buildShader();
		}
	}
}

void SbmShaderManager::rebuildShaders()
{
	std::map<std::string,SbmShaderProgram*>::iterator vi;
	for ( vi  = shaderMap.begin();
		vi != shaderMap.end();
		vi++)
	{
		SbmShaderProgram* program = vi->second;
		program->buildShader();
	}
}


SbmShaderProgram* SbmShaderManager::getShader( const std::string& entryName )
{
	SbmShaderProgram* program = NULL;
	if (shaderMap.find(entryName) != shaderMap.end())
	{
		program = shaderMap[entryName];		
	}
	return program;	
}

#endif
