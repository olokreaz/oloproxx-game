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

#include <coroutine>
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

#include <clipp.h>

int main( int, char ** )
{
	#ifndef _DEBUG
	spdlog::register_logger( g_logger );
	#endif
	spdlog::set_pattern( "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v" );

	spdlog::info( "Start application" );

	CApp app;

	app.run( );
	spdlog::info( "Finish application" );

	return 0;
}
