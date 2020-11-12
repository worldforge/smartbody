/*  sr_model.cpp - part of Motion Engine and SmartBody-lib
 *  Copyright (C) 2008  University of Southern California
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
 */

# include <stdlib.h>

# include <sr/sr_model.h>
# include <sr/sr_tree.h>
# include <sr/sr_sphere.h>
# include <sr/sr_cylinder.h>
# include <sr/sr_string_array.h>
#include <set>
# include <sr/sr_quat.h>
#include <sb/SBUtilities.h>
#include <cmath>

//# define SR_USE_TRACE1 // IO
//# define SR_USE_TRACE2 // Validation of normals materials, etc
//# include <sr/sr_trace.h>
 
//=================================== SrModel =================================================

class Pt2Comp { // simple comparison function
public:
	bool operator ()(const SrPnt2& x,const SrPnt2 y) const
	{  
		if (x[0] == y[0])
			return x[1] < y[1];
		else
			return x[0] < y[0];
	} // returns x>y
};

const char* SrModel::class_name = "Model";

SrModel::SrModel ()
 {
   culling = true;
   _scale = 1.0;
   _modelTransform = SrMat::id;
 }

SrModel::~SrModel ()
 {
   //SmartBody::util::log("Delete SrModel, name = '%s'", (const char*)name);
 }

void SrModel::init ()
 {
   culling = true;
   //M.capacity ( 0 );
   //V.capacity ( 0 );
   //Vc.capacity( 0 );
   //N.capacity ( 0 );
   //T.capacity ( 0 );
   //F.capacity ( 0 );
   //Fm.capacity ( 0 );
   //Fn.capacity ( 0 );
   //Ft.capacity ( 0 );
   //mtlnames.capacity ( 0 );
   name = "";
 }

void SrModel::compress ()
 {
   //M.compress();
   //V.compress();
   //Vc.compress();
   //N.compress();
   //T.compress();
   //F.compress();
   //Fm.compress();
   //Fn.compress();
   //Ft.compress();
   //mtlnames.compress();
   name.compress();
 }

void SrModel::validate ()
 {
   int i, j;
   SrArray<int> iarray;
   SrStringArray sarray;

   int fsize = F.size();

   // check if the model is empty
   if ( fsize==0 || V.size()==0 )
    { 
	   // preserve the name
	   std::string curName = this->name;
	   init ();
	   this->name = SrString(curName.c_str());
      compress ();
      return;
    }

   // check size of Fn
   if ( Fn.size()!=fsize || N.size()==0 )
    { Fn.resize(0);
      N.resize(0);
    }

   // check size of Ft
   if ( Ft.size()!=fsize || T.size()==0 )
    { Ft.resize(0);
      T.resize(0);
    }

   // check size of Fm
   if ( M.size()==0 )
    { Fm.resize(0);
	  mtlnames.resize(0);
    }
   else if ( Fm.size()!=fsize )
    { j = Fm.size();
      Fm.resize ( fsize );
      for ( i=j; i<fsize; i++ ) Fm[i]=0;
    }

   mtlFaceIndices.clear(); // remove all mtlFaceIndices
   for (int i=0;i<Fm.size();i++)
   {
	   int matIdx = Fm[i];
	   std::string matName = "none";	   
	   if (matIdx < mtlnames.size() && matIdx >= 0)
	   {
		   matName = mtlnames[matIdx];
	   }
	   if (mtlFaceIndices.find(matName) == mtlFaceIndices.end())
		   mtlFaceIndices[matName] = std::vector<int>();
	   mtlFaceIndices[matName].push_back(i);
   }   
 }

void SrModel::remove_redundant_materials ()
 {
   int i, j, k;
   SrArray<int> iarray;

   int fsize = F.size();

   if ( M.size()==0 )
    { Fm.resize(0);
    }
   else 
    { j = Fm.size();
      Fm.resize ( fsize );
      for ( i=j; i<fsize; i++ ) Fm[i]=0;

      // remove references to duplicated materials
      int msize = M.size();
      for ( i=0; i<msize; i++ ) 
      { 
		  for ( j=i+1; j<msize; j++ ) 	      
          {  
			  std::string m1,m2;
			  m1 = mtlnames[i];
			  m2 = mtlnames[j];
			  if ( M[i]==M[j] && m1 == m2)
              { //SR_TRACE2 ( "Detected material "<<i<<" equal to "<<j );
                for ( k=0; k<fsize; k++ ) // replace references to j by i
                  if ( Fm[k]==j ) Fm[k]=i;
              }
          }       
	  }

      // check for nonused materials
      iarray.size ( M.size() );
      iarray.setall ( -1 );

      for ( i=0; i<fsize; i++ ) 
       { if ( Fm[i]>=0 && Fm[i]<M.size() )
          iarray[Fm[i]] = 0; // mark used materials
         else Fm[i] = -1;
       }
      int toadd = 0;
      for ( i=0; i<iarray.size(); i++ ) 
       { if ( iarray[i]<0 )
          { //SR_TRACE2 ( "Detected unused material "<<i );
            //sarray.set ( i, 0 ); 
            toadd++;
          }
         else
          iarray[i] = toadd;
       }

/*      for ( i=0; i<iarray.size(); i++ ) // update material names indices
       { if ( iarray[i]<0 ) continue;
         mtlnames.set ( i-iarray[i], mtlnames[i] );
         //material_names << i-iarray[i] << srspc << sarray[i] << srspc;
       }*/
      //k = material_names.len();
      //if ( k>0 ) material_names[k-1]=0; // remove the last space

      for ( i=0; i<fsize; i++ ) // update indices
       { if ( Fm[i]<0 ) continue;
         Fm[i] -= iarray[Fm[i]];
       }
      for ( i=0,j=0; i<iarray.size(); i++ ) // compress materials
       { if ( iarray[i]<0 )
          { M.erase(M.begin()+j);
            mtlnames.erase(mtlnames.begin()+j);
          }
         else
          { j++; }
       }
    }
 }


