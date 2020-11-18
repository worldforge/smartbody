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

#include <vector>
#include <string>
#include <map>
#include <sr/sr_gl.h>
#include <sr/sr_viewer.h>

#if defined(EMSCRIPTEN)

class SbmShaderProgram
{
protected:
	GLuint     vsID, fsID, programID;
	std::string     vsFilename, fsFilename;
	std::string     vsShaderStr, fsShaderStr;
	bool       isBuilt;
public:
	SbmShaderProgram();
	~SbmShaderProgram();		
	void initShaderProgram(const char* vsName, const char* fsName) {}
	void initShaderProgramStr(const char* shaderVS, const char* shaderFS) {}
	GLuint getShaderProgram() { return programID; }

	void buildShader() {}
	bool finishBuild() { return isBuilt; }
public:
  static char *textFileRead(const char *fn) { return NULL; } // text file reading
	static void printShaderInfoLog(GLuint obj) {}
	static void printProgramInfoLog(GLuint obj) {}
	static void printOglError(const char* tag) {}
protected:
	void loadShader(GLuint sID, const char* shaderFileName) {}
	void loadShaderStr(GLuint sID, const char* shaderStr) {}
};


class SbmShaderManager
{
public:
	enum { SUPPORT_OPENGL_3_0, SUPPORT_OPENGL_2_0, NO_GPU_SUPPORT };
protected:
	std::map<std::string,SbmShaderProgram*> shaderMap;
	SrViewer* viewer;
	bool shaderInit;	
	static int shaderSupport;
private:
	// for singleton
	static SbmShaderManager* _singleton;
	SBAPI SbmShaderManager(void) {}
	SBAPI ~SbmShaderManager(void) {}
public:
	SBAPI static SbmShaderManager& singleton() {}
	SBAPI static void destroy_singleton() {}

	SBAPI bool initOpenGL() { return true;  }
	SBAPI bool initGLExtension() { return true;  }
	SBAPI void setViewer(SrViewer* vw) {}
	SBAPI void addShader(const char* entryName,const char* vsName, const char* fsName, bool shaderFile = true) {}
  SBAPI SbmShaderProgram* getShader(const std::string& entryName) { return NULL; }
	SBAPI void buildShaders() {}
	SBAPI static int getShaderSupport() {
		return SUPPORT_OPENGL_2_0;
	}
};

#else
class SbmShaderProgram
{
protected:
	GLuint     vsID, fsID, programID;
	std::string     vsFilename, fsFilename;
	std::string     vsShaderStr, fsShaderStr;
	bool       isBuilt;
public:
	SbmShaderProgram();
	~SbmShaderProgram();		
	SBAPI void initShaderProgram(const char* vsName, const char* fsName);
	SBAPI void initShaderProgramStr(const char* shaderVS, const char* shaderFS);
	SBAPI GLuint getShaderProgram() { return programID; }

	SBAPI void buildShader();
	SBAPI bool finishBuild() { return isBuilt; }
public:
	SBAPI static char *textFileRead(const char *fn); // text file reading 	
	SBAPI static void printShaderInfoLog(GLuint obj);
	SBAPI static void printProgramInfoLog(GLuint obj);
	SBAPI static void printOglError(const char* tag);
protected:
	void loadShader(GLuint sID, const char* shaderFileName);
	void loadShaderStr(GLuint sID, const char* shaderStr);
};


class SbmShaderManager
{
public:
	enum { SUPPORT_OPENGL_3_0, SUPPORT_OPENGL_2_0, NO_GPU_SUPPORT };
protected:
	std::map<std::string,SbmShaderProgram*> shaderMap;
	SrViewer* viewer;
	bool shaderInit;	
	static int shaderSupport;
private:
	// for singleton
	static SbmShaderManager* _singleton;
	SBAPI SbmShaderManager(void);
	SBAPI ~SbmShaderManager(void);
public:
	SBAPI static SbmShaderManager& singleton();
	SBAPI static void destroy_singleton();

	SBAPI bool initOpenGL();
	SBAPI bool initGLExtension();	

	SBAPI bool checkShaderInit(int &counter);
	SBAPI void setViewer(SrViewer* vw);
	SBAPI void addShader(const std::string& entryName, const std::string& vsName, const std::string& fsName, bool shaderFile = true);
	SBAPI SbmShaderProgram* getShader(const std::string& entryName);
	SBAPI void buildShaders();
	SBAPI void rebuildShaders();
	SBAPI static int getShaderSupport();
	SBAPI void releaseAllShaders();
};
#endif
