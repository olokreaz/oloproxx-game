module;
#include <oloproxx/config>

#include <chrono>
#include <filesystem>
#include <string>
#include <windows.h>

#include <argh.h>

#include <glog/logging.h>

export module systems.console;
import utils;
import config;
import types;


namespace systems::console
{
	using namespace std::chrono_literals;


	export enum class EWindowStatus : bool
	{
		eHide ,
		eShow
	};


	export class Console
	{
		Console( ) = delete;

		static inline argh::parser s_args;

		inline static EWindowStatus s_status;
		inline static HWND          s_hConsole;

	public:
		static void initialize( char **av, int ac )
		{

			if ( !s_hConsole ) s_hConsole = GetConsoleWindow( );


			FLAGS_alsologtostderr = true;
			FLAGS_log_dir         = ( utils::exe_path( ) / "logs" ) . string( ) . c_str( );


			google::InitGoogleLogging( av[ 0 ] );
			google::EnableLogCleaner( 3 );

			s_args . parse( av, ac );
		}

		static void setConsoleTitle( const std::string &tittle )
		{
			SetConsoleTitleA( tittle . c_str( ) );
			DLOG( INFO ) << "Set Console Tittle: " << tittle;
		}


		static void setStatus( const EWindowStatus status, const HWND hwnd = nullptr )
		{
			const int  command = status == EWindowStatus::eShow ? SW_SHOW : SW_HIDE;
			const HWND window  = hwnd ? hwnd : s_hConsole;

			ShowWindow( window, command );

			if ( window == s_hConsole )
			{
				s_status = status;
				DLOG( INFO ) << ( status == EWindowStatus::eShow ? "Show" : "Hide" ) << " Console";
			} else
				DLOG( INFO ) << ( status == EWindowStatus::eShow ? "Show" : "Hide" ) << " Window";
		}

		const argh::parser static Args( ) { return s_args; }
	};
}
