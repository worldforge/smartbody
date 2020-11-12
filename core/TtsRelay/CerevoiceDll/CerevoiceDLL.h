
#ifndef CEREVOICE_DLL_H
#define CEREVOICE_DLL_H


#ifdef CEREVOICE_DLL_EXPORTS
#define CEREVOICE_DLL_API __declspec(dllexport)
#else
#define CEREVOICE_DLL_API __declspec(dllimport)
#endif


#include "vhcl_public.h"


#ifdef __cplusplus
extern "C" {
#endif


struct CEREVOICE_DLL_GenerateAudioReplyInterop
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


CEREVOICE_DLL_API bool CEREVOICE_DLL_Init(const char * visemeMapping);
CEREVOICE_DLL_API void CEREVOICE_DLL_SetVisemeMapping(const char * visemeMapping);
CEREVOICE_DLL_API bool CEREVOICE_DLL_GenerateAudio(const char * message, const char * outputFilename, const char * messageOutputFileName, const char * voice, char * xmlReplyReturn, CEREVOICE_DLL_GenerateAudioReplyInterop * generateAudioReplyInterop);
CEREVOICE_DLL_API bool CEREVOICE_DLL_Close();


#ifdef __cplusplus
}
#endif

#endif  // CEREVOICE_DLL_H
