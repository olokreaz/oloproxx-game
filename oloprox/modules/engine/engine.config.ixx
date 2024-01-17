module;
#include <functional>
#include <memory>
#include <source_location>
#include <string>
#include <typeindex>
#include <unordered_map>

#include <boost/algorithm/string.hpp>

#include <nlohmann/json.hpp>

#include <boost/dll.hpp>

#include <cpp_redis/cpp_redis>

export module engine.config;
import utils;

namespace json = nlohmann;

namespace engine::config
{
	struct PropertyBase
	{
		virtual                ~PropertyBase( ) = default;
		json::json &           jsonObject;
		const std::string_view name;
		const std::type_index  type;

		virtual void init( ) = 0;

		PropertyBase( json::json &jsonObject, std::string_view name, std::type_index type ) : jsonObject { jsonObject },
													name { name },
													type { type } {}
	};

	export template< class T > class Property : protected PropertyBase
	{
		const T m_default_v;
		T       m_v;

	protected:
		void init( ) override
		{
			if ( jsonObject . contains( name . data( ) ) ) m_v = jsonObject[ name . data( ) ] . get< T >( );
			else
			{
				m_v                          = m_default_v;
				jsonObject[ name . data( ) ] = m_default_v;
			}
		}

	public:
		Property( std::string_view name, json::json &jsonObject, T default_v = T( ) ) : PropertyBase { jsonObject, name, typeid( T ) },
												m_default_v { default_v } {}

		T& operator*( ) const { return m_v; }

		T* operator->( ) const { return &m_v; }

		[[nodiscard]] operator T( ) const { return m_v; }

		Property& operator=( const T &v )
		{
			m_v                          = v;
			jsonObject[ name . data( ) ] = v;
			return *this;
		}
	};

	export class IConfig
	{
	protected:
		std::string_view              m_config_id;
		json::json                    m_config;
		std::vector< PropertyBase * > m_properties;

	public:
		void __table( json::json &j ) { m_config = std::move( j ); }
		void __id( std::string_view id ) { m_config_id = id; }

		template< class _Ty > _Ty operator[]( std::string_view name ) const { return m_config[ name ]; }

		void init( ) { for ( const auto &x : m_properties ) x -> init( ); }

		std::string_view id( ) const { return m_config_id; }

		virtual ~IConfig( ) = default;
	};

	export class Exception : public std::exception
	{
	public:
		const std::string          m_what_msg;
		const std::source_location m_location;
		const std::string          config_id;

		~Exception( ) noexcept override;

		[[nodiscard]] const char* what( ) const override { return m_what_msg . c_str( ); }

		Exception( const std::string &name, const std::string &msg, std::source_location loc = std::source_location::current( ) ) : m_what_msg { msg },
																	m_location { loc },
																	config_id { name } {}
	};

	// Реестр для регистрации и создания конфигураций
	export class ConfigRegistry final
	{
		std::unordered_map< std::string, std::shared_ptr< IConfig > > m_configs;
		std::unordered_map< std::string, json::json >                 m_config_jsons;
		std::unordered_map< std::string, std::string >                m_config_files;

	public:
		void __registerConfig( std::string id, std::function< std::shared_ptr< IConfig >( ) > creator )
		{
			if ( id . empty( ) ) throw Exception( "ConfigRegistry::__rewgisterConfig", "id is empty" );
			auto className = id;
			boost::to_lower( id );
			if ( id . find( "config" ) == std::string::npos ) throw Exception( className, "ID must have containts Config in Symbol" );
			auto x = id . substr( 0, id . find( "config" ) );

			m_configs[ x ] = creator( );

			std::filesystem::path p = boost::dll::program_location( ) . string( );
			p /= "configs";
			p /= "_" + x + ".json";

			m_config_files[ x ] = p . string( );

			if ( !exists( p ) ) { if ( !std::ofstream( p ) . is_open( ) ) throw Exception( className, "Can't create config file or open files" ); } else
			{
				m_config_jsons[ x ] = json::json::parse( std::ifstream( p ) );
				m_configs[ x ] -> __id( x );
				m_configs[ x ] -> __table( m_config_jsons[ x ] );
			}
			m_configs[ x ] -> init( );
		}

		template< class T > requires std::is_base_of_v< IConfig, T >
		std::shared_ptr< T > at( std::string_view id )
		{
			std::string x( id . data( ) );
			boost::to_lower( x );
			auto it = m_configs . find( x . data( ) );
			if ( it != m_configs . end( ) ) return std::dynamic_pointer_cast< T >( it -> second );
			return nullptr;
		}

		template< class T > requires std::is_base_of_v< IConfig, T >
		std::shared_ptr< T > at( )
		{
			auto x = std::string( typeid( T ) . name( ) );
			boost::to_lower( x );
			return at< T >( x . substr( 0, x . find( "config" ) ) );
		}

		std::shared_ptr< IConfig > at( const std::string &id ) const
		{
			boost::to_lower( id );
			auto it = m_configs . find( id );
			if ( it != m_configs . end( ) ) return it -> second;
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
