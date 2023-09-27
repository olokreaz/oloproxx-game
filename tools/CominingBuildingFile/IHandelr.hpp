#pragma once
#include <filesystem>
#include <string>

#include <yaml-cpp/yaml.h>

#include <boost/regex.hpp>

#include <efsw/efsw.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace help
{
	namespace fs = std::filesystem;

	template< class T > constexpr auto get_Log_name( )
	{
		return fmt::format(
				"observer<{}>",
				std::string( typeid( T ) . name( ) ) . substr( 5 )
				);
	}

	class IObserver
	{
	protected:
		virtual void _onAdd( const fs::path &path ) { }
		virtual void _onRemove( const fs::path &path ) { }
		virtual void _onModify( const fs::path &path, const fs::path &to ) { }
		virtual void _onMove( const fs::path &from, const fs::path &to ) { }

		virtual bool _filter( fs::path &path ) { return true; }
		virtual void _update_context( fs::path &path ) { }
	};

	template< typename T > requires std::derived_from< T, IObserver >
	class Handler final : public efsw::FileWatchListener , public T
	{
		inline static auto s_log = spdlog::stdout_color_mt( get_Log_name< T >( ) );

	public:
		void handleFileAction( efsw::WatchID watchid, const std::string &dir, const std::string &filename, efsw::Action action, std::string oldFilename ) override
		{
			auto path     = fs::path( dir ) / filename;
			auto old_path = fs::path( dir ) / oldFilename;

			this -> _update_context( path );

			if ( !this -> _filter( path ) ) return;

			s_log -> trace( "{} {} {}", dir, filename, oldFilename );

			std::string action_str = "Unknow";

			switch ( action )
			{
				case efsw::Action::Add: this -> _onAdd( path );
					action_str = "Added";
					break;
				case efsw::Action::Delete: this -> _onRemove( path );
					action_str = "Deleted";
					break;
				case efsw::Action::Modified: this -> _onModify( path, old_path );
					action_str = "Modified";
					break;
				case efsw::Action::Moved: this -> _onMove( path, old_path );
					action_str = "Moved";
					break;
			}
			s_log -> info( "{} - {}", path . string( ), action_str );
		}
	};

	class FileCopier
	{
		std::string                source;
		std::string                destination;
		std::vector< std::string > ignore;

	public:
		FileCopier(
				const std::string &                                   src,
				const std::string &                                   dst,
				const std::vector< std::string > &                    ign,
				const std::unordered_map< std::string, std::string > &specifire
				) : source( src ),
				destination( dst ),
				ignore( ign ) { }

		void copy( ) { copy_directory( fs::path( source ), fs::path( destination ) ); }

	private:
		void copy_file( const fs::path &src, const fs::path &dst ) { fs::copy( src, dst, fs::copy_options::overwrite_existing ); }

		void copy_directory( const fs::path &src, const fs::path &dst )
		{
			fs::create_directories( dst );
			for ( auto &item : fs::directory_iterator( src ) )
			{
				fs::path next = dst / item . path( ) . filename( );
				if ( fs::is_directory( item . status( ) ) ) { if ( !validate_path( item . path( ) ) ) copy_directory( item . path( ), next ); } else if (
					fs::is_regular_file( item . status( ) ) )
					if ( !validate_path( item . path( ) ) ) copy_file( item . path( ), next );
			}
		}

		bool validate_path( const fs::path &path )
		{
			std::string pathStr = path . string( );
			for ( const auto &pattern : ignore )
			{
				boost::regex e( pattern );
				if ( boost::regex_search( pathStr, e ) ) return true;
			}
			return false;
		}
	};

	struct PathFormTo
	{
		fs::path src;
		fs::path dst;
	};
}
