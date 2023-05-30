// #pragma once
module;
#include <string>

#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingtypes.h>
#include <steam/steamtypes.h>

export module socket.client;

namespace socket
{
	export class CClient {
		int16                 m_nPort = { 27015 };
		std::string           m_strAddress;
		SteamNetworkingIPAddr m_ipAddress;

		HSteamNetConnection m_hConnection = k_HSteamNetConnection_Invalid;

	public:
		int16&       port( ) { return m_nPort; }
		std::string& address( ) { return m_strAddress; }

		CClient( ) {}
		~CClient( ) {}

		void update_addres_connection( )
		{
			m_ipAddress.Clear( );
			m_ipAddress.ParseString( m_strAddress.c_str( ) );
			if ( m_ipAddress.m_port == 0 ) m_ipAddress.m_port = m_nPort;
		}

		void run( ) {}
	};
}
