  OgreViewer build via Visual Studio 2008
===========================================

These solution and project files are ready to build OgreViewer in both the 
Release (bin/win32) and Debug (bin/win32_debug) builds.

However, when launching from Visual Studio, the default working directory is
the directory of the project file, rather than the directory of the target
executable.  Further more, this project property is not stored in the .vcproj
file, since it is considered user data and stored in the .user file.  (Microsoft
desperately needs to consider adding a default value in the .vcproj.)

To correct this, open the properties of the OgreViewer project inside Visual 
Studio.  Select "All Configurations" from the "Configuration" drop down.
In the tree, navigate to "Configuration Properties > Debugging".  In the
"Working Directory" field, type "$(OutDir)".  Save immedately via the "OK" or
"Apply" buttons.