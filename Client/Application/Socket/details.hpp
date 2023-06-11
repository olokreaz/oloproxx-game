#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <steam/steamnetworkingtypes.h>
#include <steam/steamtypes.h>

namespace details::Networking
{
	static void NetDebugOutput(
		ESteamNetworkingSocketsDebugOutputType eType
		, const char *                         szmsg
	)
	{
		static std::shared_ptr< spdlog::logger > logger = spdlog::get( "Networking" );
		switch ( eType ) {
			case k_ESteamNetworkingSocketsDebugOutputType_Bug: logger -> critical( szmsg );
				ExitProcess( EXIT_FAILURE );
				break;
			case k_ESteamNetworkingSocketsDebugOutputType_Error: logger -> error( szmsg );
				break;
			case k_ESteamNetworkingSocketsDebugOutputType_Important: logger -> warn( szmsg );
				break;
			case k_ESteamNetworkingSocketsDebugOutputType_Warning: logger -> warn( szmsg );
				break;
			case k_ESteamNetworkingSocketsDebugOutputType_Msg: logger -> info( szmsg );
				break;
			case k_ESteamNetworkingSocketsDebugOutputType_Verbose:
			case k_ESteamNetworkingSocketsDebugOutputType_Debug: logger -> debug( szmsg );
				break;
			default: logger -> info( szmsg );
		}
	}

	template< class... T > static void NetFatalError( const std::string_view msg, T... data )
	{
		NetDebugOutput( k_ESteamNetworkingSocketsDebugOutputType_Bug, fmt::format( fmt::runtime( msg ), data... ) . c_str( ) );
	}

	static result< ISteamNetworkingMessage * > recieve(
		uint32 hPool_and_Conn
		, bool bIsServer = true
	)
	{
		ISteamNetworkingMessage *pIncomingMsg = nullptr;
		WHILE {
			int nMsgs;
			if ( !bIsServer )
				nMsgs = SteamNetworkingSockets( ) ->
						ReceiveMessagesOnConnection( hPool_and_Conn
										, &pIncomingMsg
										, 1
									);
			else
				nMsgs = SteamNetworkingSockets( ) ->
						ReceiveMessagesOnPollGroup( hPool_and_Conn
									, &pIncomingMsg
									, 1
									);

			assert( nMsgs == 1 && pIncomingMsg );

			if ( nMsgs < 0 ) spdlog::critical( "Failed to recieve message from server" );
			if ( nMsgs == 0 ) continue;

			void *                    pMsgData = pIncomingMsg -> m_pData;
			types::CPackage< void * > pkg      = static_cast< types::CPackage< void * > * >( pMsgData );

			if ( pIncomingMsg ) co_return pIncomingMsg;
		}
	}

	class CNetUser : public types::CUser {
	public:
		HSteamNetConnection m_hConn = { k_HSteamNetConnection_Invalid };
	};
}
