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

#ifndef _SBASSET_H_
#define _SBASSET_H_

#include <sb/SBTypes.h>
#include <sb/SBObject.h>

namespace SmartBody {

class SBAsset : public SBObject
{
	public:
		SBAPI SBAsset();
		SBAPI ~SBAsset();

		SBAPI bool isLoaded();
		SBAPI void load();
		SBAPI void unload();
		SBAPI std::string getFullFilePath();
		SBAPI void setFullFilePath(std::string filename);

	protected:
		bool _loaded;
		std::string fullFilePath;		
};

}

#endif