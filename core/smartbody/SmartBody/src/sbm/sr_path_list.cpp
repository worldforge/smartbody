#include "sr_path_list.h"
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBScene.h>

srPathList::srPathList()
{
	_curIndex = 0;
}

srPathList::~srPathList() = default;
		
bool srPathList::insert(const std::string& path)
{
	for (auto & _path : _paths)
	{
		if (_path == path)
			return false;
	}

	_paths.emplace_back(path);
	return true;	
}

bool srPathList::remove(const std::string& path)
{
	for (auto iter = _paths.begin();
		iter != _paths.end();
		iter++)
	{
		if ((*iter) == path)
		{
			_paths.erase(iter);
			return true;
		}
	}

	return false;	
}
		
void srPathList::removeAll()
{
	_paths.clear();
}
	

void srPathList::reset()
{	
	_curIndex = 0;
}
		
std::string srPathList::next_path(const boost::filesystem::path& prefixPath)
{
	if (size_t(_curIndex) >= _paths.size())
		return "";

	// if the path is an absolute path, don't prepend the media path
	boost::filesystem::path p = _paths[_curIndex];
	if (!prefixPath.empty() && !p.has_root_path())
	{
		 p = prefixPath / p;
	}

	p.normalize();

#ifdef WIN32
	std::string convertedPath = p.string();
	boost::replace_all(convertedPath, "\\", "/");
	_curIndex++;
	return convertedPath;
#else
	_curIndex++;
	return p.string();
#endif

}

std::string srPathList::next_filename(char *buffer, const char *name, const boost::filesystem::path& prefixPath)
{
	std::string path = next_path(prefixPath);
	if( !path.empty() )	{
		std::stringstream strstr;
		strstr << path << "/" << name;
		return( strstr.str() );
	}
	return( "" );
}
