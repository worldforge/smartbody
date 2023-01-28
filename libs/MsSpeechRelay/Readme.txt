                            MS Speech Relay
                     University of Southern California
                    Institute for Creative Technologies
                                 ©2009
             ==============================================

MS Speech Relay is a VHuman compatible front-end for Microsoft SAPI speech
synthesis.  This package does not include a copy of SAPI. SAPI is included
with Windows XP and later. The details for the Microsoft Speech API (v5.3) can be found at:
http://msdn.microsoft.com/en-us/library/ms723627(VS.85).aspx

Compiling:
This C# code uses the Managed API (System.Speech.Synthesis)

The included C# project files expect the Vhmsg library to be
installed at:
    ..\..\lib\vhmsg\vhmsg-net

MS Speech Relay is also intended as an example of how to implement
a speech audio and viseme server for VHuman's RemoteSpeech message
protocol.

Dependencies:
ActiveMQ: Apache.NMS.ActiveMQ.dll, Apache.NMS.dll
ElUtil: elutil.net.dll

Usage:
Type "MsSpeechRelay -h" or "MsSpeechRelay --help" on the command line for program options

The current version is functional, and includes support for choosing different voices. It
responds to VHuman messages(vrAllCall, RemoteSpeechCmd) and can be terminated remotely(by vrKillComponent rvoice {all|msspeechrelay}). The SAPI TTS engine is fully SSML compatible,
and correspondingly, MS Speech Relay can handle all SSML tags. The code does not cache any
audio. The code is simple, and amply documented :)