#include <fstream>
#include <iostream>
#include <fmt/core.h>
#include <fmt/format.h>

#include <boost/algorithm/string.hpp>

#include <utils.hpp>

bool utils::LoadGeneralConfig(const fs::path &dir, Commandline &cli)
{
	if (!fs::exists(dir))
		create_directories(dir);

	if (!fs::exists(dir / "config.yml"))
		{
			cli.write(fmt::format("Not Found File settins:{},\n Create him? Y(yes)/N(no)"
					      , (dir / "config.yml").string()
					)
				);

			while (true)
				if (cli.has_command())
					{
						if (cli.get_command() == "Y")
							{
								std::ofstream fs(
									(dir / "config.yml").string()
									);
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

bool utils::ParseLoadFile(const fs::path project_dir, Commandline &cli, YAML::Node config)
{
	config = YAML::LoadFile((project_dir / "config.yaml").string());
	if (config.size() == 0)
		{
			cli.write(fmt::format(
					"This file {{ {} }} haven't settings, Create default settings?"
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

void utils::GenereteConfig(const fs::path project_dir, Commandline &cli, YAML::Node &config)
{
	if (!utils::ParseLoadFile(project_dir, cli, config))
		{
			std::cout << "Generated File " << project_dir / "config.yaml" << std::endl;
			config[ "General" ][ "Config dir" ] = std::vector {
					(project_dir / ".config").string()
					, (project_dir / ".settings").string()
				};

			config[ "General" ][ "Temp dir" ] = boost::filesystem::temp_directory_path().
				string();
			config[ "General" ][ "Logged" ][ "dir" ]   = (project_dir / ".log").string();
			config[ "General" ][ "Logged" ][ "level" ] = "info";
		}
}

utils::file::file(fs::path path):
	m_path {path} {}

void operator<<(utils::file i, YAML::Node &root)
{
	using namespace fmt::literals;
	std::ofstream ofs(i.m_path.string(), std::ios::trunc);
	if (!ofs.is_open())
		throw fmt::system_error(errno
					, "Can't open file {{ {file} }}\n"
					, "file"_a = i.m_path.string()
			);
	ofs << root;
	ofs.close();
}

void operator>>(utils::file i, YAML::Node &root)
{
	using namespace fmt::literals;
	std::ifstream ifs(i.m_path.string());
	if (!ifs.is_open())
		throw fmt::system_error(errno
					, "Can't open file {{ {file} }}\n"
					, "file"_a = i.m_path.string()
			);
	root = YAML::Load(ifs);
	ifs.close();
}
