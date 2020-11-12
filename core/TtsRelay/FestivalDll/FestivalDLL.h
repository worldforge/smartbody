
#ifndef FESTIVAL_DLL_H
#define FESTIVAL_DLL_H


#ifdef WIN_BUILD
    #ifdef FESTIVAL_DLL_EXPORTS
    #define FESTIVAL_DLL_API __declspec(dllexport)
    #else
    #define FESTIVAL_DLL_API __declspec(dllimport)
    #endif
#else
#define FESTIVAL_DLL_API
#endif


#include "vhcl_public.h"


#ifdef __cplusplus
extern "C" {
#endif


struct FESTIVAL_DLL_GenerateAudioReplyInterop
{
   char * soundFile;
   size_t workBreakListNum;
   double * wordBreakListStart;
   double * wordBreakListEnd;
   size_t markListNum;
   char ** markListName;
   double * markListTime;
   size_t visemeListNum;
   char ** visemeListType;
   double * visemeListStart;
   double * visemeListArticulation;
};


FESTIVAL_DLL_API bool FESTIVAL_DLL_Init(const char * visemeMapping);
FESTIVAL_DLL_API void FESTIVAL_DLL_SetVisemeMapping(const char * visemeMapping);
FESTIVAL_DLL_API bool FESTIVAL_DLL_GenerateAudio(const char * message, const char * outputFilename, const char * messageOutputFileName, const char * voice, char * xmlReplyReturn, FESTIVAL_DLL_GenerateAudioReplyInterop * generateAudioReplyInterop);
FESTIVAL_DLL_API bool FESTIVAL_DLL_Close();

FESTIVAL_DLL_API const char * FESTIVAL_DLL_GetFestivalLibDir();


#ifdef __cplusplus
}
#endif

#endif  // FESTIVAL_DLL_H
