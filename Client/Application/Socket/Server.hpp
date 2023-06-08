#pragma once
#include <concurrencpp/concurrencpp.h>
#include <spdlog/spdlog.h>
#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamtypes.h>

#include <types/Package.hpp>

using concurrencpp::result;

#define WHILE while (true)
#define WHILE_QUIT while ( !*m_pbQuit )

class CServer {
	std::shared_ptr< spdlog::logger > m_logger     = { spdlog::get( "Networking" ) };
	bool *                            m_pbQuit     = nullptr;
	ISteamNetworkingSockets *         m_pInterface = { SteamNetworkingSockets( ) };
	SteamNetworkingIPAddr             m_serverLocalAddr;
	uint16                            m_nPort = { 40'000 };

	struct settings_socket_t {
		HSteamListenSocket hListenSock = { k_HSteamListenSocket_Invalid };
		HSteamNetPollGroup hPollGroup  = { k_HSteamNetPollGroup_Invalid };
	}                          m_settings_socket;

	static inline CServer *s_pCallbackInstance = { nullptr };
	std::thread *          m_pThread           = { nullptr };

public:
	void           init( );
	result< void > run( );

	void close( );

	explicit CServer( bool *bQuit );

protected:
	result< void > recieve( );

	template< class T > result< void > send(
		const HSteamNetConnection     hConn
		, const types::CPackage< T > *pkg
	) const noexcept;

	template< class T > result< void > notify(
		const types::CPackage< T > pkg
		, std::initializer_list< types::CUser >
	) const noexcept;

	result< void > UserInputCallback( std::string input );

private:
	void onNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo );

	static void CallbackNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo );

	result< void > PollConnectionStateChages( );
	friend class CApplication;
};
