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

# include <sr/sr_gl.h>
# include <sr/sr_sn.h>
# include <sr/sr_gl_render_funcs.h>
# include <sr/sr_sa_render_mode.h>
# include <sr/sr_sa_gl_render.h>
#include <sb/SBUtilities.h>

//# define SR_USE_TRACE1 // constructor / destructor
//# define SR_USE_TRACE2 // render
//# define SR_USE_TRACE3 // matrix
//# include <sr/sr_trace.h>

//=============================== SrOGLData ====================================

struct SrOGLData : public SrSnShapeBase::RenderLibData
 { public :
    unsigned list;
    SrSaGlRender::render_function rfunc;
   public :
    SrOGLData ( SrSnShapeBase* s );
   ~SrOGLData () { 
#if USE_GL_FIXED_PIPELINE
	glDeleteLists(list,1); 
#endif
   }
 };

SrOGLData::SrOGLData ( SrSnShapeBase* s )
 {
   //SmartBody::util::log("SrOGLData constructor");
   int i;
   SrArray<SrSaGlRender::RegData>& rfuncs = SrSaGlRender::_rfuncs;

   //SR_TRACE2 ( "Initializing render data for "<<s->inst_class_name() );
   
   s->changed ( true );
   //SmartBody::util::log("s->changed");
   s->set_renderlib_data ( this );
   //SmartBody::util::log("s->set_render_lib_data()");

   // we could use here a sorted array, but as the number of registered classes
   // is normally low (<10), the overhead is not worth.
   for ( i=0; i<rfuncs.size(); i++ )
     if ( sr_compare(rfuncs[i].class_name,s->inst_class_name())==0 ) break;
   //SmartBody::util::log("after sr_compare, rfunc size = %d",rfuncs.size());
   if ( i==rfuncs.size() )
   {
     SmartBody::util::log("SrSaGlRender: shape [%s] not registered!",s->inst_class_name());
     sr_out.fatal_error("SrSaGlRender: shape [%s] not registered!",s->inst_class_name());
   }

   rfunc = rfuncs[i].rfunc;
#if USE_GL_FIXED_PIPELINE
   list = glGenLists ( 1 ); // creates one list
// sr_out<<"is list: "<<glIsList(list)<<srnl;
#endif

 }

//=============================== SrSaGlRender ====================================

SrArray<SrSaGlRender::RegData> SrSaGlRender::_rfuncs;

void register_render_function ( const char* class_name, SrSaGlRender::render_function rfunc ) // friend
 {
   SrArray<SrSaGlRender::RegData> &rf = SrSaGlRender::_rfuncs;

   int i;
   for ( i=0; i<rf.size(); i++ ) 
    { if ( sr_compare(rf[i].class_name,class_name)==0 ) break;
    }
   if ( i==rf.size() ) rf.push(); // if not found, push a new position

   rf[i].class_name = class_name;
   rf[i].rfunc = rfunc;
   //SmartBody::util::log("render function %s is set to %d, _rfuncs size = %d",class_name, i, rf.size());
 }

SrSaGlRender::SrSaGlRender () 
 { 
   //SR_TRACE1 ( "Constructor" );
   if ( _rfuncs.size()==0 ) // no functions registered	
   {  
      //SmartBody::util::log("register render functions");
      register_render_function ( "model",    SrGlRenderFuncs::render_model );
      register_render_function ( "lines",    SrGlRenderFuncs::render_lines );
      register_render_function ( "points",   SrGlRenderFuncs::render_points );
      register_render_function ( "box",      SrGlRenderFuncs::render_box );
      register_render_function ( "sphere",   SrGlRenderFuncs::render_sphere );
      register_render_function ( "cylinder", SrGlRenderFuncs::render_cylinder );
      register_render_function ( "polygon",  SrGlRenderFuncs::render_polygon );
      register_render_function ( "polygons", SrGlRenderFuncs::render_polygons );
    }
 }

SrSaGlRender::~SrSaGlRender ()
 {
   
   //SR_TRACE1 ( "Destructor" );
 }

void SrSaGlRender::restore_render_mode ( SrSn* n )
 {
   SrSaRenderMode a;
   a.apply ( n );
 }

void SrSaGlRender::override_render_mode ( SrSn* n, srRenderMode m )
 {
   SrSaRenderMode a(m);
   a.apply ( n );
 }

//==================================== virtuals ====================================

void SrSaGlRender::get_top_matrix ( SrMat& mat )
 {
#if USE_GL_FIXED_PIPELINE
   glGetFloatv ( GL_MODELVIEW_MATRIX, (float*)mat );
#endif
 }

int SrSaGlRender::matrix_stack_size ()
{
   int value = 0;
#if USE_GL_FIXED_PIPELINE
   glGetIntegerv ( GL_MODELVIEW_STACK_DEPTH, &value );
#endif
   return value;
 }

void SrSaGlRender::init_matrix ()
 {
   // we do nothing here as SrViewer has already defined
   // the camera/visualization matrix in the stack
 }

void SrSaGlRender::mult_matrix ( const SrMat& mat )
 {
#if USE_GL_FIXED_PIPELINE
   //SR_TRACE3("Mult Matrix");
	 SrMat tm = mat;
   glMultMatrixf ( (float*)tm );
#endif
 }

void SrSaGlRender::push_matrix ()
 {
#if USE_GL_FIXED_PIPELINE
   //SR_TRACE3("Push Matrix");
   glPushMatrix ();
#endif
 }

void SrSaGlRender::pop_matrix ()
 {
#if USE_GL_FIXED_PIPELINE
   //SR_TRACE3("Pop Matrix");
   glPopMatrix ();
#endif
 }

bool SrSaGlRender::shape_apply ( SrSnShapeBase* s )
 {
   // 1. Ensures that render data is initialized
   //SmartBody::util::log("SrSAGLRender::shape_apply(), s = %d",s);
   SrOGLData* ogl = (SrOGLData*) s->get_renderlib_data();
   //SmartBody::util::log("after s->get_renderlib_data(), ogl = %d", ogl);
   if ( !ogl ) {	   
	   ogl = new SrOGLData ( s );
   }
   //SmartBody::util::log("check ogl");
   // 2. Render only if needed
   if ( !s->visible() ) return true;

#if USE_GL_FIXED_PIPELINE // no display list for OpenGL ES Rendering
   //SmartBody::util::log("check isSrModel");
   // 3. Check if lists are up to date
   bool isSrModel = sr_compare(s->inst_class_name(),"model") == 0;

   if ( !s->haschanged() && !isSrModel)
   { //SR_TRACE2 ( "Calling list..." );	  
      glCallList ( ogl->list ); 
      return true;
   }

   // 4. If the list is not up to date, regenerate it calling the render function
   //SR_TRACE2 ( "Creating and executing list of SrSn"<<s->inst_class_name()<<"..." );
  
   if (!isSrModel)
   {
	   glNewList ( ogl->list, GL_COMPILE_AND_EXECUTE );
	   ogl->rfunc ( s );
	   glEndList ();
	   s->changed(false);
   }
   else
#endif
   {
	   //SmartBody::util::log("ogl->rfunc");
	   ogl->rfunc(s);
   }   

   return true;
 }

//======================================= EOF ====================================

