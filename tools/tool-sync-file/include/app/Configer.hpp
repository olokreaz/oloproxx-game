#pragma once
#include <filesystem>
#include <string>

#include <libconfig.h++>
#include <boost/functional/hash.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <steam/steamtypes.h>

namespace lc = libconfig;

namespace help
{
	using namespace fmt::literals;
	using namespace std::chrono_literals;
	namespace fs = std::filesystem;

	struct CConfig
	{
		/**
		 * \brief
		 * \param key pattern
		 * \param value destination
		 */
		fs::path                                       source;
		fs::path                                       destination;
		std::unordered_map< std::string, std::string > special;
		std::vector< std::string >                     ignore;

		/** \details относительно папки нахождения exe */
		void load( fs::path path );
		/** \details относительно папки нахождения exe */
		void save( fs::path path );
		/** \details относительно папки нахождения exe */
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
		uint64     m_hash;
		lc::Config m_config;
		fs::path   m_config_path;
	};
}
