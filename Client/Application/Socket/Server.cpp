#include "Server.hpp"

void CServer::init( )
{
	m_logger->info( "Initializing server..." );
	m_serverLocalAddr.Clear( );
	m_serverLocalAddr.m_port = m_nPort;
	SteamNetworkingUtils( )->SetDebugOutputFunction(
							k_ESteamNetworkingSocketsDebugOutputType_Msg
							, details::NetworkDebugOutput
							);
	m_logger->info( "Server initialized" );
}

result< void > CServer::recieve( ) { co_return; }
result< void > CServer::send( HSteamNetConnection hConn ) const noexcept { co_return; }
result< void > CServer::notify( ) { co_return; }

void CServer::run( )
{
	m_logger->info( "Running server..." );

	m_settings.hListenSock = m_pInterface->CreateListenSocketIP( m_serverLocalAddr, 0, nullptr );
	if ( m_settings.hListenSock == k_HSteamListenSocket_Invalid ) {
		m_logger->error( "Failed to create listen socket" );
		return;
	}

	m_settings.hPollGroup = m_pInterface->CreatePollGroup( );
	if ( m_settings.hPollGroup == k_HSteamNetPollGroup_Invalid ) {
		m_logger->error( "Failed to create poll group" );
		return;
	}

	m_logger->info( "Server running" );

}

void CServer::close( )
{
	m_logger->info( "Closing server..." );
	m_pInterface->CloseListenSocket( m_settings.hListenSock );
	m_pInterface->DestroyPollGroup( m_settings.hPollGroup );
	m_logger->info( "Server closed" );
}

void CServer::onNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo )
{
	std::string temp;

	switch ( pInfo->m_info.m_eState ) {
		case k_ESteamNetworkingConnectionState_None : break;

		case k_ESteamNetworkingConnectionState_ClosedByPeer :
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally :
			{

				// the saying goodbye code

				m_pInterface->CloseConnection( pInfo->m_hConn, 0, nullptr, false );
				break;
			}

		case k_ESteamNetworkingConnectionState_Connecting :
			{
				m_pInterface->AcceptConnection( pInfo->m_hConn );
				m_pInterface->SetConnectionPollGroup( pInfo->m_hConn
									, m_settings.hPollGroup
								);

				break;
			}

		case k_ESteamNetworkingConnectionState_Connected : { break ; }

		default : { break; }
	}
}

void CServer::CallbackNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo )
{
	s_pCallbackInstance->onNetConnectionStatusChanged( pInfo );
}

void CServer::PollConnectionStateChages( )
{
	s_pCallbackInstance = this;
	m_pInterface->RunCallbacks( );
}
