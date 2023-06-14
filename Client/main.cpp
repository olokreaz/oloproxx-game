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
const static string logfilename = fmt::format(
						"logs/{:%Y-%m-%d}.log"
						, fmt::localtime( std::time( nullptr ) )
						);
#endif

#include <Application/Application.hpp>

int main( int argc, char **argv )
{
	#ifndef _DEBUG
	spdlog::set_default_logger( spdlog::basic_logger_mt( "Global"  , logfilename) );
	spdlog::set_pattern( "[%Y-%m-%d %H:%M:%S.%e] [%n] [%^---%l---%$] [thread %t] %v" );
	#else
	set_default_logger( spdlog::stdout_color_mt( "Global" ) );
	spdlog::stdout_color_mt( "Steam" );
	spdlog::stdout_color_mt( "Networking" );
	spdlog::stdout_color_mt( "Discord Bot" );
	// spdlog::stdout_color_mt("Discord Rpc" );
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
