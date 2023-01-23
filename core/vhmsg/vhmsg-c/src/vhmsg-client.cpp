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


#include "vhmsg-client.h"

#include "vhcl.h"

#if defined(WIN_BUILD)
#include <windows.h>
#endif

#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include <list>

// disable ActiveMQ warnings
#if defined(WIN_BUILD)
#pragma warning(push)
// warning C4290: C++ exception specification ignored except to indicate a
// function is not __declspec(nothrow)
#pragma warning(disable:4290)
#endif

#include <activemq/library/ActiveMQCPP.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>

#if defined(WIN_BUILD)
#pragma warning(pop)
#endif

#include "pthread.h"

#include "HttpUtility.h"


using std::string;
using std::wstring;
using std::vector;
using std::list;
using std::pair;
using std::make_pair;

using namespace vhmsg;


// choose to hide/show debug messages
#ifdef DEBUG_BUILD
//#define Log  OutputDebug
#else
#define Log
#endif


static int g_initCount = 0;


template <class S>
static pair<S, S> split(const S & m, const S & delim)
{
    size_t index = m.find_first_of( delim );
    if ( index == string::npos )
    {
        return make_pair(m, S());
    }
    else
    {
        return make_pair(m.substr( 0, index ), m.substr( index + 1 ));
    }
}


static string encodeString(const string & in_string)
{
// copied from bool Client::Send( const char * op, const char * arg )
#ifdef ANDROID_BUILD
    return HtppUtility_UrlEncode_ByteChar(in_string);
#else
    return vhcl::WStringToAnsi( HtppUtility_UrlEncode( vhcl::AnsiToWString( in_string ) ) );
#endif
}


static string encodeString(const std::wstring & in_string)
{
#ifdef ANDROID_BUILD
    // This is going to break next time you try to compile the code on Android.
    // I'm letting it break because the #define has no place in this
    // file. The conversion functions exist for a reason.
    // HttpUtil exists for a reason -- its goal is to a have a single
    // conversion interface for strings transmitted over VHMSG. Having
    // a special case for Android in this file clatters the code and violates
    // the idea of the unifed interface. Please unify the
    // conversion interface in the HttpUtil file and get rid of the define
    // everywhere in this file.
    //
    // and, this is a perfect example why #define is bad: whoever designed
    // the Android version of this code did not account for possibility of
    // encoding a wstring. Now we need that, what are we supposed to do?
    return "";
#else
    return vhcl::WStringToAnsi( HtppUtility_UrlEncode( in_string ) );
#endif
}

static pair<string, wstring> decodeString(const string& in_string)
{
    wstring ws = HtppUtility_UrlDecode( in_string );
  
#ifdef ANDROID_BUILD
    string as = HtppUtility_UrlDecode_ByteChar( in_string );
#else
    string as = vhcl::WStringToAnsi( ws );
#endif
    return std::make_pair(as, ws);
}

class vhmsg::ClientImpl
{
public:
   class InternalListener : public cms::MessageListener
   {
      private:
         Listener * m_listener;
         ClientImpl * m_client;
         bool m_ignoreMessages;

      public:
         InternalListener() { m_listener = nullptr; m_client = nullptr; m_ignoreMessages = false; }
         void SetListener( Listener * listener ) { m_listener = listener; }
         Listener * GetListener() { return m_listener; }
         void SetClient( ClientImpl * client ) { m_client = client; }
         void SetIgnore( bool ignore ) { m_ignoreMessages = ignore; }

         void onMessage( const cms::Message * message ) throw() override;
   };


   std::string m_host;
   std::string m_port;
   std::string m_scope;

   cms::Connection * m_connection;
   cms::Session * m_session;
   cms::Destination * m_destination;
   vector< pair<string, cms::MessageConsumer *> > m_consumers;
   cms::MessageProducer * m_producer;

   InternalListener m_internalListener;
   InternalListener m_ignoreListener;

