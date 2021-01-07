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

#ifndef _HEIGHTFIELD_
#define _HEIGHTFIELD_

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <sb/SBTypes.h>

#define TERRAIN_SCALE	2000.0f
#define ELEVATION_SCALE 200.0f

class Heightfield
{
	public:
		SBAPI Heightfield();
		SBAPI ~Heightfield();
		
		SBAPI void load( char* filename );
		SBAPI void set_scale( float x, float y, float z );
		SBAPI void set_origin( float x, float y, float z );
		SBAPI void set_auto_origin();
		
//		void paste_img();
		SBAPI float get_elevation( float x, float z, float *normal_p = nullptr );

		struct MeshData {
			int 	mesh_resx;
			int 	mesh_resz;
			float	mesh_scale[ 3 ];
			float	mesh_origin[ 3 ];

			float *vertex_arr;
			float *normal_arr;
			unsigned char *color_arr;

			bool dirty_normals;
		};

		const MeshData& getMeshData() const {
			return mesh_data;
		}

		void load_normals( );
	private:
		void clear();
		void defaults();
		
		unsigned char* parse_ppm( FILE *stream );
		unsigned char* read_ppm( const char* filename );
#ifdef WIN32
		unsigned char *LoadBitmapFile( char *filename, BITMAPINFOHEADER* bitmapInfoHeader );
#endif

		void initializeTerrain( unsigned char* terrain );
		void calc_normal( float *N_out, float *A, float *B, float *C, float *S );
		void normalize_arr3( float V[ 3 ] );

		float get_raw_elevation( int i, int j );

#ifdef WIN32
		BITMAPINFOHEADER* _header;
#endif
		unsigned char* _imageData;

		int image_width;
		int image_height;
		int num_color_comps;

		MeshData mesh_data;

};
#endif
