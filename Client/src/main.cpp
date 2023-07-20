#include <iostream>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

import Application;
import system;
import types;

int main( int, char ** ) {
	sys::Console::INIT( );
	sys::Console::setLogLevel( spdlog::level::trace );

	app::CApplication app;

	try { app . run( ); } catch ( ... ) {
		// get last input of user
		fmt::print( "\nPress any key to close window..." );
		std::cin . get( );
		return -1;
	}

	return 0;
}
