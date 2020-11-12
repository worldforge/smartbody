Instructions for using the SmartBody SDK for OSX
------------------------------------------------

Ari Shapiro, Ph.D. 12/15/14

Quick Start
-----------
Go to the SmartBodySDK/bin folder.
Run the startSBGUI.command program.
The sbgui executable is a graphical user interface to the SmartBody engine. 
Please see the SmartBody manual for how to use it.

To run the simple smartbody program, run the startSimpleSmartBody.command file.
The simplesmartbody executable is an example of how to connect to and interact with the SmartBody data.


How to Build SmartBody
----------------------
In the SmartBodySDK folder, run 
./buildCMake.sh 
This will build SmartBody from source using CMake (but not using XCode).
The build currently targets OSX 10.9.
Source code for SmartBody is located in the src/ folder, and headers for the dependent libraries are in the include/ folder.



