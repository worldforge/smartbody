/*
 *  sr.cpp - part of Motion Engine and SmartBody-lib
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

#include <stdio.h>
#include <string.h>

#include <sr/sr.h>

//================================ sr.cpp ============================================

float sr_todeg ( float radians )
 {
   return 180.0f * radians / float(SR_PI);
 }

double sr_todeg ( double radians )
 {
   return 180.0 * radians / double(SR_PI);
 }

float sr_torad ( float degrees )
 {
   return float(SR_PI) * degrees / 180.0f;
 }

double sr_torad ( double degrees )
 {
   return double(SR_PI) * degrees / 180.0;
 }

float sr_trunc ( float x ) 
 { 
   return (float) (int) (x); 
 }

double sr_trunc ( double x ) 
 { 
   return (double) (int) (x); 
 }

float sr_round ( float x ) 
 { 
   return (float) (int) ((x>0.0)? (x+0.5f) : (x-0.5f)); 
 }

double sr_round ( double x )
 { 
   return (double) (int) ((x>0.0)? (x+0.5) : (x-0.5)); 
 }

float sr_floor ( float x ) 
 { 
   return (float) (int) ((x>0.0)? x : (x-1.0f));
 }

double sr_floor ( double x ) 
 { 
   return (double) (int) ((x>0.0)? x : (x-1.0)); 
 }

float sr_ceil ( float x ) 
 { 
   return (float) (int) ((x>0.0)? (x+1.0f) : (x));
 }

double sr_ceil ( double x ) 
 { 
   return (double) (int) ((x>0.0)? (x+1.0) : (x));
 }

int sr_sqrt ( int n )
 { 
   register int i, s=0, t;

   for ( i=15; i>=0; i-- )
    { t = ( s | (1<<i) );
      if (t*t<=n) s=t;
    }
   return s;
 }

int sr_fact ( int x )
 {
   if ( x<2 ) return 1;
   int m = x;
   while ( --x>1 ) m *= x;
   return m;
 }

int sr_pow ( int b, int e )
 {
   if ( e<=0 ) return 1;
   int pow=b;
   while ( --e>0 ) pow*=b;
   return pow;
 }

int sr_compare ( const char *s1, const char *s2 )
 {
   int c1, c2; // ANSI definition of toupper() uses int types

   while ( *s1 && *s2 )
    { c1 = SR_UPPER(*s1);
      c2 = SR_UPPER(*s2);
      if ( c1!=c2 ) return c1-c2;
      s1++; s2++;
    }
   if ( !*s1 && !*s2 ) return 0;
   return !*s1? -1:1;
 }

int sr_compare_cs ( const char *s1, const char *s2 )
 {
   int c1, c2; // ANSI definition of toupper() uses int types

   while ( *s1 && *s2 )
    { c1 = *s1;
      c2 = *s2;
      if ( c1!=c2 ) return c1-c2;
      s1++; s2++;
    }
   if ( !*s1 && !*s2 ) return 0;
   return !*s1? -1:1;
 }

int sr_compare ( const char *s1, const char *s2, int n )
 {
   int c1, c2; // ANSI definition of toupper() uses int types

   //   printf("[%s]<>[%s] (%d)\n",s1,s2,n);

   while ( *s1 && *s2 )
    { c1 = SR_UPPER(*s1);
      c2 = SR_UPPER(*s2);
      if ( c1!=c2 ) return c1-c2;
      s1++; s2++; n--;
      if ( n==0 ) return n; // are equal
    }
   if ( !*s1 && !*s2 ) return 0;
   return !*s1? -1:1;
 }

int sr_compare_cs ( const char *s1, const char *s2, int n )
 {
   int c1, c2; // ANSI definition of toupper() uses int types

   //   printf("[%s]<>[%s] (%d)\n",s1,s2,n);

   while ( *s1 && *s2 )
    { c1 = *s1;
      c2 = *s2;
      if ( c1!=c2 ) return c1-c2;
      s1++; s2++; n--;
      if ( n==0 ) return n; // are equal
    }
   if ( !*s1 && !*s2 ) return 0;
   return !*s1? -1:1;
 }

int sr_compare ( const int *i1, const int *i2 )
 {
   return *i1-*i2;
 }

int sr_compare ( const float *f1, const float *f2 )
 {
   return SR_COMPARE(*f1,*f2);
 }

int sr_compare ( const double *d1, const double *d2 )
 {
   return SR_COMPARE(*d1,*d2);
 }

char* sr_string_new ( const char *tocopy )
 {
   if ( !tocopy ) return 0;
   char *s = new char [ strlen(tocopy)+1 ];
   strcpy ( s, tocopy );
   return s;
 }

char* sr_string_set ( char*& s, const char *tocopy ){
	if( s )
		delete[] s;
	if ( tocopy ) {
		s = new char [ strlen(tocopy)+1 ];
		strcpy ( s, tocopy );
	} else {
		s=0;
	}
	return s;
}

char* sr_string_realloc ( char*& s, int size )
 {
   char *news = 0;

   if ( size>0 )
    { news = new char[size];
      news[0] = 0;
      if ( s )
       { int i;
         for ( i=0; i<size; i++ )
          { news[i]=s[i]; if(!s[i]) break; }
       }
      news[size-1] = 0;
    }

   delete []s;
   s = news;
   return s;
 }

void sr_stdout_to_file ()
 {
   freopen ( "stdout.txt", "w", stdout );
   setbuf ( stdout, NULL );
   freopen ( "stderr.txt", "w", stderr );
   setbuf ( stderr, NULL );
 }

//=== End of File =======================================================================

