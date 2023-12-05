#include <iostream>
#include <string>
#include <vector>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include <glm/glm.hpp>

import systems;
import types;

int main( int argc, char **argv )
{
	systems::console::Console::initialize( );
	systems::console::Console::setLogLevel( spdlog::level::trace );
	systems::console::Console::setConsoleTitle( "oloprox - terminal" );

	set_default_logger( systems::logging::create_logger( "global" ) );

	return 0;
}
