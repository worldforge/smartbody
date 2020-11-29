/*************************************************************************/
/*                                                                       */
/*                Centre for Speech Technology Research                  */
/*                     University of Edinburgh, UK                       */
/*                       Copyright (c) 1996,1997                         */
/*                        All Rights Reserved.                           */
/*                                                                       */
/*  Permission is hereby granted, free of charge, to use and distribute  */
/*  this software and its documentation without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,  */
/*  distribute, sublicense, and/or sell copies of this work, and to      */
/*  permit persons to whom this work is furnished to do so, subject to   */
/*  the following conditions:                                            */
/*   1. The code must retain the above copyright notice, this list of    */
/*      conditions and the following disclaimer.                         */
/*   2. Any modifications must be clearly marked as such.                */
/*   3. Original authors' names are not deleted.                         */
/*   4. The authors' names are not used to endorse or promote products   */
/*      derived from this software without specific prior written        */
/*      permission.                                                      */
/*                                                                       */
/*  THE UNIVERSITY OF EDINBURGH AND THE CONTRIBUTORS TO THIS WORK        */
/*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      */
/*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL THE UNIVERSITY OF EDINBURGH NOR THE CONTRIBUTORS BE LIABLE     */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   */
/*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          */
/*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       */
/*  THIS SOFTWARE.                                                       */
/*                                                                       */
/*************************************************************************/
/*                      Author :  Shridhar Ravikumar                     */
/*                      Date   :  September 2010                         */
/*-----------------------------------------------------------------------*/
/*                                                                       */
/*						Festival Text To Speech Relay					 */
/* 																		 */
/*=======================================================================*/

#include "vhcl.h"

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
//New Modifications to better use XML structure of input message
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <algorithm>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif
#ifndef SB_NO_VHMSG
#include "vhmsg-tt.h"
#endif
#include <festival.h>
#include <VHDuration.h>
#include <sstream>
#include <string>

XERCES_CPP_NAMESPACE_USE

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
//#define FESTIVAL_HEAP_SIZE 999999

bool isDone = false;



#if !defined(WIN32) && !defined(__APPLE__)
	extern SpeechRequestData xmlMetaData;
	extern std::map<std::string, std::pair<std::string, double> > phonemeToViseme;
	std::string mapping;
#else 
#if defined(__APPLE__)
	std::string mapping;
	extern SpeechRequestData xmlMetaData;
	extern std::multimap<std::string, std::pair<string, double> > phonemeToViseme;
#else

	std::string mapping;
	extern SpeechRequestData xmlMetaData;
	extern std::multimap<std::string, std::pair<string, double> > phonemeToViseme;
#endif
#endif
std::string festivalLibDir = "";

#ifdef WIN32
string cache_directory = "..\\..\\data\\cache\\festival\\";
const char * FESTIVAL_RELAY_LIB_DIR = "..\\..\\lib\\festival\\festival\\lib";
#else
string cache_directory = "../../data/cache/festival/";
const char * FESTIVAL_RELAY_LIB_DIR = "../../lib/festival/festival/lib";
#endif


/// Cleans up spurious whitespaces in string, and removes weird \n's
void cleanString(std::string &message)
{
	/// Remove newlines and carriage-returns
	message.erase( std::remove(message.begin(), message.end(), '\r'), message.end() );
	message.erase( std::remove(message.begin(), message.end(), '\n'), message.end() );
	/// If it's a space string, we want to leave the last whitespace
	while ( message.find_last_of(" ") == message.length() - 1  && message.length() > 1)
	{
		//fprintf(stderr,"Debug: Reducing length by 1 to remove whitespace at end\n");
		message.resize( message.length() - 1 );
	}

	size_t pos;
	while (1)
	{
		pos = message.find("  ");
		if (pos == std::string::npos)
			break;
		//fprintf(stderr,"Debug: replacing 2 whitespaces at %d(%s) with 1 whitespace\n",pos, message.substr(pos,2).c_str());
		message.replace( pos, 2, " " );
	}

	while (( message.find_first_of(" ") == 0 ) && (message.length() > 1))
	{		
		//fprintf(stderr,"Debug: Cleaning initial whitespace %s\n",message[0]);
		message = message.substr(1);
	}
}


