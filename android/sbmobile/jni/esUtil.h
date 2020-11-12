//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

//
/// \file ESUtil.h
/// \brief A utility library for OpenGL ES.  This library provides a
///        basic common framework for the example applications in the
///        OpenGL ES 2.0 Programming Guide. 
//
#ifndef ESUTIL_H
#define ESUTIL_H

///
//  Includes
//
#include <sb/SBTypes.h>
#if defined(__ANDROID__)
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#else 
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif

#if 0
#ifdef __cplusplus
extern "C" {
#endif  
///
//  Macros
//
#define ESUTIL_API
#define ESCALLBACK


/// esCreateWindow flag - RGB color buffer
#define ES_WINDOW_RGB           0
/// esCreateWindow flag - ALPHA color buffer
#define ES_WINDOW_ALPHA         1 
/// esCreateWindow flag - depth buffer
#define ES_WINDOW_DEPTH         2 
/// esCreateWindow flag - stencil buffer
#define ES_WINDOW_STENCIL       4
/// esCreateWindow flat - multi-sample buffer
#define ES_WINDOW_MULTISAMPLE   8


///
// Types
//

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

typedef struct
{
    GLfloat   m[4][4];
} ESMatrix;
#endif
  
typedef struct
{
   // Handle to a program object
   GLuint programObject;
   
   //Attribute locations
   GLint positionLoc;
   GLint texCoordLoc;
   
   //Sampler location
   GLint samplerLoc;

   //Uniform locations
   GLint mvpLoc;
   
} BackgroundData;


typedef struct
{
   // Handle to a program object
   GLuint programObject;
   
   //Attribute locations
   GLint positionLoc;
   GLint normalLoc;
   GLint texCoordLoc;

   GLint boneIDLoc;
   GLint boneWeightLoc;
   GLint tangentLoc;
   
   //Sampler location
   GLint samplerLoc;
   GLint normalMapLoc;
   GLint specularMapLoc;

   //Uniform locations
   GLint mvpLoc;
   GLint mvLoc;
   GLint meshScaleLoc;
   GLint QuaternionLoc;
   GLint TranslationLoc;
   GLint blendTextureLightLoc;

   //material
   GLint mtrlAmbientLoc;
   GLint mtrlDiffuseLoc;
   GLint mtrlShininessLoc;

   //VBOs
   GLuint meshPosObject;
   GLuint meshNormalObject;
   GLuint meshTangentObject;
   GLuint meshTexCoordObject;
   GLuint subMeshTriObject;
   GLuint boneWeightObject;
   GLuint boneIdObject;

   
} UserData;


typedef struct
{
   // Handle to a program object
   GLuint programObject;
   
   //Attribute locations
   GLint positionLoc;
   GLint normalLoc;

   //Uniform locations
   GLint mvpLoc;
   GLint mvLoc;
   GLint colorLoc;
   GLint lightEnabledLoc;

   //VBOs
   GLuint posObject;
   GLuint normalObject;
   GLuint indexObject;
   GLuint jointPosObject;
   GLuint boneIdxObject;

} ShapeData;


typedef struct _escontext
{
	/// Put your user data here...
	UserData*       userData;
	ShapeData*	   shapeData;
	BackgroundData*       backgroundData;

	// fbo data
	GLuint fboTexID;
	GLuint fboID;
	GLuint fboDepthBuf;

	/// Window width
	GLint       width;

	/// Window height
	GLint       height;

#if 0
	/// Window handle
	EGLNativeWindowType  hWnd;

	/// EGL display
	EGLDisplay  eglDisplay;

	/// EGL context
	EGLContext  eglContext;

	/// EGL surface
	EGLSurface  eglSurface;

	/// Callbacks
	void (ESCALLBACK *drawFunc) (struct _escontext *);
	void (ESCALLBACK *keyFunc) (struct _escontext *, unsigned char, int, int);
	void (ESCALLBACK *updateFunc) (struct _escontext *, float deltaTime);
#endif
} ESContext;

extern ESContext esContext;


#if 0
///
//  Public Functions
//

//
///
/// \brief Initialize ES framework context.  This must be called before calling any other functions.
/// \param esContext Application context
//
void ESUTIL_API esInitContext(ESContext *esContext);

//
/// \brief Create a window with the specified parameters
/// \param esContext Application context
/// \param title Name for title bar of window
/// \param width Width in pixels of window to create
/// \param height Height in pixels of window to create
/// \param flags Bitfield for the window creation flags 
///         ES_WINDOW_RGB     - specifies that the color buffer should have R,G,B channels
///         ES_WINDOW_ALPHA   - specifies that the color buffer should have alpha
///         ES_WINDOW_DEPTH   - specifies that a depth buffer should be created
///         ES_WINDOW_STENCIL - specifies that a stencil buffer should be created
///         ES_WINDOW_MULTISAMPLE - specifies that a multi-sample buffer should be created
/// \return GL_TRUE if window creation is succesful, GL_FALSE otherwise
GLboolean ESUTIL_API esCreateWindow(ESContext *esContext, const char *title, GLint width, GLint height, GLuint flags);

//
/// \brief Register a draw callback function to be used to render each frame
/// \param esContext Application context
/// \param drawFunc Draw callback function that will be used to render the scene
//
void ESUTIL_API esRegisterDrawFunc(ESContext *esContext, void (ESCALLBACK *drawFunc) (ESContext*));

//
/// \brief Register an update callback function to be used to update on each time step
/// \param esContext Application context
/// \param updateFunc Update callback function that will be used to render the scene
//
void ESUTIL_API esRegisterUpdateFunc(ESContext *esContext, void (ESCALLBACK *updateFunc) (ESContext*, float));

//
/// \brief Register an keyboard input processing callback function
/// \param esContext Application context
/// \param keyFunc Key callback function for application processing of keyboard input
//
void ESUTIL_API esRegisterKeyFunc(ESContext *esContext,
	void (ESCALLBACK *drawFunc) (ESContext*, unsigned char, int, int));
//
/// \brief Log a message to the debug output for the platform
/// \param formatStr Format string for error log.  
//
void ESUTIL_API esLogMessage(const char *formatStr, ...);

//
/// \brief Loads a 24-bit TGA image from a file
/// \param fileName Name of the file on disk
/// \param width Width of loaded image in pixels
/// \param height Height of loaded image in pixels
///  \return Pointer to loaded image.  NULL on failure. 
//
char* ESUTIL_API esLoadTGA(char *fileName, int *width, int *height);
#ifdef __cplusplus
}
#endif
#endif

#endif // ESUTIL_H
