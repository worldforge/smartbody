#ifndef _BONEBUSHANDLER_H
#define _BONEBUSHANDLER_H

#include "vhcl.h"
#include <string>
#include "bonebus.h"

class BoneBusHandler
{
	public:
		static void OnClientConnect( const std::string & clientName, void * userData );
		static void OnCreateCharacter( const int characterID, const std::string & characterType, const std::string & characterName, const int skeletonType, void * userData );
		static void OnUpdateCharacter( const int characterID, const std::string & characterType, const std::string & characterName, const int skeletonType, void * userData );
		static void OnDeleteCharacter( const int characterID, void * userData );
		static void OnSetCharacterPosition( const int characterID, const float x, const float y, const float z, void * userData );
		static void OnSetCharacterRotation( const int characterID, const float w, const float x, const float y, const float z, void * userData );
		static void OnBoneRotations( const bonebus::BulkBoneRotations * bulkBoneRotations, void * userData );
		static void OnBonePositions( const bonebus::BulkBonePositions * bulkBonePositions, void * userData );
		static void OnBoneId( const int characterID, const std::string boneName, const int boneId, void * user_data );
		static void OnVisemeId( const int characterID, const std::string visemeName, const int visemeId, void * user_data ) ;
};

#endif