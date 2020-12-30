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

class Message
{
   private:
      std::string m_message;
      std::map< std::string, std::string > m_messageMap;

      std::wstring m_wmessage;
      std::map< std::wstring, std::wstring > m_wmessageMap;

   public:
      Message() = default;
      const std::string & GetString() const { return m_message; }
      void SetString( const char * message ) { m_message = message; }

      const std::wstring & GetWString() const { return m_wmessage; }
      void SetWString( const wchar_t * message ) { m_wmessage = message; }


      const std::map< std::string, std::string > & GetMap() const { return m_messageMap; }
      void InsertItem( const std::pair< std::string, std::string > & p ) { m_messageMap.insert( p ); }

      const std::map< std::wstring, std::wstring > & GetWMap() const { return m_wmessageMap; }
      void InsertWItem( const std::pair< std::wstring, std::wstring > & p ) { m_wmessageMap.insert( p ); }
};


class Listener
{
   public:
      virtual void OnMessage( const Message & message ) {}
};



};

#endif  // VHMSG_H
