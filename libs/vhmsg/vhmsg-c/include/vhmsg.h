/*
    This file is part of VHMsg written by Edward Fast at 
    University of Southern California's Institute for Creative Technologies.
    http://www.ict.usc.edu
    Copyright 2008 Edward Fast, University of Southern California

    VHMsg is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VHMsg is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with VHMsg.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VHMSG_H
#define VHMSG_H

#include <string>
#include <map>


namespace vhmsg
{

struct Message
{
      std::string message;
      std::map< std::string, std::string > messageMap;

      std::wstring wmessage;
      std::map< std::wstring, std::wstring > wmessageMap;

};


struct Listener
{
      virtual void OnMessage( const Message & message ) {}
};



};

#endif  // VHMSG_H
