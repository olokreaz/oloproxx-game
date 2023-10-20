#include "FileSynchronizer.hpp"

#include <optional>
#include <fmt/std.h>
#include <glob/glob.hpp>

bool CFileSynchronizer::validate( const fs::path &path ) const
{
	for ( auto &pattern : m_pConfig -> ignore ) if ( glob::fnmatch( path, pattern ) ) return false;

	return true;
}

std::optional< std::string > CFileSynchronizer::checkForSpecialPath( fs::path path )
{
	for ( const auto &[ pattern, value ] : m_pConfig -> special ) if ( glob::fnmatch( path, pattern ) ) return pattern;
	return std::nullopt;
}

void CFileSynchronizer::handleFileAction( efsw::WatchID watchid, const std::string &dir, const std::string &filename, efsw::Action action, std::string oldFilename )
{
	const auto  path    = fs::path( dir ) / filename;
	std::string sAction = "";

	if ( !validate( path ) ) return;

	std::optional< std::string > pattern = checkForSpecialPath( path );

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
		default: { sAction = "Неизвестное действие"; }
	}

	m_logger -> info( "{}: {}", sAction, path );
}
