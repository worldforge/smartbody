//
//  test.h
//  sbmwrapper
//
//  Created by Yuyu Xu on 8/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef __MINIMAL_WRAPPER__
#define __MINIMAL_WRAPPER__
#include <sb/SBSceneListener.h>
#include <sb/SBColObject.h>
#include <sr/sr_mat.h>
#include "esUtil.h"
#include <emscripten/bind.h>
#include <emscripten.h>

class DeformableMeshInstance;

class AppListener : public SmartBody::SBSceneListener, public SmartBody::SBObserver
{
public:
	AppListener(); 
	~AppListener();

	virtual void OnCharacterCreate( const std::string & name, const std::string & objectClass );
	virtual void OnCharacterDelete( const std::string & name );
	virtual void OnCharacterUpdate( const std::string & name );
	virtual void OnPawnCreate( const std::string & name );
	virtual void OnPawnDelete( const std::string & name );

	virtual void OnSimulationUpdate();

	virtual void notify(SmartBody::SBSubject* subject);

};

#if __cplusplus
extern "C"
{
#endif
	
	//int cameraMode;
	void SBInitialize(ESContext *esContext);
	void SBSetupDrawing(int w, int h);
	void SBDrawFrame(ESContext *esContext);
	void SBDrawCharacters(ESContext *esContext);
	void SBDrawPawns(ESContext *esContext);
	void SBDrawColObject(ESContext *esContext, SBGeomObject* colObj, float pawnSize);
	void SBUpdate(float t);
	void SBCameraOperation(float dx, float dy);
#if __cplusplus
}
#endif
#endif
