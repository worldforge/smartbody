
#include "vhcl.h"
#include "cerevoice_tts.h"

#include <map>
#include <algorithm>
#include <sstream>

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

extern "C"
{
   #include "cerenorm_wrap.h"
   #include "cerevoice.h"
   #include "cerevoice_io.h"
}

#include "cerevoice_pmod.h"


XERCES_CPP_NAMESPACE_USE


using std::string;
using std::vector;


/// Structure to store XML speech request message metadata
struct SpeechRequestMessageData
{
   /// List of timing tags, and the corresponding words in the XML message
   /// eg. T0, T1, ... and hello, captain respectively
   std::vector<std::string> tags;

   /// Speech ID (usually sp1)
   std::string speechIdentifier;
};

#define MAXLINESZ 4095
#define MAXSPURTSZ 32000

// license and voice file
CPRC_voice * g_voice;
std::map<string, CPRC_voice *> g_voices;
std::map<string, string> g_phonemeToViseme;

//normalization file names
char * g_abbfile;
char * g_pbreakfile;
char * g_homographfile;
char * g_reductionfile;
char * g_rulesfile;

string g_visemeMapping;

extern vector<double> g_wordBreakListStart;
extern vector<double> g_wordBreakListEnd;
extern vector<string> g_markListName;
extern vector<double> g_markListTime;
extern vector<string> g_visemeListType;
extern vector<double> g_visemeListStart;
extern vector<double> g_visemeListArticulation;


