#include <filesystem>

#include <sago/platform_folders.h>

#include <spdlog/spdlog.h>

import system;
import types;

inline static const std::filesystem::path kAppDataPath = std::filesystem::path( sago::getDataHome( ) ) / ".oloprox";

static bool g_bQuit = false;

int wmain( int, wchar_t ** )
{
	sys::Console::initialize( );
	sys::Console::setLogLevel( spdlog::level::trace );
	sys::Console::setConsoleTitle( "oloprox - louncher" );

	// WHILE { }

	return 0;
}
