#include <filesystem>

#include <spdlog/spdlog.h>

import system;
import types;

static bool g_bQuit = false;

#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QtQml>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

int wmain( int, wchar_t ** )
{
	sys::Console::initialize( );
	sys::Console::setLogLevel( spdlog::level::trace );
	sys::Console::setConsoleTitle( "oloprox - terminal" );

	QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );

	QGuiApplication app( __argc, __argv );

	QQmlApplicationEngine engine;
	const QUrl            url( QStringLiteral( "qrc:/qml/ui/main.qml" ) );
	QObject::connect(
			&engine, &QQmlApplicationEngine::objectCreated,
			&app,
			[url] ( QObject *obj, const QUrl &objUrl ) { if ( !obj && url == objUrl ) QCoreApplication::exit( -1 ); }, Qt::QueuedConnection
			);

	engine . load( url );

	return app . exec( );
}