   bool m_isOpen;


   bool m_immediateMethod;
   bool m_bypassMode;
   bool m_subscribedAll;

   // variables needed for polling
   list< Message > m_messages;
   bool m_messageLockInitialized;
   pthread_mutex_t m_messageLock;
   pthread_cond_t  m_waitCondition;
   pthread_mutex_t m_waitMutex;



   ClientImpl()
   {
      m_connection = nullptr;
      m_session = nullptr;
      m_destination = nullptr;
      m_producer = nullptr;

      m_ignoreListener.SetIgnore( true );

      m_isOpen = false;

      m_immediateMethod = true;
      m_bypassMode = false;
      m_subscribedAll = false;
      m_messageLockInitialized = false;
   }
};


/// <summary>
/// Constructor
/// </summary>
Client::Client()
   : pimpl( new ClientImpl() )
{
   SetServerFromEnv();
   SetPortFromEnv();
   SetScopeFromEnv();

   pthread_mutex_init( &pimpl->m_messageLock, nullptr );
   pimpl->m_messageLockInitialized = true;
   pthread_cond_init( &pimpl->m_waitCondition, nullptr );
   pthread_mutex_init( &pimpl->m_waitMutex, nullptr );
}


/// <summary>
/// Destructor
/// </summary>
Client::~Client()
{
   CloseConnection();

   delete pimpl;
   pimpl = nullptr;
}


/// <summary>
/// Opens a connection to the server.
/// <para />
/// By default, it uses 3 system environment variables as parameters
/// <para />
/// VHMSG_SERVER - This specifies the server to connect to.  It can either
/// be an ip address or domain name
/// <para />
/// VHMSG_PORT - This specifies the port to connect to.  
/// <para />
/// VHMSG_SCOPE - A unique id used to distinguish messages sent by different
/// modules using the same server.  For example, if two users
/// are using the same server, they would set different scopes so that they
/// wouldn't receives each other's messages.
/// </summary>
bool Client::OpenConnection()
{
   string brokerURI = "tcp://" + pimpl->m_host + ":" + pimpl->m_port;

   bool useTopic = true;



   activemq::core::ActiveMQConnectionFactory * connectionFactory = nullptr;

   try
   {
      // initializeLibrary() is required to be called before any other activemq
      // call. this is meant to be a global per-process call.  So keep track of
      // how many vhmsg clients we have initialized.  Only call init on the
      // first call, and call shutdownLibrary() only after the last one has
      // been closed.
      if ( g_initCount++ == 0 )
      {
         activemq::library::ActiveMQCPP::initializeLibrary();
      }

      // Create a ConnectionFactory
      connectionFactory = new activemq::core::ActiveMQConnectionFactory( brokerURI );

      // Create a Connection
      pimpl->m_connection = connectionFactory->createConnection();
      pimpl->m_connection->start();

      // free the factory, we are done with it.
      delete connectionFactory;
      connectionFactory = nullptr;

      // Create a Session
      pimpl->m_session = pimpl->m_connection->createSession( cms::Session::AUTO_ACKNOWLEDGE );

      // Create the destination (Topic or Queue)
      if ( useTopic )
      {
         pimpl->m_destination = pimpl->m_session->createTopic( pimpl->m_scope.c_str() );
      }
      else
      {
         pimpl->m_destination = pimpl->m_session->createQueue( pimpl->m_scope.c_str() );
      }

      // Create a MessageProducer from the Session to the Topic or Queue
      pimpl->m_producer = pimpl->m_session->createProducer( pimpl->m_destination );
      pimpl->m_producer->setDeliveryMode( cms::DeliveryMode::NON_PERSISTENT );
   }
   catch ( cms::CMSException & )
   {
      delete connectionFactory;
      return false;
   }

   pimpl->m_isOpen = true;
   return true;
}


