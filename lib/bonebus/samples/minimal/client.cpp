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

#include "bonebus.h"


int main()
{
   BoneBusClient bonebus;

   bonebus.OpenConnection( "localhost" );


   BoneBusCharacter * character = NULL;

   while ( 1 )
   {

      bonebus.Update();

      if ( _kbhit() )
      {
         int key = _getch();

         if ( key == '1' )
         {
            character = bonebus.CreateCharacter( "Ed", "DoctorNew", false );
         }
         else if ( key == 'b' )
         {
            character->StartSendBoneRotations();
            character->AddBoneRotation( "spine3", 1, 0, (float)rand(), 0 );
            character->EndSendBoneRotations();
         }
         else if ( key == 'p' )
         {
            character->StartSendBonePositions();
            character->AddBonePosition( "spine3", (float)( rand() % 20 ), 0, 0 );
            character->EndSendBonePositions();
         }
         else if ( key == 'r' )
         {
            character->SetRotation( (float)( rand() % 20 ), 1, 0, 0 );
         }
         else if ( key == 't' )
         {
            character->SetPosition( 1, 0, (float)( rand() % 30 ) );
         }
         else if ( key == 'd' )
         {
            bonebus.DeleteCharacter( character );
         }
         else if ( key == 'q' )
         {
            break;
         }
      }
   }


   bonebus.CloseConnection();

   return 0;
}
