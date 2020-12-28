/*
   This file is part of VHMsg written by Edward Fast at 
   University of Southern California's Institute for Creative Technologies.
   http://www.ict.usc.edu
   Copyright 2008 Edward Fast, University of Southern California

   VHMsg is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VHMsg is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with VHMsg.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <conio.h>
#include <string>

#include "bonebus.h"

using std::string;


void OnClientConnect( string clientName, void * userData )
{
   printf( "Client Connected! - %s\n", clientName.c_str() );
}


void OnCreateCharacter( const int characterID, const std::string characterType, const std::string characterName, const int skeletonType, void * userData )
{
   printf( "Character Create! - %d, %s, %s, %d\n", characterID, characterType.c_str(), characterName.c_str(), skeletonType );
}


void OnDeleteCharacter( const int characterID, void * userData )
{
   printf( "Character Delete! - %d\n", characterID );
}


void OnSetCharacterPosition( const int characterID, const float x, const float y, const float z, void * userData )
{
   printf( "Set Character Position! - %d - %5.2f %5.2f %5.2f\n", characterID, x, y, z );
}


void OnSetCharacterRotation( const int characterID, const float w, const float x, const float y, const float z, void * userData )
{
   printf( "Set Character Rotation! - %d - %5.2f %5.2f %5.2f %5.2f\n", characterID, w, x, y, z );
}


void OnBoneRotations( const BoneBusServer::BulkBoneRotations * bulkBoneRotations, void * userData )
{
   printf( "Set Bone Rotations! - %d %d %d\n", bulkBoneRotations->packetId, bulkBoneRotations->charId, bulkBoneRotations->numBoneRotations );
}


void OnBonePositions( const BoneBusServer::BulkBonePositions * bulkBonePositions, void * userData )
{
   printf( "Set Bone Positions! - %d %d %d\n", bulkBonePositions->packetId, bulkBonePositions->charId, bulkBonePositions->numBonePositions );
}


int main()
{
   BoneBusServer bonebus;

   bonebus.SetOnClientConnectCallback( OnClientConnect, NULL );
   bonebus.SetOnCreateCharacterFunc( OnCreateCharacter, NULL );
   bonebus.SetOnDeleteCharacterFunc( OnDeleteCharacter, NULL );
   bonebus.SetOnSetCharacterPositionFunc( OnSetCharacterPosition, NULL );
   bonebus.SetOnSetCharacterRotationFunc( OnSetCharacterRotation, NULL );
   bonebus.SetOnBoneRotationsFunc( OnBoneRotations, NULL );
   bonebus.SetOnBonePositionsFunc( OnBonePositions, NULL );
   bonebus.OpenConnection();

   while ( 1 )
   {
      bonebus.Update();

      if ( _kbhit() )
      {
         int key = _getch();

         if ( key == 'q' )
         {
            break;
         }
      }
   }


   bonebus.CloseConnection();

   return 0;
}
