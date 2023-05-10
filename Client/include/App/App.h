#pragma once
#include <commandline.h>
#include <discord_rpc.h>
#include <spdlog/spdlog.h>
#include <steam/steam_api.h>

// extern std::shared_ptr< spdlog::logger > g_logger;
extern Commandline g_cmd;

class CApp {
	void SteamInit( )
	{
		spdlog::info( "Start SteamAPI init" );
		if ( !SteamAPI_Init( ) ) spdlog::error( "Failed SteamAPI init" );
		else spdlog::info( "Finish SteamAPI init" );
	}

	void ShutdownSteam( )
	{
		spdlog::info( "Shoutdown SteamAPI" );
		SteamAPI_Shutdown( );
	}

	void IinitDiscord( ) { Discord_Initialize( "1093253355322753144", nullptr, 1, nullptr ); }
	void ShutdownDiscord( ) { Discord_Shutdown( ); }

	void UpdateDiscord( )
	{
		DiscordRichPresence discordPresence;
		memset( &discordPresence, 0, sizeof( discordPresence ) );
		discordPresence.state          = "Playing Solo";
		discordPresence.details        = "Competitive";
		discordPresence.startTimestamp = 1507665886;
		discordPresence.endTimestamp   = 1507665886;
		discordPresence.largeImageText = "Numbani";
		discordPresence.smallImageText = "Rogue - Level 100";
		discordPresence.partyId        = "ae488379-351d-4a4f-ad32-2b9b01c91657";
		discordPresence.partySize      = 1;
		discordPresence.partyMax       = 5;
		discordPresence.joinSecret     = "MTI4NzM0OjFpMmhuZToxMjMxMjM= ";
		Discord_UpdatePresence( &discordPresence );

	}

	void updatestatus( ) { UpdateDiscord( ); }

	void update( ) { while ( true ) if ( g_cmd.has_command( ) ) if ( g_cmd.get_command( ) == "!stop" ) break; }

public:
	int run( )
	{
		update( );
		return 0;
	}

	CApp( ) { SteamInit( ); }
	~CApp( ) = default;
};