/// <summary>
/// Opens a connection to the server using a specified host.
/// See <see cref="GetConnection()"/>.
/// </summary>
/// <param name="host">the host to connect to.  It can either be an ip address
/// or domain name</param>
/// <seealso cref="OpenConnection()"/>
bool Client::OpenConnection( const char * server )
{
   pimpl->m_host = server;
   return OpenConnection();
}


/// <summary>
/// Opens a connection to the server using a specified host.
/// See <see cref="GetConnection()"/>.
/// </summary>
/// <param name="host">the host to connect to.  It can either be an ip address
/// or domain name</param>
/// <param name="port">the numeric port to connect to.</param>
/// <seealso cref="OpenConnection()"/>
bool Client::OpenConnection( const char * server, const char * port )
{
   pimpl->m_host = server;
   pimpl->m_port = port;
   return OpenConnection();
}


/// <summary>
/// Closes the connection to the server that was previously opened via
/// <see cref="GetConnection()"/>.
/// </summary>
void Client::CloseConnection()
{
   pimpl->m_isOpen = false;

   try
   {
      bool shutdownActiveMQLibrary = false;
      if ( pimpl->m_connection != nullptr )  // isOpen()?
      {
         g_initCount--;
         if ( g_initCount == 0 )
            shutdownActiveMQLibrary = true;
      }

      if ( pimpl->m_messageLockInitialized )
      {
         pthread_mutex_lock( &pimpl->m_messageLock );
         pthread_mutex_unlock( &pimpl->m_messageLock );
         pimpl->m_messageLockInitialized = false;
      }

      delete pimpl->m_destination;
      pimpl->m_destination = nullptr;

      size_t i;
      for ( i = 0; i < pimpl->m_consumers.size(); i++ )
      {
         delete pimpl->m_consumers[ i ].second;
      }
      pimpl->m_consumers.clear();

      delete pimpl->m_producer;
      pimpl->m_producer = nullptr;

      if ( pimpl->m_session != nullptr )
         pimpl->m_session->close();

      if ( pimpl->m_connection != nullptr )
         pimpl->m_connection->close();

      delete pimpl->m_session;
      pimpl->m_session = nullptr;

      delete pimpl->m_connection;
      pimpl->m_connection = nullptr;

      if ( shutdownActiveMQLibrary )
      {
         activemq::library::ActiveMQCPP::shutdownLibrary();
      }
   }
   catch ( cms::CMSException & e )
   {
      e.printStackTrace();
   }


   delete pimpl->m_connection;
   pimpl->m_connection = nullptr;
}


bool Client::IsOpen()
{
   return pimpl->m_isOpen;
}


/// <summary>
/// Sends a message to the server using 2 arguments.  The first argument is
/// the first word in the message, the second argument is the rest of
/// the message.
/// </summary>
/// <param name="op">The first word in the message</param>
/// <param name="arg">The rest of the message</param>
bool Client::Send( const char * op, const char * arg )
{
   string arg_encoded = encodeString(arg);
   string mess = string( op ) + " " + arg_encoded;

   cms::TextMessage * message = nullptr;
   try
   {
      message = pimpl->m_session->createTextMessage( mess );
   }
   catch ( cms::CMSException & )
   {
      return false;
   }

   message->setStringProperty( "ELVISH_SCOPE", pimpl->m_scope );
   message->setStringProperty( "MESSAGE_PREFIX", op );
   //message->setStringProperty( op, arg );

   try
   {
      pimpl->m_producer->send( message );
   }
   catch ( cms::CMSException & )
   {
      delete message;
      return false;
   }

   delete message;
   return true;
}


/// <summary>
/// Sends a message to the server.  The argument contains the full message
/// </summary>
/// <param name="opandarg">The full message to send</param>
bool Client::Send( const char * message )
{
   pair<string, string> opArg = split(string(message), string(" "));
   return Send( opArg.first.c_str(), opArg.second.c_str() );
}


