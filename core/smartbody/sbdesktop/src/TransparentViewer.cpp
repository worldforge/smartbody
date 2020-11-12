
#include "TransparentViewer.h"
#include <windowsx.h>
//#include <GL/glaux.h>



#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
//#pragma comment (lib, "glaux.lib")

#include <assert.h>
#include <tchar.h>

#ifdef  assert
#define verify(expr) if(!expr) assert(0)
#else verify(expr) expr
#endif

#include <sb/SBScene.h>
#include <sr/sr_camera.h>
#include <sr/sr_light.h>
#include <sr/sr_gl.h>
#include <sr/sr_sn_group.h>
#include <external/glew/wglew.h>
#include <vector>
#include <sbm/GPU/SbmShader.h>
#include <sbm/GPU/SbmTexture.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <strsafe.h>

std::string Std_VS =
"varying vec4 vPos;\n\
varying vec3 normal;\n\
void main()\n\
{\n\
	vPos = gl_TextureMatrix[7]* gl_ModelViewMatrix * gl_Vertex;\n\
	gl_Position = ftransform();\n\
	normal = normalize(gl_NormalMatrix * gl_Normal);\n\
	gl_FrontColor = gl_FrontMaterial.diffuse*gl_LightSource[0].diffuse *vec4(max(dot(normal, normalize(gl_LightSource[0].position.xyz)), 0.0));\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;\n\
}";
std::string Std_FS = 
"uniform sampler2D tex;\n\
uniform int useShadowMap;\n\
varying vec4 vPos;\n\
float shadowCoef()\n\
{\n\
	int index = 0;\n\
	vec4 shadow_coord = vPos/vPos.w;//gl_TextureMatrix[1]*vPos;\n\
	shadow_coord.z += 0.000005;\n\
	//shadow_coord.w = shadow_coord.z;\n\
	//shadow_coord.z = float(index);\n\
	float shadow_d = texture2D(tex, shadow_coord.st).x;\n\
	float diff = 1.0;\n\
	diff = shadow_d - shadow_coord.z;\n\
	return clamp( diff*850.0 + 1.0, 0.0, 1.0);//(shadow_d-0.9)*10;//clamp( diff*250.0 + 1.0, 0.0, 1.0);\n\
}\n\
void main()\n\
{\n\
	const float shadow_ambient = 1.0;\n\
	float shadow_coef = 1.0;\n\
	if (useShadowMap == 1) \n\
		shadow_coef = shadowCoef();\n\
	gl_FragColor = gl_Color*shadow_ambient * shadow_coef;\n\
}";


std::string Shadow_FS = 
"void main (void)\n\
{\n\
gl_FragColor = gl_Color;\n\
}";

const int SHADOW_MAP_RES = 2048;


const TCHAR szAppName[]=_T("SmartBody Desktop");
const TCHAR wcWndName[]=_T("SmartBody Desktop");

HDC hDC = 0;            
HGLRC m_hrc = 0;        
int w(320);
int h(240); 
int x(200);
int y(150);

HDC pdcDIB = 0;                 
HBITMAP hbmpDIB = 0;            
BYTE *bmp_cnt(NULL);        
int cxDIB(0); 
int cyDIB(0);   
int pitchDIB = 0;
BITMAPINFOHEADER BIH;   

HDC   g_hPBufferDC = 0;
HGLRC g_hPBufferRC = 0;
HPBUFFERARB g_hPBuffer = 0;
HWND curhWnd = 0;

TransparentViewer* TransparentViewer::viewer = NULL;

void TransparentViewer::resizeViewer()
{
	if (!curhWnd) return;
	TransparentViewer::viewer->CreateDIB(TransparentViewer::viewer->width, TransparentViewer::viewer->height);			
	TransparentViewer::viewer->CreateHGLRC();			
	//verify(wglMakeCurrent(pdcDIB, m_hrc));			
	TransparentViewer::viewer->initSC();
	TransparentViewer::viewer->resizeSC(TransparentViewer::viewer->width, TransparentViewer::viewer->height);
	TransparentViewer::viewer->init_opengl ( TransparentViewer::viewer->width, TransparentViewer::viewer->height); 
	TransparentViewer::viewer->renderSC();
	SbmDeformableMeshGPU::useGPUDeformableMesh = false;
}

