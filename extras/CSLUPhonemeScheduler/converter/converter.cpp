#include "vhcl.h"
#include "converter.h"
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>


// extract phoneme from possible initial or final metacharacters in ntp token
// Note some phoneme names contain '(' but never as initial character
// We leave "|" as is, it will never match any phoneme.
static std::string ntpToken2phoneme(std::string& token)
{
	int s = 0;
	int e = token.size() - 1;

	while (token[s] == '[' || token[s] == '(')
		++s;
	while (token[e] == ']' || token[e] == ')')
		--e;

	return token.substr(s, (e-s) + 1);
}

static std::string unescapePhoneme(std::string& token)
{
	if (token == "\\>") // txt1 prints > phoneme as \>
		return ">";
	return token;
}

void CSLUConverter::generateDiphoneBML(const std::string& iFileName, const std::string& oFileName, const std::string& ntpFileName, const std::string& txtFileName, const std::string& baseFileName, const std::map<std::string, std::string> mapping)
{
	std::ifstream ifile(iFileName.c_str());
	std::ifstream ntpFile(ntpFileName.c_str());
	std::ifstream txtFile(txtFileName.c_str());
	std::ofstream ofile(oFileName.c_str());

	if (!ifile.good() || !ofile.good() || !ntpFile.good())
		return;

	std::vector<std::string> lines;
	while (!ifile.eof())
	{
		std::string line;
		std::getline(ifile, line);
		if (line != "")
			lines.push_back(line);
	}
	lines.erase(lines.begin(), lines.begin() + 2);

	std::vector<double> startTimes;
	std::vector<double> endTimes;
	std::vector<std::string> phonemes;
	std::vector<std::string> phonemesBeforeMapping;
	for (size_t i = 0; i < lines.size(); ++i)
	{
		std::vector<std::string> tokens;
		vhcl::Tokenize(lines[i], tokens);
		if (tokens.size() != 3)
		{
			std::cout << "Mal-formatted output from cslu: " << lines[i] << std::endl;
			continue;
		}
		std::map<std::string, std::string>::const_iterator iter = mapping.find(tokens[2]);
		if (iter == mapping.end())
		{
			std::cout << "Mal-formatted output from mapping: " << lines[i] << std::endl;
			continue;
		}
		startTimes.push_back(atof(tokens[0].c_str()) / 1000);
		endTimes.push_back(atof(tokens[1].c_str()) / 1000);
		phonemes.push_back(iter->second);
		phonemesBeforeMapping.push_back(iter->first);
	}

	// guess the word timing
	std::vector<std::string> ntpLines;
	std::string ntpLine = "";
	while (!ntpFile.eof())
	{
		std::string line;
		std::getline(ntpFile, line);
		if (line != "")
			ntpLines.push_back(line);
	}
	if (ntpLines.size() > 0)
		ntpLine = ntpLines[0];	// ntp file is supposed to only have one line
	
	std::vector<std::string> txtLines;
	std::string txtLine = "";
	while (!txtFile.eof())
	{
		std::string line;
		std::getline(txtFile, line);
		if (line != "")
			txtLines.push_back(line);
	}
	if (txtLines.size() > 0)
		txtLine = txtLines[0];	// ntp file is supposed to only have one line

	std::vector<std::string> words;
	std::vector<double> wordStartTimes;
	std::vector<double> wordEndTimes;
	vhcl::Tokenize(txtLine, words);
	std::vector<std::string> ntpTokens;
	vhcl::Tokenize(ntpLine, ntpTokens, " ");
	int phonemeIndex = 0;
	int ntpId = 1;			// start after initial [.pau]
	int sPhonemeId = 0;		// current word starts on first phoneme
	int ePhonemeId = -1;	// until current word end detected

	while (phonemeIndex < (int)phonemesBeforeMapping.size())
	{
		if (phonemesBeforeMapping[phonemeIndex] == ".pau")  // interword pause in phoneme list
		{
			phonemeIndex++;
			sPhonemeId++;		// advance start of current word
			continue;
		}

		// scan through ntpTokens until end-of-word marker, incrementing phonemeIndex on matches
		while (ntpId < (int)ntpTokens.size() && ntpTokens[ntpId] != "[.pau]" ) {
			if (phonemeIndex >= (int)phonemesBeforeMapping.size()) { // all phonemes have been matched
				ntpId++; // continue to [.pau] at end of ntpTokens
			}
			else if (unescapePhoneme(phonemesBeforeMapping[phonemeIndex]) == ntpToken2phoneme(ntpTokens[ntpId++]))
			{
				phonemeIndex++;
			}
		}

		if (ntpTokens[ntpId] == "[.pau]")	// at end of word marker in pronunciation. Should always be true here. 
		{
			// record word with last phoneme time as end
			ePhonemeId = phonemeIndex - 1;
			wordStartTimes.push_back(startTimes[sPhonemeId]);
			wordEndTimes.push_back(endTimes[ePhonemeId]);
			sPhonemeId = phonemeIndex;
			ntpId++;
		}
	}

	if (words.size() != wordStartTimes.size())
	{
		std::cout << iFileName << ": " << "Ntp words not matching txt words!" << std::endl;
	}

	// generate BML file
	ofile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
	ofile << "<bml>" << std::endl;
	ofile << "	<speech id=\"sp1\" start=\"0.0\" ready=\"0.1\" stroke=\"0.1\" relax=\"0.2\" end=\"0.2\">" << std::endl;
	ofile << "		<text>" << std::endl;
	for (size_t i = 0; i < words.size(); ++i)
	{
		ofile << "			<sync id=\"T" << i * 2 + 0 << "\" time=\"" << wordStartTimes[i] << "\" />" << words[i] << std::endl;
		ofile << "			<sync id=\"T" << i * 2 + 1 << "\" time=\"" << wordEndTimes[i] << "\" />" << std::endl;
	}
	ofile << "		</text>" << std::endl;
	ofile << "		<description level=\"1\" type=\"audio/x-wav\">" << std::endl;
	ofile << "			<file ref=\"" << baseFileName << "\" />" << std::endl;
	ofile << "		</description>" << std::endl;
	ofile << "	</speech>" << std::endl;
	for (size_t i = 0; i < phonemes.size(); ++i)
	{
		ofile << "	<lips viseme=\"" << phonemes[i] << "\" articulation=\"1.0\" start=\"" << startTimes[i] << "\" ready=\"" << startTimes[i] << "\" relax=\"" << endTimes[i] << "\" end=\"" << endTimes[i] << "\" />" << std::endl;
	}
	ofile << "	<curves>" << std::endl;
	ofile << "	</curves>" << std::endl;
	ofile << "</bml>" << std::endl;
	ofile.close();

}

