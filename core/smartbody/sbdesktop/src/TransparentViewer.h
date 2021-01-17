# ifndef TRANSPARENT_VIEWER_H
# define TRANSPARENT_VIEWER_H

//#define _WIN32_WINNT 0x0500
#include <GL/glew.h>
#include <windows.h>
#include <sr/sr_viewer.h>
#include <sr/sr_light.h>
#include <sr/sr_event.h>
#include <sr/sr_camera.h>
#include <sr/sr_color.h>
#include <sr/sr_sn_shape.h>
#include <sr/sr_sa_gl_render.h>
#include <sr/sr_sa_bbox.h>
#include <sb/SBObject.h>

#include <GL/glu.h>
#include "TransparencyRenders.h"

class TransparentViewerData
 { public :
   SrSn*  root;              // contains the user scene
  
   SrLight light;

   bool shadowmode;
   SrEvent    event;      // The translated event from fltk to sr format
   SrColor    bcolor;     // Background color currently used
   SrBox      bbox;       // Bounding box of the root, calculated with viewall
   SrCamera   camera;     // The current camera parameters

   SrSnLines* scenebox;  // contains the bounding box to display, and use in view_all
   SrSnLines* sceneaxis; // the current axis being displayed

   SrSaGlRender render_action;
   SrSaBBox bbox_action;

   GLuint  shadowMapID, depthMapID, depthFB, rboID;
   GLfloat shadowCPM[16];
 };


class TransparentViewer : public SrViewer, public SmartBody::SBObserver
 {
	public:

	TransparentViewer();

   TransparentViewer ( int x, int y, int w, int h, const char *label=0 );

    virtual ~TransparentViewer();

	virtual BOOL init(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str,int nWinMode);

    virtual SrSn *root ();

    virtual void root ( SrSn *r );


    virtual void view_all ();

    virtual void render ();

	virtual void makeGLContext();

	virtual void label_viewer(const char* str);

    virtual SrCamera* get_camera();
    virtual void set_camera ( const SrCamera &cam );

	virtual void show_viewer();
	virtual void hide_viewer();

	void resizeSC(int width,int height);
	BOOL initSC();
	BOOL renderSC();
	void resizeViewer();

	void copyPBufferToImage();
	void RedrawLayeredWindow();

	bool initGLExtension();
	bool initPBuffer();
	void CreateDIB(int cx, int cy);
	BOOL CreateHGLRC();
	void draw(HDC pdcDest);
	void updateLights();
	void init_opengl ( int w, int h );
	void initShadowMap();
	void makeShadowMap();
	void MakeShadowMatrix( GLfloat points[3][3], GLfloat light[4], GLfloat matrix[4][4] );


	TransparentViewerData _data;

	static TransparentViewer* viewer;
	bool isInitted;
	bool isValid;

	TransparencyRenders* _renders;
	std::vector<SrLight> _lights;
	std::vector<BYTE> imageBuffer;
	int width;
	int height;

 };


//================================ End of File =================================================

# endif // FLTK_VIEWER_H
