#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <steam/steamnetworkingtypes.h>
#include <steam/steamtypes.h>

namespace details
{
	void NetworkDebugOutput(
		ESteamNetworkingSocketsDebugOutputType eType
		, const char *                         szmsg
	)
	{
		static std::shared_ptr< spdlog::logger > logger = spdlog::get( "Networking" );
		switch ( eType ) {
			case k_ESteamNetworkingSocketsDebugOutputType_Bug :
			case k_ESteamNetworkingSocketsDebugOutputType_Error : logger->error( szmsg );
				break;
			case k_ESteamNetworkingSocketsDebugOutputType_Important : logger->warn( szmsg );
				break;
			case k_ESteamNetworkingSocketsDebugOutputType_Warning : logger->warn( szmsg );
				break;
			case k_ESteamNetworkingSocketsDebugOutputType_Msg : logger->info( szmsg );
				break;
			case k_ESteamNetworkingSocketsDebugOutputType_Verbose :
			case k_ESteamNetworkingSocketsDebugOutputType_Debug : logger->debug( szmsg );
				break;
			default : logger->info( szmsg );
		}
	}
}
