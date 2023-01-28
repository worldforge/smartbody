/*************************************************************
Copyright (C) 2017 University of Southern California

This file is part of Smartbody.

Smartbody is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Smartbody is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Smartbody.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************/

#ifndef  __SBM_CHARACTER_LISTENER__
#define  __SBM_CHARACTER_LISTENER__
#include <string>
#include <sb/SBTypes.h>
#include <sb/SBObject.h>

namespace SmartBody
{
// This class is meant for listening to specific events that could be handled externally from smartbody
// Currently being used by smartbody-dll
class SBSceneListener
{
public:
	SBAPI SBSceneListener() = default;
	virtual SBAPI ~SBSceneListener() = default;
	virtual SBAPI void OnCharacterCreate( const std::string & name, const std::string & objectClass ) {}
	virtual SBAPI void OnCharacterDelete( const std::string & name ) {}
	virtual SBAPI void OnCharacterUpdate( const std::string & name ) {}

	virtual SBAPI void OnPawnCreate( const std::string & name ) {}
	virtual SBAPI void OnPawnDelete( const std::string & name ) {}
	
	virtual SBAPI void OnViseme( const std::string & name, const std::string & visemeName, const float weight, const float blendTime ) {}
	virtual SBAPI void OnChannel( const std::string & name, const std::string & channelName, const float value) {}
	virtual SBAPI void OnLogMessage( const std::string & message) {}

	virtual SBAPI void OnEvent( const std::string & eventName, const std::string & eventParameters ) {}

	virtual SBAPI void OnObjectCreate(SmartBody::SBObject* object) {}
	virtual SBAPI void OnObjectDelete(SmartBody::SBObject* object) {}
	
	virtual SBAPI void OnSimulationStart() {}
	virtual SBAPI void OnSimulationEnd() {}
	virtual SBAPI void OnSimulationUpdate() {}
};

}

#endif
