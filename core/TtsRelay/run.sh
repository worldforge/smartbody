#!/bin/sh

#@rem unzip festival voices
#if not exist bin\lib\festival\festival\lib\voices\english goto runTTS
#pushd bin\lib\festival\festival\lib\voices\english
#call prepare.bat
#popd


#:runTTS
pushd bin/Release
mono TtsRelayGui.exe $1 $2 $3 $4 $5 $6 $7 $8
popd
