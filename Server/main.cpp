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

namespace utils {
bool LoadGeneralConfig(const fs::path &dir, Commandline &cli)
{
	// Get the %appdata%/project.name path
	if (!fs::exists(dir))
		create_directories(dir);// Create the directory if it doesn't exist

	if (!fs::exists(dir / "config.yaml"))
		{
			cli.write(fmt::format("Not Found File settins:{}, Create him? Y(yes)/N(no)"
					      , (dir / "config.yaml").string()
					)
				);

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
	const fs::path project_dir
	, Commandline &cli
	, YAML::Node config
	)
{
	config = YAML::LoadFile((project_dir / "config.yaml").string());
	if (config.size() == 0)
		{
			cli.write(fmt::format("This file {{ {} }} haven't settings, Create default settings?"
					      , (project_dir / "config.yaml").string()
					)
				);
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

void GenereteConfig(const fs::path project_dir, Commandline &cli, YAML::Node &config)
{
	if (!utils::ParseLoadFile(project_dir, cli, config))
		{
			std::cout << "Generated File " << project_dir / "config.yaml" << std::endl;
			config[ "General" ][ "Config dir" ] = std::vector {
					(project_dir / ".config").string()
					, (project_dir / ".settings").string()
				};

			config[ "General" ][ "Temp dir" ] = boost::filesystem::temp_directory_path().string();

			std::ofstream fout((project_dir / "config.yaml").string(), std::ios::trunc);
			if (fout.is_open())
				fout << config;
		}
}

template < typename T >
std::string GetTypeName(T)
{
	std::string s = typeid(T).name();
	return s.substr(s.find(" ") != std::string::npos ? s.find(" ") + 1 : 0);
}
}

#include <lua.hpp>

int main(int, char **)
{
	const fs::path project_dir = fs::path(sago::getConfigHome()) / "oloprox" / "dooplet";
	Commandline cli;

	utils::LoadGeneralConfig(project_dir, cli);

	YAML::Node config;
	utils::GenereteConfig(project_dir, cli, config);


	return 0;
}
