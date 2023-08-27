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

using namespace fmt::literals;
using namespace std;

const fs::path gitignoreFile = ".gitignore";
const fs::path shadersFolder = "shaders";

using Hash_t = std::string;
std::unordered_map< string, Hash_t > g_fileHashes;

std::string compute_sha256( const std::string &file_path ) {
	CryptoPP::SHA256 hash;
	Hash_t           result;

	CryptoPP::FileSource( file_path . c_str( ),
				true,
				new CryptoPP::HashFilter( hash,
							new CryptoPP::HexEncoder( new CryptoPP::StringSink( result ) )
							)
			);

	return result;
}

enum EBinType {
	eDebug   = 0 ,
	eRelease = 1 ,
};

struct Config {
	fs::path                   source;
	fs::path                   destination;
	EBinType                   bin;
	std::vector< std::string > ignore_ext;
	std::vector< std::string > ignore_dirs;
	std::vector< std::string > ignore_files;
	YAML::Node                 node;
};

static Config g_config;

Config load_config( const fs::path &path ) {
	YAML::Node node = YAML::LoadFile( path . string( ) );
	Config     c
			{
			.source = node[ "source" ] . as< string >( ),
			.destination = node[ "destination" ] . as< string >( ),
			.bin = node[ "bin" ] . as< string >( ) == "debug" ? eDebug : eRelease,
			.ignore_ext = node[ "ignore" ][ "extentions" ] . as< vector< string > >( ),
			.ignore_dirs = node[ "ignore" ][ "dirs" ] . as< vector< string > >( ),
			.ignore_files = node[ "ignore" ][ "files" ] . as< vector< string > >( ),
			.node = node
			};
	return c;
}

void print_cfg( const Config &cfg ) {
	fmt::println( "========================================" );
	fmt::println( "source: {}", cfg . source . string( ) );
	fmt::println( "destination: {}", cfg . destination . string( ) );
	fmt::println( "bin: {}", cfg . bin == eDebug ? "debug" : "release" );
	fmt::println( "ignore_ext: {}", cfg . ignore_ext );
	fmt::println( "ignore_dirs: {}", cfg . ignore_dirs );
	fmt::println( "ignore_files: {}", cfg . ignore_files );
	fmt::println( "========================================" );
}

void copy_to( const fs::path &source, const fs::path &destination ) {
	// Создаем целевой каталог, если он еще не существует
	if ( !fs::exists( destination ) ) fs::create_directories( destination );

	// Итерируем через исходный каталог и рекурсивно копируем все вложенные файлы и каталоги
	if (fs::exists(source))
		if (fs::is_directory(source))
	for ( const auto &dir_entry : fs::recursive_directory_iterator( source ) ) {
		auto current = dir_entry . path( );
		auto target  = destination / fs::relative( current, source );

		// Проверка на игнорируемые расширения файлов
		if ( std::ranges::find( g_config . ignore_ext, current . extension( ) ) != g_config . ignore_ext . end( ) ) continue;

		// Проверка на игнорируемые директории
		if ( std::ranges::find( g_config . ignore_dirs, current . parent_path( ) . stem( ) ) != g_config . ignore_dirs . end( ) ) continue;

		// Проверка на игнорируемые файлы
		if ( std::ranges::find( g_config . ignore_files, current . stem( ) ) != g_config . ignore_files . end( ) ) continue;

		// Является ли текущий элемент каталогом?
		if ( fs::is_directory( current ) ) fs::create_directories( target ); // Если да, то создаем копию каталога
		else {
			auto hash = compute_sha256( current . string( ) ); // в противном случае это файл, проверяем его хеш
			if ( g_fileHashes . count( target . string( ) ) == 0 || g_fileHashes[ target . string( ) ] != hash ) {
				fs::copy_file( current, target, fs::copy_options::overwrite_existing ); // Если файл не существует или хеш отличается, копируем его
				g_fileHashes[ target . string( ) ] = hash;
			}
		}
	}
	else fs::copy_file( source, destination, fs::copy_options::overwrite_existing );
}

bool delete_path( const fs::path target ) {
	if ( fs::exists( target ) ) {
		// Если цель - это директория, удаляем её и всё внутри
		if ( fs::is_directory( target ) ) return fs::remove_all( target );
		// Если цель - это файл, удаляем его
		if ( fs::is_regular_file( target ) ) return fs::remove( target );
	}
	spdlog::info( "Target does not exist" );
	return false;
}

class CCopyFileListener final : public efsw::FileWatchListener {
public:
	CCopyFileListener( ) {}