// read the input RemoteSpeechCommand message and store it's data so we can use it for generating the output RemoteSpeechReply message
std::string storeXMLMetaData( const std::string & txt)
{

   /// Put in some defaults, and do basic cleanup just to be safe
   xmlMetaData.speechIdentifier = "sp1";
   xmlMetaData.tags.clear();
   xmlMetaData.words.clear();
   /// Start an XML parser to parse the message we have received
   XMLPlatformUtils::Initialize();
   XercesDOMParser *parser = new XercesDOMParser();

   std::string truncatedTxt = txt.substr(txt.find_first_of(">")+1);
   char * message = (char*)truncatedTxt.c_str();

   std::string actualText = "";

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
			   XMLString::release(&speechID);
		   }
		   else if ( !strcmp( XMLString::transcode( root->getNodeName() ), "speech") ) {
			   /// Else, the message might contain only the speech tag, in which case the above code will fail, so we need a fallback
			   DOMElement *speechElement = root;
			   char *speechID = XMLString::transcode(speechElement->getAttribute(XMLString::transcode("id")));
			   xmlMetaData.speechIdentifier = std::string(speechID);			   
			   XMLString::release(&speechID);
		   }
		   else
		   {
			   /// Oops, for some reason all of the above didn't work, default to the default speech id
			   fprintf(stderr, "Warning: Could not find speech tag in message, creating message beginning by default\n");
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


					   //print the DOM to a memory terminal 
						DOMLSSerializer* theSerializer2 = DOMImplementation::getImplementation()->createLSSerializer();
						XMLFormatTarget *myFormatTarget2 = new MemBufFormatTarget();
						DOMLSOutput* myLSOutput2 = DOMImplementation::getImplementation()->createLSOutput();
						myLSOutput2->setByteStream( myFormatTarget2 );

						// serialize a DOMNode to an internal memory buffer
						theSerializer2->write(element, myLSOutput2);

						// print the final bml code to the terminal
						string output =
						(char*)  ((MemBufFormatTarget*)myFormatTarget2)->getRawBuffer();		
					   

					   XMLCh *mark = (XMLCh*)element->getAttribute(XMLString::transcode("name"));
					   XMLCh *speech = NULL;
					   DOMNode *speechNode = element->getFirstChild();
					   


					   if ( speechNode == NULL ) 
					   {
						   speechNode = element->getNextSibling();		
					   }
					   if ( (speechNode !=NULL) && ( speechNode->getNodeType() == DOMNode::TEXT_NODE ) )
					   {
						   speech = (XMLCh*)speechNode->getNodeValue();
					   }					   					   



					   /// Get the timing tag as a string
					   char * t1, *t2;
					   std::string markString(t1 = XMLString::transcode(mark));
					   std::string speechString;

					   if(speechNode !=NULL)
					   {
						   t2 = (speech)? (char*) XMLString::transcode(speech): (char*) " ";
						   speechString = t2;
					   }
					   else
						   speechString = "";
						   
					   XMLString::release(&t1);
					   XMLString::release(&t2);

					   if( !strcmp(markString.c_str(),"") || !strcmp(speechString.c_str(),"") )
					   {
						   /// Null strings tend to cause problems with later code sections
						   if ( !strcmp(speechString.c_str(),"") ) speechString = " ";
					   }
					   else
					   {
						   std::string temporaryText = speechString;
						   cleanString(temporaryText);
						   /// Words can be used to match up which tags we need to see
							/// Push tag and word into xmlMetaData
							xmlMetaData.tags.emplace_back(markString);
							xmlMetaData.words.emplace_back(temporaryText);
							actualText += " ";
							actualText += temporaryText;
							cleanString(actualText);
					   }
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

   if ( actualText != "" ) {
	   return actualText;
   }
   else {
	   //fprintf(stderr, "Error: Unable to instantiate DOM Xml parser, exiting \n");
	   return "";
   }
}


