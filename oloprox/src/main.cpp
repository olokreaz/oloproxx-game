#include <filesystem>

#include <spdlog/spdlog.h>

import system;
import types;

static bool g_bQuit = false;


int wmain( int, wchar_t ** )
{
	sys::Console::initialize( );
	sys::Console::setLogLevel( spdlog::level::trace );
	sys::Console::setConsoleTitle( "oloprox - terminal" );




	return 0;
}
