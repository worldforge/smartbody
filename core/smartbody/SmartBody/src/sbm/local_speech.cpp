/*************************************************************
Copyright (C) 2017 University of Southern California

This file is part of Smartbody.

Smartbody is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Smartbody is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Smartbody.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************/



#define USE_FESTIVAL_RELAY 0
//#define USE_CEREVOICE 1
#ifdef USE_CEREVOICE
#define USE_CEREPROC_RELAY 1
#else
#define USE_CEREPROC_RELAY 0
#endif

#include "local_speech.h"
#ifdef WIN32
#include <direct.h>
#define getFullPath(result,filename) _fullpath(result,filename,_MAX_PATH)
#define checkPermission(pathname,mode) _access(pathname,mode)
#define makeDirectory(pathname) _mkdir(pathname)
#else
#include <unistd.h>
#define _MAX_PATH 3000
#define getFullPath(result,filename) realpath(filename,result)
#define checkPermission(pathname,mode) access(pathname,mode)
#define makeDirectory(pathname) mkdir(pathname,0777)
#endif

#if USE_FESTIVAL_RELAY
#include <festival.h>
#include <VHDuration.h>
#endif

#if USE_CEREPROC_RELAY
#include <cerevoice_eng.h>
#include <cerevoice_eng_int.h>
#endif

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <sys/types.h>
#if 0
#include <sys/timeb.h>
#endif
#include <sstream>
#include <float.h>
#include "time.h"

#include "sbm/xercesc_utils.hpp"
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include "sbm/BMLDefs.h"
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBCommandManager.h>
#include <sb/SBVHMsgManager.h>


using namespace std;
using namespace SmartBody;
using namespace xml_utils;

XERCES_CPP_NAMESPACE_USE

#if !USE_FESTIVAL_RELAY
struct SpeechRequestMessageData {
	/// List of timing tags, and the corresponding words in the XML message
	/// eg. T0, T1, ... and hello, captain respectively
	/// words don't have any specific usage yet, but may have in the future, if tags are trimmed from their current protocol of 2 per word
	std::vector<std::string> tags, words;
	/// Speech ID (usually sp1)
	std::string speechIdentifier;
};
#endif

void SpeechRelayLocal::setVoiceAndLicenses(const std::vector<std::string>& voiceList, const std::vector<std::string>& licenseList)
{
}

void SpeechRelayLocal::setLicenseInfo(const std::string& voice, const std::string& key, const std::string& value)
{
	std::map<std::string, std::map<std::string, std::string> >::iterator iter = _voiceLicenseInfo.find(voice);
	if (iter == _voiceLicenseInfo.end())
	{
		_voiceLicenseInfo[voice] = std::map<std::string, std::string>();
		iter = _voiceLicenseInfo.find(voice);
	}
	(*iter).second[key] = value;
}

/// Cleans up spurious whitespaces in string, and removes weird \n's
void SpeechRelayLocal::cleanString(std::string &message)
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
	while ( message.find("  ") != std::string::npos )
	{
		pos = message.find(" ");
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

std::string SpeechRelayLocal::removeXMLTagsAndNewLines( const std::string & txt , SpeechRequestMessageData & xmlMetaData)
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

   size_t position = truncatedTxt.find("><");
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

   //SmartBody::util::log("removeXMLTag, message = %s",message);


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
		   DOMNodeList *messageList = root->getElementsByTagName(UTF16("speech"));
		   if ( messageList && messageList->getLength() > 0)
		   {
			   DOMElement *speechElement = dynamic_cast<DOMElement*>(messageList->item(0));
			   xmlMetaData.speechIdentifier = std::string(UTF8(speechElement->getAttribute(UTF16("id"))));
			   //actualText = "<speech id=\"" + std::string(speechID) + "\" ref=\"" + XMLString::transcode(speechElement->getAttribute(UTF16("ref"))) + "\" type=\"" + XMLString::transcode(speechElement->getAttribute(UTF16("type"))) + "\">";
		   }
       else if ( !strcmp( UTF8(root->getNodeName()), "speech") ) {
			   /// Else, the message might contain only the speech tag, in which case the above code will fail, so we need a fallback
			   DOMElement *speechElement = root;
			   xmlMetaData.speechIdentifier = std::string(UTF8(speechElement->getAttribute(UTF16("id"))));
			   //actualText = "<speech id=\"" + std::string(speechID) + "\" ref=\"" + XMLString::transcode(speechElement->getAttribute(UTF16("ref"))) + "\" type=\"" + XMLString::transcode(speechElement->getAttribute(UTF16("type"))) + "\">";
		   }
		   else
		   {
			   /// Oops, for some reason all of the above didn't work, default to the default speech id
			   fprintf(stderr, "Warning: Could not find speech tag in message, creating message beginning by default\n");
			   //actualText = truncatedTxt.substr(0, truncatedTxt.find_first_of(">") + 1);
       }
		   messageList = root->getElementsByTagName( UTF16("mark"));

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
             std::string markString = std::string(UTF8(element->getAttribute(UTF16("name"))));
					   const XMLCh *speech = NULL;
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
						   speech = speechNode->getNodeValue();
					   }
					   /// Get the timing tag as a string
             std::string speechString = speech ? std::string(UTF8(speech)) : " ";
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
							xmlMetaData.tags.push_back(markString);
							xmlMetaData.words.push_back(temporaryText);

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
     doc->release();
   }
   else
   {
	   fprintf(stderr, "Error: XML DOM document is null!\n");
   }

   //actualText += "</speech>\n";
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

std::string SpeechRelayLocal::CreateMarkTimeStamps(std::string text)
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