void SrModel::remove_redundant_texcoord()
{
	int i, j, k;
	float ang;
	float texDiff;
	SrArray<int> iarray;

	int fsize = F.size();

	if ( T.size()==0 || Ft.size()!=fsize )
	{ T.resize(0); 
	Ft.resize(0);
	}
	else 
	{ // remove references to duplicated normals
		int nsize = T.size();
		std::map<SrPnt2, int, Pt2Comp> tMap;
		std::map<int,int> tNewIdxMap;
		std::vector<SrPnt2>  newTexCoordList;
		for (int i=0;i<T.size();i++)
		{
			if (tMap.find(T[i]) == tMap.end())					
			{
				int newIdx = newTexCoordList.size();
				tMap[T[i]] = newIdx;
				tNewIdxMap[i] = newIdx;
				newTexCoordList.push_back(T[i]);
			}
			else
			{
				tNewIdxMap[i] = tMap[T[i]];
			}
		}

		for ( k=0; k<fsize; k++ ) // replace references to j by i
		{ 
			for (int i=0;i<3;i++)
			{
				int oldIdx = Ft[k][i];
				Ft[k][i] = tNewIdxMap[oldIdx];
			}
		}
		T.resize(newTexCoordList.size());
		for (unsigned int i=0;i<newTexCoordList.size();i++)
			T[i] = newTexCoordList[i];
#if 0
		for ( i=0; i<nsize; i++ ) 
		{ for ( j=i+1; j<nsize; j++ ) 
		{ texDiff = (T[i]-T[j]).norm();
		if ( texDiff < 1e-9 )
		{ //SR_TRACE2 ( "Detected normal "<<i<<" close to "<<j );
			for ( k=0; k<fsize; k++ ) // replace references to j by i
			{ if ( Ft[k].a==j ) Ft[k].a=i;
			  if ( Ft[k].b==j ) Ft[k].b=i;
			  if ( Ft[k].c==j ) Ft[k].c=i;
			}
		}
		}
		}

		// check for nonused materials
		iarray.size ( nsize );
		iarray.setall ( -1 );

		for ( i=0; i<fsize; i++ )  // mark used materials
		{ iarray[Ft[i].a] = 1;
		iarray[Ft[i].b] = 1;
		iarray[Ft[i].c] = 1;
		}

		int toadd = 0;
		for ( i=0; i<iarray.size(); i++ ) 
		{ if ( iarray[i]<0 )
		{ //SR_TRACE2 ( "Detected unused normal "<<i );
			toadd++;
		}
		else
			iarray[i] = toadd;
		}

		for ( i=0; i<fsize; i++ ) // update indices
		{ Ft[i].a -= iarray[Ft[i].a];
		Ft[i].b -= iarray[Ft[i].b];
		Ft[i].c -= iarray[Ft[i].c];
		}

		for ( i=0,j=0; i<iarray.size(); i++ ) // compress T
		{ if ( iarray[i]<0 )
		{ T.remove(j); }
		else
		{ j++; }
		}
#endif
	}

}

void SrModel::remove_redundant_normals ( float dang )
 {
   int i, j, k;
   float ang;
   SrArray<int> iarray;

   int fsize = F.size();

   if ( N.size()==0 || Fn.size()!=fsize )
    { N.resize(0); 
      Fn.resize(0);
    }
   else 
    { // remove references to duplicated normals
      int nsize = N.size();
      for ( i=0; i<nsize; i++ ) 
       { for ( j=i+1; j<nsize; j++ ) 
          { ang = angle ( N[i],N[j] );
            if ( ang<=dang )
             { //SR_TRACE2 ( "Detected normal "<<i<<" close to "<<j );
               for ( k=0; k<fsize; k++ ) // replace references to j by i
                { if ( Fn[k][0]==j ) Fn[k][0]=i;
                  if ( Fn[k][1]==j ) Fn[k][1]=i;
                  if ( Fn[k][2]==j ) Fn[k][2]=i;
                }
             }
          }
       }

      // check for nonused materials
      iarray.size ( nsize );
      iarray.setall ( -1 );

      for ( i=0; i<fsize; i++ )  // mark used materials
       { iarray[Fn[i][0]] = 1;
         iarray[Fn[i][1]] = 1;
         iarray[Fn[i][2]] = 1;
       }

      int toadd = 0;
      for ( i=0; i<iarray.size(); i++ ) 
       { if ( iarray[i]<0 )
          { //SR_TRACE2 ( "Detected unused normal "<<i );
            toadd++;
          }
         else
          iarray[i] = toadd;
       }

      for ( i=0; i<fsize; i++ ) // update indices
       { Fn[i][0] -= iarray[Fn[i][0]];
         Fn[i][1] -= iarray[Fn[i][1]];
         Fn[i][2] -= iarray[Fn[i][2]];
       }

      for ( i=0,j=0; i<iarray.size(); i++ ) // compress N
       { if ( iarray[i]<0 )
          { 
			  //N.remove(j); 
			  N.erase(N.begin()+j);
		  }
         else
          { j++; }
       }
    }
 }

std::vector<int> SrModel::merge_redundant_vertices ( float prec )
 {
   prec = prec*prec;
   
   int fsize = F.size();
   int vsize = V.size();
   int i, j;

   // build iarray marking replacements:
   std::vector<int> iarray;
   iarray.resize ( vsize );
   for ( i=0; i<vsize; i++ ) iarray[i]=i;
   
   for ( i=0; i<vsize; i++ )
    for ( j=i+1; j<vsize; j++ )
     { //if ( i==j ) break; // keep i < j
       if ( dist2(V[i],V[j])<prec ) // equal
        { iarray[j]=iarray[i];
        }
     }

   // fix face indices:
   for ( i=0; i<fsize; i++ )
    { F[i][0] = iarray[ F[i][0] ];
      F[i][1] = iarray[ F[i][1] ];
      F[i][2] = iarray[ F[i][2] ];
    }

   // compress indices:   
   int ind=0;
   bool newv;
   std::vector<int> newArrayMap(vsize);
   for ( i=0; i<vsize; i++ )
    { newv = iarray[i]==i? true:false;
      V[ind] = V[i];
      newArrayMap[i] = ind;
      if ( newv ) ind++;
    }
   V.resize ( ind );
   // fix face indices again:
   for ( i=0; i<fsize; i++ )
    { F[i][0] = newArrayMap[ F[i][0] ];
      F[i][1] = newArrayMap[ F[i][1] ];
      F[i][2] = newArrayMap[ F[i][2] ];
    }

   for (unsigned int i=0;i<iarray.size();i++)
	   iarray[i] = newArrayMap[iarray[i]];

   return iarray;
 }

