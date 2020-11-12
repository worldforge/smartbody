                             Cepstral Relay
                     University of Southern California
                    Institute for Creative Technologies
                                 ©2008
             ==============================================

Cepstral Relay is a VHuman compatible front-end for Cepstral speech
synthesis.  This package does not include a copy of Cepstral.  You will
need to license their SDK to build this software.  Please visit their
website at:
   http://www.cepstral.com/

The included VisualC project files expect the CereVoice SDK to be
installed at:
    ..\..\lib\cepstral

Cepstral Relay is also intended as an example of how to implement
a speech audio and viseme server for VHuman's RemoteSpeech message
protocol.



The current version of Cepstral Relay is very early.  While it is
functional, successfully processing RemoteSpeechCmd messages and
sending RemoteSpeechReply messages, it lacks many feature we believe
are critical for a production environment:

1.) There is no clean-up or exit hooks, leading to numerous memory
    leaks.  A good start would involve moving the cepstral_tts globals
    into the cepstral_tts class, and adding uninit() and destructor
    methods.

2.) There is no support for the VHuman component messages.  These
    messages make it practical to control a multi-process, distributed
    simulation environment.  For more information, see the wiki
    documentation on these messages:
        http://smartbody.wiki.sourceforge.net/VHuman+Component+Messages

3.) It does not implement caching.  Caching the synthesized audio and
    timing data has proven very useful for improving VHuman response
    time, to the point where we believe it is critical when using
    speech synthesis in a realtime conversational environment.
    When addressing this issue, one should start by utilizing unique
    filenames for the audio files, rather than reusing the same file.

4.) The code has little documentation.  :(


