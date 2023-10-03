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
	using namespace fmt::literals;

	struct PathFormTo
	{
		fs::path src;
		fs::path dst;
	};

	struct Config
	{
		fs::path                                       source;
		fs::path                                       destination;
		std::unordered_map< std::string, std::string > modified_files;
		std::vector< std::string >                     ignore;

		void load( fs::path path )
		{
			YAML::Node configYaml = YAML::LoadFile( path . string( ) );

			source      = configYaml[ "source" ] . as< std::string >( );
			destination = configYaml[ "destination" ] . as< std::string >( );
			ignore      = configYaml[ "ignore" ] . as< std::vector< std::string > >( );

			for ( auto &i : configYaml[ "copy_src_to_dst" ] . as< std::vector< help::PathFormTo > >( ) ) modified_files . emplace( );
		}
	};

	template< class T > constexpr auto get_class_name( std::string_view Type_Name = "", std::string_view scope = "" )
	{
		assert( scope . size( ) == 0 || scope . size( ) > 2, "scope must be empty or have 2 characters" );
		return fmt::format(
				"{type-name}{scope-left}{type}{scope-right}",
				"type"_a        = std::string( typeid( T ) . name( ) ) . substr( 5 ),
				"type-name"_a   = Type_Name,
				"scope-left"_a  = scope . size( ) ? scope[ 0 ] : scope,
				"scope-right"_a = scope . size( ) ? scope . size( ) == 1 ? scope[ 1 ] : scope[ 2 ] : scope
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
		inline static auto s_log = spdlog::stdout_color_mt( get_class_name< T >( "observer", "<>" ) );

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

	class FileWrapper { };

	class File
	{
		static inline auto _log = spdlog::stdout_color_st( help::get_class_name< FileWrapper >( "filesystem", "<>" ) );
		FileWrapper        m_file;
	};

	class DirectoryWrapper
	{
		static inline auto _log = spdlog::stdout_color_st( help::get_class_name< DirectoryWrapper >( "filesystem", "<>" ) );
		fs::path           m_path;

	public:
		explicit DirectoryWrapper( fs::path &&path ) : m_path { std::move( path ) }
		{
			if ( !fs::is_directory( m_path ) )
			{
				m_path = m_path . parent_path( );
				_log -> debug( "path {}, path changed to {}", m_path . string( ), m_path . parent_path( ) . string( ) );
			}
		}

		explicit DirectoryWrapper( const fs::path &path )
		{
			if ( !fs::is_directory( path ) )
			{
				m_path = path . parent_path( );
				_log -> debug( "path {}, path changed to {}", m_path . string( ), m_path . parent_path( ) . string( ) );
			} else m_path = path;
		}
	};

	class Directory
	{
		DirectoryWrapper m_dir;

		void copy_dir( const fs::path &src, const fs::path &dst )
		{
			if ( !fs::exists( dst ) ) fs::create_directories( dst );

			for ( auto &item : fs::recursive_directory_iterator( src ) )
			{
				fs::path relativePathFromSrc = fs::relative( item . path( ), src );
				fs::path dstItemPath         = dst / relativePathFromSrc;

				if ( fs::is_directory( item . status( ) ) ) fs::create_directories( dstItemPath );
				else if ( fs::is_regular_file( item . status( ) ) ) fs::copy( item, dstItemPath, fs::copy_options::overwrite_existing );
				// За исключением симлинков и других необычных типов файлов
			}
		}

		void copy_file( const fs::path &src, const fs::path &dst )
		{
			if ( !fs::is_regular_file( src ) ) throw std::runtime_error( "src is not a regular file" );
			else fs::copy_file( src, dst, fs::copy_options::overwrite_existing );
		}

	public:
		void copy( const fs::path dst ) { if ( fs::is_directory( dst ) ) throw std::runtime_error( "" ); }
	};
}
