#pragma once
#include <filesystem>
#include <ranges>
#include <string>

#include <fmt/std.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <cppfs/Diff.h>
#include <cppfs/FileHandle.h>
#include <cppfs/Tree.h>
#include <cppfs/fs.h>

#include <efsw/efsw.hpp>

#include <pcrecpp.h>

namespace fs = std::filesystem;

namespace help
{
	struct Config;
}

extern help::Config g_config;

namespace help
{
	namespace fs = std::filesystem;
	using namespace fmt::literals;

	struct PathFormTo
	{
		fs::path src;
		fs::path dst;
	};

	struct Config
	{
		fs::path                                                                                  source;
		fs::path                                                                                  destination;
		std::unordered_map< std::string /*glob patter or full path*/, std::string /* path to */ > specific_path;
		std::vector< std::string >                                                                ignore;

		void load( fs::path path );
	};

	std::string calculateSha256( const std::filesystem::path &filePath );

	class IObserver
	{
	protected:
		IObserver( const fs::path _root, const fs::path _dist )
		{
			/* Create file Descriptor */
			_dirRoot = cppfs::fs::open( _root . string( ) );
			_dirDist = cppfs::fs::open( _dist . string( ) );
			/* Create trees */
			_treeRoot = _dirRoot . readTree( );
			_treeDist = _dirDist . readTree( );
		}

	public:
		cppfs::FileHandle _dirRoot = cppfs::fs::open( g_config . source . string( ) );
		cppfs::FileHandle _dirDist = cppfs::fs::open( g_config . destination . string( ) );

		std::unique_ptr< cppfs::Tree > _treeRoot = _dirRoot . readTree( );
		std::unique_ptr< cppfs::Tree > _treeDist = _dirRoot . readTree( );

		virtual void _CopyFile( const fs::path &src, const fs::path &dst ) = 0;
		virtual void _CopyDir( const fs::path &src, const fs::path &dst ) = 0;
		virtual void _RemoveFile( const fs::path &src, const fs::path &dst ) = 0;
		virtual void _RemoveDir( const fs::path &src, const fs::path &dst ) = 0;

		virtual void _update( fs::path &path ) { }
	};

	template< typename T > requires std::derived_from< T, IObserver >
	class Handler final : public efsw::FileWatchListener
	{
		inline static auto s_log = spdlog::stdout_color_mt( std::string( typeid( T ) . name( ) ) . substr( 5 ) );

		std::vector< pcrecpp::RE > m_ignore;

		bool PathValidator( fs::path &path )
		{
			return !std::ranges::any_of( m_ignore, [&path] ( pcrecpp::RE &item ) { return item . FullMatch( path . string( ) ); } );
		}

	protected:
		std::unique_ptr< IObserver > _obsrv = T::create( );

	public:
		Handler( ) = default;

		Handler( std::vector< std::string > &ignore )
		{
			m_ignore . reserve( ignore . size( ) );
			for ( auto &item : ignore ) m_ignore . push_back( pcrecpp::RE( item ) );
		}

		Handler( std::vector< std::string > &&ignore )
		{
			m_ignore . reserve( ignore . size( ) );
			for ( auto &item : ignore ) m_ignore . push_back( pcrecpp::RE( std::move( item ) ) );
		}

		void handleFileAction(
				efsw::WatchID watchid, const std::string &dir, const std::string &filename,
				efsw::Action  action, std::string         oldFilename
				) override
		{
			auto diff = _obsrv -> _treeRoot -> createDiff( *_obsrv -> _treeDist );

			for ( auto &item : diff -> changes( ) )
			{
				fs::path path = item . path( );

				_obsrv -> _update( path );

				if ( PathValidator( path ) ) return;

				using Operation = cppfs::Change::Operation;

				std::string action_str { };
				switch ( item . operation( ) )
				{
					case Operation::CopyFile:
					{
						_obsrv -> _CopyFile( path, path );
						action_str = "Copy File";
						break;
					}
					case Operation::CopyDir:
					{
						_obsrv -> _CopyDir( path, path );
						action_str = "Copy Directory";
						break;
					}
					case Operation::RemoveFile:
					{
						_obsrv -> _RemoveFile( path, path );
						action_str = "Remove File";
						break;
					}
					case Operation::RemoveDir:
					{
						_obsrv -> _RemoveDir( path, path );
						action_str = "Remove Directory";
						break;
					}
					case Operation::None:
					default:
					{
						action_str = "Nono";
						break;
					}
				}

				s_log -> trace( "{} {}", path, action_str );
			}
		}
	};
}