bool SrModel::load ( SrInput &in )
 {
   int i;
   SrString s;

   if ( !in.valid() ) return false;
   // ensure proper comment style
   in.comment_style ( '#' );

   // check signature
   in >> s;
   if ( s!="SrModel" ) return false;

   // clear arrays and set culling to true
   init ();

   while ( !in.finished() )
    { 
      if ( !in.read_field(s) )
       { if ( in.finished() ) break; // EOF reached
         return false;
       }

      //SR_TRACE1 ( '<' << s << "> Field found...\n" );

      if ( s=="culling" ) // read culling state
       { in >> i; 
         culling = i? true:false;
         if ( !in.close_field(s) ) return false;
       }
      else if ( s=="name" ) // read name (is a SrInput::String type )
       { in.get_token();
         name = in.last_token();
         if ( !in.close_field(s) ) return false;
       }
      else if ( s=="vertices" ) // read vertices: x y z
       { in >> i; V.resize(i);
         for ( i=0; i<V.size(); i++ ) 
          fscanf ( in.filept(), "%f %f %f", &V[i].x, &V[i].y, &V[i].z ); // equiv to: in >> V[i];
         if ( !in.close_field(s) ) return false;
       }
      else if ( s=="vertices_per_face" ) // read F: a b c
       { in >> i; F.resize(i);
         for ( i=0; i<F.size(); i++ )
          {
#if defined(__ANDROID__) || defined(SB_IPHONE) || defined(EMSCRIPTEN)
            fscanf ( in.filept(), "%hud %hud %hud", &F[i][0], &F[i][1], &F[i][2] ); // equiv to: in >> F[i];
#else
            fscanf ( in.filept(), "%d %d %d", &F[i][0], &F[i][1], &F[i][2] ); // equiv to: in >> F[i];
#endif
            //F[i].validate();
          }
         if ( !in.close_field(s) ) return false;
       }
      else if ( s=="normals_per_face" ) // read Fn: a b c
       { in >> i; Fn.resize(i);
         for ( i=0; i<Fn.size(); i++ )
#if defined(__ANDROID__) || defined(SB_IPHONE) || defined(EMSCRIPTEN)
           fscanf ( in.filept(), "%hud %hud %hud", &Fn[i][0], &Fn[i][1], &Fn[i][2] ); // equiv to: in >> Fn[i];
#else
          fscanf ( in.filept(), "%d %d %d", &Fn[i][0], &Fn[i][1], &Fn[i][2] ); // equiv to: in >> Fn[i];
#endif
         if ( !in.close_field(s) ) return false;
       }
      else if ( s=="textcoords_per_face" ) // read Ft: a b c
       { in >> i; Ft.resize(i);
         for ( i=0; i<Ft.size(); i++ ) in >> Ft[i];
         if ( !in.close_field(s) ) return false;
       }
      else if ( s=="materials_per_face" ) // read Fm: i
       { in >> i; Fm.resize(i);
         for ( i=0; i<Fm.size(); i++ ) fscanf ( in.filept(), "%d", &Fm[i] ); // equiv to: in >> Fm[i];
         if ( !in.close_field(s) ) return false;
       }
      else if ( s=="normals" ) // read N: x y z
       { in >> i; N.resize(i);
         for ( i=0; i<N.size(); i++ )
          fscanf ( in.filept(), "%f %f %f", &N[i].x, &N[i].y, &N[i].z ); // equiv to: in >> N[i];
         if ( !in.close_field(s) ) return false;
       }
      else if ( s=="textcoords" ) // read T: x y
       { in >> i; T.resize(i);
         for ( i=0; i<T.size(); i++ ) in >> T[i];
         if ( !in.close_field(s) ) return false;
       }
      else if ( s=="materials" ) // read M: mtls
       { in >> i; M.resize(i);
         for ( i=0; i<M.size(); i++ ) in >> M[i];
         if ( !in.close_field(s) ) return false;
       }
      else if ( s=="material_names" ) // read materials
       { SrString buf1, buf2;
         //mtlnames.capacity ( 0 ); // clear all
		 mtlnames.clear();
         mtlnames.resize ( M.size() ); // realloc

         while ( 1 )
          { if ( in.get_token()!=SrInput::Integer ) { in.unget_token(); break; }
            i = atoi ( in.last_token() );
            in.get_token();
            mtlnames[i] = in.last_token() ;
          }
         if ( !in.close_field(s) ) return false;
       }
      else // unknown field, just skip it:
       { if ( !in.skip_field(s) ) return false;
       }
    }

   //SR_TRACE1 ( "OK.\n" );

   return true;
 }

 void SrModel::computeTangentBiNormal()
 {	 
	 int nVtx = V.size();
	 Tangent.resize(nVtx); Tangent.assign(Tangent.size(),SrVec(0,0,0));//Tangent.setall(SrVec(0,0,0));
	 BiNormal.resize(nVtx); BiNormal.assign(BiNormal.size(),SrVec(0,0,0));//BiNormal.setall(SrVec(0,0,0));
	 //N.setall(SrVec(0,0,0));

	 if (Ft.size() != F.size())
		 return;
	 for (int i=0;i<F.size();i++)
	 {
		 int idx[3]; idx[0] = F[i][0]; idx[1] = F[i][1]; idx[2] = F[i][2];
		 int tidx[3]; tidx[0] = Ft[i][0]; tidx[1] = Ft[i][1]; tidx[2] = Ft[i][2];
		 SrVec sv[2];
		 SrPnt2 st[2];		
		 if (idx[0] < 0 || 
			 idx[1] < 0 ||
			 idx[2] < 0)
		 {
			 // bad indices
		 }
		 else if (tidx[0] < 0 ||
			 tidx[1] < 0 ||
			 tidx[2] < 0)
		 {
			 // bad indices
		 }
		 else if (tidx[0] >=  T.size() || tidx[1] >= T.size() || tidx[2] >= T.size())
		 {
			 // bad indices
		 }
		 else
		 {
			 for (int k=0;k<2;k++)
			 {
				 sv[k] = V[idx[k+1]] - V[idx[0]];
				 st[k] = T[tidx[k+1]] - T[tidx[0]];
			 }	
		 }
		 float cp = st[0].y * st[1].x - st[0].x * st[1].y;
		 SrVec nvec = cross(sv[0],sv[1]); nvec.normalize(); 
		 SrVec tvec = (sv[0]*st[1].y - sv[1]*st[0].y)/cp; tvec.normalize();
		 SrVec bvec = (sv[0]*st[1].x - sv[1]*st[0].x)/(-cp); bvec.normalize();		

		 if (dot(cross(tvec,bvec),nvec) < -0.1f)
		 {
			 //printf("mirror texture coord...\n");
			 bvec = -bvec;
		 }
		 
		  if (idx[0] < 0 || 
			 idx[1] < 0 ||
			 idx[2] < 0)
		 {
			 // bad indices
		 }
		 else
		 {
			 for (int k=0;k<3;k++)
			 {
				 Tangent[idx[k]] += tvec;
				 BiNormal[idx[k]] += bvec;
				 //N[idx[k]] += nvec;
			 }
		 }
	 }

	 for (int i=0;i<nVtx;i++)
	 {		
		 Tangent[i].normalize();
		 BiNormal[i].normalize();	
		 //if (Tangent[i].norm() > 1.01f || Tangent[i].norm() < 0.99f)
		 //	 SmartBody::util::log("Error, tangent for vtx %d is %s", i, Tangent[i].toString().c_str());
		 /*
		 if (std::isnan(Tangent[i][0]) || std::isnan(Tangent[i][1]) || std::isnan(Tangent[i][2]))
			 SmartBody::util::log("Error, tangent for vtx %d is nan", i);
		 if (std::isnan(BiNormal[i][0]) || std::isnan(BiNormal[i][1]) || std::isnan(BiNormal[i][2]))
			 SmartBody::util::log("Error, binormal for vtx %d is nan", i);
		  */
		 //N[i].normalize();
	 }
 }

