#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <steam/steam_api.h>

#include "include/App/App.h"

using namespace fmt::literals;
using namespace std;

namespace vs = views;
namespace rng = ranges;

#include <absl/time/clock.h>
#ifndef _DEBUG
static inline shared_ptr< spdlog::logger > g_logger
		= spdlog::basic_logger_mt( "Global"
					   , "logs/" + FormatTime( "%Y-%m-%d %H-%M-%S"
								   , absl::Now( )
								   , absl::LocalTimeZone( )
								 ) +
					   ".log"
					 );
#endif

static inline Commandline        g_cmd;
static inline shared_ptr< CApp > app( new CApp( ) );

int main( int, char ** )
{
	#ifndef _DEBUG
	set_default_logger( g_logger );
	#endif

	
	
	return app->run( );
}
