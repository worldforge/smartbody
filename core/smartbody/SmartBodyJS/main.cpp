


#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBJavascript.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBSceneListener.h>

#include <stdlib.h>

#include "esUtil.h"
#include "minimalwrapper.h"
#include <emscripten.h>
#include <emscripten/html5.h>


ESContext esContext;

void Draw ( ESContext *esContext )
{
	SBDrawFrame(esContext);
}

void Update ( ESContext *esContext, float dt){
	SBUpdate(0.1);
}

void one_iter(){
	if (esContext.updateFunc != NULL)
		esContext.updateFunc(&esContext, 0.1);
    if (esContext.drawFunc != NULL)
        esContext.drawFunc(&esContext);
    eglSwapBuffers(esContext.eglDisplay, esContext.eglSurface);
}

int main( int argc, char ** argv )
{ 
	MeshData  meshData;

	esInitContext ( &esContext );
	esContext.meshData = &meshData;
	esCreateWindow ( &esContext, "Simple SmartBody", 1368, 768, ES_WINDOW_RGB );
	SBInitialize(&esContext);
	SBSetupDrawing(esContext.width, esContext.height);
	//setup javascript environment
	esRegisterDrawFunc ( &esContext, Draw );
	esRegisterUpdateFunc( &esContext, Update);

	EM_ASM(
		allReady();
	);
	emscripten_set_main_loop(one_iter, 0, 1);
	
}
