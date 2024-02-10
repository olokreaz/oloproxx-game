#include <oloproxx/config>

#include <stacktrace>
#include <string>
#include <windows.h>
#include <glog/logging.h>
#include <glog/stl_logging.h>

#include "oloproxx/engine/engine.hpp"

#pragma comment(lib, "vulkan-1.lib")

import systems;
import types;
import utils;


int main( int argc, char **argv )
{
	SetConsoleOutputCP( 65001 );
	SetConsoleCP( 65001 );
	try
	{
		systems::console::Console::initialize( argv, argc );
		systems::console::Console::setConsoleTitle( "oloproxx" );

		Engine engine;
		engine . run( );
	} catch ( const std::exception &ex ) { return -1; }


	return 0;
}
