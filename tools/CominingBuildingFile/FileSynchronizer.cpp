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
	for ( const auto &pattern : m_pConfig -> special | std::views::keys ) if ( glob::fnmatch( path, pattern ) ) return pattern;
	return std::nullopt;
}

void CFileSynchronizer::_SpecificProcess( const fs::path &src, const fs::path &dest, const efsw::Action &action )
{
	/* TODO */
}

void CFileSynchronizer::_DedaultProcess( const fs::path &src, const fs::path &dest, const efsw::Action &action )
{
	std::error_code ec;

	switch ( action )
	{
		case efsw::Action::Add:
		case efsw::Action::Modified:
		case efsw::Action::Moved:
		{
			utils::filesystem::copy_to( src, dest );
			return;
		}
		case efsw::Action::Delete:
		{
			utils::filesystem::remove( dest );
			return;
		}
	}
	std::unreachable( );
}

void CFileSynchronizer::handleFileAction( efsw::WatchID watchid, const std::string &dir, const std::string &filename, efsw::Action action, std::string oldFilename )
{
	const auto path = fs::path( dir ) / filename;

	std::string sAction = "неизвестное действие";

	if ( !validate( path ) ) return;

	const auto relative = fs::relative( path, m_pConfig -> source );
	const auto source   = m_pConfig -> source / relative;
	auto       dest     = m_pConfig -> destination / relative;

	const std::optional< std::string > pattern = checkForSpecialPath( path );

	if ( pattern ) dest = m_pConfig -> destination / m_pConfig -> special[ *pattern ];
	const bool bIsSpecific = ( bool ) pattern;

	bIsSpecific ? this -> _SpecificProcess( source, dest, action ) : this -> _DedaultProcess( source, dest, action );
}
