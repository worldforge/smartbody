


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

#include "vhcl.h"

#include "FestivalDLL.h"

#include <algorithm>
#include <sstream>

#ifdef WIN_BUILD
#include <windows.h>
#endif
#include "festival.h"
#include "VHDuration.h"
#include <map>

bool FInitialized = false;

XERCES_CPP_NAMESPACE_USE

using std::string;

#ifdef WIN_BUILD
const char * FESTIVAL_RELAY_LIB_DIR = "..\\..\\lib\\festival\\festival\\lib";
#else
const char * FESTIVAL_RELAY_LIB_DIR = "../../lib/festival/festival/lib";
#endif
// Globals from \lib\festival\festival\src\modules\VHDuration\VHDuration.cc
extern SpeechRequestData xmlMetaData;

#ifdef WIN_BUILD
std::string mapping;
extern std::multimap<std::string, std::pair<string, double> > phonemeToViseme;
#else
std::string mapping;
extern std::multimap<std::string, std::pair<string, double> > phonemeToViseme;
#endif


std::string generateReply(const char * utterance, const char * soundFileName);

#ifdef WIN_BUILD
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
#else
//I don't think this is necessary//
// _attribute_((constructor)) void foobar()
//{
//}
#endif

FESTIVAL_DLL_API void set_phonemes_to_visemes()
{
    phonemeToViseme.clear();

    if (mapping == "sbmold")
    {
        phonemeToViseme.insert(make_pair("pau", make_pair("_", 1.0)));  // SIL
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
        phonemeToViseme.insert(make_pair("y",  make_pair("Sh", 1.0))); //  Y
        phonemeToViseme.insert(make_pair("z",  make_pair("Z", 1.0)));  //  Z
        phonemeToViseme.insert(make_pair("zh", make_pair("Sh", 1.0)));  // ZH
    }
    else if (mapping == "facefx")
    {
        phonemeToViseme.insert(make_pair("pau", make_pair("open", 0.400000)));
        phonemeToViseme.insert(make_pair("T", make_pair("open", 0.400000)));
        phonemeToViseme.insert(make_pair("t", make_pair("open", 0.400000)));
        phonemeToViseme.insert(make_pair("k", make_pair("open", 0.250000)));
        phonemeToViseme.insert(make_pair("g", make_pair("open", 0.250000)));
        phonemeToViseme.insert(make_pair("n", make_pair("open", 0.400000)));
        phonemeToViseme.insert(make_pair("ng", make_pair("open", 0.400000)));
        phonemeToViseme.insert(make_pair("th", make_pair("open", 0.450000)));
        phonemeToViseme.insert(make_pair("dx", make_pair("open", 0.450000)));
        phonemeToViseme.insert(make_pair("s", make_pair("open", 0.150000)));
        phonemeToViseme.insert(make_pair("z", make_pair("open", 0.150000)));
        phonemeToViseme.insert(make_pair("hh", make_pair("open", 0.300000)));
        phonemeToViseme.insert(make_pair("r", make_pair("open", 0.100000)));
        phonemeToViseme.insert(make_pair("l", make_pair("open", 0.400000)));
        phonemeToViseme.insert(make_pair("iy", make_pair("open", 0.200000)));
        phonemeToViseme.insert(make_pair("eh", make_pair("open", 0.500000)));
        phonemeToViseme.insert(make_pair("aa", make_pair("open", 0.550000)));
        phonemeToViseme.insert(make_pair("ao", make_pair("open", 0.400000)));
        phonemeToViseme.insert(make_pair("uw", make_pair("open", 0.400000)));
        phonemeToViseme.insert(make_pair("ah", make_pair("open", 0.500000)));
        phonemeToViseme.insert(make_pair("ih", make_pair("open", 0.500000)));
        phonemeToViseme.insert(make_pair("uh", make_pair("open", 0.400000)));
        phonemeToViseme.insert(make_pair("@", make_pair("open", 0.500000)));
        phonemeToViseme.insert(make_pair("ae", make_pair("open", 0.500000)));
        phonemeToViseme.insert(make_pair("er", make_pair("open", 0.400000)));
        phonemeToViseme.insert(make_pair("ey", make_pair("open", 0.500000)));
        phonemeToViseme.insert(make_pair("aw", make_pair("open", 0.500000)));
        phonemeToViseme.insert(make_pair("ay", make_pair("open", 0.500000)));
        phonemeToViseme.insert(make_pair("oy", make_pair("open", 0.400000)));
        phonemeToViseme.insert(make_pair("ow", make_pair("open", 0.400000)));
        phonemeToViseme.insert(make_pair("r", make_pair("W", 0.700000)));
        phonemeToViseme.insert(make_pair("y", make_pair("W", 0.500000)));
        phonemeToViseme.insert(make_pair("w", make_pair("W", 0.850000)));
        phonemeToViseme.insert(make_pair("ao", make_pair("W", 0.550000)));
        phonemeToViseme.insert(make_pair("uw", make_pair("W", 0.550000)));
        phonemeToViseme.insert(make_pair("uh", make_pair("W", 0.550000)));
        phonemeToViseme.insert(make_pair("oy", make_pair("W", 0.550000)));
        phonemeToViseme.insert(make_pair("ow", make_pair("W", 0.550000)));
        phonemeToViseme.insert(make_pair("sh", make_pair("ShCh", 0.850000)));
        phonemeToViseme.insert(make_pair("zh", make_pair("ShCh", 0.850000)));
        phonemeToViseme.insert(make_pair("y", make_pair("ShCh", 0.300000)));
        phonemeToViseme.insert(make_pair("ch", make_pair("ShCh", 0.850000)));
        phonemeToViseme.insert(make_pair("jh", make_pair("ShCh", 0.850000)));
        phonemeToViseme.insert(make_pair("er", make_pair("ShCh", 0.500000)));
        phonemeToViseme.insert(make_pair("p", make_pair("PBM", 0.900000)));
        phonemeToViseme.insert(make_pair("b", make_pair("PBM", 0.900000)));
        phonemeToViseme.insert(make_pair("m", make_pair("PBM", 0.900000)));
        phonemeToViseme.insert(make_pair("f", make_pair("FV", 0.750000)));
        phonemeToViseme.insert(make_pair("v", make_pair("FV", 0.750000)));
        phonemeToViseme.insert(make_pair("s", make_pair("wide", 0.500000)));
        phonemeToViseme.insert(make_pair("z", make_pair("wide", 0.500000)));
        phonemeToViseme.insert(make_pair("iy", make_pair("wide", 0.800000)));
        phonemeToViseme.insert(make_pair("eh", make_pair("wide", 0.600000)));
        phonemeToViseme.insert(make_pair("ah", make_pair("wide", 0.600000)));
        phonemeToViseme.insert(make_pair("ih", make_pair("wide", 0.600000)));
        phonemeToViseme.insert(make_pair("@", make_pair("wide", 0.600000)));
        phonemeToViseme.insert(make_pair("ae", make_pair("wide", 0.600000)));
        phonemeToViseme.insert(make_pair("ey", make_pair("wide", 0.600000)));
        phonemeToViseme.insert(make_pair("aw", make_pair("wide", 0.600000)));
        phonemeToViseme.insert(make_pair("ay", make_pair("wide", 0.600000)));
        phonemeToViseme.insert(make_pair("k", make_pair("tBack", 0.800000)));
        phonemeToViseme.insert(make_pair("g", make_pair("tBack", 0.800000)));
        phonemeToViseme.insert(make_pair("eh", make_pair("tBack", 0.400000)));
        phonemeToViseme.insert(make_pair("ah", make_pair("tBack", 0.400000)));
        phonemeToViseme.insert(make_pair("ih", make_pair("tBack", 0.400000)));
        phonemeToViseme.insert(make_pair("@", make_pair("tBack", 0.400000)));
        phonemeToViseme.insert(make_pair("ae", make_pair("tBack", 0.400000)));
        phonemeToViseme.insert(make_pair("ey", make_pair("tBack", 0.400000)));
        phonemeToViseme.insert(make_pair("aw", make_pair("tBack", 0.400000)));
        phonemeToViseme.insert(make_pair("ay", make_pair("tBack", 0.400000)));
        phonemeToViseme.insert(make_pair("T", make_pair("tRoof", 0.800000)));
        phonemeToViseme.insert(make_pair("t", make_pair("tRoof", 0.800000)));
        phonemeToViseme.insert(make_pair("n", make_pair("tRoof", 0.800000)));
        phonemeToViseme.insert(make_pair("ng", make_pair("tRoof", 0.800000)));
        phonemeToViseme.insert(make_pair("s", make_pair("tRoof", 0.400000)));
        phonemeToViseme.insert(make_pair("z", make_pair("tRoof", 0.400000)));
        phonemeToViseme.insert(make_pair("sh", make_pair("tRoof", 0.400000)));
        phonemeToViseme.insert(make_pair("zh", make_pair("tRoof", 0.400000)));
        phonemeToViseme.insert(make_pair("y", make_pair("tRoof", 0.400000)));
        phonemeToViseme.insert(make_pair("l", make_pair("tRoof", 0.800000)));
        phonemeToViseme.insert(make_pair("ch", make_pair("tRoof", 0.400000)));
        phonemeToViseme.insert(make_pair("jh", make_pair("tRoof", 0.400000)));
        phonemeToViseme.insert(make_pair("iy", make_pair("tRoof", 0.200000)));
        phonemeToViseme.insert(make_pair("er", make_pair("tRoof", 0.500000)));
        phonemeToViseme.insert(make_pair("th", make_pair("tTeeth", 0.900000)));
        phonemeToViseme.insert(make_pair("dx", make_pair("tTeeth", 0.900000)));
    }
}



