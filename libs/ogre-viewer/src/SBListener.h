#ifndef _SBLISTENER_H
#define _SBLISTENER_H

#include <string>

class OgreRenderer;

class SBListener
{
	public:
		SBListener(OgreRenderer* app);

		virtual void OnCharacterCreate( const std::string & name );
		virtual void OnCharacterCreate( const std::string & name, const std::string & objectClass );
		virtual void OnCharacterDelete( const std::string & name );
		virtual void OnCharacterChange( const std::string & name );
		virtual void OnPawnCreate( const std::string& name );

	protected:
		OgreRenderer* m_app;


};


#endif
