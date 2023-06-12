#include <string>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <steam/steamtypes.h>

using namespace std;
using namespace fmt::literals;

#ifndef _DEBUG
#include <spdlog/sinks/basic_file_sink.h>
const static string logfilename = fmt::format( "logs/{:%Y-%m-%d}.log"
						, fmt::localtime( std::time( nullptr ) )
						);
#endif

#include <Application/Application.hpp>

void foo( std::initializer_list< uint64 > list ) { fmt::print( "foo: {}\n", list ); }

int main( int argc, char **argv )
{
	#ifndef _DEBUG
	static inline shared_ptr< spdlog::logger > g_logger = spdlog::basic_logger_mt( "Global"  , logfilename);
	spdlog::set_default_logger( g_logger );
	spdlog::set_pattern( "[%Y-%m-%d %H:%M:%S.%e] [%n] [%^---%l---%$] [thread %t] %v" );
	#else

	spdlog::stdout_color_mt( "Global" );
	set_default_logger( spdlog::get( "Global" ) );

	spdlog::stdout_color_mt( "Steam" );
	spdlog::stdout_color_mt( "Networking" );

	#endif
	spdlog::set_pattern( "[%Y-%m-%d %H:%M:%S.%e] [%n] [%^---%l---%$] [thread %t] %v" );

	bool bQuit = false;

	CApplication app( &bQuit );
	app . init( );
	app . run( );


	return 0;
}

/*
 get Refernce server / client from https://github.com/ValveSoftware/GameNetworkingSockets/blob/master/examples/example_chat.cpp
 */
