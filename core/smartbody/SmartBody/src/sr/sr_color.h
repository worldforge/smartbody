/*
 *  sr_color.h - part of Motion Engine and SmartBody-lib
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

# ifndef SR_COLOR_H
# define SR_COLOR_H

/** \file sr_color.h 
 * A color definition
 */

#include <sb/SBTypes.h>
# include <sr/sr_input.h>
# include <sr/sr_output.h>

/*! \class SrColor sr_color.h
    \brief specifies a color

     SrColor specifies a color using 8 bits for each basic color (red,green,blue)
     and more 8 bits for the alpha (the opacity). In this way, each component can
     have a value from 0 to 255 and the total class has a sizeof of 4 bytes.
     The default constructor initializes with values (r,g,b,a)=(127,127,127,255). */



class SBAPI SrColor
 { public :

  static const SrColor black;   //!< black color (0,0,0)
  static const SrColor red;    //!< red color (255,0,0)
  static const SrColor green;   //!< green color (0,255,0)
  static const SrColor yellow;  //!< yellow color (255,255,0)
  static const SrColor blue;   //!< blue color (0,0,255)
  static const SrColor magenta; //!< magenta color (255,0,255)
  static const SrColor cyan;    //!< cyan color (0,255,255)
  static const SrColor white;   //!< white color (255,255,255)
  static const SrColor gray;    //!< gray color (127,127,127)
  static const SrColor darkgray; //!< gray color (64,64,64)
  static const SrColor lightgray;//!< gray color (225,225,225)
    
    srbyte r; //!< r component, in {0,...,255}, default is 127
    srbyte g; //!< g component, in {0,...,255}, default is 127
    srbyte b; //!< b component, in {0,...,255}, default is 127
    srbyte a; //!< a component, in {0,...,255}, default is 255, that is full opacity
 
   public :

    /*! Default constructor. Initializes with color gray. */
    SrColor ();

    /*! Constructor setting all components. */
    SrColor ( srbyte x, srbyte y, srbyte z, srbyte w=255 ) { set(x,y,z,w); }

    /*! Constructor setting all components. */
    SrColor ( int x, int y, int z, int w=255 ) { set(x,y,z,w); }

    /*! Constructor setting all components with float types. */
    SrColor ( float x, float y, float z, float w=1.0f ) { set(x,y,z,w); }

    /*! Constructor from a 4 dimension float pointer. */
    SrColor ( const float v[4] ) { set(v); }

    srbyte* data() { return &r;}
    const srbyte* data() const { return &r;}

    /*! Sets the components of the color, the alpha value has a default parameter of 255. */
    void set ( srbyte x, srbyte y, srbyte z, srbyte w=255 );

    /*! Sets the components of the color with integer values also betwenn 1 and 255,
        the alpha value has a default parameter of 255. */
    void set ( int x, int y, int z, int w=255 );

    /*! Sets the components of the color with float values, each one inside [0.0,1.0],
        the alpha value has a default parameter of 1.0. */
    void set ( float x, float y, float z, float w=1.0f );

    /*! Sets the components from and array of four floats. */
    void set ( const float v[4] ) { set(v[0],v[1],v[2],v[3]); }

    /*! Sets the color with a string containing one of the following:
        black, red, green, yellow, blue, magenta, cyan, white, gray */
    void set ( const char* s );

    /*! Put the four components in f[], translating each one to range [0.0,1.0] */
    void get ( float f[4] ) const; 

    /*! Put the four components in i[], each component varying from 0 to 255. */
    void get ( int i[4] ) const;

    /*! Put the four components in b[], each component varying from 0 to 255. */
    void get ( srbyte b[4] ) const;

	/*! Returns a color across the hue space from given parameter in [0,1] */
	static SrColor interphue(float hue);

    /*! Comparison equal operator. */
    friend bool operator == ( const SrColor &c1, const SrColor &c2 );

    /*! Comparison difference operator. */
    friend bool operator != ( const SrColor &c1, const SrColor &c2 );

    /*! Interpolates two colors. */
    friend SrColor lerp ( const SrColor &c1, const SrColor &c2, float t );

    /*! Outputs in format 'r g b a'. */
    friend SrOutput& operator<< ( SrOutput& o, const SrColor& v );

    /*! Reads from format 'r g b a'. */
    friend SrInput& operator>> ( SrInput& in, SrColor& v );
 };

//================================ End of File =================================================

# endif  // SR_COLOR_H

