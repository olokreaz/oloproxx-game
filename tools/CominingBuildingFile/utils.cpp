#include "utils.hpp"

#include <filesystem>
#include <fstream>
#include <vector>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
namespace json = nlohmann;

void help::Config::load( fs::path path )
{
	json::json j           = json::json::parse( std::ifstream( path ) );
	this -> source         = j[ "global" ][ "source" ] . get< std::string >( );
	this -> destination    = j[ "global" ][ "destination" ] . get< std::string >( );
	this -> modified_files = j[ "paths" ] . get< std::unordered_map< std::string, std::string > >( );
	this -> ignore         = j[ "global" ][ "ignore" ] . get< std::vector< std::string > >( );
}

std::string help::calculateSha256( const std::filesystem::path &filePath )
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
