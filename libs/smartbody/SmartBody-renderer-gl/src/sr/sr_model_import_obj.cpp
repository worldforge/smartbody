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


#include <sr/sr_string_array.h>
#include <sr/sr_model.h>
#include "SBABI.h"
#include "SBUtilities.h"

#include <boost/algorithm/string.hpp>
#include <iostream>

#if !defined(SB_IPHONE)
#include <sbm/GPU/SbmTexture.h>
#endif

//# define SR_USE_TRACE1    // keyword tracking
//#include <sr/sr_trace.h>

# define GETID(n,A) in>>n; if (n>0) n--; else if (n<0) n+=A.size()
 
static void get_face ( SrInput& in, SrModel& m, int& vc, int& vt, int& vn )
 {
   vc = vt = vn = -1;
   GETID(vc,m.V);

   if ( in.get_token()!=SrInput::Delimiter ) // was only: vc
    { in.unget_token(); return; }

   if ( in.get_token()==SrInput::Delimiter ) // is: vc//vn
    { GETID(vn,m.N);
      return;
    }

   in.unget_token();

   GETID(vt,m.T); // is: vc/vt

   if ( in.get_token()==SrInput::Delimiter )
    { GETID(vn,m.N); // is: vc/vt/vn
      return;
    }

   if ( !in.finished() ) in.unget_token (); // is only: vc/vt
 }

static SrColor read_color ( SrInput& in )
 {
   float r, g, b;
   in >> r >> g >> b;
   SrColor c(r,g,b);
   return c;
 }

static void load_texture(int type, const char* file, const SrStringArray& paths)
{
	/*
#if !defined(SB_IPHONE)
	SrString s;
	SrInput in;
	std::string imageFile = file;
	in.init( fopen(file,"r"));
	int i = 0;
	SmartBody::util::log("[load_texture] Loading texture: '%s' \n", file);
	while ( !in.valid() && i < paths.size())
	{
		s = paths[i++];
		SmartBody::util::log("Path = '%s'\n", (const char*)s);
		s << file;
		//SmartBody::util::log("Evaluating path " << i << " / " <<  paths.size() << ":\t" << s << "\n";
		SmartBody::util::log("Evaluating path %d / %d:\t '%s' \n", i, paths.size(), (const char*)s);
		imageFile = s;
		
		in.init (fopen(s,"r") );
	}
	if (!in.valid()) {
		SmartBody::util::log("[load_texture] ERROR: Invalid inpath for texture: '%s' \n", file);
		return;		
	}

	SbmTextureManager& texManager = SbmTextureManager::singleton();
	texManager.loadTexture(type, file, s);
	
#endif
*/
#if !defined (__ANDROID__) && !defined(SB_IPHONE) && !defined(EMSCRIPTEN)
	SrString s;
	SrInput in;
	std::string imageFile = file;
	in.init(fopen(file, "r"));
	int i = 0;
	while (!in.valid() && i < paths.size())
	{
		s = paths[i++];
		s << file;
		imageFile = s;
		in.init(fopen(s, "r"));
	}
	if (!in.valid()) return;
	SbmTextureManager& texManager = SbmTextureManager::singleton();
	SmartBody::util::log("loading texture file = %s", imageFile.c_str());
	texManager.loadTexture(type, file, s);
#endif
}