bool Client::Send( const Message & in_message )
{
    string op;
    string encodedArg;

    if (in_message.message != string() )
    {
        pair<string, string> opArg = split(in_message.message, string(" "));
        op = opArg.first;
        encodedArg = encodeString(opArg.second);
    }
    else if (in_message.wmessage != wstring() )
    {
        pair<wstring, wstring> opArg = split(in_message.wmessage, wstring(L" "));
        op = vhcl::WStringToAnsi(opArg.first);
        encodedArg = encodeString(opArg.second);
    }
    else
    {
        return false;
    }

    std::map< string, string > args;
    const string esss = encodeString(string());
    for ( std::map< std::string, std::string >::const_iterator iter = in_message.messageMap.begin();
          iter != in_message.messageMap.end(); iter++ )
    {
        string encodedValue = encodeString(iter->second);

        if ( iter->first == op && encodedArg == esss && encodedArg != encodedValue )
        {
            encodedArg = encodedValue;
        }

        args.insert(make_pair(iter->first, encodedValue));
    }

    std::wstring wop = vhcl::AnsiToWString( op );
    const string esws = encodeString(wstring());
  
    for ( std::map< std::wstring, std::wstring >::const_iterator iter = in_message.wmessageMap.begin();
          iter != in_message.wmessageMap.end(); iter++ )
    {
        string encodedValue = encodeString(iter->second);
        if ( iter->first == wop && encodedArg == esws && encodedArg != encodedValue )
        {
            encodedArg = encodedValue;
        }

        args.insert(make_pair(vhcl::WStringToAnsi(iter->first), encodedValue));
    }


    cms::TextMessage * message = nullptr;

    try
    {
        message = pimpl->m_session->createTextMessage( op + " " + encodedArg );
    }
    catch ( cms::CMSException & )
    {
        return false;
    }

    message->setStringProperty( "ELVISH_SCOPE", pimpl->m_scope );
    message->setStringProperty( "MESSAGE_PREFIX", op );

    for ( std::map< std::string, std::string >::const_iterator iter = args.begin();
          iter != args.end(); iter++ )
    {
        message->setStringProperty( iter->first, iter->second );
    }

    try
    {
        pimpl->m_producer->send( message );
    }
    catch ( cms::CMSException & )
    {
        delete message;
        return false;
    }

    delete message;
    return true;
}


/// <summary>
/// 
/// </summary>
void Client::EnablePollingMethod()
{
   pimpl->m_immediateMethod = false;
}


/// <summary>
/// 
/// </summary>
void Client::EnableImmediateMethod()
{
   pimpl->m_immediateMethod = true;
}


/// <summary>
/// 
/// </summary>
void Client::SetBypassMode( bool enable )
{
   // This is an explicit bypass of the VHMsg message protocol to help support
   // non-vhmsg clients. Default is false.
   // Bypass mode doesn't require ELVISH_SCOPE and MESSAGE_PREFIX ActiveMQ
   // properties to be set.
   // All ActiveMQ messages will be received.
   // A single Subscribe() call will be required, but it's parameter will
   // be ignored.
   // Also, this function must be called first before Subscribe()
   pimpl->m_bypassMode = enable;
}


/// <summary>
/// 
/// </summary>
void Client::SetListener( Listener * listener )
{
   pimpl->m_internalListener.SetListener( listener );
   pimpl->m_internalListener.SetClient( this->pimpl );
}