FESTIVAL_DLL_API bool FESTIVAL_DLL_Init(const char * visemeMapping)
{
   FESTIVAL_DLL_SetVisemeMapping(visemeMapping);

   festival_libdir = FESTIVAL_RELAY_LIB_DIR;

   // Moving initialize to generate audio. On linux, festival can only run on a single thread
   //see http://www.linuxquestions.org/questions/linux-general-1/festival-c-api-and-pthread-839667/
   //int heap_size = FESTIVAL_HEAP_SIZE;
   //int load_init_files = 1; // we want the festival init files loaded
   //festival_initialize(load_init_files,heap_size);

   printf( "Festival Text To Speech Engine:\n" );
   printf( "Initializing....\n");

   // setting voice to be used
   //std::string voice_command = vhcl::Format( "(%s)", "unused" );
   //festival_eval_command(voice_command.c_str());

   // setting the duration method to be used by festival

   //festival_eval_command("(Parameter.set `Duration_Method Duration_Default)");

   // this command hooks our virtual human method such that every time an utterance is synthesized, our method is called on it
   // in order to generate the virtual human message (RemoteSpeechReply)
   //festival_eval_command("(set! after_synth_hooks (list Duration_VirtualHuman))");

   return true;
}


FESTIVAL_DLL_API void FESTIVAL_DLL_SetVisemeMapping(const char * visemeMapping)
{
   mapping = visemeMapping;

   set_phonemes_to_visemes();
}


