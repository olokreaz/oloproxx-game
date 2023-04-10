#include <SimpleIni.h>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <fmt/core.h>
#include <sago/platform_folders.h>


namespace fs = boost::filesystem;


namespace error_handler
{

	// SI_OK = 0; //!< No error
	// SI_UPDATED = 1; //!< An existing value was updated
	// SI_INSERTED = 2; //!< A new value was inserted
	// 
	// for any error with (retval < 0) 
	// SI_FAIL = -1; //!< Generic failure
	// SI_NOMEM = -2; //!< Out of memory error
	// SI_FILE = -3; //!< File error (see errno for detail error)

	void LoadConfigFile(std::string path, short code) {}


}


int main(int, char **)
{
	fs::path project_file = fs::path(sago::getConfigHome()) / "Dooplet"; // Get the %appdata%/project.name path
	create_directories(project_file);                                    // Create the directory if it doesn't exist

	CSimpleIniA ini;
	int         error = ini.LoadFile((project_file / "settings.ini").c_str());
	if (error != 0)
		error_handler::LoadConfigFile((project_file / "settings.ini").string(), error);

	int                      o;
	const long long unsigned op = 0;
	return 0;
}
