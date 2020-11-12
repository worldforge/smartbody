
#include "vhcl.h"

#include "RvoiceDLL.h"

#include <hash_map>

#include <windows.h>
#include <winsock2.h>

#include "rapi.h"

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


using std::string;
using std::vector;
using std::make_pair;
using stdext::hash_map;


int g_rvoiceHandle = 0;

string g_outputFilename;
string g_bookmarkIntermediate;
int g_waveSize = 0;
double g_curTime = 0;
double g_startWordTime = 0;

vector<double> g_wordBreakListStart;
vector<double> g_wordBreakListEnd;
vector<string> g_markListName;
vector<double> g_markListTime;
vector<string> g_visemeListType;
vector<double> g_visemeListStart;
vector<double> g_visemeListArticulation;

//map[ "en_sp" ][ "pau" ] == "_";
hash_map<string, string> g_visemeMaps;

string g_visemeMapping;

vector<char> g_audioData;

XERCES_CPP_NAMESPACE_USE



const char * facefxMapping = {
   "<mapping>"
  "<entry phoneme=\"pau\" target=\"open\" amount=\"0.000000\" />"
  "<entry phoneme=\"t\" target=\"open\" amount=\"0.400000\" />"
  "<entry phoneme=\"d\" target=\"open\" amount=\"0.400000\" />"
  "<entry phoneme=\"k\" target=\"open\" amount=\"0.250000\" />"
  "<entry phoneme=\"g\" target=\"open\" amount=\"0.250000\" />"
  "<entry phoneme=\"n\" target=\"open\" amount=\"0.400000\" />"
  "<entry phoneme=\"ng\" target=\"open\" amount=\"0.400000\" />"
  "<entry phoneme=\"th\" target=\"open\" amount=\"0.450000\" />"
  "<entry phoneme=\"dh\" target=\"open\" amount=\"0.450000\" />"
  "<entry phoneme=\"s\" target=\"open\" amount=\"0.150000\" />"
  "<entry phoneme=\"z\" target=\"open\" amount=\"0.150000\" />"
  "<entry phoneme=\"r\" target=\"open\" amount=\"0.100000\" />"
  "<entry phoneme=\"l\" target=\"open\" amount=\"0.400000\" />"
  "<entry phoneme=\"h\" target=\"open\" amount=\"0.200000\" />"
  "<entry phoneme=\"i@\" target=\"open\" amount=\"0.200000\" />"
  "<entry phoneme=\"e\" target=\"open\" amount=\"0.350000\" />"
  "<entry phoneme=\"e@\" target=\"open\" amount=\"0.500000\" />"
  "<entry phoneme=\"a\" target=\"open\" amount=\"0.550000\" />"
  "<entry phoneme=\"aa\" target=\"open\" amount=\"0.550000\" />"
  "<entry phoneme=\"o\" target=\"open\" amount=\"0.400000\" />"
  "<entry phoneme=\"u@\" target=\"open\" amount=\"0.400000\" />"
  "<entry phoneme=\"@@\" target=\"open\" amount=\"0.500000\" />"
  "<entry phoneme=\"uu\" target=\"open\" amount=\"0.400000\" />"
  "<entry phoneme=\"u\" target=\"open\" amount=\"0.400000\" />"
  "<entry phoneme=\"@\" target=\"open\" amount=\"0.500000\" />"
  "<entry phoneme=\"ei\" target=\"open\" amount=\"0.500000\" />"
  "<entry phoneme=\"au\" target=\"open\" amount=\"0.500000\" />"
  "<entry phoneme=\"ai\" target=\"open\" amount=\"0.500000\" />"
  "<entry phoneme=\"oi\" target=\"open\" amount=\"0.400000\" />"
  "<entry phoneme=\"oo\" target=\"open\" amount=\"0.400000\" />"
  "<entry phoneme=\"r\" target=\"W\" amount=\"0.700000\" />"
  "<entry phoneme=\"y\" target=\"W\" amount=\"0.500000\" />"
  "<entry phoneme=\"w\" target=\"W\" amount=\"0.850000\" />"
  "<entry phoneme=\"o\" target=\"W\" amount=\"0.550000\" />"
  "<entry phoneme=\"u@\" target=\"W\" amount=\"0.550000\" />"
  "<entry phoneme=\"uu\" target=\"W\" amount=\"0.550000\" />"
  "<entry phoneme=\"u\" target=\"W\" amount=\"0.550000\" />"
  "<entry phoneme=\"oi\" target=\"W\" amount=\"0.550000\" />"
  "<entry phoneme=\"oo\" target=\"W\" amount=\"0.550000\" />"
  "<entry phoneme=\"sh\" target=\"ShCh\" amount=\"0.850000\" />"
  "<entry phoneme=\"zh\" target=\"ShCh\" amount=\"0.850000\" />"
  "<entry phoneme=\"y\" target=\"ShCh\" amount=\"0.300000\" />"
  "<entry phoneme=\"ch\" target=\"ShCh\" amount=\"0.850000\" />"
  "<entry phoneme=\"p\" target=\"PBM\" amount=\"0.900000\" />"
  "<entry phoneme=\"b\" target=\"PBM\" amount=\"0.900000\" />"
  "<entry phoneme=\"m\" target=\"PBM\" amount=\"0.900000\" />"
  "<entry phoneme=\"f\" target=\"FV\" amount=\"0.750000\" />"
  "<entry phoneme=\"v\" target=\"FV\" amount=\"0.750000\" />"
  "<entry phoneme=\"s\" target=\"wide\" amount=\"0.500000\" />"
  "<entry phoneme=\"z\" target=\"wide\" amount=\"0.500000\" />"
  "<entry phoneme=\"i@\" target=\"wide\" amount=\"0.800000\" />"
  "<entry phoneme=\"e\" target=\"wide\" amount=\"0.250000\" />"
  "<entry phoneme=\"e@\" target=\"wide\" amount=\"0.600000\" />"
  "<entry phoneme=\"@@\" target=\"wide\" amount=\"0.600000\" />"
  "<entry phoneme=\"@\" target=\"wide\" amount=\"0.600000\" />"
  "<entry phoneme=\"ei\" target=\"wide\" amount=\"0.600000\" />"
  "<entry phoneme=\"au\" target=\"wide\" amount=\"0.600000\" />"
  "<entry phoneme=\"ai\" target=\"wide\" amount=\"0.600000\" />"
  "<entry phoneme=\"k\" target=\"tBack\" amount=\"0.800000\" />"
  "<entry phoneme=\"g\" target=\"tBack\" amount=\"0.800000\" />"
  "<entry phoneme=\"e@\" target=\"tBack\" amount=\"0.400000\" />"
  "<entry phoneme=\"@@\" target=\"tBack\" amount=\"0.400000\" />"
  "<entry phoneme=\"@\" target=\"tBack\" amount=\"0.400000\" />"
  "<entry phoneme=\"ei\" target=\"tBack\" amount=\"0.400000\" />"
  "<entry phoneme=\"au\" target=\"tBack\" amount=\"0.400000\" />"
  "<entry phoneme=\"ai\" target=\"tBack\" amount=\"0.400000\" />"
  "<entry phoneme=\"t\" target=\"tRoof\" amount=\"0.800000\" />"
  "<entry phoneme=\"d\" target=\"tRoof\" amount=\"0.800000\" />"
  "<entry phoneme=\"n\" target=\"tRoof\" amount=\"0.800000\" />"
  "<entry phoneme=\"ng\" target=\"tRoof\" amount=\"0.800000\" />"
  "<entry phoneme=\"s\" target=\"tRoof\" amount=\"0.400000\" />"
  "<entry phoneme=\"z\" target=\"tRoof\" amount=\"0.400000\" />"
  "<entry phoneme=\"sh\" target=\"tRoof\" amount=\"0.400000\" />"
  "<entry phoneme=\"zh\" target=\"tRoof\" amount=\"0.400000\" />"
  "<entry phoneme=\"y\" target=\"tRoof\" amount=\"0.400000\" />"
  "<entry phoneme=\"l\" target=\"tRoof\" amount=\"0.800000\" />"
  "<entry phoneme=\"ch\" target=\"tRoof\" amount=\"0.400000\" />"
  "<entry phoneme=\"i@\" target=\"tRoof\" amount=\"0.200000\" />"
  "<entry phoneme=\"e\" target=\"tRoof\" amount=\"0.200000\" />"
  "<entry phoneme=\"th\" target=\"tTeeth\" amount=\"0.900000\" />"
  "<entry phoneme=\"dh\" target=\"tTeeth\" amount=\"0.900000\" />"
"</mapping>"
};



