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


#ifndef __SBM_AUDIO_H
#define __SBM_AUDIO_H

#include <sb/SBTypes.h>

namespace SmartBody {
	class SBScene;
	void attachAudioToScene(SBScene& scene);
}

SBAPI bool AUDIO_Init();
float AUDIO_Play( const char * audio_file );
void AUDIO_Stop( const char * audio_file );
SBAPI void AUDIO_Close();


#endif // __SBM_AUDIO_H
