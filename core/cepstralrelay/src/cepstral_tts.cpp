/*
   Part of SBM: SmartBody Module
   Copyright (C) 2008  University of Southern California

   SBM is free software: you can redistribute it and/or
   modify it under the terms of the Lesser GNU General Public License
   as published by the Free Software Foundation, version 3 of the
   license.

   SBM is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   Lesser GNU General Public License for more details.

   You should have received a copy of the Lesser GNU General Public
   License along with SBM.  If not, see:
       http://www.gnu.org/licenses/lgpl-3.0.txt

   CONTRIBUTORS:
      Edward Fast, USC
      Thomas Amundsen, USC
*/

#include "vhcl.h"

#include "cepstral_tts.h"

#include <map>

#include <xercesc/util/XMLUTF8Transcoder.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif
#include <xercesc/util/OutOfMemoryException.hpp>

#include "swift.h"


XERCES_CPP_NAMESPACE_USE


swift_port * port = NULL;
swift_engine * engine = NULL;
std::string fpathout;
DOMElement * rootElem;
DOMElement * wordElement;
DOMImplementation * impl;
DOMDocument * doc;
std::map<std::string, std::string> phonemeToViseme;
float word_end = 0;


swift_result_t audio_callback( swift_event * event, swift_event_t type, void * udata );


// class is taken from somewhere, unsure of it's origins.  Here is one of many examples of use across the net:
//    http://mail-archives.apache.org/mod_mbox/xerces-c-dev/200210.mbox/%3C7C78B8615661D311B15500A0C9AB28C1915E40@earth.telestream.net%3E
class XStr
{
   public:
      // -----------------------------------------------------------------------
      //  Constructors and Destructor
      // -----------------------------------------------------------------------
      XStr( const char * const toTranscode )
      {
         // Call the private transcoding method
         fUnicodeForm = XMLString::transcode( toTranscode );
      }

      ~XStr()
      {
         XMLString::release( &fUnicodeForm );
      }


      // -----------------------------------------------------------------------
      //  Getter methods
      // -----------------------------------------------------------------------
      const XMLCh * unicodeForm() const
      {
         return fUnicodeForm;
      }

   private:
       // -----------------------------------------------------------------------
       //  Private data members
       //
       //  fUnicodeForm
       //      This is the Unicode XMLCh format of the string.
       // -----------------------------------------------------------------------
       XMLCh * fUnicodeForm;
};

#define X( str ) XStr( str ).unicodeForm()


