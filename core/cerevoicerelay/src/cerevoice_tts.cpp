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
      Arno Hartholt, USC
	  Abhinav Golas, USC
*/

#include "vhcl.h"
#include "cerevoice_tts.h"

#include <map>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <conio.h>
#include <io.h>
#include <direct.h>

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

/// New Modifications to better use XML structure of input message

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <vector>

/// Structure to store XML speech request message metadata
struct SpeechRequestMessageData {
	/// List of timing tags, and the corresponding words in the XML message
	/// eg. T0, T1, ... and hello, captain respectively
	/// words don't have any specific usage yet, but may have in the future, if tags are trimmed from their current protocol of 2 per word
	std::vector<std::string> tags, words;
	/// Speech ID (usually sp1)
	std::string speechIdentifier;
};

/// End new modifications
/*
extern "C"
{
   #include "cerenorm_wrap.h"
   #include "cerevoice.h"
   #include "cerevoice_io.h"
}

#include "cerevoice_pmod.h"
*/

#include "cerevoice_eng.h"
#include "cerevoice_eng_int.h"

#define MAXLINESZ 4095
#define MAXSPURTSZ 32000

XERCES_CPP_NAMESPACE_USE

// license and voice file 
char * lic_text;
char * signature;
CPRC_voice * voice;
std::map<std::string, CPRC_voice *> voices;
std::map<std::string, std::string> phonemeToViseme;
static char * EMPTY_STRING = "";

//normalization file names
char * abbfile;
char * pbreakfile;
char * homographfile;
char * reductionfile;
char * rulesfile;

/// Cleans up spurious whitespaces in string, and removes weird \n's
void cleanString(std::string &message)
{
	/// Remove newlines and carriage-returns
	message.erase( std::remove(message.begin(), message.end(), '\r'), message.end() );
	message.erase( std::remove(message.begin(), message.end(), '\n'), message.end() );
	/// If it's a space string, we want to leave the last whitespace
	while ( message.find_last_of(" ") == message.length() - 1  && message.length() > 1)
	{
		fprintf(stderr,"Debug: Reducing length by 1 to remove whitespace at end\n");
		message.resize( message.length() - 1 );
	}

	unsigned int pos;
	while ( (pos = message.find("  ")) != std::string::npos )
	{
		fprintf(stderr,"Debug: replacing 2 whitespaces at %d(%s) with 1 whitespace\n",pos, message.substr(pos,2).c_str());
		message.replace( pos, 2, " " );
	}

	while ( message.find_first_of(" ") == 0 )
	{
		//quick fix but there seems to be some problem with fprintf
		//fprintf(stderr,"Debug: Cleaning initial whitespace %s\n",message[0]);
		message = message.substr(1);
	}
}

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

