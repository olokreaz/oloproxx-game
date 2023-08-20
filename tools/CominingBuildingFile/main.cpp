#include <argh.h>
#include <chrono>
#include <csignal>
#include <filesystem>
#include <iostream>

#include <efsw/efsw.hpp>
#include <yaml-cpp/yaml.h>

#include <fmt/core.h>
#include <fmt/ranges.h>

namespace fs = std::filesystem;
using namespace std;
using namespace fmt::literals;

const fs::path    gitignoreFile = ".gitignore";
const fs::path    shadersFolder = "shaders";
const std::string spvExtension  = ".spv";
const std::string Debug         = "Debug";
const std::string Release       = "Release";

struct Config {
	fs::path                   source;
	fs::path                   destination;
	std::string                binType;
	std::vector< std::string > ignore_ext;
	std::vector< std::string > ignore_dirs;
	std::vector< std::string > ignore_files;
};

Config load_config( const fs::path &path ) {
	YAML::Node config = YAML::LoadFile( path . string( ) );
	Config     c;
	c . source       = config[ "source" ] . as< std::string >( );
	c . destination  = config[ "destination" ] . as< std::string >( );
	c . binType      = config[ "debug" ] . as< bool >( ) ? Debug : Release;
	c . ignore_ext   = config[ "ignore" ][ "extentions" ] . as< std::vector< std::string > >( );
	c . ignore_dirs  = config[ "ignore" ][ "dirs" ] . as< std::vector< std::string > >( );
	c . ignore_files = config[ "ignore" ][ "files" ] . as< std::vector< std::string > >( );
	return c;
}

void print_cfg( Config &cfg ) {
	fmt::println( "" );
	fmt::println( "====================================" );
	fmt::println( "source: {}", cfg . source . string( ) );
	fmt::println( "destination: {}", cfg . destination . string( ) );
	fmt::println( "binType: {}", cfg . binType );
	fmt::println( "ignore_ext: {}", cfg . ignore_ext );
	fmt::println( "ignore_dirs: {}", cfg . ignore_dirs );
	fmt::println( "ignore_files: {}", cfg . ignore_files );
	fmt::println( "====================================" );
}

struct CCopyMoveFile {
	fs::path source;
	fs::path destination;

	void copy( fs::path file ) {
		auto sourcePath      = source / file;
		auto destinationPath = destination / file;
	}
};

class CCopyFileListener final : public efsw::FileWatchListener {
public:
	CCopyFileListener( Config &cfg ): m_cfg { cfg } {}

	void handleFileAction( efsw::WatchID       watchid,
				const std::string &dir,
				const std::string &filename,
				efsw::Action       action,
				std::string        oldFilename ) override {
		auto extension = fs::path( filename ) . extension( );
		if ( std::ranges::find( m_cfg . ignore_ext, extension ) != m_cfg . ignore_ext . end( ) ) return;

		switch ( action ) {
			case efsw::Action::Add: {
				std::println( std::cout,
						"File {} added in {}",
						filename,
						dir
					);
				break;
			}
			case efsw::Action::Modified: {
				std::println( std::cout,
						"File {}/{} modified",
						dir,
						filename );
				break;
			}
			case efsw::Action::Moved: {
				std::println( std::cout,
						"File {} moved to {}",
						filename,
						dir );
				break;
			}
			case efsw::Action::Delete: {
				std::println( std::cout,
						"File removed {}/{}",
						dir,
						filename );
				break;
			}
			default: {
				std::println( std::cout,
						"File {}/{}, can't found action: {}",
						dir,
						filename,
						std::to_underlying( action ) );
				break;
			}
		}
		oldFilename != "" ? std::println( std::cout, "oldFilename {}", oldFilename ) : void( );
	}

	void setNewConfing( Config cfg ) { m_cfg = cfg; }

protected:
	CCopyMoveFile mover;

private:
	Config m_cfg;
};

class ConfigUpdateListener : public efsw::FileWatchListener {
public:
	ConfigUpdateListener( Config *ptr ) : pCfg { ptr } { }

	void handleFileAction( efsw::WatchID watchid, const std::string &dir, const std::string &filename, efsw::Action action, std::string oldFilename ) override {
		if ( action == efsw::Action::Modified && filename == "config.yml" ) {
			Config cfg = load_config( filename );
			fmt::println( "{:>10}", "load new configuration file" );

			fmt::println( "{:>5}", "old variable config" );
			print_cfg( *pCfg );
			fmt::println( "" );
			fmt::println( "{:>5}", "new variable config" );
			print_cfg( cfg );
			*pCfg = cfg;
		}
	}

protected:
	Config *pCfg = nullptr;
};

static bool g_bQuit = { false };

void stop_signal( int code ) {
	fmt::println( "Thx for using this tool, Goodbay" );
	g_bQuit = true;
}

int main( int argc, char **argv ) {
	signal( SIGTERM, stop_signal );
	signal( SIGBREAK, stop_signal );
	signal( SIGABRT, stop_signal );
	signal( SIGINT, stop_signal );

	Config config = load_config( "config.yml" );

	efsw::FileWatcher    watcher;
	ConfigUpdateListener Configlistener( &config );
	CCopyFileListener    listener( config );

	watcher . followSymlinks( false );
	watcher . allowOutOfScopeLinks( false );

	string execute_path = fs::path( argv[ 0 ] ) . parent_path( ) . string( );

	auto wId1 = watcher . addWatch( config . source . string( ), &listener, true );
	auto wId2 = watcher . addWatch( execute_path, &Configlistener, true );

	fmt::println( "Watching wID = {}:{} and wID = {}:{}", wId1, config . source . string( ), wId2, execute_path );

	watcher . watch( );

	while ( !g_bQuit ) this_thread::sleep_for( 50ms );

	return 0;
}
