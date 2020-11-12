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

#ifndef _GENERICVIEWER_
#define _GENERICVIEWER_

#include <sb/SBTypes.h>
#include <string>

class SBAPI GenericViewer
{
	public:
		GenericViewer();
		GenericViewer(int x, int y, int w, int h);
		virtual ~GenericViewer();

		virtual void label_viewer(std::string name);
		virtual void show_viewer();
		virtual void hide_viewer();
		virtual void update_viewer();

};

class SBAPI GenericViewerFactory
{
	public:
		GenericViewerFactory();
		
		virtual GenericViewer* create(int x, int y, int w, int h);
		virtual void destroy(GenericViewer* viewer);

};

#endif