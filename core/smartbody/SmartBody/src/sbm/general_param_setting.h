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

#ifndef SBM_GENERAL_PARAM_SETTING_HPP
#define SBM_GENERAL_PARAM_SETTING_HPP

#include <map>
#include <string>
#include <vector>


///////////////////////////////////////////////////////////////
struct GeneralParam{
	int size;
	std::vector<std::string> char_names;
};

///////////////////////////////////////////////////////////////
//			<ExtraDataName, Shader(type, size and value)>
typedef std::map< std::string, GeneralParam * > GeneralParamMap;


#endif // SBM_SHADER_SETTING_HPP