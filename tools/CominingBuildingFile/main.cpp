#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>

#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <yaml-cpp/yaml.h>
namespace fs = std::filesystem;

using namespace fmt::literals;

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
	}
};

std::string calculateSHA256( const fs::path &filePath )
{
	std::ifstream       file( filePath, std::ios::binary );
	std::vector< char > buffer( std::istreambuf_iterator< char >( file ), { } );

	CryptoPP::SHA256     hash;
	std::string          digest;
	CryptoPP::HexEncoder encoder;

	hash . Update( ( const CryptoPP::byte * ) buffer . data( ), buffer . size( ) );
	digest . resize( hash . DigestSize( ) );
	hash . Final( ( CryptoPP::byte * ) &digest[ 0 ] );

	encoder . Attach( new CryptoPP::StringSink( digest ) );
	encoder . Put( ( CryptoPP::byte * ) &digest[ 0 ], hash . DigestSize( ) );
	encoder . MessageEnd( );

	return digest;
}

int main( int, char ** )
{
	try
	{
		Config config;
		config . load( "config.yaml" );
	} catch ( const std::exception &e ) { }

	return 0;
}
