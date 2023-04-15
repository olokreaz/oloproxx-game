#include <argh.h>
#include <commandline.h>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/filesystem/string_file.hpp>
#include <fmt/core.h>
#include <sago/platform_folders.h>

namespace fs = boost::filesystem;

#include <yaml-cpp/yaml.h>

bool LoadGeneralConfig(const fs::path &dir, Commandline &cli)
{
	// Get the %appdata%/project.name path
	if (!fs::exists(dir))
		create_directories(dir); // Create the directory if it doesn't exist

	if (!fs::exists(dir / "config.yaml"))
		{
			cli.write(fmt::format("Not Found File settins:{}, Create him? Y(yes)/N(no)",
						(dir / "config.yaml").string()));

			while (true)
				if (cli.has_command())
					{
						if (cli.get_command() == "Y")
							{
								std::ofstream
									fs((dir / "config.yaml").string());
								// Creat General Config
								if (!fs.is_open())
									throw "Dont Create File";
								fs.close();
								return false;
							}

						if (cli.get_command() == "N")
							{
								cli.write("okay ♥");
								exit(0);
							}

						cli.write("Please, send Y/N (yes or no)");
					}
		}

	return true;
}

bool ParseLoadFile(
	const fs::path project_dir,
	Commandline &  cli,
	YAML::Node     config
	)
{
	config = YAML::LoadFile((project_dir / "config.yaml").string());
	if (config.size() == 0)
		{
			cli.write(fmt::format("This file {{ {} }} haven't settings, Create default settings?",
						(project_dir / "config.yaml").string()));
			while (true)
				if (cli.has_command())
					{
						if (cli.get_command() == "Y")
							return false;

						if (cli.get_command() == "N")
							{
								cli.write("okay ♥");
								exit(0);
							}

						cli.write("Please, send Y/N (yes or no)");
					}
		}
	return true;
}

int main(int, char **)
{
	const fs::path project_dir = fs::path(sago::getConfigHome()) / "oloprox" / "dooplet";
	Commandline    cli;

	LoadGeneralConfig(project_dir, cli);
	std::cout << typeid(cli).name() << std::endl;

	YAML::Node config;
	if (!ParseLoadFile(project_dir, cli, config))
		{
			std::cout << "Generated File " << project_dir / "config.yaml" << std::endl;
			config[ "General" ][ "Config dir" ] = std::vector<std::string> {
					(project_dir / ".config").string(),
					(project_dir / ".settings").string()
				};

			config[ "General" ][ "Temp dir" ] = boost::filesystem::temp_directory_path().string();

			std::ofstream fout((project_dir / "config.yaml").string());
			if (fout.is_open())
				fout << config;
		}

	return 0;
}