void cepstral_tts::init()
{
   //fpathout = "";


   phonemeToViseme[ "pau" ] = "_";  // SIL
   phonemeToViseme[ "aa" ]  = "Ao"; // AA
   phonemeToViseme[ "ae" ]  = "Ih"; // AE
   phonemeToViseme[ "ah" ]  = "Ih"; // AH
   phonemeToViseme[ "ao" ]  = "Ao"; // AO
   phonemeToViseme[ "ax" ]  = "Ih"; // AX
   phonemeToViseme[ "aw" ]  = "Ih"; // AW
   phonemeToViseme[ "ay" ]  = "Ih"; // AY
   phonemeToViseme[ "b" ]   = "BMP";//  B
   phonemeToViseme[ "ch" ]  = "j";  // CH
   phonemeToViseme[ "d" ]   = "D";  //  D
   phonemeToViseme[ "dh" ]  = "Th"; // DH
   phonemeToViseme[ "dx" ]  = "D";  // ??
   phonemeToViseme[ "eh" ]  = "Ih"; // EH
   phonemeToViseme[ "er" ]  = "Er"; // ER
   phonemeToViseme[ "ey" ]  = "Ih"; // ?? probably EY
   phonemeToViseme[ "f" ]   = "F";  //  F
   phonemeToViseme[ "g" ]   = "KG"; //  G
   phonemeToViseme[ "h" ]   = "Ih"; // HH
   phonemeToViseme[ "hh" ]  = "Ih"; // HH
   phonemeToViseme[ "ih" ]  = "Ih"; // IH
   phonemeToViseme[ "iy" ]  = "EE"; // IY
   phonemeToViseme[ "jh" ]  = "j";  // JH
   phonemeToViseme[ "k" ]   = "KG"; //  K
   phonemeToViseme[ "l" ]   = "D";  //  L
   phonemeToViseme[ "m" ]   = "BMP";//  M
   phonemeToViseme[ "n" ]   = "NG"; //  N
   phonemeToViseme[ "ng" ]  = "NG"; // NG
   phonemeToViseme[ "ow" ]  = "Oh"; // OW
   phonemeToViseme[ "oy" ]  = "Oh"; // OY
   phonemeToViseme[ "p" ]   = "BMP";//  P
   phonemeToViseme[ "r" ]   = "R";  //  R
   phonemeToViseme[ "s" ]   = "Z";  //  S
   phonemeToViseme[ "sh" ]  = "j";  // SH
   phonemeToViseme[ "T" ]   = "D";  //  T ?
   phonemeToViseme[ "t" ]   = "D";  //  T ?
   phonemeToViseme[ "th" ]  = "Th"; // TH
   phonemeToViseme[ "uh" ]  = "Oh"; // UH
   phonemeToViseme[ "uw" ]  = "Oh"; // UW
   phonemeToViseme[ "v" ]   = "F";  //  V
   phonemeToViseme[ "w" ]   = "OO"; //  W
   phonemeToViseme[ "y" ]   = "OO"; //  Y
   phonemeToViseme[ "z" ]   = "Z";  //  Z
   phonemeToViseme[ "zh" ]  = "J";  // ZH
   //phonemeToViseme[ "i" ]  = "";  // 
   //phonemeToViseme[ "j" ]  = "";  // 
   //need phonemeToViseme entries for "i" and "j"?


   // Open the Swift TTS Engine
   if ( ( engine = swift_engine_open( NULL ) ) == NULL ) 
   {
      fprintf( stderr, "Failed to open Swift Engine.\n" );
      goto all_done;
   }

   // Open a Swift Port through which to make TTS calls
   if ( ( port = swift_port_open( engine, NULL ) ) == NULL ) 
   {
      fprintf( stderr, "Failed to open Swift Port.\n" );
      goto all_done;
   }

   swift_port_set_param_string( port, "tts/content-type", "text/ssml", NULL );

   // Find the first voice on the system
   swift_voice * voice;
   if ( ( voice = swift_port_find_first_voice( port, NULL, NULL ) ) == NULL )
   {
      fprintf( stderr, "Failed to find any voices!\n" );
      goto all_done;
   }

   // Set the voice found by find_first_voice() as the port's current voice
   if ( SWIFT_FAILED( swift_port_set_voice( port, voice ) ) )
   {
      fprintf( stderr, "Failed to set voice.\n" );
      goto all_done;
   }

   // Set the voice found by find_first_voice() as the port's current voice
   if ( SWIFT_FAILED( swift_port_set_voice( port, voice ) ) )
   {
      fprintf( stderr, "Failed to set voice.\n" );
      goto all_done;
   }

   // Initialize the XML4C2 system.
   try
   {
      XMLPlatformUtils::Initialize();
   }
   catch ( const XMLException & toCatch )
   {
      char * pMsg = XMLString::transcode( toCatch.getMessage() );
      XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n" << "  Exception message:" << pMsg;
      XMLString::release( &pMsg );
   }

all_done:
   return;
}


void create_xml_document( const char * file_name )
{
   impl =  DOMImplementationRegistry::getDOMImplementation( X( "Core" ) );

   if ( impl != NULL )
   {
      int errorCode = 0;

      try
      {
         //XMLCh * end = XMLString::transcode( "end" );
         //XMLCh * start = XMLString::transcode( "start" );
         XMLCh * name = XMLString::transcode( "name" );
         XMLCh * file_path = XMLString::transcode( file_name );

         doc = impl->createDocument(
            0,              // root element namespace URI.
            X( "speak" ),   // root element name
            0);             // document type object (DTD).

         rootElem = doc->getDocumentElement();

         DOMElement * soundFileElement = doc->createElement( X( "soundFile" ) );
         soundFileElement->setAttribute( name, file_path );
         rootElem->appendChild( soundFileElement );

         wordElement = doc->createElement( X( "word" ) );
      }
      catch ( const OutOfMemoryException & )
      {
         XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
         errorCode = 5;
      }
      catch ( const DOMException & e )
      {
         XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
         errorCode = 2;
      }
      catch ( const XMLException & toCatch )
      {
         printf( "XMLException occurred: %s", toCatch.getCode() );
         errorCode = 4;
      }
      catch (...)
      {
         XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
         errorCode = 3;
      }
   }
}


