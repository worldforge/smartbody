#include "SBWindowListener.h"
#include <sb/SBScene.h>
#include "Session.h"

SBWindowListener::SBWindowListener()
{
}

SBWindowListener::~SBWindowListener()
{
	Session::current->scene.removeSceneListener(this);
}

void SBWindowListener::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{
}

void SBWindowListener::OnCharacterDelete( const std::string & name )
{
}

void SBWindowListener::OnCharacterUpdate( const std::string & name )
{
}
      
void SBWindowListener::OnPawnCreate( const std::string & name )
{
}

void SBWindowListener::OnPawnDelete( const std::string & name )
{
}

void SBWindowListener::OnSimulationStart()
{
}

void SBWindowListener::OnSimulationEnd()
{
}

void SBWindowListener::OnSimulationUpdate()
{
}

void SBWindowListener::OnObjectCreate( SmartBody::SBObject* object )
{
}

void SBWindowListener::OnObjectDelete( SmartBody::SBObject* object )
{
}

void SBWindowListener::windowShow()
{
	Session::current->scene.addSceneListener(this);
}

void SBWindowListener::windowHide()
{
	Session::current->scene.removeSceneListener(this);
}



