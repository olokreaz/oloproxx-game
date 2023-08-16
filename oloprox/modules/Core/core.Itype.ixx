module;

#include <memory>
#include <typeindex>
#include <unordered_map>

export module core.Itype;

namespace core::interface {
	export class IHandler {
	public:
		virtual ~IHandler( ) = default;

		template< typename... Args > void operator()( Args &&... args ) { handleImpl( std::index_sequence_for< Args... >( ), std::forward< Args >( args )... ); }

		// Нужно реализовать в каждом конкретном классе обработчика
		virtual void handle( ) = 0;

	protected:
		template< class Self > void registerate( );

	private:
		template< std::size_t... I, typename... Args > void handleImpl( std::index_sequence< I... >, Args &&... args ) { handle( ( I, args )... ); }
	};

	export struct HandlersRegistry {
		static HandlersRegistry& instance( ) {
			static HandlersRegistry registry;
			return registry;
		}

		template< typename T > void registerHandler( ) { m_handlers[ typeid( T ) ] = std::make_shared< T >( ); }

		std::shared_ptr< IHandler >& getHandler( std::type_index index ) { return m_handlers[ index ]; }

	private:
		std::unordered_map< std::type_index, std::shared_ptr< IHandler > > m_handlers;
	};

	template< class Self > void IHandler::registerate( ) { HandlersRegistry::instance( ) . registerHandler< Self >( ); }
}