const char * facefxMapping = {
   "<mapping>"
   "   <entry phoneme=\"@\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"sil\" target=\"open\" amount=\"0.000000\" />"
   "   <entry phoneme=\"t\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"d\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"k\" target=\"open\" amount=\"0.250000\" />"
   "   <entry phoneme=\"g\" target=\"open\" amount=\"0.250000\" />"
   "   <entry phoneme=\"n\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"ng\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"RA\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"RU\" target=\"open\" amount=\"0.250000\" />"
   "   <entry phoneme=\"FLAP\" target=\"open\" amount=\"0.300000\" />"
   "   <entry phoneme=\"PH\" target=\"open\" amount=\"0.100000\" />"
   "   <entry phoneme=\"th\" target=\"open\" amount=\"0.450000\" />"
   "   <entry phoneme=\"DH\" target=\"open\" amount=\"0.450000\" />"
   "   <entry phoneme=\"s\" target=\"open\" amount=\"0.150000\" />"
   "   <entry phoneme=\"z\" target=\"open\" amount=\"0.150000\" />"
   "   <entry phoneme=\"CX\" target=\"open\" amount=\"0.250000\" />"
   "   <entry phoneme=\"X\" target=\"open\" amount=\"0.250000\" />"
   "   <entry phoneme=\"GH\" target=\"open\" amount=\"0.250000\" />"
   "   <entry phoneme=\"hh\" target=\"open\" amount=\"0.300000\" />"
   "   <entry phoneme=\"r\" target=\"open\" amount=\"0.100000\" />"
   "   <entry phoneme=\"l\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"H\" target=\"open\" amount=\"0.200000\" />"
   "   <entry phoneme=\"TS\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"iy\" target=\"open\" amount=\"0.200000\" />"
   "   <entry phoneme=\"E\" target=\"open\" amount=\"0.350000\" />"
   "   <entry phoneme=\"EN\" target=\"open\" amount=\"0.350000\" />"
   "   <entry phoneme=\"eh\" target=\"open\" amount=\"0.500000\" />"
   "   <entry phoneme=\"A\" target=\"open\" amount=\"0.550000\" />"
   "   <entry phoneme=\"aa\" target=\"open\" amount=\"0.550000\" />"
   "   <entry phoneme=\"AAN\" target=\"open\" amount=\"0.550000\" />"
   "   <entry phoneme=\"ao\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"AON\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"O\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"ON\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"uw\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"EU\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"OE\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"OEN\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"ah\" target=\"open\" amount=\"0.500000\" />"
   "   <entry phoneme=\"ih\" target=\"open\" amount=\"0.500000\" />"
   "   <entry phoneme=\"UU\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"uh\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"ax\" target=\"open\" amount=\"0.500000\" />"
   "   <entry phoneme=\"UX\" target=\"open\" amount=\"0.500000\" />"
   "   <entry phoneme=\"ae\" target=\"open\" amount=\"0.500000\" />"
   "   <entry phoneme=\"er\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"AXR\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"EXR\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"ey\" target=\"open\" amount=\"0.500000\" />"
   "   <entry phoneme=\"aw\" target=\"open\" amount=\"0.500000\" />"
   "   <entry phoneme=\"ay\" target=\"open\" amount=\"0.500000\" />"
   "   <entry phoneme=\"oy\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"ow\" target=\"open\" amount=\"0.400000\" />"
   "   <entry phoneme=\"r\" target=\"W\" amount=\"0.700000\" />"
   "   <entry phoneme=\"y\" target=\"W\" amount=\"0.500000\" />"
   "   <entry phoneme=\"w\" target=\"W\" amount=\"0.850000\" />"
   "   <entry phoneme=\"ao\" target=\"W\" amount=\"0.550000\" />"
   "   <entry phoneme=\"AON\" target=\"W\" amount=\"0.550000\" />"
   "   <entry phoneme=\"O\" target=\"W\" amount=\"0.550000\" />"
   "   <entry phoneme=\"ON\" target=\"W\" amount=\"0.550000\" />"
   "   <entry phoneme=\"uw\" target=\"W\" amount=\"0.550000\" />"
   "   <entry phoneme=\"UY\" target=\"W\" amount=\"0.850000\" />"
   "   <entry phoneme=\"EU\" target=\"W\" amount=\"0.550000\" />"
   "   <entry phoneme=\"OE\" target=\"W\" amount=\"0.550000\" />"
   "   <entry phoneme=\"OEN\" target=\"W\" amount=\"0.550000\" />"
   "   <entry phoneme=\"UU\" target=\"W\" amount=\"0.550000\" />"
   "   <entry phoneme=\"uh\" target=\"W\" amount=\"0.550000\" />"
   "   <entry phoneme=\"oy\" target=\"W\" amount=\"0.550000\" />"
   "   <entry phoneme=\"ow\" target=\"W\" amount=\"0.550000\" />"
   "   <entry phoneme=\"sh\" target=\"ShCh\" amount=\"0.850000\" />"
   "   <entry phoneme=\"zh\" target=\"ShCh\" amount=\"0.850000\" />"
   "   <entry phoneme=\"y\" target=\"ShCh\" amount=\"0.300000\" />"
   "   <entry phoneme=\"ch\" target=\"ShCh\" amount=\"0.850000\" />"
   "   <entry phoneme=\"jh\" target=\"ShCh\" amount=\"0.850000\" />"
   "   <entry phoneme=\"er\" target=\"ShCh\" amount=\"0.500000\" />"
   "   <entry phoneme=\"AXR\" target=\"ShCh\" amount=\"0.500000\" />"
   "   <entry phoneme=\"EXR\" target=\"ShCh\" amount=\"0.500000\" />"
   "   <entry phoneme=\"p\" target=\"PBM\" amount=\"0.900000\" />"
   "   <entry phoneme=\"b\" target=\"PBM\" amount=\"0.900000\" />"
   "   <entry phoneme=\"m\" target=\"PBM\" amount=\"0.900000\" />"
   "   <entry phoneme=\"PH\" target=\"FV\" amount=\"0.400000\" />"
   "   <entry phoneme=\"f\" target=\"FV\" amount=\"0.750000\" />"
   "   <entry phoneme=\"v\" target=\"FV\" amount=\"0.750000\" />"
   "   <entry phoneme=\"s\" target=\"wide\" amount=\"0.500000\" />"
   "   <entry phoneme=\"z\" target=\"wide\" amount=\"0.500000\" />"
   "   <entry phoneme=\"iy\" target=\"wide\" amount=\"0.800000\" />"
   "   <entry phoneme=\"E\" target=\"wide\" amount=\"0.250000\" />"
   "   <entry phoneme=\"EN\" target=\"wide\" amount=\"0.250000\" />"
   "   <entry phoneme=\"eh\" target=\"wide\" amount=\"0.600000\" />"
   "   <entry phoneme=\"ah\" target=\"wide\" amount=\"0.600000\" />"
   "   <entry phoneme=\"ih\" target=\"wide\" amount=\"0.600000\" />"
   "   <entry phoneme=\"ax\" target=\"wide\" amount=\"0.600000\" />"
   "   <entry phoneme=\"UX\" target=\"wide\" amount=\"0.600000\" />"
   "   <entry phoneme=\"ae\" target=\"wide\" amount=\"0.600000\" />"
   "   <entry phoneme=\"ey\" target=\"wide\" amount=\"0.600000\" />"
   "   <entry phoneme=\"aw\" target=\"wide\" amount=\"0.600000\" />"
   "   <entry phoneme=\"ay\" target=\"wide\" amount=\"0.600000\" />"
   "   <entry phoneme=\"k\" target=\"tBack\" amount=\"0.800000\" />"
   "   <entry phoneme=\"g\" target=\"tBack\" amount=\"0.800000\" />"
   "   <entry phoneme=\"RU\" target=\"tBack\" amount=\"0.800000\" />"
   "   <entry phoneme=\"CX\" target=\"tBack\" amount=\"0.800000\" />"
   "   <entry phoneme=\"X\" target=\"tBack\" amount=\"0.800000\" />"
   "   <entry phoneme=\"GH\" target=\"tBack\" amount=\"0.800000\" />"
   "   <entry phoneme=\"eh\" target=\"tBack\" amount=\"0.400000\" />"
   "   <entry phoneme=\"ah\" target=\"tBack\" amount=\"0.400000\" />"
   "   <entry phoneme=\"ih\" target=\"tBack\" amount=\"0.400000\" />"
   "   <entry phoneme=\"ax\" target=\"tBack\" amount=\"0.400000\" />"
   "   <entry phoneme=\"UX\" target=\"tBack\" amount=\"0.400000\" />"
   "   <entry phoneme=\"ae\" target=\"tBack\" amount=\"0.400000\" />"
   "   <entry phoneme=\"ey\" target=\"tBack\" amount=\"0.400000\" />"
   "   <entry phoneme=\"aw\" target=\"tBack\" amount=\"0.400000\" />"
   "   <entry phoneme=\"ay\" target=\"tBack\" amount=\"0.400000\" />"
   "   <entry phoneme=\"t\" target=\"tRoof\" amount=\"0.800000\" />"
   "   <entry phoneme=\"d\" target=\"tRoof\" amount=\"0.800000\" />"
   "   <entry phoneme=\"n\" target=\"tRoof\" amount=\"0.800000\" />"
   "   <entry phoneme=\"ng\" target=\"tRoof\" amount=\"0.800000\" />"
   "   <entry phoneme=\"RA\" target=\"tRoof\" amount=\"0.500000\" />"
   "   <entry phoneme=\"FLAP\" target=\"tRoof\" amount=\"0.600000\" />"
   "   <entry phoneme=\"s\" target=\"tRoof\" amount=\"0.400000\" />"
   "   <entry phoneme=\"z\" target=\"tRoof\" amount=\"0.400000\" />"
   "   <entry phoneme=\"sh\" target=\"tRoof\" amount=\"0.400000\" />"
   "   <entry phoneme=\"zh\" target=\"tRoof\" amount=\"0.400000\" />"
   "   <entry phoneme=\"y\" target=\"tRoof\" amount=\"0.400000\" />"
   "   <entry phoneme=\"l\" target=\"tRoof\" amount=\"0.800000\" />"
   "   <entry phoneme=\"TS\" target=\"tRoof\" amount=\"0.800000\" />"
   "   <entry phoneme=\"ch\" target=\"tRoof\" amount=\"0.400000\" />"
   "   <entry phoneme=\"jh\" target=\"tRoof\" amount=\"0.400000\" />"
   "   <entry phoneme=\"iy\" target=\"tRoof\" amount=\"0.200000\" />"
   "   <entry phoneme=\"E\" target=\"tRoof\" amount=\"0.200000\" />"
   "   <entry phoneme=\"EN\" target=\"tRoof\" amount=\"0.200000\" />"
   "   <entry phoneme=\"er\" target=\"tRoof\" amount=\"0.500000\" />"
   "   <entry phoneme=\"AXR\" target=\"tRoof\" amount=\"0.500000\" />"
   "   <entry phoneme=\"EXR\" target=\"tRoof\" amount=\"0.500000\" />"
   "   <entry phoneme=\"th\" target=\"tTeeth\" amount=\"0.900000\" />"
   "   <entry phoneme=\"DH\" target=\"tTeeth\" amount=\"0.900000\" />"
   "</mapping>"
};


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