//  removes XML tags and new lines from a string - used to take out
//  time markings before processing text, otherwise
//  silence is put in place of them
//  the opening speech tag will remain, and a new line must be put at the end
std::string removeXMLTagsAndNewLines( const std::string & txt , SpeechRequestMessageData & xmlMetaData)
{
//#define _PARSER_DEBUG_MESSAGES_ON
   std::stringstream txtstream;

   /// Put in some defaults, and do basic cleanup just to be safe
   xmlMetaData.speechIdentifier = "sp1";
   xmlMetaData.tags.clear();
   xmlMetaData.words.clear();
   /// Start an XML parser to parse the message we have received

   XMLPlatformUtils::Initialize();
   XercesDOMParser *parser = new XercesDOMParser();

   std::string truncatedTxt = txt.substr(txt.find_first_of(">")+1);
   

   //******************************************************************************************
// Code to properly format the message with indentation as otherwise Xerces throws a tantrum

   char * transferTxt = new char[truncatedTxt.length() * 2];

   unsigned int position = truncatedTxt.find("><");
   unsigned int i = 0;
   unsigned int j = 0;
   while(position != std::string::npos)
   {
	   transferTxt[i] = truncatedTxt[j];
	   ++i;
	   ++j;
	   if( j == position)
	   {
		   transferTxt[i] = '>'; ++i;
		   transferTxt[i] = '\n'; ++i;
		   transferTxt[i] = '<'; ++i;

		   position = truncatedTxt.find("><",position+1);

		   j= j+2;
	   }	   
   }

   while(j < truncatedTxt.length())
   {
	   transferTxt[i] = truncatedTxt[j];
	   ++j; 
	   ++i;
   }

   transferTxt[i] = '\0';

   //******************************************************************************************


   char * message = transferTxt;



   std::string actualText = "";
#ifdef _PARSER_DEBUG_MESSAGES_ON
   fprintf(stderr, "Debug: Parsing message \"%s\"\n",message);
#endif
   /// Set up a parser for XML message in memory - code sourced from unknown online reference for Xerces XML library
   MemBufInputSource memIS((const XMLByte*)message, strlen(message), "XMLBuffer");
   parser->parse(memIS);
   DOMDocument *doc = parser->getDocument();
   if ( doc )
   {
	   DOMElement *root = doc->getDocumentElement();

	   if ( root ) 
	   {
		   /// Get all nodes which have the "mark" tag, from these we can extract the timing tags, and speech text
		   DOMNodeList *messageList = root->getElementsByTagName(XMLString::transcode("speech"));
		   if ( messageList && messageList->getLength() > 0)
		   {
			   DOMElement *speechElement = dynamic_cast<DOMElement*>(messageList->item(0));
			   char *speechID = XMLString::transcode(speechElement->getAttribute(XMLString::transcode("id")));
			   xmlMetaData.speechIdentifier = std::string(speechID);
			   actualText = "<speech id=\"" + std::string(speechID) + "\" ref=\"" + XMLString::transcode(speechElement->getAttribute(X("ref"))) + "\" type=\"" + XMLString::transcode(speechElement->getAttribute(X("type"))) + "\">";
			   XMLString::release(&speechID);
		   }
		   else if ( !strcmp( XMLString::transcode( root->getNodeName() ), "speech") ) {
			   /// Else, the message might contain only the speech tag, in which case the above code will fail, so we need a fallback
			   DOMElement *speechElement = root;
			   char *speechID = XMLString::transcode(speechElement->getAttribute(XMLString::transcode("id")));
			   xmlMetaData.speechIdentifier = std::string(speechID);
			   actualText = "<speech id=\"" + std::string(speechID) + "\" ref=\"" + XMLString::transcode(speechElement->getAttribute(X("ref"))) + "\" type=\"" + XMLString::transcode(speechElement->getAttribute(X("type"))) + "\">";
			   XMLString::release(&speechID);
		   }
		   else
		   {
			   /// Oops, for some reason all of the above didn't work, default to the default speech id
			   fprintf(stderr, "Warning: Could not find speech tag in message, creating message beginning by default\n");
			   actualText = truncatedTxt.substr(0, truncatedTxt.find_first_of(">") + 1);
		   }
		   messageList = root->getElementsByTagName( X("mark"));

		   /// Store all the book marks in the input message, so that they can be retrieved later
		   if ( messageList ) 
		   {
			   XMLSize_t nLetters = messageList->getLength();

			   for ( XMLSize_t i = 0; i < nLetters; i++ )
			   {
				   DOMNode* node = messageList->item(i);
				   /// We only want to parse XML elements
				   if (node->getNodeType() && node->getNodeType() == DOMNode::ELEMENT_NODE)
				   {
					   DOMElement *element = dynamic_cast<DOMElement*>( node );
					   XMLCh *mark = (XMLCh*)element->getAttribute(XMLString::transcode("name"));
					   XMLCh *speech = NULL;
					   DOMNode *speechNode = element->getFirstChild();
					   if ( speechNode == NULL ) 
					   {
						   speechNode = element->getNextSibling();
					   }
					   if ( speechNode->getNodeType() == DOMNode::TEXT_NODE )
					   {
#ifdef _PARSER_DEBUG_MESSAGES_ON
						   fprintf(stderr, "Voila! found the text\n");
#endif
						   speech = (XMLCh*)speechNode->getNodeValue();
					   }
					   /// Get the timing tag as a string
					   char * t1, *t2;
					   std::string markString(t1 = XMLString::transcode(mark));
					   std::string speechString(t2 = (speech)?XMLString::transcode(speech): " ");
					   XMLString::release(&t1);
                       if(speech)
					    XMLString::release(&t2);
					   /// This code is still not watertight with regards to memory, needs some knowledge of Xerces memory management
					   //if ( mark ) XMLString::release(&mark);
					   //if ( speech ) XMLString::release(&speech);
					   if( markString == "" || speechString == "" )
					   {
#ifdef _PARSER_DEBUG_MESSAGES_ON
						   fprintf(stderr, "The mark or speech strings are null, mark: 0x%x, speech: 0x%x \n", markString, speechString);
#endif
						   /// Null strings tend to cause problems with later code sections
						   if ( speechString == "" ) speechString = " ";
					   }
					   else
					   {
						   std::string temporaryText = speechString;
						   cleanString(temporaryText);
#ifdef _PARSER_DEBUG_MESSAGES_ON
						   //fprintf(stderr, "Got timing tag: \"%s\" and text: \"%s\"\n", markString.c_str(), temporaryText.c_str());
#endif
						   /// Words can be used to match up which tags we need to see
							/// Push tag and word into xmlMetaData
							xmlMetaData.tags.emplace_back(markString);
							xmlMetaData.words.emplace_back(temporaryText);

							actualText += temporaryText;
					   }
					   /// As said before, needs proper memory management for Xerces
					  // if(mark)
							//XMLString::release(&mark);
					  // if(speech)
					  // {
							//XMLString::release(&speech);
							//XMLString::release(&speechString);
					   //}
					   //if ( markString )
						  // XMLString::release(&markString);
					   //if ( !_stricmp(" ", speechString) )
						  // delete speechString;
					   //else XMLString::release(&speechString);
					  // if(speechString)
							//XMLString::release(&markString);
				   }
			   }
		   }
		   else 
		   {
			   fprintf(stderr, "Error: Got no nodes with specified tag.\n");
		   }

	   }
	   else
	   {
		   fprintf(stderr, "Error: Could not extract root element from XML DOM document\n");
	   }
   }
   else
   {
	   fprintf(stderr, "Error: XML DOM document is null!\n");
   }
   doc->release();

   actualText += "</speech>\n";
   if ( actualText != "" ) {
#ifdef _PARSER_DEBUG_MESSAGES_ON
#undef _PARSER_DEBUG_MESSAGES_ON
	   fprintf(stderr,"Got text: \"%s\"\n",actualText.c_str());
#endif
	   return actualText;
   }
   else {
	   fprintf(stderr, "Error: Unable to instantiate DOM Xml parser, reverting to previous text-only parser\n");

	   /// Old non-XML parser based cleanup code, will not store any tags etc. Intended as a fallback in case something goes wrong with the parser above, which should not happen
	   //for the entire input string
	   for ( unsigned int i = 0; i < txt.length(); i++ )
	   {
		  //if the character is an opening bracket
		  if ( txt.at( i ) == '<' )
		  {
			 //loop until hitting the ending bracket
			 //so that this text is not input into
			 //the stringstream
			 while ( i < txt.length() && txt.at( i ) != '>' )
			 {
				i++;
			 }

			 //to skip over the ending bracket
			 i++;
		  }

		  //add the character to the stringstream
		  if ( i < txt.length() && txt.at( i ) != '\n' )
			 txtstream << txt.at( i );
	   }

	   txtstream << "\n";
   }
#ifdef _PARSER_DEBUG_MESSAGES_ON
#undef _PARSER_DEBUG_MESSAGES_ON
#endif
   fprintf(stderr,"Got text: \"%s\"\n",txtstream.str().c_str());
   return txtstream.str() + "</speech>";
}