std::string SpeechRelayLocal::TransformTextWithTimes(std::string txt)
{
#ifdef __APPLE__
	return txt;
#else
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
		   DOMNodeList *messageList = root->getElementsByTagName(UTF16("speech"));
		   if ( messageList && messageList->getLength() > 0)
		   {
			   DOMElement *speechElement = dynamic_cast<DOMElement*>(messageList->item(0));
			   actualText = actualText.append("<speech id=\""
                                        + std::string(UTF8(speechElement->getAttribute(UTF16("id"))))
                                        + "\" ref=\""
                                        + UTF8(speechElement->getAttribute(UTF16("ref")))
                                        + "\" type=\""
                                        + UTF8(speechElement->getAttribute(UTF16("type")))
                                        + "\">\n\n");
		   }
       else if ( !strcmp( UTF8(root->getNodeName()), "speech") ) {
         /// Else, the message might contain only the speech tag, in which case the above code will fail, so we need a fallback
         DOMElement *speechElement = root;
         std::string textContent = CreateMarkTimeStamps(std::string(UTF8(speechElement->getTextContent())));

         actualText = actualText.append("<speech id=\""
                                        + std::string("sp1")
                                        + "\" ref=\""
                                        + UTF8(speechElement->getAttribute(UTF16("ref")))
                                        + "\" type=\"" + "application/ssml+xml" + "\">\n\n");
         actualText = actualText.append(textContent);
         actualText = actualText.append("</speech>");
       }
	   }
   }
   return actualText;
#endif
}

#if USE_CEREPROC_RELAY

CereprocSpeechRelayLocal::CereprocSpeechRelayLocal()
{

}

CereprocSpeechRelayLocal::~CereprocSpeechRelayLocal()
{

}

void CereprocSpeechRelayLocal::setVoiceAndLicenses(const std::vector<std::string>& voicelist, const std::vector<std::string>& licenseList)
{
	for (size_t v = 0; v < voicelist.size(); v++)
		voices.push_back(voicelist[v]);

	for (size_t l = 0; l < licenseList.size(); l++)
		licenses.push_back(licenseList[l]);
}


void CereprocSpeechRelayLocal::initSpeechRelay( std::string libPath, std::string cacheDir )
{	
	SmartBody::util::log("start init cerevoice relay");
	voiceEngine = CPRCEN_engine_new();

	for (int i=0;i<voices.size();i++)	
	{
		std::string fullVoiceName = libPath + "/" + voices[i];
		std::string fullLicenseName = libPath + "/" + licenses[i];
		int success = CPRCEN_engine_load_voice(voiceEngine, fullLicenseName.c_str(), NULL, fullVoiceName.c_str(),CPRC_VOICE_LOAD_EMB_AUDIO);
		if (!success)
			SmartBody::util::log("Cerevoice Local Relay : load voice %s fail.",fullVoiceName.c_str());
		else
			SmartBody::util::log("Cerevoice Local Relay : load voice %s success.",fullVoiceName.c_str());
	}

	// if there are any voices that needed to be loaded directly from the license information:
	for (std::map<std::string, std::map<std::string, std::string> >::iterator iter = _voiceLicenseInfo.begin();
		 iter != _voiceLicenseInfo.end();
		 iter++)
	{
		std::string voice = (*iter).first;
		std::string licenseText = "";
		std::string signature = "";
		std::string voiceFile = "";
		
		for (std::map<std::string, std::string>::iterator iter2 = (*iter).second.begin();
			 iter2 != (*iter).second.end();
			 iter2++)
		{
			if ((*iter2).first == "licenseText")
				licenseText = (*iter2).second;
			if ((*iter2).first == "signature")
				signature = (*iter2).second;
			if ((*iter2).first == "voiceFile")
				voiceFile = (*iter2).second;
		}
		/*
		SmartBody::util::log("LOADING VOICE %s WITH:", voice.c_str());
		SmartBody::util::log("(%s)", licenseText.c_str());
		SmartBody::util::log("(%s)", signature.c_str());
		SmartBody::util::log("(%s)", voiceFile.c_str());
		*/
		int success = CPRCEN_engine_load_voice_licensestr(voiceEngine, licenseText.c_str(), signature.c_str(), "", voiceFile.c_str(), CPRC_VOICE_LOAD_EMB_AUDIO);
		if (!success)
			SmartBody::util::log("Cerevoice Local Relay : load voice %s with direct engine licensing fail.", voice.c_str());
		else
			SmartBody::util::log("Cerevoice Local Relay : load voice %s with direct engine licensing success.", voice.c_str());


	}
	
	if (!voiceEngine)
	{
		SmartBody::util::log("fail to initialize Cerevoice engine");
	}
	else
		SmartBody::util::log("Success to initialize Cerevoice engine");
	cacheDirectory = cacheDir;
	set_phonemes_to_visemes();
}