FESTIVAL_DLL_API bool FESTIVAL_DLL_GenerateAudio(const char * message, const char * outputFilename, const char * messageOutputFileName, const char * voice, char * xmlReplyReturn, FESTIVAL_DLL_GenerateAudioReplyInterop * generateAudioReplyInterop)
{
    ///moving initialize to here...
    if (!FInitialized)

    {
    int heap_size = FESTIVAL_HEAP_SIZE;
    int load_init_files = 1; // we want the festival init files loaded
    festival_initialize(load_init_files,heap_size);
    festival_eval_command("(Parameter.set `Duration_Method Duration_Default)");
    FInitialized=true;

    }

    ////////////////////////////////
   // setting voice to be used
   std::string voice_command = vhcl::Format( "(%s)", voice );
   int ret = festival_eval_command(voice_command.c_str());
   if (ret == FALSE)
   {
       fprintf(stderr,"Could not set voice to '%s'.", voice);
   }

   // this command hooks our virtual human method such that every time an utterance is synthesized, our method is called on it
   // in order to generate the virtual human message (RemoteSpeechReply)
   ret = festival_eval_command("(set! after_synth_hooks (list Duration_VirtualHuman))");
   if (ret == FALSE)
   {
       fprintf(stderr,"Could find hook to Virtual Human module.");
   }

   xmlMetaData.g_wordBreakListStart.clear();
   xmlMetaData.g_wordBreakListEnd.clear();
   xmlMetaData.g_markListName.clear();
   xmlMetaData.g_markListTime.clear();
   xmlMetaData.g_visemeListType.clear();
   xmlMetaData.g_visemeListStart.clear();
   xmlMetaData.g_visemeListArticulation.clear();

   //Generate the audio
   string replyXML = generateReply(message, outputFilename);

   string remoteSpeechReply;
   remoteSpeechReply  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
   remoteSpeechReply += "<speak>\n";
   remoteSpeechReply += vhcl::Format( "   <soundFile name=\"%s\"/>\n", messageOutputFileName );
   remoteSpeechReply += replyXML;
   remoteSpeechReply += "</speak>";

   strcpy( xmlReplyReturn, remoteSpeechReply.c_str() );


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
   generateAudioReplyInterop->visemeListArticulation = NULL;

   generateAudioReplyInterop->soundFile = new char [strlen(outputFilename) + 1];
   strcpy(generateAudioReplyInterop->soundFile, outputFilename);


   generateAudioReplyInterop->workBreakListNum = xmlMetaData.g_wordBreakListStart.size();
   generateAudioReplyInterop->wordBreakListStart = new double [xmlMetaData.g_wordBreakListStart.size()];
   if (!xmlMetaData.g_wordBreakListStart.empty())
      memcpy(generateAudioReplyInterop->wordBreakListStart, &xmlMetaData.g_wordBreakListStart[0], sizeof(double) * xmlMetaData.g_wordBreakListStart.size());

   generateAudioReplyInterop->wordBreakListEnd = new double [xmlMetaData.g_wordBreakListEnd.size()];
   if (!xmlMetaData.g_wordBreakListEnd.empty())
      memcpy(generateAudioReplyInterop->wordBreakListEnd, &xmlMetaData.g_wordBreakListEnd[0], sizeof(double) * xmlMetaData.g_wordBreakListEnd.size());


   generateAudioReplyInterop->markListNum = xmlMetaData.g_markListName.size();
   generateAudioReplyInterop->markListName = new char * [xmlMetaData.g_markListName.size()];
   for (size_t i = 0; i < xmlMetaData.g_markListName.size(); i++)
   {
      generateAudioReplyInterop->markListName[i] = new char [xmlMetaData.g_markListName[i].size() + 1];
      strcpy(generateAudioReplyInterop->markListName[i], xmlMetaData.g_markListName[i].c_str());
   }

   generateAudioReplyInterop->markListTime = new double [xmlMetaData.g_markListTime.size()];
   if (!xmlMetaData.g_markListTime.empty())
      memcpy(generateAudioReplyInterop->markListTime, &xmlMetaData.g_markListTime[0], sizeof(double) * xmlMetaData.g_markListTime.size());


   generateAudioReplyInterop->visemeListNum = xmlMetaData.g_visemeListType.size();
   generateAudioReplyInterop->visemeListType = new char * [xmlMetaData.g_visemeListType.size()];
   for (size_t i = 0; i < xmlMetaData.g_visemeListType.size(); i++)
   {
      generateAudioReplyInterop->visemeListType[i] = new char [xmlMetaData.g_visemeListType[i].size() + 1];
      strcpy(generateAudioReplyInterop->visemeListType[i], xmlMetaData.g_visemeListType[i].c_str());
   }

   generateAudioReplyInterop->visemeListStart = new double [xmlMetaData.g_visemeListStart.size()];
   if (!xmlMetaData.g_visemeListStart.empty())
      memcpy(generateAudioReplyInterop->visemeListStart, &xmlMetaData.g_visemeListStart[0], sizeof(double) * xmlMetaData.g_visemeListStart.size());

   generateAudioReplyInterop->visemeListArticulation = new double [xmlMetaData.g_visemeListArticulation.size()];
   if (!xmlMetaData.g_visemeListArticulation.empty())
      memcpy(generateAudioReplyInterop->visemeListArticulation, &xmlMetaData.g_visemeListArticulation[0], sizeof(double) * xmlMetaData.g_visemeListArticulation.size());

   return true;
}


