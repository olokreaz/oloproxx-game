module;

#include <string>
#include <windows.h>

#include <argh.h>
#include <sigc++/sigc++.h>

#include <spdlog/spdlog.h>

export module systems.console;
import utils;
import config;
import types;

namespace systems::console
{
	export enum class EWindowStatus
	{
		eHide ,
		eShow
	};

	export class Console
	{
		Console( ) = delete;

		inline static std::shared_ptr < spdlog::logger > s_log = utils::create_logger( "application" );
		static inline argh::parser                       s_args;

		inline static EWindowStatus s_status;
		inline static HWND          s_hConsole;

	public:
		static void initialize( char **av, int ac )
		{
			using namespace std::chrono_literals;
			if ( !s_hConsole ) s_hConsole = GetConsoleWindow( );

			register_logger( s_log );
			set_default_logger( s_log );

			spdlog::set_automatic_registration( true );
			spdlog::set_level( spdlog::level::trace );
			spdlog::flush_on( spdlog::level::warn );
			spdlog::flush_every( 1s );
			spdlog::set_pattern( config::kLogger_pattern );

			s_args . parse( av, ac );
		}

		static void setConsoleTitle( const std::string &tittle )
		{
			SetConsoleTitleA( tittle . c_str( ) );
			spdlog::debug( "Set Console Tittle: {}", tittle );
		}

		static void setLogLevel( const spdlog::level::level_enum level, std::shared_ptr < spdlog::logger > logger = spdlog::default_logger( ) )
		{
			logger -> set_level( level );
			spdlog::info( "{} logger, setted {} level", logger -> name( ), to_string_view( level ) );
		}

		static void setStatus( const EWindowStatus status, const HWND hwnd = nullptr )
		{
			const int  command = static_cast < bool >( status ) ? SW_SHOW : SW_HIDE;
			const HWND window  = hwnd ? hwnd : s_hConsole;

			ShowWindow( window, command );

			if ( window == s_hConsole )
			{
				s_status = status;
				spdlog::debug( "{} Console", status == EWindowStatus::eShow ? "Show" : "Hide" );
			} else spdlog::debug( "{} Window", status == EWindowStatus::eShow ? "Show" : "Hide" );
		}

		const argh::parser static Args( ) { return s_args; }
	};
}