//Aded by Apar Suri
//Added to remove the , and . from the code so that the issue with the time is resolved. 
//Apparently cerevoice API does not handle . and , properly in a sentence
std::string replacePausePunctuationsFromText(std::string text_string) {
	  std::string::size_type pos = 0;

	  for (pos = 0; pos < text_string.length(); ++pos)
	  {
		  if (text_string.at(pos)==',' || text_string.at(pos)=='.' || text_string.at(pos)=='?' || text_string.at(pos)=='!')
		  {
			  text_string.replace(pos, 1, "");
		  }
	  }
	  
	 /* while ( (pos = text_string.find(",", pos)) !=std:: string::npos ) {
		  text_string.replace( pos, 1, "");
        pos++;
	  }

	  pos = 0;
	  while ( (pos = text_string.find(".", pos)) !=std:: string::npos ) {
		  text_string.replace( pos, 1, "");
        pos++;
	  }

	  pos = 0;
	  while ( (pos = text_string.find("?", pos)) !=std:: string::npos ) {
		  text_string.replace( pos, 1, "");
        pos++;
	  }

	  pos = 0;
	  while ( (pos = text_string.find("!", pos)) !=std:: string::npos ) {
		  text_string.replace( pos, 1, "");
        pos++;
	  }*/

	  return text_string;
}


//returns true if file exists, else false
bool fileExists( const char * fileName )
{
   return _access( fileName, 0 ) == 0;
}


static void _make_xml( char * spurtxml, char * inputline, char breaktype )
{
   char xmlhead[] = "<?xml version='1.0'?>\n<!DOCTYPE spurt SYSTEM \"spurt.dtd\">\n<spurt spurt_id=\"test\" speaker_id=\"unknown\"><pause><break time=\"0.1\" type=\"#\"/></pause><speech>\n";
   char xmlfoot[] = "</speech><pause><break time=\"0.1\" type=\"#\"/></pause></spurt>\n";
   char * p;

   // replace break type
   p = xmlhead;
   while ( *p )
   {
      if ( *p == '#' )
      {
         *p = breaktype;
      }

      p++;
   }

   p = xmlfoot;

   while ( *p )
   {
      if ( *p == '#' )
      {
         *p = breaktype;
      }

      p++;
   }

   spurtxml[ 0 ] = '\0';
   strcat( spurtxml, xmlhead );
   strcat( spurtxml, inputline );
   strcat( spurtxml, xmlfoot );
}

/// Phoneme to viseme mapping - Maps phonemes Cerevoice returns to visemes SmartBody can show
void set_phonemes_to_visemes()
{
   phonemeToViseme[ "sil" ] = "_";  // SIL
   phonemeToViseme[ "aa" ]  = "Ao"; // AA
   phonemeToViseme[ "ae" ]  = "Ih"; // AE
   phonemeToViseme[ "ah" ]  = "Ih"; // AH
   phonemeToViseme[ "ao" ]  = "Ao"; // AO
   phonemeToViseme[ "ax" ]  = "Ih"; // AX
   phonemeToViseme[ "@" ]  = "Ih"; // Shouldn't happen!
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
   //possibly need phonemeToViseme entries for "i" and "j"?
}


