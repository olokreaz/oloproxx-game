#pragma once
#include <filesystem>
#include <ranges>
#include <string>

#include <fmt/chrono.h>
#include <fmt/std.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <libconfig.h++>

#include <pcrecpp.h>

using namespace fmt::literals;
using namespace std::chrono_literals;

namespace fs = std::filesystem;
namespace lc = libconfig;

namespace help
{
	struct Config;
}

namespace help
{
	namespace fs = std::filesystem;
	using namespace fmt::literals;

	struct Config
	{
		fs::path                                                                                  source;
		fs::path                                                                                  destination;
		std::unordered_map< std::string /*glob patter or full path*/, std::string /* path to */ > special;
		std::vector< std::string >                                                                ignore;

		void load( fs::path path );
		void save( fs::path path );
		void save( ) { this -> save( this -> m_config_path ); }

	private:
		lc::Config m_config;
		fs::path   m_config_path;
	};

	std::string calculateSha256( const std::filesystem::path &filePath );
}
