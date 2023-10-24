#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

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

	void copy_directory_to( fs::path &src, fs::path &path );
	void copy_file_to( fs::path &src, fs::path &path );
	void remove_file( fs::path &path );
	void remove_directory( fs::path &path );
}
