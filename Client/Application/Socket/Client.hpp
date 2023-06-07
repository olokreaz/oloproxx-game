#pragma once
#include <concurrencpp/concurrencpp.h>

#include <spdlog/spdlog.h>

#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamtypes.h>

#include <types/Package.hpp>
#include "details.hpp"

using concurrencpp::result;

class CClient {
	std::shared_ptr< spdlog::logger > m_logger     = { spdlog::get( "Networking" ) };
	bool *                            m_bQuit      = nullptr;
	ISteamNetworkingSockets *         m_pInterface = { SteamNetworkingSockets( ) };
	uint16                            m_nPort      = { 40'000 };
	SteamNetworkingIPAddr             m_serverLocalAddr { };

public:
	struct settings_socket_t {
		HSteamListenSocket hListenSock = { k_HSteamListenSocket_Invalid };
	};

private:
	settings_socket_t m_settings;

	static inline CClient *s_pCallbackInstance = { nullptr };

	result< ISteamNetworkingMessage * > m_recieve( )
	{
		ISteamNetworkingMessage *pIncomingMsg = nullptr;
		while ( true ) {
			const int nMsgs = m_pInterface->ReceiveMessagesOnConnection( m_settings.hListenSock
					, &pIncomingMsg
					, 1
				);
			assert( nMsgs == 1 && pIncomingMsg );

			if ( nMsgs < 0 ) spdlog::critical( "Failed to recieve message from server" );
			if ( nMsgs == 0 ) continue;

			if ( pIncomingMsg ) co_return pIncomingMsg;
		}
	}

protected:
	void connect( );
	void disconnect( );

public:
	void init( );
	void run( );

	void close( );

	explicit CClient( bool *bQuit ) : m_bQuit( bQuit ) { }

protected:
	result< void > recieve( );

	template< class T > result< void > send( const types::CPackage< T > pkg ) const noexcept;

private:
	void onNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo );

	static void CallbackNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo );

	void PollConnectionStateChages( );
};
