﻿#include "Configer.hpp"

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
	this -> m_config_path = path;

	try
	{
		m_config . readFile( path . string( ) );

		const lc::Setting &root = m_config . getRoot( );

		{
			std::string src  = root[ "global" ][ "source" ];
			std::string dest = root[ "global" ][ "destination" ];

			this -> source      = src;
			this -> destination = dest;
		}

		this -> special . reserve( root[ "paths" ] . getLength( ) );
		for ( size_t i = 0; i < root[ "paths" ] . getLength( ); ++i )
		{
			const auto &pair = root[ "paths" ][ i ];
			this -> special . insert( { pair[ "pattern" ], pair[ "destination" ] } );
		}

		this -> ignore . reserve( root[ "ignore" ] . getLength( ) );
		for ( size_t i = 0; i < root[ "ignore" ] . getLength( ); ++i ) this -> ignore . push_back( root[ "ignore" ][ i ] );
	} catch ( const lc::ParseException &e ) { spdlog::error( "Error in config file: {}, {}, {}", e . what( ), e . getFile( ), e . getLine( ) ); }
}

void help::Config::save( fs::path path )
{
	lc::Config cfg;
	auto &     root                   = cfg . getRoot( );
	root[ "global" ][ "source" ]      = this -> source . string( );
	root[ "global" ][ "destination" ] = this -> destination . string( );

	for ( auto &i : this -> special )
	{
		auto &pair            = root[ "paths" ] . add( lc::Setting::TypeGroup );
		pair[ "pattern" ]     = i . first;
		pair[ "destination" ] = i . second;
	}

	for ( auto &i : this -> ignore ) root[ "ignore" ] . add( lc::Setting::TypeString ) = i;

	cfg . writeFile( path . string( ) );
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