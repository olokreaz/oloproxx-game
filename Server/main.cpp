#include <commandline.h>
#include <string>
#include <boost/filesystem/path.hpp>
#include <sago/platform_folders.h>

#include <yaml-cpp/yaml.h>

namespace fs = boost::filesystem;

# include <utils.hpp>;

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
