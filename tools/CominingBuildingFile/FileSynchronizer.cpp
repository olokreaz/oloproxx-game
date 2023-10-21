#include "FileSynchronizer.hpp"

#include <optional>

#include <glob/glob.hpp>

#include <cppfs/FileHandle.h>
#include <cppfs/fs.h>

bool CFileSynchronizer::validate( const fs::path &path ) const
{
	return std::ranges::all_of( m_pConfig -> ignore, [&path ] ( const std::string &pattern ) { return glob::fnmatch( path, pattern ); } );
}

std::optional< std::string > CFileSynchronizer::checkForSpecialPath( const fs::path &path ) const
{
	for ( const auto &pattern : m_pConfig -> special | std::views::keys ) if ( glob::fnmatch( path, pattern ) ) return pattern;

	return std::nullopt;
}

void CFileSynchronizer::handleFileAction( efsw::WatchID watchid, const std::string &dir, const std::string &filename, efsw::Action action, std::string oldFilename )
{
	const auto  path    = fs::path( dir ) / filename;
	std::string sAction = "неизвестное действие";

	if ( !validate( path ) ) return;

	const auto source = m_pConfig -> source / path;
	auto       dest   = m_pConfig -> destination / path;

	cppfs::FileHandle hSource = cppfs::fs::open( source . string( ) );
	cppfs::FileHandle hDestPath;

	if ( const std::optional< std::string > pattern = checkForSpecialPath( path ); pattern . has_value( ) ) dest = m_pConfig -> special[ *pattern ] / dest;

	switch ( action )
	{
		case efsw::Action::Add:
		{
			sAction = "Добавление";
			break;
		}
		case efsw::Action::Delete:
		{
			sAction = "Удаление";
			break;
		}
		case efsw::Action::Modified:
		{
			sAction = "Модификация";
			break;
		}
		case efsw::Action::Moved:
		{
			sAction = "Перемещение";
			break;
		}
	}

	m_logger -> info( "{}: {}", sAction, path );
}
