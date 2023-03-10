/*
 *  main.cpp - part of SBM: SmartBody Module
 *  Copyright (C) 2008  University of Southern California
 *
 *  SBM is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SBM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SBM.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Arno Hartholt, USC
 */

#include "vhcl.h"

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <process.h>
#include <string>
#include <sstream> 
#include <fstream>

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
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "vhmsg-tt.h"


XERCES_CPP_NAMESPACE_USE

using std::string;
using std::vector;
using std::map;


class XStr
{
public :
    XStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()


struct Marker  // Not used
{ 
   string id;
   string time;
};


struct Word   // Currently not used; markers are placed following protocol, see below
{
   string text;
   Marker first_marker;
   Marker second_marker;
};

struct Viseme
{
   string viseme;
   string start;
   string end;

   Viseme( const string & _viseme, const string & _start, const string & _end ) { viseme = _viseme; start = _start; end = _end; }
};

struct Utterance
{
   string utterance_id;          // ID in NPC Editor / audio file name minus .wav
   string speech_id;             // sp1, etc. Static or dynamic? Generated by NVBG?
   string speaker;               
   string addressee;
   string text;                  // Whole utterance text
   vector<string> words;         // All the individual words in the utterance
   map<string, string> markers;  // Mapping of ID (T0, T1, etc.) to time. All the markers wrap around words. 
                                 // Assumed protocol: <marker T0/>word_0<marker T1/><marker T2/>word_1<marker T3/>
   vector< Viseme > visemes;     // Viseme info parsed from the .ltf file
};


typedef map<string, string> markerType;

typedef map<string, Utterance> utteranceType;
utteranceType m_utterances;

bool m_reply_received = false;
char * m_request_message = "RemoteSpeechCmd";
char * m_reply_message = "RemoteSpeechReply";
string output_dir = "";
string m_speech_id_prefix = "sp";
string m_marker_id_prefix = "T";


string remove_double_quotes( const string & s )
{
   string c = s;

   while ( c.find( "\"" ) != string::npos )
   {
      c.replace( c.find( "\"" ), 1, "" );
   }

   return c;
}


string int_to_string( const int i )
{
   std::ostringstream ostr;
   ostr << i;
   return ostr.str();
}


void write_string_to_file( const string & name, const string & text )
{
   string file_name = name + ".bml";
   string path = output_dir + "/";
   string file = path + file_name;
   std::ofstream out( file.c_str() );
   if ( out )
   {
      out << text;
      out.close();
   }
   else 
   {
      printf( "\n\nError when trying to write to file '%s.bml'.\n\nPress any key to exit.", file_name.c_str() );
      _getch();
      exit(1);
   }
}


string create_bml( Utterance & ut )
{
   string result = "";
   int errorCode = 0;
   DOMImplementation * impl =  DOMImplementationRegistry::getDOMImplementation( X( "Core" ) );

   if ( impl != NULL )
   {
      try
      {
         XMLCh * id = XMLString::transcode( "id" );
         XMLCh * start = XMLString::transcode( "start" );
         XMLCh * ready = XMLString::transcode( "ready" );
         XMLCh * stroke = XMLString::transcode( "stroke" );
         XMLCh * relax = XMLString::transcode( "relax" );
         XMLCh * end = XMLString::transcode( "end" );
         XMLCh * time = XMLString::transcode( "time" );
         XMLCh * level = XMLString::transcode( "level" );
         XMLCh * type = XMLString::transcode( "type" );
         //XMLCh * ref = XMLString::transcode( "ref" );
         XMLCh * viseme = XMLString::transcode( "viseme" );
         XMLCh * articulation = XMLString::transcode( "articulation" );

         string first_marker_id = m_marker_id_prefix + int_to_string( 0 );
         string file_name = ut.utterance_id + ".wav";
         //XMLCh * id_value = XMLString::transcode( file_name.c_str() );

         DOMDocument* doc = impl->createDocument(
            0,                    // root element namespace URI.
            X( "bml" ),           // root element name
            0 );                  // document type object (DTD).

         //doc->setEncoding( X( "UTF-8" ) );
         doc->setXmlVersion( X( "1.0" ) );
         
         DOMElement * rootElement = doc->getDocumentElement();

         DOMElement * speechElement = doc->createElement( X( "speech" ) );
         speechElement->setAttribute( id, X( ut.speech_id.c_str() ) );
         speechElement->setAttribute( start, X( "0" ) );

         if ( ut.markers.size() > 0 )
         {
            speechElement->setAttribute( ready, X( ut.markers[ first_marker_id ].c_str() ) );
         }
         else
         {
            speechElement->setAttribute( ready, X( "0" ) );
         }

         speechElement->setAttribute( stroke, X( "0" ) );                              // <----- TODO: How to get this??
         speechElement->setAttribute( relax, X( "0" ) );                               // <----- TODO: How to get this??
         speechElement->setAttribute( end, X( "0" ) );                                 // <----- TODO: How to get this??
         rootElement->appendChild( speechElement );

         DOMElement * textElement = doc->createElement( X( "text" ) );
         speechElement->appendChild( textElement );

         DOMElement * descriptionElement = doc->createElement( X( "description" ) );
         descriptionElement->setAttribute( level, X( "1" ));
         descriptionElement->setAttribute( type, X( "audio/x-wav" ));
         speechElement->appendChild( descriptionElement );

         DOMElement * fileElement = doc->createElement( X( "file" ) );
         fileElement->setAttribute( level, X( file_name.c_str() ));                         
         descriptionElement->appendChild( fileElement );

         // Add synch points (markers)
         for (unsigned int i = 0; i < ut.markers.size(); i++)
         {
            string key = m_marker_id_prefix + int_to_string( i );
            DOMElement * synchElement = doc->createElement( X( "sync" ) );
            synchElement->setAttribute( id, X( key.c_str() ) );
            synchElement->setAttribute( time, X( ut.markers[ key ].c_str() ) );
            textElement->appendChild( synchElement );
            
            // Add word
            if (i % 2 == 0)
            {
               DOMText * word = doc->createTextNode( X( ut.words.at( i / 2 ).c_str() ) );
               textElement->appendChild( word );
            }
         }


         // add viseme info  (if exists)
         for ( size_t i = 0; i < ut.visemes.size(); i++ )
         {
            DOMElement * lipsElement = doc->createElement( X( "lips" ) );
            lipsElement->setAttribute( viseme, X( ut.visemes[ i ].viseme.c_str() ) );
            lipsElement->setAttribute( articulation, X( "1.0" ) );
            lipsElement->setAttribute( start, X( ut.visemes[ i ].start.c_str() ) );
            lipsElement->setAttribute( ready, X( ut.visemes[ i ].start.c_str() ) );
            lipsElement->setAttribute( relax, X( ut.visemes[ i ].end.c_str() ) );
            lipsElement->setAttribute( end,   X( ut.visemes[ i ].end.c_str() ) );
            rootElement->appendChild( lipsElement );
         }


         DOMLSSerializer * theSerializer = DOMImplementation::getImplementation()->createLSSerializer();
         theSerializer->getDomConfig()->setParameter( X( "format-pretty-print" ), true);
         XMLCh * xml_result = theSerializer->writeToString( rootElement );
         result = XMLString::transcode( xml_result );
         printf( "XML Result: '%s'", result.c_str() );
         theSerializer->release();
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

   // TODO: Get this from Xerces
   string full_result = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" + result;
   return full_result;
}


void tt_client_callback( const char * op, const char * args, void * user_data )
{
   // Parsers TTS reply message. Example message:
   // RemoteSpeechReply utah 1 OK: <?xml version="1.0" encoding="UTF-8"?><speak><soundFile name="C:\saso\saso\libs\beavin\..\..\..\dimr\tmpaudio\utah-yer_new_to_town.wav"/>
   //   <viseme start="0.0" type="_"/>
   //   <viseme start="0.0" type="_"/>
   //   <mark name="sp1:T0" time="0.049977324263038546"/><word end="0.16997732426303855" start="0.049977324263038546">
   //      <viseme start="0.049977324263038546" type="OO"/>
   //      <viseme start="0.1199546485260771" type="Oh"/>
   //   </word><mark name="sp1:T2" time="0.16997732426303855"/><mark name="sp1:T1" time="0.16997732426303855"/>
   //   <word end="0.2619954648526077" start="0.16997732426303855">
   //      <viseme start="0.16997732426303855" type="Ih"/>
   //      <viseme start="0.22798185941043084" type="R"/>
   //   </word><mark name="sp1:T4" time="0.2619954648526077"/><mark name="sp1:T3" time="0.2619954648526077"/>
   //   <word end="0.3929705215419501" start="0.2619954648526077">
   //      <viseme start="0.2619954648526077" type="NG"/>
   //      <viseme start="0.28798185941043086" type="OO"/>
   //      <viseme start="0.35297052154195013" type="Oh"/>
   //   </word><mark name="sp1:T6" time="0.3929705215419501"/><mark name="sp1:T5" time="0.3929705215419501"/>
   //   <word end="0.5249886621315193" start="0.3929705215419501">
   //      <viseme start="0.3929705215419501" type="D"/>
   //      <viseme start="0.4799546485260771" type="Ih"/>
   //   </word><mark name="sp1:T8" time="0.5249886621315193"/><mark name="sp1:T7" time="0.5249886621315193"/>
   //   <word end="0.9479818594104309" start="0.5249886621315193">
   //      <viseme start="0.5249886621315193" type="D"/>
   //      <viseme start="0.6079818594104308" type="Ih"/>
   //      <viseme start="0.8379591836734694" type="NG"/>
   //   </word><mark name="sp1:T9" time="0.9479818594104309"/>
   //   <viseme start="0.9479818594104309" type="_"/>
   //   <viseme start="1.0029931972789117" type="_"/>
   //</speak>

   if ( strcmp( op, m_reply_message ) == 0 )
   {
      printf( "\nReceived - '%s %s'\n", op, args );

      string message_c = args;

      // parse the string
      std::vector< std::string > tokens;
      const string delimiters = " ";
      vhcl::Tokenize( message_c, tokens, delimiters );

      string agent_name = tokens.at( 0 );
      string message_id = tokens.at( 1 );
      string file_name = tokens.at( 2 );
      size_t prefix_length = message_c.find( file_name, 0 ) + file_name.length() + 1;
      string utterance = message_c.substr( prefix_length );  // strip off the prefix, only using the xml

      // remove anything after </speech> tag
      size_t postfix_pos = utterance.rfind( "</speech>" );
      if ( postfix_pos != string::npos )
         utterance = utterance.substr( 0, postfix_pos + 9 );

      // Parse response
      xercesc::XercesDOMParser * parser = new XercesDOMParser;

      parser->setValidationScheme( XercesDOMParser::Val_Never );
      parser->setDoNamespaces( false );
      parser->setDoSchema( false );
      parser->setLoadExternalDTD( false );
      
      // Tags
      //XMLCh * speak = XMLString::transcode( "speak" );
      //XMLCh * viseme = XMLString::transcode( "viseme" );
      XMLCh * mark = XMLString::transcode( "mark" );
      //XMLCh * word = XMLString::transcode( "word" );
      XMLCh * sound_file = XMLString::transcode( "soundFile" );

      // Attributes
      XMLCh * name = XMLString::transcode( "name" );
      //XMLCh * start = XMLString::transcode( "start" );
      //XMLCh * end = XMLString::transcode( "end" );
      XMLCh * time = XMLString::transcode( "time" );
      //XMLCh * type = XMLString::transcode( "type" );
      
      string utterance_id = "";;

      try
      {
         MemBufInputSource inputSource( (const XMLByte*)utterance.c_str(), (unsigned int)strlen(utterance.c_str()), "XMLBuffer" );
         parser->parse( inputSource );

         // no need to free this pointer - owned by the parent parser object
         DOMDocument* xmlDoc = parser->getDocument();

         // Get the top-level element ("speak")
         DOMElement* elementRoot = xmlDoc->getDocumentElement();
         if( !elementRoot ) throw(std::runtime_error( "empty XML document" ));

         DOMNodeList*      children = elementRoot->getChildNodes();
         const  XMLSize_t nodeCount = children->getLength();

         // First find utterance_id 
         for( XMLSize_t xx = 0; xx < nodeCount; ++xx )
         {
            DOMNode* currentNode = children->item(xx);
            if( currentNode->getNodeType() &&  // true is not NULL
               currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element 
            {
                // Found node which is an Element. Re-cast node as element
               DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );

               // If element is soundFile tag, get name and parse it, so we get our utterance ID
               if( XMLString::equals(currentElement->getTagName(), sound_file))
               {
                  const XMLCh* xmlch_name  = currentElement->getAttribute(name);
                  utterance_id = XMLString::transcode(xmlch_name);
                  // Get rid of path and extension to obtain ID
                  size_t last_slash = utterance_id.rfind( "\\" );
                  if ( last_slash != string::npos )
                     utterance_id = utterance_id.substr( last_slash + 1, utterance_id.size() );
                  utterance_id = utterance_id.substr( 0, utterance_id.size() - 4 );
                  break;
               }
            }
         }

         if ( utterance_id == "" )
         {
            printf( "\n\nERROR while parsing TTS reply: no soundFile tag.\n\nPress any key to exit.\n" );
            _getch();
            exit(1);
         }
         else
         {
            Utterance * ut = &m_utterances[ utterance_id ];

            // Now look for all the markers and add their timing info to the Utterance
            for( XMLSize_t xx = 0; xx < nodeCount; ++xx )
            {
               DOMNode* currentNode = children->item(xx);
               if( currentNode->getNodeType() &&  // true is not NULL
                  currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element 
               {
                  DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
                  if( XMLString::equals(currentElement->getTagName(), mark))
                  {
                     const XMLCh* xmlch_name  = currentElement->getAttribute(name);
                     string name = XMLString::transcode(xmlch_name);

                     const XMLCh* xmlch_time  = currentElement->getAttribute(time);
                     string time = XMLString::transcode(xmlch_time);
                     
                     ut->markers[ name ] = time;
                  }
               }
            }
         }

      }
      catch( xercesc::XMLException& e )
      {
         char * message = xercesc::XMLString::transcode( e.getMessage() );
         XERCES_STD_QUALIFIER cerr << "An error occurred parsing the document" << message << XERCES_STD_QUALIFIER endl;
         XMLString::release( &message );
      }

      m_reply_received = true;
   }
   else if ( strcmp( op, "vrKillComponent" ) == 0 )
   {
      if ( _stricmp( args, "word_breaker" ) == 0 ||
         _stricmp( args, "all" ) == 0 )
      {
         exit(0);
      }
   }
}


void vhmsg_loop( void * ignore )
{
   while ( m_reply_received == false )
   {
      vhmsg::ttu_wait( 0.5 );
   }

   _endthread();
}


void init_messaging()
{
   vhmsg::ttu_set_client_callback( tt_client_callback );
   int err = vhmsg::ttu_open();
   if ( err == vhmsg::TTU_SUCCESS )
   {
      printf( "VHMSG_SERVER: %s\n", vhmsg::ttu_get_server() );
      printf( "VHMSG_SCOPE: %s\n", vhmsg::ttu_get_scope() );

      err = vhmsg::ttu_register( m_request_message );
      err = vhmsg::ttu_register( m_reply_message );
      err = vhmsg::ttu_register( "vrKillComponent" );
   }
   else
   {
      printf( "An error occured while tring to establish ActiveMQ connection.\n\nPress any key to exit.\n" );
      _getch();
      exit(1);
   }
}


void init_XML()
{
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
}


// Creates TTS request message. Example message:
// RemoteSpeechCmd speak utah 1 M021 utah-yer_new_to_town.wav <?xml version="1.0" encoding="UTF-8"?><speech id="sp1" ref="" type="application/ssml+xml">
//	 <mark name="sp1:T0" />you 
//	 <mark name="sp1:T1" />
//	 <mark name="sp1:T2" />are
//	 <mark name="sp1:T3" />
//	 <mark name="sp1:T4" />new
//	 <mark name="sp1:T5" />
//	 <mark name="sp1:T6" />to
//	 <mark name="sp1:T7" />
//	 <mark name="sp1:T8" />town
//	 <mark name="sp1:T9" />
//	 </speech>
string create_tts_request( const string & utterance_id, const string & voice )
{
   Utterance * ut = &m_utterances[ utterance_id ];

   // For each word, attach numbered markers before and after it
   string words_and_markers = ""; 
   string mark_id = "T";
   string mark_begin = "<mark name=\"";
   string mark_end = "\"/>";

   for (unsigned int i = 0; i < ut->words.size(); i++ )
   {
      string mrkr_id = mark_id + int_to_string( i * 2 );
      string marker1 = mark_begin + mrkr_id + mark_end;
      ut->markers[ mrkr_id ] = "-1";

      mrkr_id = mark_id + int_to_string( i * 2 + 1 );
      string marker2 = mark_begin + mrkr_id + mark_end;
      ut->markers[ mrkr_id ] = "-1";

      words_and_markers += marker1 + ut->words.at( i ) + marker2;
   }

   string xml = "speak " + ut->speaker + " 1 " + voice + " " + ut->utterance_id + ".wav <?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<speech id=\"" + ut->speech_id + "\" ref=\"\" type=\"application/ssml+xml\">" + words_and_markers + "</speech>";

   return xml;
}


void parse_ltf_file( const string & ltf_file_name, Utterance & utterance )
{
   // phonemeIndex isn't needed unless you want to use the strings for debugging purposes
   // the 0-40 number indices are used in the .ltf file  (eg, index 10 equals phoneme "Oy", which is mapped to Viseme "oh")
   // phonemeToViseme was created by hand, using the chart below (taken from doctor.map).  Phonemes are along the side, Visemes are at the top.  Eat, Earth, etc are the Impersonator names, EE, Er, Ih are the names used by the Bonebus
/*
   //phoneme-viseme map Impersonator
   //                          Eat   Earth If    Ox    Oat   Wet   Size Church Fave Though Told Bump   New   Roar Cage
   const string phon2Vis[] = { "EE", "Er", "Ih", "Ao", "oh", "OO", "Z", "j",   "F", "Th",  "D", "BMP", "NG", "R", "KG" };

                           Iy=0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Ih=0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Eh=0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Ey=0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Ae=0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Aa=0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Aw=0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Ay=0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Ah=0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Ao=0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Oy=0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Ow=0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Uh=0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Uw=0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Er=0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Ax=0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           S =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Sh=0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Z =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Zh=0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           F =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.70, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Th=0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00
                           V =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.70, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Dh=0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00
                           M =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00
                           N =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00
                           Ng=0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00
                           L =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00
                           R =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00
                           W =0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Y =0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Hh=0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           B =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00
                           D =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00
                           Jh=0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           G =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.20
                           P =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00
                           T =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00
                           K =0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.20
                           Ch=0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.85, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00

                           Sil=0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           ShortSil=0.00, 0.00, 0.20, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
                           Flap=0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.50, 0.00, 0.00, 0.00, 0.00
*/

   //                                  0                             5                             10                            15                          20                           25                          30                            35                            40
// const char * phonemeIndex[]    = { "Iy", "Ih", "Eh", "Ey", "Ae", "Aa", "Aw", "Ay", "Ah", "Ao", "Oy", "Ow", "Uh", "Uw", "Er", "Ax", "S", "Sh", "Z", "Zh", "F", "Th", "V", "Dh", "M",   "N",  "Ng", "L", "R", "W",  "Y",  "Hh", "B",   "D", "Jh", "G",  "P",   "T", "K",  "Ch", "Sil", "ShortSil", "Flap" };
   const char * phonemeToViseme[] = { "EE", "Ih", "Ih", "Ih", "Ih", "Ao", "Ih", "Ih", "Ih", "Ao", "oh", "oh", "oh", "oh", "Er", "Ih", "Z", "j",  "Z", "j",  "F", "Th", "F", "Th", "BMP", "NG", "NG", "D", "R", "OO", "OO", "Ih", "BMP", "D", "j",  "KG", "BMP", "D", "KG", "j",  "_",   "_",        "_" };


   utterance.visemes.clear();

   FILE * f = fopen( ltf_file_name.c_str(), "r" );
   if ( f == NULL )
   {
      printf( "ERROR: Couldn't load .ltf file:  %s\n", ltf_file_name.c_str() );
      return;
   }


   // search for the "phoneme list" section in the file

   char line[ 512 ];
   while ( fgets( line, 512, f ) != NULL )
   {
      string strLine = line;
      if ( strLine.find( "// Phoneme Timing List" ) != strLine.npos )
      {
         break;
      }
   }

   while ( fgets( line, 512, f ) != NULL )
   {
      string strLine = line;

      if ( strLine.find( "// Function Curve Data" ) != strLine.npos )
      {
         // we've reached the end of the section, we're done
         break;
      }

      if ( strLine.length() < 0 )
      {
         continue;
      }

      // we're looking for a line in the following format:
      //   <phoneme index> <start time> <end time>
      //   eg: 40 0 0.123

      vector<string> tokens;
      vhcl::Tokenize( strLine, tokens, " \r\n" );

      if ( tokens.size() < 3 )
      {
         // line is in the wrong format
         continue;
      }

      string strPhonemeIndex = tokens[ 0 ];
      string strStartTime = tokens[ 1 ];
      string strEndTime = tokens[ 2 ];

      string strVisemeIndex = phonemeToViseme[ atoi( strPhonemeIndex.c_str() ) ];

      utterance.visemes.emplace_back( Viseme( strVisemeIndex, strStartTime, strEndTime ) );
   }

   fclose( f );
}


int main( int argc, char * argv[] )
{
   string voice = "MicrosoftAnna";
   string input_file = "";
   string ltf_dir = "";
   vector<string> error_utterances;
   //bool quit = false;

   // Parse command line parameters
   for ( int i = 1; i < argc; i += 2 )
   {
      if ( argc <= i + 1 )
      {
         printf( "Missing argument for parameter %s.\n\nPress any key to exit.\n", argv[ i ] );
         _getch();
         exit(1);
      }
	   else if ( strcmp( argv[ i ], "-voice" ) == 0 )
      {
         printf( "Voice: %s\n", argv[ i + 1 ] );
         voice = argv[ i + 1 ];
      }
      else if ( strcmp( argv[ i ], "-input" ) == 0 )
      {
         printf( "Input file: %s\n", argv[ i + 1 ] );
         input_file = argv[ i + 1 ];
      }
      else if ( strcmp( argv[ i ], "-ltf" ) == 0 )
      {
         printf( "Using LTF files for saving phoneme->viseme info\n" );
         printf( "LTF Directory: %s\n", argv[ i + 1 ] );
         ltf_dir = argv[ i + 1 ];
      }
      else if ( strcmp( argv[ i ], "-output" ) == 0 )
      {
         printf( "Output directory: %s\n", argv[ i + 1 ] );
         output_dir = argv[ i + 1 ];
      } 
      else
      {
         printf( "Unknown argument '%s'. \n\n"
            "Known arguments:\n"
			"-voice <voice>, voice to use when querying TTS engine\n"
            "-input <input_file_name>, file with utterances text and ID tuples.\n"
            "-ltf <ltf_directory_name>, optionally parse Impersonator .ltf files to use for phoneme info.\n"
            "-output <output_directory_name>, location where word break boundary timing files will be saved.\n\n"
            "Press any key to exit.\n", argv[ i ] );
         _getch();
         exit(1);
      }
   }

   // Initialize ActiveMQ
   init_messaging();

   // Init Xerces
   init_XML();

   // Read input file
   std::ifstream in( input_file.c_str() );
   string s;
   int counter = 1;
   if ( in )
   {
      // Get tab-delimited lines (utterance_id, utterance_text)
      while ( getline( in, s ) )
      {
         //printf( "u_id: '%s'\n", s.c_str() );
         s = remove_double_quotes( s );

         // Split up line in key and text 
         vector< string > main_tokens;
         const string tab_delimiter = "\t";
         vhcl::Tokenize( s, main_tokens, tab_delimiter );

         // catch the case where there's no text after the id
         string text = "";
         if ( main_tokens.size() > 1 )
         {
            text = main_tokens[ 1 ];
         }

         // Split up text in words
         vector< string > word_tokens;
         const string space_delimiter = " ";
         vhcl::Tokenize( text, word_tokens, space_delimiter );

         // Create Utterance struct
         struct Utterance ut;
         ut.utterance_id = main_tokens[ 0 ];
         ut.speech_id = "sp" + int_to_string( counter );
         ut.text = text;
         ut.addressee = "all";
         ut.speaker = "uknown";
         ut.words = word_tokens;

         // Add struct to map
         m_utterances[ ut.utterance_id ] = ut;

         counter++;
      }



      // TTS interaction
      string xml_request = "";
      // For each utterance
      for ( utteranceType::iterator it = m_utterances.begin(); it != m_utterances.end(); ++it )
      {
         // no TTS interaction required
         if ( it->second.text == "" )
         {
            continue;
         }

         printf( "Key (ID): '%s'\n", it->first.c_str() );
         printf( "Text '%s'\n\n", it->second.text.c_str() );

         // Create xml request to send to TTS
         xml_request = create_tts_request( it->second.utterance_id, voice );

         // Send request to TTS
         vhmsg::ttu_notify2( m_request_message, xml_request.c_str() );

         // Wait for response
         m_reply_received = false;
         _beginthread( vhmsg_loop, 0, NULL );
         int timer = 0;
         while ( !m_reply_received )
         {
            Sleep( 100 );
            timer += 100;
            if ( timer > 30000000 )
            {
               it->second.markers.clear();

               error_utterances.emplace_back( it->second.utterance_id );
               m_reply_received = true;  // force thread to exit
               Sleep( 1000 );    // give thread time to exit
            }
         }
         Sleep( 100 );  // give thread time to exit
      }


      // LTF processing
      if ( ltf_dir != "" )
      {
         for ( utteranceType::iterator it = m_utterances.begin(); it != m_utterances.end(); ++it )
         {
            string utterance_id = it->first;
            string ltf_file_name = ltf_dir + "\\" + utterance_id + ".ltf";

            parse_ltf_file( ltf_file_name, it->second );
         }
      }


      // generate bml and write out the file
      for ( utteranceType::iterator it = m_utterances.begin(); it != m_utterances.end(); ++it )
      {
         // Write to file
         write_string_to_file( it->first, create_bml( it->second ) );
      }

   }
   else
   {
      printf( "\nFile '%s' could not be opened. Please check if file exists. \n\nPress any key to exit.\n", input_file.c_str() );
      _getch();
      exit(1);
   }

   for ( unsigned int i = 0; i < error_utterances.size(); i++ )
   {
      printf( "\n\n\nSkipped utterances:\n" );
      printf( "- %s\n", error_utterances.at( i ).c_str() ); 
   }

   printf(" \nPress any key to exit.\n ");
   _getch();
   vhmsg::ttu_close();

   return 0;
}
