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

/*
 *	dooplet = MTA5MzIyNjU3Nzg1NDIwNjAxMw.GrGLn7.AL1KrP1xw_aX7qnAwdcgmEV_nYen9P_AiZ1zXc
 *	oloprox = MTA5MzI1MzM1NTMyMjc1MzE0NA.GpsW3c.a3w1AtXOIR82jCNRvP6b4lfkBXNfdBRc0waQBw
 */

#include <yaml-cpp/yaml.h>

int main(int, char **)
{
	const fs::path project_dir = fs::path(sago::getConfigHome()) / "oloprox" / "dooplet";
	// Get the %appdata%/project.name path
	if (!fs::exists(project_dir))
		create_directories(project_dir); // Create the directory if it doesn't exist

	if (!fs::exists(project_dir / "config.yaml"))
		{
			Commandline cli;
			cli.write(fmt::format("Not Found File settins:{}, Create him? Y(yes)/N(no)",
						(project_dir / "config.yaml").string()));
			while (true)
				if (cli.has_command())
					{
						if (cli.get_command() == "yes") { }
						else if (cli.get_command() == "no")
							exit(1);
					}
		}

	YAML::Node General_config = YAML::LoadFile((project_dir / "config.yaml").string());

	return 0;
}
