#pragma once
#include <filesystem>
#include <string>

#include <boost/functional/hash.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <steam/steamtypes.h>

#include <yaml-cpp/yaml.h>

namespace yml = YAML;

namespace help
{
	using namespace fmt::literals;
	using namespace std::chrono_literals;
	namespace fs = std::filesystem;

	struct CConfig
	{
		class CSpecial
		{
		public:
			std::string           pattern;
			std::string           destination;
			std::optional< bool > bRelease = std::nullopt;

			friend std::size_t hash_value( const CSpecial &obj )
			{
				std::size_t seed = 0x2EA94BAB;
				boost::hash_combine( seed, obj . pattern );
				boost::hash_combine( seed, obj . destination );
				boost::hash_combine( seed, obj . bRelease );
				return seed;
			}
		};

		/**
		 * \brief
		 * \param key pattern
		 * \param value destination
		 */
		std::unordered_map< std::string, CSpecial > special;

		fs::path                   source;
		fs::path                   destination;
		std::vector< std::string > ignore;
		bool                       bReleased = false;

		/** \details относительно папки нахождения %appdata% */
		void load( fs::path path );
		/** \details относительно папки нахождения %appdata% */
		void save( fs::path path );
		/** \details относительно папки нахождения %appdata% */
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
		uint64    m_hash;
		yml::Node m_root;
		fs::path  m_config_path;
	};
}
