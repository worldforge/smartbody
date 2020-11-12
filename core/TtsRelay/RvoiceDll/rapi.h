#ifndef RAPI_LIBRARY_HEADER_FILE
#define RAPI_LIBRARY_HEADER_FILE

//
// Core RAPI Success and Failure codes.
//
#define RAPI_SUCCESS			 0x01
#define RAPI_FAIL			 0x00
#define RAPI_FAILED_TO_ALLOCATE_MEMORY	-0x01
#define RAPI_FAIL_POINTER_ARG	        -0x02
#define RAPI_INVALID_ARG		-0x03
#define RAPI_UNHANDLED_EXCEPTION	-0x04
#define RAPI_UNHANDLED_USER_EXCEPTION	-0x05
#define RAPI_PERMISSION_DENIED      	-0x06
#define RAPI_SERVER_LICENCE_FAILURE     -0x07
#define RAPI_SERVER_INVALID_DATA        -0x08
#define RAPI_SERVER_NOT_AVAILABLE       -0x09
#define RAPI_VOICE_NOT_AVAILABLE        -0x0A
//
// RAPI calback events
//
#define RAPI_AUDIO_EVENT                 0x10
#define RAPI_WORD_BOUNDARY		 0x06
#define RAPI_PHON_BOUNDARY		 0x07
#define RAPI_BOOKMARK_EVENT		 0x0C
#define RAPI_FINISHED_EVENT		 0x0E

//
// RAPI stream data types

#define RAPI_PCM_16bit_22050Hz		 0x0F
#define RAPI_PCM_16bit_16KHz		 0x0D
#define RAPI_PCM_16bit_8KHz		 0x01
#define RAPI_MULAW_8bit_8KHz		 0x02
#define RAPI_ALAW_8bit_8KHz              0x0E

//
// RAPI Server Status codes
//
#define RAPI_SERVER_AVAILABLE		 0x01
#define RAPI_SERVER_UNAVAILABLE		-0x01
#define RAPI_SERVER_NOT_KNOWN		 0x00

//
// RAPI Server control codes
//
#define RAPI_CONTROL_SHUTDOWN_SERVER     0x00
#define RAPI_CONTROL_UPDATE_SERVER       0x01

//
// RAPI Event Request codes
//
#define RAPI_NO_EVENTS			 0x00
#define RAPI_ALL_EVENTS			 0x01
//
// RAPI XML Capabilities
//
// (Note that setting these is dependent on version of rVoice server.)
//
#define RAPI_NO_XML                      0x00
#define RAPI_XML_TAGS                    0x01
//
// RAPI Legacy Protocol capabilities
//
// (Note that these can only be used if all instances of a particular voice
// and/or single request are stored on a particular level of legacy server).
#define RAPI_NO_LEGACY_PROTOCOL          0x00
#define RAPI_PROTOCOL_1                  0x01



#if defined WIN32
#define DECLARE_RAPI_API __declspec(dllexport)
#else
#define DECLARE_RAPI_API
#endif

//
// RAPI Viseme definitions.
//

#define RAPI_VISEME_0 0
#define RAPI_VISEME_1 1
#define RAPI_VISEME_2 2
#define RAPI_VISEME_3 3
#define RAPI_VISEME_4 4
#define RAPI_VISEME_5 5
#define RAPI_VISEME_6 6
#define RAPI_VISEME_7 7
#define RAPI_VISEME_8 8
#define RAPI_VISEME_9 9
#define RAPI_VISEME_10 10
#define RAPI_VISEME_11 11
#define RAPI_VISEME_12 12
#define RAPI_VISEME_13 13
#define RAPI_VISEME_14 14
#define RAPI_VISEME_15 15
#define RAPI_VISEME_16 16
#define RAPI_VISEME_17 17
#define RAPI_VISEME_18 18
#define RAPI_VISEME_19 19
#define RAPI_VISEME_20 20
#define RAPI_VISEME_21 21

#define RAPI_NO_VISEME RAPI_VISEME_0

//
// RAPI callback function definitions.
//

typedef void (rapi_callback_function)(
/* User supplied data pointer   */  void *user_data,
/* Type of event                */  long event_type,
/* PCM or MULAW data            */  const char *data_chunk,
/* Type of data passed in       */  long data_type,
/* Length of data passed in     */  long data_length,
/* Phone set used by this data  */  const char *phone_set,
/* Event specific information   */  const char *cparam1,
/* Event specific information   */  const char *cparam2);

