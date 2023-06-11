#include "Server.hpp"

#include "details.hpp"

void CServer::init( )
{
	m_logger -> info( "Initializing server..." );
	m_serverLocalAddr . Clear( );
	m_serverLocalAddr . m_port = m_nPort;

	m_logger -> info( "Server initialized" );
}

result< void > CServer::recieve( )
{
	auto pIcomingMsg = co_await details::Networking::recieve( m_settings_socket . hListenSock );

	co_return;
}

result< void > CServer::run( )
{
	m_logger -> info( "Running server..." );

	m_settings_socket . hListenSock = m_pInterface -> CreateListenSocketIP( m_serverLocalAddr, 0, nullptr );
	if ( m_settings_socket . hListenSock == k_HSteamListenSocket_Invalid ) {
		m_logger -> error( "Failed to create listen socket" );
		co_return;
	}

	m_settings_socket . hPollGroup = m_pInterface -> CreatePollGroup( );
	if ( m_settings_socket . hPollGroup == k_HSteamNetPollGroup_Invalid ) {
		details::Networking::NetFatalError( "Failed to create poll group" );
		co_return;
	}

	m_logger -> info( "Server running" );

	m_pThread = new std::thread( [this]( ) -> result< void >
					{
						WHILE_QUIT {
							co_await recieve( );
							co_await PollConnectionStateChages( );
						}
					}
				);
	m_pThread -> detach( );
}

void CServer::closeConnections( ) { }

void CServer::close( )
{
	closeConnections( );

	m_logger -> info( "Closing server..." );
	m_pThread -> join( );
	m_pInterface -> DestroyPollGroup( m_settings_socket . hPollGroup );
	m_pInterface -> CloseListenSocket( m_settings_socket . hListenSock );
	m_logger -> info( "Server closed" );
}

CServer::CServer( bool *bQuit ): m_pbQuit( bQuit )
				, m_serverLocalAddr { } { }

result< void > CServer::UserInputCallback( std::string input )
{
	// CCommandManager
	co_return;
}

void CServer::onNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo )
{
	std::string temp;

	switch ( pInfo -> m_info . m_eState ) {
		case k_ESteamNetworkingConnectionState_None: break;

		case k_ESteamNetworkingConnectionState_ClosedByPeer:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
			{
				// the saying goodbye code

				m_pInterface -> CloseConnection( pInfo -> m_hConn, 0, nullptr, false );
				break;
			}

		case k_ESteamNetworkingConnectionState_Connecting:
			{
				m_pInterface -> AcceptConnection( pInfo -> m_hConn );
				m_pInterface -> SetConnectionPollGroup( pInfo -> m_hConn
									, m_settings_socket . hPollGroup
									);

				break;
			}

		case k_ESteamNetworkingConnectionState_Connected: { break ; }

		default: { break; }
	}
}

void CServer::CallbackNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo )
{
	s_pCallbackInstance -> onNetConnectionStatusChanged( pInfo );
}

result< void > CServer::PollConnectionStateChages( )
{
	s_pCallbackInstance = this;
	m_pInterface -> RunCallbacks( );
	co_return;
}

template< class T > result< void > CServer::send(
	const HSteamNetConnection     hConn
	, const types::CPackage< T > *pkg
) const noexcept
{
	m_pInterface -> SendMessageToConnection( hConn
						, pkg
						, sizeof pkg
						, k_nSteamNetworkingSend_Reliable
						, nullptr
						, nullptr
						);
	co_return;
}

template< class T > result< void > CServer::notify( const types::CPackage< T > pkg, std::initializer_list< types::CUser > ) const noexcept
{
	while ( true ) for ( auto &user: { 1, 2, 3, 4 } /*temp code*/ ) send( user, &pkg );
}
