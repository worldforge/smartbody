/*
*  main.cpp - part of SmartBody Project's OgreViewer
*  Copyright (C) 2009  University of Southern California
*
*  SmartBody is free software: you can redistribute it and/or
*  modify it under the terms of the Lesser GNU General Public License
*  as published by the Free Software Foundation, version 3 of the
*  license.
*s
*  SmartBody is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  Lesser GNU General Public License for more details.
*
*  You should have received a copy of the Lesser GNU General Public
*  License along with SmartBody.  If not, see:
*      http://www.gnu.org/licenses/lgpl-3.0.txt
*
*/

#include "vhcl.h"
#include "OgreRenderer.h"
#include <vector>
#include <string>
#include "ini.h"
#include "bonebus.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
std::string macBundlePath()
{
    char path[1024];
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    assert(mainBundle);
    
    CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
    assert(mainBundleURL);
    
    CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
    assert(cfStringRef);
    
    CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);
    
    CFRelease(mainBundleURL);
    CFRelease(cfStringRef);
    
    return std::string(path);
}
#endif

static int inihandler(void* user, const char* section, const char* name,
                   const char* value)
{
	OgreRenderer* renderer = (OgreRenderer*) user;

#ifdef WIN32
    #define MATCH(s, n) _stricmp(section, s) == 0 && _stricmp(name, n) == 0
#else
    #define MATCH(s, n) strcasecmp(section, s) == 0 && strcasecmp(name, n) == 0
#endif
    if (MATCH("GENERAL", "UseBoneBus"))
	{
		std::string boneBusValue(value);
		if (boneBusValue == "true" || boneBusValue == "True" || boneBusValue == "TRUE")
		{
			renderer->setUseBoneBus(true);
		}
		else if (boneBusValue == "false" || boneBusValue == "False" || boneBusValue == "FALSE")
		{
			renderer->setUseBoneBus(false);
		}
    }
	else if (MATCH("GENERAL", "DefaultPyFile"))
	{
		std::string temp = "python scene.run(\"" + std::string(value) + "\")";
		renderer->m_initialCommands.emplace_back(temp);
	}
	else if (MATCH("GENERAL", "ScriptPath"))
	{
		std::string temp = "python scene.addAssetPath(\"script\", \"" + std::string(value) + "\")";
		renderer->m_initialCommands.emplace_back(temp);
    }
	else if (MATCH("GENERAL", "MediaPath"))
	{
		std::string temp = "python scene.setMediaPath(\"" + std::string(value) + "\")";
		// make sure that the media path is run first
		renderer->m_initialCommands.insert(renderer->m_initialCommands.begin(), temp);
    }
	else if (MATCH("GENERAL", "PythonLibPath"))
	{
		std::string temp = "python scene.setMediaPath(\"" + std::string(value) + "\")";
		// make sure that the media path is run first
		renderer->m_initialCommands.insert(renderer->m_initialCommands.begin(), temp);
    }
	else if (MATCH("GENERAL", "Scene"))
	{
		renderer->m_initialMeshName = std::string(value);
    }
	else
	{
        return 0;  /* unknown section/name, error */
    }
    return 1;
}


