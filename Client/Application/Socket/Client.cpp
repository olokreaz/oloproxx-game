#include "Client.hpp"
void           CClient::connect( ) {}
void           CClient::disconnect( ) {}
void           CClient::init( ) {}
void           CClient::run( ) {}
void           CClient::close( ) {}
result< void > CClient::recieve( ) { SteamNetworkingMessage_t *pIcomingMsg = co_await m_recieve( ); }

template< class T > result< void > CClient::send( const types::CPackage< T > pkg ) const noexcept
{
	m_pInterface->SendMessageToConnection( m_settings.hListenSock
						, pkg.data( )
						, sizeof pkg
						, k_nSteamNetworkingSend_Reliable
						);
	co_return;
}

void CClient::onNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo ) {}
void CClient::CallbackNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo ) {}
void CClient::PollConnectionStateChages( ) {}