static void RapiCallbackFunction(
                       void       *pContext,
                       long        event_type,
                       const char *data_chunk,
                       long        data_type,
                       long        data_length,
                       const char *phone_set,
                       const char *cparam1,
                       const char *cparam2
                       );


BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
   switch ( fdwReason )
   {
      case DLL_PROCESS_ATTACH:
      case DLL_THREAD_ATTACH:
      case DLL_THREAD_DETACH:
      case DLL_PROCESS_DETACH:
      default:
         break;
   }

   return TRUE;
}



RVOICE_DLL_API bool RVOICE_DLL_Init(const char * visemeMapping)
{
   printf( "RvoiceDLL - Init()\n" );

   rAPI_Set_Servers( "bootcamp.ict.usc.edu" );
   g_rvoiceHandle = rAPI_Create_Handle( RAPI_ALL_EVENTS, RAPI_PCM_16bit_22050Hz, RapiCallbackFunction, NULL );
   rAPI_Use_Server( g_rvoiceHandle, "bootcamp.ict.usc.edu" );
   rAPI_Set_XML( g_rvoiceHandle, true ? 1 : 0 );

   RVOICE_DLL_SetVisemeMapping(visemeMapping);

   return true;
}


RVOICE_DLL_API void RVOICE_DLL_SetVisemeMapping(const char * visemeMapping)
{
   g_visemeMapping = visemeMapping;
   g_visemeMaps.clear();

   if (strcmp(g_visemeMapping.c_str(), "sbm") == 0)
   {
      // en_ep
      g_visemeMaps.insert( make_pair<string, string>( "pau", "_" ) ); // SIL
      g_visemeMaps.insert( make_pair<string, string>( "@", "Ih" ) );  // AX
      g_visemeMaps.insert( make_pair<string, string>( "@@", "Er" ) ); // ER
      g_visemeMaps.insert( make_pair<string, string>( "a", "Ih" ) );  // ??
      g_visemeMaps.insert( make_pair<string, string>( "aa", "Ao" ) ); // AA
      g_visemeMaps.insert( make_pair<string, string>( "ai", "Ih" ) ); // AY
      g_visemeMaps.insert( make_pair<string, string>( "au", "Ih" ) ); // AW
      g_visemeMaps.insert( make_pair<string, string>( "b", "BMP" ) ); //  B
      g_visemeMaps.insert( make_pair<string, string>( "ch", "j" ) );  // CH
      g_visemeMaps.insert( make_pair<string, string>( "d", "D" ) );   //  D
      g_visemeMaps.insert( make_pair<string, string>( "dh", "Th" ) ); // DH
      g_visemeMaps.insert( make_pair<string, string>( "e", "Ih" ) );  // EH
      g_visemeMaps.insert( make_pair<string, string>( "e@", "Ih" ) ); // ??
      g_visemeMaps.insert( make_pair<string, string>( "ei", "Ih" ) ); // EY (bagshaw used sampa)
      g_visemeMaps.insert( make_pair<string, string>( "f", "F" ) );   //  F
      g_visemeMaps.insert( make_pair<string, string>( "g", "KG" ) );  //  G
      g_visemeMaps.insert( make_pair<string, string>( "h", "Ih" ) );  // HH
      g_visemeMaps.insert( make_pair<string, string>( "i", "Ih" ) );  // IH
      g_visemeMaps.insert( make_pair<string, string>( "i@", "Ih" ) ); // ??  probably IH
      g_visemeMaps.insert( make_pair<string, string>( "ii", "EE" ) ); // IY
      g_visemeMaps.insert( make_pair<string, string>( "jh", "j" ) );  // JH
      g_visemeMaps.insert( make_pair<string, string>( "k", "KG" ) );  //  K
      g_visemeMaps.insert( make_pair<string, string>( "l", "D" ) );   //  L
      g_visemeMaps.insert( make_pair<string, string>( "m", "BMP" ) ); //  M
      g_visemeMaps.insert( make_pair<string, string>( "n", "NG" ) );  //  N
      g_visemeMaps.insert( make_pair<string, string>( "ng", "NG" ) ); // NG
      g_visemeMaps.insert( make_pair<string, string>( "o", "Ao" ) );  // ?? probably AO
      g_visemeMaps.insert( make_pair<string, string>( "oi", "EE" ) ); // OY
      g_visemeMaps.insert( make_pair<string, string>( "oo", "Ao" ) ); // AO
      g_visemeMaps.insert( make_pair<string, string>( "ou", "OW" ) ); // ???? <-- added by EDF
      g_visemeMaps.insert( make_pair<string, string>( "p", "BMP" ) ); //  P
      g_visemeMaps.insert( make_pair<string, string>( "r", "R" ) );   //  R
      g_visemeMaps.insert( make_pair<string, string>( "s", "Z" ) );   //  S
      g_visemeMaps.insert( make_pair<string, string>( "sh", "j" ) );  // SH
      g_visemeMaps.insert( make_pair<string, string>( "t", "D" ) );   //  T
      g_visemeMaps.insert( make_pair<string, string>( "th", "Th" ) ); // TH
      g_visemeMaps.insert( make_pair<string, string>( "u@", "Oh" ) ); // OW
      g_visemeMaps.insert( make_pair<string, string>( "uh", "Ih" ) ); // AH
      g_visemeMaps.insert( make_pair<string, string>( "uu", "Oh" ) ); // UW
      g_visemeMaps.insert( make_pair<string, string>( "u", "UH" ) );  // ???? <-- added by EDF
      g_visemeMaps.insert( make_pair<string, string>( "v", "F" ) );   //  V
      g_visemeMaps.insert( make_pair<string, string>( "w", "OO" ) );  //  W
      g_visemeMaps.insert( make_pair<string, string>( "y", "OO" ) );  //  Y
      g_visemeMaps.insert( make_pair<string, string>( "z", "Z" ) );   //  Z
      g_visemeMaps.insert( make_pair<string, string>( "zh", "j" ) );  // ZH
   }
   else if (strcmp(g_visemeMapping.c_str(), "facefx") == 0)
   {
       // Mapping for facefx has been initialized in string above
   }
}