LRESULT CALLBACK WindowFunc(HWND hWnd,UINT msg, WPARAM wParam, LPARAM lParam)
{
    //PAINTSTRUCT ps;

    switch(msg) 
    {
        case WM_ERASEBKGND:
            return 0;
        break;

        case WM_CREATE:
        break;

		case WM_NCHITTEST:
			return HTCAPTION;   // allows dragging of the window

		case WM_CLOSE:
			 if(m_hrc)
            {
                wglMakeCurrent(NULL, NULL);
                wglDeleteContext(m_hrc) ;
            }			
            PostQuitMessage(0) ;
		break;

        case WM_DESTROY:
            return 0;
        break;

		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				int msgboxID = MessageBox(
					NULL,
					"Are you sure you want to quit?",
					"Quit SmartBody Desktop",
					MB_OKCANCEL				);

				switch (msgboxID)
				{
				case IDCANCEL:
					break;
				case IDOK:
					SendMessage(curhWnd, WM_CLOSE, 0, 0);	
					break;
				}	
				return 0;
			}
			else if (wParam == 'W')
			{
				SrCamera* camera = SmartBody::SBScene::getScene()->getActiveCamera();
				SrVec eye = camera->getEye();
				eye[1] = eye[1] + .9f;
				camera->setEye(eye[0], eye[1], eye[2]);
			}
			else if (wParam == 'S')
			{
				SrCamera* camera = SmartBody::SBScene::getScene()->getActiveCamera();
				SrVec eye = camera->getEye();
				eye[1] = eye[1] - .9f;
				camera->setEye(eye[0], eye[1], eye[2]);
			}
			else if (wParam == 'D')
			{
				SrCamera* camera = SmartBody::SBScene::getScene()->getActiveCamera();
				SrVec eye = camera->getEye();
				eye[0] = eye[0] + .9f;
				camera->setEye(eye[0], eye[1], eye[2]);
			}
			else if (wParam == 'A')
			{
				SrCamera* camera = SmartBody::SBScene::getScene()->getActiveCamera();
				SrVec eye = camera->getEye();
				eye[0] = eye[0] - .9f;
				camera->setEye(eye[0], eye[1], eye[2]);
			}
			else if (wParam == 'Q')
			{
				SrCamera* camera = SmartBody::SBScene::getScene()->getActiveCamera();
				SrVec eye = camera->getEye();
				eye[2] = eye[2] + .9f;
				camera->setEye(eye[0], eye[1], eye[2]);
			}
			else if (wParam == 'E')
			{
				SrCamera* camera = SmartBody::SBScene::getScene()->getActiveCamera();
				SrVec eye = camera->getEye();
				eye[2] = eye[1] - .9f;
				camera->setEye(eye[0], eye[1], eye[2]);
			}
			break;

        case WM_PAINT:
            //hDC = BeginPaint(hWnd, &ps);
            
            //EndPaint(hWnd, &ps);
        break;

        case WM_SIZE:
            TransparentViewer::viewer->width = LOWORD(lParam); 
			TransparentViewer::viewer->height = HIWORD(lParam);         
//             wglMakeCurrent(NULL, NULL);
//             wglDeleteContext(m_hrc);

			//TransparentViewer::viewer->resizeViewer();
        break;

        default: 
            return DefWindowProc(hWnd,msg,wParam,lParam);
    }

    return 0;
}


TransparentViewer::TransparentViewer()
{
 
   isInitted = false;

}


TransparentViewer::TransparentViewer ( int xpos, int ypos, int width, int height, const char *label )
{
	TransparentViewer();
	TransparentViewer::viewer = this;
	_renders = new TransparencyRenders(this);

	_data.light.init();
	_data.root = new SrSnGroup; 
	_data.shadowmode = false;
	isValid = false;
	w = width;
	h = height;
	x = xpos;
	y = ypos;
}

