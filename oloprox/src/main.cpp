#include <string>
#include <vector>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include <oloprox/data/data_test_txt.hpp>

import systems;
import types;

static bool g_bQuit = false;

#define DATA_TEXT 101

int wmain( int, wchar_t ** )
{
	systems::console::Console::initialize( );
	systems::console::Console::setLogLevel( spdlog::level::trace );
	systems::console::Console::setConsoleTitle( "oloprox - terminal" );

	set_default_logger( systems::logging::create_logger( "global" ) );

	const auto test = std::string( ( char * ) build_data_bassets_test_txt, build_data_bassets_test_txt_len );

	fmt::print( "{}\n", test );

	return 0;
}