static void read_materials ( std::vector<SrMaterial>& M,
                             std::vector<std::string>& mnames,
							 std::map<std::string,std::string>& mtlTexMap,
							 std::map<std::string,std::string>& mtlTexBumpMap,
							 std::map<std::string,std::string>& mtlTexKsMap,
							 std::map<std::string,std::string>& mtlTexNsMap,
                             const SrString& file,
                             const SrStringArray& paths )
 {
	SrString s;
	SrInput in;
	int i=0;

	in.lowercase_tokens(false);
	in.init ( fopen(file,"rt") );
	
	while ( !in.valid() && i < paths.size() )
    {
		s = paths[i++];
		s << file;
		in.init ( fopen(s,"rt") );

	}
	
	if ( !in.valid() )
	{
		std::cerr << "ERROR: Could not read materials\n";
		return; // could not get materials
	}

	while ( !in.finished() )
	{
		in.get_token();
		if ( in.last_token()=="newmtl" )
		{ 
			SrMaterial material;
			material.init();
			//M.push().init();
			M.emplace_back(material);
			//SR_TRACE1 ( "new material: "<<in.last_token() );
			SrString matName;
			in.getline(matName);
			matName.trim();
			mnames.emplace_back( (const char*) matName );
		}	  
		else if ( in.last_token()=="Ka" )
		{
			M.back().ambient = read_color ( in );
		}
		else if ( in.last_token()=="Kd" )
		{
			M.back().diffuse = read_color ( in );
		}
		else if ( in.last_token()=="Ks" )
		{
			M.back().specular = read_color ( in );
		}
		else if ( in.last_token()=="Ke" ) // not sure if this one exists
		{
			M.back().emission = read_color ( in );
		}
		else if ( in.last_token()=="Ns" )
		{
			in >> i;
			M.back().shininess = i;
		}
		else if ( in.last_token()=="map_Kd") // texture map
		{
		 
			SrString map_Kd, dotstr, ext;
			in.getline(map_Kd);
			// in.getline may retrieve the EOF marker, eliminate it
#if !defined(__ANDROID__) && !defined(SB_IPHONE)
			if (map_Kd[map_Kd.len() - 1] == EOF)
#endif
				map_Kd.substring(0, map_Kd.len() - 2);
			map_Kd.trim();
		  
			std::string texFile = (const char*) map_Kd;
			std::string mtlName = mnames.back();
			mtlTexMap[mtlName] = texFile;		  
	  
			std::cerr << "Reading map_kd:     " << texFile << "\n";
	  }
	  else if ( in.last_token()=="map_bump") // texture map
	  {
			SrString mapBump, dotstr, ext;
			in.getline(mapBump);
			// in.getline may retrieve the EOF marker, eliminate it
#if !defined(__ANDROID__) && !defined(SB_IPHONE)
			if (mapBump[mapBump.len() - 1] == EOF)
#endif
				mapBump.substring(0, mapBump.len() - 2);

			mapBump.trim();

			std::string texFile = (const char*) mapBump;
			std::string mtlName = mnames.back();
			mtlTexBumpMap[mtlName] = texFile;		  
	  }
	else if ( in.last_token()=="map_Ks") // texture map
	  {
			SrString mapKs, dotstr, ext;	 		  
			in.getline(mapKs);
			// in.getline may retrieve the EOF marker, eliminate it
#if !defined(__ANDROID__) && !defined(SB_IPHONE)
			if (mapKs[mapKs.len() - 1] == EOF)
#endif
				mapKs.substring(0, mapKs.len() - 2);
			mapKs.trim();

			std::string texFile = (const char*) mapKs;
			std::string mtlName = mnames.back();
			mtlTexKsMap[mtlName] = texFile;		  
	  }
	else if ( in.last_token()=="map_Ns") // texture map
	  {
			SrString mapNs, dotstr, ext;
			in.getline(mapNs);
			// in.getline may retrieve the EOF marker, eliminate it
#if !defined(__ANDROID__) && !defined(SB_IPHONE)
			if (mapNs[mapNs.len() - 1] == EOF)
#endif
				mapNs.substring(0, mapNs.len() - 2);
			mapNs.trim();
		 
			std::string texFile = (const char*) mapNs;
			std::string mtlName = mnames.back();
			mtlTexNsMap[mtlName] = texFile;		  
	  }
      else if ( in.last_token()=="illum" ) // dont know what is this one
       { 
		   in >> i;
       }
    }
 }