TransparentViewer::~TransparentViewer ()
{
}

BOOL TransparentViewer::init(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str,int nWinMode)
{
	WNDCLASSEX wc;
    memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEX);    
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WindowFunc;
    wc.cbClsExtra  = 0;
    wc.cbWndExtra  = 0;
    wc.hInstance = hThisInst;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW);
    wc.lpszClassName = szAppName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, _T("RegisterClassEx - failed"), _T("Error"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    curhWnd = CreateWindowEx(WS_EX_LAYERED , szAppName, wcWndName,
                    WS_POPUP|WS_VISIBLE, x, y, w, h,
                    NULL, NULL, hThisInst, NULL);
    if(!curhWnd){
        MessageBox(NULL, _T("CreateWindowEx - failed"), _T("Error"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    //verify(SetLayeredWindowAttributes(curhWnd, 0x0, 0, LWA_COLORKEY));

	isInitted = true;

	return TRUE;

}

SrSn *TransparentViewer::root ()
{
	return _data.root;
}

void TransparentViewer::root ( SrSn *r )
{
	_data.root = r;
}


void TransparentViewer::view_all ()
{
}

void TransparentViewer::render ()
{
 //InvalidateRect(curhWnd, NULL, TRUE);
 //hDC = GetDC(curhWnd); 
 //wglMakeCurrent(g_hPBufferDC, g_hPBufferRC);
 TransparentViewer::viewer->renderSC(); // OpenGL -> DIB
 TransparentViewer::viewer->copyPBufferToImage();
 //TransparentViewer::viewer->draw(hDC);  // DIB -> hDC
 TransparentViewer::viewer->RedrawLayeredWindow();

} 

void TransparentViewer::makeGLContext()
{
}

void TransparentViewer::label_viewer(const char* str)
{
}


SrCamera* TransparentViewer::get_camera()
{
	return &_data.camera;
}

void TransparentViewer::set_camera ( const SrCamera &cam )
{
	_data.camera = cam;
}

void TransparentViewer::show_viewer()
{
}

void TransparentViewer:: hide_viewer()
{
}

BOOL TransparentViewer::initSC()
{
    glEnable(GL_ALPHA_TEST);        
    glEnable(GL_DEPTH_TEST);        
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);          
    glEnable(GL_LIGHT0);            

    glEnable(GL_BLEND);             
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
	//glClearColor(0.3, 0.3, 0.3, 1.0);

    return 0;
}

void TransparentViewer::resizeSC(int width,int height)
{
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW );
    glLoadIdentity();
}

BOOL TransparentViewer::renderSC()
{
	bool hasShaderSupport = initGLExtension();

//  	SbmShaderManager& ssm = SbmShaderManager::singleton();	
//  	bool hasShaderSupport = false;
//  	hasShaderSupport = ssm.initGLExtension();				

	//glViewport ( 0, 0, viewer->width, viewer->height );
	SrMat mat ( SrMat::NotInitialized );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	

	SrCamera* cam = SmartBody::SBScene::getScene()->getActiveCamera();
	if (!cam)
	{
		cam = SmartBody::SBScene::getScene()->createCamera("defaultCamera");
	}
	cam->setAspectRatio((float)viewer->width/(float)viewer->height);
	glMatrixMode ( GL_PROJECTION );
    glLoadMatrix ( _data.camera.get_perspective_mat(mat) );

   glMatrixMode ( GL_MODELVIEW );

   glLoadMatrix ( cam->get_view_mat(mat) );
   glScalef (cam->getScale(), cam->getScale(), cam->getScale() );

   updateLights();
	glEnable ( GL_LIGHTING );
	for (size_t x = 0; x < _lights.size(); x++)
	{
		glLight ( x, _lights[x] );		
	}

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.2f);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.08f);	

	static GLfloat mat_emissin[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_ambient[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_diffuse[] = { 1.0,  1.0,    1.0,    1.0 };
	static GLfloat mat_speclar[] = { 0.0,  0.0,    0.0,    1.0 };
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, mat_emissin );
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_speclar );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0 );
	glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );
	glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_NORMALIZE );

	glDisable( GL_COLOR_MATERIAL );

	if (_data.shadowmode == true && hasShaderSupport)
		makeShadowMap();

	/*
	glPushMatrix();

    glColor3f(0, 1, 1);
    glBegin(GL_TRIANGLES);                              // Drawing Using Triangles
        glColor3f(1.0f,0.0f,0.0f);                      // Set The Color To Red
        glVertex3f( 0.0f, 1.0f, 0.0f);                  // Top
        glColor3f(0.0f,1.0f,0.0f);                      // Set The Color To Green
        glVertex3f(-1.0f,-1.0f, 0.0f);                  // Bottom Left
        glColor3f(0.0f,0.0f,1.0f);                      // Set The Color To Blue
        glVertex3f( 1.0f,-1.0f, 0.0f);                  // Bottom Right
    glEnd();

    glPopMatrix();
	*/

	_renders->drawGrid();
	_renders->drawCharacters();

    //glFlush();

    return 0;
}

