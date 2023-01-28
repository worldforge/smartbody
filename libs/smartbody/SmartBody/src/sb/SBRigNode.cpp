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

#include "SBRigNode.h"
#include "SBUtilities.h"


namespace SmartBody {

SBRigNode::SBRigNode() 
{
}

SBRigNode::~SBRigNode()
{
}

SBSetDrivenKeyNode::SBSetDrivenKeyNode() : SBRigNode()
{
}

SBSetDrivenKeyNode::~SBSetDrivenKeyNode()
{
}

void SBSetDrivenKeyNode::setRange(double range)
{
	_range = range;
}

double SBSetDrivenKeyNode::getValue(double value)
{
	if (_range == 0.0)
		return 0.0;
	return value / _range;
}

SBSetDrivenKeySequenceNode::SBSetDrivenKeySequenceNode() : SBRigNode()
{
}

SBSetDrivenKeySequenceNode::~SBSetDrivenKeySequenceNode()
{
}

void SBSetDrivenKeySequenceNode::setRange(double range, int numValues)
{
	_range = range;
	_numValues = numValues;
}

double SBSetDrivenKeySequenceNode::getValue(double value, int index)
{
	if (_range == 0.0)
		return 0.0;

	if (_numValues == 0)
		return 0.0;

	if (index >= _numValues)
	{
		SmartBody::util::log("Cannot retrieve key for node %d, only %d available.", index, _numValues);
		return 0.0;
	}
	if (index < 0)
	{
		SmartBody::util::log("Bad key value %d, only %d available.", index, _numValues);
		return 0.0;
	}

	// how big are the partitions
	double partitionSize = _range / (double) _numValues;

	// which partition are we in?
	int whichPart = (int) (value / partitionSize);

	// in the i_th part, bring the i_th value up
	if (index == whichPart)
	{
		return (value - ((double) whichPart * partitionSize)) / partitionSize;
	}

	// in the i_th part, bring the (i-1)_th value down
	if (index == whichPart - 1)
	{
		return 1.0 - (value - ((double) whichPart * partitionSize)) / partitionSize;
	}

	return 0.0;
}

}
