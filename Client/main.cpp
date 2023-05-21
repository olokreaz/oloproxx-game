#include <algorithm>
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
static inline shared_ptr< spdlog::logger > g_logger
		= spdlog::basic_logger_mt( "Global"
					   , fmt::format( "logs/{:%Y-%m-%d}.log"
							  , fmt::localtime( std::time( nullptr ) )
							)
					 );
#endif

CApp_t g_app( new CApp( ) );

import <iostream>;

int main( int, char ** )
{
	#ifndef _DEBUG
	set_default_logger( g_logger );
	#endif
	spdlog::set_pattern( "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v" );

	tf::Taskflow tf( "main" );

	struct testData {
		string id;
		string name;
		string password_crypted;
	}              data;

	return 0;
	// return app->run( );
}
