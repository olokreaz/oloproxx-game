#pragma once
#include <commandline.h>
#include <map>
#include <spdlog/spdlog.h>
#include <steam/steam_api.h>
#include <taskflow/taskflow.hpp>

#include <clipp.h>

import socket.client;
import socket.server;

enum class EModeRun : int16 {
	none = 0
	, client
	, server
	,
};

class CApplication {
public:
	inline static std::shared_ptr< CApplication > s_app           = nullptr;
	std::shared_ptr< socket::CServer >                    m_pSocketServer = nullptr;
	std::shared_ptr< socket::CClient >                    m_pSocketClient = nullptr;

private:
	tf::Taskflow m_AppTask;

	void SteamInit( );
	void SteamShutdown( );
	void update( );

	void cli_parser_show_opt( clipp::group *cl, std::string str = ".exe" );
	bool cli_parser_start_app( clipp::group *cli );
	void app_start_choose_mode( );

public:
	CApplication( );
	~CApplication( );

	static inline EModeRun m_eMode = { EModeRun::none };
	static inline bool     s_bQuit = { false };
};
