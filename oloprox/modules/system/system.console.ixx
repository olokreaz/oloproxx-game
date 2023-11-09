module;

#include <string>
#include <windows.h>
#include <sigc++/sigc++.h>
#include <spdlog/spdlog.h>

#include <magic_enum_all.hpp>

export module system.console;
import system.logging;
import app.config;
import types;

namespace system::console
{
	export enum class EWindowStatus
	{
		eHide ,
		eShow
	};

	export class Console
	{
		Console( ) = delete;

	public:
		static void initialize( )
		{
			using namespace std::chrono_literals;
			if ( !m_hConsole )
			{
				m_hConsole = GetConsoleWindow( );

				auto logger = logging::create_logger( "console" );

				register_logger( logger );
				set_default_logger( logger );

				spdlog::set_automatic_registration( true );
				spdlog::set_level( spdlog::level::trace );
				spdlog::flush_on( spdlog::level::warn );
				spdlog::flush_every( 1s );
				spdlog::set_pattern( config::kLogger_pattern );
			}
		}

	protected:
		inline static std::shared_ptr< spdlog::logger > _log;
		inline static sigc::signal< void( uint32 ) >    _on_exit;

	public:
		static void registrate_handler_for_exit( sigc::slot< void( uint32 ) > slot ) { _on_exit . connect( slot ); }

		static void registrate_handler_for_exit( std::initializer_list< sigc::slot< void( uint32 ) > > slots { for ( auto &slot : slots ) _on_exit . connect( slot ); }

		static void setConsoleTitle( const std::string &tittle )
		{
			SetConsoleTitleA( tittle . c_str( ) );
			spdlog::info( "Set Console Tittle: {}", tittle );
		}

		static void setLogLevel( const spdlog::level::level_enum level )
		{
			spdlog::set_level( level );
			spdlog::info( "Set Log Level: {}", magic_enum::enum_name( level ) );
		}

		auto static setStatus( const EWindowStatus status, const HWND hwnd = nullptr )
		{
			const int  command = status ? SW_SHOW : SW_HIDE;
			const HWND window  = hwnd ? hwnd : m_hConsole;

			ShowWindow( window, command );

			if ( window == m_hConsole )
			{
				m_status = status;
				spdlog::debug( "{} Console", status == EWindowStatus::eShow ? "Show" : "Hide" );
			} else spdlog::debug( "{} Window: {}", status == EWindowStatus::eShow ? "Show" : "Hide", hwnd -> unused );
		}

		auto static getState( ) { return m_status; }

	private:
		inline static EWindowStatus m_status;
		inline static HWND          m_hConsole;
	};
}