/// <summary>
/// Subscribes to a message.  This notifies the server that we are interested
/// in messages that contain the given argument as the first word in the
/// message.
/// See <see cref="SendMessage(string,string)"/>
/// <para />
/// For each message that is received, the MessageEvent handler will be called
/// for all listeners.  See <see cref="OnMessage"/>
/// <para />
/// More than one subscribtion can be made.  Alternatively, an asterik (*) may
/// be sent as a special-case argument that indicates we're interested in *all*
/// messages.
/// This should be used very sparingly because it can cause quite a bit of
/// network traffic.
/// </summary>
/// <param name="req">Indicates what types of messages we are interested in
/// receiving.  This tells the server to send messages where the first word
/// matches req</param>
bool Client::Subscribe( const char * req )
{
   // check if we've already subscribed to this message
   for ( size_t i = 0; i < pimpl->m_consumers.size(); i++ )
   {
      if ( pimpl->m_consumers[ i ].first == req )
      {
         return true;
      }
   }

   // special case for asterisk.  If we pass in an asterisk, we are subscribing
   // to all messages
   string messageSelector;
   if ( strcmp( req, "*" ) == 0 )
   {
      messageSelector = "ELVISH_SCOPE = '" + pimpl->m_scope + "' AND MESSAGE_PREFIX LIKE '%'";
      pimpl->m_subscribedAll = true;
   }
   else
   {
      messageSelector = "ELVISH_SCOPE = '" + pimpl->m_scope + "' AND MESSAGE_PREFIX = '" + req + "'";
   }

   cms::MessageConsumer * consumer;

   try
   {
      if ( pimpl->m_bypassMode )
      {
         consumer = pimpl->m_session->createConsumer( pimpl->m_destination );
      }
      else
      {
         consumer = pimpl->m_session->createConsumer( pimpl->m_destination, messageSelector );
      }
   }
   catch ( cms::CMSException & )
   {
      return false;
   }

   pimpl->m_consumers.emplace_back( make_pair( req, consumer ) );

   try
   {
      consumer->setMessageListener( &pimpl->m_internalListener );
   }
   catch ( cms::CMSException & )
   {
      pimpl->m_consumers.pop_back();
      return false;
   }

   // if we subscribed to "*", remove all the other listeners to prevent
   // duplicate messages
   if ( pimpl->m_subscribedAll )
   {
       for ( size_t i = 0; i < pimpl->m_consumers.size(); i++ )
       {
          if ( pimpl->m_consumers[ i ].first.compare( "*" ) != 0 )
          {
              pimpl->m_consumers[ i ].second->setMessageListener( &pimpl->m_ignoreListener );
          }
       }
   }

   return true;
}


bool Client::Unsubscribe( const char * req )
{
   // if we are unsubscribing from "*", re-add all the other listeners so that
   // messages continue to be received (only once)
   if ( strcmp( req, "*" ) == 0 )
   {
      for ( size_t i = 0; i < pimpl->m_consumers.size(); i++ )
      {
         pimpl->m_consumers[ i ].second->setMessageListener( &pimpl->m_internalListener );
      }

      pimpl->m_subscribedAll = false;
   }


   for ( size_t i = 0; i < pimpl->m_consumers.size(); i++ )
   {
      if ( pimpl->m_consumers[ i ].first == req )
      {
         pimpl->m_consumers[ i ].second->close();
         pimpl->m_consumers.erase( pimpl->m_consumers.begin() + i );
         return true;
      }
   }

   return false;
}


/// <summary>
/// 
/// </summary>
void Client::Poll()
{
   if ( !pimpl->m_immediateMethod )
   {
      for ( ;; )
      {
         pthread_mutex_lock( &pimpl->m_messageLock );

         if ( pimpl->m_messages.empty() )
         {
            pthread_mutex_unlock( &pimpl->m_messageLock );
            break;
         }

         Message message = pimpl->m_messages.front();
         pimpl->m_messages.pop_front();

         pthread_mutex_unlock( &pimpl->m_messageLock );


         pimpl->m_internalListener.GetListener()->OnMessage( message );
      }
   }
}


/// <summary>
/// 
/// </summary>
void Client::WaitAndPoll( const double waitTimeSeconds )
{
   pthread_mutex_lock( &pimpl->m_waitMutex );

   timespec ts;
   vhcl::PosixClockGetTime((long*)&ts.tv_sec, (long*)&ts.tv_nsec);

   // add the offset
   ts.tv_sec  += (int)floor( waitTimeSeconds );
   // second to nanosecond
   ts.tv_nsec += (int)( ( waitTimeSeconds - floor( waitTimeSeconds ) ) * 1000000000 );

   pthread_cond_timedwait( &pimpl->m_waitCondition, &pimpl->m_waitMutex, &ts );

   Poll();

   pthread_mutex_unlock( &pimpl->m_waitMutex );
}


