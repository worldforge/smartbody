//
//  test.h
//  sbmwrapper
//
//  Created by Yuyu Xu on 8/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#if __cplusplus
extern "C"
{
#endif
    int cameraMode;
    void SBInitialize(const char* path);
    void SBSetupDrawing(int w, int h);
    void SBDrawFrame(int w, int h);
    void SBDrawCharacters();
    void drawLights();
    void SBUpdate(float t);
    void SBExecuteCmd(const char* command);
    void SBExecutePythonCmd(const char* command);
    void SBCameraOperation(float dx, float dy);
#if __cplusplus
}
#endif