void TransparentViewer::copyPBufferToImage()
{
	// Copy the contents of the framebuffer - which in our case is our pbuffer -
	// to our bitmap image in local system memory. Notice that we also need
	// to invert the pbuffer's pixel data since OpenGL by default orients the
	// bitmap image bottom up. Our Windows DIB wrapper expects images to be
	// top down in orientation.
	if (imageBuffer.size() != viewer->width*viewer->height*4)
		imageBuffer.resize(viewer->width*viewer->height*4);

// 	static BYTE pixels[IMAGE_WIDTH * IMAGE_HEIGHT * 4] = {0};
// 
	
 	glPixelStorei(GL_PACK_ALIGNMENT, 1);	
 	glReadPixels(0, 0, viewer->width, viewer->height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, &imageBuffer[0]);
 	for (int i = 0; i < viewer->height; ++i)
 	{
//  		memcpy(&bmp_cnt[pitchDIB * i],
//  			&imageBuffer[((viewer->height - 1) - i) * (viewer->width * 4)],
//  			viewer->width * 4);

		memcpy(&bmp_cnt[pitchDIB * i],
			&imageBuffer[i * (viewer->width * 4)],
			viewer->width * 4);
 	}
	
}

void TransparentViewer::RedrawLayeredWindow()
{
	// The call to UpdateLayeredWindow() is what makes a non-rectangular
	// window possible. To enable per pixel alpha blending we pass in the
	// argument ULW_ALPHA, and provide a BLENDFUNCTION structure filled in
	// to do per pixel alpha blending.	
	HDC hdc = GetDC(curhWnd);

	if (hdc)
	{
		HGDIOBJ hPrevObj = 0;
		POINT ptDest = {0, 0};
		POINT ptSrc = {0, 0};
		SIZE client = {viewer->width, viewer->height};
		BLENDFUNCTION blendFunc = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

		hPrevObj = SelectObject(pdcDIB, hbmpDIB);
		ClientToScreen(curhWnd, &ptDest);

		UpdateLayeredWindow(curhWnd, hdc, &ptDest, &client,
			pdcDIB, &ptSrc, 0, &blendFunc, ULW_ALPHA);

		SelectObject(pdcDIB, hPrevObj);
 		ReleaseDC(curhWnd, hdc);
	}
}

void TransparentViewer::CreateDIB(int cx, int cy)
{
    assert(cx > 0); 
    assert(cy > 0);

    cxDIB = cx ;
    cyDIB = cy ;

    int iSize = sizeof(BITMAPINFOHEADER);   
    memset(&BIH, 0, iSize);

    BIH.biSize = iSize;
    BIH.biWidth = cx;   
    BIH.biHeight = cy;  
    BIH.biPlanes = 1;   
    BIH.biBitCount = 32;    
    BIH.biCompression = BI_RGB;

	pitchDIB = ((viewer->width * 32 + 31) & ~31) >> 3;

    if(pdcDIB) 
        verify(DeleteDC(pdcDIB));

    pdcDIB = CreateCompatibleDC(NULL);	
    assert(pdcDIB);

    if(hbmpDIB) 
        verify(DeleteObject(hbmpDIB));

    hbmpDIB = CreateDIBSection(
        pdcDIB,         
        (BITMAPINFO*)&BIH,  
        DIB_RGB_COLORS,     
        (void**)&bmp_cnt,       
        NULL,
        0);

    assert(hbmpDIB);
    assert(bmp_cnt);

    if(hbmpDIB)
        SelectObject(pdcDIB, hbmpDIB);
}