void cerevoice_tts::init( std::vector<char *> vctVoices )
{
   // Settings definitions
   voice_path = "../../data/cereproc/voices/";
   voice_file_extension = ".voice";
   license_file_extension = ".lic";

   voiceEngine = CPRCEN_engine_new();
   // the below 2 paths are being set just as default values
   // the actual paths are obtained from SBM through the RemoteSpeechCmd and set at run time
   temp_audio_dir_cereproc = "../../data/cache/audio/";
   temp_audio_dir_player = "data/cache/audio/";

   abbfile       = "../../lib/cerevoice/veng_db/en/norm/abb.txt";
   pbreakfile    = "../../lib/cerevoice/veng_db/en/norm/pbreak.txt";
   homographfile = "../../lib/cerevoice/veng_db/en/homographs/rules.dat";
   reductionfile = "../../lib/cerevoice/veng_db/en/reduction/rules.dat";
   rulesfile     = "../../lib/cerevoice/veng_db/en/gb/norm/rules.py";

   for ( unsigned int i = 0; i < vctVoices.size(); i++ )
   {
      load_voice( vctVoices[ i ] );
   }

   set_phonemes_to_visemes();
   /*
   if ( !fileExists( abbfile ) )
   {
      std::cout<<"Error: normalization file abb.txt not found! See README files for setup information. Press any key to exit.\n";
      _getch();
      exit(1);
   }

   if ( !fileExists( pbreakfile ) )
   {
      std::cout<<"Error: normalization file pbreak.txt not found! See README files for setup information. Press any key to exit.\n";
      _getch();
      exit(1);
   }

   if ( !fileExists( homographfile ) )
   {
      std::cout<<"Error: normalization file homographsrules.dat not found! See README files for setup information. Press any key to exit.\n";
      _getch();
      exit(1);
   }

   if ( !fileExists( reductionfile ) )
   {
      std::cout<<"Error: normalization file reductionrules.dat not found! See README files for setup information. Press any key to exit.\n";
      _getch();
      exit(1);
   }

   if ( !fileExists( rulesfile ) )
   {
      std::cout<<"Error: normalization file normrules.py not found! See README files for setup information. Press any key to exit.\n";
      _getch();
      exit(1);
   }
   */

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
}


static int _read_license( char * licfile, char ** text, char ** signature )
{
   int text_sz = 0;
   int sig_sz = 0;
   int last_line_sz = 0;
   int line_sz = 0;

   FILE * licfp = fopen( licfile, "r" );
   int c = fgetc( licfp );

   while ( !feof( licfp ) )
   {
      line_sz++;

      if ( c == '\n' )
      {
         last_line_sz = line_sz;
         line_sz = 0;
      }

      text_sz++;
      c = fgetc( licfp );
   }

   rewind( licfp );

   if ( !line_sz )
      text_sz = text_sz - last_line_sz;
   else
      text_sz = text_sz - line_sz;

   if ( !line_sz )
      sig_sz = last_line_sz - 1;
   else
      sig_sz = line_sz;

   *text = (char *)malloc( text_sz + 1 );
   fread( *text, sizeof(char), text_sz, licfp );
   (*text)[ text_sz ] = '\0';

   *signature = (char *)malloc( sig_sz + 1 );
   fread( *signature, sizeof(char), sig_sz, licfp );
   (*signature)[sig_sz] = '\0';
   fclose( licfp );

   return 1;
}


void cerevoice_tts::load_voice( char * voice_id ) 
{

	std::string testVoiceID = "cerevoice_katherine_3.0.8_22k";
    std::string testLicenseFile = "katherine2";
   char * lic_text = EMPTY_STRING;
   char * signature = EMPTY_STRING;
   char lic_file[256] = "";
   char voice_file[256] = "";

   // Create full license file name
   strcat( lic_file, voice_path );
   strcat( lic_file, voice_id );
   //strcat( lic_file, testLicenseFile.c_str());
   strcat( lic_file, license_file_extension );

   // Make sure license file exists
   if ( !fileExists( lic_file ) )
   {
      std::cout<<"Error: license file not found! See README files for setup information. Press any key to exit.\n";
      _getch();
      exit(1);
   }
   //_read_license( lic_file, &lic_text, &signature );

   // Create full voice file name   
   strcat( voice_file, voice_path );
   strcat( voice_file, voice_id );
   //strcat( voice_file, testVoiceID.c_str());
   strcat( voice_file, voice_file_extension );
   
   // Make sure voice file exists
   if( !fileExists( voice_file ) )
   {
      std::cout<<"Error: voice file not found! See README files for setup information. Press any key to exit.\n";
      _getch();
      exit(1);
   }

   // Load voice and add to map, with the voice_id being the key
   int success = CPRCEN_engine_load_voice(voiceEngine, lic_file, NULL, voice_file,CPRC_VOICE_LOAD_EMB_AUDIO);

   //std::string result = tts("hello how are you","test.wav","text.xml",voice_id);
   if (!success)
   {
	   //printf("voiceFile = %s, licFile = %s")
	   std::cout<<"Error: voice file loaded fail. Press any key to exit.\n";
	   _getch();
	   exit(1);
   }
   //voices[ voice_id ] = CPRCEN_engine_load_voice( voiceEngine, voice_file, lic_text, static_cast<int>( strlen( lic_text ) ), signature, static_cast<int>( strlen( signature ) ) );
   //voices[ voice_id ] = CPRC_load_voicef( lic_file, voice_file );
 
   // Free license strings (aalocated in read_license)
   //free( lic_text );
   //free( signature );
}

