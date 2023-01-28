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

#include "SBParser.h"
#define _USE_MATH_DEFINES
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
#include <parser/GotIter.h>
#include <parser/InputTree.h>
#include <parser/Bchart.h>
#include <parser/ECArgs.h>
#include <parser/MeChart.h>
#include <parser/extraMain.h>
#include <parser/AnsHeap.h>
#include <parser/Params.h>
#include <parser/TimeIt.h>
#include <parser/ewDciTokStrm.h>
#include <parser/Link.h>

#include <sb/SBParseNode.h>
#include "SBUtilities.h"

#define log2(x) (log (x) / M_LN2)

MeChart* curChart;

namespace SmartBody {

SBParserListener::SBParserListener()
{
}

void SBParserListener::onWord(std::string timeMarker, std::string word, std::string emphasis)
{
}

void SBParserListener::onPartOfSpeech(std::string timeMarker, std::string partOfSpeech)
{
}

std::string SBParserListener::getCurrentBML()
{
	return _bml;
}

void SBParserListener::addBML(std::string bml)
{
	_bml.append(bml);
}

void SBParserListener::resetBML()
{
	_bml = "";
}


SBParser::SBParser()
{
	_initialized = false;
}

SBParser::~SBParser()
{
}

void SBParser::initialize(const std::string& param1, const std::string& param2)
{
	int argc = 3;
	std::string arg0 = "";
	std::string arg1 = param1;
	std::string arg2 = param2;
	char** argv = new char*[3];
	argv[0] = (char*) arg0.c_str();
	argv[1] = (char*) arg1.c_str();
	argv[2] = (char*) arg2.c_str();

	SmartBody::util::log("starting Charniak parser..");

	ECArgs args( argc, argv );

	params.init( args );
	TimeIt timeIt;
	ECString  path( args.arg( 0 ) );
	generalInit(path);

	_initialized = true;
}


SmartBody::SBParseNode* SBParser::parse(const std::string& input)
{
	if (!_initialized)
	{
		SmartBody::util::log("Cannot parse until parser has been initialized.");
		return nullptr;
	}

	std::string result;


	int totUnparsed = 0;
	double log600 = log2(600.0);



	SmartBody::util::log("parser ready");

	std::string string_to_parse = input;
	int      sentenceCount = 0;  //counts all sentences so we can use e.g,1/50;
	std::istringstream iss (istringstream::in);	
	istream* nontokStream = &iss;

	//SmartBody 
	std::ostringstream oss (ostringstream::out);

	ECString flnm = "dummy";
	ewDciTokStrm* tokStream = nullptr;

	iss.str(string_to_parse);				

	SentRep* srp;
	//srp = new SentRep(*nontokStream, SentRep::SGML);			//ours is non-tokenized
	srp = new SentRep(iss, SentRep::SGML);			//ours is non-tokenized

	int len = srp->length();
	if(len > params.maxSentLen) 
		return nullptr;
	if(len == 0) 
		return nullptr;
	if( !params.field().in(sentenceCount) ) 
		return nullptr;

	MeChart*	chart = new MeChart( *srp );
	curChart = chart;

	try {
		chart->parse( );
	} 
	catch(...)
	{ 
		SmartBody::util::log("Error: parsing error from natural language parser");
		return nullptr;
	}	

	Item* topS = chart->topS();
	if(!topS)
	{
		totUnparsed++;
		if(!Bchart::silent)
		{
			cerr << "Parse failed" << endl;
			cerr << *srp << endl;
		}
		delete chart;
		return nullptr;
	}

	// compute the outside probabilities on the items so that we can
	// skip doing detailed computations on the really bad ones 
	chart->set_Alphas();

	Bst& bst = chart->findMapParse();
	if( bst.empty())
	{
		totUnparsed++;
		if(!Bchart::silent)
		{
			cerr << "Parse failed" << endl;
			cerr << *srp << endl;
		}
		delete chart;
		return nullptr;
	}

	int numVersions = 0;
	Link diffs(0);
	int numDiff = 0;
	std::vector<InputTree*> saveTrees;
	saveTrees.reserve(Bchart::Nth);
	vector<double> saveProbs;
	saveProbs.reserve(Bchart::Nth);
	for(numVersions = 0 ; ; numVersions++)
	{
		short pos = 0;
		Val* v = bst.next(numVersions);
		if(!v) break;
		InputTree* mapparse=inputTreeFromBsts(v,pos,*srp);
		bool isU;
		diffs.is_unique(mapparse, isU);
		//cerr << "V " << numVersions << " " << isU << " " << v->prob() << "\n" << *mapparse << endl;
		if(isU)
		{
			saveTrees.emplace_back(mapparse);
			saveProbs.emplace_back(v->prob());
			numDiff++;
		}
		else
		{
			delete mapparse;
		}
		if(numDiff >= Bchart::Nth) break;
	}
	ECString ind;
	if(srp->sentenceName.empty())
		ind = intToString(sentenceCount);
	else ind = srp->sentenceName;
	if(Bchart::Nth > 1)
		std::cout << numDiff << "\t" << ind <<"\n";

	SBParseNode* sbParseNode = nullptr;
	for(int i = 0 ; i < numDiff ; i++)
	{
		short pos = 0;
		InputTree*  mapparse = saveTrees[i];
		

		double logP = log(saveProbs[i]);
		logP -= (srp->length()*log600);
		if(Bchart::Nth > 1)
			std::cout <<  logP << "\n";
		if(Bchart::prettyPrint) 
		{		
			oss << *mapparse << " oss\n";
		}
		else
		{
			mapparse->printproper(oss);				
		}					
		
		result = oss.str();	
	
		string_to_parse.clear();

		sbParseNode = new SBParseNode();
		createParseTree(mapparse, sbParseNode);

		delete mapparse;
		
	}
	std::cout << endl;
	oss.str("");

	delete chart;
	delete srp;
	iss.clear();

	return sbParseNode;
}

void SBParser::createParseTree(InputTree* inputTree, SBParseNode* node)
{
	if( inputTree->word().length() != 0 )
	{
		node->setWord(inputTree->word());
		node->setTerm(inputTree->term());
	}
	else
	{
		std::stringstream strstr;
		strstr << inputTree->term() << " " << inputTree->ntInfo();
		node->setTerm(strstr.str());

		ConstInputTreesIter subTreeIter = inputTree->subTrees().begin();
		InputTree* subTree = nullptr;
		for( ; subTreeIter != inputTree->subTrees().end() ; subTreeIter++ )
		{
			subTree = *subTreeIter;
			SBParseNode* child = new SBParseNode();
			node->addChild(child);
			createParseTree(subTree, child);
		}
	}
}

void SBParser::cleanUp(SmartBody::SBParseNode* node)
{
	if (node)
		delete node;
}

bool SBParser::isInitialized()
{
	return _initialized;
}

std::string SBParser::parseUtterance(SBParserListener* listener, std::string utterance)
{
	if (!listener)
	{
		SmartBody::util::log("Parse listener not available. Use SBParser::parse() to parse directly, or add a listener.");
		return "";
	}

	listener->resetBML();

	std::vector<std::string> tokens;
	SmartBody::util::tokenize(utterance, tokens, " ");

	// users can put a '-' to indicate negative expression, or a '+' to indicate positive expression
	// in front of a word. This information must be extracted from the utterance, and presented
	// later when doing per-word processing
	std::map<int, std::string> wordEmphasisMap;
	for (size_t w = 0; w < tokens.size(); w++)
	{
		if (tokens[w][0] == '-')
		{
			wordEmphasisMap[w] = "-";
			tokens[w] = tokens[w].substr(1);
		}
		else if (tokens[w][0] == '+')
		{
			wordEmphasisMap[w] = "+";
			tokens[w] = tokens[w].substr(1);
		}	
	}

	std::string encapsulatedUtterance = "<s> ";
	encapsulatedUtterance.append(utterance);
	encapsulatedUtterance.append(" </s>");
	

	std::stringstream strstr;
	strstr << "<act xmlns:sbm=\"http://ict.usc.edu\">" << std::endl;
	strstr << "\t<bml>" << std::endl;
	strstr << "\t\t<speech id=\"sp1\" type=\"application/ssml+xml\">" << std::endl;

	for (size_t i = 0; i < tokens.size(); i++)
	{
		strstr << "\t\t\t<mark name=\"T" << (i * 2) << "\"/>" << tokens[i] << std::endl;
		strstr << "\t\t\t<mark name=\"T" << (i * 2 + 1) << "\"/>" << std::endl;
	}
	strstr << "\t\t</speech>" << std::endl;

	std::string parseBML = strstr.str();

	listener->addBML(parseBML);

	int curWord = 0;

	// syntactic analysis
	if (isInitialized())
	{
		// if the parser is initialized, traverse
		SmartBody::SBParseNode* node = this->parse(encapsulatedUtterance);
		parseTraverse(listener, node, curWord);
	}

	// per-word analysis
	for (size_t i = 0; i < tokens.size(); i++)
	{
		std::stringstream strstr;
		strstr << "T" << (i * 2);
		std::map<int, std::string>::iterator iter = wordEmphasisMap.find(i);
		if (iter == wordEmphasisMap.end())
			listener->onWord(strstr.str(), tokens[i], "");
		else
			listener->onWord(strstr.str(), tokens[i], (*iter).second);
	}

	listener->addBML("\r\n");
	listener->addBML("\t</bml>\r\n</act>\r\n");

	return listener->getCurrentBML();
}

void SBParser::parseTraverse(SBParserListener* listener, SmartBody::SBParseNode* node, int& curWord)
{
	if (!node)
		return;

	// removed per-word processing; run that separately
	//if (!node->isTerminal())
	//{
		//std::stringstream strstr;
		//strstr << "T" << (2 * curWord);
		//curWord++;
		//listener->onWord(strstr.str(), node->getWord());
	//}

	std::stringstream strstr;
	strstr << "T" << (2 * curWord);
	std::string terminal = SmartBody::util::replace(node->getTerm(), " ", "");
	listener->onPartOfSpeech(strstr.str(), terminal);

	for (int c = 0; c < node->getNumChildren(); c++)
	{
		SmartBody::SBParseNode* child = node->getChild(c);
		parseTraverse(listener, child, curWord);
	}

}


}

