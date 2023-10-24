#pragma once
#include <fstream>
#include <string>
#include <vector>

#include <cppfs/FileHandle.h>
#include <cppfs/fs.h>

#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

#include <spdlog/spdlog.h>

namespace utils
{
	namespace fs = std::filesystem;

	inline std::shared_ptr< spdlog::logger > create_logger( std::string_view name )
	{
		return std::make_shared< spdlog::logger >( name . data( ), spdlog::default_logger( ) -> sinks( ) . begin( ), spdlog::default_logger( ) -> sinks( ) . end( ) );
	}

	inline std::string calculateSha256( const fs::path &filePath )
	{
		std::ifstream file( filePath, std::ios::binary );
		std::vector   buffer( ( std::istreambuf_iterator( file ) ), std::istreambuf_iterator< char >( ) );

		CryptoPP::SHA256 hash;
		CryptoPP::byte   digest[ CryptoPP::SHA256::DIGESTSIZE ];

		hash . Update( ( const CryptoPP::byte * ) buffer . data( ), buffer . size( ) );
		hash . Final( digest );

		std::string s( ( const char * ) digest, CryptoPP::SHA256::DIGESTSIZE );

		std::string            hexDigest;
		CryptoPP::StringSource ss2( s, true, new CryptoPP::HexEncoder( new CryptoPP::StringSink( hexDigest ) ) );

		return hexDigest;
	}

	namespace filesystem
	{
		void copy_to( const fs::path &src, const fs::path &dest )
		{
			auto hSrc  = cppfs::fs::open( src . string( ) );
			auto hDest = cppfs::fs::open( dest . string( ) );
			hSrc . isFile( ) ? hSrc . copy( hDest ) : hSrc . copyDirectoryRec( hDest );
		}

		void remove( const fs::path &path )
		{
			auto hSrc = cppfs::fs::open( path . string( ) );
			hSrc . isFile( ) ? hSrc . remove( ) : hSrc . removeDirectoryRec( );
		}
	}}