std::string cerevoice_tts::addUselTag(std::string text)
{
	std::string tempText = text;
	unsigned int indexOfStartTag = text.find_first_of(">");
	if (indexOfStartTag > 0 && indexOfStartTag < text.size())
	{
		text.insert(indexOfStartTag + 1, "<usel genre=\'spon\'>");
		unsigned int indexOfEndTag = text.find_last_of("<");
		if (indexOfEndTag > 0 && indexOfEndTag < text.size())
		{
			text.insert(indexOfEndTag, "</usel>");
			return text;
		}
		/*std::string tempText = text.substr(0, indexOfStartTag + 1);
		std::string finalText = tempText.append("");
		if (indexOfStartTag + 2 < text.size()) {
			tempText = tempText.substr(indexOfStartTag + 2);
			tempText.insert(
			finalText = finalText.append(tempText);
		}*/
	}
	return tempText;
}

std::string CreateMarkTimeStamps(std::string text)
{
	std::string tempText = text;
	std::string markUp = "";
	int i = 0;
	while (tempText!= "")
	{
		std::string temp = tempText;
		temp = temp.substr(0, tempText.find_first_of(" "));

		char number[256];
		sprintf(number, "%d", i);
		markUp = markUp.append("<mark name=\"T");
		markUp = markUp.append(number);
		markUp = markUp.append("\" />");
		markUp = markUp.append(temp + "\n\n");
		sprintf(number, "%d", ++i);
		markUp = markUp.append("<mark name=\"T");
		markUp = markUp.append(number);
		markUp = markUp.append("\" />\t\n\n");
		++i;
		if (tempText.size() > temp.size())
		{
			tempText = tempText.substr(temp.size() + 1);
		}
		else
		{
			tempText = "";
		}
	}
	return markUp;
}

std::string TransformTextWithTimes(std::string txt)
{
	//std::string text_string = "";

	//#define _PARSER_DEBUG_MESSAGES_ON
   std::stringstream txtstream;

   /// Start an XML parser to parse the message we have received
   XMLPlatformUtils::Initialize();
   XercesDOMParser *parser = new XercesDOMParser();

   std::string truncatedTxt = txt.substr(txt.find_first_of(">")+1);
   char * message = (char*)truncatedTxt.c_str();

   std::string actualText = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
#ifdef _PARSER_DEBUG_MESSAGES_ON
   fprintf(stderr, "Debug: Parsing message \"%s\"\n",message);
#endif
   /// Set up a parser for XML message in memory - code sourced from unknown online reference for Xerces XML library
   MemBufInputSource memIS((const XMLByte*)message, strlen(message), "XMLBuffer");
   parser->parse(memIS);
   DOMDocument *doc = parser->getDocument();
   if ( doc )
   {
	   DOMElement *root = doc->getDocumentElement();

	   if ( root ) 
	   {
		   /// Get all nodes which have the "mark" tag, from these we can extract the timing tags, and speech text
		   DOMNodeList *messageList = root->getElementsByTagName(XMLString::transcode("speech"));
		   if ( messageList && messageList->getLength() > 0)
		   {
			   DOMElement *speechElement = dynamic_cast<DOMElement*>(messageList->item(0));
			   char *speechID = XMLString::transcode(speechElement->getAttribute(XMLString::transcode("id")));

			   actualText = actualText.append("<speech id=\"" + std::string(speechID) + "\" ref=\"" + XMLString::transcode(speechElement->getAttribute(X("ref"))) + "\" type=\"" + XMLString::transcode(speechElement->getAttribute(X("type"))) + "\">\n\n");
			   XMLString::release(&speechID);
		   }
		   else if ( !strcmp( XMLString::transcode( root->getNodeName() ), "speech") ) {
			   /// Else, the message might contain only the speech tag, in which case the above code will fail, so we need a fallback
			   DOMElement *speechElement = root;
			   char *speechID = XMLString::transcode(speechElement->getAttribute(XMLString::transcode("id")));
			   char *text = XMLString::transcode(speechElement->getTextContent());

			   std::string textContent = CreateMarkTimeStamps(text);
			   /*if (!speechID)
			   {
				   speechID = "sp1";
			   }*/
			   //hard coding sp1
			   speechID = "sp1";
			   //XMLString::transcode(speechElement->getAttribute(X("type"))) 
			   actualText = actualText.append("<speech id=\"" + std::string(speechID) + "\" ref=\"" + XMLString::transcode(speechElement->getAttribute(X("ref"))) + "\" type=\"" + "application/ssml+xml" + "\">\n\n");
			   actualText = actualText.append(textContent);
			   actualText = actualText.append("</speech>");
			   //XMLString::release(&speechID);
		   }
	   }
   }
   return actualText;
}