	void handleFileAction( efsw::WatchID       watchid,
				const std::string &dir_,
				const std::string &filename_,
				efsw::Action       action,
				std::string        oldFilename ) override {
		fs::path dir      = dir_;
		fs::path filename = filename_;
		fs::path path     = dir / filename;

		if ( filename . string( ) . ends_with( "~" ) ) return;

		{
			auto extension = fs::path( filename_ ) . extension( );
			if ( std::ranges::find( g_config . ignore_ext, extension ) != g_config . ignore_ext . end( ) ) return;
		}

		{
			auto extension = fs::path( filename_ ) . extension( );
			if ( std::ranges::find( g_config . ignore_ext, extension ) != g_config . ignore_ext . end( ) ) return;
		}

		if ( std::ranges::find( g_config . ignore_dirs, dir_ ) != g_config . ignore_dirs . end( ) ) return;
		if ( std::ranges::find( g_config . ignore_files, ( fs::path( dir_ ) / filename_ ) . string( ) ) != g_config . ignore_files . end( ) ) return;

		fs::path rPath = fs::relative( path, g_config . source );

		std::string DebugPath   = "bin Debug";
		std::string ReleasePath = "bin Release";

		if ( ( g_config . bin == eDebug && rPath . string( ) . find( ReleasePath ) != std::string::npos ) ||
			( g_config . bin == eRelease && rPath . string( ) . find( DebugPath ) != std::string::npos ) )
			return;

		// Если путь содержит "bin/Debug" или "bin/Release", убираем эту часть из пути
		if ( g_config . bin == eDebug && rPath . string( ) . find( DebugPath ) != std::string::npos ) {
			std::string s     = rPath . string( );
			size_t      start = s . find( DebugPath ) + DebugPath . length( );
			rPath             = s . substr( start );
		} else if ( g_config . bin == eRelease && rPath . string( ) . find( ReleasePath ) != std::string::npos ) {
			std::string s     = rPath . string( );
			size_t      start = s . find( ReleasePath ) + ReleasePath . length( );
			rPath             = s . substr( start );
		}

		string strAction = "coppied";

		switch ( action ) {
			case efsw::Action::Add: {
				copy_to( g_config . source / rPath, g_config . destination / ( is_directory( rPath ) ? rPath : rPath . parent_path( ) ) );
				break;
			}
			case efsw::Action::Modified: {
				copy_to(
					g_config . source / rPath,
					g_config . destination / ( is_directory( rPath ) ? rPath : rPath . parent_path( ) )
					);

				break;
			}
			case efsw::Action::Moved: {
				copy_to(
					g_config . source / rPath,
					g_config . destination / ( is_directory( rPath ) ? rPath : rPath . parent_path( ) )
					);

				break;
			}
			case efsw::Action::Delete: {
				delete_path( g_config . destination / rPath );
				strAction = "deleted";
				break;
			}
			default: { break; }
		}
		spdlog::info( "{} {} -> {}", strAction, path . string( ), ( g_config . destination / rPath ) . string( ) );
	}
};

class ConfigUpdateListener : public efsw::FileWatchListener {
public:
	ConfigUpdateListener( Config *ptr ) : pCfg { ptr } { }

	void handleFileAction( efsw::WatchID watchid, const std::string &dir, const std::string &filename, efsw::Action action, std::string oldFilename ) override {
		if ( filename == "config.yml" ) {
			*pCfg = load_config( "config.yml" );
			fmt::println( "load configuration {}", ( fs::path( dir ) / filename ) . string( ) );
			print_cfg( *pCfg );
		}
	}

protected:
	Config *pCfg = nullptr;
};

static bool g_bQuit = { false };

void stop_signal( int code ) {
	fmt::println( "Goodbay, thx for using my tool <3" );
	g_bQuit = true;
}

struct CWatcherID {
	fs::path      path;
	efsw::WatchID id;
};

class CNotifyer final : public efsw::FileWatchListener {
public:
	void handleFileAction( efsw::WatchID watchid, const std::string &dir, const std::string &filename = "", efsw::Action action = efsw::Action::Add,
				std::string  oldFilename                                                  = "" ) override {
		if ( filename . ends_with( "~" ) ) return;
		switch ( action ) {
			case efsw::Action::Add: {
				spdlog::info( "File {}/{} added", dir, filename );
				break;
			}
			case efsw::Action::Modified: {
				spdlog::info( "File {}/{} modified", dir, filename );
				break;
			}
			case efsw::Action::Moved: {
				spdlog::info( "File {}/{} moved", dir, filename );
				break;
			}
			case efsw::Action::Delete: {
				spdlog::info( "File {}/{} deleted", dir, filename );
				break;
			}
			default: ;
		}
	}
};

int main( int argc, char **argv ) {
	signal( SIGTERM, stop_signal );
	signal( SIGBREAK, stop_signal );
	signal( SIGABRT, stop_signal );
	signal( SIGINT, stop_signal );

	auto execute_path = fs::current_path( );
	g_config          = load_config( execute_path / "config.yml" );

	efsw::FileWatcher    watcher;
	ConfigUpdateListener Configlistener( &g_config );
	CCopyFileListener    listener;
	CNotifyer            notifyer;

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
