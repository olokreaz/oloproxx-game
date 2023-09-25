#pragma once
#include <filesystem>
#include <efsw/efsw.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace help
{
	namespace fs = std::filesystem;

	class IObserver
	{
	protected:
		virtual void _onAdd( const fs::path &path ) {}
		virtual void _onRemove( const fs::path &path ) {}
		virtual void _onModify( const fs::path &path, const fs::path &to ) {}
		virtual void _onMove( const fs::path &from, const fs::path &to ) {}

		virtual bool _filter( fs::path &path ) { return true; }
		virtual void _update_context( fs::path &path ) {}
	};

	template< typename T > requires std::derived_from< T, IObserver >
	class Handler final : public efsw::FileWatchListener , public T
	{
		inline static auto s_log = spdlog::stdout_color_mt(
								fmt::format(
									"observer<{}>",
									std::string( typeid( T ) . name( ) ) . substr( 5 )
									)
								);

	public:
		void handleFileAction( efsw::WatchID watchid, const std::string &dir, const std::string &filename, efsw::Action action, std::string oldFilename ) override
		{
			auto        path       = fs::path( dir ) / filename;
			auto        old_path   = fs::path( dir ) / oldFilename;
			std::string action_str = "Unknow";

			this -> _update_context( path );

			if ( !this -> _filter( path ) ) return;

			s_log -> trace( "{} {} {}", dir, filename, oldFilename );

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
				default: action_str = "Unknown";
			}
			s_log -> info( "{} - {}", path . string( ), action_str );
		}
	};
}
