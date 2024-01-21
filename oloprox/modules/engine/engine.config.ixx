module;
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <source_location>
#include <string>
#include <typeindex>
#include <unordered_map>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <boost/algorithm/string.hpp>

#include <libconfig.hh>

export module engine.config;
import utils;

namespace lc = libconfig;

namespace engine::config
{
	// Реестр для регистрации и создания конфигураций
	export class ConfigRegistry final
	{
		lc::Config                                       m_config;
		std::unordered_map< std::string, lc::Setting * > m_components;

	protected:
		static inline auto _logger = utils::create_logger( "ConfigRegistry" );

		ConfigRegistry( )
		{
			try { if ( std::filesystem::exists( "settings.conf" ) ) m_config . readFile( "settings.conf" ); } catch ( const lc::FileIOException &e )
			{
				_logger -> error( "Error load config: {}", e . what( ) );
			}
			catch ( const lc::ParseException &e ) { _logger -> error( "Error parse config: {}", e . what( ) ); }
			catch ( const std::exception &    e ) { _logger -> error( "Error: {}", e . what( ) ); }
			catch ( ... ) { _logger -> error( "Unknown error" ); }
		}

	public:
		static ConfigRegistry& instance( )
		{
			static ConfigRegistry instance;
			return instance;
		}

	protected:
		lc::Setting& __registerConfig( std::string id )
		{
			boost::to_lower( id );
			spdlog::info( "{} config registrated", id );
			return *( m_components[ id ] = &( m_config . getRoot( )[ id ] = lc::Setting::Type::TypeGroup ) );
		}

	public:
		lc::Setting& at( std::string_view id )
		{
			std::string x( id . data( ) );
			boost::to_lower( x );
			auto it = m_components . find( x );
			if ( it != m_components . end( ) ) return *it -> second;
			return __registerConfig( x );
		}

		lc::Setting& operator[]( std::string_view id ) { return at( id ); }

		void save( )
		{
			try { m_config . writeFile( "settings.conf" ); } catch (
				lc::FileIOException &ex ) { _logger -> error( "Error save config: {}", ex . what( ) ); }
		}

		~ConfigRegistry( ) { save( ); }
	};
}
