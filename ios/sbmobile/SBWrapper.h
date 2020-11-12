//
//  test.h
//  sbmwrapper
//
//  Created by Yuyu Xu on 8/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef __MINIMAL_WRAPPER__
#define __MINIMAL_WRAPPER__
#include <sr/sr_mat.h>
#include <sb/SBTypes.h>
#include "esUtil.h"
#if __cplusplus
extern "C"
{
#endif
  
  
  void SBInitVHMSGConnection();
  void SBCloseVHMSGConnection();
  
  void SBSetup(const char* path, const char* setupScript);
  void SBInitialize();
  void SBInitGraphics(ESContext* esContext);
  void SBInitScene(const char* initScriptName);
  void SBSetupDrawing(int w, int h, ESContext* esContext);
  
  void SBDrawFrame_ES20(int w, int h, ESContext *esContext, SrMat eyeView);
  void SBDrawFBOTex_ES20(int w, int h, ESContext *esContext, SrMat eyeView);
  void SBDrawFrameAR(int width, int height, ESContext *esContext,
                     const SrMat& modelViewMat, const SrMat& projMat);
    
  void SBUpdateCharacterGPUSkin();
  void SBDrawCharacterGPUSkin(ESContext *esContext);
  void SBDrawCharacters_ES20(ESContext *esContext);
  void SBDrawBackground(ESContext* esContext);
  void SBDrawPawns(ESContext *esContext);
  void SBDrawGrid(ESContext *esContext);
#if 0
  void SBDrawFrame(int w, int h, SrMat eyeView);
  void SBDrawCharacters();
#endif
  
  void setupLights();
  void SBUpdate(float t);
  void SBExecuteCmd(const char* command);
  void SBExecutePythonCmd(const char* command);
  void SBCameraOperation(float dx, float dy, int cameraMode);
  
  bool SBPythonCommandVoid(const char * command);
  bool SBPythonCommandBool(const char * command);
  int SBPythonCommandInt(const char * command);
  float SBPythonCommandFloat(const char * command);
  void SBPythonCommandString(const char * command, char * output, int maxLen);
#if __cplusplus
}
#endif
#endif

