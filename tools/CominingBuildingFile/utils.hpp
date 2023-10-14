#pragma once
#include <filesystem>
#include <ranges>
#include <string>

#include <fmt/chrono.h>
#include <fmt/std.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <pcrecpp.h>

using namespace fmt::literals;
using namespace std::chrono_literals;

namespace fs = std::filesystem;

namespace help
{
	struct Config;
}

extern help::Config g_config;

namespace help
{
	namespace fs = std::filesystem;
	using namespace fmt::literals;

	struct PathFormTo
	{
		fs::path src;
		fs::path dst;
	};

	struct Config
	{
		fs::path                                                                                  source;
		fs::path                                                                                  destination;
		std::unordered_map< std::string /*glob patter or full path*/, std::string /* path to */ > specific_path;
		std::vector< std::string >                                                                ignore;

		void load( fs::path path );
	};

	std::string calculateSha256( const std::filesystem::path &filePath );
}