bool SrModel::save ( SrOutput &o ) const
 {
   int i;

   // save header as a comment
   o << "# SR - Simulation and Representation Toolkit\n" <<
        "# Marcelo Kallmann 1996-2004\n\n";

   // save signature
   o << "SrModel\n\n";

   // save name
   if ( name.len()>0 )
    { o << "<name> \"" << name << "\" </name>\n\n"; }

   // save culling state
   if ( !culling )
    { o << "<culling> 0 </culling>\n\n"; }

   // save vertices (V)
   if ( V.size() ) 
    { o << "<vertices> " << (int) V.size() << srnl;
      for ( i=0; i<V.size(); i++ ) o << V[i] << srnl;
      o << "</vertices>\n\n";
    }

   // save faces (F)
   if ( F.size() )
    { o << "<vertices_per_face> " << (int) F.size() << srnl;
      for ( i=0; i<F.size(); i++ ) o << F[i] << srnl;
      o << "</vertices_per_face>\n\n";
    }

   // save normals (N)
   if ( N.size() )
    { o << "<normals> " << (int) N.size() << srnl;
      for ( i=0; i<N.size(); i++ ) o << N[i] << srnl;
      o << "</normals>\n\n";
    }

   // save normals per face (Fn)
   if ( Fn.size() )
    { o << "<normals_per_face> " << (int) Fn.size() << srnl;
      for ( i=0; i<Fn.size(); i++ ) o << Fn[i] << srnl;
      o << "</normals_per_face>\n\n";
    }

   // save texture coordinates (T)
   /* Not yet supported so not saved
   if ( T.size() )
    { o << "<textcoords> " << N.size() << srnl;
      for ( i=0; i<T.size(); i++ ) o << T[i] << srnl;
      o << "</textcoords>\n\n";
    } */

   // save texture coordinates per face (Ft)
   /* Not yet supported so not saved
   if ( Ft.size() )
    { o << "<textcoords_per_face> " << Ft.size() << srnl;
      for ( i=0; i<Ft.size(); i++ ) o << Ft[i] << srnl;
      o << "</textcoords_per_face>\n\n";
    } */

   // save materials (M)
   if ( M.size() )
    { o << "<materials> " << (int) M.size() << srnl;
      for ( i=0; i<M.size(); i++ ) o << M[i] << srnl;
      o << "</materials>\n\n";
    }

   // save materials per face (Fm)
   if ( Fm.size() )
    { o << "<materials_per_face> " << (int) Fm.size() << srnl;
      for ( i=0; i<Fm.size(); i++ ) o << Fm[i] << srnl;
      o << "</materials_per_face>\n\n";
    }

   // save material names if there is one:
   bool savemtl=false;
   for ( i=0; i<mtlnames.size(); i++ )
    { if ( mtlnames[i][0] ) { savemtl=true; break; }
    }
   
   if ( savemtl )
    { o << "<material_names> " << srnl;
      for ( i=0; i<mtlnames.size(); i++ )
       { 
		   if ( mtlnames[i].size() > 0 ) 
			   o << i << srspc << mtlnames[i].c_str() << srnl;
       }
      o << "</material_names>\n\n";
    }

   // done.
   return true;
 }

struct EdgeNode : public SrTreeNode // EdgeNode is only internally used :
 { int a, b;
   EdgeNode ( int x, int y ) : a(x), b(y) {}
   EdgeNode () : a(0), b(0) {}
   EdgeNode ( const EdgeNode& e ) : a(e.a), b(e.b) {}
  ~EdgeNode () {}
   friend SrOutput& operator<< ( SrOutput& out, const EdgeNode& e ) { return out; };
   friend SrInput& operator>> ( SrInput& inp, EdgeNode& e ) { return inp; }
   friend int sr_compare ( const EdgeNode* e1, const EdgeNode* e2 )
    { return e1->a!=e2->a ? e1->a-e2->a : e1->b-e2->b; }
 };

void SrModel::make_edges ( SrArray<int> &E )
 {
   SrTree<EdgeNode> t;

   E.size(0);
   if ( F.empty() ) return;
   
   for (size_t i=0; i<F.size(); i++ )
    { t.insert_or_del ( new EdgeNode ( SR_MIN(F[i][0],F[i][1]), SR_MAX(F[i][0],F[i][1]) ) );
      t.insert_or_del ( new EdgeNode ( SR_MIN(F[i][1],F[i][2]), SR_MAX(F[i][1],F[i][2]) ) );
      t.insert_or_del ( new EdgeNode ( SR_MIN(F[i][2],F[i][0]), SR_MAX(F[i][2],F[i][0]) ) );
    }
   E.size ( 2*t.elements() );
   E.size ( 0 );
   for ( t.gofirst(); t.cur()!=SrTreeNode::null; t.gonext() )
    { E.push() = t.cur()->a;
      E.push() = t.cur()->b;
    }
 }