RVOICE_DLL_API bool RVOICE_DLL_GenerateAudio(const char * message, const char * outputFilename, const char * messageOutputFileName, const char * voice, char * xmlReplyReturn, RVOICE_DLL_GenerateAudioReplyInterop * generateAudioReplyInterop)
{
   g_outputFilename = outputFilename;
   g_outputFilename = vhcl::Replace(g_outputFilename, ".wav", ".aiff");

   g_bookmarkIntermediate = "";

   g_waveSize = 0;
   g_curTime = 0;
   g_startWordTime = 0;
   g_audioData.clear();

   g_wordBreakListStart.clear();
   g_wordBreakListEnd.clear();
   g_markListName.clear();
   g_markListTime.clear();
   g_visemeListType.clear();
   g_visemeListStart.clear();
   g_visemeListArticulation.clear();

   rAPI_Speak( g_rvoiceHandle, voice, message );

   strcpy( xmlReplyReturn, "" );

   generateAudioReplyInterop->soundFile = NULL;
   generateAudioReplyInterop->workBreakListNum = 0;
   generateAudioReplyInterop->wordBreakListStart = NULL;
   generateAudioReplyInterop->wordBreakListEnd = NULL;
   generateAudioReplyInterop->markListNum = 0;
   generateAudioReplyInterop->markListName = NULL;
   generateAudioReplyInterop->markListTime = NULL;
   generateAudioReplyInterop->visemeListNum = 0;
   generateAudioReplyInterop->visemeListType = NULL;
   generateAudioReplyInterop->visemeListStart = NULL;

   generateAudioReplyInterop->soundFile = new char [g_outputFilename.size() + 1];
   strcpy(generateAudioReplyInterop->soundFile, g_outputFilename.c_str());


   generateAudioReplyInterop->workBreakListNum = g_wordBreakListStart.size();
   generateAudioReplyInterop->wordBreakListStart = new double [g_wordBreakListStart.size()];
   if (!g_wordBreakListStart.empty())
      memcpy(generateAudioReplyInterop->wordBreakListStart, &g_wordBreakListStart[0], sizeof(double) * g_wordBreakListStart.size());

   generateAudioReplyInterop->wordBreakListEnd = new double [g_wordBreakListEnd.size()];
   if (!g_wordBreakListEnd.empty())
      memcpy(generateAudioReplyInterop->wordBreakListEnd, &g_wordBreakListEnd[0], sizeof(double) * g_wordBreakListEnd.size());


   generateAudioReplyInterop->markListNum = g_markListName.size();
   generateAudioReplyInterop->markListName = new char * [g_markListName.size()];
   for (size_t i = 0; i < g_markListName.size(); i++)
   {
      generateAudioReplyInterop->markListName[i] = new char [g_markListName[i].size() + 1];
      strcpy(generateAudioReplyInterop->markListName[i], g_markListName[i].c_str());
   }

   generateAudioReplyInterop->markListTime = new double [g_markListTime.size()];
   if (!g_markListTime.empty())
      memcpy(generateAudioReplyInterop->markListTime, &g_markListTime[0], sizeof(double) * g_markListTime.size());


   generateAudioReplyInterop->visemeListNum = g_visemeListType.size();
   generateAudioReplyInterop->visemeListType = new char * [g_visemeListType.size()];
   for (size_t i = 0; i < g_visemeListType.size(); i++)
   {
      generateAudioReplyInterop->visemeListType[i] = new char [g_visemeListType[i].size() + 1];
      strcpy(generateAudioReplyInterop->visemeListType[i], g_visemeListType[i].c_str());
   }

   generateAudioReplyInterop->visemeListStart = new double [g_visemeListStart.size()];
   if (!g_visemeListStart.empty())
      memcpy(generateAudioReplyInterop->visemeListStart, &g_visemeListStart[0], sizeof(double) * g_visemeListStart.size());

   generateAudioReplyInterop->visemeListArticulation = new double [g_visemeListArticulation.size()];
   if (!g_visemeListArticulation.empty())
      memcpy(generateAudioReplyInterop->visemeListArticulation, &g_visemeListArticulation[0], sizeof(double) * g_visemeListArticulation.size());

   return true;
}