// remove extra tabs from the string and replce with spaces
void removeTabsFromString(string &spoken_text)
{
	std::string::size_type pos = 0;

	for (pos = 0; pos < spoken_text.length(); ++pos)
	{
	  if (spoken_text.at(pos)== 9)
	  {
		  if(spoken_text.at(pos-1) == 9)
			spoken_text.replace(pos, 1, "");
		  else
			spoken_text.replace(pos, 1, " ");
	  }
	}
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
#ifdef WIN32
		sprintf_s(number, "%d", i);
#else
		sprintf(number, "%d", i);
#endif
		markUp = markUp.append("<mark name=\"T");
		markUp = markUp.append(number);
		markUp = markUp.append("\" />");
		markUp = markUp.append(temp + "\n\n");
#ifdef WIN32
		sprintf_s(number, "%d", ++i);
#else
		sprintf(number, "%d", ++i);
#endif
		markUp = markUp.append("<mark name=\"T");
		markUp = markUp.append(number);
		markUp = markUp.append("\" />\n\n");
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
			   speechID = (char*) "sp1";
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


// generate the sound file for the specified text
std::string generateReply(const char * utterance,const char * soundFileName)
{

	EST_Wave wave;

	string spoken_text = storeXMLMetaData( utterance );
	//printf("done first time\n");
	if (!spoken_text.compare("") && spoken_text != "")
	{
		puts(spoken_text.append("\n").c_str());
	}

	if (xmlMetaData.tags.size() <= 0)
	{
		spoken_text = TransformTextWithTimes(utterance);
		//printf("done transforming\n");
		if (!spoken_text.compare("") && spoken_text != "")
		{
			puts(spoken_text.append("\n").c_str());
		}
		spoken_text = storeXMLMetaData(spoken_text);
		//printf("done second time\n");
	}

	removeTabsFromString(spoken_text);

	printf( "generateReply() - \nbefore: '%s'\nafter: '%s'\n'%s'\n", utterance, spoken_text.c_str(), soundFileName );

	//festival_say_text(spoken_text.c_str());
	int result = festival_text_to_wave(spoken_text.c_str(),wave);
	if (result == FALSE)
	{
		printf("Problem creating sound file from speech '%s'", spoken_text.c_str());
	}
    //wave.save(soundFileName,"riff");
    //int saveResult = wave.save(soundFileName,"wav");
	int saveResult = wave.save(soundFileName,"aiff");
    if (saveResult == FALSE)
    {
    	printf("Problem writing sound to file '%s'", soundFileName);
    }

	return xmlMetaData.replyString;
}


// process the input RemoteSpeechCommand message
void process_message( const char * message )
{
	std::string message_c = message;

	// parse the string
	std::vector< std::string > tokens;
	const std::string delimiters = " ";
	vhcl::Tokenize( message_c, tokens, delimiters );

	/// Get non-XML components of the message
	std::string command = tokens.at( 0 );

	std::string agent_name = tokens.at( 1 );
	std::string message_id = tokens.at( 2 );
	std::string voice_id = tokens.at( 3 );
	std::string file_name = tokens.at( 4 );
	size_t prefix_length = message_c.find( file_name, 0 ) + file_name.length() + 1;
	std::string utterance = message_c.substr( prefix_length );  // strip off the prefix, only using the xml

	// remove anything after </speech> tag
	size_t postfix_pos = utterance.rfind( "</speech>" );
	if ( postfix_pos != std::string::npos )
	  utterance = utterance.substr( 0, postfix_pos + 9 );

	// parse out just the sound file name and give it a .wav file type
	int pos = file_name.find( ".aiff" );
	int pos2 = file_name.find( "utt" );

	file_name = file_name.substr( pos2, pos - pos2 ) + ".wav";

	std::string festival_file_name = cache_directory + file_name;

	//Generate the audio
	string replyXML = generateReply(utterance.c_str(),festival_file_name.c_str());
	printf("\nafter reply, replyXML = %s\n",replyXML.c_str());
	string remoteSpeechReply = agent_name+" "+message_id+" OK: <?xml version=\"1.0\" encoding=\"UTF-8\"?><speak><soundFile name=\"";

#ifdef WIN32
	char full[ _MAX_PATH ];
#else
	char full[PATH_MAX];
#endif
#ifdef WIN32
	if ( _fullpath( full, festival_file_name.c_str(), _MAX_PATH ) == NULL )
#else
	realpath(festival_file_name.c_str(), full);
	if (full == NULL) 
#endif
	{
		printf("\nError converting path sent from SBM to absolute path\n");
	}
	string soundPathName = full;

	remoteSpeechReply += soundPathName+"\"/>";
	remoteSpeechReply += replyXML + "</speak>";


	//Only send out a reply when result is not empty, ignore otherwise as a nother voice relay might pick up the request
	if ( replyXML.compare("") != 0 )
	{
      printf("%s", remoteSpeechReply.c_str());
	  vhmsg::ttu_notify2( "RemoteSpeechReply", remoteSpeechReply.c_str() );
	}
}


// handles messages on receiving them
void tt_client_callback( const char * op, const char * args, void * user_data )
{
   string sOp = op;
   string sArgs = args;
   vector< string > splitArgs;
   vhcl::Tokenize( sArgs, splitArgs );

   if ( sOp.compare( "vrAllCall" ) == 0 )
   {
      vhmsg::ttu_notify2( "vrComponent", "tts" );
   }
   if ( sOp.compare( "RemoteSpeechCmd" ) == 0 )
   {
      process_message( args );
   }
   if ( sOp.compare( "vrKillComponent" ) == 0 )
   {
      if ( splitArgs.size() > 0 )
      {
         if ( splitArgs[ 0 ].compare( "tts" ) == 0 ||
              splitArgs[ 0 ].compare( "all" ) == 0 )
         {
			vhmsg::ttu_notify2( "vrProcEnd", "tts festival" );
			isDone =true;
         }
      }
   }
}



//Phoneme to viseme mapping
 void set_phonemes_to_visemes()
{

     if(mapping == "sbmold")
    {
	
        phonemeToViseme.insert(std::make_pair("pau", std::make_pair("_", 1.0)));  // SIL
        phonemeToViseme.insert(make_pair("aa", make_pair("Ao", 1.0))); // AA
        phonemeToViseme.insert(make_pair("ae", make_pair("Ih", 1.0))); // AE
        phonemeToViseme.insert(make_pair("ah", make_pair("Ih", 1.0))); // AH
        phonemeToViseme.insert(make_pair("ao", make_pair("Ao", 1.0))); // AO
        phonemeToViseme.insert(make_pair("ax", make_pair("Ih", 1.0))); // AX
        phonemeToViseme.insert(make_pair("@",  make_pair("Ih", 1.0))); //
        phonemeToViseme.insert(make_pair("aw", make_pair("Ih", 1.0))); // AW
        phonemeToViseme.insert(make_pair("ay", make_pair("Ih", 1.0))); // AY
        phonemeToViseme.insert(make_pair("b",  make_pair("BMP", 1.0)));//  B
        phonemeToViseme.insert(make_pair("ch", make_pair("j", 1.0)));  // CH
        phonemeToViseme.insert(make_pair("d",  make_pair("D", 1.0)));  //  D
        phonemeToViseme.insert(make_pair("dh", make_pair("Th", 1.0))); // DH
        phonemeToViseme.insert(make_pair("dx", make_pair("D", 1.0)));  // ??
        phonemeToViseme.insert(make_pair("eh", make_pair("Ih", 1.0))); // EH
        phonemeToViseme.insert(make_pair("er", make_pair("Er", 1.0))); // ER
        phonemeToViseme.insert(make_pair("ey", make_pair("Ih", 1.0))); //
        phonemeToViseme.insert(make_pair("f",  make_pair("F", 1.0)));  //  F
        phonemeToViseme.insert(make_pair("g",  make_pair("KG", 1.0))); //  G
        phonemeToViseme.insert(make_pair("hh", make_pair("Ih", 1.0))); // HH
        phonemeToViseme.insert(make_pair("ih", make_pair("Ih", 1.0))); // IH
        phonemeToViseme.insert(make_pair("iy", make_pair("EE", 1.0))); // IY
        phonemeToViseme.insert(make_pair("jh", make_pair("j", 1.0)));  // JH
        phonemeToViseme.insert(make_pair("k",  make_pair("KG", 1.0))); //  K
        phonemeToViseme.insert(make_pair("l",  make_pair("D", 1.0)));  //  L
        phonemeToViseme.insert(make_pair("m",  make_pair("BMP", 1.0)));//  M
        phonemeToViseme.insert(make_pair("n",  make_pair("NG", 1.0))); //  N
        phonemeToViseme.insert(make_pair("ng", make_pair("NG", 1.0))); // NG
        phonemeToViseme.insert(make_pair("ow", make_pair("Oh", 1.0))); // OW
        phonemeToViseme.insert(make_pair("oy", make_pair("Oh", 1.0))); // OY
        phonemeToViseme.insert(make_pair("p",  make_pair("BMP", 1.0)));//  P
        phonemeToViseme.insert(make_pair("r",  make_pair("R", 1.0)));  //  R
        phonemeToViseme.insert(make_pair("s",  make_pair("Z", 1.0)));  //  S
        phonemeToViseme.insert(make_pair("sh", make_pair("j", 1.0)));  // SH
        phonemeToViseme.insert(make_pair("T",  make_pair("D", 1.0)));  // T?
        phonemeToViseme.insert(make_pair("t",  make_pair("D", 1.0)));  // T?
        phonemeToViseme.insert(make_pair("th", make_pair("Th", 1.0))); // TH
        phonemeToViseme.insert(make_pair("uh", make_pair("Oh", 1.0))); // UH
        phonemeToViseme.insert(make_pair("uw", make_pair("Oh", 1.0))); // UW
        phonemeToViseme.insert(make_pair("v",  make_pair("F", 1.0)));  //  V
        phonemeToViseme.insert(make_pair("w",  make_pair("OO", 1.0))); //  W
        phonemeToViseme.insert(make_pair("y",  make_pair("OO", 1.0))); //  Y
        phonemeToViseme.insert(make_pair("z",  make_pair("Z", 1.0)));  //  Z
        phonemeToViseme.insert(make_pair("zh", make_pair("J", 1.0)));  // ZH
    }
	else if (mapping == "sbm")
	{
        phonemeToViseme.insert(make_pair("pau", make_pair("_", 1.0)));  // SIL
        phonemeToViseme.insert(make_pair("aa", make_pair("Aa", 1.0))); // AA
        phonemeToViseme.insert(make_pair("ae", make_pair("Ah", 1.0))); // AE
        phonemeToViseme.insert(make_pair("ah", make_pair("Ah", 1.0))); // AH
        phonemeToViseme.insert(make_pair("ao", make_pair("Ao", 1.0))); // AO
        phonemeToViseme.insert(make_pair("ax", make_pair("Ah", 1.0))); // AX
        phonemeToViseme.insert(make_pair("@",  make_pair("Ih", 1.0))); // ??
        phonemeToViseme.insert(make_pair("aw", make_pair("Ah", 1.0))); // AW
        phonemeToViseme.insert(make_pair("ay", make_pair("Ay", 1.0))); // AY
        phonemeToViseme.insert(make_pair("b",  make_pair("BMP", 1.0)));//  B
        phonemeToViseme.insert(make_pair("ch", make_pair("Sh", 1.0)));  // CH
        phonemeToViseme.insert(make_pair("d",  make_pair("D", 1.0)));  //  D
        phonemeToViseme.insert(make_pair("dh", make_pair("Th", 1.0))); // DH
        phonemeToViseme.insert(make_pair("dx", make_pair("D", 1.0)));  // ??
        phonemeToViseme.insert(make_pair("eh", make_pair("Eh", 1.0))); // EH
        phonemeToViseme.insert(make_pair("er", make_pair("Er", 1.0))); // ER
        phonemeToViseme.insert(make_pair("ey", make_pair("Eh", 1.0))); //
        phonemeToViseme.insert(make_pair("f",  make_pair("F", 1.0)));  //  F
        phonemeToViseme.insert(make_pair("g",  make_pair("Kg", 1.0))); //  G
        phonemeToViseme.insert(make_pair("hh", make_pair("Ih", 1.0))); // HH
        phonemeToViseme.insert(make_pair("ih", make_pair("Ih", 1.0))); // IH
        phonemeToViseme.insert(make_pair("iy", make_pair("Ih", 1.0))); // IY
        phonemeToViseme.insert(make_pair("jh", make_pair("Sh", 1.0)));  // JH
        phonemeToViseme.insert(make_pair("k",  make_pair("Kg", 1.0))); //  K
        phonemeToViseme.insert(make_pair("l",  make_pair("L", 1.0)));  //  L
        phonemeToViseme.insert(make_pair("m",  make_pair("BMP", 1.0)));//  M
        phonemeToViseme.insert(make_pair("n",  make_pair("D", 1.0))); //  N
        phonemeToViseme.insert(make_pair("ng", make_pair("Kg", 1.0))); // NG
        phonemeToViseme.insert(make_pair("ow", make_pair("Ow", 1.0))); // OW
        phonemeToViseme.insert(make_pair("oy", make_pair("Oy", 1.0))); // OY
        phonemeToViseme.insert(make_pair("p",  make_pair("BMP", 1.0)));//  P
        phonemeToViseme.insert(make_pair("r",  make_pair("R", 1.0)));  //  R
        phonemeToViseme.insert(make_pair("s",  make_pair("Z", 1.0)));  //  S
        phonemeToViseme.insert(make_pair("sh", make_pair("Sh", 1.0)));  // SH
        phonemeToViseme.insert(make_pair("T",  make_pair("D", 1.0)));  // T?
        phonemeToViseme.insert(make_pair("t",  make_pair("D", 1.0)));  // T?
        phonemeToViseme.insert(make_pair("th", make_pair("Th", 1.0))); // TH
        phonemeToViseme.insert(make_pair("uh", make_pair("Eh", 1.0))); // UH
        phonemeToViseme.insert(make_pair("uw", make_pair("W", 1.0))); // UW
        phonemeToViseme.insert(make_pair("v",  make_pair("F", 1.0)));  //  V
        phonemeToViseme.insert(make_pair("w",  make_pair("W", 1.0))); //  W
        phonemeToViseme.insert(make_pair("y",  make_pair("Ih", 1.0))); //  Y
        phonemeToViseme.insert(make_pair("z",  make_pair("Z", 1.0)));  //  Z
        phonemeToViseme.insert(make_pair("zh", make_pair("Sh", 1.0)));  // ZH
	}
   
}







// initialize activeMQ
bool open_activeMQ()
{
   vhmsg::ttu_set_client_callback( tt_client_callback );

   int err = vhmsg::ttu_open();
   if ( err != vhmsg::TTU_SUCCESS )
   {
      cout<<"ttu_open failed!";
      return false;
   }

   // register for messages
   vhmsg::ttu_register( "vrKillComponent" );
   vhmsg::ttu_register( "vrAllCall" );
   vhmsg::ttu_register( "RemoteSpeechCmd" );
   vhmsg::ttu_notify2( "vrComponent", "tts" );

   return true;
}


// close activeMQ
void close_activeMQ()
{
   vhmsg::ttu_close();
}


#ifdef WIN32
BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
    //char mesg[128];

    switch(CEvent)
    {
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
		vhmsg::ttu_notify2("vrProcEnd", "tts festival");
        break;    
	default:
        break;

    }
    return TRUE;
}
#endif



