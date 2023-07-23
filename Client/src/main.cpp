#include <iostream>
#include <spdlog/spdlog.h>

import system;
import types;

int main( int, char ** ) {
	sys::Console::INIT( );
	sys::Console::setLogLevel( spdlog::level::trace );

	return 0;
}
