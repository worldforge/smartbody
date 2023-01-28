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

#ifndef _SBRIGNODE_
#define _SBRIGNODE_


namespace SmartBody {

class SBRigNode
{
	public:
		SBRigNode();
		~SBRigNode();
};

class SBSetDrivenKeyNode : public SBRigNode
{
	public:
		SBSetDrivenKeyNode();
		~SBSetDrivenKeyNode();

		void setRange(double range);
		double getValue(double value);
			
	protected:
		double _range;
};

class SBSetDrivenKeySequenceNode : public SBRigNode
{
	public:
		SBSetDrivenKeySequenceNode();
		~SBSetDrivenKeySequenceNode();

		void setRange(double range, int numValues);
		double getValue(double value, int index);
			
	protected:
		double _range;
		int _numValues;
};

}




#endif