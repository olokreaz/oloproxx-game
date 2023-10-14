#pragma once
#include <filesystem>
#include <ranges>
#include <string>

#include <fmt/chrono.h>
#include <fmt/std.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <cppfs/Diff.h>
#include <cppfs/FileHandle.h>
#include <cppfs/FilePath.h>
#include <cppfs/Tree.h>
#include <cppfs/fs.h>

#include <efsw/efsw.hpp>

#include <pcrecpp.h>

using namespace fmt::literals;
using namespace std::chrono_literals;

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
		std::shared_ptr< std::vector< std::string > > m_pIgnore;

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

		cppfs::FileHandle _dirRoot { };
		cppfs::FileHandle _dirDist { };

		std::unique_ptr< cppfs::Tree > _treeRoot { };
		std::unique_ptr< cppfs::Tree > _treeDist { };

	public:
		virtual void _CopyFile( const fs::path &src, const fs::path &dst ) = 0;
		virtual void _CopyDir( const fs::path &src, const fs::path &dst ) = 0;
		virtual void _RemoveFile( const fs::path &src, const fs::path &dst ) = 0;
		virtual void _RemoveDir( const fs::path &src, const fs::path &dst ) = 0;

		virtual void _update( fs::path &path ) {}

		const std::shared_ptr< std::vector< std::string > > get_ignoire( ) const { return m_pIgnore; }

		std::unique_ptr< cppfs::Diff > _ctx( ) { return this -> _treeRoot -> createDiff( *this -> _treeDist ); }
	};

	class CWorker final
	{
		using ptr = std::unique_ptr< IObserver >;
		std::vector< ptr > m_observers;

		void _update( ) {}

	protected:
		cppfs::FileHandle _dirRoot { };
		cppfs::FileHandle _dirDist { };

		std::unique_ptr< cppfs::Tree > _treeRoot { };
		std::unique_ptr< cppfs::Tree > _treeDist { };

	public:
		CWorker( const std::filesystem::path src, const std::filesystem::path dist )
		{
			if ( fs::is_directory( src ) || fs::exists( src ) || fs::is_directory( dist ) )
			{
				spdlog::error( "Source or destination is not directory" );
				throw std::runtime_error( "Source or destination is not directory" );
			}

			_dirRoot = cppfs::fs::open( src . string( ) );
			_dirDist = cppfs::fs::open( dist . string( ) );

			_treeRoot = _dirRoot . readTree( );
			_treeDist = _dirDist . readTree( );
		}

		~CWorker( ) = default;

		template< class T > requires std::derived_from< T, IObserver >
		size_t register_handler( )
		{
			this -> m_observers . push_back( T::create( ) );
			return this -> m_observers . size( );
		}

		void remove_handler( size_t id ) { if ( id > 0 && id <= m_observers . size( ) ) this -> m_observers . erase( this -> m_observers . begin( ) + ( id - 1 ) ); }

		void run( std::jthread &thread )
		{
			thread = std::jthread(
						[this] ( std::stop_token token )
						{
							while ( true )
							{
								if ( !token . stop_requested( ) ) return;

								if (
									const auto diff = this -> _treeRoot -> createDiff( *this -> _treeDist ); ( diff -> changes( ) . size( )
										> 1 )
									|| diff -> changes( ) . size( ) == 1 && fs::path( diff -> changes( )[ 0 ] . path( ) ) .
									extension( )
									== ".exe" || fs::path( diff -> changes( )[ 0 ] . path( ) ) . extension( ) == ".dll" ) {}

								std::this_thread::sleep_for( 100ms );
							}
						}
						);
		}
	};
}