int main(int argc, char **argv)
{
	std::string scriptFile = "";
	std::string voice = "voice_kal_diphone";

	mapping = "sbm";

#ifdef WIN32
	std::string festivalLibDir = "..\\..\\lib\\festival\\festival\\lib";
#else
	std::string festivalLibDir = "../../lib/festival/festival/lib";
#endif

	for(int i=1; i<argc; ++i)
	{
		printf("%s\n", argv[i]);
#ifdef WIN32
		if(!strcmp(argv[i],"-hideConsole"))
		{
			HWND hWnd = GetConsoleWindow();
			ShowWindow( hWnd, SW_HIDE );
		}
		else 
#endif
		if (!strcmp(argv[i], "-script"))
		{
			if (argc > i + 1)
			{
				scriptFile = argv[i + 1];
				i++;
			}
			else
			{
				printf("Use: -script <scriptfile>");
			}
		}
		else if (!strcmp(argv[i], "-voice"))
		{
			if (argc > i + 1)
			{
				voice = argv[i + 1];
				i++;
			}
			else
			{
				printf("Use: -voice <voice>");
			}
		}
		else if (!strcmp(argv[i], "-festivalLibDir"))
		{
			if (argc > i + 1)
			{
				festivalLibDir = argv[i + 1];
				i++;
			}
			else
			{
				printf("Use: -festivalLibDir <dir>");
			}
		}
		else if (!strcmp(argv[i], "-cacheDir"))
		{
			if (argc > i + 1)
			{
				cache_directory = argv[i + 1];
				i++;
			}
			else
			{
				printf("Use: -cacheDir <dir>");
			}
		}
		else if (!strcmp(argv[i], "-mapping"))
		{
			if (argc > i + 1)
			{
				mapping = argv[i + 1];
				i++;
			}
			else
			{
				printf("Use: -mapping <sbm | sbmold | facefx>");
			}
		}
	}

	festival_libdir = festivalLibDir.c_str();

    int heap_size = FESTIVAL_HEAP_SIZE;  
    int load_init_files = 1; // we want the festival init files loaded
    festival_initialize(load_init_files,heap_size);



    set_phonemes_to_visemes();


#ifdef WIN32
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE)==FALSE)
	{
		// unable to install handler... 
		// display message to the user

		printf("Unable to install handler for console events!\n");
		return -1;
	}
