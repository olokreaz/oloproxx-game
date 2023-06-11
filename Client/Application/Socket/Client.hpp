#pragma once
#include <concurrencpp/concurrencpp.h>

#include <spdlog/spdlog.h>

#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamtypes.h>

#include <types/Package.hpp>

#define WHILE while (true)
#define WHILE_QUIT while ( !*m_pbQuit )

using concurrencpp::result;

class CClient {
	std::shared_ptr< spdlog::logger > m_logger     = { spdlog::get( "Networking" ) };
	bool *                            m_bQuit      = nullptr;
	ISteamNetworkingSockets *         m_pInterface = { SteamNetworkingSockets( ) };

	std::thread *m_pThread = { nullptr };

public:
	struct settings_socket_t {
		HSteamNetConnection   hConn   = { k_HSteamNetConnection_Invalid };
		uint16                m_nPort = { 40'000 };
		SteamNetworkingIPAddr m_serverLocalAddr { };
	};

private:
	settings_socket_t m_settings_connections;

	static inline CClient *s_pCallbackInstance = { nullptr };

protected:
	void           connect( std::string ip, uint16 port );
	void           disconnect( );
	result< void > UserInputCallback( );

public:
	void init( );
	void run( );

	void close( );

	explicit CClient( bool *bQuit ) : m_bQuit( bQuit ) { }

protected:
	result< void > recieve( );

	template< class T > result< void > send( const types::CPackage< T > pkg ) const noexcept;
	template< class T > result< void > notify( const types::CPackage< T > pkg ) const noexcept;

private:
	void onNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo );

	static void CallbackNetConnectionStatusChanged( SteamNetConnectionStatusChangedCallback_t *pInfo );

	void PollConnectionStateChages( );

	friend class CApplication;
};
