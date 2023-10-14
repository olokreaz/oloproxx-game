#include "utils.hpp"

#include <filesystem>
#include <fstream>
#include <vector>

#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

#include <libconfig.h++>

namespace fs = std::filesystem;
namespace lc = libconfig;

void help::Config::load( fs::path path )
{
	// utf8 ru
	std::locale::global( std::locale( "ru_RU.UTF-8" ) );

	try
	{
		lc::Config config;
		config . readFile( path . string( ) );

		const lc::Setting &root = config . getRoot( );

		{
			std::string src  = root[ "global" ][ "source" ];
			std::string dest = root[ "global" ][ "destination" ];

			this -> source      = src;
			this -> destination = dest;
		}

		this -> specific_path . reserve( root[ "paths" ] . getLength( ) );
		for ( size_t i = 0; i < root[ "paths" ] . getLength( ); ++i )
		{
			const auto &pair = root[ "paths" ][ i ];
			this -> specific_path . insert( { pair[ "pattern" ], pair[ "destination" ] } );
		}

		this -> ignore . reserve( root[ "ignore" ] . getLength( ) );
		for ( size_t i = 0; i < root[ "ignore" ] . getLength( ); ++i ) this -> ignore . push_back( root[ "ignore" ][ i ] );
	} catch ( const lc::ParseException &e ) { spdlog::error( "Error in config file: {}, {}, {}", e . what( ), e . getFile( ), e . getLine( ) ); }
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
