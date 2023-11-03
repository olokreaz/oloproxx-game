#include <filesystem>

#include <spdlog/spdlog.h>

import system;
import types;

static bool g_bQuit = false;

#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtQml/QtQml>
#include <QtQml/QQmlApplicationEngine>

int wmain( int, wchar_t ** )
{
	sys::Console::initialize( );
	sys::Console::setLogLevel( spdlog::level::trace );
	sys::Console::setConsoleTitle( "oloprox - terminal" );

	QApplication app( __argc, __argv );

	app . setApplicationName( "oloprox" );


	return app . exec( );
}