//
// Core RAPI functions.
//
#ifdef __cplusplus
extern "C"
{
#endif

//
// Create a rAPI synthesis handle.
//
// Affects internal handle state : Yes
//
long DECLARE_RAPI_API rAPI_Create_Handle(
/* Need phone and word events       */  long events_requested,
/* Data type required               */  long data_type,
/* User callback function           */  rapi_callback_function *function,
/* User data                        */  void *user_data);

//
// Close a rAPI synthesis handle.
//
// Affects internal handle state : Yes
//
long  DECLARE_RAPI_API rAPI_Close_Handle(
/* Synthesis handle                 */  long handle);

//
// Get rAPI Version.
//
// Affects internal handle state : No
//
long  DECLARE_RAPI_API rAPI_Version_Major();
long  DECLARE_RAPI_API rAPI_Version_Minor();

//
// Get Last Error.
//
long  DECLARE_RAPI_API rAPI_Get_Last_Error(
/* Synthesis handle                 */  long handle);

//
// rAPI Clean up before exit.
//
// Affects internal handle state : Yes
//
void DECLARE_RAPI_API rAPI_Clean_Up();
//
// Enumerate all known servers.
//
// Affects internal handle state : Yes
//
long  DECLARE_RAPI_API rAPI_Enumerate_Servers(
/* Synthesis handle	                */  long handle);

//
// Next server.
//
// Affects internal handle state : Yes
//
DECLARE_RAPI_API const char* rAPI_Next_Server(
/* Synthesis handle                 */  long handle);

//
// Enumerate server voices.
//
// Affects internal handle state : Yes
//
long  DECLARE_RAPI_API rAPI_Enumerate_Server_Voices(
/* Synthesis handle                 */  long handle,
/* Server to check                  */  const char *server);
//
// Get next voice.
//
// Affects internal handle state : Yes
//
DECLARE_RAPI_API const char* rAPI_Next_Voice(
/* Synthesis handle                 */  long handle);

//
// Is Server available?.
//
// Affects internal handle state : No
//
long  DECLARE_RAPI_API rAPI_Is_Server_Available(
/* Synthesis handle                 */  long handle,
/* Server to check                  */  const char *server);

//
// Enumerate all known voices.
//
// Affects internal handle state : Yes
//
long  DECLARE_RAPI_API rAPI_Enumerate_Voices(
/* Synthesis handle                 */  long handle);

//
// Speak (blocking).
//
// Affects internal handle state : Yes
//
long  DECLARE_RAPI_API rAPI_Speak(
/* Synthesis handle                 */  long handle,
/* Voice to use                     */  const char *voice,
/* Text to synthesise               */  const char *text);

//
// Speak (non-blocking).
//
// Affects internal handle state : Yes
//
long  DECLARE_RAPI_API rAPI_Speak_async(
/* Synthesis handle                 */  long handle,
/* Voice to use                     */  const char *voice,
/* Text to synthesise               */  const char *text);

//
// Convert a local phone set to an approximate IPA pair.
//
// Affects internal handle state : No
//
long  DECLARE_RAPI_API rAPI_Convert_Phone_to_IPA(
/* Phone to convert                 */  const char *phone,
/* Phone set to use                 */  const char *phone_set,
/* Returned IPA symbol1             */  long *IPA1,
/* Returned IPA symbol2             */  long *IPA2);

//
// Convert IPA symbols to an appoximate viseme.
//
// Affects internal handle state : No
//
long  DECLARE_RAPI_API rAPI_Convert_IPA_to_Visemes(
/* First IPA symbol                 */  long IPA1,
/* Second IPA symbol                */  long IPA2,
/* Returned first viseme            */  long *viseme1,
/* Returned second viseme           */  long *viseme2);
//
// Get server load.
//
// Affects internal handle state : No
//
float  DECLARE_RAPI_API rAPI_Get_Server_Load(
/* Server to check                  */  const char *server);

//
// Set Active Server (override registry).
//
// Call before creating any handles.
//
// Affects internal handle state : No
//
long DECLARE_RAPI_API rAPI_Set_Servers(
/* Servers to check                 */  const char *server_list);

//
// Override registry timeout.
//
// Call before Creating any handles.
//
// Affects internal handle state : No
//
void DECLARE_RAPI_API rAPI_Set_Timeout(
/* New timeout                      */  long time);

//
// Set XML capability of input
//
long  DECLARE_RAPI_API rAPI_Set_XML(
/* Synthesis handle                 */  long handle,
/* Is the input XML?                */  long xml_type);

//
// Retreives the gender of a particular voice.
//
DECLARE_RAPI_API const char* rAPI_Get_Voice_Gender(
/* Synthesis handle                 */  long handle,
/* The voice concerned              */  const char *voice);

//
// Retreives the LangID (Win32) of a particular voice.
//
DECLARE_RAPI_API const char* rAPI_Get_Voice_LangID(
/* Synthesis handle                 */             long handle,
/* The voice concerned              */             const char *voice);

//
// Retreives the ISO LangID of a particular voice.
//
DECLARE_RAPI_API const char* rAPI_Get_Voice_ISO_LangID(
/* Synthesis handle                 */             long handle,
/* The voice concerned              */             const char *voice);

//
// Retreives the UUID of a particular voice.
//
DECLARE_RAPI_API const char* rAPI_Get_Voice_UUID(
/* Synthesis handle                 */           long handle,
/* The voice concerned              */           const char *voice);

//
// Retreives the description of a particular voice.
//
DECLARE_RAPI_API const char* rAPI_Get_Voice_Description(
/* Synthesis handle                 */                  long handle,
/* The voice concerned              */                  const char *voice);


//
// Request that a handle only ever uses a particular server that is in the 
// SetServers list.
//
// Calling this function with a NULL argument resets the handle so
// that it will use any server available.
//
long  DECLARE_RAPI_API rAPI_Use_Server(
/* Synthesis handle                 */  long handle,
/* The Server's name                */  const char *server_name);

//
// Set the length of the delay between sentences in milliseconds.
//
long  DECLARE_RAPI_API rAPI_Set_Delay_Length(
/* Synthesis handle                 */  long handle,
/* The length of the sentence delay */  const long sentence_delay);

//
// Set the global volume, where 0 is the lowest value and 100 is the highest
//
long DECLARE_RAPI_API rAPI_Set_Volume(
/* Synthesis handle                 */  long handle,
/* The new volume level             */  const long global_volume);

//
// Set the global pitch, where -10 is the lowest value and 10 is the highest
//
long DECLARE_RAPI_API rAPI_Set_Pitch(
/* Synthesis handle                 */  long handle,
/* The new pitch                    */  const long pitch);

//
// Set the global rate, where -10 is the lowest value and 10 is the highest
//
long DECLARE_RAPI_API rAPI_Set_Rate(
/* Synthesis handle                 */  long handle,
/* The new rate                     */  const long rate);

//
//Set the real time value for the synthesis
//
long DECLARE_RAPI_API rAPI_Set_Realtime(
/* Synthesis handle                 */  long handle,
/* The new realtime value           */  const long realtime);

//
// Cancel a Speak request on a handle. Can be called inside a RAPI callback 
// function to halt processing. This will only abort blocking (non-async) 
// Speak calls once audio has started to be returned from the server. It 
// will not abort a blocking Speak call during the initial time before audio 
// is received.
//
long DECLARE_RAPI_API rAPI_Abort_Speak(
/* Synthesis handle                 */  long handle);

//
// Retreives the language of a particular voice.
//
DECLARE_RAPI_API const char* rAPI_Get_Voice_Language(
/* Synthesis handle                 */  long handle,
/* The voice concerned              */  const char *voice);

//
// Send a control command to a specified server
// Valid commands are: 
//      RAPI_CONTROL_SHUTDOWN_SERVER
//      RAPI_CONTROL_UPDATE_SERVER
//
// Return value is one of 
//      RAPI_INVALID_ARG 
//      RAPI_FAIL
//      RAPI_SUCCESS
//      RAPI_PERMISSION_DENIED
//
// Affects internal handle state : Yes
//

long DECLARE_RAPI_API rAPI_Send_Control_Request(
/* Server                           */  const char *server,
/* Server password                  */  const char *password,
/* Request to send                  */  int req);


//////////////////////////////////////////////////////////////////////////////
//
// The Following Functions are Deprecated and have no effect.
//
//////////////////////////////////////////////////////////////////////////////

//
// Set legacy protocol level.
//
long  DECLARE_RAPI_API rAPI_Set_Protocol(
/* Synthesis handle                 */  long handle,
/* Set protocol type                */  long protocol);

//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
};
#endif

#if defined WIN32
#include "rapiw32.h"
#endif

#endif