////Aded by Apar Suri
////Added to remove the , and . from the code so that the issue with the time is resolved. 
////Apparently cerevoice API does not handle . and , properly in a sentence
//std::string replacePausePunctuationsFromText(std::string text_string) {
//	  std::string::size_type pos = 0;
//
//	  for (pos = 0; pos < text_string.length(); ++pos)
//	  {
//		  if (text_string.at(pos)==',' || text_string.at(pos)=='.' || text_string.at(pos)=='?' || text_string.at(pos)=='!')
//		  {
//			  text_string.replace(pos, 1, "");
//		  }
//	  }
//	  
//	 /* while ( (pos = text_string.find(",", pos)) !=std:: string::npos ) {
//		  text_string.replace( pos, 1, "");
//        pos++;
//	  }
//
//	  pos = 0;
//	  while ( (pos = text_string.find(".", pos)) !=std:: string::npos ) {
//		  text_string.replace( pos, 1, "");
//        pos++;
//	  }
//
//	  pos = 0;
//	  while ( (pos = text_string.find("?", pos)) !=std:: string::npos ) {
//		  text_string.replace( pos, 1, "");
//        pos++;
//	  }
//
//	  pos = 0;
//	  while ( (pos = text_string.find("!", pos)) !=std:: string::npos ) {
//		  text_string.replace( pos, 1, "");
//        pos++;
//	  }*/
//
//	  return text_string;
//}

