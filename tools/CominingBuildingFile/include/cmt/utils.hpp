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
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <cmt/config.hpp>

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

		// Создаём daily file sink
		static auto siDialy = std::make_shared< spdlog::sinks::daily_file_sink_mt >( ( fs::current_path( ) / APP_LOG_DIR / "log.txt" ) . string( ), 0, 0 );
		// Создаём stdout color sink
		static auto siStdout = std::make_shared< spdlog::sinks::stdout_color_sink_mt >( );

		// Создаем нового логгера с полученным именем
		auto logger = std::make_shared< spdlog::logger >( name );

		// Очищаем sinks логгера
		logger -> sinks( ) . clear( );
		// Добавляем stdout и daily file sinks в логгер
		logger -> sinks( ) . push_back( siStdout );
		logger -> sinks( ) . push_back( siDialy );

		// Устанавливаем уровень сообщений, при котором произойдет автоматическое сброса буфера
		logger -> flush_on( spdlog::level::warn );

		// Устанавливаем паттерн для форматирования сообщений логгера
		logger -> set_pattern( APP_LOGGER_PATTERN );

		logger -> set_level( spdlog::get_level( ) );

		// Логгер сообщает о своем создании
		logger -> trace( "logger created" );

		// Возвращаем созданный логгер
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
		static void create_dirs( std::unordered_map< std::string, std::string > &vPath )
		{
			for ( auto &p : vPath | std::ranges::views::values ) if ( !fs::exists( p ) ) fs::create_directories( p );
		}

		static void copy_to( const fs::path &src, const fs::path &dest )
		{
			auto hSrc  = cppfs::fs::open( src . string( ) );
			auto hDest = cppfs::fs::open( dest . string( ) );
			if ( hSrc . isFile( ) ) hSrc . copy( hDest );
			else hSrc . copyDirectoryRec( hDest );
		}

		static void remove( const fs::path &path )
		{
			auto hSrc = cppfs::fs::open( path . string( ) );
			if ( hSrc . isFile( ) ) hSrc . remove( );
			else hSrc . removeDirectoryRec( );
		}
	}}
