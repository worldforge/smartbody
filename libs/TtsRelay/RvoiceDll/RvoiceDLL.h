
#ifndef RVOICE_DLL_H
#define RVOICE_DLL_H


#ifdef RVOICE_DLL_EXPORTS
#define RVOICE_DLL_API __declspec(dllexport)
#else
#define RVOICE_DLL_API __declspec(dllimport)
#endif


#include "vhcl_public.h"


#ifdef __cplusplus
extern "C" {
#endif


struct RVOICE_DLL_GenerateAudioReplyInterop
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


RVOICE_DLL_API bool RVOICE_DLL_Init(const char * visemeMapping);
RVOICE_DLL_API void RVOICE_DLL_SetVisemeMapping(const char * visemeMapping);
RVOICE_DLL_API bool RVOICE_DLL_GenerateAudio(const char * message, const char * outputFilename, const char * messageOutputFileName, const char * voice, char * xmlReplyReturn, RVOICE_DLL_GenerateAudioReplyInterop * generateAudioReplyInterop);
RVOICE_DLL_API bool RVOICE_DLL_Close();


#ifdef __cplusplus
}
#endif

#endif  // RVOICE_DLL_H