std::string cerevoice_tts::tts( const char * text, const char * cereproc_file_name, const char * player_file_name, std::string voice_id )
{
   char * result = "";

   printf( "cerevoice_tts::tts() - '%s' - '%s' - '%s' - '%s'\n", text, cereproc_file_name, player_file_name, voice_id.c_str() );

   /*
      Create a text normalisation parser
      The parser is configured with a file defining abbreviations to be
      expanded, and a file that defines the break times for particular pauses.
      Both files are easily configurable by the user.
      A homograph rules file is also loaded.

      The parser is cleared between input documents to allow parsing of
      multiple files.
   */

   // Create a reference to the normaliser, which needs to be passed in to all normaliser functions
   /*
   int norm_id = Normaliser_create( elNEWID );
   Normaliser_set_abbreviations( norm_id, abbfile );
   Normaliser_set_pbreaks( norm_id, pbreakfile );
   Normaliser_set_rules( norm_id, rulesfile );
   Normaliser_set_homographs( norm_id, homographfile );
   Normaliser_set_reductions( norm_id, reductionfile );
   */

   // file to record database coverage data
   //char * logfile = EMPTY_STRING;
   //char * covfile = EMPTY_STRING;
   //char * lexfile = EMPTY_STRING;

   // If voice doesn't exist, ignore; another voice relay might pick that up
//    if ( !voices[ voice_id ] )
//    {
//       fprintf( stderr, "ERROR: voice load failed, check voice file and license integrity.\n\nIgnoring voice '%s'.\n", voice_id );
//    }
//    else
   {    

	  
	   // Local variable to store message XML metadata
	  SpeechRequestMessageData xmlMetaData;
      // Feed in input text, further data is to come
	  //Added by Apar Suri
	  //Replacing . and , with "" so because there seems to be a bug in CPRCPMOD_spurt_synth while synthesizing multiple spurts
	  //Apparently a time of 0.0 and 0.1 seems to be added just after the , or .
	  std::string text_string = removeXMLTagsAndNewLines( text, xmlMetaData);

	  if (xmlMetaData.tags.size() <= 0 )
	  {
		  //text_string = text;
		  text_string =  TransformTextWithTimes(text);
		  text_string = removeXMLTagsAndNewLines(text_string, xmlMetaData);
	  }

	 /* if (xmlMetaData.tags.size() <= 0)
	  {
		  std::vector<std::string> words;
		  int numberOfWords = findNumberOfWords(text_string, words);
		  xmlMetaData.speechIdentifier = "sp1";
		  xmlMetaData.tags.clear();
		  xmlMetaData.words.clear();
		  
		  for (int wordTimingIndex = 0; wordTimingIndex < numberOfWords * 2; ++wordTimingIndex)
		  {
			  std::string timeStamp = "T" + wordTimingIndex;
			  xmlMetaData.tags.emplace_back(timeStamp);
			  xmlMetaData.words.emplace_back(words.at(wordTimingIndex));
		  }
	  }*/
	  //commenting replacing pause punctuations
	  //text_string = replacePausePunctuationsFromText(text_string);

	  //Checking to see if it is conv voice, then add the tag "<usel genre='spon'>"
	  if (voice_id.compare("starconv") == 0)
	  {
		  text_string = addUselTag(text_string);
	  }

	  CPRCEN_channel_handle chan = CPRCEN_engine_open_channel(voiceEngine, "", "", voice_id.c_str(), "");
	  char* fileName = const_cast<char*>(cereproc_file_name);
	  CPRCEN_engine_channel_to_file(voiceEngine, chan, fileName, CPRCEN_RIFF); /* File output on channel */
	  // 	/* Speak with streaming input */
	  CPRC_abuf* abuf = CPRCEN_engine_channel_speak(voiceEngine, chan, text, strlen(text),1);



      /* Reset the parser.  This has to be done between input documents
      or the xml will be invalid and the parse will fail.
      */
      //Normaliser_reset_parser( norm_id );

      /* synthesise */

      /* Reused: spurt, abuf, ltssrch
      New each synthesis: lxsrch
      Pitch modification is performed, use CPRC_spurt_synth
      to avoid using the pitch modification library
      */

      // make the output file name from the input file less the extension, and the output dir
      //CPRC_riff_save( abuf, cereproc_file_name );

      // Watch for special case help request
      int errorCode = 0;


      DOMImplementation * impl =  DOMImplementationRegistry::getDOMImplementation( X( "Core" ) );

      if ( impl != NULL )
      {
         try
         {
            //XMLCh * end = XMLString::transcode( "end" );
            //XMLCh * start = XMLString::transcode( "start" );
            XMLCh * name = XMLString::transcode( "name" );
            XMLCh * time = XMLString::transcode( "time" );
           // XMLCh * id = XMLString::transcode( "id" );
            XMLCh * file_path = XMLString::transcode( player_file_name );

            DOMDocument* doc = impl->createDocument(
               0,                    // root element namespace URI.
               X( "speak" ),         // root element name
               0 );                  // document type object (DTD).

            DOMElement * rootElem = doc->getDocumentElement();

            DOMElement * soundFileElement = doc->createElement( X( "soundFile" ) );
            soundFileElement->setAttribute( name, file_path );
            rootElem->appendChild( soundFileElement );

            DOMElement * wordElement = doc->createElement( X( "word" ) );

            int num_words = 0;

            for ( int i = 0; i < abuf->trans_sz; i++ )
            {
               if ( abuf->trans[ i ].type == CPRC_ABUF_TRANS_PHONE )
               {
                 std::map<std::string, std::string>::iterator iter = phonemeToViseme.find( abuf->trans[ i ].name );

                 //check added to avoid crashing on entries which are not defined
                 if ( iter != phonemeToViseme.end() )
                 {

                    XMLCh * start = XMLString::transcode( "start" );
                    XMLCh * end = XMLString::transcode( "end" );
                    XMLCh * type = XMLString::transcode( "type" );
                    XMLCh * phone_type = XMLString::transcode( iter->second.c_str() );

                    std::string end_f = vhcl::Format( "%0.6f", abuf->trans[i].end );
                    XMLCh * end_time = XMLString::transcode( end_f.c_str() );

                    std::string start_f = vhcl::Format( "%0.6f", abuf->trans[i].start );
                    XMLCh * start_time = XMLString::transcode( start_f.c_str() );

                    DOMElement * visemeElement = doc->createElement( X( "viseme" ) );
                    visemeElement->setAttribute( start, start_time );
                    visemeElement->setAttribute( type, phone_type );

                    if ( strcmp( abuf->trans[ i ].name, "sil" ) == 0 )
                    {
                      rootElem->appendChild( visemeElement );
                    }
                    else
                    {
                      wordElement->appendChild( visemeElement );
                    }

                    if ( i < ( abuf->trans_sz - 1 ) )
                    {
                        if ( ( abuf->trans[ i + 1 ].type == CPRC_ABUF_TRANS_WORD ) || ( abuf->trans[ i + 1 ].type == CPRC_ABUF_TRANS_MARK ) //) {
                           || ( strcmp( abuf->trans[ i + 1 ].name, "sil" ) == 0 ) )
                        {
                           wordElement->setAttribute( end, end_time );

                           if ( wordElement->hasChildNodes() )
                              rootElem->appendChild( wordElement );
   
                           wordElement = doc->createElement( X( "word" ) );

                           std::string word_start_f = vhcl::Format( "%0.6f", abuf->trans[ i + 1 ].start );
                           XMLCh * word_start_time = XMLString::transcode( word_start_f.c_str() );
                           wordElement->setAttribute( start, word_start_time );

                           // Add marker information back
                           // Should come from initial XML message, 
                           // but for now is constructed following the known NVBG protocol (markers around words)
						   // HACK AVOIDED: Fixed to use XML metadata as extracted during parse
                           std::ostringstream ostr;
						   //changed by Apar Suri so that it does not coincide with the outer loop i
                           int double_words = num_words * 2;
                           ostr << double_words;
						   std::string s = xmlMetaData.speechIdentifier + ":";
                           //std::string s = "sp1:T" + ostr.str();
						   //fprintf(stderr, "%d/%d ",num_words * 2, xmlMetaData.tags.size()-1);
						   //std::string s = xmlMetaData.speechIdentifier + ":" + xmlMetaData.tags[num_words * 2];
						   if ( num_words * 2 < (int)xmlMetaData.tags.size() ) {
							   /// If we have an existing tag for this number, use it
							   s += xmlMetaData.tags[num_words * 2];
						   }
						   else {
							   /// Extract alphabetical part of the tags, in the hope that we're following a coherent naming pattern, and only numbers are changed
							   std::string last_tag = xmlMetaData.tags[xmlMetaData.tags.size() - 1];
							   int j;
							   for ( j = 0; j < (int)last_tag.size(); j++ ) {
								   if ( !isalpha(last_tag[j]) ) {
									   break;
								   }
							   }
							   s += last_tag.substr(0, j) + ostr.str();
							   fprintf(stderr,"Warning: Reference to unspecified tag, constructing one on-the-fly to be: %s\n", s.c_str());
						   }

                           DOMElement * markElement = doc->createElement( X( "mark" ) );
                           XMLCh * mark_name = XMLString::transcode( s.c_str() );
                           markElement->setAttribute( name, mark_name ) ;
                           markElement->setAttribute( time, word_start_time );
                           rootElem->appendChild( markElement );

                           std::ostringstream ostr2;
                           double_words = num_words * 2 + 1;
                           ostr2 << double_words;
						   s = xmlMetaData.speechIdentifier + ":";
                           //s = "sp1:T" + ostr2.str();
						   //fprintf(stderr, "%d/%d ",num_words * 2+1, xmlMetaData.tags.size()-1);
						   //s = xmlMetaData.speechIdentifier + ":" + xmlMetaData.tags[num_words * 2 + 1];

						   /// Modifications to use XML tags saved at previous step - use stored tags, or generate new ones if unseen tag referenced (reason for unseen tags being referenced is still not known)
						   if ( num_words * 2 + 1 < (int)xmlMetaData.tags.size() ) {
							   /// If we have an existing tag for this number, use it
							   s += xmlMetaData.tags[num_words * 2 + 1];
						   }
						   else {
							   /// Extract alphabetical part of the tags, in the hope that we're following a coherent naming pattern, and only numbers are changed
							   std::string last_tag = xmlMetaData.tags[xmlMetaData.tags.size() - 1];
							   int j;
							   for ( j = 0; j < (int)last_tag.size(); j++ ) {
								   if ( !isalpha(last_tag[j]) ) {
									   break;
								   }
							   }
							   s += last_tag.substr(0, j) + ostr2.str();
							   fprintf(stderr,"Warning: Reference to unspecified tag, constructing one on-the-fly to be: %s\n", s.c_str());
						   }
                           DOMElement * markElement2 = doc->createElement( X( "mark" ) );
                           XMLCh * mark_name2 = XMLString::transcode( s.c_str() );
                           markElement2->setAttribute( name, mark_name2 ) ;
                           markElement2->setAttribute( time, end_time );
                           rootElem->appendChild( markElement2 );

                           //float word_start = abuf->trans[ i + 1 ].start;
                           num_words++;
                       }
                    }
                 }
                 else
                 {
                   printf( "COULDN'T FIND MATCH FOR: %s\n", abuf->trans[ i ].name );
                 }
               }
               /*else if  ( abuf->trans[ i ].type == CPRC_ABUF_TRANS_MARK )
               {
                    DOMElement * markElement = doc->createElement( X( "mark" ) );
                    XMLCh * mark_name = XMLString::transcode( abuf->trans[ i ].name );
                    markElement->setAttribute( name, mark_name ) ;

                    std::string word_start_f = vhcl::Format( "%0.6f", abuf->trans[ i ].start );
                    XMLCh * word_start_time = XMLString::transcode( word_start_f.c_str() );
                    markElement->setAttribute( time, word_start_time );

                    rootElem->appendChild( markElement );
               }*/
            }

            DOMLSSerializer* theSerializer = DOMImplementation::getImplementation()->createLSSerializer();
            XMLCh * xml_result = theSerializer->writeToString( rootElem );
            result = XMLString::transcode( xml_result );
            theSerializer->release();

			CPRCEN_engine_channel_close(voiceEngine,chan);
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

      //int speech_sz = 0;
      //speech_sz += abuf->wav_sz;
      //CPRC_lts_search_reset( ltssrch );
      //CPRC_spurt_clear( spurt );
      //int fno = 0;
      //fno += 1;
   }

   return std::string( result );
}