float SrModel::count_mean_vertex_degree ()
 {
   if ( F.empty() ) return 0.0f;

   SrArray<int> vi(V.size());
   for (size_t i=0; i<vi.size(); i++ ) vi[i]=0;

   for (size_t i=0; i<F.size(); i++ )
    { vi[F[i][0]]++; vi[F[i][1]]++; vi[F[i][2]]++; }

   double k=0;
   for (size_t i=0; i<vi.size(); i++ ) k += double(vi[i]);
   return float( k/double(vi.size()) );

   /* old way:
   SrTree<srEdgeNode> t;
   
   for ( i=0; i<F.size(); i++ )
    { t.insert_or_del ( new srEdgeNode ( F[i].a, F[i].b ) );
      t.insert_or_del ( new srEdgeNode ( F[i].b, F[i].a ) );
      t.insert_or_del ( new srEdgeNode ( F[i].b, F[i].c ) );
      t.insert_or_del ( new srEdgeNode ( F[i].c, F[i].b ) );
      t.insert_or_del ( new srEdgeNode ( F[i].c, F[i].a ) );
      t.insert_or_del ( new srEdgeNode ( F[i].a, F[i].c ) );
    }

   return (float)t.size() / (float)V.size(); 
   */
 }

void SrModel::get_bounding_box ( SrBox &box ) const
 {
   box.set_empty ();
   if ( V.empty() ) return;
   int i, s=V.size();
   for ( i=0; i<s; i++ ) box.extend ( V[i] );
 }

void SrModel::translate ( const SrVec &tr )
 {
	saveOriginalVertices();

	//_translation = tr;
	SrMat transMat; transMat.set_translation(tr);
	_modelTransform = _modelTransform*transMat;

	recalculateVertices();
 }

void SrModel::rotate( const SrVec &r )
 {
	saveOriginalVertices();

	//_rotation = r;
	SrQuat rotQuat = SrQuat(r); // use axis angle instead of Euler angle ?	
	SrMat rotMat; rotQuat.get_mat(rotMat);
	_modelTransform = _modelTransform*rotMat;

	recalculateVertices();
 }

void SrModel::scale ( float factor )
 {
   saveOriginalVertices();

   //_scale = factor;
   SrMat scaleMat; scaleMat.scale(factor);
   _modelTransform = _modelTransform*scaleMat;

	recalculateVertices();
 }


SBAPI void SrModel::resetTransform()
{
	saveOriginalVertices();
	
	// reset model transform to identity matrix
	_modelTransform = SrMat::id;

	recalculateVertices();
}

SBAPI void SrModel::addTransform( const SrMat& transform )
{
	saveOriginalVertices();

	// add new transform on top of current model transform
	_modelTransform = _modelTransform*transform;

	recalculateVertices();
}



void SrModel::recalculateVertices()
{
#if 0
	SrVec xaxis(1, 0, 0);
	SrVec yaxis(0, 1, 0);
	SrVec zaxis(0, 0, 1);
	SrQuat xrot(xaxis, _rotation[0]);
	SrQuat yrot(yaxis, _rotation[1]);
	SrQuat zrot(zaxis, _rotation[2]);
	SrQuat finalRot = xrot * yrot * zrot;
#endif

	SrQuat finalRot = SrQuat(_rotation); // use axis angle instead of Euler angle ?

	int i, s=V.size();
	for ( i=0; i<s; i++ ) 
	{
		//SrVec vTemp = VOrig[i]*_scale;
		//V[i] = vTemp*finalRot + _translation;
		V[i] = VOrig[i] * _modelTransform;
		//V[i] = ((VOrig[i] + _translation) * _scale * finalRot);
		//V[i] = (VOrig[i] * _scale) * finalRot + _translation;
	}
}


void SrModel::centralize ()
 {
   SrVec v; SrBox box;

   get_bounding_box(box);

   v = box.center() * -1.0;
   translate ( v );
 }

void SrModel::normalize ( float maxcoord )
 {
   SrVec boxCenter;
   SrVec p; 
   SrBox box;

   get_bounding_box(box);

   boxCenter = box.center() * -1.0;
   //translate ( p );

   box+=boxCenter;
   SR_POS(box.a.x); SR_POS(box.a.y); SR_POS(box.a.z);
   SR_POS(box.b.x); SR_POS(box.b.y); SR_POS(box.b.z);

   p.x = SR_MAX(box.a.x,box.b.x);
   p.y = SR_MAX(box.a.y,box.b.y);
   p.z = SR_MAX(box.a.z,box.b.z);

   float maxactual = SR_MAX3(p.x,p.y,p.z);
   float resizeScale = maxcoord/maxactual;   
   scale ( resizeScale );  // Now we normalize to get the desired radius
   SrVec newP = boxCenter*resizeScale;
   translate(newP);
 }

struct VertexNode : public SrTreeNode // only internally used
 { int v, i, f;
   VertexNode ( int a, int b, int c ) : v(a), i(b), f(c) {}
   VertexNode () { v=i=f=0; }
   VertexNode ( const VertexNode& x ) : v(x.v), i(x.i), f(x.f) {}
  ~VertexNode () {}
   friend SrOutput& operator<< ( SrOutput& out, const VertexNode& v ) { return out; };
   friend SrInput& operator>> ( SrInput& inp, VertexNode& v ) { return inp; }
   friend int sr_compare ( const VertexNode* v1, const VertexNode* v2 )
    { return v1->v!=v2->v ? v1->v-v2->v   // vertices are different
                          : v1->i-v2->i;  // vertices are equal: use index i
    }
 };

static void insertv ( SrTree<VertexNode>& t, SrArray<int>& vi, int v, int f )
 {
   // array vi is only used to generated a suitable tree key sorting the vertices.
   VertexNode *n = new VertexNode(v,++vi[v],f);
   if ( !t.insert(n) ) sr_out.fatal_error("Wrong faces in SrModel::smooth ()!\n");
 }

int SrModel::common_vertices_of_faces ( int i1, int i2 )
 {
   int i, j, c=0;
#if defined(__ANDROID__) || defined(SB_IPHONE) || defined(EMSCRIPTEN)
   unsigned short *f1 = &(F[i1][0]);
   unsigned short *f2 = &(F[i2][0]);
#else
   int *f1 = &(F[i1][0]);
   int *f2 = &(F[i2][0]);
#endif
   for ( i=0; i<3; i++ )
    { for ( j=0; j<3; j++ )
       { if ( f1[i]==f2[j] ) c++; //sr_out<<i<<","<<j<<srspc;
       }
    }
   return c;
 }

