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

#ifndef _SBPARSETREE_H_
#define _SBPARSETREE_H_

#include <sb/SBTypes.h>
#include <string>
#include <vector>

namespace SmartBody {

class SBParseNode
{
	public:
		SBAPI SBParseNode();
		SBAPI SBParseNode(const std::string& word, const std::string& term);
		SBAPI ~SBParseNode();

		SBAPI bool isTerminal();
		SBAPI void setWord(const std::string& word);
		SBAPI void setTerm(const std::string& term);

		SBAPI const std::string& getWord();
		SBAPI const std::string& getTerm();

		SBAPI void addChild(SBParseNode* node);
		SBAPI int getNumChildren();
		SBAPI SBParseNode* getChild(int num);

	protected:
		std::string _word;
		std::string _term;
		std::vector<SBParseNode*> _children;

};

}


#endif