/*  sr_sn_group.cpp - part of Motion Engine and SmartBody-lib
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

#include <sr/sr_sn_group.h>

//# define SR_USE_TRACE1  // Const/Dest
//#include <sr/sr_trace.h>

//======================================= SrSnGroup ====================================

const char* SrSnGroup::class_name = "Group";

SrSnGroup::SrSnGroup ()
          :SrSn ( SrSn::TypeGroup, SrSnGroup::class_name )
 {
   //SR_TRACE1 ( "Constructor" );
   _separator = false;
 }

SrSnGroup::~SrSnGroup ()
 {
   //SR_TRACE1 ( "Destructor" );
   remove_all ();
 }

void SrSnGroup::capacity ( int c ) 
 { 
   if ( c<=_children.size() ) return;
   _children.reserve( c );
 }

SrSn*  SrSnGroup::get ( int pos ) const
 { 
   if ( _children.empty() ) return nullptr;
   if ( pos<0 || pos>=_children.size() ) return _children.back().get();
   return _children[pos].get();
 }

int SrSnGroup::search ( SrSn *n ) const
 {
   for ( int i=0; i<_children.size(); i++ )
    { if ( _children[i]==n ) return i; }

   return -1;
 }

void SrSnGroup::add ( boost::intrusive_ptr<SrSn> sn, int pos )
 {

   if ( pos<0 || pos>=_children.size() ) // Append
    { _children.emplace_back(std::move(sn));
    }
   else // Insert
    { _children.insert(_children.begin() + pos, std::move(sn));
    }
 }

boost::intrusive_ptr<SrSn> SrSnGroup::remove ( int pos )
 {
   //SR_TRACE3 ( "remove "<<pos );

   if ( _children.empty() ) // empty
    {
   	return nullptr;
    }
   else if ( pos<0 || pos>=_children.size() ) // get last child
    {
   	auto sn = _children.back();
   	_children.pop_back();
   	return sn;
    }
   else // remove item in the middle
    {
   	auto result = _children.erase(_children.begin() + pos);
   	return *result;
    }
 }

boost::intrusive_ptr<SrSn> SrSnGroup::remove ( SrSn *n )
 {
   int pos = search ( n );
   if ( pos<0 ) return nullptr;
   return remove ( pos );
 }

boost::intrusive_ptr<SrSn> SrSnGroup::replace ( int pos, SrSn *sn )
 {
   //SR_TRACE3 ( "replace "<<pos );

   if ( _children.empty() || pos<0 || pos>=_children.size() ) return nullptr; // invalid pos

   auto old = _children[pos];
   _children[pos] = sn;
   return old;
 }

void SrSnGroup::remove_all ()
 {
	_children.clear();
 }

//======================================= EOF ====================================

