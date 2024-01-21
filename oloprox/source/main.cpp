#include <oloproxx/config>

#include <stacktrace>
#include <string>
#include <vector>

#include <fmt/std.h>
#include <spdlog/spdlog.h>



#include <string>

import systems;
import types;
import utils;

import engine.config;
import engine.window;
import engine.types;



int main( int argc, char **argv )
{
	try
	{
		systems::console::Console::initialize( argv, argc );
		systems::console::Console::setLogLevel( spdlog::level::trace );
		systems::console::Console::setConsoleTitle( "oloproxx" );
		set_default_logger( utils::create_logger( "application" ) );

		engine::window::Window window;
	} catch ( const std::exception &ex )
	{
		O_EXCEPTION_HANDLER( main, ex )
		return -1;
	}

	return 0;
}
