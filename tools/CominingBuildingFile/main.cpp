#include <complex.h>
#include <csignal>
#include <filesystem>

#include <ranges>
#include <unordered_map>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/xchar.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <efsw/efsw.hpp>

#include "Configer.hpp"
#include "FileSynchronizer.hpp"

namespace fs = std::filesystem;

using namespace fmt::literals;
using namespace std::literals;

void ExitHandler( int ) { std::this_thread::sleep_for( 2s ); }

bool g_bQuit = false;

int wmain( int, wchar_t ** )
{
	std::signal( SIGTERM, ExitHandler );
	std::signal( SIGINT, ExitHandler );

	std::shared_ptr< help::Config > config = std::make_shared< help::Config >( );

	config -> load( L"config.local" );

	auto file_sink = std::make_shared< spdlog::sinks::daily_file_sink_mt >( "logs/log.txt", 0, 0 );

	auto logger = std::make_shared< spdlog::logger >( "global", file_sink );

	spdlog::register_logger( logger );

	spdlog::set_default_logger( logger );

	spdlog::flush_every( 1s );
	spdlog::flush_on( spdlog::level::level_enum::warn );

	spdlog::set_level( spdlog::level::trace );

	spdlog::set_pattern( "[ %Y:%m:%d - %H:%M:%S:%F ] [ %l ] [ %t ] <%n> %v" );

	efsw::FileWatcher watcher;
	CFileSynchronizer synchronizer( config );

	watcher . addWatch( config -> source . string( ), &synchronizer, true );

	watcher . watch( );

	while ( !g_bQuit ) std::this_thread::sleep_for( 1s );

	config -> save( );
	return 0;
}
