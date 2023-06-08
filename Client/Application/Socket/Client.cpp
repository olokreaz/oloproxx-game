#include "Client.hpp"

#include "details.hpp"
void CClient::connect( std::string ip, uint16 port ) { }

void CClient::disconnect( )
{
	m_pInterface -> CloseConnection( m_settings_connections . hConn, 0, nullptr, false );
	m_settings_connections . hConn = k_HSteamListenSocket_Invalid;
}

result< void > CClient::UserInputCallback( ) { co_return; }

void CClient::init( ) {}

void CClient::run( )
{
	m_pThreadSelf = new std::thread( [ this ]() -> result< void >
						{
							WHILE {
								this -> recieve( );
								this -> PollConnectionStateChages( );
							}
						}
					);
	m_pThreadSelf -> detach( );
}

void CClient::close( )
{
	*m_bQuit = true;
	m_pThreadSelf -> join( );
	delete m_pThreadSelf;
}

result< void > CClient::recieve( )
{
	SteamNetworkingMessage_t *pIcomingMsg = co_await details::Networking::recieve( m_settings_connections . hConn
											, false
											);
}

template< class T > result< void > CClient::send( const types::CPackage< T > pkg ) const noexcept
{
	m_pInterface -> SendMessageToConnection( m_settings_connections . hConn
						, pkg
						, sizeof pkg
						, k_nSteamNetworkingSend_Reliable
						);
	co_return;
}

void CClient::onNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo )
{
	std::string temp;

	switch ( pInfo -> m_info . m_eState ) {
		case k_ESteamNetworkingConnectionState_None: break;

		case k_ESteamNetworkingConnectionState_ClosedByPeer:
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
			{
				*m_bQuit = true;

				// Print an appropriate message
				if ( pInfo -> m_eOldState == k_ESteamNetworkingConnectionState_Connecting )
					// Note: we could distinguish between a timeout, a rejected connection,
					// or some other transport problem.
					spdlog::info( "We sought the remote host, yet our efforts were met with defeat.  ({})"
							, pInfo -> m_info . m_szEndDebug
						);
				else if ( pInfo -> m_info . m_eState ==
					k_ESteamNetworkingConnectionState_ProblemDetectedLocally )
					spdlog::info( "Alas, troubles beset us; we have lost contact with the host.  ({})"
							, pInfo -> m_info . m_szEndDebug
						);
				else
					// NOTE: We could check the reason code for a normal disconnection
					spdlog::info( "The host hath bidden us farewell.  ({})"
							, pInfo -> m_info . m_szEndDebug
						);

				m_pInterface -> CloseConnection( pInfo -> m_hConn, 0, nullptr, false );
				m_settings_connections . hConn = k_HSteamNetConnection_Invalid;
				break;
			}

		case k_ESteamNetworkingConnectionState_Connecting:
			// We will get this callback when we start connecting.
			// We can ignore this.
			break;

		case k_ESteamNetworkingConnectionState_Connected: spdlog::info( "Connected to server OK" );
			break;

		default:
			// Silences -Wswitch
			break;
	}
}

void CClient::CallbackNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo )
{
	s_pCallbackInstance -> onNetConnectionStatusChanged( pInfo );
}

void CClient::PollConnectionStateChages( )
{
	s_pCallbackInstance = this;
	m_pInterface -> RunCallbacks( );
}
