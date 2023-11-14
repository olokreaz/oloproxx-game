#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <fmt/ranges.h>

import systems;
import types;

static bool g_bQuit = false;

#include <windows.h>
#include  <wil/resource.h>

#define DATA_TEXT 101

int wmain( int, wchar_t ** )
{
	systems::console::Console::initialize( );
	systems::console::Console::setLogLevel( spdlog::level::trace );
	systems::console::Console::setConsoleTitle( "oloprox - terminal" );

	set_default_logger( systems::logging::create_logger( "global" ) );


	return 0;
}
