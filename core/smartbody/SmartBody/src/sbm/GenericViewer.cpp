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

#include "GenericViewer.h"

GenericViewer::GenericViewer()
{
}

GenericViewer::GenericViewer(int x, int y, int w, int h)
{
}

GenericViewer::~GenericViewer()
{
}

void GenericViewer::label_viewer(std::string name)
{
}

void GenericViewer::show_viewer()
{
}

void GenericViewer::hide_viewer()
{
}

void GenericViewer::update_viewer()
{
}

GenericViewerFactory::GenericViewerFactory()
{
}

GenericViewer* GenericViewerFactory::create(int x, int y, int w, int h)
{
	return new GenericViewer(x, y, w, h);
}


void GenericViewerFactory::destroy(GenericViewer* viewer)
{
	delete viewer;
	viewer = NULL;
}

