#include <cmt/FileSynchronizer.hpp>

#include <optional>
#include <ranges>

#include <glob/glob.hpp>

#include <cppfs/FileHandle.h>
#include <cppfs/fs.h>

#include<fmt/std.h>

/**
\brief Validates a file path against the ignore patterns in the configuration
\param path File path to validate
\return Returns true if none of the ignore patterns match the file path
*/
bool CFileSynchronizer::validate( const fs::path &path ) const
{
	return std::ranges::none_of( m_pConfig -> ignore, [&path ] ( const std::string &pattern ) { return glob::fnmatch( path, pattern ); } );
}

std::optional< std::string > CFileSynchronizer::checkForSpecialPath( const fs::path &path ) const
{
	for ( const auto &key : m_pConfig -> special | std::views::keys )
		if (
			glob::fnmatch(
					path, fs::path( key ) . is_relative( )
							? ( m_pConfig -> source / key ) . string( )
							: key
					)
		)
			return key;

	return { };
}

void CFileSynchronizer::handleFileAction( efsw::WatchID watchid, const std::string &dir, const std::string &filename, efsw::Action action, std::string oldFilename )
{
	const auto path = fs::path( dir ) / filename;

	std::string sAction = "неизвестное действие";

	fmt::println( "call {}", __FUNCTION__ );

	if ( !validate( path ) ) return;
	const std::optional< std::string > pattern = checkForSpecialPath( path );

	const auto relative = fs::relative( path, m_pConfig -> source );
	const auto source   = m_pConfig -> source / relative;
	const auto dest     = m_pConfig -> destination / ( pattern ? m_pConfig -> special[ *pattern ] : relative );

	this -> m_logger -> debug( "{} {}", fmt::underlying( action ), path );

	switch ( action )
	{
		case efsw::Action::Add:
		case efsw::Action::Modified:
		case efsw::Action::Moved:
		{
			utils::ufs::copy_to( path, dest );
			return;
		}
		case efsw::Action::Delete:
		{
			utils::ufs::remove( dest );
			return;
		}
	}
	std::unreachable( );
}