static bool process_line ( const SrString& line,
                           SrModel& m,
                           SrStringArray& paths,
                           std::vector<std::string>& mnames,
						   std::map<std::string,std::string>& mtlTexMap,
						   std::map<std::string,std::string>& mtlTexBumpMap,
						   std::map<std::string,std::string>& mtlTexKsMap,
						   std::map<std::string,std::string>& mtlTexNsMap,
                           int& curmtl )
 {
   SrInput in (line);
   in.get_token();

   if ( in.last_token().len()==0 ) return true;

   if ( in.last_token()=="v" ) // v x y z [w]
    { //SR_TRACE1 ( "v" );
      //m.V.push();
	  SrVec topVec;
      in >> topVec;	 
	  m.V.emplace_back(topVec);
	  // more to read after the 
	  if (in.get_token() != SrInput::EndOfFile)
	  {
		  in.unget_token();
		  SrVec vColor;
		  in >> vColor;
		  vColor /= 255.f; // normalize color
		  m.Vc.emplace_back(vColor);
	  }
    }
   else if ( in.last_token()=="vn" ) // vn i j k
    { //SR_TRACE1 ( "vn" );
      SrVec vN;
      in >> vN;
	  m.N.emplace_back(vN);
    }
   else if ( in.last_token()=="vt" ) // vt u v [w]
    { //SR_TRACE1 ( "vt" );
	
		SrVec2 vT;
		in >> vT;
		m.T.emplace_back(vT);
    }
   else if ( in.last_token()=="g" )
    { //SR_TRACE1 ( "g" );
    }
   else if ( in.last_token()=="f" ) // f v/t/n v/t/n v/t/n (or v/t or v//n or v)
    { //SR_TRACE1 ( "f" );
      int i=0;
      SrArray<int> v, t, n;
      v.capacity(8); t.capacity(8); n.capacity(8);
      while ( true )
       { if ( in.get_token()==SrInput::EndOfFile ) break;
         in.unget_token();
         get_face ( in, m, v.push(), t.push(), n.push() );
         if ( in.last_error()==SrInput::UnexpectedToken ) return false;
       }

      if ( v.size()<3 ) return false;

      for ( i=2; i<v.size(); i++ )
       { m.F.emplace_back(SrVec3i( v[0], v[i-1], v[i] ));
         m.Fm.emplace_back(curmtl);

         if ( t[0]>=0 && t[1]>=0 && t[i]>=0 )
          m.Ft.emplace_back(SrVec3i( t[0], t[i-1], t[i] ));

         if ( n[0]>=0 && n[1]>=0 && n[i]>=0 )
          m.Fn.emplace_back(SrVec3i(  n[0], n[i-1], n[i] ));
       }
    }
   else if ( in.last_token()=="s" ) // smoothing groups not supported
    { //SR_TRACE1 ( "s" );
      in.get_token();
    }
   else if ( in.last_token()=="usemap" ) // usemap name/off
    { //SR_TRACE1 ( "usemap" );
    }
   else if ( in.last_token()=="usemtl" ) // usemtl name
    { //SR_TRACE1 ( "usemtl" );
      //in.get_token ();
	  SrString matName;
	  in.getline(matName);
	  matName.trim();
	  curmtl = std::find(mnames.begin(), mnames.end(), (const char*) matName) - mnames.begin();
      //curmtl = mnames.lsearch ( matName );
      //SR_TRACE1 ( "curmtl = " << curmtl << " (" << in.last_token() << ")" );
    }
   else if ( in.last_token()=="mtllib" ) // mtllib file1 file2 ...
    { //SR_TRACE1 ( "mtllib" );
      SrString token, file;
		int pos = line.get_next_string ( token, 0 ); // parse again mtllib
		while ( true )
		{ 
			pos = line.get_next_string ( token, pos );
			if (pos < 0)
				break;
			token.extract_file_name ( file );
			token.replace ( "\\", "/" ); // avoid win-unix problems

			paths.push ( token );
			read_materials ( m.M, mnames, mtlTexMap, mtlTexBumpMap, mtlTexKsMap, mtlTexNsMap, file, paths );
		}
	}
	return true;
 }



/*! This function import the .obj format. If the import
    is succesfull, true is returned
    and the model m will contain the imported object, otherwise false
    is returned. */
bool SrModel::import_obj ( const char* file )
{
	SrInput in ( fopen(file,"rt") );

	std::cerr << "OBJ path:           " << file << "\n";

	if ( !in.valid() ) 
		return false;
	in.comment_style ( '#' );
	in.lowercase_tokens ( false );

	SrString path = file;
	SrString filename;
	path.extract_file_name(filename);
	SrStringArray paths;
	paths.push ( path );
  
	//SR_TRACE1 ( "First path:" << path );

	

	init ();
	name = filename;
	//name.remove_file_extension();
	int curmtl = 0;
	M.emplace_back(SrMaterial());
	M.back().diffuse = SrColor::gray;
	mtlnames.emplace_back("noname");

	SrString line;
	while ( in.getline(line)!=EOF )
	{ if ( !process_line(line, *this, paths, mtlnames, mtlTextureNameMap, mtlNormalTexNameMap, mtlSpecularTexNameMap, mtlNormalTexNameMap, curmtl) )
		return false;
    }

	validate ();
	remove_redundant_materials ();
//	remove_redundant_normals ();
	compress ();

   // after remove all redundant materials, load the corresponding textures   
#if !defined(SB_IPHONE)
   for (size_t i=0;i<M.size();i++)
   {
	   std::string matName = mtlnames[i];
	   if (mtlTextureNameMap.find(matName) != mtlTextureNameMap.end())
	   {
		   load_texture(SbmTextureManager::TEXTURE_DIFFUSE, mtlTextureNameMap[matName].c_str(),paths);	   
	   }	
	   if (mtlNormalTexNameMap.find(matName) != mtlNormalTexNameMap.end())
	   {
		   load_texture(SbmTextureManager::TEXTURE_NORMALMAP, mtlNormalTexNameMap[matName].c_str(),paths);	   
	   }
   }
#endif
   //SR_TRACE1("Ok!");
   return true;
 }

//============================ EOF ===============================