void CSLUConverter::convertFromCSLUToBML(const std::string& rootDirectory, const std::string& mappingFileFullPath)
{
	// get mapping
	std::map<std::string, std::string> worldbet2diphone;
	std::ifstream mappingFile(mappingFileFullPath.c_str());
	std::string line;
	while (mappingFile.good())
	{
		getline(mappingFile, line);
		if (line == "")	continue;
		std::vector<std::string> tokens;
		vhcl::Tokenize(line, tokens, "	");
		if (tokens.size() != 4)
		{
			std::cout << "Mal-formatted mapping file" << std::endl;
			continue;
		}
		worldbet2diphone[tokens[0]] = tokens[2];
	}

	for (std::map<std::string, std::string>::iterator iter = worldbet2diphone.begin(); iter != worldbet2diphone.end(); ++iter)
	{
		std::cout << iter->first << "		" << iter->second << std::endl;
	}
	std::cout << "Number of mapping: " << worldbet2diphone.size() << std::endl;
	mappingFile.close();

	// parse files
	boost::filesystem::path path(rootDirectory);
	if (!boost::filesystem::exists(path))
	{
		std::cout << "Directory " << rootDirectory << " does not exist, exiting..." << std::endl;
		return;
	}

	boost::filesystem::directory_iterator endIter;
	for (boost::filesystem::directory_iterator dirIter(path); 
		dirIter != endIter;
		dirIter++)
	{
		if (!boost::filesystem::is_directory(dirIter->status()))
		{
			std::string fileName = dirIter->path().string();
			std::string filebasename = boost::filesystem::basename(fileName);
			std::string fileextension = boost::filesystem::extension(fileName);
			if (fileextension == ".txt1")
			{
				std::string iFileName = rootDirectory + "/" + filebasename + ".txt1";
				std::string oFileName = rootDirectory + "/" + filebasename + ".bml";
				std::string ntpFileName = rootDirectory + "/" + filebasename + ".ntp";
				std::string txtFileName = rootDirectory + "/" + filebasename + ".txt";
				generateDiphoneBML(iFileName, oFileName, ntpFileName, txtFileName, filebasename, worldbet2diphone);
			}
		}
	}
}