BOOL TransparentViewer::CreateHGLRC()
{
#if 0 // render to bitmap memory with software openGL
    DWORD dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_BITMAP | PFD_GENERIC_ACCELERATED;// | PFD_DOUBLEBUFFER;

    PIXELFORMATDESCRIPTOR pfd ;
    memset(&pfd,0, sizeof(PIXELFORMATDESCRIPTOR)) ;
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); 
    pfd.nVersion = 1;                       
    pfd.dwFlags =  dwFlags ;                
    pfd.iPixelType = PFD_TYPE_RGBA ;        
    pfd.cColorBits = 24 ;                   
    pfd.cDepthBits = 32 ;                   
    pfd.iLayerType = PFD_MAIN_PLANE ;       

   int PixelFormat = ChoosePixelFormat(pdcDIB, &pfd);
   if (PixelFormat == 0){
      assert(0);
      return FALSE ;
   }

   BOOL bResult = SetPixelFormat(pdcDIB, PixelFormat, &pfd);
   if (bResult==FALSE){
      assert(0);
      return FALSE ;
   }

   m_hrc = wglCreateContext(pdcDIB);
   if (!m_hrc){
      assert(0);
      return FALSE;
   }
#else // use frame buffer object with hardware acceleration instead of render to bitmap
	DWORD dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;

	PIXELFORMATDESCRIPTOR pfd = {0} ;
	memset(&pfd,0, sizeof(PIXELFORMATDESCRIPTOR)) ;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); 
	pfd.nVersion = 1;                       
	pfd.dwFlags =  dwFlags ;                
	pfd.iPixelType = PFD_TYPE_RGBA ;        
	pfd.cColorBits = 24 ;                   
	pfd.cDepthBits = 16 ;                   
	pfd.iLayerType = PFD_MAIN_PLANE ;  

	if (!curhWnd) 
		return false;

	if ( !(hDC = GetDC(curhWnd)) ) 
		return false;

	int PixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (PixelFormat == 0){
		assert(0);
		return FALSE ;
	}

	BOOL bResult = SetPixelFormat(hDC, PixelFormat, &pfd);
	if (bResult==FALSE){		
		DWORD dw =  GetLastError();
		assert(0);
		return FALSE ;
	}

	m_hrc = wglCreateContext(hDC);
	if (!m_hrc){
		assert(0);
		return FALSE;
	}
	if (!wglMakeCurrent(hDC, m_hrc))
		return FALSE;

	if (!initGLExtension()) return FALSE;
	if (!initPBuffer()) return FALSE;

	// release dummy dc
	wglMakeCurrent(hDC, 0);
	ReleaseDC(curhWnd, hDC);
	hDC = 0;
	// We are only doing off-screen rendering. So activate our pbuffer once.
	wglMakeCurrent(g_hPBufferDC, g_hPBufferRC);
#endif
   return TRUE;
}

bool TransparentViewer::initGLExtension()
{
	SbmShaderManager& ssm = SbmShaderManager::singleton();
	SbmTextureManager& texm = SbmTextureManager::singleton();

	if (!isValid)
	{
		bool hasShaderSupport = ssm.initGLExtension();		
		if (hasShaderSupport)
			initShadowMap();
		isValid = true;
	}

	bool hasOpenGL        = ssm.initOpenGL();
	bool hasShaderSupport = false;
	// init OpenGL extension
	if (hasOpenGL)
	{
		hasShaderSupport = ssm.initGLExtension();			
	}
	// update the shader map
	if (hasShaderSupport)
	{
		ssm.buildShaders();
		texm.updateTexture();
	}
	return hasShaderSupport;
}

