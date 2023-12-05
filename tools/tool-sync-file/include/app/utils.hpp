#pragma once
#include <fstream>
#include <ranges>
#include <string>
#include <vector>

#include <cppfs/FileHandle.h>
#include <cppfs/fs.h>

#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

import tools_sync_file.config;

namespace utils
{
	namespace fs = std::filesystem;

	/** @brief Создает и настраивает логгер с указанным именем.
	* Логгер записывает логи одновременно в файл каждый день и в stdout.
	* Также при создании логгера добавляется trace сообщение.
	*
	* @param name Имя создаваемого логгера.
	* @return Разделяемый указатель на созданный логгер.
	*/
	inline std::shared_ptr< spdlog::logger > create_logger( std::string name )
	{
		using namespace std::chrono_literals;

		static auto siDialy  = std::make_shared< spdlog::sinks::daily_file_sink_mt >( ( fs::current_path( ) / config::kLogs_dir / "log.txt" ) . string( ), 0, 0 );
		static auto siStdout = std::make_shared< spdlog::sinks::stdout_color_sink_mt >( );
		static bool bInit    = false;

		if ( !bInit )
		{
			siStdout -> set_color( spdlog::level::trace, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE );
			bInit = true;
		}

		auto logger = std::make_shared< spdlog::logger >( name );

		logger -> sinks( ) . clear( );

		logger -> sinks( ) . push_back( siStdout );
		logger -> sinks( ) . push_back( siDialy );

		logger -> flush_on( spdlog::level::warn );
		logger -> set_pattern( config::kLogger_pattern );
		logger -> set_level( spdlog::get_level( ) );

		logger -> trace( "logger created" );

		return logger;
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

	namespace ufs
	{
		static void create_dirs( std::vector< std::string > &vPath ) { for ( auto &p : vPath ) if ( !fs::exists( p ) ) fs::create_directories( p ); }

		static void copy_to( const fs::path src, const fs::path dest )
		{
			auto hSrc  = cppfs::fs::open( src . string( ) );
			auto hDest = cppfs::fs::open( dest . string( ) );

			if ( !hDest . exists( ) && hDest . isDirectory( ) ) hDest . createDirectory( );
			if ( hSrc . isFile( ) ) hSrc . copy( hDest );
			else hSrc . copyDirectoryRec( hDest );
		}

		static void remove( const fs::path &path )
		{
			auto hSrc = cppfs::fs::open( path . string( ) );
			if ( hSrc . isFile( ) && hSrc . exists( ) ) hSrc . remove( );
			else if ( hSrc . isDirectory( ) && hSrc . exists( ) ) hSrc . removeDirectoryRec( );
		}
	}}