void SrModel::flat ()
 {
   N.resize(0);
   Fn.resize(0);
   compress ();
 }

void SrModel::set_one_material ( const SrMaterial& m )
 {
   mtlnames.resize(1);
   //mtlnames.size(1);
   M.resize(1);
   Fm.resize ( F.size() );
   M[0] = m;
   for (size_t i=0; i<Fm.size(); i++ ) Fm[i]=0;
   compress ();
 }

void SrModel::clear_materials ()
 {
   mtlnames.resize (0);
   M.resize (0);
   Fm.resize (0);
   compress ();
 }

void SrModel::clear_textures ()
 {
   T.resize(0);
   Ft.resize(0);
   compress ();
 }

//v:current vtx, vi:vertices around v indicating the faces around v, vec:just a buffer
static void gen_normal ( int v, SrArray<SrVec>& vec, SrArray<int>& vi, SrModel *self, float crease_angle )
 {
   int i, j, tmp;
   float ang;

   vec.size(vi.size());

   //sr_out<<"original:\n";
   //for ( i=0; i<vi.size(); i++ ) sr_out<<self->F[vi[i]].a<<","<<self->F[vi[i]].b<<","<<self->F[vi[i]].c<<srnl;

   // order faces around vertex (could use qsort in SrArray):
   for ( i=0; i<vi.size(); i++ )
    { for ( j=i+1; j<vi.size(); j++ )
       { if ( self->common_vertices_of_faces(vi[i],vi[j])==2 ) // share an edge
	      { SR_SWAP(vi[i+1],vi[j]);
            break;
	      }
       }
    }

   // gen normals for each face around v:
   for ( i=0; i<vi.size(); i++ ) 
    { vec[i]= self->face_normal ( vi[i] ); }

   // search for the first edge with a big angle and rearrange array, so
   // that the array starts with a "crease angled edge":
   bool angfound = false;
   for ( i=0; i<vec.size(); i++ )
    { ang = angle ( vec[i], vec[(i+1)%vec.size()]);
      if ( ang>crease_angle ) 
       { for ( j=0; j<=i; j++ ) { vec.push()=vec[j]; vi.push()=vi[j]; }
	     vec.remove ( 0, i+1 );
	     vi.remove ( 0, i+1 );
         angfound = true;
         break;
       }
    }
   if ( !angfound ) return; // no crease angles in this face cluster

   // Finally set the normals:
   SrVec n;
   float x=1.0f;
   int init=0;
   std::vector<SrVec>& N = self->N;
   for ( i=0; i<vec.size(); i++ )
    { ang = angle ( vec[i], vec[(i+1)%vec.size()]);

      if ( ang>crease_angle )
       { n = SrVec::null;
         x = 0.0f;
         for ( j=init; j<=i; j++ ) { n+=vec[j]; x=x+1.0f; }
         n /= x; // n is the mean normal of the previous set of smoothed faces around v

         for ( j=init; j<=i; j++ ) 
          { SrVec3i &fn=self->Fn[vi[j]];
            /*if ( f.n<0 ) 
              { f.n=N.size(); N.insert(N.size(),3); N[f.n]=N[f.a]; N[f.n+1]=N[f.b]; N[f.n+2]=N[f.c]; }
            if ( v==f.a ) N[f.n]=n;
	         else if ( v==f.b ) N[f.n+1]=n;
	          else N[f.n+2]=n;
            */
            if ( v==fn[0] ) fn[0] = N.size();
	         else if ( v==fn[1] ) fn[1] = N.size();
	          else fn[2] = N.size();
            N.push_back(n);
          }

         init = i+1;
       }
    } 
 }

void SrModel::smooth ( float crease_angle )
 {
   int i, v;
   SrTree<VertexNode> t;
   SrArray<int> vi;
   SrArray<SrVec> vec; // this is just a buffer to be used in gen_normal()

   if ( !V.size() || !F.size() ) return;

   Fn.resize ( F.size() );

   vi.size(V.size());
   for (size_t j=0; j<vi.size(); j++ ) vi[j]=0;

   for (size_t j=0; j<F.size(); j++ )
    { insertv ( t, vi, F[j][0], j );
      insertv ( t, vi, F[j][1], j );
      insertv ( t, vi, F[j][2], j );
	  Fn[j] = F[j];
    }

   // first pass will interpolate face normals around each vertex:
   N.resize ( V.size() );
   vi.size(0);
   t.gofirst ();
   while ( t.cur()!=SrTreeNode::null )
    { v = t.cur()->v;
      vi.push() = t.cur()->f;
      t.gonext();
      if ( t.cur()==SrTreeNode::null || v!=t.cur()->v )
       { SrVec n = SrVec::null;
         for ( i=0; i<vi.size(); i++ ) n += face_normal ( vi[i] );
         N[v] = n / (float)vi.size();
         vi.size(0);
       }
    }

   if ( crease_angle<0 ) return; // only smooth everything

   // second pass will solve crease angles:
   vi.size(0);
   t.gofirst ();
   while ( t.cur()!=SrTreeNode::null )
    { v = t.cur()->v;
      vi.push() = t.cur()->f;
      t.gonext();
      if ( t.cur()==SrTreeNode::null || v!=t.cur()->v )
       { gen_normal ( v, vec, vi, this, crease_angle );
         vi.size(0);
       }
    }

 //  the following function is too slow, so being commented out at this point. would come back later
 //  remove_redundant_normals ();
   
   compress ();
 }

SrVec SrModel::face_normal ( int f ) const
 { 
   SrVec n; 
   const SrVec3i& fac = F[f];
   if (fac[0] >= 0 ||
	   fac[1] >= 0 ||
	   fac[2] >= 0)
   {
	n.cross ( V[fac[1]]-V[fac[0]], V[fac[2]]-V[fac[0]] ); 
   }
   n.normalize(); 
   return n; 
 }

float SrModel::face_area ( int f ) const
{ 
	SrVec n; 
	const SrVec3i& fac = F[f];
	n.cross ( V[fac[1]]-V[fac[0]], V[fac[2]]-V[fac[0]] ); 
	
	return n.norm(); 
}