bool TransparentViewer::initPBuffer()
{
	// Create a pbuffer for off-screen rendering. Notice that since we aren't
	// going to be using the pbuffer for dynamic texturing (i.e., using the
	// pbuffer containing our rendered scene as a texture) we don't need to
	// request for WGL_BIND_TO_TEXTURE_RGBA_ARB support in the attribute list.

	int attribList[] =
	{
		WGL_DRAW_TO_PBUFFER_ARB, TRUE,      // allow rendering to the pbuffer
		WGL_SUPPORT_OPENGL_ARB,  TRUE,      // associate with OpenGL
		WGL_DOUBLE_BUFFER_ARB,   FALSE,     // single buffered
		WGL_RED_BITS_ARB,   8,              // minimum 8-bits for red channel
		WGL_GREEN_BITS_ARB, 8,              // minimum 8-bits for green channel
		WGL_BLUE_BITS_ARB, 8,              // minimum 8-bits for blue channel
		WGL_ALPHA_BITS_ARB, 8,              // minimum 8-bits for alpha channel
		WGL_DEPTH_BITS_ARB, 16,             // minimum 16-bits for depth buffer
		0
	};

	int format = 0;
	UINT matchingFormats = 0;

	if (!wglChoosePixelFormatARB(hDC, attribList, 0, 1, &format, &matchingFormats))
	{
		MessageBox(0, _T("wglChoosePixelFormatARB() failed"), _T("Error"), MB_ICONSTOP);
		return false;
	}

	if (!(g_hPBuffer = wglCreatePbufferARB(hDC, format, viewer->width, viewer->height, 0)))
	{
		MessageBox(0, _T("wglCreatePbufferARB() failed"), _T("Error"), MB_ICONSTOP);
		return false;
	}

	if (!(g_hPBufferDC = wglGetPbufferDCARB(g_hPBuffer)))
	{
		MessageBox(0, _T("wglGetPbufferDCARB() failed"), _T("Error"), MB_ICONSTOP);
		return false;
	}

	if (!(g_hPBufferRC = wglCreateContext(g_hPBufferDC)))
	{
		MessageBox(0, _T("wglCreateContext() failed for PBuffer"), _T("Error"), MB_ICONSTOP);
		return false;
	}

	return true;
}


// DIB -> hDC
void TransparentViewer::draw(HDC pdcDest)
{
	assert(pdcDIB);

    verify(BitBlt(pdcDest, 0, 0, w, h, pdcDIB, 0, 0, SRCCOPY));
}

void TransparentViewer::updateLights()
{
	SrLight light;
	light.directional = false;
	light.directional = true;
	light.diffuse = SrColor( 1.0f, 1.0f, 1.0f );
	light.position = SrVec( 100.0, 250.0, 400.0 );
//	light.constant_attenuation = 1.0f/cam.scale;
	light.constant_attenuation = 1.0f;
	_lights.push_back(light);

	SrLight light2 = light;
	light2.directional = false;
	light2.diffuse = SrColor( 1.0f, 1.0f, 1.0f );
	light2.position = SrVec( 100.0, 500.0, -200.0 );
//	light2.constant_attenuation = 1.0f;
//	light2.linear_attenuation = 2.0f;
	_lights.push_back(light2);
}

void TransparentViewer::init_opengl ( int w, int h )
 {   
   //sr_out<<"INIT"<<srnl;
   glViewport ( 0, 0, w, h );
   glEnable ( GL_DEPTH_TEST );
   glEnable ( GL_LIGHT0 ); 
   glEnable ( GL_LIGHTING );

   //glEnable ( GL_BLEND ); // for transparency
   //glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

   glCullFace ( GL_BACK );
   glDepthFunc ( GL_LEQUAL );
   glFrontFace ( GL_CCW );

   glEnable ( GL_POLYGON_SMOOTH );

   //glEnable ( GL_LINE_SMOOTH );
   //glHint ( GL_LINE_SMOOTH_HINT, GL_NICEST );

   glEnable ( GL_POINT_SMOOTH );
   glPointSize ( 2.0 );

   glShadeModel ( GL_SMOOTH );

   SrCamera& cam = _data.camera;
   //cam.zfar = 1000000;
   float scale = 1.f/SmartBody::SBScene::getScene()->getScale();
   
   // camera near = 0.1 m, camera far plane is 100 m
   cam.setNearPlane(0.1f*scale);
   cam.setFarPlane(1000.f*scale);


   // init shader
   //initShadowMap();
   //SbmShaderManager::singleton().addShader("Basic","",Std_FS.c_str(),false);


   updateLights();
 }

