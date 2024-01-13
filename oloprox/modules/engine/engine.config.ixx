module;
#include <functional>
#include <memory>
#include <source_location>
#include <string>
#include <unordered_map>
#include <toml/toml.h>

#include <boost/algorithm/string.hpp>

export module engine.config;
import systems.utils;

namespace engine::config
{
	// Интерфейс для конфигураций
	class IConfig
	{

	public:

		virtual      ~IConfig( ) = default;
	};

	class Exception : public std::exception
	{
		std::string          m_what_msg;
		std::source_location m_location;

	public:
		const std::string config_id;

		Exception( const std::string &name, const std::string &msg, std::source_location loc = std::source_location::current( ) ) : m_what_msg { msg },
																	m_location { loc },
																	config_id { name } {}
	};

	// Реестр для регистрации и создания конфигураций
	class ConfigRegistry
	{
		std::unordered_map< std::string, std::function< std::shared_ptr< IConfig >( ) > > m_configs;

	protected:
		auto m_logger = systems::utils::create_logger( "ConfigRegistry" );

	public:
		void __registerConfig( const std::string id, std::function< std::shared_ptr< IConfig >( ) > creator )
		{
			if ( id . empty( ) ) throw Exception( "ConfigRegistry::__rewgisterConfig", "id is empty" );
			auto className = id;
			boost::to_lower( id );
			if ( id . find( "config" ) == std::string::npos ) throw Exception( className, "ID must have containts Config in Symbol " );
			auto x = id . substr( 0, id . find( "config" ) );

			m_configs[ id ] = creator;
		}

		std::shared_ptr< IConfig > at( const std::string &id ) const
		{
			auto it = m_configs . find( id );
			if ( it != m_configs . end( ) ) return it -> second();
			return nullptr;
		}

		std::shared_ptr< IConfig > operator[]( const std::string &id ) const { return at( id ); }

		static std::shared_ptr< ConfigRegistry > instance( )
		{
			static std::shared_ptr< ConfigRegistry > instance = std::shared_ptr< ConfigRegistry >( new ConfigRegistry );
			return instance;
		}
	};
}