void SrModel::invert_faces ()
 {
   int i, tmp;
   for ( i=0; i<F.size(); i++ ) SR_SWAP ( F[i][1], F[i][2] );
   for ( i=0; i<Fn.size(); i++ ) SR_SWAP ( Fn[i][1], Fn[i][2] );
 }

void SrModel::invert_normals ()
 {
   int i;
   for ( i=0; i<N.size(); i++ ) N[i]*=-1.0;
 }

void SrModel::apply_transformation ( const SrMat& mat )
 {
   int i, size;
   SrMat m = mat;

   size = V.size();
   for ( i=0; i<size; i++ ) V[i] = V[i] * m;

   m.setl4 ( 0, 0, 0, 1 ); // remove translation to normals
   size = N.size();
   for ( i=0; i<size; i++ ) N[i] = N[i] * m;
 }

void SrModel::add_model ( const SrModel& m )
 {
   int origv = V.size();
   int origf = F.size();
   int orign = N.size();
   int origt = T.size();
   int origm = M.size();
   int mfsize = m.F.size();
   int i;

   if ( m.V.size()==0 || mfsize==0 ) return;

   // add vertices and faces:
   V.resize ( origv+m.V.size() );
   for ( i=0; i<m.V.size(); i++ ) V[origv+i] = m.V[i];

   F.resize ( origf+mfsize );
   for ( i=0; i<mfsize; i++ )
    { const SrVec3i& f = m.F[i];
      F[origf+i] = SrVec3i ( f[0]+origv, f[1]+origv, f[2]+origv );
    }
   
   // add the normals:
   if ( m.Fn.size()>0 )
    { N.resize ( orign+m.N.size() );
      for ( i=0; i<m.N.size(); i++ ) N[orign+i] = m.N[i];

      Fn.resize ( origf+mfsize );
      for ( i=0; i<mfsize; i++ )
       { const SrVec3i& f = m.Fn[i];
         Fn[origf+i] = SrVec3i ( f[0]+orign, f[1]+orign, f[2]+orign );
       }
    }

   if ( m.Ft.size()>0 )
   { T.resize ( origt+m.T.size() );
   for ( i=0; i<m.T.size(); i++ ) T[origt+i] = m.T[i];

   Ft.resize ( origf+mfsize );
   for ( i=0; i<mfsize; i++ )
   { const SrVec3i& f = m.Ft[i];
   Ft[origf+i] = SrVec3i ( f[0]+origt, f[1]+origt, f[2]+origt );
   }
   }

   // add material names
   // add the materials:
   if ( m.Fm.size()>0 )
    { M.resize ( origm+m.M.size() );
      for ( i=0; i<m.M.size(); i++ ) M[origm+i] = m.M[i];
      Fm.resize ( origf+mfsize );
      for ( i=0; i<mfsize; i++ )
       { Fm[origf+i] = m.Fm[i]+origm;
       }
    }
   for ( i=0; i<m.mtlnames.size(); i++ )
   {
	   bool newMat = true;
	   mtlnames.push_back ( m.mtlnames[i] );
   }   
   
   // insert material map to texture name
   mtlTextureNameMap.insert(m.mtlTextureNameMap.begin(), m.mtlTextureNameMap.end());
   mtlNormalTexNameMap.insert(m.mtlNormalTexNameMap.begin(), m.mtlNormalTexNameMap.end());
   mtlSpecularTexNameMap.insert(m.mtlSpecularTexNameMap.begin(), m.mtlSpecularTexNameMap.end());
   mtlTransparentTexNameMap.insert(m.mtlTransparentTexNameMap.begin(), m.mtlTransparentTexNameMap.end());
   mtlGlossyTexNameMap.insert(m.mtlGlossyTexNameMap.begin(), m.mtlGlossyTexNameMap.end());

   validate(); 
//    save(sr_out);
 }

void SrModel::operator = ( const SrModel& m )
 {
   M = m.M;
   V = m.V;
   Vc = m.Vc;
   N = m.N;
   T = m.T;
   F = m.F;
   Fm = m.Fm;
   Fn = m.Fn;
   Ft = m.Ft;

   culling = m.culling;

   mtlnames = m.mtlnames;

   mtlTextureNameMap = m.mtlTextureNameMap;
   mtlNormalTexNameMap = m.mtlNormalTexNameMap;
   mtlSpecularTexNameMap = m.mtlSpecularTexNameMap;
   mtlTransparentTexNameMap = m.mtlTransparentTexNameMap;
   mtlGlossyTexNameMap = m.mtlGlossyTexNameMap;
   mtlFaceIndices = m.mtlFaceIndices;
    
   name = m.name;
 }

void SrModel::make_box ( const SrBox& b )
 {
   init ();

   name = "box";

   V.resize ( 8 );

   // side 4 has all z coordinates equal to a.z, side 5 equal to b.z
   b.get_side ( V[0], V[1], V[2], V[3], 4 );
   b.get_side ( V[4], V[5], V[6], V[7], 5 );

   F.resize ( 12 );
   F[0] = SrVec3i(1,0,4); F[1] = SrVec3i(1,4,7); // plane crossing -X
   F[2] = SrVec3i(3,2,6); F[3] = SrVec3i(3,6,5); // plane crossing +X
   F[4] = SrVec3i(7,4,6); F[5] = SrVec3i(4,5,6); // plane crossing +Z
   F[6] = SrVec3i(0,1,2); F[7] = SrVec3i(0,2,3); // plane crossing -Z
   F[8] = SrVec3i(2,1,7); F[9] = SrVec3i(2,7,6); // plane crossing +Y
   F[10] = SrVec3i(0,3,4); F[11] = SrVec3i(4,3,5); // plane crossing -Y

   F.resize ( 12 );

 }

void SrModel::make_sphere ( const SrSphere& s, float resolution )
 {
   init ();

   name = "sphere";
 }

