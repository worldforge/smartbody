#ifndef _SBLISTENER_H
#define _SBLISTENER_H

#include "vhcl.h"
#include "smartbody-dll.h"
#include <string>
#include "OgreDemoApp.h"
#include "OgreFramework.h"

class DemoApp;

class SBListener : public SmartbodyListener
{
	public:
		SBListener(DemoApp* app);

		virtual void OnCharacterCreate( const std::string & name );
		virtual void OnCharacterCreate( const std::string & name, const std::string & objectClass );
		virtual void OnCharacterDelete( const std::string & name );
		virtual void OnCharacterChange( const std::string & name );

	protected:
		DemoApp* m_app;


};


#endif