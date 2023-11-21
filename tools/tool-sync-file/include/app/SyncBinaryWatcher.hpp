#pragma once
#include <app/Configer.hpp>
#include <app/utils.hpp>

#include <algorithm>
#include <filesystem>
#include <ranges>

#include <fmt/std.h>
#include <spdlog/spdlog.h>

#include <glob/glob.hpp>

#include <cppfs/Diff.h>
#include <cppfs/FileEventHandler.h>
#include <cppfs/FileHandle.h>
#include <cppfs/FileVisitor.h>
#include <cppfs/FileWatcher.h>
#include <cppfs/FunctionalFileEventHandler.h>
#include <cppfs/FunctionalFileVisitor.h>
#include <cppfs/Tree.h>

namespace fs = std::filesystem;

class CSyncBinaryWatcher final : public cppfs::FileEventHandler
{
	std::shared_ptr< spdlog::logger > m_logger = utils::create_logger( "CSyncBinaryWatcher" );

	const std::shared_ptr< help::CConfig > m_pConfig;

	std::optional< std::string > try_take_specific_path( fs::path path )
	{
		for ( const auto &[ pattern, obj ] : m_pConfig -> special )
			if (
				( obj . bRelease
					? *obj . bRelease == m_pConfig -> bReleased
					: true )
				&& glob::fnmatch( path, fs::path( pattern ) . is_relative( ) ? ( m_pConfig -> source / pattern ) . string( ) : pattern )
			)
				return obj . destination;
		return std::nullopt;
	}

	bool valide_on_ignore( fs::path path )
	{
		return std::ranges::none_of( m_pConfig -> ignore, [&path] ( const std::string &pattern ) { return glob::fnmatch( path, pattern ); } );
	}

	const fs::path& context( ) const;
	fs::path        dest_context;

public:
	CSyncBinaryWatcher( std::shared_ptr< help::CConfig > pCfg ) : m_pConfig { std::move( pCfg ) } {}

protected:
	void update( cppfs::FileHandle &fh );
	void onFileEvent( cppfs::FileHandle &fh, cppfs::FileEvent event ) override;
	void onFileCreated( cppfs::FileHandle &fh ) override;
	void onFileRemoved( cppfs::FileHandle &fh ) override;
	void onFileModified( cppfs::FileHandle &fh ) override;
	void onFileAttrChanged( cppfs::FileHandle &fh ) override;
};
