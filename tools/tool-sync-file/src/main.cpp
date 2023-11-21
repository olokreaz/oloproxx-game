#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <csignal>
#include <filesystem>

#include <app/Configer.hpp>
#include <app/utils.hpp>

#include <cppfs/FileHandle.h>
#include <cppfs/FileWatcher.h>
#include <cppfs/fs.h>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fmt/std.h>
#include <fmt/xchar.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>

#include <magic_enum_all.hpp>

#include <argh.h>

#include <app/SyncBinaryWatcher.hpp>

namespace fs = std::filesystem;

using namespace fmt::literals;
using namespace std::literals;

import app.commiting.config;

bool g_bQuit = false;

void ExitHandler( int )
{
	g_bQuit = true;
	std::this_thread::sleep_for( 2s );
}

int wmain( int, wchar_t ** )
{
	std::signal( SIGTERM, ExitHandler );
	std::signal( SIGINT, ExitHandler );

	argh::parser cmdl( __argv );

	spdlog::set_automatic_registration( true );
	spdlog::set_level( spdlog::level::trace );
	spdlog::flush_on( spdlog::level::warn );
	spdlog::flush_every( 1s );
	spdlog::set_pattern( config::kLogger_pattern );

	spdlog::trace( "Start setup settings logger" );

	auto logger = utils::create_logger( config::kGlobal_logger_name );

	spdlog::trace( "Set new default logger {}", logger -> name( ) );

	set_default_logger( logger );

	spdlog::trace( "Finished setted new default logger" );
	spdlog::info( "Strart Application" );

	std::shared_ptr< help::CConfig > pConfig = std::make_shared< help::CConfig >( );

	pConfig -> bReleased = cmdl[ { "-d", "--debug" } ] ? false : cmdl[ { "-r", "--release" } ];

	pConfig -> load( config::kConfig_file_name );

	spdlog::debug( "Source: {}", pConfig -> source );
	spdlog::debug( "Destination: {}", pConfig -> destination );

	for ( const auto &pair : pConfig -> special ) spdlog::debug( "Special: {} -> {} : {}", pair . first, pair . second . destination, pair . second . bRelease );
	for ( const auto &ignore : pConfig -> ignore ) spdlog::debug( "Ignore: {}", ignore );

	cppfs::FileWatcher watcher;
	CSyncBinaryWatcher syncWatcher( pConfig );

	auto hSource = cppfs::fs::open( pConfig -> source . string( ) );

	watcher . addHandler( &syncWatcher );
	watcher . add(
			hSource, cppfs::FileEvent::FileCreated | cppfs::FileEvent::FileRemoved | cppfs::FileEvent::FileModified | cppfs::FileEvent::FileAttrChanged,
			cppfs::Recursive
			);

	while ( !g_bQuit )
	{
		watcher . watch( );
		std::this_thread::sleep_for( 1s );
	}

	pConfig -> save( );

	return 0;
}
