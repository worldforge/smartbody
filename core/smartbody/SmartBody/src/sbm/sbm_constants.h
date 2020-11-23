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

#ifndef SR_FLAG_H
#define SR_FLAG_H

#ifndef nullptr
#define nullptr 0x0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef CMD_SUCCESS
#define CMD_SUCCESS 0
#endif
#ifndef CMD_FAILURE
#define CMD_FAILURE -1
#endif
#ifndef CMD_NOT_FOUND
#define CMD_NOT_FOUND 1
#endif


#define EMPTY_STRING	""
#define WHITE_SPACE		" \t\n\r"


#define MAX_FILENAME_LEN	(512)
#define MAX_JOINT_LABEL_LEN	(128)

#endif
