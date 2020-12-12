/*
*  sr_sn_colorsurf.cpp - part of Motion Engine and SmartBody-lib
*  Copyright (C) 2010  University of Southern California
*
*  SmartBody-lib is free software: you can redistribute it and/or
*  modify it under the terms of the Lesser GNU General Public License
*  as published by the Free Software Foundation, version 3 of the
*  license.
*
*  SmartBody-lib is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  Lesser GNU General Public License for more details.
*
*  You should have received a copy of the Lesser GNU General Public
*  License along with SmarBody-lib.  If not, see:
*      http://www.gnu.org/licenses/lgpl-3.0.txt
*
*  CONTRIBUTORS:
*      Marcelo Kallmann, USC (currently UC Merced)
*      David Huang
*/

# include <sr/sr_sn_colorsurf.h>
#if !defined(__ANDROID__) && !defined(SB_IPHONE) && !defined(EMSCRIPTEN)
# include <sr/sr_gl.h>
#endif

//======================================= SrSnColorSurf ====================================

const char* SrSnColorSurf::class_name = "SrSnColorSurf";

SrSnColorSurf::SrSnColorSurf ( const char* classname ) : SrSnShapeBase ( classname )
{
	//SR_TRACE5 ( "Protected Constructor" );
	_model = new SrModel;
	_model->ref();
	surfaceScale = SrVec(1,1,1);
}

SrSnColorSurf::SrSnColorSurf ( SrModel* m ) : SrSnShapeBase ( class_name )
{
	//SR_TRACE5 ( "Constructor" );
	_model = m? m : new SrModel;
	_model->ref();
	surfaceScale = SrVec(1,1,1);
}

SrSnColorSurf::~SrSnColorSurf ()
{
	//SR_TRACE5 ( "Destructor" );
	_model->unref();
}

void SrSnColorSurf::model ( SrModel* m )
{
	if ( _model==m ) return;
	_model->unref();
	_model = m? m : new SrModel;
	_model->ref();
	changed(true);
}

void SrSnColorSurf::get_bounding_box ( SrBox& b ) const
{ 
	_model->get_bounding_box(b);
}

void SrSnColorSurf::gl_prior_render_node () const
{
}

void SrSnColorSurf::gl_render_node(bool alphaBlend) const
{
	//SR_TRACE5 ( "Render "<<instance_name() );

	SrModel& model = *_model;

	//SR_TRACE2 ( "Faces="<<model.F.size() );

	std::vector<SrVec3i>& F = model.F;
	std::vector<SrVec>&         V = model.V;
	std::vector<SrVec>&         N = model.N;
	std::vector<SrMaterial>&    M = model.M;

	int fsize = F.size();
	int vsize = V.size();
	int msize = M.size();
	int nsize = N.size();
	if ( fsize==0 ) return;
	if ( vsize!=msize )
	{ printf("Materials size != vertices size in SrSnColorSurf !"); return; }

#if !defined(__APPLE__) && !defined(__ANDROID__) && !defined(SB_IPHONE) && !defined(EMSCRIPTEN)
	if ( nsize<vsize ) glDisable ( GL_LIGHTING ); else glEnable ( GL_LIGHTING );

	if(alphaBlend)
	{
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND); 
	}

	if ( model.culling ) // back-face culling is enabled/disabled through "model()->culling"
		glEnable ( GL_CULL_FACE );
	else
		glDisable ( GL_CULL_FACE );

   switch ( SrSnShapeBase::render_mode() )
    { case srRenderModeFlat :
           glShadeModel ( GL_FLAT );
           glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
           break;
      case srRenderModeLines :
           glShadeModel ( GL_SMOOTH );
           glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
           break;
      case srRenderModePoints :
           glShadeModel ( GL_SMOOTH );
           glPolygonMode ( GL_FRONT_AND_BACK, GL_POINT );
           break;
      default:
           glShadeModel ( GL_SMOOTH );
           glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
           break;
    }
   glPushMatrix();
   glScalef(surfaceScale[0],surfaceScale[1],surfaceScale[2]);


   int f;
   if ( nsize<vsize )
    { for ( f=0; f<fsize; f++ )
       { SrVec3i& fac = F[f];
         glBegin ( GL_TRIANGLES );
         glColor(M[fac[0]].diffuse); glVertex ( V[fac[0]] );
         glColor(M[fac[1]].diffuse); glVertex ( V[fac[1]] );
         glColor(M[fac[2]].diffuse); glVertex ( V[fac[2]] );
         glEnd (); 
       }
    }
   else
    { for ( f=0; f<fsize; f++ )
       { SrVec3i& fac = F[f];
         glBegin ( GL_TRIANGLES );
         glNormal ( N[fac[0]] ); glMaterial(M[fac[0]]); glVertex ( V[fac[0]] );
         glNormal ( N[fac[1]] ); glMaterial(M[fac[1]]); glVertex ( V[fac[1]] );
         glNormal ( N[fac[2]] ); glMaterial(M[fac[2]]); glVertex ( V[fac[2]] );
         glEnd (); 
       }
    }
   glPopMatrix();
   if(alphaBlend) glDisable(GL_BLEND);
   glEnable ( GL_LIGHTING );
   //SR_TRACE2 ( "End Render "<<instance_name() );
#endif
 }

//================================ EOF =================================================
