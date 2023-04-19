#include <argh.h>
#include <commandline.h>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/filesystem/string_file.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <sago/platform_folders.h>

namespace fs = boost::filesystem;

#include <yaml-cpp/yaml.h>

import utils;

int main(int, char **)
{
	const fs::path project_dir = fs::path(sago::getConfigHome()) / "oloprox" / "dooplet";
	Commandline cli;

	utils::LoadGeneralConfig(project_dir, cli);

	YAML::Node config;
	utils::file(project_dir / "config.yaml") >> config;
	utils::GenereteConfig(project_dir, cli, config);

	

	return 0;
}