void TransparentViewer::initShadowMap()
{    
	// init basic shader for rendering 
	SbmShaderManager::singleton().addShader("Basic",Std_VS.c_str(),Std_FS.c_str(),false);
	SbmShaderManager::singleton().addShader("BasicShadow","",Shadow_FS.c_str(),false);
	// init shadow map and frame buffer 
	
	int depth_size = SHADOW_MAP_RES;
	//glGenTextures(1, &_data->shadowMapID);
	//SmartBody::util::log("Shadow map ID = %d\n",_data->shadowMapID);	
	glGenFramebuffersEXT(1, &_data.depthFB);	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _data.depthFB);
	glDrawBuffer(GL_FRONT_AND_BACK);
	

	glGenTextures(1, &_data.shadowMapID);
	glBindTexture(GL_TEXTURE_2D, _data.shadowMapID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, depth_size, depth_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, depth_size, depth_size, 0, GL_RGBA, GL_FLOAT, NULL);	
	glBindTexture(GL_TEXTURE_2D,0);


	glGenTextures(1, &_data.depthMapID);
	glBindTexture(GL_TEXTURE_2D, _data.depthMapID);	
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, depth_size, depth_size, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);	
	
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    //glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, depth_size, depth_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D,0);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, _data.depthMapID,0);
	//glBindTexture(GL_TEXTURE_2D, _data->shadowMapID);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D, _data.shadowMapID, 0);

	GLenum FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	/*
	glGenRenderbuffersEXT(1, &_data->rboID);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _data->rboID);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
		depth_size, depth_size);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	*/
}

void TransparentViewer::MakeShadowMatrix( GLfloat points[3][3], GLfloat light[4], GLfloat matrix[4][4] )	
{
	GLfloat plane[ 2 ][ 3 ];
    GLfloat coeff[4];
    GLfloat dot;

    // Find the plane equation coefficients
    // Find the first three coefficients the same way we find a normal.
//    calcNormal(points,planeCoeff);

	plane[ 0 ][ 0 ] = points[ 1 ][ 0 ] - points[ 0 ][ 0 ];
	plane[ 0 ][ 1 ] = points[ 1 ][ 1 ] - points[ 0 ][ 1 ];
	plane[ 0 ][ 2 ] = points[ 1 ][ 2 ] - points[ 0 ][ 2 ];
	plane[ 1 ][ 0 ] = points[ 2 ][ 0 ] - points[ 1 ][ 0 ];
	plane[ 1 ][ 1 ] = points[ 2 ][ 1 ] - points[ 1 ][ 1 ];
	plane[ 1 ][ 2 ] = points[ 2 ][ 2 ] - points[ 1 ][ 2 ];
	
	coeff[ 0 ] = plane[ 0 ][ 1 ] * plane[ 1 ][ 2 ] - plane[ 0 ][ 2 ] * plane[ 1 ][ 1 ];
	coeff[ 1 ] = plane[ 0 ][ 2 ] * plane[ 1 ][ 0 ] - plane[ 0 ][ 0 ] * plane[ 1 ][ 2 ];
	coeff[ 2 ] = plane[ 0 ][ 0 ] * plane[ 1 ][ 1 ] - plane[ 0 ][ 1 ] * plane[ 1 ][ 0 ];

    // Find the last coefficient by back substitutions
    coeff[3] = -( ( coeff[ 0 ] * points[ 2 ][ 0 ] ) + ( coeff[ 1 ] * points[ 2 ][ 1 ] ) + ( coeff[ 2 ] * points[ 2 ][ 2 ] ) );

    // Dot product of plane and light position
    dot = coeff[0] * light[0] + coeff[1] * light[1] + coeff[2] * light[2] + coeff[3] * light[3];

    matrix[0][0] = dot - light[0] * coeff[0];
    matrix[1][0] = 0.0f - light[0] * coeff[1];
    matrix[2][0] = 0.0f - light[0] * coeff[2];
    matrix[3][0] = 0.0f - light[0] * coeff[3];

    matrix[0][1] = 0.0f - light[1] * coeff[0];
    matrix[1][1] = dot - light[1] * coeff[1];
    matrix[2][1] = 0.0f - light[1] * coeff[2];
    matrix[3][1] = 0.0f - light[1] * coeff[3];

    matrix[0][2] = 0.0f - light[2] * coeff[0];
    matrix[1][2] = 0.0f - light[2] * coeff[1];
    matrix[2][2] = dot - light[2] * coeff[2];
    matrix[3][2] = 0.0f - light[2] * coeff[3];

    matrix[0][3] = 0.0f - light[3] * coeff[0];
    matrix[1][3] = 0.0f - light[3] * coeff[1];
    matrix[2][3] = 0.0f - light[3] * coeff[2];
    matrix[3][3] = dot - light[3] * coeff[3];
}

