#pragma once
#include <memory>
#include <efsw/efsw.hpp>
#include <spdlog/spdlog.h>

#include "Configer.hpp"
#include "utils.hpp"

class CFileSynchronizer final : public efsw::FileWatchListener
{
	std::shared_ptr< help::Config > m_pConfig;
	std::shared_ptr< spdlog::logger >     m_logger = utils::create_logger( "CFileSynchronizer" );

	bool                         validate( const fs::path &path ) const;
	std::optional< std::string > checkForSpecialPath( const fs::path & ) const;

public:
	explicit CFileSynchronizer( std::shared_ptr< help::Config > p_config ) : m_pConfig { p_config }
	{
		// m_ignore . reserve( m_pConfig -> ignore . size( ) );
		// for ( auto &i : m_pConfig -> ignore ) m_ignore . emplace_back( i );
		//
		//
		// m_ignore . reserve( m_pConfig -> specific_path . size( ) );
		// for ( auto &i : m_pConfig -> specific_path ) m_specific_path . emplace( i . first, i . second );
	}

	void handleFileAction( efsw::WatchID watchid, const std::string &dir, const std::string &filename, efsw::Action action, std::string oldFilename ) override;
};