FESTIVAL_DLL_API bool FESTIVAL_DLL_Close()
{
   return true;
}


FESTIVAL_DLL_API const char * FESTIVAL_DLL_GetFestivalLibDir()
{
    return festival_libdir;
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
         fUnicodeForm = xercesc_3_1::XMLString::transcode( toTranscode );
      }

      ~XStr()
      {
         xercesc_3_1::XMLString::release( &fUnicodeForm );
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
   while ( (pos = message.find("  ")) != std::string::npos )
   {
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

   fprintf(stderr, "Incoming txt :%s \n",txt.c_str());
   /// Put in some defaults, and do basic cleanup just to be safe
   xmlMetaData.speechIdentifier = "sp1";
   xmlMetaData.tags.clear();
   xmlMetaData.words.clear();
   /// Start an XML parser to parse the message we have received
   xercesc_3_1::XMLPlatformUtils::Initialize();
   xercesc_3_1::XercesDOMParser *parser = new xercesc_3_1::XercesDOMParser();

   std::string truncatedTxt = txt.substr(txt.find_first_of(">")+1);
   fprintf(stderr, "Truncated  txt :%s \n",truncatedTxt.c_str());

   char * message = (char*)truncatedTxt.c_str();

   std::string actualText = "";

   /// Set up a parser for XML message in memory - code sourced from unknown online reference for Xerces XML library
   xercesc_3_1::MemBufInputSource memIS((const XMLByte*)message, strlen(message), "XMLBuffer");
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
                  //string output =
                  //(char*)  ((MemBufFormatTarget*)myFormatTarget2)->getRawBuffer();
                  //fprintf(stderr, "output = %s \n",output.c_str());

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
                  //char * t1, *t2;
                  std::string t1, t2;
                  std::string markString(t1 = XMLString::transcode(mark));
                  std::string speechString;

                  if(speechNode !=NULL)
                  {
                     t2 = (speech)?XMLString::transcode(speech): " ";
                     speechString = t2;
                     //T2 Never used... release causes error message
                     //char* T2 = &t2[0];
                     //if (speech) XMLString::release(&T2);
                  }
                  else
                     speechString = "";
                   //T1 never used... release causes error message
                  //char* T1 = &t1[0];
                  //XMLString::release(&T1);
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
                     xmlMetaData.tags.push_back(markString);
                     xmlMetaData.words.push_back(temporaryText);
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
   fprintf(stderr, "Success.. returning %s \n", actualText.c_str());
      return actualText;
   }
   else {
      fprintf(stderr, "Error: Unable to instantiate DOM Xml parser, exiting \n");
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



// generate the sound file for the specified text
std::string generateReply(const char * utterance, const char * soundFileName)
{
   string spoken_text = storeXMLMetaData( utterance );
   //printf("done first time\n");
   if (!spoken_text.compare("") && spoken_text != "")
   {
      puts(spoken_text.append("\n").c_str());
   }

   if (xmlMetaData.tags.size() <= 0)
   {
      spoken_text = TransformTextWithTimes(utterance);
      printf("done transforming\n");
      if (!spoken_text.compare("") && spoken_text != "")
      {
         puts(spoken_text.append("\n").c_str());
      }
      spoken_text = storeXMLMetaData(spoken_text);
      printf("done second time\n");
   }
   removeTabsFromString(spoken_text);
   printf( "generateReply() - \nbefore: '%s'\nafter: '%s'\n'%s'\n", utterance, spoken_text.c_str(), soundFileName );

   //festival_say_text(spoken_text.c_str());

   EST_Wave wave;
   festival_text_to_wave(spoken_text.c_str(), wave);
   wave.save(soundFileName, "riff");

   return xmlMetaData.replyString;
}