void TransparentViewer::makeShadowMap()
{

	SrLight &light = _data.light;
	float shad_modelview[16];
	
	//	glDisable(GL_LIGHTING);
	//glDisable(GL_TEXTURE_2D);
	// since the shadow maps have only a depth channel, we don't need color computation
	// glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();	
	glLoadIdentity();
	gluPerspective(70,1,30.f,3000.f);
	
	SrLight& shadowLight = _lights[0]; // assume direction light
	SrVec dir = shadowLight.position;
	dir.normalize();
	float distance = 800;
	dir*= distance;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	// the light is from position to origin
	//gluLookAt(light.position[0], light.position[1], light.position[2], 0.f, 0.f, 0.f, 0.0f, 1.0f, 0.0f);
	//gluLookAt(light.position[0], light.position[1], light.position[2], 0,0,0, -1.0f, 0.0f, 0.0f);
	//gluLookAt(0, 0, 0, -light.position[0], -light.position[1], -light.position[2], -1.0f, 0.0f, 0.0f);
	//gluLookAt(0, 600, 700, 0,0,0, 0.0f, 1.0f, 0.0f);
	gluLookAt(dir[0],dir[1],dir[2],0,0,0,0,1,0);
	glGetFloatv(GL_MODELVIEW_MATRIX, shad_modelview);
	// redirect rendering to the depth texture
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _data.depthFB);
	// store the screen viewport
	glPushAttrib(GL_VIEWPORT_BIT);
	int depth_size = SHADOW_MAP_RES;
	// and render only to the shadowmap
	glViewport(0, 0, depth_size, depth_size);
	// offset the geometry slightly to prevent z-fighting
	// note that this introduces some light-leakage artifacts
	//glPolygonOffset( 1.0f, 4096.0f);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	// draw all faces since our terrain is not closed.
	//
	glDisable(GL_CULL_FACE);	
	//glCullFace(GL_FRONT);
	// for all shadow maps:
	// make the current depth map a rendering target
	//glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, depth_tex_ar, 0, i);
	//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, _data->shadowMapID, 0);
	//glBindTexture(GL_TEXTURE_2D, _data->depthMapID);
	//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 
	// clear the depth texture from last time
	//glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	
	// draw the scene
	//terrain->Draw(minZ);	
	_renders->drawCharacters(true);
	glMatrixMode(GL_PROJECTION);
	// store the product of all shadow matries for later
	glMultMatrixf(shad_modelview);
	glGetFloatv(GL_PROJECTION_MATRIX, _data.shadowCPM);
	

	// revert to normal back face culling as used for rendering
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glPopAttrib(); 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	//SbmShaderProgram::printOglError("shadowMapError");

}

