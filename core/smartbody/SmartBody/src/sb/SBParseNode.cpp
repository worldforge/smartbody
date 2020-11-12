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

#include "SBParseNode.h"

namespace SmartBody {

SBParseNode::SBParseNode()
{
	_word = "";
	_term = "";
}

SBParseNode::SBParseNode(const std::string& word, const std::string& term)
{
	_word = word;
	_term = term;
}

SBParseNode::~SBParseNode()
{
	for (size_t c = 0; c < _children.size(); c++)
	{
		delete _children[c];
	}	
}

bool SBParseNode::isTerminal()
{
	return _word == "";
}

void SBParseNode::setWord(const std::string& word)
{
	_word = word;
}

void SBParseNode::setTerm(const std::string& term)
{
	_term = term;
}

const std::string& SBParseNode::getWord()
{
	return _word;
}

const std::string& SBParseNode::getTerm()
{
	return _term;
}

void SBParseNode::addChild(SBParseNode* node)
{
	_children.push_back(node);
}

int SBParseNode::getNumChildren()
{
	return _children.size();
}

SBParseNode* SBParseNode::getChild(int num)
{
	if (num >= 0 && _children.size() > (size_t) num)
		return _children[num];
	else
		return NULL;
}

}
