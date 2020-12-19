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

#include <sb/SBPhonemeManager.h>
#include <sb/SBPhoneme.h>
#include <sb/SBScene.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBCharacter.h>
#include "SBUtilities.h"
#include <algorithm>
#include <vector>
#include <set>

#include <fstream>
#include <cctype>
#include <cstring>
#include <sbm/sbm_speech.hpp>
#include "rapidxml_utils.hpp"
#include "rapidxml.hpp"
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <fstream>

namespace SmartBody {

SBPhonemeManager::SBPhonemeManager()
{
	setName("PhonemeManager");

	addPhonemeMapping("SIL", "_");
	addPhonemeMapping("PAU", "_");
	addPhonemeMapping("sp", "_");
	addPhonemeMapping("spn", "_");
	addPhonemeMapping("AW", "Ah");
	addPhonemeMapping("AA", "Aa");
	addPhonemeMapping("AE", "Ah");
	addPhonemeMapping("AH", "Ah");
	addPhonemeMapping("AO", "Ao");
	addPhonemeMapping("AX", "Ah");
	addPhonemeMapping("@", "Ah");
	addPhonemeMapping("AY", "Ay");
	addPhonemeMapping("B", "BMP");
	addPhonemeMapping("BMP", "BMP");
	addPhonemeMapping("CH", "Sh");
	addPhonemeMapping("D", "D");
	addPhonemeMapping("DH", "Th");
	addPhonemeMapping("DX", "D");
	addPhonemeMapping("EH", "Eh");
	//addPhonemeMapping("EL", "Eh");
	addPhonemeMapping("EN", "Eh");
	addPhonemeMapping("ER", "Er");
	addPhonemeMapping("EY", "Eh");
	addPhonemeMapping("F", "F");
	addPhonemeMapping("G", "Kg");
	addPhonemeMapping("HH", "Ih");
	addPhonemeMapping("IH", "Ih");
	addPhonemeMapping("IY", "Ih");
	addPhonemeMapping("JH", "Sh");
	addPhonemeMapping("K", "Kg");
	addPhonemeMapping("KG", "Kg");
	addPhonemeMapping("EL", "L");
	addPhonemeMapping("L", "L");
	addPhonemeMapping("M", "BMP");
	addPhonemeMapping("N", "D");
	addPhonemeMapping("NG", "Kg");
	addPhonemeMapping("OW", "Ow");
	addPhonemeMapping("OY", "Oy");
	addPhonemeMapping("P", "BMP");
	addPhonemeMapping("R", "R");
	addPhonemeMapping("S", "Z");
	addPhonemeMapping("T", "D");
	addPhonemeMapping("SH", "Sh");
	addPhonemeMapping("TH", "Th");
	addPhonemeMapping("UH", "Eh");
	addPhonemeMapping("UW", "W");
	addPhonemeMapping("V", "F");
	addPhonemeMapping("W", "W");
	addPhonemeMapping("Y", "Ih");
	addPhonemeMapping("Z", "Z");
	addPhonemeMapping("ZH", "Sh");

	_fastMapDirty = true;
}

void SBPhonemeManager::addPhonemeMapping(const std::string& from, const std::string& to)
{
	std::string upperCaseFrom = from;
	std::string upperCaseTo = to;
	std::transform(upperCaseFrom.begin(), upperCaseFrom.end(), upperCaseFrom.begin(), ::toupper);
	std::transform(upperCaseTo.begin(), upperCaseTo.end(), upperCaseTo.begin(), ::toupper);

	auto iter = _phonemeToCommonPhonemeMap.find(upperCaseFrom);
	if (iter != _phonemeToCommonPhonemeMap.end())
	{
		SmartBody::util::log("Phoneme mapping from %s->%s already found, replacing with %s->%s", (*iter).first.c_str(), (*iter).second.c_str(), upperCaseFrom.c_str(), upperCaseTo.c_str());
	}
	_phonemeToCommonPhonemeMap[from] = upperCaseTo;
}

std::string SBPhonemeManager::getPhonemeMapping(const std::string& from)
{
	std::string uppercaseFrom = from;
	std::transform(uppercaseFrom.begin(), uppercaseFrom.end(), uppercaseFrom.begin(), ::toupper);
	auto iter = _phonemeToCommonPhonemeMap.find(uppercaseFrom);
	if (iter != _phonemeToCommonPhonemeMap.end())
	{
		return  (*iter).second;
	}
	else
	{
		return "";
	}
	
}



std::vector<std::string> SBPhonemeManager::getCommonPhonemes()
{
	std::set<std::string> commonPhonemeSet;

	for (auto & iter : _phonemeToCommonPhonemeMap)
	{
		commonPhonemeSet.insert(iter.second);
	}

	std::vector<std::string> commonPhonemesVector;

	for (const auto & iter : commonPhonemeSet)
	{
		commonPhonemesVector.emplace_back(iter);
	}
	return commonPhonemesVector;
}

SBPhonemeManager::~SBPhonemeManager() = default;


void SBPhonemeManager::deleteDiphoneSet(const std::string& name)
{
	if (_diphoneMap.find(name) != _diphoneMap.end())
	{
		_diphoneMap.erase(name);
	}
}

SBDiphone* SBPhonemeManager::createDiphone(const std::string& fromPhoneme, const std::string& toPhoneme, const std::string& name)
{
	std::string upperCaseFromPhoneme = fromPhoneme;
	std::string upperCaseToPhoneme = toPhoneme;
	std::transform(upperCaseFromPhoneme.begin(), upperCaseFromPhoneme.end(), upperCaseFromPhoneme.begin(), ::toupper);
	std::transform(upperCaseToPhoneme.begin(), upperCaseToPhoneme.end(), upperCaseToPhoneme.begin(), ::toupper);
	SBDiphone* diphone = getDiphone(upperCaseFromPhoneme, upperCaseToPhoneme, name);
	if (diphone)
	{
		;//SmartBody::util::log("Diphone set %s already contain diphone pair %s to %s, return existing one.", name.c_str(), lowerCaseFromPhoneme.c_str(), lowerCaseToPhoneme.c_str());
	}
	else
	{
		diphone = new SBDiphone(upperCaseFromPhoneme, upperCaseToPhoneme);
		_diphoneMap[name].emplace_back(diphone);
	}

	_fastMapDirty = true;
	return diphone;
}

std::vector<std::unique_ptr<SBDiphone>>& SBPhonemeManager::getDiphones(const std::string& name)
{	
	auto iter = _diphoneMap.find(name);
	if (iter == _diphoneMap.end())
	{
		auto result = _diphoneMap.emplace(name, std::vector<std::unique_ptr<SBDiphone>>());
		return result.first->second;
	} else {
		return iter->second;
	}
}

SBDiphone* SBPhonemeManager::getMappedDiphone(const std::string& fromPhoneme, const std::string& toPhoneme, const std::string& name)
{
	std::string mappedFrom = getPhonemeMapping(fromPhoneme);
	std::string mappedTo = getPhonemeMapping(toPhoneme);
	return getDiphone(fromPhoneme, toPhoneme, name);
}

SBDiphone* SBPhonemeManager::getDiphone(const std::string& fromPhoneme, const std::string& toPhoneme, const std::string& name)
{
	std::string upperCaseFromPhoneme = fromPhoneme;
	std::string upperCaseToPhoneme = toPhoneme;
	std::transform(upperCaseFromPhoneme.begin(), upperCaseFromPhoneme.end(), upperCaseFromPhoneme.begin(), ::toupper);
	std::transform(upperCaseToPhoneme.begin(), upperCaseToPhoneme.end(), upperCaseToPhoneme.begin(), ::toupper);

	// fast access of diphones
	if (_fastMapDirty)
	{
		createFastMap();
		_fastMapDirty = false;
	}

	std::stringstream strstr;
	strstr << name << "_" << upperCaseFromPhoneme << "_" << upperCaseToPhoneme;
	auto iter = _fastDiphoneMap.find(strstr.str());
	if (iter != _fastDiphoneMap.end())
	{
		return (*iter).second;
	}
	else
	{
		return nullptr;
	}

	/*
	// slower access - linear search of diphones
	std::vector<SBDiphone*>& diphones = getDiphones(name);
	for (size_t i = 0; i < diphones.size(); i++)
	{
		if (diphones[i]->getFromPhonemeName() == upperCaseFromPhoneme && 
			diphones[i]->getToPhonemeName() == upperCaseToPhoneme)
		{
			return diphones[i];
		}
	}
	*/

	return nullptr;
}

int SBPhonemeManager::getNumDiphoneMap()
{
	return _diphoneMap.size();
}

int SBPhonemeManager::getNumDiphones(const std::string& name)
{
	auto& diphones = getDiphones(name);
	return diphones.size();
}

std::vector<std::string> SBPhonemeManager::getDiphoneMapNames()
{
	std::vector<std::string> diphoneMaps;
	for (auto & iter : _diphoneMap)
	{
		diphoneMaps.emplace_back(iter.first);
	}

	return diphoneMaps;

}

void SBPhonemeManager::normalizeCurves(const std::string& name)
{
	auto iter = _diphoneMap.find(name);
	if (iter != _diphoneMap.end())
	{
		// iterate through the phone bigrams
		for (auto iter2 = _diphoneMap.begin();
			 iter2 != _diphoneMap.end();
			 iter2++)
		{
			// get the phone bigram
			auto& diphones = getDiphones(iter->first);
			for (auto& diphone : diphones)
			{
					// get the keys and find the largest value in the list
				float maxVal = -1.0f;
				std::vector<std::string> visemes = diphone->getVisemeNames();
				for (auto & viseme : visemes)
				{
						std::vector<float>& keys = diphone->getKeys(viseme);
					bool isTime = true;
					for (float & val : keys)
					{
							if (isTime)
						{
							isTime = false;
						}
						else
						{
							if (val > maxVal)
								maxVal = val;
							isTime = true;
						}
					}
				}

				if (maxVal <= -0.0f )
				{
					continue;
				}

				// normalize the values according the the scale
				SmartBody::util::log("Normalize scale is %f", maxVal);
				float scale = 1.0f / ((1.0f + maxVal) / 2.0f);
				for (auto & viseme : visemes)
				{
						std::vector<float>& keys = diphone->getKeys(viseme);
					bool isTime = true;
					for (float & val : keys)
					{
							if (isTime)
						{
							isTime = false;
						}
						else
						{
							val *= scale;
							isTime = true;
						}
					}
				}

			}
		}
	}
}

void SBPhonemeManager::loadDictionary(const std::string& language, const std::string& file)
{
	std::ifstream filestream( file.c_str() );
	if (!filestream.good())
	{
		SmartBody::util::log("Could not open dictionary file %s", file.c_str());
		return;
	}
	
	int numWords = 0;
	char line[8192];
	while(!filestream.eof() && 
		   filestream.good())
	{
		filestream.getline(line, 4096, '\n');
        if (line[strlen(line) - 1] == '\r')
			line[strlen(line) - 1] = '\0';

		if (strlen(line) == 0) // ignore blank lines
			continue;

		std::string strLine(line);
		if (line[0] == ';')
			continue;
		std::vector<std::string> tokens;
		SmartBody::util::tokenize(strLine, tokens, " ");
		if (tokens.size() > 0)
		{
			std::vector<std::string> phonemes;
			for (size_t t = 1; t < tokens.size(); t++)
			{
				// for vowel sounds, remove the emphasis
				if (tokens[t].size() == 3)
				{
					phonemes.emplace_back(tokens[t].substr(0,2));
				}
				else
				{
					phonemes.emplace_back(tokens[t]);
				}
			}
				
			addDictionaryWord("English", tokens[0], phonemes);
			numWords++;
		}

	}
	filestream.close();
	SmartBody::util::log("Dictionary for language %s loaded with %d words.", language.c_str(), numWords);
	_dictionaryFileMap[language] = file;
}

void SBPhonemeManager::addDictionaryWord(const std::string& language, const std::string& word, std::vector<std::string>& phonemes)
{
	std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator iter = _wordToPhonemeMaps.find(language);
	if (iter == _wordToPhonemeMaps.end())
	{
		// create a new map
		std::map<std::string, std::vector<std::string> > languageMap;
		_wordToPhonemeMaps[language] = languageMap;
		iter = _wordToPhonemeMaps.find(language);
	}

	std::map<std::string, std::vector<std::string> >::iterator phonemeLookupIter = (*iter).second.find(word);
	if (phonemeLookupIter != (*iter).second.end())
	{
		// if the word already exists, warn and replace it
		SmartBody::util::log("Dictionary word %s in language %s already found, replacing...", word.c_str(), language.c_str());
	}
	(*iter).second[word] = phonemes;
}

std::vector<std::string>& SBPhonemeManager::getDictionaryWord(const std::string& language, const std::string& word)
{
	std::string uppercaseWord = SmartBody::util::toUpper(word);
	
	std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator iter = _wordToPhonemeMaps.find(language);
	if (iter == _wordToPhonemeMaps.end())
	{
		SmartBody::util::log("Dictionary word %s cannot be found: no lanugage %s present.", uppercaseWord.c_str(), language.c_str());
		return _emptyPhonemeSet;
	}

	std::map<std::string, std::vector<std::string> >& wordPhonemeMap = (*iter).second;

	std::map<std::string, std::vector<std::string> >::iterator phonemeLookupIter = wordPhonemeMap.find(uppercaseWord);
	if (phonemeLookupIter == wordPhonemeMap.end())
	{
		SmartBody::util::log("Dictionary word %s in language %s cannot be found.", uppercaseWord.c_str(), language.c_str());
		return _emptyPhonemeSet;
	}

	return (*phonemeLookupIter).second;
}

std::string SBPhonemeManager::getDictionaryFile(const std::string& language)
{
	std::map<std::string, std::string>::iterator iter = _dictionaryFileMap.find(language);
	if (iter == _dictionaryFileMap.end())
		return "";
	return (*iter).second;

}

int SBPhonemeManager::getNumDictionaryWords(const std::string& language)
{
	std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator iter = _wordToPhonemeMaps.find(language);
	if (iter != _wordToPhonemeMaps.end())
	{
		return (*iter).second.size();
	}
	else
	{
		return 0;
	}
}

void SBPhonemeManager::setEnable(bool val)
{
	SBService::setEnable(val);
}

std::vector<std::string> SBPhonemeManager::getPhonemesRealtime(const std::string& character, int amount)
{
	std::vector<std::string> phonemes;
	std::map<std::string, std::vector<RealTimePhoneme> >::iterator iter = _realtimePhonemes.find(character);
	if (iter != _realtimePhonemes.end())
	{
		int num = 0;
		for (std::vector<RealTimePhoneme>::iterator phonemeIter = (*iter).second.begin();
			 phonemeIter != (*iter).second.end();
			 phonemeIter++)
		{
			if (num >= amount)
				break;
			phonemes.emplace_back((*phonemeIter).phoneme);
			num++;
		}

		return phonemes;
	}
	else
	{
		return std::vector<std::string>();
	}
}

std::vector<double> SBPhonemeManager::getPhonemesRealtimeTimings(const std::string& character, int amount)
{
	std::vector<double> phonemeTimings;
	std::map<std::string, std::vector<RealTimePhoneme> >::iterator iter = _realtimePhonemes.find(character);
	if (iter != _realtimePhonemes.end())
	{
		int num = 0;
		for (std::vector<RealTimePhoneme>::iterator phonemeIter = (*iter).second.begin();
			 phonemeIter != (*iter).second.end();
			 phonemeIter++)
		{
			if (num >= amount)
				break;
			phonemeTimings.emplace_back((*phonemeIter).time);
			num++;
		}

		return phonemeTimings;
	}
	else
	{
		return std::vector<double>();
	}
}

void SBPhonemeManager::removePhonemesRealtime(const std::string& character, int amount)
{

	std::map<std::string, std::vector<RealTimePhoneme> >::iterator iter = _realtimePhonemes.find(character);
	if (iter != _realtimePhonemes.end())
	{
		if ((*iter).second.size() < (size_t) amount)
			amount = (*iter).second.size();
		int num = 0;
		while (num < amount)
		{
      std::vector<RealTimePhoneme>::iterator phonemeIter = (*iter).second.begin();
      if (phonemeIter != (*iter).second.end()) {
        (*iter).second.erase(phonemeIter);
      }
			num++;
		}
	}
}

void SBPhonemeManager::removePhonemesRealtimeByTime(const std::string& character, double minTime)
{

	std::map<std::string, std::vector<RealTimePhoneme> >::iterator iter = _realtimePhonemes.find(character);
	if (iter != _realtimePhonemes.end())
	{
		bool remove = true;
		while (remove)
		{
			remove = false;
			for (std::vector<RealTimePhoneme>::iterator phonemeIter = (*iter).second.begin();
				 phonemeIter != (*iter).second.end();
				 phonemeIter++)
			{
				if ((*phonemeIter).time < minTime)
				{
					(*iter).second.erase(phonemeIter);
					remove = true;
					break;
				}
			}
		}
	}
}


void SBPhonemeManager::setPhonemesRealtime(const std::string& character, const std::string& phoneme)
{
	std::map<std::string, std::vector<RealTimePhoneme> >::iterator iter = _realtimePhonemes.find(character);
	if (iter == _realtimePhonemes.end())
	{
		_realtimePhonemes[character] = std::vector<RealTimePhoneme>();
		iter = _realtimePhonemes.find(character);
	}
	RealTimePhoneme rtp;
	rtp.phoneme = phoneme;
	rtp.time = SmartBody::SBScene::getScene()->getSimulationManager()->getTime();
	(*iter).second.emplace_back(rtp);
	SmartBody::util::log("Got phoneme %s at time %f",  rtp.phoneme.c_str(), rtp.time);
}

void SBPhonemeManager::clearPhonemesRealtime(const std::string& character, const std::string& phoneme)
{
	auto iter = _realtimePhonemes.find(character);
	if (iter != _realtimePhonemes.end())
	{
		(*iter).second.clear();
	}
}


void SBPhonemeManager::generatePhoneTrigrams(const std::string& lipsyncSetName)
{
	
	auto iter = _diphoneMap.find(lipsyncSetName);
	if (iter == _diphoneMap.end())
	{
		SmartBody::util::log("Cannot produce trigrams for lip sync set name: %s, does not exist.", lipsyncSetName.c_str());
		return;
	}

	auto& allDiphones = (*iter).second;
	for (auto bigramIter = allDiphones.begin();
		 bigramIter != allDiphones.end();
		 bigramIter++)
	{
		for (auto bigramIter2 = (bigramIter + 1);
			 bigramIter2 != allDiphones.end();
			 bigramIter2++)
		{
			std::vector<SmartBody::VisemeData*> visemes;
			const std::string& fromPhoneme = (*bigramIter)->getFromPhonemeName();
			const std::string& toPhoneme = (*bigramIter)->getToPhonemeName();

			const std::string& fromPhoneme2 = (*bigramIter2)->getFromPhonemeName();
			const std::string& toPhoneme2 = (*bigramIter2)->getToPhonemeName();

			if (toPhoneme != fromPhoneme2)
				continue;

			VisemeData* phoneme1 = new VisemeData(fromPhoneme, 0);
			visemes.emplace_back(phoneme1);
			VisemeData* phoneme2 = new VisemeData(toPhoneme, 1);
			visemes.emplace_back(phoneme2);
			VisemeData* phoneme3 = new VisemeData(fromPhoneme2, 1);
			visemes.emplace_back(phoneme3);
			VisemeData* phoneme4 = new VisemeData(toPhoneme2, 2);
			visemes.emplace_back(phoneme4);

			//std::map<std::string, std::vector<float> > trigramCurves = BML::SpeechRequest::generateCurvesGivenDiphoneSet(&visemes, lipsyncSetName, std::string characterName)
		
		}
		
	}

}

void SBPhonemeManager::exportForService(const std::string filename)
{
	std::ofstream file(filename);
	if (!file.good())
	{
		SmartBody::util::log("File %s cannot be opened for writing. No lip sync export done.", filename.c_str());
		return;
	}
	// export the mappings
	for (std::map<std::string, std::string>::iterator iter = _phonemeToCommonPhonemeMap.begin();
		iter != _phonemeToCommonPhonemeMap.end();
		iter++)
	{
		std::string from = (*iter).first;
		std::string to = (*iter).second;
		std::string fromLower = SmartBody::util::toLower(from);
		std::string toLower = SmartBody::util::toLower(to);
		file << "Map " << fromLower << " " << toLower << std::endl;
	}


	// export the animations
	for (std::map<std::string, SBDiphone* >::iterator iter = _fastDiphoneMap.begin();
		iter != _fastDiphoneMap.end();
		iter++)
	{
		SBDiphone* diphone = (*iter).second;

		std::string diphoneCode = (*iter).first;

		std::vector<std::string> tokens;
		SmartBody::util::tokenize(diphoneCode, tokens, "_");

		std::string diphoneFrom = tokens[1];
		std::string diphoneTo = tokens[2];

		std::string diphoneFromLower = SmartBody::util::toLower(diphoneFrom);
		std::string diphoneToLower = SmartBody::util::toLower(diphoneTo);

		file << "Diphone " << diphoneFromLower << " " << diphoneToLower << std::endl;

		int numVisemes = diphone->getNumVisemes();
		const std::vector<std::string>& visemeNames = diphone->getVisemeNames();

		for (int v = 0; v < numVisemes; v++)
		{
			file << "Shape " << visemeNames[v] << " ";
			std::vector<float>& keys = diphone->getKeys(visemeNames[v]);
			file << keys.size() << " ";
			for (size_t k = 0; k < keys.size(); k++)
			{
				file << keys[k] << " ";
			}
			file << std::endl;
		}
	}

	file.close();
	SmartBody::util::log("File %s written.", filename.c_str());

}


void SBPhonemeManager::saveLipSyncAnimation(const std::string characterName, const std::string lipsyncFile, const std::string outputFile)
{
	// load the lip sync file (.bml)
	rapidxml::file<char> bmlFile(lipsyncFile.c_str());
	rapidxml::xml_document<> bmldoc;
	bmldoc.parse< 0 >((char*) bmlFile.data());
	rapidxml::xml_node<>* bmlnode = bmldoc.first_node("bml");

	if (!bmlnode)
	{
		SmartBody::util::log( "Could not find <bml> tag in %s. No output file named '%s' written.", lipsyncFile.c_str(), outputFile.c_str());
		return;
	}

	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(characterName);
	if (!character)
	{
		SmartBody::util::log("Could not find character with name '%s'. No output file named '%s' written.", characterName.c_str(), outputFile.c_str());
		return;
	}

	std::string lipSyncSetName = character->getStringAttribute("lipSyncSetName");
	if (lipSyncSetName == "")
	{
		SmartBody::util::log("Could not find lip sync data set '%s' for character with name '%s'. Please set the 'lipsyncSetName' attribute to a valid lip sync set first. No output file named '%s' written.", lipSyncSetName.c_str(), characterName.c_str(), outputFile.c_str());
		return;
	}

	auto& diphones = this->getDiphones(lipSyncSetName);
	if (diphones.size() == 0)
	{
		SmartBody::util::log("Lip sync data for set '%s' is empty. Please load a lip sync set first. No output file named '%s' written.", lipSyncSetName.c_str(), outputFile.c_str());
		return;
	}

	std::vector<VisemeData*> visemes;

	rapidxml::xml_node<>* node = bmlnode->first_node("lips");
	while (node)
	{
		rapidxml::xml_attribute<>* visemeAttr = node->first_attribute("viseme");
		std::string viseme = "";
		if (visemeAttr)
			viseme = visemeAttr->value();

		rapidxml::xml_attribute<>* artiulationAttr = node->first_attribute("articulation");
		std::string articulation = "";
		if (artiulationAttr)
			articulation = artiulationAttr->value();

		rapidxml::xml_attribute<>* startAttr = node->first_attribute("start");
		std::string start = "";
		if (startAttr)
			start = startAttr->value();

		rapidxml::xml_attribute<>* endAttr = node->first_attribute("end");
		std::string end = "";
		if (endAttr)
			end = endAttr->value();


		float startTime = (float)atof( start.c_str() );
		float endTime = (float)atof( end.c_str() );

		float weight = (float) atof( articulation.c_str());
		VisemeData* curViseme = new VisemeData(viseme, weight, startTime,  endTime - startTime);

		visemes.emplace_back(curViseme);

		node = node->next_sibling("lips");
	}

	// generate the curves
	std::vector<SmartBody::VisemeData*> debugVisemeCurves;
	std::map<std::string, std::vector<float> > tempCurves = generateCurvesGivenDiphoneSet(&visemes, lipSyncSetName, characterName, true, true, true, debugVisemeCurves);
	

	// save the curves to a .dae file
	std::ofstream daeStream (outputFile.c_str(), std::ios::out);
	if (!daeStream.good())
	{
		SmartBody::util::log("Cannot write to file '%s'. No output file written.", outputFile.c_str());
		return;
	}

	daeStream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
	daeStream << "<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">" << std::endl;
	daeStream << "<asset>" << std::endl;
	daeStream << "<contributor>" << std::endl;
	daeStream << "<author>SmartBody</author>" << std::endl;
	daeStream << "<authoring_tool>SmartBody</authoring_tool>" << std::endl;
	daeStream << "<comments>" << std::endl;
	daeStream << "Lip sync data generated by SmartBody" << std::endl;
	daeStream << "</comments>" << std::endl;
	daeStream << "<source_data>" << lipsyncFile << "</source_data>" << std::endl;
	daeStream << "</contributor>" << std::endl;

	time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	daeStream << "<created>" << buf << "</created>" << std::endl;
	daeStream << "<modified>" << buf << "</modified>" << std::endl;

	daeStream << "<library_animations>" << std::endl;

	boost::filesystem::path p(lipsyncFile);
	std::string extension = boost::filesystem::extension(p);
	std::string basename = boost::filesystem::basename(p);

	std::string animName = basename + "-anim";

	daeStream << "<animation name=\"" << basename << "\" id=\"" << animName << "\">" << std::endl;

	for (auto & tempCurve : tempCurves)
	{
		std::string channelName = tempCurve.first;
		std::string sourceName = animName + "-" + channelName;

		// input
		daeStream << "<source id=\"" << sourceName << "-input\">" << std::endl;
		daeStream << "<float_array id=\"" << sourceName << "-input-array\" count=\"";
		std::vector<float>& values = tempCurve.second;
		daeStream << values.size() / 2.0 << "\">";
		for (size_t i = 0; i < values.size(); i+=2)
		{
			daeStream << values[i] << " ";
		}
		daeStream << "</float_array>" << std::endl;
		daeStream << "<technique_common>" << std::endl;
		daeStream << "<accessor source=\"#" << sourceName << "-input-array\" count=\"" << values.size() / 2.0 << "\" stride=\"1\">" << std::endl;
		daeStream << "<param name=\"TIME\" type=\"float\"/>" << std::endl;
		daeStream << "</accessor>" << std::endl;
		daeStream << "</technique_common>" << std::endl;
		daeStream << "</source>" << std::endl;

		// output
		daeStream << "<source id=\"" << sourceName << "-output\">" << std::endl;
		daeStream << "<float_array id=\"" << sourceName << "-output-array\" count=\"";

		daeStream << values.size() / 2.0 << "\">";
		for (size_t i = 1; i < values.size(); i+=2)
		{
			daeStream << values[i] << " ";
		}
		daeStream << "</float_array>" << std::endl;
		daeStream << "<technique_common>" << std::endl;
		daeStream << "<accessor source=\"#" << sourceName << "-output-array\" count=\"" << values.size() / 2.0 << "\" stride=\"1\">" << std::endl;
		daeStream << "<param name=\"TRANSFORM\" type=\"float\"/>" << std::endl;
		daeStream << "</accessor>" << std::endl;
		daeStream << "</technique_common>" << std::endl;
		daeStream << "</source>" << std::endl;

		// interpolation
		daeStream << "<source id=\"" << sourceName << "-interpolations\">" << std::endl;
		daeStream << "<NAME_array id=\"" << sourceName << "-interpolations-array\" count=\"";
		daeStream << values.size() / 2.0 << "\">";
		for (size_t i = 1; i < values.size(); i+=2)
		{
			daeStream << "LINEAR ";
		}
		daeStream << "</NAME_array>" << std::endl;
		
		daeStream << "<technique_common>" << std::endl;
		daeStream << "<accessor source=\"#" << sourceName << "-interpolations-array\" count=\"" << values.size() / 2.0 << "\" stride=\"1\">" << std::endl;
		daeStream << "<param name=\"INTERPOLATION\" type=\"float\"/>" << std::endl;
		daeStream << "</accessor >" << std::endl;
		daeStream << "</technique_common>" << std::endl;
		daeStream << "</source>" << std::endl;

		// sampler
		daeStream << "<sampler id=\"" << sourceName << "-sampler\">";
		daeStream << "<input semantic=\"INPUT\" source=\"#" << sourceName << "-input\"/>";
		daeStream << "<input semantic=\"OUTPUT\" source=\"#" << sourceName << "-output\"/>";
		daeStream << "<input semantic=\"INTERPOLATION\" source=\"#" << sourceName << "-interpolations\"/>";
		daeStream << "</sampler>";


		daeStream << "<channel source=\"#" << sourceName << "-sampler\" target=\"" << channelName << "\"/>" << std::endl;
	}


	daeStream << "</animation>" << std::endl;
	daeStream << "</library_animations>" << std::endl;
	daeStream << "</asset>" << std::endl;
	daeStream << "</COLLADA>" << std::endl;

	daeStream.close();


}

void SBPhonemeManager::createFastMap()
{
	_fastDiphoneMap.clear();

	for (auto & iter : _diphoneMap)
	{
		std::string setName = iter.first;
		for (auto & iter2 : iter.second)
		{
			std::stringstream strstr;
			strstr << setName << "_" << iter2->getFromPhonemeName() << "_" << iter2->getToPhonemeName();
			_fastDiphoneMap.emplace(strstr.str(), iter2.get());
		}
	}

	_fastMapDirty = false;
}

std::map<std::string, std::vector<float> > SBPhonemeManager::generateCurvesGivenDiphoneSet(std::vector<SmartBody::VisemeData*>* visemes,
																						   std::string mappingName,
																						   std::string characterName,
																						   bool secondpass,
																						   bool thirdpass,
																						   bool fourthpass,
																						   std::vector<SmartBody::VisemeData*>& debugVisemeCurves) {
	SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(characterName);
	std::map<std::string, std::vector<float> > finalCurves;
	if (!character)
		return finalCurves;

	SmartBody::SBPhonemeManager* phonemeManager = SmartBody::SBScene::getScene()->getDiphoneManager();

	// map the visemes to the common set
	for (auto v : *visemes) {
			std::string commonSetId = phonemeManager->getPhonemeMapping(v->id());
		if (!commonSetId.empty())
			v->setId(commonSetId);
	}

	const std::string& diphoneMap = mappingName;
	VisemeData* curViseme = nullptr;
	VisemeData* prevViseme = nullptr;
	VisemeData* nextViseme = nullptr;
	std::vector<float> visemeTimeMarkers;
	std::vector<VisemeData*> visemeRawData;
	int consecutiveUnfoundCurves = 0;
	for (size_t i = 0; i < (*visemes).size(); i++) {
		if (i > 0)
			prevViseme = (*visemes)[i - 1];
		if (i < ((*visemes).size() - 1))
			nextViseme = (*visemes)[i + 1];
		curViseme = (*visemes)[i];
		visemeTimeMarkers.emplace_back(curViseme->time());
		if (prevViseme != nullptr) {
			SBDiphone* diphone = SmartBody::SBScene::getScene()->getDiphoneManager()->getDiphone(prevViseme->id(), curViseme->id(), diphoneMap);
			if (!diphone) {
				consecutiveUnfoundCurves++;
				if (consecutiveUnfoundCurves > 1) {
					// check for single phoneme between pauses, such as "_ A _"
					if (strcmp(curViseme->id(), "_") == 0 &&
						strcmp((*visemes)[i - 2]->id(), "_") == 0) {
						diphone = SmartBody::SBScene::getScene()->getDiphoneManager()->getDiphone(prevViseme->id(), prevViseme->id(), diphoneMap);
						consecutiveUnfoundCurves = 0;
					}
				}
			}

			float blendIval = 0.0f;

			float transitionPadding = (float) character->getDoubleAttribute("diphoneTransition");
			if (!character->getBoolAttribute("phoneBigramAnimation")) {
				blendIval = curViseme->time() - prevViseme->time() + 2.0f * transitionPadding;
			} else {
				if (nextViseme != nullptr)
					blendIval = nextViseme->time() - prevViseme->time();
				else
					blendIval = 2.0f * (curViseme->time() - prevViseme->time());
			}

			// ad-hoc, blend interval should not be less than 0.1f
			//if (blendIval < 0.2f)
			//	blendIval = 0.2f;

			if (diphone) {
				const std::vector<std::string>& visemeNames = diphone->getVisemeNames();
				bool hasBMP = false;
				for (int v = 0; v < diphone->getNumVisemes(); v++) {
					if (visemeNames[v] == "bmp") {
						hasBMP = true;
						break;
					}
				}
				for (int v = 0; v < diphone->getNumVisemes(); v++) {
					if (visemeNames[v] == "open" && hasBMP)
						continue;

					std::vector<float> curve = diphone->getKeys(visemeNames[v]);
					for (size_t k = 0; k < curve.size(); k++) {
						if (!character->getBoolAttribute("phoneBigramAnimation")) {
							if ((k % 2) == 0) {
								curve[k] *= blendIval;
								curve[k] += prevViseme->time();
								curve[k] -= (*visemes)[0]->time();
								curve[k] -= transitionPadding;
							} else {
								curve[k] *= character->getDiphoneScale();
								/*
								if (curve[k] > 1.0f)	//clamp to 1
									curve[k] = 1.0f;
								if (curve[k] < 0.05)	// clamp to 0.0
									curve[k] = 0.0f;
								*/
							}
						} else {
							if ((k % 2) == 0) {
								curve[k] *= blendIval;
								curve[k] += prevViseme->time();
								curve[k] -= (*visemes)[0]->time();
							} else {
								curve[k] *= character->getDiphoneScale();
								if (curve[k] > 1.0f)    //clamp to 1
									curve[k] = 1.0f;
								if (curve[k] < 0.05)    // clamp to 0.0
									curve[k] = 0.0f;
							}
						}
					}

					VisemeData* vcopy = new VisemeData(visemeNames[v], 0.0f);
					vcopy->setFloatCurve(curve, curve.size() / 2, 2);
					visemeRawData.emplace_back(vcopy);

					// debug
					/*
					std::stringstream debugNameSS;
					debugNameSS << "[" << i << "]" << diphone->getFromPhonemeName() << "-" << diphone->getToPhonemeName() << "-" << visemeNames[v];
					VisemeData* debugDiphoneVisemeCurve = new VisemeData(debugNameSS.str(), 0.0f);
					debugDiphoneVisemeCurve->setFloatCurve(curve, curve.size() / 2, 2);
					debugDiphoneVisemeCurve->setCurveInfo("0");
					debugVisemeCurves.emplace_back(debugDiphoneVisemeCurve);
					*/
				}
			}
		}
	}

	// get rid of all zero data
	for (size_t i = 0; i < visemeRawData.size(); i++) {
		if (visemeRawData[i]->getFloatCurve().size() == 0) {
			delete visemeRawData[i];
			visemeRawData[i] = nullptr;
			visemeRawData.erase(visemeRawData.begin() + i);
			i--;
		}
	}

	// process the diphone raw data
	std::vector<VisemeData*> visemeProcessedData;
	std::map<std::string, int> isProcessed;
	int counter = 0;
	for (auto & i : visemeRawData) {
		bool firstTime = true;
		if (isProcessed.find(i->id()) == isProcessed.end()) {
			isProcessed.insert(std::make_pair(i->id(), counter));
			counter++;
		} else
			firstTime = false;

		int index = isProcessed[i->id()];
		if (firstTime) {
			VisemeData* newV = new VisemeData(i->id(), i->time());
			newV->setFloatCurve(i->getFloatCurve(), i->getFloatCurve().size() / 2, 2);
			visemeProcessedData.emplace_back(newV);
		} else    // stitch the curves
		{
			std::vector<float>& stitchingCurve = i->getFloatCurve();
			std::vector<float>& origCurve = visemeProcessedData[index]->getFloatCurve();
			std::vector<float> newCurve(stitchCurve(origCurve, stitchingCurve));
			visemeProcessedData[index]->setFloatCurve(newCurve, newCurve.size() / 2, 2);
			/*
			std::stringstream debugNameSS;
			debugNameSS << "stitching-" << visemeProcessedData[index]->id() << i;
			VisemeData* debugStitch = new VisemeData(debugNameSS.str(), visemeProcessedData[index]->time());
			debugStitch->setCurveInfo("1");
			debugStitch->setFloatCurve(newCurve, newCurve.size() / 2, 2);
			debugVisemeCurves.emplace_back(debugStitch);
			*/
		}
	}

	// stitch and smooth
	for (auto & i : visemeProcessedData) {
		VisemeData* debugVwoSmoothing = new VisemeData(i->id(), i->time());
		debugVwoSmoothing->setCurveInfo("2");
		debugVwoSmoothing->setFloatCurve(i->getFloatCurve(), i->getFloatCurve().size() / 2, 2);

		if (secondpass) {
			if (strcmp(i->id(), "PBM") == 0)
				smoothCurve(i->getFloatCurve(), visemeTimeMarkers, (float) character->getDoubleAttribute("lipSyncSmoothWindow-PBM"));
			else if (strcmp(i->id(), "FV") == 0)
				smoothCurve(i->getFloatCurve(), visemeTimeMarkers, (float) character->getDoubleAttribute("lipSyncSmoothWindow-FV"));
			else
				smoothCurve(i->getFloatCurve(), visemeTimeMarkers, (float) character->getDoubleAttribute("lipSyncSmoothWindow"));
		}
		VisemeData* debugVwSmoothing = new VisemeData(i->id(), i->time());
		debugVwSmoothing->setFloatCurve(i->getFloatCurve(), i->getFloatCurve().size() / 2, 2);
		debugVwSmoothing->setCurveInfo("3");

		debugVisemeCurves.emplace_back(debugVwoSmoothing);
		debugVisemeCurves.emplace_back(debugVwSmoothing);
	}

	if (thirdpass) {
		if (character->getBoolAttribute("lipSyncConstraint")) {
			// rule based 'open' curve processing, pbm
			int openIndex = -1;
			int bmpIndex = -1;
			int fvIndex = -1;
			int wIndex = -1;
			int wideIndex = -1;
			int shchIndex = -1;
			for (size_t i = 0; i < visemeProcessedData.size(); i++) {
				//SmartBody::util::log("viseme name %s", visemeProcessedData[i]->id());
				if (strcmp(visemeProcessedData[i]->id(), "open") == 0)
					openIndex = i;
				if (strcmp(visemeProcessedData[i]->id(), "PBM") == 0)
					bmpIndex = i;
				if (strcmp(visemeProcessedData[i]->id(), "FV") == 0)
					fvIndex = i;
				if (strcmp(visemeProcessedData[i]->id(), "W") == 0)
					wIndex = i;
				if (strcmp(visemeProcessedData[i]->id(), "ShCh") == 0)
					shchIndex = i;
				if (strcmp(visemeProcessedData[i]->id(), "wide") == 0)
					wideIndex = i;
			}


			if (character->getBoolAttribute("constrainPBM") && bmpIndex >= 0) {
				if (openIndex >= 0) {
					constrainCurve(visemeProcessedData[openIndex]->getFloatCurve(), visemeProcessedData[bmpIndex]->getFloatCurve(), (float) character->getDoubleAttribute("openConstraintByPBM"));
				}
				if (wideIndex >= 0) {
					constrainCurve(visemeProcessedData[wideIndex]->getFloatCurve(), visemeProcessedData[bmpIndex]->getFloatCurve(), (float) character->getDoubleAttribute("wideConstraintByPBM"));
				}
				if (shchIndex >= 0) {
					constrainCurve(visemeProcessedData[shchIndex]->getFloatCurve(), visemeProcessedData[bmpIndex]->getFloatCurve(), (float) character->getDoubleAttribute("shchConstraintByPBM"));
				}
			}
			if (character->getBoolAttribute("constrainFV") && fvIndex >= 0) {
				if (openIndex >= 0) {
					constrainCurve(visemeProcessedData[openIndex]->getFloatCurve(), visemeProcessedData[fvIndex]->getFloatCurve(), (float) character->getDoubleAttribute("openConstraintByFV"));
				}
				if (wideIndex > 0) {
					constrainCurve(visemeProcessedData[wideIndex]->getFloatCurve(), visemeProcessedData[fvIndex]->getFloatCurve(), (float) character->getDoubleAttribute("wideConstraintByFV"));
				}
			}
			if (character->getBoolAttribute("constrainShCh") && shchIndex >= 0) {
				if (openIndex >= 0) {
					constrainCurve(visemeProcessedData[openIndex]->getFloatCurve(), visemeProcessedData[shchIndex]->getFloatCurve(), (float) character->getDoubleAttribute("openConstraintByShCh"));
				}
			}
			if (character->getBoolAttribute("constrainW") && wIndex >= 0) {
				if (openIndex >= 0) {
					constrainCurve(visemeProcessedData[openIndex]->getFloatCurve(), visemeProcessedData[wIndex]->getFloatCurve(), (float) character->getDoubleAttribute("openConstraintByW"));
				}
			}
			if (character->getBoolAttribute("constrainWide") && wideIndex >= 0) {
				if (openIndex >= 0) {
					constrainCurve(visemeProcessedData[openIndex]->getFloatCurve(), visemeProcessedData[wideIndex]->getFloatCurve(), (float) character->getDoubleAttribute("openConstraintByWide"));
				}
			}

			double pressedLipsWidenAmount = character->getDoubleAttribute("constrainPressedLips");
			if (pressedLipsWidenAmount > 0.0) {
				constrainWidenCurve(visemeProcessedData[bmpIndex]->getFloatCurve(), pressedLipsWidenAmount);
			}

			if (openIndex >= 0) {
				VisemeData* debugconstrainCurve = new VisemeData(visemeProcessedData[openIndex]->id(), visemeProcessedData[openIndex]->time());
				debugconstrainCurve->setCurveInfo("4");
				debugconstrainCurve->setFloatCurve(visemeProcessedData[openIndex]->getFloatCurve(), visemeProcessedData[openIndex]->getFloatCurve().size() / 2, 2);
				debugVisemeCurves.emplace_back(debugconstrainCurve);
			}

			if (wideIndex >= 0) {
				VisemeData* debugProcessWideCurve = new VisemeData(visemeProcessedData[wideIndex]->id(), visemeProcessedData[wideIndex]->time());
				debugProcessWideCurve->setCurveInfo("4");
				debugProcessWideCurve->setFloatCurve(visemeProcessedData[wideIndex]->getFloatCurve(), visemeProcessedData[wideIndex]->getFloatCurve().size() / 2, 2);
				debugVisemeCurves.emplace_back(debugProcessWideCurve);
			}
			if (wIndex >= 0) {
				VisemeData* debugProcessWCurve = new VisemeData(visemeProcessedData[wIndex]->id(), visemeProcessedData[wIndex]->time());
				debugProcessWCurve->setCurveInfo("4");
				debugProcessWCurve->setFloatCurve(visemeProcessedData[wIndex]->getFloatCurve(), visemeProcessedData[wIndex]->getFloatCurve().size() / 2, 2);
				debugVisemeCurves.emplace_back(debugProcessWCurve);
			}
			if (shchIndex >= 0) {
				VisemeData* debugProcessShChCurve = new VisemeData(visemeProcessedData[shchIndex]->id(), visemeProcessedData[shchIndex]->time());
				debugProcessShChCurve->setCurveInfo("4");
				debugProcessShChCurve->setFloatCurve(visemeProcessedData[shchIndex]->getFloatCurve(), visemeProcessedData[shchIndex]->getFloatCurve().size() / 2, 2);
				debugVisemeCurves.emplace_back(debugProcessShChCurve);
			}
		}
	}

	// apply low pass filter to the curve
	for (size_t i = 0; i < visemeProcessedData.size(); i++) {
		if (fourthpass)
			filterCurve(visemeProcessedData[i]->getFloatCurve(), character->getDiphoneSpeedLimit());
		VisemeData* debugVwFiltering = new VisemeData(visemeProcessedData[i]->id(), visemeProcessedData[i]->time());
		debugVwFiltering->setFloatCurve(visemeProcessedData[i]->getFloatCurve(), visemeProcessedData[i]->getFloatCurve().size() / 2, 2);
		debugVwFiltering->setCurveInfo("5");
		debugVisemeCurves.emplace_back(debugVwFiltering);

		//VisemeData* newV = new VisemeData(visemeProcessedData[i]->id(), visemeProcessedData[i]->time());
		//newV->setFloatCurve(visemeProcessedData[i]->getFloatCurve(), visemeProcessedData[i]->getFloatCurve().size() / 2, 2);

		std::vector<float> finalCurveData;
		for (size_t j = 0; j < visemeProcessedData[i]->getFloatCurve().size(); ++j)
			finalCurveData.emplace_back(visemeProcessedData[i]->getFloatCurve()[j]);

		if (finalCurves.find(visemeProcessedData[i]->id()) == finalCurves.end()) {
			finalCurves.insert(std::make_pair(visemeProcessedData[i]->id(), finalCurveData));
		} else {
			SmartBody::util::log("Cannot have two final curves that have same name!");
		}
	}

	for (size_t r = 0; r < visemeRawData.size(); r++)
		delete visemeRawData[r];

	for (size_t p = 0; p < visemeProcessedData.size(); p++)
		delete visemeProcessedData[p];


	return finalCurves;
}



}