void CleanString(std::string &message)
{
   /// Cleans up spurious whitespaces in string, and removes weird \n's

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
   while ( (pos = message.find("  ")) != string::npos )
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


std::string RemoveXMLTagsAndNewLines( const std::string & txt , SpeechRequestMessageData & xmlMetaData)
{
   //  removes XML tags and new lines from a string - used to take out
   //  time markings before processing text, otherwise
   //  silence is put in place of them
   //  the opening speech tag will remain, and a new line must be put at the end

//#define _PARSER_DEBUG_MESSAGES_ON
   std::stringstream txtstream;

   /// Put in some defaults, and do basic cleanup just to be safe
   xmlMetaData.speechIdentifier = "sp1";
   xmlMetaData.tags.clear();

   /// Start an XML parser to parse the message we have received
   XMLPlatformUtils::Initialize();
   XercesDOMParser *parser = new XercesDOMParser();

   // remove the xml header
   string truncatedTxt = txt.substr(txt.find_first_of(">")+1);


   //******************************************************************************************
   // Code to properly format the message with indentation as otherwise Xerces throws a tantrum

   string transferTxt;

   unsigned int position = truncatedTxt.find("><");
   unsigned int j = 0;
   while (position != string::npos)
   {
      transferTxt.append(1, truncatedTxt[j]);
      ++j;

      if (j == position)
      {
         transferTxt.append(1, '>');
         transferTxt.append(1, '\n');
         transferTxt.append(1, '<');

         position = truncatedTxt.find("><", position + 1);
         j = j + 2;
      }
   }

   while (j < truncatedTxt.length())
   {
      transferTxt.append(1, truncatedTxt[j]);
      ++j;
   }

   //******************************************************************************************

   const char * message = transferTxt.c_str();


   string actualText = "";
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
               xmlMetaData.speechIdentifier = string(speechID);
               actualText = "<speech id=\"" + string(speechID) + "\" ref=\"" + XMLString::transcode(speechElement->getAttribute(X("ref"))) + "\" type=\"" + XMLString::transcode(speechElement->getAttribute(X("type"))) + "\">";
               XMLString::release(&speechID);
           }
           else if ( !strcmp( XMLString::transcode( root->getNodeName() ), "speech") ) {
               /// Else, the message might contain only the speech tag, in which case the above code will fail, so we need a fallback
               DOMElement *speechElement = root;
               char *speechID = XMLString::transcode(speechElement->getAttribute(XMLString::transcode("id")));
               xmlMetaData.speechIdentifier = string(speechID);
               actualText = "<speech id=\"" + string(speechID) + "\" ref=\"" + XMLString::transcode(speechElement->getAttribute(X("ref"))) + "\" type=\"" + XMLString::transcode(speechElement->getAttribute(X("type"))) + "\">";
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
                       string markString(t1 = XMLString::transcode(mark));
                       string speechString(t2 = (speech)?XMLString::transcode(speech): " ");
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
                           string temporaryText = speechString;
                           CleanString(temporaryText);
#ifdef _PARSER_DEBUG_MESSAGES_ON
                           //fprintf(stderr, "Got timing tag: \"%s\" and text: \"%s\"\n", markString.c_str(), temporaryText.c_str());
#endif
                            /// Push tag into xmlMetaData
                            xmlMetaData.tags.emplace_back(markString);
                            //xmlMetaData.words.emplace_back(temporaryText);

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
   else
   {
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

   printf("Got text: \"%s\"\n",txtstream.str().c_str());
   return txtstream.str() + "</speech>";
}


/*
std::string ReplacePausePunctuationsFromText(std::string text_string)
{
   //Aded by Apar Suri
   //Added to remove the , and . from the code so that the issue with the time is resolved.
   //Apparently cerevoice API does not handle . and , properly in a sentence

   for (size_t pos = 0; pos < text_string.length(); ++pos)
   {
      if (text_string.at(pos) == ',' ||
          text_string.at(pos) == '.' ||
          text_string.at(pos) == '?' ||
          text_string.at(pos) == '!')
      {
         text_string.replace(pos, 1, "");
      }
   }

   return text_string;
}
*/


bool FileExists( const char * fileName )
{
   // returns true if file exists, else false
   return _access( fileName, 0 ) == 0;
}


/// Phoneme to viseme mapping - Maps phonemes Cerevoice returns to visemes SmartBody can show
void set_phonemes_to_visemes( const std::string & visemeMapping )
{
   g_visemeMapping = visemeMapping;
   g_phonemeToViseme.clear();

   if ( visemeMapping == "sbmold" )
   {
      g_phonemeToViseme[ "sil" ] = "_";  // SIL
      g_phonemeToViseme[ "aa" ]  = "Ao"; // AA
      g_phonemeToViseme[ "ae" ]  = "Ih"; // AE
      g_phonemeToViseme[ "ah" ]  = "Ih"; // AH
      g_phonemeToViseme[ "ao" ]  = "Ao"; // AO
      g_phonemeToViseme[ "ax" ]  = "Ih"; // AX
      g_phonemeToViseme[ "@" ]  = "Ih"; // Shouldn't happen!
      g_phonemeToViseme[ "aw" ]  = "Ih"; // AW
      g_phonemeToViseme[ "ay" ]  = "Ih"; // AY
      g_phonemeToViseme[ "b" ]   = "BMP";//  B
      g_phonemeToViseme[ "ch" ]  = "j";  // CH
      g_phonemeToViseme[ "d" ]   = "D";  //  D
      g_phonemeToViseme[ "dh" ]  = "Th"; // DH
      g_phonemeToViseme[ "dx" ]  = "D";  // ??
      g_phonemeToViseme[ "eh" ]  = "Ih"; // EH
      g_phonemeToViseme[ "er" ]  = "Er"; // ER
      g_phonemeToViseme[ "ey" ]  = "Ih"; // ?? probably EY
      g_phonemeToViseme[ "f" ]   = "F";  //  F
      g_phonemeToViseme[ "g" ]   = "KG"; //  G
      g_phonemeToViseme[ "hh" ]  = "Ih"; // HH
      g_phonemeToViseme[ "ih" ]  = "Ih"; // IH
      g_phonemeToViseme[ "iy" ]  = "EE"; // IY
      g_phonemeToViseme[ "jh" ]  = "j";  // JH
      g_phonemeToViseme[ "k" ]   = "KG"; //  K
      g_phonemeToViseme[ "l" ]   = "D";  //  L
      g_phonemeToViseme[ "m" ]   = "BMP";//  M
      g_phonemeToViseme[ "n" ]   = "NG"; //  N
      g_phonemeToViseme[ "ng" ]  = "NG"; // NG
      g_phonemeToViseme[ "ow" ]  = "Oh"; // OW
      g_phonemeToViseme[ "oy" ]  = "Oh"; // OY
      g_phonemeToViseme[ "p" ]   = "BMP";//  P
      g_phonemeToViseme[ "r" ]   = "R";  //  R
      g_phonemeToViseme[ "s" ]   = "Z";  //  S
      g_phonemeToViseme[ "sh" ]  = "j";  // SH
      g_phonemeToViseme[ "T" ]   = "D";  //  T ?
      g_phonemeToViseme[ "t" ]   = "D";  //  T ?
      g_phonemeToViseme[ "th" ]  = "Th"; // TH
      g_phonemeToViseme[ "uh" ]  = "Oh"; // UH
      g_phonemeToViseme[ "uw" ]  = "Oh"; // UW
      g_phonemeToViseme[ "v" ]   = "F";  //  V
      g_phonemeToViseme[ "w" ]   = "OO"; //  W
      g_phonemeToViseme[ "y" ]   = "OO"; //  Y
      g_phonemeToViseme[ "z" ]   = "Z";  //  Z
      g_phonemeToViseme[ "zh" ]  = "J";  // ZH
      //g_phonemeToViseme[ "i" ]  = "";  //
      //g_phonemeToViseme[ "j" ]  = "";  //
      //possibly need phonemeToViseme entries for "i" and "j"?
   }
   else if (visemeMapping == "sbm")
   {
      g_phonemeToViseme[ "pau" ] = "_";  // SIL
      g_phonemeToViseme[ "aa" ]  = "Aa"; // AA
      g_phonemeToViseme[ "ae" ]  = "Ah"; // AE
      g_phonemeToViseme[ "ah" ]  = "Ah"; // AH
      g_phonemeToViseme[ "ao" ]  = "Ao"; // AO
      g_phonemeToViseme[ "ax" ]  = "Ah"; // AX
      g_phonemeToViseme[ "@" ]   = "Ih"; // ??
      g_phonemeToViseme[ "aw" ]  = "Ah"; // AW
      g_phonemeToViseme[ "ay" ]  = "Ay"; // AY
      g_phonemeToViseme[ "b" ]   = "BMP";//  B
      g_phonemeToViseme[ "ch" ]  = "Sh";  // CH
      g_phonemeToViseme[ "d" ]   = "D";  //  D
      g_phonemeToViseme[ "dh" ]  = "Th"; // DH
      g_phonemeToViseme[ "dx" ]  = "D";  // ??
      g_phonemeToViseme[ "eh" ]  = "Eh"; // EH
      g_phonemeToViseme[ "er" ]  = "Er"; // ER
      g_phonemeToViseme[ "ey" ]  = "Eh"; //
      g_phonemeToViseme[ "f" ]   = "F";  //  F
      g_phonemeToViseme[ "g" ]   = "Kg"; //  G
      g_phonemeToViseme[ "hh" ]  = "Ih"; // HH
      g_phonemeToViseme[ "ih" ]  = "Ih"; // IH
      g_phonemeToViseme[ "iy" ]  = "Ih"; // IY
      g_phonemeToViseme[ "jh" ]  = "Sh";  // JH
      g_phonemeToViseme[ "k" ]   = "Kg"; //  K
      g_phonemeToViseme[ "l" ]   = "L";  //  L
      g_phonemeToViseme[ "m" ]   = "BMP";//  M
      g_phonemeToViseme[ "n" ]   = "D"; //  N
      g_phonemeToViseme[ "ng" ]  = "Kg"; // NG
      g_phonemeToViseme[ "ow" ]  = "Ow"; // OW
      g_phonemeToViseme[ "oy" ]  = "Oy"; // OY
      g_phonemeToViseme[ "p" ]   = "BMP";//  P
      g_phonemeToViseme[ "r" ]   = "R";  //  R
      g_phonemeToViseme[ "s" ]   = "Z";  //  S
      g_phonemeToViseme[ "sh" ]  = "Sh";  // SH
      g_phonemeToViseme[ "T" ]   = "D";  // T?
      g_phonemeToViseme[ "t" ]   = "D";  // T?
      g_phonemeToViseme[ "th" ]  = "Th"; // TH
      g_phonemeToViseme[ "uh" ]  = "W"; // UH
      g_phonemeToViseme[ "uw" ]  = "W"; // UW
      g_phonemeToViseme[ "v" ]   = "F";  //  V
      g_phonemeToViseme[ "w" ]   = "W"; //  W
      g_phonemeToViseme[ "y" ]   = "Sh"; //  Y
      g_phonemeToViseme[ "z" ]   = "Z";  //  Z
      g_phonemeToViseme[ "zh" ]  = "Sh";  // ZH
    }
   else if (visemeMapping == "facefx")
   {
      g_phonemeToViseme[ "sil" ] = "_";  // SIL
   }
}


void cerevoice_tts::init( std::vector<char *> vctVoices, const std::string & visemeMapping )
{
   // Settings definitions
   voice_path = "..\\..\\..\\..\\..\\data\\cereproc\\voices\\";
   voice_file_extension = ".voice";
   license_file_extension = ".lic";

   // the below 2 paths are being set just as default values
   // the actual paths are obtained from SBM through the RemoteSpeechCmd and set at run time
   temp_audio_dir_cereproc = "..\\..\\..\\..\\..\\data\\cache\\audio\\";
   temp_audio_dir_player = "data\\cache\\audio\\";

   g_abbfile       = "../../../../../lib/cerevoice/veng_db/en/norm/abb.txt";
   g_pbreakfile    = "../../../../../lib/cerevoice/veng_db/en/norm/pbreak.txt";
   g_homographfile = "../../../../../lib/cerevoice/veng_db/en/homographs/rules.dat";
   g_reductionfile = "../../../../../lib/cerevoice/veng_db/en/reduction/rules.dat";
   g_rulesfile     = "../../../../../lib/cerevoice/veng_db/en/gb/norm/rules.py";

   for ( unsigned int i = 0; i < vctVoices.size(); i++ )
   {
      load_voice( vctVoices[ i ] );
   }

   setVisemeMapping( visemeMapping );

   if ( !FileExists( g_abbfile ) )
   {
      printf("Error: normalization file abb.txt not found!\n");
      //exit(1);
   }

   if ( !FileExists( g_pbreakfile ) )
   {
      printf("Error: normalization file pbreak.txt not found!\n");
      //exit(1);
   }

   if ( !FileExists( g_homographfile ) )
   {
      printf("Error: normalization file homographsrules.dat not found!\n");
      //exit(1);
   }

   if ( !FileExists( g_reductionfile ) )
   {
      printf("Error: normalization file reductionrules.dat not found!\n");
      //exit(1);
   }

   if ( !FileExists( g_rulesfile ) )
   {
      printf("Error: normalization file normrules.py not found!\n");
      //exit(1);
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
}


void cerevoice_tts::setVisemeMapping( const std::string & visemeMapping )
{
   set_phonemes_to_visemes( visemeMapping );
}


int _read_license( const char * licfile, char ** text, char ** signature )
{
   int text_sz = 0;
   int sig_sz = 0;
   int last_line_sz = 0;
   int line_sz = 0;

   FILE * licfp = fopen( licfile, "r" );
   if (licfp == NULL)
   {
       return 0;
   }

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


void cerevoice_tts::load_voice( const char * voice_id )
{
   // Create full license file name
   string lic_file;
   lic_file = voice_path;
   lic_file += voice_id;
   lic_file += license_file_extension;

   // Make sure license file exists
   if ( !FileExists( lic_file.c_str() ) )
   {
      printf("Error: license file not found! - %s.\n", lic_file.c_str());
      //exit(1);
   }

   char * lic_text = NULL;
   char * signature = NULL;
   _read_license( lic_file.c_str(), &lic_text, &signature );

   // Create full voice file name
   string voice_file;
   voice_file = voice_path;
   voice_file += voice_id;
   voice_file += voice_file_extension;

   // Make sure voice file exists
   if( !FileExists( voice_file.c_str() ) )
   {
      printf("Error: voice file '%s' not found!\n", voice_file.c_str());
      //exit(1);
   }

   // Load voice and add to map, with the voice_id being the key
   if (lic_text == NULL || signature == NULL || !FileExists( voice_file.c_str() ) )
   {
      g_voices[ voice_id ] = NULL;
   }
   else
   {
      g_voices[ voice_id ] = CPRC_load_voice( voice_file.c_str(), lic_text, static_cast<int>( strlen( lic_text ) ), signature, static_cast<int>( strlen( signature ) ) );
      //voices[ voice_id ] = CPRC_load_voicef( lic_file, voice_file );
   }

   // Free license strings (aalocated in read_license)
   free( lic_text );
   free( signature );
}


std::string cerevoice_tts::addUselTag(std::string text)
{
   string tempText = text;
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
   }
   return tempText;
}


std::string CreateMarkTimeStamps(const std::string & text)
{
   string tempText = text;
   string markUp = "";
   int i = 0;
   while (tempText!= "")
   {
      string temp = tempText;
      temp = temp.substr(0, tempText.find_first_of(" "));

      markUp = markUp.append("<mark name=\"T");
      markUp = markUp.append(vhcl::ToString(i));
      markUp = markUp.append("\" />");
      markUp = markUp.append(temp + "\n\n");
      i++;
      markUp = markUp.append("<mark name=\"T");
      markUp = markUp.append(vhcl::ToString(i));
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


std::string TransformTextWithTimes(const std::string & txt)
{
   //std::string text_string = "";

   //#define _PARSER_DEBUG_MESSAGES_ON
   std::stringstream txtstream;

   /// Start an XML parser to parse the message we have received
   XMLPlatformUtils::Initialize();
   XercesDOMParser *parser = new XercesDOMParser();

   string truncatedTxt = txt.substr(txt.find_first_of(">")+1);
   char * message = (char*)truncatedTxt.c_str();

   string actualText = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
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

            actualText = actualText.append("<speech id=\"" + string(speechID) + "\" ref=\"" + XMLString::transcode(speechElement->getAttribute(X("ref"))) + "\" type=\"" + XMLString::transcode(speechElement->getAttribute(X("type"))) + "\">\n\n");
            XMLString::release(&speechID);
         }
         else if ( !strcmp( XMLString::transcode( root->getNodeName() ), "speech") )
         {
            /// Else, the message might contain only the speech tag, in which case the above code will fail, so we need a fallback
            DOMElement *speechElement = root;
            char *speechID = XMLString::transcode(speechElement->getAttribute(XMLString::transcode("id")));
            char *text = XMLString::transcode(speechElement->getTextContent());

            string textContent = CreateMarkTimeStamps(text);
            /*if (!speechID)
            {
               speechID = "sp1";
            }*/
            //hard coding sp1
            speechID = "sp1";
            //XMLString::transcode(speechElement->getAttribute(X("type")))
            actualText = actualText.append("<speech id=\"" + string(speechID) + "\" ref=\"" + XMLString::transcode(speechElement->getAttribute(X("ref"))) + "\" type=\"" + "application/ssml+xml" + "\">\n\n");
            actualText = actualText.append(textContent);
            actualText = actualText.append("</speech>");
            //XMLString::release(&speechID);
         }
      }
   }

   return actualText;
}


void cerevoice_tts::tts( const char * text, const char * cereproc_file_name, const char * player_file_name, const string & voice_id )
{
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

   // If voice doesn't exist, ignore; another voice relay might pick that up
   if ( !g_voices[ voice_id ] )
   {
      printf( "\nERROR: voice load failed, check voice file and license integrity.\n\nIgnoring voice '%s'.\n", voice_id.c_str() );
      return;
   }

   // Create a reference to the normaliser, which needs to be passed in to all normaliser functions
   int norm_id = Normaliser_create( elNEWID );
   Normaliser_set_abbreviations( norm_id, g_abbfile );
   Normaliser_set_pbreaks( norm_id, g_pbreakfile );
   Normaliser_set_rules( norm_id, g_rulesfile );
   Normaliser_set_homographs( norm_id, g_homographfile );
   Normaliser_set_reductions( norm_id, g_reductionfile );

   // file to record database coverage data
   //char * logfile = "";
   //char * covfile = "";
   //char * lexfile = "";

   // Set up a spurt, an audio buffer for data, and a lexicon search structure
   CPRC_spurt * spurt = CPRC_spurt_new( g_voices[ voice_id ] );
   CPRC_abuf * abuf = CPRC_abuf_new( 22050 );
   CPRC_lts_search * ltssrch = CPRC_lts_search_new( 0 );
   CPRC_lexicon_search * lxsrch = CPRC_lexicon_search_new();

   // Local variable to store message XML metadata
   SpeechRequestMessageData xmlMetaData;

   // Feed in input text, further data is to come
   //Added by Apar Suri
   //Replacing . and , with "" so because there seems to be a bug in CPRCPMOD_spurt_synth while synthesizing multiple spurts
   //Apparently a time of 0.0 and 0.1 seems to be added just after the , or .
   string text_string = RemoveXMLTagsAndNewLines( text, xmlMetaData);

   if (xmlMetaData.tags.size() <= 0 )
   {
      //text_string = text;
      text_string =  TransformTextWithTimes(text);
      text_string = RemoveXMLTagsAndNewLines(text_string, xmlMetaData);
   }

   /*
   if (xmlMetaData.tags.size() <= 0)
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
   }
   */

   //commenting replacing pause punctuations
   //text_string = replacePausePunctuationsFromText(text_string);

   //Checking to see if it is conv voice, then add the tag "<usel genre='spon'>"
   if (voice_id.compare("starconv") == 0)
   {
      text_string = addUselTag(text_string);
   }

   printf("Normaliser_parse() - %s\n", text_string.c_str());

   Normaliser_parse( norm_id, const_cast<char*>(text_string.c_str()), 1 );
   //Normaliser_parse( norm_id, "", 1 );

   int numspts = Normaliser_get_num_spurts( norm_id );

   if ( numspts )
   {
      int sptcount = 0;
      for ( int n = 0; n < numspts; n++ )
      {
         CPRCPMOD_spurt_synth( Normaliser_get_spurt( norm_id, n ), spurt, lxsrch, ltssrch, abuf );
         sptcount++;
      }
   }

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
   CPRC_riff_save( abuf, cereproc_file_name );


   {
      try
      {
         bool wordElementHasChildNodes = false;  // using the previous logic when this code had xml

         int num_words = 0;


         DOMNodeList * list_of_faceFX_phonemes = NULL;

         if ( g_visemeMapping == "facefx" )
         {
            XercesDOMParser *parser = new XercesDOMParser;
            parser->parse(MemBufInputSource((const XMLByte *)facefxMapping, strlen(facefxMapping), "facefxMapping", false));
            DOMDocument* rule_input;
            rule_input = parser->getDocument();
            list_of_faceFX_phonemes = rule_input->getElementsByTagName(XMLString::transcode( "entry" ));
         }


         double  end_time_d = 0;
         double  word_start_d = 0;

         for ( int i = 0; i < abuf->trans_sz; i++ )
         {
            if ( abuf->trans[ i ].type == CPRC_ABUF_TRANS_PHONE )
            {
               size_t loopLength = 1;

               if ( g_visemeMapping == "facefx" )
               {
                  loopLength = list_of_faceFX_phonemes->getLength();
               }

               string  phone_type_s;
               bool phonemeMatched = false;

               for (size_t phonemeCounter = 0; phonemeCounter < loopLength; phonemeCounter++)
               {
                  string phoneme;
                  string viseme;
                  string articulation;

                  if ( g_visemeMapping == "facefx" )
                  {
                     DOMElement* entry = (DOMElement*)list_of_faceFX_phonemes->item(phonemeCounter);

                     phoneme = XMLString::transcode(entry->getAttribute(XMLString::transcode( ("phoneme") )));
                     viseme = XMLString::transcode(entry->getAttribute(XMLString::transcode( ("target") )));
                     articulation = XMLString::transcode(entry->getAttribute(XMLString::transcode( ("amount") )));
                  }

                  std::map<string, string>::iterator iter = g_phonemeToViseme.find( abuf->trans[ i ].name );
                  string phonemeInSoundFile = abuf->trans[ i ].name;

                  // Check added to avoid crashing on entries which are not defined
                  if ( (g_visemeMapping == "sbm" && iter != g_phonemeToViseme.end()) ||
                       (g_visemeMapping == "facefx" && phoneme == phonemeInSoundFile) )
                  {
                     phonemeMatched = true;
                     if ( g_visemeMapping == "sbm" )
                     {
                        phone_type_s = iter->second;
                     }

                     end_time_d = abuf->trans[i].end;
                     double start_time_d = abuf->trans[i].start;

                     if ( g_visemeMapping == "sbm" )
                     {
                        g_visemeListType.emplace_back(phone_type_s);
                        g_visemeListStart.emplace_back(start_time_d);
                        g_visemeListArticulation.emplace_back(1.0);
                     }
                     else if ( g_visemeMapping == "facefx" )
                     {
                        g_visemeListType.emplace_back(viseme);
                        g_visemeListStart.emplace_back(start_time_d);
                        g_visemeListArticulation.emplace_back(vhcl::ToDouble(articulation));
                     }

                     if ( strcmp( abuf->trans[ i ].name, "sil" ) == 0 )
                     {
                     }
                     else
                     {
                        wordElementHasChildNodes = true;
                     }
                  }
               }


               if ( ( i < ( abuf->trans_sz - 1 ) ) && phonemeMatched )
               {
                  if ( ( abuf->trans[ i + 1 ].type == CPRC_ABUF_TRANS_WORD ) ||
                       ( abuf->trans[ i + 1 ].type == CPRC_ABUF_TRANS_MARK ) ||
                       ( strcmp( abuf->trans[ i + 1 ].name, "sil" ) == 0 ) )
                  {
                     if ( wordElementHasChildNodes )
                     {
                        g_wordBreakListStart.emplace_back(word_start_d);
                        g_wordBreakListEnd.emplace_back(end_time_d);
                     }

                     wordElementHasChildNodes = false;

                     word_start_d = abuf->trans[ i + 1 ].start;   // saved for later


                     if (xmlMetaData.tags.size() > 0)
                     {
                     // Add marker information back
                     // Should come from initial XML message,
                     // but for now is constructed following the known NVBG protocol (markers around words)
                     // HACK AVOIDED: Fixed to use XML metadata as extracted during parse
                     //changed by Apar Suri so that it does not coincide with the outer loop i
                     int double_words = num_words * 2;
                     string s = xmlMetaData.speechIdentifier + ":";

                     if ( num_words * 2 < (int)xmlMetaData.tags.size() )
                     {
                        /// If we have an existing tag for this number, use it
                        s += xmlMetaData.tags[num_words * 2];
                     }
                     else
                     {
                        /// Extract alphabetical part of the tags, in the hope that we're following a coherent naming pattern, and only numbers are changed
                        string last_tag = xmlMetaData.tags[xmlMetaData.tags.size() - 1];
                        size_t j;
                        for ( j = 0; j < last_tag.size(); j++ )
                        {
                           if ( !isalpha(last_tag[j]) )
                           {
                              break;
                           }
                        }

                        s += last_tag.substr(0, j) + vhcl::ToString(double_words);
                        printf("Warning: Reference to unspecified tag, constructing one on-the-fly to be: %s\n", s.c_str());
                     }

                     g_markListName.emplace_back(s);
                     g_markListTime.emplace_back(word_start_d);


                     double_words = num_words * 2 + 1;
                     s = xmlMetaData.speechIdentifier + ":";

                     /// Modifications to use XML tags saved at previous step - use stored tags, or generate new ones if unseen tag referenced (reason for unseen tags being referenced is still not known)
                     if ( num_words * 2 + 1 < (int)xmlMetaData.tags.size() )
                     {
                        /// If we have an existing tag for this number, use it
                        s += xmlMetaData.tags[num_words * 2 + 1];
                     }
                     else
                     {
                        /// Extract alphabetical part of the tags, in the hope that we're following a coherent naming pattern, and only numbers are changed
                        string last_tag = xmlMetaData.tags[xmlMetaData.tags.size() - 1];
                        size_t j;
                        for ( j = 0; j < last_tag.size(); j++ )
                        {
                           if ( !isalpha(last_tag[j]) )
                           {
                              break;
                           }
                        }

                        s += last_tag.substr(0, j) + vhcl::ToString(double_words);
                        printf("Warning: Reference to unspecified tag, constructing one on-the-fly to be: %s\n", s.c_str());
                     }

                     g_markListName.emplace_back(s);
                     g_markListTime.emplace_back(end_time_d);
                     }

                     num_words++;
                  }
               }
            }
         }
      }
      catch ( const OutOfMemoryException & )
      {
         XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
      }
      catch ( const DOMException & e )
      {
         XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
      }
      catch ( const XMLException & toCatch )
      {
         printf( "XMLException occurred: %d", toCatch.getCode() );
      }
      catch (...)
      {
         XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
      }
   }

   //int speech_sz = 0;
   //speech_sz += abuf->wav_sz;
   CPRC_lts_search_reset( ltssrch );
   CPRC_spurt_clear( spurt );
   //int fno = 0;
   //fno += 1;
}