#endif


	printf( "Festival Text To Speech Engine:\n\n" );
	printf( "Initializing....\n");
	printf( "Hooking up VH Module\n");

	std::vector<std::string> festivalCommands;
	bool scriptFileRead = false;
	if (scriptFile != "")
	{
		std::ifstream scriptStream(scriptFile.c_str());
		if (!scriptStream.good())
		{
			printf("Cannot open script file: %s. Using default Festival commands instead.\n", scriptFile.c_str()); 
		}
		else
		{
			char line[4096];
			while(!scriptStream.eof() && scriptStream.good())
			{
				scriptStream.getline(line, 4096, '\n');
				festivalCommands.emplace_back(line);
			}
			scriptStream.close();
			scriptFileRead = true;
		}
	}

	printf("Festival lib directory (use -festivalLibDir): %s\n", festivalLibDir.c_str());
	printf("Cache directory (use -cacheDir)             : %s\n", cache_directory.c_str());
	printf("Voice (use -voice)                          : %s\n", voice.c_str());
	printf("Script (use -script)                        : %s\n", scriptFile.c_str());

	if (!scriptFileRead)
	{
		printf("Running default Festival commands - %s\n\n", voice.c_str());
		// setting voice to be used
		std::string voice_command = "(" + voice + ")";
		festivalCommands.emplace_back(voice_command.c_str());
		// setting the duration method to be used by festival
		festivalCommands.emplace_back("(Parameter.set `Duration_Method Duration_Default)");
		// this command hooks our virtual human method such that every time an utterance is synthesized, our method is called on it
		// in order to generate the virtual human message (RemoteSpeechReply)
		festivalCommands.emplace_back("(set! after_synth_hooks (list Duration_VirtualHuman))");
		 
	}

	printf("\n");
	for (size_t x = 0; x < festivalCommands.size(); x++)
	{
		printf("%s\n", festivalCommands[x].c_str());
		festival_eval_command(festivalCommands[x].c_str());
	}
	printf("\n");

	
	printf( "Starting ActiveMQ\n");
	// initialize activeMQ
	bool success = open_activeMQ();
	if(!success)
	{
		cout<<"Could not initialize ActiveMQ";
		return 1;
	}

	printf( "Checking for Cache Directory\n");
	// check to see if cache directory exists and if not create it
	bool cacheDirExists = false;
#ifdef WIN32
	if( (_access( cache_directory.c_str(), 0 ) == 0 ) )
	{
		cacheDirExists = true;
	}
#else
	struct stat statbuf;
	if (stat(cache_directory.c_str(), &statbuf) != -1)
	{
		if (S_ISDIR(statbuf.st_mode))
		{
			cacheDirExists = true;
		}
	}
#endif
	if (!cacheDirExists)
    	{
		std::string temp = "";
		std::vector< std::string > tokens;
		const std::string delimiters = "\\/";

		vhcl::Tokenize( cache_directory.c_str(), tokens, delimiters );

		printf( "Warning, audio cache directory, %s, does not exist. Creating directory...\n", cache_directory.c_str() );
		for (unsigned int i = 0; i < tokens.size(); i++)
		{
#ifdef WIN32
		 temp += tokens.at( i ) + "\\";
		 _mkdir( temp.c_str() );
#else
		temp += tokens.at( i ) + "/";
		mkdir(temp.c_str(), 0777);
#endif
		printf("Creating directory '%s'", temp.c_str());
		}
	}

	printf( "Done Initializing\n");

	// poll for messages
	while(!isDone)
	{
		vhmsg::ttu_wait(1.0);
	}

	close_activeMQ();
    return 0;
}
