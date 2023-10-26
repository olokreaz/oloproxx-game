#pragma once
#include <memory>

#include <efsw/efsw.hpp>
#include <spdlog/spdlog.h>

#include <cmt/Configer.hpp>
#include <cmt/utils.hpp>

namespace fs = utils::fs;

class CFileSynchronizer final : public efsw::FileWatchListener
{
	std::shared_ptr< help::CConfig >  m_pConfig;
	std::shared_ptr< spdlog::logger > m_logger;

	bool                         validate( const fs::path &path ) const;
	std::optional< std::string > checkForSpecialPath( const fs::path & ) const;

public:
	CFileSynchronizer( std::shared_ptr< help::CConfig > p_config ) : m_pConfig { p_config }
	{
		m_logger = utils::create_logger( "CFileSynchronizer" );;
		m_logger -> trace( "call {}", __FUNCTION__ );
		utils::ufs::create_dirs( m_pConfig -> special );
	}

	void handleFileAction( efsw::WatchID watchid, const std::string &dir, const std::string &filename, efsw::Action action, std::string oldFilename ) override;
};