/// <summary>
/// 
/// </summary>
const std::string & Client::GetServer() const
{
   return pimpl->m_host;
}


/// <summary>
/// 
/// </summary>
const std::string & Client::GetPort() const
{
   return pimpl->m_port;
}


/// <summary>
/// Gets/Sets the scope being used for the connection
/// </summary>
const std::string & Client::GetScope() const
{
   return pimpl->m_scope;
}


/// <summary>
/// Gets/Sets the scope being used for the connection
/// </summary>
void Client::SetScope( const char * scope )
{
   pimpl->m_scope = scope;
}


/// <summary>
/// 
/// </summary>
void Client::SetServerFromEnv()
{
   const char * host = getenv( "VHMSG_SERVER" );
   if ( host != nullptr )
   {
      pimpl->m_host = host;
   }
   else
   {
      pimpl->m_host = "localhost";
   }
}


/// <summary>
/// 
/// </summary>
void Client::SetPortFromEnv()
{
   const char * port = getenv( "VHMSG_PORT" );
   if ( port != nullptr )
   {
      pimpl->m_port = port;
   }
   else
   {
      pimpl->m_port = "61616";
   }
}


/// <summary>
/// 
/// </summary>
void Client::SetScopeFromEnv()
{
   const char * scope = getenv( "VHMSG_SCOPE" );
   if ( scope != nullptr )
   {
      pimpl->m_scope = scope;
   }
   else
   {
      pimpl->m_scope = "DEFAULT_SCOPE";
   }
}


/// <summary>
/// 
/// </summary>
void ClientImpl::InternalListener::onMessage( const cms::Message * message ) throw()
{
   if ( m_ignoreMessages )
       return;

   if ( m_listener )
   {
      Message m;

      pair<string, wstring> s_ws = decodeString( ((cms::TextMessage *)message)->getText() );
      m.message = s_ws.first;
      m.wmessage = s_ws.second;

      vector< string > properties = message->getPropertyNames();
      for (auto & property : properties)
      {
         if ( property == "ELVISH_SCOPE" )
         {
            continue;
         }
         else if ( property == "MESSAGE_PREFIX" )
         {
            // note that the value of MESSAGE_PREFIX is never encoded, there is
            // no need to decode it.
            const string & prefix = message->getStringProperty( property );
            string remainder = m.message;
            remainder.erase( 0, prefix.length() + 1 );

            m.messageMap.emplace(prefix, remainder );
            m.wmessageMap.emplace( vhcl::AnsiToWString( prefix ), vhcl::AnsiToWString( remainder )  );
            continue;
         }

         pair<string, wstring> s_ws_value = decodeString( message->getStringProperty( property ) );
		 m.messageMap.emplace( property, s_ws_value.first );
		 m.wmessageMap.emplace( vhcl::AnsiToWString( property ), s_ws_value.second );
      }


      if ( m_client->m_immediateMethod )
      {
         m_listener->OnMessage( m );
      }
      else
      {
         pthread_mutex_lock( &m_client->m_messageLock );

         //printf( "onMessage: '%s'\n", message.message.c_str() );

         m_client->m_messages.emplace_back( m );  // this makes a copy

         pthread_mutex_unlock( &m_client->m_messageLock );

         // signal the other thread that we've received a message
         // (only used in WaitAndPoll() )
         pthread_mutex_lock( &m_client->m_waitMutex );
         pthread_cond_signal( &m_client->m_waitCondition );
         pthread_mutex_unlock( &m_client->m_waitMutex );
      }
   }
}