//#undef WIN32
#ifdef WIN32
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#else
int main(int argc, char* argv[])
#endif
{
		// Create application object
	OgreRenderer app;

	app.setUseBoneBus(true);

	// Processing command line arguments, if there are, then use dll mode
#ifdef WIN32
	std::string commandLine = lpCmdLine;
	std::stringstream strstr(commandLine);
#else
	std::stringstream strstr;
	for (int i = 0; i < argc; i++)
		strstr << argv[i] << " ";
#endif
	std::istream_iterator<std::string> it(strstr);
	std::istream_iterator<std::string> end;
	std::vector<std::string> tokenzied(it, end);
	int numTokens = tokenzied.size();
	int tokenCounter = 0;
	// if no parameters are given, look for a file named config.ini in the local path
	bool implicitConfig = false;
	if (numTokens == 0)
	{
		tokenzied.emplace_back("-config");
		tokenzied.emplace_back("config.ini");
		numTokens = 2;
		implicitConfig = true;
	}
	while (tokenCounter < numTokens)
	{
		std::string op = tokenzied[tokenCounter];
		if (op == "-config")
		{
			tokenCounter++;
			if (tokenCounter < numTokens)
			{
				std::vector<std::string> commands;
				if (ini_parse(tokenzied[tokenCounter].c_str(), inihandler, &app) < 0)
				{
					if (!implicitConfig)
						printf("Can't load configuration file '%s'\n", tokenzied[tokenCounter].c_str());
					continue;
				}
			}
		}
		
		if (op == "-seqpath" || op == "-scriptpath")
		{
			tokenCounter++;
			if (tokenCounter < numTokens)
			{
				std::string command = "python scene.addAssetPath(\"script\", \"" + tokenzied[tokenCounter] + "\")";
				app.m_initialCommands.emplace_back(command);
				app.setUseBoneBus(false);
			}
		}
		if (op == "-seq")
		{
			tokenCounter++;
			if (tokenCounter < numTokens)
			{
				std::string command = "seq " + tokenzied[tokenCounter];
				printf("-------------- command = %s\n",command.c_str());;
				app.m_initialCommands.emplace_back(command);
			}
		}
		if (op == "-script")
		{
			tokenCounter++;
			if (tokenCounter < numTokens)
			{
				std::string command = "python scene.run(\"" + tokenzied[tokenCounter] + "\")";
				app.m_initialCommands.emplace_back(command);
				app.setUseBoneBus(false);
			}
		}
		if (op == "-mediapath")
		{
			tokenCounter++;
			if (tokenCounter < numTokens)
			{
				std::string command = "python scene.setMediaPath(\"" + tokenzied[tokenCounter] + "\")";
				app.m_initialCommands.emplace_back(command);
				app.setUseBoneBus(false);
			}
		}
		if (op == "-scene")
		{
			tokenCounter++;
			if (tokenCounter < numTokens)
			{
				app.m_initialMeshName = tokenzied[tokenCounter];
			}
		}
		if (op.size() > 11 && op.substr(0,11)  == "-mediapath=")
		{
			std::string mediaPath = op.substr(11);
			if (tokenCounter < numTokens)
			{
				std::string command = "python scene.setMediaPath(\"" + mediaPath + "\")";
				app.m_initialCommands.emplace_back(command);
			}
		}
		if (op == "-help")
		{
			printf("Usage: OgreViewer [-config filename] [-scriptpath scriptpathname] [-script scriptname] [-mediapath mediapathname] [-scene startupmeshname]\n");
			printf("If the OgreViewer is not passed any parameters, it will look for a configuration file called 'config.ini' in the current direction.\n");
			printf("If the OgreViewer is not passed any startup scripts or cannot find a config.ini, it will start in BoneBus mode and attempt to connect to SmartBody process.\n");
			printf("If no SmartBody process is available, the OgreViewer will wait and render SmartBody characters after such a process is started.\n");
			printf("\n");
			printf("The following is a sample configuration script:\n");
			printf("\n");
			printf("[GENERAL]\n");
			printf("UseBoneBus=false\n");
			printf("ScriptPath=sbm-common/scripts\n");
			printf("[DefaultPyFile=default-init.py\n");
			printf("[MediaPath=../../../../data\n");
			printf("[PythonLibPath=.\n");
			printf("[Scene=vh_basic_level.mesh\n");
			
			return 0;
		}
		tokenCounter++;
	}


	if (app.m_initialCommands.size() > 0)
	{
		if (app.isUseBoneBus())
		{
			printf("OgreViewer is running in BoneBus mode.");
			app.m_initialCommands.clear();
		}
	}

	if (app.m_initialCommands.size() > 0)
		app.setUseBoneBus(false);

	try
	{
		app.go();
	}
	catch ( Exception & e )
	{
#ifdef WIN32
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		printf("An exception has occured: %s", e.getFullDescription().c_str());
#endif
	}

	return 0;
}
