#include <filesystem>
#include <string>

#include <sago/platform_folders.h>

#include <spdlog/spdlog.h>

import system;
import types;

import core.Itype;
import core.handler.url;

#define WHILE while (true)

inline static const std::filesystem::path kAppDataPath = std::filesystem::path( sago::getDataHome( ) ) / ".oloprox";

static bool g_bQuit = false;

class IHandler
{
public:
	template< typename... Args > void operator()( Args... arg ) { }

	virtual void handle( ) { }

};

int wmain( int, wchar_t ** )
{
	sys::Console::initialize( );
	sys::Console::setLogLevel( spdlog::level::trace );
	sys::Console::setConsoleTitle( "oloprox - louncher" );

	// WHILE { }

	return 0;
}
