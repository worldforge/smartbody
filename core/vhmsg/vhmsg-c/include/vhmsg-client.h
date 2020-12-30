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

#ifndef VHMSG_CLIENT_H
#define VHMSG_CLIENT_H


#include "vhmsg.h"


namespace vhmsg
{

class Client
{
   private:
      class ClientImpl * pimpl;

   public:
      Client();
      virtual ~Client();


      bool OpenConnection();
      bool OpenConnection( const char * server );
      bool OpenConnection( const char * server, const char * port );

      void CloseConnection();

      bool IsOpen();

      bool Send( const char * name, const char * value );
      bool Send( const char * message );
      bool Send( const Message & in_message );

      void EnablePollingMethod();
      void EnableImmediateMethod();
      void SetBypassMode( bool enable );
      void SetListener( Listener * listener );
      bool Subscribe( const char * req );
      bool Unsubscribe( const char * req );

      void Poll();
      void WaitAndPoll( double waitTimeSeconds );

      const std::string & GetServer() const;
      const std::string & GetPort() const;
      const std::string & GetScope() const;
      void SetScope( const char * scope );

   protected:
      void SetServerFromEnv();
      void SetScopeFromEnv();
      void SetPortFromEnv();
};


};

#endif  // VHMSG_CLIENT_H
