#include <chrono>
#include <csignal>
#include <yaml-cpp/yaml.h>

#include <efsw/efsw.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

#include <spdlog/spdlog.h>

#define fs_boost

#ifdef fs_boost
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#include "deteils.hpp"

using namespace fmt::literals;
using namespace std;

const fs::path gitignoreFile = ".gitignore";
const fs::path shadersFolder = "shaders";

std::unordered_map< string, Hash_t > g_fileHashes;

static Config g_config;

Config load_config( const fs::path &path )
{
	YAML::Node node = YAML::LoadFile( path . string( ) );
	Config     c
			{
					.source = node[ "source" ] . as< string >( ),
					.destination = node[ "destination" ] . as< string >( ),
					.bin = node[ "bin" ] . as< string >( ) == "debug" ? eDebug : eRelease,
					.ignore = node[ "ignore" ] . as< vector< string > >( ),
					.node = node
			};
	return c;
}

class CCopyFileListener final : public efsw::FileWatchListener
{
public:
	CCopyFileListener( ) {}

	void handleFileAction(
			efsw::WatchID      watchid,
			const std::string &dir_,
			const std::string &filename_,
			efsw::Action       action,
			std::string        oldFilename
			) override
	{
		fs::path dir      = dir_;
		fs::path filename = filename_;
		fs::path current  = dir / filename;

		if ( filename . string( ) . ends_with( "~" ) ) return;

		// Проверка на игнорируемые расширения файлов
		if ( std::ranges::find( g_config . ignore, current . extension( ) ) != g_config . ignore . end( ) ) return;
		// Проверка на игнорируемые директории
		if ( std::ranges::find( g_config . ignore, current . parent_path( ) . stem( ) ) != g_config . ignore . end( ) ) return;
		// Проверка на игнорируемые файлы
		if ( std::ranges::find( g_config . ignore, current . stem( ) ) != g_config . ignore . end( ) ) return;

		fs::path rPath = fs::relative( current, g_config . source );

		std::string DebugPath   = "bin Debug";
		std::string ReleasePath = "bin Release";

		if ( ( g_config . bin == eDebug && rPath . string( ) . find( ReleasePath ) != std::string::npos ) ||
			( g_config . bin == eRelease && rPath . string( ) . find( DebugPath ) != std::string::npos ) )
			return;

		// Если путь содержит "bin/Debug" или "bin/Release", убираем эту часть из пути
		if ( g_config . bin == eDebug && rPath . string( ) . find( DebugPath ) != std::string::npos )
		{
			std::string s     = rPath . string( );
			size_t      start = s . find( DebugPath ) + DebugPath . length( );
			rPath             = s . substr( start );
		} else if ( g_config . bin == eRelease && rPath . string( ) . find( ReleasePath ) != std::string::npos )
		{
			std::string s     = rPath . string( );
			size_t      start = s . find( ReleasePath ) + ReleasePath . length( );
			rPath             = s . substr( start );
		}

		string strAction = "coppied";

		switch ( action )
		{
			case efsw::Action::Add:
			{
				copy_to(
					g_config . source / rPath, g_config . destination / ( is_directory( rPath ) ? rPath : rPath . parent_path( ) )
					);
				break;
			}
			case efsw::Action::Modified:
			{
				copy_to(
					g_config . source / rPath,
					g_config . destination / ( is_directory( rPath ) ? rPath : rPath . parent_path( ) )
					);

				break;
			}
			case efsw::Action::Moved:
			{
				copy_to(
					g_config . source / rPath,
					g_config . destination / ( is_directory( rPath ) ? rPath : rPath . parent_path( ) )
					);

				break;
			}
			case efsw::Action::Delete:
			{
				delete_path( g_config . destination / rPath );
				strAction = "deleted";
				break;
			}
			default: { break; }
		}
		spdlog::info( "{} {} -> {}", strAction, current . string( ), ( g_config . destination / rPath ) . string( ) );
	}
};

class CObserverMovedFileHandler : public IObserverHandler
{
	fs::path m_relativePath;

protected:
	void add_( const fs::path &path, efsw::Action action, fs::path oldpath ) override
	{
		m_relativePath = fs::relative( path, g_config . source );
		spdlog::debug( "[CObserverMovedFileHandler]: get relative path \"{}\".", m_relativePath . string( ) );
		copy_to( path, g_config . destination / m_relativePath );
	}
};

static bool g_bQuit = { false };

class CObserverConfig : public IObserverHandler
{
protected:
	void modified_( const fs::path &path, efsw::Action action, fs::path oldpath ) override
	{
		if ( path . filename( ) == "config.yml" )
		{
			g_config = load_config( path );
			spdlog::info( "load configuration" );
		}
	}

	void add_( const fs::path &path, efsw::Action action, fs::path oldpath ) override {}
	void moved_( const fs::path &path, efsw::Action action, fs::path oldpath ) override {}
	void delete_( const fs::path &path, efsw::Action action, fs::path oldpath ) override {}
	void default_( const fs::path &path, efsw::Action action, fs::path oldpath ) override {}
};
class CObserverNotifyer final : public IObserverHandler
{
protected:
	void add_( const fs::path &path, efsw::Action action, fs::path oldpath ) override { spdlog::info( "{} {}", path . string( ), to_underlying( action ) ); }
	void modified_( const fs::path &path, efsw::Action action, fs::path oldpath ) override { spdlog::info( "{} {}", path . string( ), to_underlying( action ) ); }
	void moved_( const fs::path &path, efsw::Action action, fs::path oldpath ) override { spdlog::info( "{} {}", path . string( ), to_underlying( action ) ); }
	void delete_( const fs::path &path, efsw::Action action, fs::path oldpath ) override { spdlog::info( "{} {}", path . string( ), to_underlying( action ) ); }
	void default_( const fs::path &path, efsw::Action action, fs::path oldpath ) override { spdlog::info( "{} {}", path . string( ), to_underlying( action ) ); }
};

struct CWatcherID
{
	fs::path      path;
	efsw::WatchID id;
};

void stop_signal( int code )
{
	fmt::println( "Goodbay, thx for using my tool <3" );
	g_bQuit = true;
}

int main( int argc, char **argv )
{
	signal( SIGTERM, stop_signal );
	signal( SIGBREAK, stop_signal );
	signal( SIGABRT, stop_signal );
	signal( SIGINT, stop_signal );

	auto execute_path = fs::current_path( );
	g_config          = load_config( execute_path / "config.yml" );

	efsw::FileWatcher watcher;
	CCopyFileListener listener;

	CObserver< CObserverNotifyer > notifyer;
	CObserver< CObserverConfig >   Configlistener;

	watcher . followSymlinks( false );
	watcher . allowOutOfScopeLinks( false );

	CWatcherID sourceID {
					g_config . source,
					watcher . addWatch( g_config . source . string( ), &listener, true )
			};

	CWatcherID configID
			{
					execute_path,
					watcher . addWatch( execute_path . string( ), &Configlistener, false )
			};

	CWatcherID destinationID {
					g_config . destination,
					watcher . addWatch( g_config . destination . string( ), &notifyer, true )
			};

	fmt::println( "Watching {} & {} for changes...", sourceID . path . string( ), sourceID . id );
	fmt::println( "Watching {} & {} for changes...", configID . path . string( ), configID . id );
	fmt::println( "Watching {} & {} for changes...", destinationID . path . string( ), destinationID . id );

	watcher . watch( );

	while ( !g_bQuit ) this_thread::sleep_for( 75ms );

	this_thread::sleep_for( 5s );

	return 0;
}
