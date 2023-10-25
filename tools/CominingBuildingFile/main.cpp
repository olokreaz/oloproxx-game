#include <complex.h>
#include <csignal>
#include <filesystem>

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <efsw/efsw.hpp>

#include <cmt/Configer.hpp>
#include <cmt/FileSynchronizer.hpp>

namespace fs = std::filesystem;

using namespace fmt::literals;
using namespace std::literals;

void ExitHandler( int ) { std::this_thread::sleep_for( 2s ); }

bool g_bQuit = false;

auto operator""arch( const char *str, uint64 len ) {}

int wmain( int, wchar_t ** )
{
	std::signal( SIGTERM, ExitHandler );
	std::signal( SIGINT, ExitHandler );

	spdlog::set_automatic_registration( true );
	spdlog::set_level( spdlog::level::trace );
	spdlog::flush_on( spdlog::level::warn );
	spdlog::flush_every( 1s );
	spdlog::set_pattern( APP_LOGGER_PATTERN );

	std::shared_ptr< help::CConfig > config = std::make_shared< help::CConfig >( );

	config -> load( "config.local" );

	spdlog::trace( "Start setup settings logger" );

	auto logger = utils::create_logger( "global" );

	spdlog::trace( "set new default logger {}", logger -> name( ) );

	set_default_logger( logger );

	spdlog::trace( "finished setted new default logger" );

	spdlog::info( "Strart Work Application" );

	efsw::FileWatcher watcher;
	CFileSynchronizer synchronizer( config );

	watcher . addWatch( config -> source . string( ), &synchronizer, true );

	watcher . watch( );

	while ( !g_bQuit ) std::this_thread::sleep_for( 1s );

	config -> save( );
	return 0;
}
