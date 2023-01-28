#ifndef RAPI_WIN32_LIBRARY_HEADER_FILE
#define RAPI_WIN32_LIBRARY_HEADER_FILE

#if defined _WINDOWS_

//
// Microsoft Windows Specific callback function to natively support UNICODE
//

typedef void (rapi_callback_functionW)(
                                      void *user_data,
                                      long event_type,
                                      const char *data_chunk,
                                      long data_type,
                                      long data_length,
                                      const WCHAR *phone_set,
                                      const WCHAR *cparam1,
                                      const WCHAR *cparam2
                                      );

//
// Microsoft Windows specific UNICODE functions.
//
#ifdef __cplusplus
extern "C"
{
#endif
//
// Speak (blocking).
//
// Affects internal handle state : Yes
//
long  DECLARE_RAPI_API rAPI_SpeakW(long handle, const WCHAR *voice, const WCHAR *text);
//
// Speak (non-blocking).
//
// Affects internal handle state : Yes
//
long  DECLARE_RAPI_API rAPI_Speak_asyncW(long handle, const WCHAR *voice, const WCHAR *text);
//
// Request the load on a prticular server.
//
// Affects internal handle state : No
//
float  DECLARE_RAPI_API rAPI_Get_Server_LoadW(const WCHAR *server);

//
// Is Server available?.
//
// Affects internal handle state : No
//
long  __declspec(dllexport) rAPI_Is_Server_AvailableW(long handle, const WCHAR *server);
//
// Next server voice.
//
// Affects internal handle state : Yes
//
long  DECLARE_RAPI_API rAPI_Enumerate_Server_VoicesW(long handle, const WCHAR *server);
//
// Next server.
//
// Affects internal handle state : Yes
//
DECLARE_RAPI_API const WCHAR* rAPI_Next_ServerW(long handle);
//
// Create a rAPI synthesis handle.
//
// Affects internal handle state : Yes
//
long DECLARE_RAPI_API rAPI_Create_HandleW(long events_requested, long data_type, rapi_callback_functionW *function, void *user_data);
//
// Get the next voice in an enumeration.
//
// Affects internal handle state : Yes
//
DECLARE_RAPI_API const WCHAR* rAPI_Next_VoiceW(long handle);
//
// Convert a local phone set to an approximate IPA pair.
//
// Affects internal handle state : No
//
long  DECLARE_RAPI_API rAPI_Convert_Phone_to_IPAW(const WCHAR *phone, const WCHAR *phone_set, long *IPA1, long *IPA2);
//
// Set Active Server (override registry).
//
// Call before creating any handles.
//
// Affects internal handle state : No
//
long DECLARE_RAPI_API rAPI_Set_ServersW(const WCHAR *server_list);

DECLARE_RAPI_API const WCHAR* rAPI_Get_Voice_GenderW(long handle, const WCHAR *voice);

DECLARE_RAPI_API const WCHAR* rAPI_Get_Voice_LanguageW(long handle, const WCHAR *voice);

//
// Send a control request to server.
//
long  DECLARE_RAPI_API rAPI_Send_Control_RequestW (
/* Server to shutdown               */  const WCHAR *server,
/* Server password                  */  const WCHAR *password,
/* Control request code             */  int request);

#ifdef __cplusplus
};
#endif

//_WINDOWS_
#endif

#endif
