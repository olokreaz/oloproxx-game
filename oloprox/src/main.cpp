#include <filesystem>

#include <spdlog/spdlog.h>

#include <QtWidgets/QApplication>

import system;
import types;

static bool g_bQuit = false;

int wmain( int, wchar_t ** )
{
	sys::Console::initialize( );
	sys::Console::setLogLevel( spdlog::level::trace );
	sys::Console::setConsoleTitle( "oloprox - terminal" );

	set_default_logger( sys::create_logger( "global" ) );

	QApplication app( __argc, __argv );

	return app.exec( );
}