std::string CereprocSpeechRelayLocal::textToSpeech( const char * text, const char * cereproc_file_name, std::string voice_id )
{
	std::string reply = "";
	//CPRCEN_channel_handle chan = CPRCEN_engine_open_default_channel(voiceEngine); /* Create channel */
	CPRCEN_channel_handle chan = CPRCEN_engine_open_channel(voiceEngine, "", "", voice_id.c_str(), "");
	char* fileName = const_cast<char*>(cereproc_file_name);
	CPRCEN_engine_channel_to_file(voiceEngine, chan, fileName, CPRCEN_RIFF); /* File output on channel */
	// 	/* Speak with streaming input */
	//SmartBody::util::log("text to speech, voice_id = %s",voice_id.c_str());

	SpeechRequestMessageData xmlMetaData;
	// Feed in input text, further data is to come
	//Added by Apar Suri
	//Replacing . and , with "" so because there seems to be a bug in CPRCPMOD_spurt_synth while synthesizing multiple spurts
	//Apparently a time of 0.0 and 0.1 seems to be added just after the , or .
	std::string text_string = removeXMLTagsAndNewLines( text, xmlMetaData);

	CPRC_abuf* abuf = CPRCEN_engine_channel_speak(voiceEngine, chan, text_string.c_str(), strlen(text_string.c_str()),1);

	if (xmlMetaData.tags.size() <= 0 )
	{
		//text_string = text;
		text_string =  TransformTextWithTimes(text);
		text_string = removeXMLTagsAndNewLines(text_string, xmlMetaData);
	}


	 int errorCode = 0;


      DOMImplementation * impl =  DOMImplementationRegistry::getDOMImplementation( UTF16( "Core" ) );

      if ( impl != NULL )
      {
         try
         {
            DOMDocument* doc = impl->createDocument(
               0,                    // root element namespace URI.
               UTF16( "speak" ),     // root element name
               0 );                  // document type object (DTD).

            DOMElement * rootElem = doc->getDocumentElement();

            DOMElement * soundFileElement = doc->createElement( UTF16( "soundFile" ) );
            soundFileElement->setAttribute( UTF16("name"), UTF16(cereproc_file_name) );
            rootElem->appendChild( soundFileElement );

            DOMElement * wordElement = doc->createElement( UTF16( "word" ) );

            int num_words = 0;

            for ( int i = 0; i < abuf->trans_sz; i++ )
            {
               if ( abuf->trans[ i ].type == CPRC_ABUF_TRANS_PHONE )
               {
                 std::map<std::string, std::string>::iterator iter = phonemeToViseme.find( abuf->trans[ i ].name );

                 //check added to avoid crashing on entries which are not defined
                 if ( iter != phonemeToViseme.end() )
                 {

                    std::string end_f = SmartBody::util::format( "%0.6f", abuf->trans[i].end );

                    std::string start_f = SmartBody::util::format( "%0.6f", abuf->trans[i].start );

                    DOMElement * visemeElement = doc->createElement( UTF16( "viseme" ) );
                    visemeElement->setAttribute( UTF16("start"), UTF16(start_f.c_str()) );
                    visemeElement->setAttribute( UTF16("type"), UTF16(iter->second.c_str()) );

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
                           wordElement->setAttribute( UTF16("end"), UTF16(end_f.c_str()) );

                           if ( wordElement->hasChildNodes() )
                              rootElem->appendChild( wordElement );
   
                           wordElement = doc->createElement( UTF16( "word" ) );

                           std::string word_start_f = SmartBody::util::format( "%0.6f", abuf->trans[ i + 1 ].start );
//                           XMLCh * word_start_time = XMLString::transcode( word_start_f.c_str() );
                           wordElement->setAttribute( UTF16("start"), UTF16(word_start_f.c_str()) );

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

                           DOMElement * markElement = doc->createElement( UTF16( "mark" ) );
//                           XMLCh * mark_name = XMLString::transcode( s.c_str() );
                           markElement->setAttribute( UTF16("name"), UTF16(s.c_str()) ) ;
                           markElement->setAttribute( UTF16("time"), UTF16(word_start_f.c_str()) );
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
                           DOMElement * markElement2 = doc->createElement( UTF16( "mark" ) );
//                           XMLCh * mark_name2 = XMLString::transcode( s.c_str() );
                           markElement2->setAttribute( UTF16("name"), UTF16(s.c_str()) ) ;
                           markElement2->setAttribute( UTF16("time"), UTF16(end_f.c_str()) );
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
                    DOMElement * markElement = doc->createElement( UTF16( "mark" ) );
                    XMLCh * mark_name = XMLString::transcode( abuf->trans[ i ].name );
                    markElement->setAttribute( name, mark_name ) ;

                    std::string word_start_f = SmartBody::util::format( "%0.6f", abuf->trans[ i ].start );
                    XMLCh * word_start_time = XMLString::transcode( word_start_f.c_str() );
                    markElement->setAttribute( time, word_start_time );

                    rootElem->appendChild( markElement );
               }*/
            }

            DOMLSSerializer* theSerializer = DOMImplementation::getImplementation()->createLSSerializer();
            XMLCh * xml_result = theSerializer->writeToString( rootElem );
            reply = std::string(UTF8(xml_result));
            XMLString::release(&xml_result);
            theSerializer->release();
         }
//          catch ( const OutOfMemoryException & )
//          {
//             XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
//             errorCode = 5; 
//          }
         catch ( const DOMException & e )
         {
            XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
            errorCode = 2;
         }
         catch ( const XMLException & toCatch )
         {
            printf( "XMLException occurred: %d", toCatch.getCode() );
            errorCode = 4;
         }
         catch (...)
         {
            XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
            errorCode = 3;
         }
      }


	

	CPRCEN_engine_channel_close(voiceEngine,chan);
	return reply;
}

// from http://stackoverflow.com/questions/13739924/remove-all-xml-tags-from-a-stdstring
std::string ParseXMLOutput(std::string &xmlBuffer)
{
    bool copy = true;
    std::string plainString = "";   
    std::stringstream convertStream;

    // remove all xml tags
    for (int i=0; i < xmlBuffer.length(); i++)
    {                   
        convertStream << xmlBuffer[i];

        if(convertStream.str().compare("<") == 0) copy = false;
        else if(convertStream.str().compare(">") == 0) 
        {
            copy = true;
            convertStream.str(std::string());
            continue;
        }

        if(copy) plainString.append(convertStream.str());       

        convertStream.str(std::string());
    }
	boost::replace_all(plainString, "\n", " ");
	boost::replace_all(plainString, "\r", " ");
	boost::replace_all(plainString, "\t", " ");
	boost::trim(plainString);
	boost::regex_replace(plainString, boost::regex("[' ']{2,}"), " ");
    return plainString;
}


void CereprocSpeechRelayLocal::processSpeechMessage( const char * message )
{
// 	CPRCEN_channel_handle chan = CPRCEN_engine_open_default_channel(voiceEngine); /* Create channel */
// 	CPRCEN_engine_channel_to_file(voiceEngine, chan, "out.wav", CPRCEN_RIFF); /* File output on channel */
// 	/* Speak with streaming input */
// 	CPRCEN_engine_channel_speak(voiceEngine, chan, txt1, strlen(txt1), 0);

	std::string message_c = message;

	//SmartBody::util::log("Cereproc process speech message = '%s'",message);

   // parse the string
   std::vector< std::string > tokens;
   const std::string delimiters = " ";
   SmartBody::util::tokenize( message_c, tokens, delimiters );

   /// Get non-XML components of the message
   std::string command = tokens.at( 0 );

   std::string agent_name = tokens.at( 1 );

   // if the agent doesn't exist locally, don't create a voice for it
   SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(agent_name);
   if (!character)
   {
	   SmartBody::util::log("Character '%s' does not exist in this context. Speech will not be generated for it.", agent_name.c_str());
	   return;
   }

   std::string message_id = tokens.at( 2 );
   std::string voice_id = tokens.at( 3 );
   std::string file_name = tokens.at( 4 );
   size_t prefix_length = message_c.find( file_name, 0 ) + file_name.length() + 1;
   std::string utterance = message_c.substr( prefix_length );  // strip off the prefix, only using the xml

   //SmartBody::util::log("voice_id = %s", voice_id.c_str());
   // remove anything after </speech> tag
   size_t postfix_pos = utterance.rfind( "</speech>" );
   if ( postfix_pos != std::string::npos )
      utterance = utterance.substr( 0, postfix_pos + 9 );
   //SmartBody::util::log("utterane = %s", utterance.c_str());
   //std::string newUtterance = ParseXMLOutput(utterance);
   //SmartBody::util::log("strippedUtterance = %s", newUtterance.c_str());
   //utterance = newUtterance;

   // parse out just the sound file name and give it a .wav file type
   int pos = file_name.find( ".aiff" );
   int pos2 = file_name.find( "utt" );   
   file_name = file_name.substr( pos2, pos - pos2 ) + ".wav";

   string cereproc_file_name = cacheDirectory + file_name;

   /// Generate the audio
   std::string xml = textToSpeech(utterance.c_str(), cereproc_file_name.c_str(), voice_id);
   // Only send out a reply when result is not empty, ignore otherwise as a nother voice relay might pick up the request
   //SmartBody::util::log("Cerevoice reply Cmd = %s",xml.c_str());
   if ( xml.compare("") != 0 )
   {
      std::string reply = agent_name;
      reply += " ";
      reply += message_id;
      reply += " OK: <?xml version=\"1.0\" encoding=\"UTF-8\"?>";
      reply += xml;
   
      //SmartBody::util::log( "REPLY: %s\n", reply.c_str() );

#ifdef _DUMP_COMM_TO_DISK
	  fprintf(_outfile, "%d: %s\n\n",_dumpCounter - 1,reply.c_str());
	  std::string dumpXMLstring = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" + xml;
	  fprintf(_outXML, "%d: %s\n\n", _dumpCounter - 1, dumpXMLstring.c_str() );
#endif
	  
	  string replyCmd = "RemoteSpeechReply ";
	  replyCmd = replyCmd + reply; //cmdConst;
	  //SmartBody::util::log("replyCmd = %s", replyCmd.c_str());
	  SmartBody::SBScene::getScene()->command(replyCmd);
	  //mcu.execute_later(replyCmd.c_str());
      //vhmsg::ttu_notify2( "RemoteSpeechReply", reply.c_str() );
   }
}

void CereprocSpeechRelayLocal::setVoice( std::string voice )
{

}

void CereprocSpeechRelayLocal::set_phonemes_to_visemes()
{
	phonemeToViseme[ "sil" ] = "_";  // SIL
	phonemeToViseme[ "aa" ]  = "Aa"; // AA
	phonemeToViseme[ "ae" ]  = "Ah"; // AE
	phonemeToViseme[ "ah" ]  = "Ah"; // AH
	phonemeToViseme[ "ao" ]  = "Ao"; // AO
	phonemeToViseme[ "ax" ]  = "Ah"; // AX
	phonemeToViseme[ "@" ]  = "Ih"; // Shouldn't happen!
	phonemeToViseme[ "aw" ]  = "Aw"; // AW
	phonemeToViseme[ "ay" ]  = "Ay"; // AY
	phonemeToViseme[ "b" ]   = "BMP";//  B
	phonemeToViseme[ "ch" ]  = "Sh";  // CH
	phonemeToViseme[ "d" ]   = "D";  //  D
	phonemeToViseme[ "dh" ]  = "Th"; // DH
	phonemeToViseme[ "dx" ]  = "D";  // ??
	phonemeToViseme[ "eh" ]  = "Eh"; // EH
	phonemeToViseme[ "er" ]  = "Er"; // ER
	phonemeToViseme[ "ey" ]  = "Eh"; // ?? probably EY
	phonemeToViseme[ "f" ]   = "F";  //  F
	phonemeToViseme[ "g" ]   = "Kg"; //  G
	phonemeToViseme[ "hh" ]  = "H"; // HH
	phonemeToViseme[ "ih" ]  = "Ih"; // IH
	phonemeToViseme[ "iy" ]  = "Ih"; // IY
	phonemeToViseme[ "jh" ]  = "Sh";  // JH
	phonemeToViseme[ "k" ]   = "Kg"; //  K	
	phonemeToViseme[ "l" ]   = "L";  //  L
	phonemeToViseme[ "m" ]   = "BMP";//  M
	phonemeToViseme[ "n" ]   = "D"; //  N
	phonemeToViseme[ "ng" ]  = "D"; // NG
	phonemeToViseme[ "ow" ]  = "Ow"; // OW
	phonemeToViseme[ "oy" ]  = "Oy"; // OY
	phonemeToViseme[ "p" ]   = "BMP";//  P
	phonemeToViseme[ "r" ]   = "R";  //  R
	phonemeToViseme[ "s" ]   = "Z";  //  S
	phonemeToViseme[ "sh" ]  = "Sh";  // SH
	phonemeToViseme[ "T" ]   = "D";  //  T ?
	phonemeToViseme[ "t" ]   = "D";  //  T ?
	phonemeToViseme[ "th" ]  = "Th"; // TH
	phonemeToViseme[ "uh" ]  = "W"; // UH
	phonemeToViseme[ "uw" ]  = "W"; // UW
	phonemeToViseme[ "v" ]   = "F";  //  V
	phonemeToViseme[ "w" ]   = "W"; //  W
	phonemeToViseme[ "y" ]   = "Sh"; //  Y
	phonemeToViseme[ "z" ]   = "Z";  //  Z
	phonemeToViseme[ "zh" ]  = "Sh";  // ZH	
}
#else
CereprocSpeechRelayLocal::CereprocSpeechRelayLocal()
{

}

CereprocSpeechRelayLocal::~CereprocSpeechRelayLocal()
{

}

void CereprocSpeechRelayLocal::setVoiceAndLicenses(const std::vector<std::string>& voiceList, const std::vector<std::string>& licenseList)
{
}

void CereprocSpeechRelayLocal::initSpeechRelay( std::string libPath, std::string cacheDirectory )
{

}

void CereprocSpeechRelayLocal::processSpeechMessage( const char * message )
{

}

std::string CereprocSpeechRelayLocal::textToSpeech( const char * text, const char * cereproc_file_name, std::string voice_id )
{
	std::string reply = "";

	return reply;
}

void CereprocSpeechRelayLocal::set_phonemes_to_visemes()
{

}

void CereprocSpeechRelayLocal::setVoice( std::string voice )
{

}
#endif

#if USE_FESTIVAL_RELAY
extern SpeechRequestData xmlMetaData;
extern std::map<string,string> phonemeToViseme;
extern std::string mapping;

FestivalSpeechRelayLocal::FestivalSpeechRelayLocal()
{
	
}

FestivalSpeechRelayLocal::~FestivalSpeechRelayLocal()
{
}

void FestivalSpeechRelayLocal::setVoiceAndLicenses(const std::vector<std::string>& voiceList, const std::vector<std::string>& licenseList)
{
}

std::string FestivalSpeechRelayLocal::generateReply(const char * utterance,const char * soundFileName)
{
	EST_Wave wave;

	//string spoken_text = storeXMLMetaData( utterance );
	string spoken_text = removeXMLTagsAndNewLines( utterance ,xmlMetaData);
	//SmartBody::util::log("after store XMLMetaData");
	//printf("done first time\n");
	if (!spoken_text.compare("") && spoken_text != "")
	{
		puts(spoken_text.append("\n").c_str());
	}
	
	SmartBody::util::log("after spoken text compare, spoken_text = %s",spoken_text.c_str());
	if (xmlMetaData.tags.size() <= 0)
	{
		spoken_text = TransformTextWithTimes(utterance);
		SmartBody::util::log("done transforming, spoken_text = %s\n",spoken_text.c_str());
		if (!spoken_text.compare("") && spoken_text != "")
		{
			puts(spoken_text.append("\n").c_str());
		}
		spoken_text = removeXMLTagsAndNewLines(spoken_text, xmlMetaData);
		//spoken_text = storeXMLMetaData( spoken_text );
		SmartBody::util::log("done second time, spoken_text = %s\n", spoken_text.c_str());
	}
	//SmartBody::util::log("before remove tab");
	removeTabsFromString(spoken_text);

	//SmartBody::util::log( "generateReply() - \nbefore: '%s'\nafter: '%s'\n'%s'\n", utterance, spoken_text.c_str(), soundFileName );

	//festival_say_text(spoken_text.c_str());
	festival_text_to_wave(spoken_text.c_str(),wave);
    wave.save(soundFileName,"riff");

	return xmlMetaData.replyString;
}

void FestivalSpeechRelayLocal::removeTabsFromString(string &spoken_text)
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
std::string FestivalSpeechRelayLocal::storeXMLMetaData( const std::string & txt)
{
	/// Put in some defaults, and do basic cleanup just to be safe
   xmlMetaData.speechIdentifier = "sp1";
   xmlMetaData.tags.clear();
   xmlMetaData.words.clear();
   /// Start an XML parser to parse the message we have received
   //xercesc_3_0::XMLPlatformUtils::Initialize();
   //xercesc_3_0::XercesDOMParser *parser = new XercesDOMParser(); 
   XMLPlatformUtils::Initialize();
   XercesDOMParser *parser = new XercesDOMParser(); 

   std::string truncatedTxt = txt.substr(txt.find_first_of(">")+1);
   char * message = (char*)truncatedTxt.c_str();

   std::string actualText = "";
   //SmartBody::util::log("message = %s",message);
   /// Set up a parser for XML message in memory - code sourced from unknown online reference for Xerces XML library
   //xercesc_3_0::MemBufInputSource memIS((const XMLByte*)message, strlen(message), "XMLBuffer");
   MemBufInputSource memIS((const XMLByte*)message, strlen(message), "XMLBuffer");
   parser->parse(memIS);
   DOMDocument *doc = parser->getDocument();
   if ( doc )
   {
	   //SmartBody::util::log("has doc");
	   DOMElement *root = doc->getDocumentElement();

	   if ( root ) 
	   {
		   //SmartBody::util::log("has root");
		   /// Get all nodes which have the "mark" tag, from these we can extract the timing tags, and speech text
		   DOMNodeList *messageList = root->getElementsByTagName(UTF16("speech"));
		   if ( messageList && messageList->getLength() > 0)
		   {
			   DOMElement *speechElement = dynamic_cast<DOMElement*>(messageList->item(0));
			   xmlMetaData.speechIdentifier = std::string(UTF8(speechElement->getAttribute(UTF16("id"))));
		   }
		   else if ( !strcmp( UTF8( root->getNodeName() ), "speech") ) {
			   /// Else, the message might contain only the speech tag, in which case the above code will fail, so we need a fallback
			   DOMElement *speechElement = root;
			   xmlMetaData.speechIdentifier = std::string(UTF8(speechElement->getAttribute(UTF16("id"))));
		   }
		   else
		   {
			   /// Oops, for some reason all of the above didn't work, default to the default speech id
			   fprintf(stderr, "Warning: Could not find speech tag in message, creating message beginning by default\n");
		   }
		   messageList = root->getElementsByTagName( UTF16("mark"));
		   //SmartBody::util::log("check messageList = %d",messageList);
		   /// Store all the book marks in the input message, so that they can be retrieved later
		   if ( messageList ) 
		   {
			   XMLSize_t nLetters = messageList->getLength();
			   //SmartBody::util::log("has messageList, length = %d",nLetters);
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
            theSerializer2->release();

						// print the final bml code to the terminal
						string output =
						(char*)  ((MemBufFormatTarget*)myFormatTarget2)->getRawBuffer();		
					   

             std::string markString = std::string(UTF8(element->getAttribute(UTF16("name"))));
					   const XMLCh *speech = NULL;
					   DOMNode *speechNode = element->getFirstChild();
					   


					   if ( speechNode == NULL ) 
					   {
						   speechNode = element->getNextSibling();		
					   }
					   if ( (speechNode !=NULL) && ( speechNode->getNodeType() == DOMNode::TEXT_NODE ) )
					   {
						   speech = speechNode->getNodeValue();
					   }					   					   



					   /// Get the timing tag as a string
						
					   std::string speechString;

					   if(speechNode !=NULL)
					   {
               speechString = speech ? std::string(UTF8(speech)) : " ";
					   }
					   else
						   speechString = "";
						   
					   if( !strcmp(markString.c_str(),"") || !strcmp(speechString.c_str(),"") )
					   {
						   /// Null strings tend to cause problems with later code sections
						   if ( !strcmp(speechString.c_str(),"") ) speechString = " ";
					   }
					   else
					   {
						   std::string temporaryText = speechString;
						   //SmartBody::util::log("tempText = %s",temporaryText.c_str());
						   cleanString(temporaryText);
						   //SmartBody::util::log("tempText after clean = %s",temporaryText.c_str());
						   /// Words can be used to match up which tags we need to see
							/// Push tag and word into xmlMetaData
							xmlMetaData.tags.push_back(markString);
							xmlMetaData.words.push_back(temporaryText);
							actualText += temporaryText;
							//SmartBody::util::log("actual text before clean = %s",actualText.c_str());
							cleanString(actualText);
							//SmartBody::util::log("actual text after clean = %s",actualText.c_str());
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

void FestivalSpeechRelayLocal::processSpeechMessage( const char * message )
{
	std::string message_c = message;

	// parse the string
	std::vector< std::string > tokens;
	const std::string delimiters = " ";
	SmartBody::util::tokenize( message_c, tokens, delimiters );

	/// Get non-XML components of the message
	std::string command = tokens.at( 0 );

	std::string agent_name = tokens.at( 1 );
   // if the agent doesn't exist locally, don't create a voice for it
   SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(agent_name):
   if (!character)
   {
	   SmartBody::util::log("Character '%s' does not exist in this context. Speech will not be generated for it.", agent_name.c_str());
	   return;
   }
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
	std::string festival_file_name = cacheDirectory + file_name;
	//Generate the audio
	//SmartBody::util::log("before reply");
	string replyXML = generateReply(utterance.c_str(),festival_file_name.c_str());
	//SmartBody::util::log("after reply, replyXML = %s",replyXML.c_str());
	string remoteSpeechReply = agent_name+" "+message_id+" OK: <?xml version=\"1.0\" encoding=\"UTF-8\"?><speak><soundFile name=\"";
	//SmartBody::util::log("remoteSpeechReply = %s",remoteSpeechReply.c_str());

	char full[ _MAX_PATH ];
	if ( getFullPath( full, const_cast<char*>(festival_file_name.c_str())) == NULL )
	{
		SmartBody::util::log("\nError converting path sent from SBM to absolute path\n");
	}
#if defined(__ANDROID__)
	string soundPathName = cacheDirectory + file_name;
#else
    string soundPathName = full;
#endif

	remoteSpeechReply += soundPathName+"\"/>";
	remoteSpeechReply += replyXML + "</speak>";
	//SmartBody::util::log("replyXML = %s\n",replyXML.c_str());
	//SmartBody::util::log("Sound path name = %s\n",soundPathName.c_str());


	
	char* cmdConst = const_cast<char*>(remoteSpeechReply.c_str());
	string replyCmd = "RemoteSpeechReply ";
	replyCmd = replyCmd + remoteSpeechReply; //cmdConst;
	//mcu.execute_later("RemoteSpeechReply", cmdConst ); //sends the remote speech command using singleton* MCU_p	
	//SmartBody::util::log("replyCmd = %s",replyCmd.c_str());
	mcu.execute_later(replyCmd.c_str());
}

// void FestivalSpeechRelayLocal::evalFestivalCommand( const char * cmd )
// {
// 	int ret = festival_eval_command(cmd);
// 	SmartBody::util::log("%s : ret = %d\n", cmd,ret);	
// }

void FestivalSpeechRelayLocal::setVoice(std::string voice)
{
	std::stringstream strstr;
	strstr << "(voice_" << voice << ")";
	int ret = festival_eval_command(strstr.str().c_str());
	//SmartBody::util::log("Voice = %s : ret = %d\n", voice.c_str(),ret);	

	festival_eval_command("(Parameter.set `Duration_Method Duration_Default)");		
	festival_eval_command("(set! after_synth_hooks (list Duration_VirtualHuman))");	
	festival_eval_command("(Parameter.set 'Duration_Stretch 0.8)");
}

void FestivalSpeechRelayLocal::set_phonemes_to_visemes()
{

	if(mapping == "sbmold")
	{
		phonemeToViseme[ "pau" ] = "_";  // SIL
		phonemeToViseme[ "aa" ]  = "Ao"; // AA
		phonemeToViseme[ "ae" ]  = "Ih"; // AE
		phonemeToViseme[ "ah" ]  = "Ih"; // AH
		phonemeToViseme[ "ao" ]  = "Ao"; // AO
		phonemeToViseme[ "ax" ]  = "Ih"; // AX
		phonemeToViseme[ "@" ]   = "Ih"; // 
		phonemeToViseme[ "aw" ]  = "Ih"; // AW
		phonemeToViseme[ "ay" ]  = "Ih"; // AY
		phonemeToViseme[ "b" ]   = "BMP";//  B
		phonemeToViseme[ "ch" ]  = "j";  // CH
		phonemeToViseme[ "d" ]   = "D";  //  D
		phonemeToViseme[ "dh" ]  = "Th"; // DH
		phonemeToViseme[ "dx" ]  = "D";  // ??
		phonemeToViseme[ "eh" ]  = "Ih"; // EH
		phonemeToViseme[ "er" ]  = "Er"; // ER
		phonemeToViseme[ "ey" ]  = "Ih"; // 
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
		phonemeToViseme[ "T" ]   = "D";  // T?
		phonemeToViseme[ "t" ]   = "D";  // T?
		phonemeToViseme[ "th" ]  = "Th"; // TH
		phonemeToViseme[ "uh" ]  = "Oh"; // UH
		phonemeToViseme[ "uw" ]  = "Oh"; // UW
		phonemeToViseme[ "v" ]   = "F";  //  V
		phonemeToViseme[ "w" ]   = "OO"; //  W
		phonemeToViseme[ "y" ]   = "OO"; //  Y
		phonemeToViseme[ "z" ]   = "Z";  //  Z
		phonemeToViseme[ "zh" ]  = "J";  // ZH
	}
	else if (mapping == "sbm")
	{
		phonemeToViseme[ "pau" ] = "_";  // SIL
		phonemeToViseme[ "aa" ]  = "Aa"; // AA
		phonemeToViseme[ "ae" ]  = "Ah"; // AE
		phonemeToViseme[ "ah" ]  = "Ah"; // AH
		phonemeToViseme[ "ao" ]  = "Ao"; // AO
		phonemeToViseme[ "ax" ]  = "Ah"; // AX
		phonemeToViseme[ "@" ]   = "Ih"; // ??
		phonemeToViseme[ "aw" ]  = "Ah"; // AW
		phonemeToViseme[ "ay" ]  = "Ay"; // AY
		phonemeToViseme[ "b" ]   = "BMP";//  B
		phonemeToViseme[ "ch" ]  = "Sh";  // CH
		phonemeToViseme[ "d" ]   = "D";  //  D
		phonemeToViseme[ "dh" ]  = "Th"; // DH
		phonemeToViseme[ "dx" ]  = "D";  // ??
		phonemeToViseme[ "eh" ]  = "Eh"; // EH
		phonemeToViseme[ "er" ]  = "Er"; // ER
		phonemeToViseme[ "ey" ]  = "Eh"; // 
		phonemeToViseme[ "f" ]   = "F";  //  F
		phonemeToViseme[ "g" ]   = "Kg"; //  G
		phonemeToViseme[ "hh" ]  = "Ih"; // HH
		phonemeToViseme[ "ih" ]  = "Ih"; // IH
		phonemeToViseme[ "iy" ]  = "Ih"; // IY
		phonemeToViseme[ "jh" ]  = "Sh";  // JH
		phonemeToViseme[ "k" ]   = "Kg"; //  K
		phonemeToViseme[ "l" ]   = "L";  //  L
		phonemeToViseme[ "m" ]   = "BMP";//  M
		phonemeToViseme[ "n" ]   = "Kg"; //  N
		phonemeToViseme[ "ng" ]  = "Kg"; // NG
		phonemeToViseme[ "ow" ]  = "Ow"; // OW
		phonemeToViseme[ "oy" ]  = "Oy"; // OY
		phonemeToViseme[ "p" ]   = "BMP";//  P
		phonemeToViseme[ "r" ]   = "R";  //  R
		phonemeToViseme[ "s" ]   = "Z";  //  S
		phonemeToViseme[ "sh" ]  = "Sh";  // SH
		phonemeToViseme[ "T" ]   = "D";  // T?
		phonemeToViseme[ "t" ]   = "D";  // T?
		phonemeToViseme[ "th" ]  = "Th"; // TH
		phonemeToViseme[ "uh" ]  = "Eh"; // UH
		phonemeToViseme[ "uw" ]  = "Oh"; // UW
		phonemeToViseme[ "v" ]   = "F";  //  V
		phonemeToViseme[ "w" ]   = "W"; //  W
		phonemeToViseme[ "y" ]   = "Ih"; //  Y
		phonemeToViseme[ "z" ]   = "Z";  //  Z
		phonemeToViseme[ "zh" ]  = "Sh";  // ZH
	}
}

void FestivalSpeechRelayLocal::initSpeechRelay(std::string libPath, std::string cacheDir)
{
	
        //freopen ("/sdcard/sbm/festivalLog.txt","w",stderr);

	mapping = "sbm";
	set_phonemes_to_visemes();

	std::string scriptFile = "";
	std::string voice = "voice_kal_diphone";
	//std::string voice = "voice_roger_hts2010";
	festivalLibDirectory = libPath;
	cacheDirectory = cacheDir;
	std::string festivalLibDir = libPath;
	std::string cache_directory = cacheDir;
	
	festival_libdir = festivalLibDir.c_str();
    int heap_size = FESTIVAL_HEAP_SIZE;  
	int load_init_files = 1; // we want the festival init files loaded
    festival_initialize(load_init_files,heap_size);

	SmartBody::util::log( "Festival Text To Speech Engine:\n\n" );
	SmartBody::util::log( "Initializing....\n");
	SmartBody::util::log( "Hooking up VH Module\n");

	std::vector<std::string> festivalCommands;
	std::ifstream scriptStream(scriptFile.c_str());
	
	SmartBody::util::log("Festival lib directory (use -festivalLibDir): %s\n", festivalLibDir.c_str());
	SmartBody::util::log("Cache directory (use -festivalLibDir)       : %s\n", cache_directory.c_str());
	SmartBody::util::log("Voice (use -voice)                          : %s\n", voice.c_str());

	//if (!scriptFileRead)
	{
		SmartBody::util::log("Running default Festival commands\n\n", voice.c_str());
		//festivalCommands.push_back("(voice_roger_hts2010)");
		// setting the duration method to be used by festival
		festivalCommands.push_back("(Parameter.set `Duration_Method Duration_Default)");
		// this command hooks our virtual human method such that every time an utterance is synthesized, our method is called on it
		// in order to generate the virtual human message (RemoteSpeechReply)
		festivalCommands.push_back("(set! after_synth_hooks (list Duration_VirtualHuman))");
		// setting duration stretch parameter
		festivalCommands.push_back("(Parameter.set 'Duration_Stretch 0.8)");
		std::stringstream strstr;
		strstr << "(set! voice_default '" << voice << ")";
		festivalCommands.push_back(strstr.str());
	}

	SmartBody::util::log("\n");
	for (size_t x = 0; x < festivalCommands.size(); x++)
	{		
		int ret = festival_eval_command(festivalCommands[x].c_str());
		SmartBody::util::log("%s : ret = %d\n", festivalCommands[x].c_str(),ret);		
	}
	SmartBody::util::log("\n");	
	


	printf( "Checking for Cache Directory\n");
	// check to see if cache directory exists and if not create it
	if( !(checkPermission( cache_directory.c_str(), 0 ) == 0 ) )
    {
		std::string temp = "";
		std::vector< std::string > tokens;
		const std::string delimiters = "\\/";
		SmartBody::util::tokenize( cache_directory.c_str(), tokens, delimiters );
		printf( "Warning, audio cache directory, %s, does not exist. Creating directory...\n", cache_directory.c_str() );
		for (unsigned int i = 0; i < tokens.size(); i++)
		{
		 temp += tokens.at( i ) + "/";
		 makeDirectory( temp.c_str() );
		}
	}
	SmartBody::util::log( "Done Initializing local speech relay\n");	    
}
#else

FestivalSpeechRelayLocal::FestivalSpeechRelayLocal()
{
	
}

FestivalSpeechRelayLocal::~FestivalSpeechRelayLocal()
{
}

void FestivalSpeechRelayLocal::setVoiceAndLicenses(const std::vector<std::string>& voiceList, const std::vector<std::string>& licenseList)
{
}

void FestivalSpeechRelayLocal::setVoice(std::string voice)
{
}


std::string FestivalSpeechRelayLocal::generateReply(const char * utterance,const char * soundFileName)
{
   std::string actualText;
   return actualText;	
}

void FestivalSpeechRelayLocal::removeTabsFromString(string &spoken_text)
{
	
}
std::string FestivalSpeechRelayLocal::storeXMLMetaData( const std::string & txt)
{
	std::string actualText;
    return actualText;	
}

void FestivalSpeechRelayLocal::processSpeechMessage( const char * message )
{
	
}

void FestivalSpeechRelayLocal::initSpeechRelay(std::string libPath, std::string cacheDir)
{
	 
}
#endif
/* Local Speech Class */
#define FLOAT_EQ(x,v) (((v - DBL_EPSILON) < x) && (x <( v + DBL_EPSILON)))
#define LOG_RHETORIC_SPEECH (0)
#define USE_CURVES_FOR_VISEMES 0

local_speech::local_speech( float timeOutInSeconds )
:	remote_speech(timeOutInSeconds) // default is 10 seconds
{}

local_speech::~local_speech()
{}

void local_speech::sendSpeechCommand(const char* cmd)
{
	//SmartBody::util::log("speech cmd = %s",cmd);
	//SmartBody::util::log("local_speech::sendSpeechCommand");
	char* cmdConst = const_cast<char*>(cmd);
	//SmartBody::SBScene::getScene()->getCommandManager()->execute_later( cmdConst ); //sends the remote speech command using singleton* MCU_p
	SBScene::getScene()->getVHMsgManager()->send2( "RemoteSpeechCmd", cmdConst );
}

