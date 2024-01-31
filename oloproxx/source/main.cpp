#include <oloproxx/config>

#include <stacktrace>
#include <string>


#include <fmt/std.h>
#include <spdlog/spdlog.h>

#include <oloproxx/engine/Window.h>

import systems;
import types;
import utils;


int main( int argc, char **argv )
{
	SetConsoleOutputCP( 65001 );
	SetConsoleCP( 65001 );
	try
	{
		systems::console::Console::initialize( argv, argc );
		systems::console::Console::setLogLevel( spdlog::level::trace );
		systems::console::Console::setConsoleTitle( "oloproxx" );
		set_default_logger( utils::create_logger( "application" ) );

		window::Window window;
	} catch ( const std::exception &ex )
	{
		O_EXCEPTION_HANDLER( main, ex )
		return -1;
	}

	return 0;
}