RVOICE_DLL_API bool RVOICE_DLL_Close()
{
   rAPI_Close_Handle( g_rvoiceHandle );
   rAPI_Clean_Up();

   return true;
}


static string MapToViseme( const string & phonemeSet, const string & phoneme )
{
   hash_map<string, string>::iterator it = g_visemeMaps.find( phoneme );
   if ( it != g_visemeMaps.end() )
   {
      return it->second;
   }
   else
   {
      return "";
   }
}


static void RapiCallbackFunction(
                       void       *pContext,
                       long        event_type,
                       const char *data_chunk,
                       long        data_type,
                       long        data_length,
                       const char *phone_set,
                       const char *cparam1,
                       const char *cparam2
                       )
{
   //printf( "OnCallback: %d - %d - %d\n", event_type, data_type, data_length );

   switch ( event_type )
   {
       case 0x06: // RAPI_WORD_BOUNDARY
           printf( "RAPI_WORD_BOUNDARY ---- %f\n", g_curTime );

           //wordBreaks.add( new Double( curTime ) );

           if ( g_startWordTime == 0 )
           {
              g_startWordTime = g_curTime;

              if (g_bookmarkIntermediate != "")
              {
                 g_markListName.push_back(g_bookmarkIntermediate);
                 g_markListTime.push_back(g_startWordTime);
              }

              g_bookmarkIntermediate = "";
              break;
           }


           g_wordBreakListStart.push_back(g_startWordTime);
           g_wordBreakListEnd.push_back(g_curTime);

           g_startWordTime = g_curTime;


           //if( outputXml != null && outputXml.getTagName().equals(TAG_WORD) )
           //{
           //    outputXml.setAttribute( ATTR_END, Double.toString(curTime) );
           //    outputXml= ( Element )outputXml.getParentNode();
           //}

           //if( curTime < firstWordBreak )
           //    firstWordBreak = curTime;

           //lastWordBreak = curTime;
           break;

      case 0x07: // RAPI_PHON_BOUNDARY
         {
           //if( !strPhoneSet.equals( phoneSet ) )
           //{
           //    phoneSet=strPhoneSet;
           //    System.out.println();
           //    System.out.println( "Phoneme set \""+phoneSet+"\": " );
           //}

            if ( !strcmp("facefx",g_visemeMapping.c_str()) )
            {
               XercesDOMParser *parser = new XercesDOMParser;
               DOMNodeList * list_of_faceFX_phonemes = NULL;
               parser->parse(MemBufInputSource((const XMLByte *)facefxMapping, strlen(facefxMapping), "facefxMapping", false));
               DOMDocument* rule_input;
               rule_input = parser->getDocument();
               list_of_faceFX_phonemes = rule_input->getElementsByTagName(XMLString::transcode( "entry" ));


               size_t loopLength = list_of_faceFX_phonemes->getLength();


               for (size_t phonemeCounter = 0; phonemeCounter < loopLength; phonemeCounter++)
               {
                  DOMElement* entry;

                  std::string phoneme;
                  std::string target;
                  std::string articulation;


                  entry = (DOMElement*)  list_of_faceFX_phonemes->item(phonemeCounter);

                  phoneme = XMLString::transcode(entry->getAttribute(XMLString::transcode( ("phoneme") )));
                  target = XMLString::transcode(entry->getAttribute(XMLString::transcode( ("target") )));
                  articulation = XMLString::transcode(entry->getAttribute(XMLString::transcode( ("amount") )));


                  //check added to avoid crashing on entries which are not defined
                  if( phoneme == cparam1)
                  {
                     string viseme = target;
                     //string viseme = MapToViseme( phone_set, cparam1 );

                     printf( "RAPI_PHON_BOUNDARY ---- %s %s %s %f\n", phone_set, cparam1, viseme.c_str(), g_curTime );

                     //phonemes.add( new PhonemeTime( viseme, curTime ) );

                     if ( viseme != "" )
                     {

                        // if the list of visemes is empty, just add the viseme to the output
                        if(g_visemeListType.empty())
                        {
                           //g_visemesIntermediate += vhcl::Format( "      <viseme start=\"%f\" type=\"%s\" articulation=\"%s\"/>\n", g_curTime, viseme.c_str(), articulation.c_str() );

                           g_visemeListType.push_back(viseme);
                           g_visemeListStart.push_back(g_curTime);
                           g_visemeListArticulation.push_back(atof(articulation.c_str()));
                        }
                        else
                        {
                           // avoiding duplicate visemes with same start times
                           if(!(!strcmp(((string)g_visemeListType.back()).c_str(),viseme.c_str()) && (((int)g_visemeListStart.back()) == g_curTime)))
                           {
                              //g_visemesIntermediate += vhcl::Format( "      <viseme start=\"%f\" type=\"%s\" articulation=\"%s\"/>\n", g_curTime, viseme.c_str(), articulation.c_str() );

                              g_visemeListType.push_back(viseme);
                              g_visemeListStart.push_back(g_curTime);
                              g_visemeListArticulation.push_back(atof(articulation.c_str()));
                           }
                        }
                     }
                  }
               }
            }
            else
            {
               string viseme = MapToViseme( phone_set, cparam1 );

               printf( "RAPI_PHON_BOUNDARY ---- %s %s %s %f\n", phone_set, cparam1, viseme.c_str(), g_curTime );

               //phonemes.add( new PhonemeTime( viseme, curTime ) );

               if ( viseme != "" )
               {
                  // if the list of visemes is empty, just add the viseme to the output
                  if(g_visemeListType.empty())
                  {
                     //g_visemesIntermediate += vhcl::Format( "      <viseme start=\"%f\" type=\"%s\"/>\n", g_curTime, viseme.c_str() );

                     g_visemeListType.push_back(viseme);
                     g_visemeListStart.push_back(g_curTime);
                     g_visemeListArticulation.push_back(1.0);
                  }
                  else
                  {
                     // avoiding duplicate visemes with same start times
                     if(!(!strcmp(((string)g_visemeListType.back()).c_str(),viseme.c_str()) && (((int)g_visemeListStart.back()) == g_curTime)))
                     {
                        //g_visemesIntermediate += vhcl::Format( "      <viseme start=\"%f\" type=\"%s\"/>\n", g_curTime, viseme.c_str() );

                        g_visemeListType.push_back(viseme);
                        g_visemeListStart.push_back(g_curTime);
                        g_visemeListArticulation.push_back(1.0);
                     }
                  }

                  if ( viseme == "_" )
                  {
                     //g_returnString += g_visemesIntermediate.replace( 0, 3, "" );
                     //g_visemesIntermediate = "";
                  }
               }
            }


                 //g_startWordTime = curTime;
              //}
           //}

/*
           if( outputXml != null )
           {
               String curTimeStr = Double.toString( curTime );

               Document xmlDoc = outputXml.getOwnerDocument();
               Element visemeElem = xmlDoc.createElement( TAG_VISEME );
               visemeElem.setAttribute( ATTR_TYPE, viseme );
               visemeElem.setAttribute( ATTR_START, curTimeStr );

               boolean isInWord = outputXml.getTagName().equals(TAG_WORD);
               if( isInWord )
               {  // continuing word
                   outputXml.appendChild( xmlDoc.createTextNode("   " ) );
               }
               else
               {
                   outputXml.appendChild( xmlDoc.createTextNode( "\n   " ) );
                   if( !viseme.equals( "_" ) )
                   { // new word
                       Element wordElem=xmlDoc.createElement( TAG_WORD );
                       wordElem.setAttribute( ATTR_START, curTimeStr );
                       outputXml.appendChild( wordElem );
                       outputXml=wordElem;
                       outputXml.appendChild( xmlDoc.createTextNode( "\n      " ) );
                       isInWord=true;

                       if( firstWord==null )
                           firstWord = wordElem;
                       lastWord = wordElem;
                   }
               }
               outputXml.appendChild( visemeElem );
               if( isInWord )
                   outputXml.appendChild( xmlDoc.createTextNode( "\n   " ) );
           }
*/
         }
         break;

      case 0x0C: // RAPI_BOOKMARK_EVENT
          printf( "RAPI_BOOKMARK_EVENT ---- '%s' %f\n", cparam1, g_curTime );

          // if it's the first bookmark, we want the time to be of the first word, not 0 which is returned on the first bookmark
          // so we store the bookmark and delay it until the word event comes through
          if ( g_startWordTime == 0 )
          {
             g_bookmarkIntermediate = cparam1;
          }
          else
          {
             g_markListName.push_back(cparam1);
             g_markListTime.push_back(g_curTime);
          }

           /*
           if( outputXml != null )
           {
               bookmarks.add( new BookmarkTime( strEventData1, curTime, outputXml, outputXml.getLastChild() ) );
           }
           */
           break;

       case 0x10: // RAPI_AUDIO_EVENT
          {
            printf( "RAPI_AUDIO_EVENT ---- %f\n", g_curTime );

           if( data_chunk == NULL )
               break;  // occassionally happens, not sure why (Anm)


            int length = data_length;
            g_waveSize += length;
            //if( DATA_FORMAT == 44100 )
            {
                g_waveSize += length;  // samples are doubled
                g_curTime = ((double)g_waveSize) / 88200;  // 88200 bytes per second (two bytes per frame)

                for ( int i = 0; i < length; i++ )
                {
                   g_audioData.push_back( data_chunk[ i + 1 ] );
                   g_audioData.push_back( data_chunk[ i ] );
                   g_audioData.push_back( data_chunk[ i + 1 ] );
                   g_audioData.push_back( data_chunk[ i ] );
                   i++;
                }

                /*
                if( outputAudio != null )
                {
                    byte[] sample=new byte[2];
                    for( int i=0; i<length; i++ )
                    {
                        // Rhetorical is Intel... write out network endian
                        sample[1]=abDataChunk[i];
                        sample[0]=abDataChunk[++i];
                        // up sample from 22050 by double each sample
                        outputAudio.write( sample );
                        outputAudio.write( sample );
                    }
                }
                */
            }
          }
          break;

       case 0x0E: // RAPI_FINISHED_EVENT
          {
            printf( "RAPI_FINISHED_EVENT ---- %f\n", g_curTime );
          /*
            if( outputAudio!=null )
            {
                if( DEBUG_FILE_POINTER ) System.out.println( "DEBUG: waveSize="+waveSize );

                final int formSize = waveSize+46; // FORM size = AIFF + COMM chunk + SSND header + block size/offset

                if( DEBUG_FILE_POINTER ) System.out.println( "DEBUG: Writing FORM Size "+formSize+" @ "+offsetFormatSize );

                outputAudio.seek( offsetFormatSize );
                outputAudio.writeInt( formSize );
                final int numFrames = waveSize/2; // number of frames = 2 bytes per frame

                if( DEBUG_FILE_POINTER ) System.out.println( "DEBUG: Writing Number of Frames "+numFrames+" @ "+offsetNumberFrames );

                outputAudio.seek( offsetNumberFrames );
                outputAudio.writeInt( numFrames );
                final int ssndSize = waveSize+8; // SSND size = sound data plus 2 4-byte ints (block offset & block size)

                if( DEBUG_FILE_POINTER ) System.out.println( "DEBUG: Writing SSND Size "+ssndSize+" @ "+offsetSsndSize );

                outputAudio.seek( offsetSsndSize );
                outputAudio.writeInt( ssndSize );
                outputAudio.close();
            }
            */


            FILE * f = fopen( g_outputFilename.c_str(), "wb" );
            if ( f == NULL )
            {
               printf( "RAPI_FINISHED_EVENT - Audio file open failed! '%s'\n", g_outputFilename.c_str() );
               break;
            }

            const char ID_FORM[] = { 'F', 'O', 'R', 'M' };
            const char ID_AIFF[] = { 'A', 'I', 'F', 'F' };
            const char ID_COMM[] = { 'C', 'O', 'M', 'M' };
            const char ID_SSND[] = { 'S', 'S', 'N', 'D' };
            const char SAMPLE_RATE_44100[] = { 64, 14, -84, 68, 0, 0, 0, 0, 0, 0 };

            int formSize = htonl( g_waveSize + 46 ); // FORM size = AIFF + COMM chunk + SSND header + block size/offset
            int commSize = htonl( 18 );
            short numChannels = htons( 1 );
            int numFrames = htonl( g_waveSize / 2 ); // number of frames = 2 bytes per frame
            short bitsPerSample = htons( 16 );
            int ssndSize  = htonl( g_waveSize + 8 ); // SSND size = sound data plus 2 4-byte ints (block offset & block size)
            int unused = 0;


            fwrite( ID_FORM, sizeof( char ), 4, f );
            fwrite( &formSize, sizeof( int ), 1, f );
            fwrite( ID_AIFF, sizeof( char ), 4, f );
            fwrite( ID_COMM, sizeof( char ), 4, f );
            fwrite( &commSize, sizeof( int ), 1, f );
            fwrite( &numChannels, sizeof( short ), 1, f );
            fwrite( &numFrames, sizeof( int ), 1, f );
            fwrite( &bitsPerSample, sizeof( short ), 1, f );
            fwrite( SAMPLE_RATE_44100, sizeof( char ), 10, f );
            fwrite( ID_SSND, sizeof( char ), 4, f );
            fwrite( &ssndSize, sizeof( int ), 1, f );
            fwrite( &unused, sizeof( int ), 1, f );
            fwrite( &unused, sizeof( int ), 1, f );

            fwrite( &g_audioData[ 0 ], sizeof( char ), g_audioData.size(), f );

            fclose( f );
          }
          break;

       default:
          break;
   }
}