void SrModel::make_cylinder ( const SrCylinder& c, float resolution, bool smooth )
 {
   init ();

   name = "cylinder";

   int nfaces = int(resolution*10.0f);
   if ( nfaces<3 ) nfaces = 3;
   
   float dang = sr2pi/float(nfaces);
   SrVec vaxis = c.b-c.a; 
   SrVec va = vaxis; 
   va.normalize(); // axial vector
   SrVec minus_va = va * -1.0f;

   SrVec vr;
   float deg = SR_TODEG ( angle(SrVec::i,va) );

   if ( deg<10 || deg>170 )
     vr = cross ( SrVec::j, va );
   else
     vr = cross ( SrVec::i, va );
   
   vr.len ( c.radius ); // radial vector

   SrMat rot;
   rot.rot ( va, dang );

   SrPnt a1 = c.a+vr;
   SrPnt b1 = c.b+vr;
   SrPnt a2 = a1 * rot;
   SrPnt b2 = a2 + vaxis;

   int i=1;

   do { if ( smooth )
         { N.push_back((a1-c.a)/c.radius); //normalized normal
         }
        //V.push()=a1; V.push()=b1;
		V.push_back(a1); V.push_back(b1);
        if ( i==nfaces ) break;
        a1=a2; b1=b2; a2=a1*rot; b2=a2+vaxis;
        i++;
      } while ( true );

   // make sides:
   int n1, n=0;
   int i1, i2, i3;
   int size = V.size();
   for ( i=0; i<size; i+=2 )
    { 
	  i1 = (i+1)%V.size();//V.validate ( i+1 );
      i2 = (i+2)%V.size();//V.validate ( i+2 );
      i3 = (i+3)%V.size();//V.validate ( i+3 );
      F.push_back(SrVec3i(i, i2, i1));
      F.push_back(SrVec3i( i1, i2, i3 ));

      if ( smooth )
       { n1 = (n+1)%N.size();//N.validate ( n+1 );
         Fn.push_back(SrVec3i( n, n1, n ));
         Fn.push_back(SrVec3i( n, n1, n1 ));
         n++;
       }
    }

   // make top and bottom:
   n=0;
   size = V.size()+1;
   for ( i=0; i<size; i+=2 )
    { i1 = (i+1)%V.size();//V.validate ( i+1 );
      i2 = (i+2)%V.size();//V.validate ( i+2 );
      i3 = (i+3)%V.size();//V.validate ( i+3 );
      F.push_back(SrVec3i(V.size(), i2, i ));
      F.push_back(SrVec3i(V.size()+1, i1, i3 ));

      if ( smooth )
       { n1 = (n+1)%N.size();//N.validate ( n+1 );
         Fn.push_back(SrVec3i(N.size(), N.size(), N.size() ));
         Fn.push_back(SrVec3i(N.size()+1, N.size()+1, N.size()+1 ));
         n++;
       }
    }

   V.push_back(c.a);
   V.push_back(c.b);
   if ( smooth )
    { N.push_back(minus_va);
      N.push_back(va);
    }

   compress ();
 }

int SrModel::pick_face ( const SrLine& line ) const
 {
   int i;
   float t, u, v;
   SrArray<float> faces;
   faces.capacity(16);
   for ( i=0; i<F.size(); i++ )
    { const SrVec3i& f = F[i];
      if ( line.intersects_triangle ( V[f[0]], V[f[1]], V[f[2]], t, u, v ) )
       { faces.push() = (float)i;
         faces.push() = t; // t==0:p1, t==1:p2
       }
    }

   int closest=-1;
   for ( i=0; i<faces.size(); i+=2 )
    { if ( closest<0 || faces[i+1]<faces[closest+1] ) closest=i;
    }

   if ( closest>=0 ) return (int)faces[closest];
   return closest;
 }


SBAPI SrVec SrModel::intersectLine( const SrLine& line ) const
{
	int i;
	float t, u, v;
	SrArray<float> faces;
	faces.capacity(16);
	std::vector<SrVec> posList;
	for ( i=0; i<F.size(); i++ )
	{ const SrVec3i& f = F[i];
	if ( line.intersects_triangle (V[f[0]] , V[f[1]], V[f[2]], t, u, v ) )
		{ faces.push() = (float)i;
		faces.push() = t; // t==0:p1, t==1:p2
		SrVec interPos = V[f[0]]*(1.f-u-v) + V[f[1]]*u +  V[f[2]]*v;
		posList.push_back(interPos);
		}
	}

	int closest=-1;
	for ( i=0; i<faces.size(); i+=2 )
	{ if ( closest<0 || faces[i+1]<faces[closest+1] ) closest=i;
	}

	if ( closest>=0 ) return posList[closest/2];
	return SrVec();

}

void SrModel::saveOriginalVertices()
{
	if (VOrig.size() != V.size())
	{
		VOrig.resize(V.capacity());
		for (int x = 0; x < V.size(); x++)
		{
			VOrig[x] = V[x];
		}
	}
}

void SrModel::restoreOriginalVertices()
{
	if (VOrig.size() == V.size())
	{
		for (int x = 0; x < V.size(); x++)
		{
			V[x] = VOrig[x];
		}
	}
}

SBAPI void SrModel::computeNormals(int flat)
{
	//return;
	

// 	if (flat)
// 	{
// 		N.size(F.size()); // set normal vector to the same size
// 		N.setall(SrPnt(0,0,0));
// 		Fn.size(F.size());
// 		for (int i=0;i<F.size();i++)
// 		{
// 			SrVec fn = face_normal(i);
// 			Fn[i].set(i, i, i);
// 			N[i] = fn;
// 		}
// 	}
// 	else
	{
		N.resize(V.size()); // set normal vector to the same size
		N.assign(N.size(),SrPnt(0,0,0));
		Fn.resize(F.size());
		for (int i=0;i<F.size();i++)
		{
			SrVec fn = face_normal(i);
			Fn[i] = F[i];
			for (int j=0;j<3;j++)
			{
				N[F[i][j]] += fn;			
			}
		}
	}
	

	for (int i=0;i<N.size();i++)
		N[i].normalize();
}

int SrModel::getNumMaterials()
{
	return M.size();
}

SrMaterial& SrModel::getMaterial(int num)
{
	if (num >= 0 && num < M.size())
		return M[num];
	else
		return _emptyMaterial;
}

int SrModel::getClosestIndex(SrVec& vec)
{
	float closestDistance = 9999999.0f;
	int closestIndex = 0;

	// extremely inefficient way to find the closest vertex
	for (int v = 0; v < V.size(); v++)
	{
		float curDist = dist(V[v], vec);
		if (curDist < closestDistance)
		{
			closestDistance = curDist;
			closestIndex = v;
		}
	}

	return closestIndex;
}


//================================ End of File =================================================
