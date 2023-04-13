#include <commandline.h>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <fmt/core.h>
#include <sago/platform_folders.h>
#include <steam/steam_api.h>

namespace fs = boost::filesystem;

// SI_OK = 0; //!< No error
// SI_UPDATED = 1; //!< An existing value was updated
// SI_INSERTED = 2; //!< A new value was inserted
// 
// for any error with (retval < 0) 
// SI_FAIL = -1; //!< Generic failure
// SI_NOMEM = -2; //!< Out of memory error
// SI_FILE = -3; //!< File error (see errno for detail error)

#include <yaml-cpp/yaml.h>

int main(int, char **)
{
	const fs::path project_dir = fs::path(sago::getConfigHome()) / "oloprox" / "dooplet";
	// Get the %appdata%/project.name path
	if (!fs::exists(project_dir))
		create_directories(project_dir); // Create the directory if it doesn't exist

	Commandline cli;
	bool        bNullFileConfig = false;

	if (!fs::exists(project_dir / "config.yaml"))
		{
			cli.write(fmt::format("Not Found File settins:{}, Create him? Y(yes)/N(no)",
						(project_dir / "config.yaml").string()));
			while (true)
				if (cli.has_command())
					{
						if (cli.get_command() == "Y")
							{
								std::ofstream
									fs((project_dir / "config.yaml").string());
								// Creat General Config
								if (!fs.is_open())
									throw;
								fs.close();
								bNullFileConfig = true;
								break;
							}
						if (cli.get_command() == "N")
							cli.write("okay ♥");
return 0;
					}
		}

	

	YAML::Node General_config = YAML::LoadFile((project_dir / "config.yaml").string()); 
		return 0;
}

/*

 */