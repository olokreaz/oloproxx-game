#include <csignal>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <yaml-cpp/yaml.h>

#include "IHandelr.hpp"

namespace fs = std::filesystem;

using namespace fmt::literals;
using namespace std::literals;

struct Config
{
	fs::path                                       source;
	fs::path                                       destination;
	std::unordered_map< std::string, std::string > modified_files;
	std::vector< std::string >                     ignore;

	void load( fs::path path )
	{
		YAML::Node configYaml = YAML::LoadFile( path . string( ) );

		source      = configYaml[ "source" ] . as< std::string >( );
		destination = configYaml[ "destination" ] . as< std::string >( );
		ignore      = configYaml[ "ignore" ] . as< std::vector< std::string > >( );

		for ( auto &i : configYaml[ "copy_src_to_dst" ] . as< std::vector< help::PathFormTo > >( ) ) modified_files . emplace( );
	}
};

Config      g_config;
static bool g_Quit;

std::string calculateSHA256( const std::filesystem::path &filePath )
{
	std::ifstream       file( filePath, std::ios::binary );
	std::vector< char > buffer( ( std::istreambuf_iterator< char >( file ) ), std::istreambuf_iterator< char >( ) );

	CryptoPP::SHA256 hash;

	CryptoPP::byte digest[ CryptoPP::SHA256::DIGESTSIZE ]; // Создаём массив байт для хранения байт хеша.

	hash . Update( ( const CryptoPP::byte * ) buffer . data( ), buffer . size( ) );
	hash . Final( digest );

	// String simply containing the digest (not human readable)
	std::string s( ( const char * ) digest, CryptoPP::SHA256::DIGESTSIZE );

	// Convert to human readable hex
	std::string            hexDigest;
	CryptoPP::StringSource ss2( s, true, new CryptoPP::HexEncoder( new CryptoPP::StringSink( hexDigest ) ) );

	return hexDigest;
}

class BinaryHandler : public help::IObserver
{
protected:
	void _onRemove( const fs::path &path ) override
	{
		const auto relative = fs::relative( path, g_config . source );
		auto       absolete = g_config . destination / relative;

		std::error_code code;
		fs::remove( absolete, code );

		if ( code ) spdlog::error( "Error removed, code error: {}, {}", code . value( ), code . message( ) );
	}

	bool _filter( fs::path &path ) override { return path . string( ) . find( ( g_config . source / "bin" ) . string( ) ) == std::string::npos; }

	void _update_context( fs::path &path ) override { }
};

void ExitHandler( int c )
{
	spdlog::info( "clean cache" );
	g_Quit = true;
	exit( 0 );
}

int main( int, char ** )
{
	signal( SIGTERM, ExitHandler );
	signal( SIGINT, ExitHandler );

	try
	{
		g_config . load( "config.yml" );

		efsw::FileWatcher              fw;
		help::Handler< BinaryHandler > handler;

		spdlog::set_level( spdlog::level::trace );

		auto wId = fw . addWatch( ( g_config . source ) . string( ), &handler, true );
		fw . watch( );

		while ( !g_Quit ) std::this_thread::sleep_for( 1s );
	} catch ( const std::exception &ex ) { spdlog::error( "{}", ex . what( ) ); }

	return 0;
}
