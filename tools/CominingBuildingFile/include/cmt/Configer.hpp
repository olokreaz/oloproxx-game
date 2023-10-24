#pragma once
#include <filesystem>
#include <string>

#include <libconfig.h++>
#include <boost/functional/hash.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

import types;

namespace lc = libconfig;

namespace help
{
	using namespace fmt::literals;
	using namespace std::chrono_literals;
	namespace fs = std::filesystem;

	struct CConfig
	{
		fs::path                                       source;
		fs::path                                       destination;
		std::unordered_map< std::string, std::string > special;
		std::vector< std::string >                     ignore;

		void load( fs::path path );
		void save( fs::path path );
		void save( ) { this -> save( this -> m_config_path ); }

		friend uint64 hash_value( const CConfig &obj )
		{
			uint64 seed = 0x53723B02;
			boost::hash_combine( seed, obj . source );
			boost::hash_combine( seed, obj . destination );
			boost::hash_combine( seed, obj . special );
			boost::hash_combine( seed, obj . ignore );
			return seed;
		}

	private:
		lc::Config m_config;
		fs::path   m_config_path;
	};

	std::string calculateSha256( const std::filesystem::path &filePath );
}
