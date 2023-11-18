#include <string>
#include <vector>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

import systems;
import types;;


int wmain( int, wchar_t ** )
{
	systems::console::Console::initialize( );
	systems::console::Console::setLogLevel( spdlog::level::trace );
	systems::console::Console::setConsoleTitle( "oloprox - terminal" );

	set_default_logger( systems::logging::create_logger( "global" ) );



	return 0;
}
