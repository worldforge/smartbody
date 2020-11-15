#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <sstream>
#include <vector>
#include <map>

class CSLUConverter
{
public:
	static void convertFromCSLUToBML(const std::string& rootDirectory, const std::string& mappingFileFullPath);
	static void generateDiphoneBML(const std::string& iFileName, const std::string& oFileName, const std::string& ntpFileName, const std::string& txtFileName, const std::string& baseFileName, const std::map<std::string, std::string> mapping);
};
