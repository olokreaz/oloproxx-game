#pragma once
#include <commandline.h>
#include <map>
#include <spdlog/spdlog.h>
#include <steam/steam_api.h>
#include <taskflow/taskflow.hpp>

#include "include/Socket/Client.h"
#include "include/Socket/Server.h"

#include <clipp.h>
#include <iostream>
#include <ranges>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/variant2.hpp>

enum class EModeRun : int16 {
	none = 0
	, client
	, server
	,
};

class CApp {
public:
	using Socket_t = boost::variant2::variant< CServer, CClient >;

private:
	tf::Taskflow m_AppTask;

	void SteamInit( );
	void SteamShutdown( );
	void update( );

	void cli_parser_show_opt( clipp::group *cl, std::string str = ".exe" );

	bool cli_parser_start_app( clipp::group *cli );

public:
	CApp( );
	~CApp( );

	static inline EModeRun m_eMode = { EModeRun::none };
	static inline bool     s_bQuit = { false };
};
