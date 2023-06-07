#pragma once
#include <concurrencpp/concurrencpp.h>
#include <spdlog/spdlog.h>
#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamtypes.h>

#include <types/Package.hpp>
#include "details.hpp"
using concurrencpp::result;

class CServer {
	std::shared_ptr< spdlog::logger > m_logger     = { spdlog::get( "Networking" ) };
	bool *                            m_bQuit      = nullptr;
	ISteamNetworkingSockets *         m_pInterface = { SteamNetworkingSockets( ) };
	SteamNetworkingIPAddr             m_serverLocalAddr;
	uint16                            m_nPort = { 40'000 };

	struct {
		HSteamListenSocket hListenSock = { k_HSteamListenSocket_Invalid };
		HSteamNetPollGroup hPollGroup  = { k_HSteamNetPollGroup_Invalid };
	}                          m_settings;

	static inline CServer *s_pCallbackInstance = { nullptr };

	result< void > m_recieve( ) { }

public:
	void init( );
	void run( );

	void close( );

	explicit CServer( bool *bQuit ) : m_bQuit( bQuit ) { }

protected:
	result< void >                     recieve( );
	template< class T > result< void > send(
		const HSteamNetConnection    hConn
		, const types::CPackage< T > pkg
	) const noexcept;
	result< void > notify( );

private:
	void onNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo );

	static void CallbackNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo );

	void PollConnectionStateChages( );
};
