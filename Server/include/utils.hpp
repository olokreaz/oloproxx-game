#pragma once

#include <commandline.h>
#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

namespace fs = boost::filesystem;

struct utils {
	static bool LoadGeneralConfig(const fs::path &dir, Commandline &cli);

	static bool ParseLoadFile(
		const fs::path project_dir
		, Commandline &cli
		, YAML::Node config
		);

	static void GenereteConfig(const fs::path project_dir, Commandline &cli, YAML::Node &config);

	static class file {
		fs::path m_path;

	public:
		file(fs::path path);

		friend void operator<<(file i, YAML::Node &root);

		friend void operator>>(file i, YAML::Node &root);

	};

};
