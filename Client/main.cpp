#include <algorithm>
#include <array>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <spdlog/spdlog.h>

#include "include/App/App.h"

using namespace std;
using namespace fmt::literals;

namespace vs = views;
namespace rng = ranges;

#ifndef _DEBUG
#include <spdlog/sinks/basic_file_sink.h>
string logfilename = fmt::format( "logs/{:%Y-%m-%d}.log"
				, fmt::localtime( std::time( nullptr ) )
			      );

static inline shared_ptr< spdlog::logger > g_logger = spdlog::basic_logger_mt( "Global"  , logfilename);
#endif

int main( int argc, char **argv )
{

	#ifndef _DEBUG
	spdlog::register_logger( g_logger );
	#endif
	spdlog::set_pattern( "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v" );

	shared_ptr< CApplication > app = make_shared< CApplication >( );

	return 0;
}

/*
 get Refernce server / client from https://github.com/ValveSoftware/GameNetworkingSockets/blob/master/examples/example_chat.cpp
 */
