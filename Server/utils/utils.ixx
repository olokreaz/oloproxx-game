module;

export module utils;
#include <vector>
#include <boost/filesystem.hpp>

export namespace utils
{
	void CreateDir(std::vector<std::string> &vDir)
	{
		for (const auto &dir : vDir)
			boost::filesystem::create_directories(dir);
		
	}
}
