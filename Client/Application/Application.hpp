#pragma once
#include <thread>

#include <spdlog/spdlog.h>
#include <steam/steam_api.h>

class CApplication {
	bool                              m_bQuit       = { false };
	std::shared_ptr< spdlog::logger > m_logger      = { spdlog::get( "Global" ) };
	std::shared_ptr< spdlog::logger > m_steamlogger = { spdlog::get( "Steam" ) };

public:
	void init( )
	{
		m_steamlogger->info( "Initializing Steam API..." );

		m_steamlogger->info( "Checking on exist steam_appid.txt file.." );

		if ( !SteamAPI_Init( ) ) {
			m_steamlogger->error( "SteamAPI_Init() failed!" );
			m_bQuit = true;
			return;
		}
		m_steamlogger->info( "Steam API initialized." );
	}

	void shoutdown( )
	{
		m_steamlogger->info( "Shutting down Steam API..." );
		SteamAPI_Shutdown( );
		m_steamlogger->info( "Steam API shut down." );
	}
	

	operator bool( ) { return m_bQuit; }
};