std::string cepstral_tts::tts( const char * text, const char * file_name )
{
   unsigned int event_mask = (unsigned int)SWIFT_EVENT_ALL;
   swift_waveform * wave = swift_waveform_new();

   create_xml_document( file_name );

   // Set audio_callback as a callback, with the output file as its param 
   swift_port_set_callback( port, &audio_callback, event_mask, wave );

   swift_result_t rv = swift_port_speak_text( port, text, 0, NULL, NULL, NULL );

   if ( SWIFT_FAILED( rv ) )
   {
      fprintf( stderr, "Failed to speak prompt: %s\n", swift_strerror( rv ) );
      goto all_done;
   }

   // print the information about the wave
//    swift_waveform_print(wave);

   //* If you need the data converted here is where you do it:
   // * swift_waveform_set_channels(wave, 2);   <-- make it stereo  
   // * swift_waveform_resample(wave, 8000);    <-- convert to 8kHz  
   // * swift_waveform_convert(wave, "ulaw");   <-- convert to uLaw  

   // save the wave to a file
   swift_waveform_save( wave, file_name, "riff" );


all_done:
   DOMLSSerializer* theSerializer = DOMImplementation::getImplementation()->createLSSerializer();
   XMLCh * xml_result = theSerializer->writeToString( rootElem );
   char * result = XMLString::transcode( xml_result );
   theSerializer->release();

   return std::string( result );
}


swift_result_t audio_callback( swift_event * event, swift_event_t type, void * udata )
{
   swift_waveform * wave = static_cast<swift_waveform *>( udata );

   // Get the string name of the event type
   const char * type_name = swift_event_type_get_name( type );

   // Get the event times
   float time_start;
   float time_len;
   swift_event_get_times( event, &time_start, &time_len );

   // Get the event text
   char * text = "";
   swift_event_get_text( event, &text );

   // ask for the data to be put into the wave object.  In this example we 
   // want the data to be concatenated onto the wave object we are sending in
   swift_event_t rv = static_cast<swift_event_t>( swift_event_get_wave( event, &wave, true ) );

   //////////////////////////////////////////////////////////////////////////////////////////

   // Watch for special case help request
   if ( impl != NULL )
   {
      int errorCode = 0;
     
      try
      {
         XMLCh * start = XMLString::transcode( "start" );
         XMLCh * end = XMLString::transcode( "end" );
         XMLCh * type = XMLString::transcode( "type" );

         std::string end_f = vhcl::Format( "%0.6f", ( time_start + time_len ) );
         XMLCh * end_time = XMLString::transcode( end_f.c_str() );

         std::string start_f = vhcl::Format( "%0.6f", time_start );
         XMLCh * start_time = XMLString::transcode( start_f.c_str() );

         if ( strcmp( type_name, "phoneme" ) == 0 )
         {
            XMLCh * phone_type;

            std::map<std::string, std::string>::iterator iter = phonemeToViseme.find( text );

            if ( iter == phonemeToViseme.end() )
            {
               printf( "COULDN'T FIND MATCH FOR: %s\n", text );
               phone_type = XMLString::transcode( text );
            }
            else
            {
               phone_type = XMLString::transcode( iter->second.c_str() );
            }

            DOMElement * visemeElement = doc->createElement( X( "viseme" ) );
            visemeElement->setAttribute( start, start_time );
            visemeElement->setAttribute( type, phone_type );

            wordElement->appendChild( visemeElement );
         }

         if ( ( strcmp( type_name, "word" ) == 0 ) || ( word_end == time_start )
           || ( strcmp( type_name, "end" ) == 0 ) )
         {
            if ( wordElement->hasChildNodes() )
               rootElem->appendChild( wordElement );

            wordElement = doc->createElement( X( "word" ) );

            wordElement->setAttribute( end, end_time );
            word_end = time_start + time_len;

            wordElement->setAttribute( start, start_time );
         }
      }
      catch ( const OutOfMemoryException & )
      {
         XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
         errorCode = 5; 
      }
      catch ( const DOMException & e )
      {
         XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
         errorCode = 2;
      }
      catch ( const XMLException & toCatch )
      {
         printf( "XMLException occurred: %s", toCatch.getCode() );
         errorCode = 4;
      }
      catch (...)
      {
         XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
         errorCode = 3;
      }
   }

   return static_cast<swift_result_t>( rv );
}
